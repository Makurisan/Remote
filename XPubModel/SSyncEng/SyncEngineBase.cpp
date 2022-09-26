#include <assert.h>
#include "SyncEngineBase.h"
#include "SyncEngine.h"
#include "SModelInd/XPubVariant.h"



CSyncEngine* CSyncEngineBase::m_pSyncEngine = 0;







/*
    Ich habe diese Bedingung definiert (offsets sind von Bedingungsdialog)
NamesAa.Au_ID + NamesAa.Author + Func( NamesAa.Year_Born, 12,  NamesAa.Note )
1,13;17,14;15,1;34,4 -2,0 40,17 59,2 64,12 -3,0;32,1
    und diese Bedingung
NamesAa_Aa.Au_ID + NamesAa_Aa.Author + Func( NamesAa_Aa.Year_Born, 12,  NamesAa_Aa.Note )
1,16;20,17;18,1;40,4 -2,0 46,20 68,2 73,15 -3,0;38,1
    Dann habe ich die Entität von NamesAa auf NamesAa_Aa umbenannt.
    Die Funktion ReplaceEntityInExpression hat folgende zwei Texte erzeugt
NamesAa_Aa.Au_ID + NamesAa_Aa.Author + Func( NamesAa_Aa.Year_Born, 12,  NamesAa_Aa.Note )
1,16;20,17;18,1;40,4 -2,0 46,20 68,2 73,15 -3,0;38,1
    Dann habe ich die Entität NamesAa_Aa zurück auf NamesAa umbenannt.
    Die Funktion ReplaceEntityInExpression hat folgende zwei Texte erzeugt
NamesAa.Au_ID + NamesAa.Author + Func( NamesAa.Year_Born, 12,  NamesAa.Note )
1,13;17,14;15,1;34,4 -2,0 40,17 59,2 64,12 -3,0;32,1


    Ich habe diese Bedingung definiert (offsets sind von Bedingungsdialog)
NamesAa.Au_ID + NamesAa.Au_ID + Func( NamesAa.Au_ID, 12,  NamesAa.Au_ID )
1,13;17,13;15,1;33,4 -2,0 39,13 54,2 59,13 -3,0;31,1
    und diese Bedingung
NamesAa.Au_ID_ID + NamesAa.Au_ID_ID + Func( NamesAa.Au_ID_ID, 12,  NamesAa.Au_ID_ID )
1,16;20,16;18,1;39,4 -2,0 45,16 63,2 68,16 -3,0;37,1
    Dann habe ich Feld Au_ID auf Au_ID_ID umbenannt.
    Die Funktion ReplaceFieldInExpression hat folgende zwei Texte erzeugt
NamesAa.Au_ID_ID + NamesAa.Au_ID_ID + Func( NamesAa.Au_ID_ID, 12,  NamesAa.Au_ID_ID )
1,16;20,16;18,1;39,4 -2,0 45,16 63,2 68,16 -3,0;37,1
    Dann habe ich Feld Au_ID_ID zurück auf Au_ID umbenannt.
    Die Funktion ReplaceFieldInExpression hat folgende zwei Texte erzeugt
NamesAa.Au_ID + NamesAa.Au_ID + Func( NamesAa.Au_ID, 12,  NamesAa.Au_ID )
1,13;17,13;15,1;33,4 -2,0 39,13 54,2 59,13 -3,0;31,1

*/

//"["/"]"/"IDENTIFIER"/"FLOATING"/"FLOATING"/"FLOATING"/"LIKE"/"SCOPERES"/"ARROW"/"ARROWSTAR"/"DOTSTAR"/"DOT"/"!"/"<"/">"/"=="/"!>"/"!<"/"!="/"<="/">="/"+"/"-"/"/"/"*"/"%"/"**"/"^"/"&"/">>"/"<<"/"||"/"|"/"~"/"&&"/"("/")"/"DECIMAL"/"HEX"/"TRUE"/"FALSE"/"NULL"/"SQ"/","/"++"/"--"/"STRING"/"%error"/"eof"
#define CONST_SYMBOLS     _XT("FLOATING;DECIMAL;HEX;TRUE;FALSE;NULL;STRING")

typedef enum TParserLevel
{
  tPLevel0 = 0,    // delimiter CONDDELIMITER_FIRST_LEVEL     _XT(';')
  tPLevel1 = 1,    // delimiter CONDDELIMITER_SECOND_LEVEL    _XT(' ')
  tPLevel2 = 2,    // delimiter CONDDELIMITER_THIRD_LEVEL     _XT(',')
};

class CBaseParser
{
protected:

  CBaseParser();
  CBaseParser(TParserLevel nLevel);
  void Initialise(TParserLevel nLevel);
  TParserLevel GetNextLevel();

  // common
  TParserLevel  m_nLevel;
  XTCHAR        m_cDelimiter;
};

class COffsetsParser : public CBaseParser
{
public:
  // common
  COffsetsParser();
  // offsets
  xtstring ConvertOffsets(const xtstring& sOffsets,
                          const xtstring& sOldText,
                          const xtstring& sNewText,
                          size_t nPosOfOldText);
protected:
  // common
  COffsetsParser(TParserLevel nLevel);
  // offsets
  xtstring ConvertOffsets(const xtstring& sOffsets,
                          size_t nPosOfOldText,
                          size_t nLenOfOldText,
                          size_t nLenOfNewText);
  xtstring ChangeIt();

  // offsets
  xtstring    m_sOffsets;
  xtstring    m_sNewOffsets;
  int m_nPosOfOldText;
  int m_nLenOfOldText;
  int m_nLenOfNewText;
};

class CConstantsParser : public CBaseParser
{
public:
  // common
  CConstantsParser();
  // constants
  bool FindAllConstants(const xtstring& sExprSymbols,
                        CintVector& cCompoundIndexes);
  xtstring GetValue(const xtstring& sExprValues,
                    int nCompoundIndex);
  bool GetPosAndLen(const xtstring& sExprOffsets,
                    int nCompoundIndex,
                    int& nPos,
                    int& nLen);
  bool ReplaceValue(xtstring& sExprValues,
                    const xtstring& sOldExprValue,
                    const xtstring& sNewExprValue,
                    int nCompoundIndex);
  TFieldType GetFieldType(const xtstring& sExprSymbols,
                          int nCompoundIndex);
protected:
  // common
  CConstantsParser(TParserLevel nLevel);
  // constants
  bool FindAllConstants(const xtstring& sExprSymbols,
                        int nActualCompoundIndex,
                        CintVector& cCompoundIndexes);
  bool FindConstants(int nActualCompoundIndex, CintVector& cCompoundIndexes);

  // constants
  xtstring  m_sExprSymbols;
};









//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CBaseParser
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CBaseParser::CBaseParser()
{
  Initialise(tPLevel0);
}

CBaseParser::CBaseParser(TParserLevel nLevel)
{
  Initialise(nLevel);
}

void CBaseParser::Initialise(TParserLevel nLevel)
{
  m_nLevel = nLevel;
  if (m_nLevel == tPLevel0)
    m_cDelimiter = CONDDELIMITER_FIRST_LEVEL;
  else if (m_nLevel == tPLevel1)
    m_cDelimiter = CONDDELIMITER_SECOND_LEVEL;
  else if (m_nLevel == tPLevel2)
    m_cDelimiter = CONDDELIMITER_THIRD_LEVEL;
  else
  {
    m_cDelimiter = _XT('y');
    assert(false);
  }
}

TParserLevel CBaseParser::GetNextLevel()
{
  if (m_nLevel == tPLevel0)
    return tPLevel1;
  else if (m_nLevel == tPLevel1)
    return tPLevel2;

  assert(false);
  return tPLevel0;
}








//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// COffsetsParser
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
COffsetsParser::COffsetsParser()
               :CBaseParser()
{
}
COffsetsParser::COffsetsParser(TParserLevel nLevel)
               :CBaseParser(nLevel)
{
}
xtstring COffsetsParser::ConvertOffsets(const xtstring& sOffsets,
                                        const xtstring& sOldText,
                                        const xtstring& sNewText,
                                        size_t nPosOfOldText)
{
  m_nPosOfOldText = (int)nPosOfOldText;
  m_nLenOfOldText = (int)sOldText.size();
  m_nLenOfNewText = (int)sNewText.size();

  m_sOffsets = sOffsets;

  return ChangeIt();
}

