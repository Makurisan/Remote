#include "FileRowParser.h"




CFileRowParser::CFileRowParser()
               :m_cParserOptions_A(ParserChar::SEMICOL_A,
                                   ParserChar::QUOTE_A,
                                   ParserChar::NUL_A,
                                   false,
                                   false,
                                   false),
                m_cParserOptions_W(ParserChar::SEMICOL_W,
                                   ParserChar::QUOTE_W,
                                   ParserChar::NUL_W,
                                   false,
                                   false,
                                   false)
{
  m_hFile = 0;
  m_pBuffer = 0;
  m_nSizeOfBuffer = 0;
  m_nFileType = tFT_UNKNOWN;
  m_nCountOfCols = 0;
  m_nActualRow = xtstring::npos;
  m_nCodePageForANSI = DEFAULT_CODEPAGE;
}

CFileRowParser::~CFileRowParser()
{
  if (m_pBuffer)
    delete m_pBuffer;
  m_pBuffer = 0;
}

void CFileRowParser::SetColDelimiter(XTCHAR chChar)
{
  m_cParserOptions_A.SetDelimiter(chChar);
  wchar_t ch;
  ch = chChar;
  m_cParserOptions_W.SetDelimiter(chChar);
}

void CFileRowParser::SetQuoter(XTCHAR chChar)
{
  m_cParserOptions_A.SetQuoter(chChar);
  wchar_t ch;
  ch = chChar;
  m_cParserOptions_W.SetQuoter(chChar);
}

void CFileRowParser::AnalyzeCountOfCols(bool bColNamesInFirstRow)
{
  m_nCountOfCols = 0;

  TFileType nFileType = GetFileType();
  if (m_nFileType == tFT_ANSI || m_nFileType == tFT_UTF8)
  {
    CStringParser_A cStringParser;
    if (bColNamesInFirstRow)
    {
      xastring sRow;
      sRow = GetRow_A(0);
      cStringParser.Parse(sRow.c_str(), m_cParserOptions_A);
      m_nCountOfCols = cStringParser.GetCount();
    }
    else
    {
      xastring sRow;
      for (size_t nI = 0; nI < CountOfRows(); nI++)
      {
        sRow = GetRow_A(nI);
        cStringParser.Parse(sRow.c_str(), m_cParserOptions_A);
        if (m_nCountOfCols < cStringParser.GetCount())
          m_nCountOfCols = cStringParser.GetCount();
      }
    }
  }
  else if (m_nFileType == tFT_UTF16_LittleEndian || m_nFileType == tFT_UTF16_BigEndian)
  {
    CStringParser_W cStringParser;
    if (bColNamesInFirstRow)
    {
      xwstring sRow;
      sRow = GetRow_W(0);
      cStringParser.Parse(sRow.c_str(), m_cParserOptions_W);
      m_nCountOfCols = cStringParser.GetCount();
    }
    else
    {
      xwstring sRow;
      for (size_t nI = 0; nI < CountOfRows(); nI++)
      {
        sRow = GetRow_W(nI);
        cStringParser.Parse(sRow.c_str(), m_cParserOptions_W);
        if (m_nCountOfCols < cStringParser.GetCount())
          m_nCountOfCols = cStringParser.GetCount();
      }
    }
  }
}

