#pragma once

#include "SModelInd/ModelExpImp.h"

#include "../SSTLdef/STLdef.h"

#include "StringParser.h"




typedef std::vector<size_t>           CRowVector;
typedef CRowVector::iterator          CRowVectorIterator;
typedef CRowVector::const_iterator    CRowVectorConstIterator;
typedef CRowVector::reverse_iterator  CRowVectorReverseIterator;

class XPUBMODEL_EXPORTIMPORT CFileRowParser
{
public:
  typedef enum TFileType
  {
    tFT_UNKNOWN,
    tFT_ANSI,
    tFT_UTF8,
    tFT_UTF16_LittleEndian,
    tFT_UTF16_BigEndian,
    tFT_UTF32_LittleEndian,
    tFT_UTF32_BigEndian,
  };
  CFileRowParser();
  virtual ~CFileRowParser();

  bool AnalyseFile(char* pFileName, TFileType& nFileType);
  bool OpenFile(char* pFileName);
  bool CloseFile();
  bool FileIsOpened(){return (m_hFile != 0);};

  void SetCodePageForANSI(unsigned int nCodePage){m_nCodePageForANSI = nCodePage;};
  void SetColDelimiter(XTCHAR chChar);
  void SetQuoter(XTCHAR chChar);
  void AnalyzeCountOfCols(bool bColNamesInFirstRow);

  TFileType GetFileType(){return m_nFileType;};

  size_t CountOfCols(){return m_nCountOfCols;};
  size_t CountOfRows(){return m_cRows.size() - 1;};

  xastring GetElement_UTF8(size_t nRow, size_t nCol);

protected:
  xastring GetRow_A(size_t nIndex);
  xwstring GetRow_W(size_t nIndex);

  size_t GetLengthOfRow(size_t nIndex);
  void EstimateFileType(FILE* hFile, TFileType& nFileType);
  size_t SizeOfBOMInFile();

  CParseOptions_A m_cParserOptions_A;
  CStringParser_A m_cParser_A;
  CParseOptions_W m_cParserOptions_W;
  CStringParser_W m_cParser_W;
  TFileType       m_nFileType;
  FILE*           m_hFile;

  CRowVector    m_cRows;
  unsigned int  m_nCodePageForANSI;
  size_t        m_nActualRow;
  size_t        m_nCountOfCols;
  size_t        m_nSizeOfFile;
  void*         m_pBuffer;
  size_t        m_nSizeOfBuffer;
};