xtstring COffsetsParser::ConvertOffsets(const xtstring& sOffsets,
                                        size_t nPosOfOldText,
                                        size_t nLenOfOldText,
                                        size_t nLenOfNewText)
{
  m_nPosOfOldText = (int)nPosOfOldText;
  m_nLenOfOldText = (int)nLenOfOldText;
  m_nLenOfNewText = (int)nLenOfNewText;

  m_sOffsets = sOffsets;

  return ChangeIt();
}

xtstring COffsetsParser::ChangeIt()
{
  int nFieldCount = m_sOffsets.GetFieldCount(m_cDelimiter);
  if (m_nLevel != tPLevel2)
  {
    for (int nI = 0; nI < nFieldCount; nI++)
    {
      xtstring sField;
      sField = m_sOffsets.GetField(m_cDelimiter, nI);
      COffsetsParser cNextLevel(GetNextLevel());
      xtstring sNewField;
      sNewField = cNextLevel.ConvertOffsets(sField, m_nPosOfOldText, m_nLenOfOldText, m_nLenOfNewText);
      if (m_sNewOffsets.size())
        m_sNewOffsets += m_cDelimiter;
      m_sNewOffsets += sNewField;
    }
  }
  else
  {
    xtstring sNr1;
    xtstring sNr2;
    int nNr1;
    int nNr2;
    assert(nFieldCount == 2);
    sNr1 = m_sOffsets.GetField(m_cDelimiter, 0);
    sNr2 = m_sOffsets.GetField(m_cDelimiter, 1);
    nNr1 = atoi(sNr1.c_str());
    nNr2 = atoi(sNr2.c_str());

    // vielleicht werden wir was machen, wenn nNr1 > 0 ist -> keine Klammer
    if (nNr1 > 0)
    {
      char p[100];
      if (nNr1 <= m_nPosOfOldText && nNr1 + nNr2 > m_nPosOfOldText)
      {
        // gerade in diesem Ding war was geaendertn, wir aendern laenge
        nNr2 -= (m_nLenOfOldText - m_nLenOfNewText);
        // sNr2 = itoa(nNr2, p, 10);
	sprintf(p, _XT("%ld"), (long)nNr2);
	sNr2 = p;
      }
      else if (nNr1 > m_nPosOfOldText)
      {
        // vor diesem Ding war ein Ding geaendert, wir aendern position
        nNr1 -= (m_nLenOfOldText - m_nLenOfNewText);
        // sNr1 = itoa(nNr1, p, 10);
	sprintf(p, _XT("%ld"), (long)nNr1);
	sNr2 = p;
      }
    }

    m_sNewOffsets = sNr1;
    m_sNewOffsets += m_cDelimiter;
    m_sNewOffsets += sNr2;
  }
  return m_sNewOffsets;
}












//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CConstantsParser
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CConstantsParser::CConstantsParser()
                 :CBaseParser()
{
}
CConstantsParser::CConstantsParser(TParserLevel nLevel)
                 :CBaseParser(nLevel)
{
}
bool CConstantsParser::FindAllConstants(const xtstring& sExprSymbols,
                                        CintVector& cCompoundIndexes)
{
  m_sExprSymbols = sExprSymbols;

  return FindConstants(0, cCompoundIndexes);
}
bool CConstantsParser::FindAllConstants(const xtstring& sExprSymbols,
                                        int nActualCompoundIndex,
                                        CintVector& cCompoundIndexes)
{
  m_sExprSymbols = sExprSymbols;

  return FindConstants(nActualCompoundIndex, cCompoundIndexes);
}
bool CConstantsParser::FindConstants(int nActualCompoundIndex,
                                     CintVector& cCompoundIndexes)
{
  xtstring sSearchedSymbols;
  sSearchedSymbols = CONST_SYMBOLS;

  for (int nOuter = 0; nOuter < m_sExprSymbols.GetFieldCount(m_cDelimiter); nOuter++)
  {
    bool bFound = false;
    int nNewCompoundIndex = 0;
    if (m_nLevel == tPLevel0)
      nNewCompoundIndex = (nOuter + 1);
    else if (m_nLevel == tPLevel1)
      nNewCompoundIndex = nActualCompoundIndex + ((nOuter + 1) << 8);
    else if (m_nLevel == tPLevel2)
      nNewCompoundIndex = nActualCompoundIndex + ((nOuter + 1) << 16);
    else
    {
      assert(false);
      return false;
    }
    for (int nInner = 0; nInner < sSearchedSymbols.GetFieldCount(CONDDELIMITER_FIRST_LEVEL); nInner++)
    {
      if (sSearchedSymbols.GetField(CONDDELIMITER_FIRST_LEVEL, nInner)
          == m_sExprSymbols.GetField(m_cDelimiter, nOuter))
      {
        cCompoundIndexes.push_back(nNewCompoundIndex);
        bFound = true;
        break; // next sExprSymbol
      }
    }
    if (!bFound)
    {
      // die suche wird nur in 2 levels durchgefuehrt
      if (m_nLevel == tPLevel0)
      {
        CConstantsParser cNextLevel(GetNextLevel());
        cNextLevel.FindAllConstants(m_sExprSymbols.GetField(m_cDelimiter, nOuter),
                                    nNewCompoundIndex,
                                    cCompoundIndexes);
      }
    }
  }
  return true;
}

xtstring CConstantsParser::GetValue(const xtstring& sExprValues,
                                    int nCompoundIndex)
{
  if (!nCompoundIndex)
    return _XT("");

  int nFirstLevelIndex, nSecondLevelIndex, nThirdLevelIndex;
  nFirstLevelIndex = nCompoundIndex & 0xFF;
  nSecondLevelIndex = nCompoundIndex >> 8;
  nSecondLevelIndex &= 0xFF;
  nThirdLevelIndex = nCompoundIndex >> 16;
  nThirdLevelIndex &= 0xFF;

  assert(nThirdLevelIndex == 0);
  nFirstLevelIndex--;
  if (nSecondLevelIndex)
    nSecondLevelIndex--;

  xtstring sFirstLevel(sExprValues);
  if (nSecondLevelIndex == 0)
    return sFirstLevel.GetField(CONDDELIMITER_FIRST_LEVEL, nFirstLevelIndex);
  xtstring sSecondLevel;
  sSecondLevel = sFirstLevel.GetField(CONDDELIMITER_FIRST_LEVEL, nFirstLevelIndex);

  // bei Konstanten, die als Parameter in Funktionen benutzt sind,
  // kommt am Ende auch Komma
  xtstring sThirdLevel;
  sThirdLevel = sSecondLevel.GetField(CONDDELIMITER_SECOND_LEVEL, nSecondLevelIndex);
  return sThirdLevel.GetField(CONDDELIMITER_THIRD_LEVEL, 0);
}

bool CConstantsParser::GetPosAndLen(const xtstring& sExprOffsets,
                                    int nCompoundIndex,
                                    int& nPos,
                                    int& nLen)
{
  nPos = -1;
  nLen = 0;
  if (!nCompoundIndex)
    return true;

  int nFirstLevelIndex, nSecondLevelIndex, nThirdLevelIndex;
  nFirstLevelIndex = nCompoundIndex & 0xFF;
  nSecondLevelIndex = nCompoundIndex >> 8;
  nSecondLevelIndex &= 0xFF;
  nThirdLevelIndex = nCompoundIndex >> 16;
  nThirdLevelIndex &= 0xFF;

  assert(nThirdLevelIndex == 0);
  nFirstLevelIndex--;
  if (nSecondLevelIndex)
    nSecondLevelIndex--;

  xtstring sPosAndLen;

  xtstring sFirstLevel(sExprOffsets);
  if (nSecondLevelIndex == 0)
    sPosAndLen = sFirstLevel.GetField(CONDDELIMITER_FIRST_LEVEL, nFirstLevelIndex);
  else
  {
    xtstring sSecondLevel;
    sSecondLevel = sFirstLevel.GetField(CONDDELIMITER_FIRST_LEVEL, nFirstLevelIndex);
    sPosAndLen = sSecondLevel.GetField(CONDDELIMITER_SECOND_LEVEL, nSecondLevelIndex);
  }

  xtstring sPos, sLen;
  sPos = sPosAndLen.GetField(CONDDELIMITER_THIRD_LEVEL, 0);
  sLen = sPosAndLen.GetField(CONDDELIMITER_THIRD_LEVEL, 1);
  nPos = atoi(sPos.c_str());
  nLen = atoi(sLen.c_str());
  if (nPos)
    // nPos ist auch von 1 numeriert
    nPos -= 1;

  return true;
}