size_t CFileRowParser::SizeOfBOMInFile()
{
  size_t nRetVal = 0;
  switch (m_nFileType)
  {
  case tFT_UTF8:
    nRetVal = 3;
    break;
  case tFT_UTF16_LittleEndian:
    nRetVal = 2;
    break;
  case tFT_UTF16_BigEndian:
    nRetVal = 2;
    break;
  case tFT_UTF32_LittleEndian:
    nRetVal = 4;
    break;
  case tFT_UTF32_BigEndian:
    nRetVal = 4;
    break;
  }
  return nRetVal;
}
void CFileRowParser::EstimateFileType(FILE* hFile,
                                      TFileType& nFileType)
{
  nFileType = tFT_UNKNOWN;

  if (!hFile)
    return;

  fseek(hFile, 0, SEEK_SET);
//  Bytes         Encoding Form
//  00 00 FE FF   UTF-32, big-endian
//  FF FE 00 00   UTF-32, little-endian
//  FE FF         UTF-16, big-endian
//  FF FE         UTF-16, little-endian
//  EF BB BF      UTF-8
 
#define UTF_BUFF_LEN  5
  unsigned char sBuffer[UTF_BUFF_LEN] = "aaaa";
  for (size_t nI = 0; nI < UTF_BUFF_LEN; nI++)
  {
    if (1 != fread(&(sBuffer[nI]), 1, 1, hFile))
      break;
  }

  if      (sBuffer[0] == 0x00 && sBuffer[1] == 0x00 && sBuffer[2] == 0xfe && sBuffer[3] == 0xff)
    // UTF-32, big-endian
    nFileType = tFT_UTF32_BigEndian;
  else if (sBuffer[0] == 0xff && sBuffer[1] == 0xfe && sBuffer[2] == 0x00 && sBuffer[3] == 0x00)
    // UTF-32, little-endian
    nFileType = tFT_UTF32_LittleEndian;
  else if (sBuffer[0] == 0xef && sBuffer[1] == 0xbb && sBuffer[2] == 0xbf)
    // UTF-8
    nFileType = tFT_UTF8;
  else if (sBuffer[0] == 0xfe && sBuffer[1] == 0xff)
    // UTF-16, big-endian
    nFileType = tFT_UTF16_BigEndian;
  else if (sBuffer[0] == 0xff && sBuffer[1] == 0xfe)
    // UTF-16, little-endian
    nFileType = tFT_UTF16_LittleEndian;
  else
    // ANSI ????
    nFileType = tFT_ANSI;
}

bool CFileRowParser::AnalyseFile(char* pFileName,
                                 TFileType& nFileType)
{
  FILE* hFile = 0;

  if (!pFileName)
    return false;

  hFile = fopen(pFileName, "rb");
  if (!hFile)
    return false;

  EstimateFileType(hFile, nFileType);
  fclose(hFile);
  return true;
}

bool CFileRowParser::OpenFile(char* pFileName)
{
  if (m_hFile)
    CloseFile();

  if (!pFileName)
    return false;

  m_hFile = fopen(pFileName, "rb");
  if (!m_hFile)
    return false;

  fseek(m_hFile, 0, SEEK_SET);
  fseek(m_hFile, 0, SEEK_END);
  m_nSizeOfFile = ftell(m_hFile);

  fseek(m_hFile, 0, SEEK_SET);
  if (!m_nSizeOfFile)
    return true;

  EstimateFileType(m_hFile, m_nFileType);

  if (m_nFileType == tFT_UNKNOWN
      || m_nFileType == tFT_UTF32_LittleEndian
      || m_nFileType == tFT_UTF32_BigEndian)
    return false;

  // jetzt suchen wir einzelne Zeilen
  fseek(m_hFile, 0, SEEK_SET);

  m_cRows.push_back(SizeOfBOMInFile());
  bool bInNewLine = false;
  if (m_nFileType == tFT_ANSI || m_nFileType == tFT_UTF8)
  {
		size_t nI = 0;
    for (; nI < m_nSizeOfFile; nI++)
    {
      char ch;
      fread(&ch, 1, 1, m_hFile);

      if (nI < SizeOfBOMInFile())
        continue;

      if (bInNewLine)
      {
        if (ch != '\r' && ch != '\n')
        {
          m_cRows.push_back(nI);
          bInNewLine = false;
        }
      }
      else
      {
        if (ch == '\r' || ch == '\n')
        {
          bInNewLine = true;
          continue;
        }
      }
    }
    m_cRows.push_back(nI);
  }
  else if (m_nFileType == tFT_UTF16_LittleEndian || m_nFileType == tFT_UTF16_BigEndian)
  {
		size_t nI = 0;
    for (; nI < m_nSizeOfFile; nI++)
    {
      if (nI < SizeOfBOMInFile())
      {
        char ch1;
        fread(&ch1, 1, 1, m_hFile);
        continue;
      }
      wchar_t ch;
      fread(&ch, 2, 1, m_hFile);
      nI++;

      if (m_nFileType == tFT_UTF16_BigEndian)
        ch = (ch<<8) + ((ch>>8) & 255);

      if (bInNewLine)
      {
        if (ch != L'\r' && ch != L'\n')
        {
          m_cRows.push_back(nI - 1);
          bInNewLine = false;
        }
      }
      else
      {
        if (ch == L'\r' || ch == L'\n')
        {
          bInNewLine = true;
          continue;
        }
      }
    }
    m_cRows.push_back(nI);
  }

  return true;
}