bool CConstantsParser::ReplaceValue(xtstring& sExprValues,
                                    const xtstring& sOldExprValue,
                                    const xtstring& sNewExprValue,
                                    int nCompoundIndex)
{
  if (!nCompoundIndex)
    return true;

  int nFirstLevelIndex, nSecondLevelIndex, nThirdLevelIndex;
  nFirstLevelIndex = nCompoundIndex & 0xFF;
  nSecondLevelIndex = nCompoundIndex >> 8;
  nSecondLevelIndex &= 0xFF;
  nThirdLevelIndex = nCompoundIndex >> 16;
  nThirdLevelIndex &= 0xFF;

  assert(nThirdLevelIndex == 0);
  nFirstLevelIndex--;
  if (nSecondLevelIndex)
    nSecondLevelIndex--;

  xtstring sFirstLevel(sExprValues);
  if (nSecondLevelIndex == 0)
  {
    int nFirstLevelCount = sFirstLevel.GetFieldCount(CONDDELIMITER_FIRST_LEVEL);
    xtstring sNewExprValues;
    for (int nI = 0; nI < nFirstLevelCount; nI++)
    {
      if (sNewExprValues.size())
        sNewExprValues += CONDDELIMITER_FIRST_LEVEL;
      xtstring sOldFirstLevel;
      sOldFirstLevel = sFirstLevel.GetField(CONDDELIMITER_FIRST_LEVEL, nI);
      if (nI == nFirstLevelIndex)
      {
        // da muessen wir Aenderung machen
        assert(sOldFirstLevel == sOldExprValue);
        if (sOldFirstLevel != sOldExprValue)
          return false;
        sNewExprValues += sNewExprValue;
      }
      else
        // da aendern wir nichts
        sNewExprValues += sOldFirstLevel;
    }
    sExprValues = sNewExprValues;
    return true;
  }
  else
  {
    int nFirstLevelCount = sFirstLevel.GetFieldCount(CONDDELIMITER_FIRST_LEVEL);
    xtstring sNewExprValues;
    for (int nI = 0; nI < nFirstLevelCount; nI++)
    {
      if (sNewExprValues.size())
        sNewExprValues += CONDDELIMITER_FIRST_LEVEL;
      xtstring sOldFirstLevel;
      sOldFirstLevel = sFirstLevel.GetField(CONDDELIMITER_FIRST_LEVEL, nI);
      if (nI == nFirstLevelIndex)
      {
        int nSecondLevelCount = sOldFirstLevel.GetFieldCount(CONDDELIMITER_SECOND_LEVEL);
        xtstring sNewExprValuesSecondLevel;
        for (int nJ = 0; nJ < nSecondLevelCount; nJ++)
        {
                    if (sNewExprValuesSecondLevel.size())
                      sNewExprValuesSecondLevel += CONDDELIMITER_SECOND_LEVEL;
                    xtstring sOldSecondLevel;
                    sOldSecondLevel = sOldFirstLevel.GetField(CONDDELIMITER_SECOND_LEVEL, nJ);
                    if (nJ == nSecondLevelIndex)
                    {
                      // da muessen wir Aenderung machen
                      assert(sOldSecondLevel == sOldExprValue);
                      if (sOldSecondLevel != sOldExprValue)
                        return false;
                      sNewExprValuesSecondLevel += sNewExprValue;
                    }
                    else
                      // da aendern wir nichts
                      sNewExprValuesSecondLevel += sOldSecondLevel;
        }
      }
      else
        // da aendern wir nichts
        sNewExprValues += sOldFirstLevel;
    }
    sExprValues = sNewExprValues;
    return true;
  }
  return false;
}

TFieldType CConstantsParser::GetFieldType(const xtstring& sExprSymbols,
                                          int nCompoundIndex)
{
  if (!nCompoundIndex)
    return tFieldTypeEmpty;

  int nFirstLevelIndex, nSecondLevelIndex, nThirdLevelIndex;
  nFirstLevelIndex = nCompoundIndex & 0xFF;
  nSecondLevelIndex = nCompoundIndex >> 8;
  nSecondLevelIndex &= 0xFF;
  nThirdLevelIndex = nCompoundIndex >> 16;
  nThirdLevelIndex &= 0xFF;

  assert(nThirdLevelIndex == 0);
  nFirstLevelIndex--;
  if (nSecondLevelIndex)
    nSecondLevelIndex--;

  xtstring sConstantSymbol;

  xtstring sFirstLevel(sExprSymbols);
  if (nSecondLevelIndex == 0)
    sConstantSymbol = sFirstLevel.GetField(CONDDELIMITER_FIRST_LEVEL, nFirstLevelIndex);
  {
    xtstring sSecondLevel;
    sSecondLevel = sFirstLevel.GetField(CONDDELIMITER_FIRST_LEVEL, nFirstLevelIndex);

    // bei Konstanten, die als Parameter in Funktionen benutzt sind,
    // kommt am Ende auch Komma
    xtstring sThirdLevel;
    sThirdLevel = sSecondLevel.GetField(CONDDELIMITER_SECOND_LEVEL, nSecondLevelIndex);
    sConstantSymbol = sThirdLevel.GetField(CONDDELIMITER_THIRD_LEVEL, 0);
  }

  TFieldType nFieldType = tFieldTypeEmpty;

  // _XT("FLOATING;DECIMAL;HEX;TRUE;FALSE;NULL;STRING")
  if (sConstantSymbol == _XT("FLOATING"))
    nFieldType = CXPubVariant::GetDefaultFieldTypeOfGroup(tFieldTypeGroupCurrency);
  else if (sConstantSymbol == _XT("DECIMAL"))
    nFieldType = CXPubVariant::GetDefaultFieldTypeOfGroup(tFieldTypeGroupNumber);
  else if (sConstantSymbol == _XT("HEX"))
    nFieldType = CXPubVariant::GetDefaultFieldTypeOfGroup(tFieldTypeGroupNumber);
  else if (sConstantSymbol == _XT("TRUE"))
    nFieldType = CXPubVariant::GetDefaultFieldTypeOfGroup(tFieldTypeGroupNumber);
  else if (sConstantSymbol == _XT("FALSE"))
    nFieldType = CXPubVariant::GetDefaultFieldTypeOfGroup(tFieldTypeGroupNumber);
  else if (sConstantSymbol == _XT("NULL"))
    nFieldType = CXPubVariant::GetDefaultFieldTypeOfGroup(tFieldTypeGroupNumber);
  else if (sConstantSymbol == _XT("STRING"))
    nFieldType = CXPubVariant::GetDefaultFieldTypeOfGroup(tFieldTypeGroupText);
  else
  {
    assert(false);
    nFieldType = CXPubVariant::GetDefaultFieldTypeOfDefaultGroup();
  }

  return nFieldType;
}












////////////////////////////////////////////////////////////////////////////////
// CSyncEngineObjects
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngineObjects::ProjConstantExistInExpression(const xtstring& sConstantName,
                                                       const xtstring& sExpression,
                                                       const xtstring& sValues,
                                                       CsizetVector& PositionsE,
                                                       CsizetVector& PositionsV)
{
  PositionsE.erase(PositionsE.begin(), PositionsE.end());
  PositionsV.erase(PositionsV.begin(), PositionsV.end());

  if (sExpression.size() == 0 || sValues.size() == 0)
    return false;
  if (sConstantName.size() == 0)
    return false;

  size_t nPos;
  // expression
  nPos = sExpression.find(sConstantName);
  while (nPos != xtstring::npos)
  {
    bool bFrontOK = false;
    bool bRearOK = false;
    if (nPos == 0)
      bFrontOK = true;
    else
    {
      XTCHAR ch = sExpression[nPos - 1];
      if (!isalnum(ch) && ch != _XT('_'))
        bFrontOK = true;
    }
    if (nPos + sConstantName.size() == sExpression.size())
      bRearOK = true;
    else
    {
      XTCHAR ch = sExpression[nPos + sConstantName.size()];
      if (!isalnum(ch) && ch != _XT('_'))
        bRearOK = true;
    }
    if (bFrontOK && bRearOK)
      PositionsE.push_back(nPos);
    nPos += sConstantName.size();
    nPos = sExpression.find(sConstantName, nPos);
  }
  // values
  nPos = sValues.find(sConstantName);
  while (nPos != xtstring::npos)
  {
    bool bFrontOK = false;
    bool bRearOK = false;
    if (nPos == 0)
      bFrontOK = true;
    else
    {
      XTCHAR ch = sValues[nPos - 1];
      if (!isalnum(ch) && ch != _XT('_'))
        bFrontOK = true;
    }
    if (nPos + sConstantName.size() == sValues.size())
      bRearOK = true;
    else
    {
      XTCHAR ch = sValues[nPos + sConstantName.size()];
      if (!isalnum(ch) && ch != _XT('_'))
        bRearOK = true;
    }
    if (bFrontOK && bRearOK)
      PositionsV.push_back(nPos);
    nPos += sConstantName.size();
    nPos = sValues.find(sConstantName, nPos);
  }
  assert(PositionsE.size() == PositionsV.size());
  return (PositionsE.size() != 0);
}
bool CSyncEngineObjects::ReplaceProjConstantInExpression(const xtstring& sConstantName,
                                                         const xtstring& sNewConstantName,
                                                         xtstring& sExpression,
                                                         xtstring& sValues,
                                                         CsizetVector& PositionsE,
                                                         CsizetVector& PositionsV,
                                                         CboolVector& Changes)
{
  xtstring sOffsets;
  return ReplaceProjConstantInExpression(sConstantName,
                                         sNewConstantName,
                                         sExpression,
                                         sValues,
                                         sOffsets,
                                         PositionsE,
                                         PositionsV,
                                         Changes);
}
bool CSyncEngineObjects::ReplaceProjConstantInExpression(const xtstring& sConstantName,
                                                         const xtstring& sNewConstantName,
                                                         xtstring& sExpression,
                                                         xtstring& sValues,
                                                         xtstring& sOffsets,
                                                         CsizetVector& PositionsE,
                                                         CsizetVector& PositionsV,
                                                         CboolVector& Changes)
{
  if (sExpression.size() == 0 || sValues.size() == 0)
    return false;

  assert(PositionsE.size() == PositionsV.size());
  assert(PositionsE.size() == Changes.size());

  if (PositionsE.size() != PositionsV.size() || PositionsE.size() != Changes.size())
    return false;

  bool bChanged = false;
  CsizetVectorReverseIterator it;
  CboolVectorReverseIterator  itB;

  // expression
  it = PositionsE.rbegin();
  itB = Changes.rbegin();
  while (it != PositionsE.rend())
  {
    if ((*itB) == true)
    {
      bChanged = true;
      sExpression.replace(sExpression.begin() + (*it),
                          sExpression.begin() + (*it) + sConstantName.size(),
                          sNewConstantName.begin(),
                          sNewConstantName.end());
    }
    it++;
    itB++;
  }
  // values
  it = PositionsV.rbegin();
  itB = Changes.rbegin();
  while (it != PositionsV.rend())
  {
    if ((*itB) == true)
    {
      bChanged = true;
      sValues.replace(sValues.begin() + (*it),
                      sValues.begin() + (*it) + sConstantName.size(),
                      sNewConstantName.begin(),
                      sNewConstantName.end());
    }
    it++;
    itB++;
  }
  // offsets
  if (bChanged)
  {
    if (sOffsets.size())
    {
      // es geht um Bedingung, oder Bedingungsvariation
      it = PositionsE.rbegin();
      itB = Changes.rbegin();
      while (it != PositionsE.rend())
      {
        if ((*itB) == true)
        {
          COffsetsParser cOff;
          sOffsets = cOff.ConvertOffsets(sOffsets, sConstantName, sNewConstantName,
                                        (*it) + 1); // die Offsets sind von 1
        }
        it++;
        itB++;
      }
    }
  }

  return bChanged;
}
xtstring CSyncEngineObjects::GetEntityFromPositionForFieldOrVariable(const xtstring sExpressionOrValues,
                                                                     size_t nFieldPos)
{
  // Wenn vor nFieldPos Entity steht, auf der Position nFieldPos - 1 muss "." stehen
  if (nFieldPos <= 2 || nFieldPos == xtstring::npos)
    return _XT("");

  if (sExpressionOrValues[nFieldPos - 1] != _XT('.'))
    return _XT("");

  xtstring sEntity;

  for (size_t nI = nFieldPos - 2; nI >= 0; nI--)
  {
    XTCHAR ch = sExpressionOrValues[nI];
    if (isalnum(ch) || ch == _XT('_'))
      sEntity = ch + sEntity;
    else
      break;
    if (nI == 0)
      break;
  }

  return sEntity;
}
bool CSyncEngineObjects::FieldExistInExpression(const xtstring& sIFSModul,
                                                const xtstring& sERModul,
                                                const xtstring& sERTable,
                                                const xtstring& sFieldName,
                                                const xtstring& sExpression,
                                                const xtstring& sValues,
                                                CsizetVector& PositionsE,
                                                CsizetVector& PositionsV)
{
  if (sExpression.size() == 0 || sValues.size() == 0)
    return false;
  if (sFieldName.size() == 0)
    return false;

  if (sIFSModul.size())
  {
    if (m_pSyncEngine)
    {
      xtstring sER = m_pSyncEngine->SE_Intern_GetERFromIFS(sIFSModul);
      if (sERModul != sER)
        return false;
    }
  }

  PositionsE.erase(PositionsE.begin(), PositionsE.end());
  PositionsV.erase(PositionsV.begin(), PositionsV.end());

  size_t nPos;
  // expression
  nPos = sExpression.find(sFieldName);
  while (nPos != xtstring::npos)
  {
    bool bFrontOK = false;
    bool bRearOK = false;
    if (nPos == 0)
      bFrontOK = true;
    else
    {
      XTCHAR ch = sExpression[nPos - 1];
      if (!isalnum(ch) && ch != _XT('_'))
        bFrontOK = true;
    }
    if (nPos + sFieldName.size() == sExpression.size())
      bRearOK = true;
    else
    {
      XTCHAR ch = sExpression[nPos + sFieldName.size()];
      if (!isalnum(ch) && ch != _XT('_'))
        bRearOK = true;
    }
    if (bFrontOK && bRearOK)
    {
      bool bInsert = true;
      if (sIFSModul.size() && m_pSyncEngine)
      {
        xtstring sEntity = GetEntityFromPositionForFieldOrVariable(sExpression, nPos);
        if (sEntity.size())
        {
          xtstring sTab = m_pSyncEngine->SE_Intern_GetERTableFromEntity(sIFSModul, sEntity);
          if (sTab != sERTable)
            bInsert = false;
        }
      }
      if (bInsert)
        PositionsE.push_back(nPos);
    }
    nPos += sFieldName.size();
    nPos = sExpression.find(sFieldName, nPos);
  }
  // values
  nPos = sValues.find(sFieldName);
  while (nPos != xtstring::npos)
  {
    bool bFrontOK = false;
    bool bRearOK = false;
    if (nPos == 0)
      bFrontOK = true;
    else
    {
      XTCHAR ch = sValues[nPos - 1];
      if (!isalnum(ch) && ch != _XT('_'))
        bFrontOK = true;
    }
    if (nPos + sFieldName.size() == sValues.size())
      bRearOK = true;
    else
    {
      XTCHAR ch = sValues[nPos + sFieldName.size()];
      if (!isalnum(ch) && ch != _XT('_'))
        bRearOK = true;
    }
    if (bFrontOK && bRearOK)
    {
      bool bInsert = true;
      if (sIFSModul.size() && m_pSyncEngine)
      {
        xtstring sEntity = GetEntityFromPositionForFieldOrVariable(sValues, nPos);
        if (sEntity.size())
        {
          xtstring sTab = m_pSyncEngine->SE_Intern_GetERTableFromEntity(sIFSModul, sEntity);
          if (sTab != sERTable)
            bInsert = false;
        }
      }
      if (bInsert)
        PositionsV.push_back(nPos);
    }
    nPos += sFieldName.size();
    nPos = sValues.find(sFieldName, nPos);
  }
  assert(PositionsE.size() == PositionsV.size());
  return (PositionsE.size() != 0);
}
bool CSyncEngineObjects::ReplaceFieldInExpression(const xtstring& sFieldName,
                                                  const xtstring& sNewFieldName,
                                                  xtstring& sExpression,
                                                  xtstring& sValues,
                                                  CsizetVector& PositionsE,
                                                  CsizetVector& PositionsV,
                                                  CboolVector& Changes)
{
  xtstring sOffsets;
  return ReplaceFieldInExpression(sFieldName,
                                  sNewFieldName,
                                  sExpression,
                                  sValues,
                                  sOffsets,
                                  PositionsE,
                                  PositionsV,
                                  Changes);
}
bool CSyncEngineObjects::ReplaceFieldInExpression(const xtstring& sFieldName,
                                                  const xtstring& sNewFieldName,
                                                  xtstring& sExpression,
                                                  xtstring& sValues,
                                                  xtstring& sOffsets,
                                                  CsizetVector& PositionsE,
                                                  CsizetVector& PositionsV,
                                                  CboolVector& Changes)
{
  if (sExpression.size() == 0 || sValues.size() == 0)
    return false;

  assert(PositionsE.size() == PositionsV.size());
  assert(PositionsE.size() == Changes.size());

  if (PositionsE.size() != PositionsV.size() || PositionsE.size() != Changes.size())
    return false;

  bool bChanged = false;
  CsizetVectorReverseIterator it;
  CboolVectorReverseIterator  itB;

  // expression
  it = PositionsE.rbegin();
  itB = Changes.rbegin();
  while (it != PositionsE.rend())
  {
    if ((*itB) == true)
    {
      bChanged = true;
      sExpression.replace(sExpression.begin() + (*it),
                          sExpression.begin() + (*it) + sFieldName.size(),
                          sNewFieldName.begin(),
                          sNewFieldName.end());
    }
    it++;
    itB++;
  }
  // values
  it = PositionsV.rbegin();
  itB = Changes.rbegin();
  while (it != PositionsV.rend())
  {
    if ((*itB) == true)
    {
      bChanged = true;
      sValues.replace(sValues.begin() + (*it),
                      sValues.begin() + (*it) + sFieldName.size(),
                      sNewFieldName.begin(),
                      sNewFieldName.end());
    }
    it++;
    itB++;
  }
  // offsets
  if (bChanged)
  {
    if (sOffsets.size())
    {
      // es geht um Bedingung, oder Bedingungsvariation
      it = PositionsE.rbegin();
      itB = Changes.rbegin();
      while (it != PositionsE.rend())
      {
        if ((*itB) == true)
        {
          COffsetsParser cOff;
          sOffsets = cOff.ConvertOffsets(sOffsets, sFieldName, sNewFieldName,
                                        (*it) + 1); // die Offsets sind von 1
        }
        it++;
        itB++;
      }
    }
  }

  return bChanged;
}
bool CSyncEngineObjects::EntityExistInExpression(const xtstring& sEntityName,
                                                 const xtstring& sExpression,
                                                 const xtstring& sValues,
                                                 CsizetVector& PositionsE,
                                                 CsizetVector& PositionsV)
{
  PositionsE.erase(PositionsE.begin(), PositionsE.end());
  PositionsV.erase(PositionsV.begin(), PositionsV.end());

  if (sExpression.size() == 0 || sValues.size() == 0)
    return false;
  if (sEntityName.size() == 0)
    return false;

  size_t nPos;
  // expression
  nPos = sExpression.find(sEntityName);
  while (nPos != xtstring::npos)
  {
    bool bFrontOK = false;
    bool bRearOK = false;
    if (nPos == 0)
      bFrontOK = true;
    else
    {
      XTCHAR ch = sExpression[nPos - 1];
      if (!isalnum(ch) && ch != _XT('_'))
        bFrontOK = true;
    }
    if (nPos + sEntityName.size() == sExpression.size())
      bRearOK = true;
    else
    {
      XTCHAR ch = sExpression[nPos + sEntityName.size()];
      if (!isalnum(ch) && ch != _XT('_'))
        bRearOK = true;
    }
    if (bFrontOK && bRearOK)
      PositionsE.push_back(nPos);
    nPos += sEntityName.size();
    nPos = sExpression.find(sEntityName, nPos);
  }
  // values
  nPos = sValues.find(sEntityName);
  while (nPos != xtstring::npos)
  {
    bool bFrontOK = false;
    bool bRearOK = false;
    if (nPos == 0)
      bFrontOK = true;
    else
    {
      XTCHAR ch = sValues[nPos - 1];
      if (!isalnum(ch) && ch != _XT('_'))
        bFrontOK = true;
    }
    if (nPos + sEntityName.size() == sValues.size())
      bRearOK = true;
    else
    {
      XTCHAR ch = sValues[nPos + sEntityName.size()];
      if (!isalnum(ch) && ch != _XT('_'))
        bRearOK = true;
    }
    if (bFrontOK && bRearOK)
      PositionsV.push_back(nPos);
    nPos += sEntityName.size();
    nPos = sValues.find(sEntityName, nPos);
  }
  assert(PositionsE.size() == PositionsV.size());
  return (PositionsE.size() != 0);
}
bool CSyncEngineObjects::ReplaceEntityInExpression(const xtstring& sEntityName,
                                                   const xtstring& sNewEntityName,
                                                   xtstring& sExpression,
                                                   xtstring& sValues,
                                                   CsizetVector& PositionsE,
                                                   CsizetVector& PositionsV,
                                                   CboolVector& Changes)
{
  xtstring sOffsets;
  return ReplaceEntityInExpression(sEntityName,
                                   sNewEntityName,
                                   sExpression,
                                   sValues,
                                   sOffsets,
                                   PositionsE,
                                   PositionsV,
                                   Changes);
}
bool CSyncEngineObjects::ReplaceEntityInExpression(const xtstring& sEntityName,
                                                   const xtstring& sNewEntityName,
                                                   xtstring& sExpression,
                                                   xtstring& sValues,
                                                   xtstring& sOffsets,
                                                   CsizetVector& PositionsE,
                                                   CsizetVector& PositionsV,
                                                   CboolVector& Changes)
{
  if (sExpression.size() == 0 || sValues.size() == 0)
    return false;

  assert(PositionsE.size() == PositionsV.size());
  assert(PositionsE.size() == Changes.size());

  if (PositionsE.size() != PositionsV.size() || PositionsE.size() != Changes.size())
    return false;

  bool bChanged = false;
  CsizetVectorReverseIterator it;
  CboolVectorReverseIterator  itB;

  // expression
  it = PositionsE.rbegin();
  itB = Changes.rbegin();
  while (it != PositionsE.rend())
  {
    if ((*itB) == true)
    {
      bChanged = true;
      sExpression.replace(sExpression.begin() + (*it),
                          sExpression.begin() + (*it) + sEntityName.size(),
                          sNewEntityName.begin(),
                          sNewEntityName.end());
    }
    it++;
    itB++;
  }
  // values
  it = PositionsV.rbegin();
  itB = Changes.rbegin();
  while (it != PositionsV.rend())
  {
    if ((*itB) == true)
    {
      bChanged = true;
      sValues.replace(sValues.begin() + (*it),
                      sValues.begin() + (*it) + sEntityName.size(),
                      sNewEntityName.begin(),
                      sNewEntityName.end());
    }
    it++;
    itB++;
  }
  // offsets
  if (bChanged)
  {
    if (sOffsets.size())
    {
      // es geht um Bedingung, oder Bedingungsvariation
      it = PositionsE.rbegin();
      itB = Changes.rbegin();
      while (it != PositionsE.rend())
      {
        if ((*itB) == true)
        {
          COffsetsParser cOff;
          sOffsets = cOff.ConvertOffsets(sOffsets, sEntityName, sNewEntityName,
                                        (*it) + 1); // die Offsets sind von 1
        }
        it++;
        itB++;
      }
    }
  }

  return bChanged;
}
bool CSyncEngineObjects::VariableExistInExpression(const xtstring& sEntityName,
                                                   const xtstring& sVarName,
                                                   const xtstring& sExpression,
                                                   const xtstring& sValues,
                                                   CsizetVector& PositionsE,
                                                   CsizetVector& PositionsV)
{
  PositionsE.erase(PositionsE.begin(), PositionsE.end());
  PositionsV.erase(PositionsV.begin(), PositionsV.end());

  if (sExpression.size() == 0 || sValues.size() == 0)
    return false;
  if (sVarName.size() == 0)
    return false;

  size_t nPos;
  // expression
  nPos = sExpression.find(sVarName);
  while (nPos != xtstring::npos)
  {
    bool bFrontOK = false;
    bool bRearOK = false;
    if (nPos == 0)
      bFrontOK = true;
    else
    {
      XTCHAR ch = sExpression[nPos - 1];
      if (!isalnum(ch) && ch != _XT('_'))
        bFrontOK = true;
    }
    if (nPos + sVarName.size() == sExpression.size())
      bRearOK = true;
    else
    {
      XTCHAR ch = sExpression[nPos + sVarName.size()];
      if (!isalnum(ch) && ch != _XT('_'))
        bRearOK = true;
    }
    if (bFrontOK && bRearOK)
    {
      bool bInsert = true;
      xtstring sEntity = GetEntityFromPositionForFieldOrVariable(sExpression, nPos);
      if (sEntity.size() && sEntity != sEntityName)
        bInsert = false;
      if (bInsert)
        PositionsE.push_back(nPos);
    }
    nPos += sVarName.size();
    nPos = sExpression.find(sVarName, nPos);
  }
  // values
  nPos = sValues.find(sVarName);
  while (nPos != xtstring::npos)
  {
    bool bFrontOK = false;
    bool bRearOK = false;
    if (nPos == 0)
      bFrontOK = true;
    else
    {
      XTCHAR ch = sValues[nPos - 1];
      if (!isalnum(ch) && ch != _XT('_'))
        bFrontOK = true;
    }
    if (nPos + sVarName.size() == sValues.size())
      bRearOK = true;
    else
    {
      XTCHAR ch = sValues[nPos + sVarName.size()];
      if (!isalnum(ch) && ch != _XT('_'))
        bRearOK = true;
    }
    if (bFrontOK && bRearOK)
    {
      bool bInsert = true;
      xtstring sEntity = GetEntityFromPositionForFieldOrVariable(sValues, nPos);
      if (sEntity.size() && sEntity != sEntityName)
        bInsert = false;
      if (bInsert)
        PositionsV.push_back(nPos);
    }
    nPos += sVarName.size();
    nPos = sValues.find(sVarName, nPos);
  }
  assert(PositionsE.size() == PositionsV.size());
  return (PositionsE.size() != 0);
}
bool CSyncEngineObjects::ReplaceVariableInExpression(const xtstring& sVarName,
                                                     const xtstring& sNewVarName,
                                                     xtstring& sExpression,
                                                     xtstring& sValues,
                                                     CsizetVector& PositionsE,
                                                     CsizetVector& PositionsV,
                                                     CboolVector& Changes)
{
  xtstring sOffsets;
  return ReplaceVariableInExpression(sVarName,
                                     sNewVarName,
                                     sExpression,
                                     sValues,
                                     sOffsets,
                                     PositionsE,
                                     PositionsV,
                                     Changes);
}
bool CSyncEngineObjects::ReplaceVariableInExpression(const xtstring& sVarName,
                                                     const xtstring& sNewVarName,
                                                     xtstring& sExpression,
                                                     xtstring& sValues,
                                                     xtstring& sOffsets,
                                                     CsizetVector& PositionsE,
                                                     CsizetVector& PositionsV,
                                                     CboolVector& Changes)
{
  if (sExpression.size() == 0 || sValues.size() == 0)
    return false;

  assert(PositionsE.size() == PositionsV.size());
  assert(PositionsE.size() == Changes.size());

  if (PositionsE.size() != PositionsV.size() || PositionsE.size() != Changes.size())
    return false;

  bool bChanged = false;
  CsizetVectorReverseIterator it;
  CboolVectorReverseIterator  itB;

  // expression
  it = PositionsE.rbegin();
  itB = Changes.rbegin();
  while (it != PositionsE.rend())
  {
    if ((*itB) == true)
    {
      bChanged = true;
      sExpression.replace(sExpression.begin() + (*it),
                          sExpression.begin() + (*it) + sVarName.size(),
                          sNewVarName.begin(),
                          sNewVarName.end());
    }
    it++;
    itB++;
  }
  // values
  it = PositionsV.rbegin();
  itB = Changes.rbegin();
  while (it != PositionsV.rend())
  {
    if ((*itB) == true)
    {
      bChanged = true;
      sValues.replace(sValues.begin() + (*it),
                      sValues.begin() + (*it) + sVarName.size(),
                      sNewVarName.begin(),
                      sNewVarName.end());
    }
    it++;
    itB++;
  }
  // offsets
  if (bChanged)
  {
    if (sOffsets.size())
    {
      // es geht um Bedingung, oder Bedingungsvariation
      it = PositionsE.rbegin();
      itB = Changes.rbegin();
      while (it != PositionsE.rend())
      {
        if ((*itB) == true)
        {
          COffsetsParser cOff;
          sOffsets = cOff.ConvertOffsets(sOffsets, sVarName, sNewVarName,
                                        (*it) + 1); // die Offsets sind von 1
        }
        it++;
        itB++;
      }
    }
  }

  return bChanged;
}
bool CSyncEngineObjects::FindAllConstantsInExprSymbols(const xtstring& sExprSymbols,
                                                       CintVector& cCompoundIndexes)
{
  CConstantsParser c;
  return c.FindAllConstants(sExprSymbols, cCompoundIndexes);
}
TFieldType CSyncEngineObjects::GetFieldTypeFromExprSymbols(const xtstring& sExprSymbols,
                                                           int nCompoundIndex)
{
  CConstantsParser c;
  return c.GetFieldType(sExprSymbols, nCompoundIndex);
}
xtstring CSyncEngineObjects::GetValueFromExprValues(const xtstring& sExprValues,
                                                    int nCompoundIndex)
{
  CConstantsParser c;
  return c.GetValue(sExprValues, nCompoundIndex);
}
bool CSyncEngineObjects::GetPosAndLenOfConstantInExprOffsets(const xtstring& sExprOffsets,
                                                             int nCompoundIndex,
                                                             int& nPos,
                                                             int& nLen)
{
  CConstantsParser c;
  return c.GetPosAndLen(sExprOffsets, nCompoundIndex, nPos, nLen);
}
bool CSyncEngineObjects::ReplaceValueInExprValues(xtstring& sExprValues,
                                                  const xtstring& sOldExprValue,
                                                  const xtstring& sNewExprValue,
                                                  int nCompoundIndex)
{
  CConstantsParser c;
  return c.ReplaceValue(sExprValues, sOldExprValue, sNewExprValue, nCompoundIndex);
}
bool CSyncEngineObjects::ChangeOffsetsBecauseValueChanged(xtstring& sExprOffsets,
                                                          const xtstring& sOldValue,
                                                          const xtstring& sNewValue,
                                                          int nPos)
{
  COffsetsParser cOff;
  sExprOffsets = cOff.ConvertOffsets(sExprOffsets, sOldValue, sNewValue, nPos + 1); // die Offsets sind von 1
  return true;
}
void CSyncEngineObjects::SE_Intern_GetGlobalTableNames(const xtstring& sIFSModulName,
                                                       CxtstringVector& arrNames)
{
  if (m_pSyncEngine)
    m_pSyncEngine->SE_Intern_GetGlobalTableNames(sIFSModulName, arrNames);
}




