bool CFileRowParser::CloseFile()
{
  m_nFileType = tFT_UNKNOWN;
  if (m_hFile)
    fclose(m_hFile);

  m_hFile = 0;
  m_cRows.erase(m_cRows.begin(), m_cRows.end());
  m_nActualRow = xtstring::npos;
  m_nCodePageForANSI = DEFAULT_CODEPAGE;
  m_nCountOfCols = 0;
  m_nSizeOfBuffer = 0;
  if (m_pBuffer)
    delete m_pBuffer;
  m_pBuffer = 0;
  return true;
}

size_t CFileRowParser::GetLengthOfRow(size_t nIndex)
{
  if (nIndex >= CountOfRows())
    return 0;

  size_t nStart, nEnd;
  nStart = m_cRows[nIndex];
  nEnd = m_cRows[nIndex + 1];
  return (nEnd - nStart);
}

xastring CFileRowParser::GetRow_A(size_t nIndex)
{
  xastring sRetRow;

  if (m_nFileType != tFT_ANSI && m_nFileType != tFT_UTF8)
    return sRetRow;

  if (nIndex >= CountOfRows())
    return sRetRow;

  size_t nLength = GetLengthOfRow(nIndex);
  if (m_nSizeOfBuffer < nLength)
  {
    if (m_pBuffer)
      delete m_pBuffer;
    m_pBuffer = 0;
    m_pBuffer = new char[nLength + 2];
    if (!m_pBuffer)
    {
      m_nSizeOfBuffer = 0;
      return sRetRow;
    }
    m_nSizeOfBuffer = nLength;
  }

  fseek(m_hFile, (long)m_cRows[nIndex], SEEK_SET);
  fread(m_pBuffer, nLength, 1, m_hFile);
  ((char*)m_pBuffer)[nLength] = 0;
  sRetRow = (char*)m_pBuffer;
  sRetRow.Trim();
  return sRetRow;
}

xwstring CFileRowParser::GetRow_W(size_t nIndex)
{
  xwstring sRetRow;

  if (m_nFileType != tFT_UTF16_LittleEndian && m_nFileType != tFT_UTF16_BigEndian)
    return sRetRow;

  if (nIndex >= CountOfRows())
    return sRetRow;

  size_t nLength = GetLengthOfRow(nIndex);
  if (m_nSizeOfBuffer < nLength)
  {
    if (m_pBuffer)
      delete m_pBuffer;
    m_pBuffer = 0;
    m_pBuffer = new char[nLength + 2];
    if (!m_pBuffer)
    {
      m_nSizeOfBuffer = 0;
      return sRetRow;
    }
    m_nSizeOfBuffer = nLength;
  }

  fseek(m_hFile, (long)m_cRows[nIndex], SEEK_SET);
  fread(m_pBuffer, nLength, 1, m_hFile);
  ((char*)m_pBuffer)[nLength] = 0;
  ((char*)m_pBuffer)[nLength + 1] = 0;
  sRetRow = (wchar_t*)m_pBuffer;
  if (m_nFileType == tFT_UTF16_BigEndian)
  {
    for (size_t nI = 0; nI < sRetRow.size(); nI++)
    {
      wchar_t wChar;
      wChar = sRetRow[nI];
      wChar = (wChar<<8) + ((wChar>>8) & 255);
      sRetRow[nI] = wChar;
    }
  }
  sRetRow.Trim();
  return sRetRow;
}

xastring CFileRowParser::GetElement_UTF8(size_t nRow, size_t nCol)
{
  xastring sRetValue;
  if (m_nFileType == tFT_ANSI || m_nFileType == tFT_UTF8)
  {
    if (nRow != m_nActualRow)
    {
      xastring sRow;
      sRow = GetRow_A(nRow);
      m_nActualRow = nRow;
      m_cParser_A.Parse(sRow.c_str(), m_cParserOptions_A);
    }
    sRetValue = m_cParser_A.GetAt((int)nCol);
    if (m_nFileType == tFT_ANSI)
    {
      xwstring s;
      s.ConvertAndAssign(sRetValue, m_nCodePageForANSI);
      sRetValue.ConvertAndAssign(s, UTF8_CODEPAGE);
    }
  }
  if (m_nFileType == tFT_UTF16_LittleEndian || m_nFileType == tFT_UTF16_BigEndian)
  {
    if (nRow != m_nActualRow)
    {
      xwstring sRow;
      sRow = GetRow_W(nRow);
      m_nActualRow = nRow;
      m_cParser_W.Parse(sRow.c_str(), m_cParserOptions_W);
    }
    sRetValue.ConvertAndAssign(m_cParser_W.GetAt((int)nCol), UTF8_CODEPAGE);
  }
  return sRetValue;
}