////////////////////////////////////////////////////////////////////////////////
// CSyncNotifier_ER
////////////////////////////////////////////////////////////////////////////////
CSyncNotifier_ER::CSyncNotifier_ER(TTypeOfERNotifier nTypeOfNotifier,
                                   CSyncERNotifiersGroup* pGroup)
{
  m_nTypeOfNotifier = nTypeOfNotifier;

  if (pGroup)
    SetSyncGroup(pGroup);
  else
  {
    if (m_pSyncEngine)
    {
      assert(m_pSyncEngine);
      assert(m_pSyncEngine->GetDefaultERGroup());
      SetSyncGroup(m_pSyncEngine->GetDefaultERGroup());
    }
  }
}
CSyncNotifier_ER::~CSyncNotifier_ER()
{
  SetSyncGroup((CSyncERNotifiersGroup*)NULL);
}
void CSyncNotifier_ER::SetSyncGroup(CSyncERNotifiersGroup* pSyncGroup)
{
  if (GetSyncGroup())
  {
    GetSyncGroup()->RemoveNotifier(this);
    CSyncNotifier::SetSyncGroup((CSyncGroup*)NULL);
  }
  if (pSyncGroup)
  {
    CSyncNotifier::SetSyncGroup((CSyncGroup*)pSyncGroup);
    GetSyncGroup()->AddNotifier(this);
  }
}
bool CSyncNotifier_ER::SE_ChangeERModelName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeERModelName(sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_ChangeTableName(const xtstring& sERModelName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeTableName(sERModelName, sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_ChangeFieldName(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeFieldName(sERModelName, sTableName, sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_ChangeParamName(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeParamName(sERModelName, sTableName, sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_ChangeRelationName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeRelationName(sERModelName, sTableName, sParentTableName, sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_CheckReferencesForERModel(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForERModel(sName, cUsers);
  return false;
}
bool CSyncNotifier_ER::SE_CheckReferencesForTable(const xtstring& sERModelName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForTable(sERModelName, sName, cUsers);
  return false;
}
bool CSyncNotifier_ER::SE_CheckReferencesForField(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sFieldName,
                                                  CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForField(sERModelName, sTableName, sFieldName, cUsers);
  return false;
}
bool CSyncNotifier_ER::SE_CheckReferencesForParam(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sParamName,
                                                  CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForParam(sERModelName, sTableName, sParamName, cUsers);
  return false;
}
bool CSyncNotifier_ER::SE_CheckReferencesForRelation(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
  return false;
}
bool CSyncNotifier_ER::SE_DeleteERModel(const xtstring& sName,
                                        size_t& nCountOfReferences,
                                        bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteERModel(sName, nCountOfReferences, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_DeleteTable(const xtstring& sERModelName,
                                      const xtstring& sName,
                                      size_t& nCountOfReferences,
                                      bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteTable(sERModelName, sName, nCountOfReferences, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_DeleteField(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sFieldName,
                                      size_t& nCountOfReferences,
                                      bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteField(sERModelName, sTableName, sFieldName, nCountOfReferences, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_DeleteParam(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sParamName,
                                      size_t& nCountOfReferences,
                                      bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteParam(sERModelName, sTableName, sParamName, nCountOfReferences, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_DeleteRelation(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_ParamAdded(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sParamName,
                                     CERModelParamInd* pParam,
                                     bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ParamAdded(sERModelName, sTableName, sParamName, pParam, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_ParamChanged(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParamName,
                                       CERModelParamInd* pParam,
                                       bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ParamChanged(sERModelName, sTableName, sParamName, pParam, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_RelationAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_RelationAdded(sERModelName, sTableName, sParentTableName, sName, pRelation, bCancelAllowed);
  return false;
}
bool CSyncNotifier_ER::SE_RelationChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CERModelRelationInd* pRelation,
                                          bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_RelationChanged(sERModelName, sTableName, sParentTableName, sName, pRelation, bCancelAllowed);
  return false;
}
xtstring CSyncNotifier_ER::SE_MF_GetModuleName()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetModuleName();
  return xtstring();
}
bool CSyncNotifier_ER::SE_MF_GetShowRelationNamesER()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetShowRelationNamesER();
  return false;
}
const CERModelTableInd* CSyncNotifier_ER::SE_MF_GetERModelTableInd(const xtstring& sName)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetERModelTableInd(sName);
  return 0;
}
xtstring CSyncNotifier_ER::SE_MF_GetDBDefinitionName_Production()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetDBDefinitionName_Production();
  return xtstring();
}
xtstring CSyncNotifier_ER::SE_MF_GetDBDefinitionName_Test()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetDBDefinitionName_Test();
  return xtstring();
}
void CSyncNotifier_ER::SE_SyncFontChanged(const xtstring& sFontName, int nFontHeight)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncFontChanged(this, sFontName, nFontHeight);
}
void CSyncNotifier_ER::SE_SyncColorChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorChanged(this, nColor);
}
void CSyncNotifier_ER::SE_SyncColorBackChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorBackChanged(this, nColor);
}
void CSyncNotifier_ER::SE_SyncColorHeaderChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorHeaderChanged(this, nColor);
}
void CSyncNotifier_ER::SE_SyncColorHeaderSelectedChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorHeaderSelectedChanged(this, nColor);
}
void CSyncNotifier_ER::SE_SyncColorHeaderTextChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorHeaderTextChanged(this, nColor);
}
void CSyncNotifier_ER::SE_SyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorHeaderSelectedTextChanged(this, nColor);
}
void CSyncNotifier_ER::SE_SyncColorSelectedItemChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorSelectedItemChanged(this, nColor);
}
void CSyncNotifier_ER::SE_SyncMoveableChanged(bool bVal)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncMoveableChanged(this, bVal);
}
void CSyncNotifier_ER::SE_SyncSelectableChanged(bool bVal)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncSelectableChanged(this, bVal);
}
void CSyncNotifier_ER::SE_SyncVisibleChanged(bool bVal)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncVisibleChanged(this, bVal);
}
void CSyncNotifier_ER::SE_SyncSizeableChanged(bool bVal)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncSizeableChanged(this, bVal);
}









////////////////////////////////////////////////////////////////////////////////
// CSyncNotifier_IFS
////////////////////////////////////////////////////////////////////////////////
CSyncNotifier_IFS::CSyncNotifier_IFS(TTypeOfIFSNotifier nTypeOfNotifier,
                                     CSyncIFSNotifiersGroup* pGroup)
{
  m_nTypeOfNotifier = nTypeOfNotifier;

  if (pGroup)
    SetSyncGroup(pGroup);
  else
  {
    if (m_pSyncEngine)
    {
      assert(m_pSyncEngine);
      assert(m_pSyncEngine->GetDefaultERGroup());
      SetSyncGroup(m_pSyncEngine->GetDefaultIFSGroup());
    }
  }
}
CSyncNotifier_IFS::~CSyncNotifier_IFS()
{
  SetSyncGroup((CSyncIFSNotifiersGroup*)NULL);
}
void CSyncNotifier_IFS::SetSyncGroup(CSyncIFSNotifiersGroup* pSyncGroup)
{
  if (GetSyncGroup())
  {
    GetSyncGroup()->RemoveNotifier(this);
    CSyncNotifier::SetSyncGroup((CSyncGroup*)NULL);
  }
  if (pSyncGroup)
  {
    CSyncNotifier::SetSyncGroup((CSyncGroup*)pSyncGroup);
    GetSyncGroup()->AddNotifier(this);
  }
}
bool CSyncNotifier_IFS::SE_ChangeIFSName(const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeIFSName(sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_IFS::SE_ChangeEntityName(const xtstring& sIFS,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeEntityName(sIFS, sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_IFS::SE_ChangeVariableName(const xtstring& sIFS,
                                              const xtstring& sEntityName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeVariableName(sIFS, sEntityName, sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_IFS::SE_ChangeGlobalERTableName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeGlobalERTableName(sIFS, sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_IFS::SE_CheckReferencesForIFS(const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForIFS(sName, cUsers);
  return false;
}
bool CSyncNotifier_IFS::SE_CheckReferencesForEntity(const xtstring& sIFS,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForEntity(sIFS, sName, cUsers);
  return false;
}
bool CSyncNotifier_IFS::SE_CheckReferencesForVariable(const xtstring& sIFS,
                                                      const xtstring& sEntityName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForVariable(sIFS, sEntityName, sName, cUsers);
  return false;
}
bool CSyncNotifier_IFS::SE_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                           const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForGlobalERTable(sIFS, sName, cUsers);
  return false;
}
bool CSyncNotifier_IFS::SE_DeleteIFS(const xtstring& sName,
                                     size_t& nCountOfReferences,
                                     bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteIFS(sName, nCountOfReferences, bCancelAllowed);
  return false;
}
bool CSyncNotifier_IFS::SE_DeleteEntity(const xtstring& sIFS,
                                        const xtstring& sName,
                                        size_t& nCountOfReferences,
                                        bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteEntity(sIFS, sName, nCountOfReferences, bCancelAllowed);
  return false;
}
bool CSyncNotifier_IFS::SE_DeleteVariable(const xtstring& sIFS,
                                          const xtstring& sEntityName,
                                          const xtstring& sName,
                                          size_t& nCountOfReferences,
                                          bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteVariable(sIFS, sEntityName, sName, nCountOfReferences, bCancelAllowed);
  return false;
}
bool CSyncNotifier_IFS::SE_DeleteGlobalERTable(const xtstring& sIFS,
                                               const xtstring& sName,
                                               size_t& nCountOfReferences,
                                               bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteGlobalERTable(sIFS, sName, nCountOfReferences, bCancelAllowed);
  return false;
}
void CSyncNotifier_IFS::SE_RelationNameChanged()
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_RelationNameChanged();
}
void CSyncNotifier_IFS::SE_RelationNameDeleted()
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_RelationNameDeleted();
}
void CSyncNotifier_IFS::SE_RelationNameAdded()
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_RelationNameAdded();
}
xtstring CSyncNotifier_IFS::SE_MF_GetModuleName()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetModuleName();
  return _XT("");
}
bool CSyncNotifier_IFS::SE_MF_GetShowRelationNames()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetShowRelationNames();
  return true;
}
bool CSyncNotifier_IFS::SE_MF_GetShowHitLines()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetShowHitLines();
  return true;
}
const CIFSEntityInd* CSyncNotifier_IFS::SE_MF_GetIFSEntityInd(const xtstring& sName)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetIFSEntityInd(sName);
  return 0;
}
xtstring CSyncNotifier_IFS::SE_MF_GetERModelName()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetERModelName();
  return _XT("");
}
void CSyncNotifier_IFS::SE_MF_GetGlobalTableNames(CxtstringVector& arrNames)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_MF_GetGlobalTableNames(arrNames);
}
xtstring CSyncNotifier_IFS::SE_MF_GetERTableFromGlobalTable(const xtstring& sGlobalTableName)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetERTableFromGlobalTable(sGlobalTableName);
  return _XT("");
}
void CSyncNotifier_IFS::SE_MF_ParamTableChanged()
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_MF_ParamTableChanged();
}
void CSyncNotifier_IFS::SE_MF_ERModelChanged(const xtstring& sNewERModelName)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_MF_ERModelChanged(sNewERModelName);
}
xtstring CSyncNotifier_IFS::SE_MF_GetParamTable()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetParamTable();
  return _XT("");
}
bool CSyncNotifier_IFS::SE_MF_SetParamTable(const xtstring& sParamTable)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_SetParamTable(sParamTable);
  return false;
}
void CSyncNotifier_IFS::SE_SyncFontChanged(const xtstring& sFontName, int nFontHeight)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncFontChanged(this, sFontName, nFontHeight);
}
void CSyncNotifier_IFS::SE_SyncColorChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorChanged(this, nColor);
}
void CSyncNotifier_IFS::SE_SyncColorBackChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorBackChanged(this, nColor);
}
void CSyncNotifier_IFS::SE_SyncColorHeaderChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorHeaderChanged(this, nColor);
}
void CSyncNotifier_IFS::SE_SyncColorHeaderSelectedChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorHeaderSelectedChanged(this, nColor);
}
void CSyncNotifier_IFS::SE_SyncColorHeaderTextChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorHeaderTextChanged(this, nColor);
}
void CSyncNotifier_IFS::SE_SyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorHeaderSelectedTextChanged(this, nColor);
}
void CSyncNotifier_IFS::SE_SyncColorSelectedItemChanged(MODEL_COLOR nColor)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncColorSelectedItemChanged(this, nColor);
}
void CSyncNotifier_IFS::SE_SyncMoveableChanged(bool bVal)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncMoveableChanged(this, bVal);
}
void CSyncNotifier_IFS::SE_SyncSelectableChanged(bool bVal)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncSelectableChanged(this, bVal);
}
void CSyncNotifier_IFS::SE_SyncVisibleChanged(bool bVal)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncVisibleChanged(this, bVal);
}
void CSyncNotifier_IFS::SE_SyncSizeableChanged(bool bVal)
{
  if (GetSyncGroup())
    GetSyncGroup()->SE_SyncSizeableChanged(this, bVal);
}












////////////////////////////////////////////////////////////////////////////////
// CSyncNotifier_WP
////////////////////////////////////////////////////////////////////////////////
CSyncNotifier_WP::CSyncNotifier_WP(TTypeOfWPNotifier nTypeOfNotifier,
                                   CSyncWPNotifiersGroup* pGroup)
{
  m_nTypeOfNotifier = nTypeOfNotifier;

  if (pGroup)
    SetSyncGroup(pGroup);
  else
  {
    if (m_pSyncEngine)
    {
      assert(m_pSyncEngine);
      assert(m_pSyncEngine->GetDefaultWPGroup());
      SetSyncGroup(m_pSyncEngine->GetDefaultWPGroup());
    }
  }
}
CSyncNotifier_WP::~CSyncNotifier_WP()
{
  SetSyncGroup((CSyncWPNotifiersGroup*)NULL);
}
void CSyncNotifier_WP::SetSyncGroup(CSyncWPNotifiersGroup* pSyncGroup)
{
  if (GetSyncGroup())
  {
    GetSyncGroup()->RemoveNotifier(this);
    CSyncNotifier::SetSyncGroup((CSyncGroup*)NULL);
  }
  if (pSyncGroup)
  {
    CSyncNotifier::SetSyncGroup((CSyncGroup*)pSyncGroup);
    GetSyncGroup()->AddNotifier(this);
  }
}
xtstring CSyncNotifier_WP::SE_MF_GetModuleName()
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_MF_GetModuleName();
  return _XT("");
}
bool CSyncNotifier_WP::SE_ChangeWPName(const xtstring& sOldName,
                                       const xtstring& sNewName,
                                       bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeWPName(sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_WP::SE_CheckReferencesForWP(const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForWP(sName, cUsers);
  return false;
}
bool CSyncNotifier_WP::SE_DeleteWP(const xtstring& sName,
                                   size_t& nCountOfReferences,
                                   bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteWP(sName, nCountOfReferences, bCancelAllowed);
  return false;
}
bool CSyncNotifier_WP::SE_ChangeWPConstantName(const xtstring& sWPName,
                                               const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_ChangeWPConstantName(sWPName, sOldName, sNewName, bCancelAllowed);
  return false;
}
bool CSyncNotifier_WP::SE_CheckReferencesForWPConstant(const xtstring& sWPName,
                                                       const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_CheckReferencesForWPConstant(sWPName, sName, cUsers);
  return false;
}
bool CSyncNotifier_WP::SE_DeleteWPConstant(const xtstring& sWPName,
                                           const xtstring& sName,
                                           size_t& nCountOfReferences,
                                           bool bCancelAllowed)
{
  if (GetSyncGroup())
    return GetSyncGroup()->SE_DeleteWPConstant(sWPName, sName, nCountOfReferences, bCancelAllowed);
  return false;
}



















////////////////////////////////////////////////////////////////////////////////
// CSyncResponse_Common
////////////////////////////////////////////////////////////////////////////////
CSyncResponse_Common::CSyncResponse_Common(TTypeOfResponseObject nTypeOfResponseObject)
                     :CSyncResponse(nTypeOfResponseObject)
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->AddCommonResponse(this);
  }
}
CSyncResponse_Common::~CSyncResponse_Common()
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->RemoveCommonResponse(this);
  }
}





////////////////////////////////////////////////////////////////////////////////
// CSyncResponse_Proj
////////////////////////////////////////////////////////////////////////////////
CSyncResponse_Proj::CSyncResponse_Proj(TTypeOfResponseObject nTypeOfResponseObject)
                   :CSyncResponse(nTypeOfResponseObject)
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->AddProjResponse(this);
  }
}
CSyncResponse_Proj::~CSyncResponse_Proj()
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->RemoveProjResponse(this);
  }
}





////////////////////////////////////////////////////////////////////////////////
// CSyncResponse_ER
////////////////////////////////////////////////////////////////////////////////
CSyncResponse_ER::CSyncResponse_ER(TTypeOfResponseObject nTypeOfResponseObject)
                 :CSyncResponse(nTypeOfResponseObject)
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->AddERResponse(this);
  }
}
CSyncResponse_ER::~CSyncResponse_ER()
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->RemoveERResponse(this);
  }
}





////////////////////////////////////////////////////////////////////////////////
// CSyncResponse_IFS
////////////////////////////////////////////////////////////////////////////////
CSyncResponse_IFS::CSyncResponse_IFS(TTypeOfResponseObject nTypeOfResponseObject)
                  :CSyncResponse(nTypeOfResponseObject)
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->AddIFSResponse(this);
  }
}
CSyncResponse_IFS::~CSyncResponse_IFS()
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->RemoveIFSResponse(this);
  }
}




////////////////////////////////////////////////////////////////////////////////
// CSyncResponse_WP
////////////////////////////////////////////////////////////////////////////////
CSyncResponse_WP::CSyncResponse_WP(TTypeOfResponseObject nTypeOfResponseObject)
                 :CSyncResponse(nTypeOfResponseObject)
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->AddWPResponse(this);
  }
}
CSyncResponse_WP::~CSyncResponse_WP()
{
  if (m_pSyncEngine)
  {
    m_pSyncEngine->RemoveWPResponse(this);
  }
}
