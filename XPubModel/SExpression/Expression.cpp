// Expression.cpp: implementation of the CExpression class.
//
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif

#include <vector>
#include <assert.h>
#include "SSTLdef/STLdef.h"
#include <iostream>
#include <algorithm>
#include <list>
#include <math.h>

#include "SModelInd/ModelDef.h"
#include "SModelInd/ModelInd.h"
#include "SModelInd/ERModelInd.h"
#include "SErrors/ErrorNumbers.h"

#include "Expression.h"

#define EXPRESSION_ERROR_CODEBASIS			0x0200

#define EXPRESSION_ERROR_NONE										0x0000 // OK
#define EXPRESSION_ERROR_UNDEFINED							EXPRESSION_ERROR_CODEBASIS + 0x0001 // undefined error
#define EXPRESSION_ERROR_ARGUMENT_FAILED				EXPRESSION_ERROR_CODEBASIS + 0x0002 // Cannot convert argument
#define EXPRESSION_ERROR_FUNCTIONNOTFOUND				EXPRESSION_ERROR_CODEBASIS + 0x0004 // Funktion wurde nicht gefunden

void Tokenize(vector<xtstring>& roResult, xtstring const& rostr, const char C)
{
  roResult.clear();
  xtstring::const_iterator pCur = rostr.begin();
  xtstring::const_iterator end = rostr.end();
  xtstring curToken;

  while(pCur != end)
  {
    bool bEscaped = false;
    bool bInQuote = false;
    bool bContinue = true;
    curToken = _XT("");

    for(;(pCur != end) && bContinue;++pCur)
    {
      if(*pCur == C)
      {
        bEscaped=false;
        if(bInQuote){
          curToken+=C;
        }
        else{
          bContinue = false;
        }
      }
      else
      {
        switch(*pCur)
        {
        case '\'':
          if(!bEscaped){
            bInQuote = !bInQuote;
            curToken += *pCur;			
          }
          else{
            bEscaped = false;
            curToken+= _XT("\'");
          }
          break;
        case 'N':
        case 'n':
          if(bEscaped){
            curToken += _XT("\n");
          }
          else{
            curToken+=*pCur;
          }
          bEscaped = false;
          break;

        default:
          bEscaped = false;
          curToken += *pCur;
        }
      }
    }
    curToken.Trim();
    roResult.push_back(curToken);
  }
}

CProductionVariable::CProductionVariable():m_bInit(false)
{
}

void CProductionVariable::SetFieldValue(const CXPubVariant& cValue)
{ 
	m_sValue = cValue;
}

void CProductionVariable::GetVariableEntityName(xtstring& cValue)
{ 
	cValue = m_cVarName;

// ENTITY rausschneiden 
	size_t pos =  cValue.find(_XT("."));
	if (pos != xtstring::npos)
		cValue.erase(pos, cValue.length() - pos);
	else
		cValue.erase(0, cValue.length());

}

void CProductionVariable::GetVariableFieldName(xtstring& cValue)
{ 
	cValue = m_cVarName;
	// ENTITY rausschneiden 
	size_t pos =  cValue.find(_XT("."));
	if (pos != xtstring::npos)
		cValue.erase(0, pos + 1);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CProductionToken::CProductionToken(CExpression *pExpression,
                                   const xtstring& cValues,
                                   const xtstring& cSymbols,
                                   const size_t& nID)
{
	m_pExpression = pExpression;
  m_nID = nID;
  
	m_cSymbols = cSymbols;
	
// eine Variable kann nur bei IDENTIFIER vorkommen
  if(m_pExpression->IsIdentifier(m_cSymbols))
	{	
		m_cVariableName = cValues; // evtl. Variablenname
	}

	if(m_pExpression->IsOperand(m_cSymbols))
	{
		if(cSymbols == EXPRESSION_IDTRUE)
			m_cValue.SetValue((short)1);
		else
		if(cSymbols == EXPRESSION_IDFALSE)
			m_cValue.SetValue((short)0);
		else
		if(cSymbols == EXPRESSION_FLOAT0 || cSymbols == EXPRESSION_FLOAT1 ||
        cSymbols == EXPRESSION_FLOAT2)
			m_cValue.SetTypedValue(cValues, tFieldTypeDouble );
		else
		if(cSymbols == EXPRESSION_DECIMAL)
			m_cValue.SetTypedValue(cValues, tFieldTypeBigInt);
		else
		{
			m_cValue.SetTypedValue(cValues, tFieldTypeVarChar);
		}
	}

}

CProductionToken::CProductionToken()
{
	Clear();
}

void CProductionToken::Clear()
{
	m_cValue.Clear();
	m_cVariableName.empty();
	m_cSymbols.empty();
}

bool CProductionToken::GetVariable()
{
  if( !m_pExpression->GetVariable(m_cVariableName, m_cValue) )
  {
    if(m_pExpression->m_bConditionEvaluated)
      throw(ERR_EXPR_NOERROR_SETRESULT_FALSE);
    else
// Verarbeitung wird abgebrochen, es wird keine Zuweisung gemacht
      throw(ERR_EXPR_ERROR_WITHOUT_MESSAGE); 
    return false;
  }
  else
    return true;
}

void CProductionToken::GetValue(CXPubVariant& cResult)
{
  GetIdentifier();
  cResult = m_cValue;
}

void CProductionToken::GetIdentifier()
{
  if(m_pExpression->IsIdentifier(m_cSymbols))
    GetVariable();   // MAK: 26.01.2005
}

void CProductionToken::GetValue(xtstring& sValue)
{
  GetIdentifier();
	sValue = (xtstring)m_cValue;
}

void CProductionToken::GetValue(long& lValue)
{
  GetIdentifier();
	lValue = (long)m_cValue;
}

void CProductionToken::GetValue(double& lValue)
{
  GetIdentifier();
	lValue = (double)m_cValue;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExpression::CExpression(XPubPythonState* pXPubPythonState,
                         CVariableValueCallBack* pVarValue)
            :m_bParseError(false),
             m_pXPubPythonState(pXPubPythonState),
             m_pVarValueCallBack(pVarValue)
{
  // m_pVarValueCallBack muss immer gesetzt werden
  // wegen CFormatWorker muss m_pVarValueCallBack zur Zeit nicht gesetzt werden
  // assert(m_pVarValueCallBack);
}

CExpression::~CExpression()
{
}

bool CExpression::GetVariable(xtstring& sVariableName, CXPubVariant& sValue)
{
  if (!m_pVarValueCallBack)
    return false;

  CProductionVariable cProdVar;
  cProdVar.m_cVarName = sVariableName;
  xtstring sEntityName, sFieldName;
  cProdVar.GetVariableEntityName(sEntityName);
  cProdVar.GetVariableFieldName(sFieldName);
  // wegen CFormatWorker muss m_pVarValueCallBack zur Zeit nicht gesetzt werden
  bool bRetVal = m_pVarValueCallBack->GetValue_CB(sEntityName, sFieldName, sValue);
  
  return bRetVal;

/*
  map<xtstring, CProductionVariable>::iterator cIt; 
  cIt = m_lstVariable.find(sVariableName);
  if( cIt != m_lstVariable.end() )
  {
    // sValue.SetTypedValue((*cIt).second.m_sValue.sValue, (*cIt).second.m_sValue.vt); 
    sValue = (*cIt).second.m_sValue;
    return true;
  }
  else
*/
}

bool CExpression::DeleteID(ProductionTokenList& prodTokenList, size_t& nID)
{
  list<CProductionToken>::iterator First = prodTokenList.begin(); 
  for (; First != prodTokenList.end(); ++First)
  {
    if ((*First).m_nID == nID)
    {
      prodTokenList.erase(First);
      return true;
    }
  }
  return false;
}

bool CExpression::IsTextType(CProductionToken& cOpLeft)
{
  TFieldType m_nFieldType = cOpLeft.m_cValue.GetType();
	return CXPubVariant::GetGroupOfFieldType(m_nFieldType) == tFieldTypeGroupText;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// das Ergebnis steht im Operator
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CExpression::EvaluateMathematical(CProductionToken& cOpLeft,
																			CProductionToken& cOpRight, CProductionToken& cOperator)
{
	double lLeft=0, lRight=0;
	cOpLeft.GetValue(lLeft);
	cOpRight.GetValue(lRight);

	double dResult;

	if(cOperator.m_cSymbols == EXPRESSION_ADDITION)
		dResult = lLeft + lRight;
	else
	if(cOperator.m_cSymbols == EXPRESSION_SUBTRACTION)
		dResult = lLeft - lRight;
	else
	if(cOperator.m_cSymbols == _XT("%"))
		dResult = fmod(lLeft, lRight);
	else
	if(cOperator.m_cSymbols == EXPRESSION_MULITPLICATION)
		dResult = lLeft * lRight;
	else
	if(cOperator.m_cSymbols == EXPRESSION_DIVISION)
	{
		if(lRight != 0.0l)
			dResult = lLeft / lRight;
		else
		{
			dResult = 0l;
			throw(ERR_EXPR_DIVIDEBYZERO);
		}
	}
	// Ergebnis als Operand im Operator zurückschreiben
	cOperator.Clear();
	cOperator.m_cSymbols = EXPRESSION_IDENTIFIER;

// wir legen Ergebnis  in double ab, auch wenn Nachkommastellen da sind
	// unused. double nMod = fmod(dResult,1l);

  cOperator.m_cSymbols = EXPRESSION_FLOAT0;
	cOperator.m_cValue.SetValue(dResult);

	return true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// das Ergebnis steht im Operator
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CExpression::EvaluateMathComparision(CProductionToken& cOpLeft,
     CProductionToken& cOpRight, CProductionToken& cOperator)
{
  double lLeft=0, lRight=0;
  cOpLeft.GetValue(lLeft);
  cOpRight.GetValue(lRight);

	bool bResult;
  if(cOperator.m_cSymbols == EXPRESSION_EQUAL_EQUAL_COMPARISON)
    bResult = lLeft == lRight;
  else
    if(cOperator.m_cSymbols == EXPRESSION_LESS_EQUAL_COMPARISON)
      bResult = lLeft <= lRight;
  else
    if(cOperator.m_cSymbols == EXPRESSION_GREATHER_EQUAL_COMPARISON)
      bResult = lLeft >= lRight;
  else
    if(cOperator.m_cSymbols == EXPRESSION_GREATHER_COMPARISON)
      bResult = lLeft > lRight;
  else
    if(cOperator.m_cSymbols == EXPRESSION_LESS_COMPARISON)
      bResult = lLeft < lRight;
  else
    if(cOperator.m_cSymbols == EXPRESSION_NOT_EQUAL_COMPARISON)
      bResult = lLeft != lRight;
  else
    if(cOperator.m_cSymbols == EXPRESSION_NOT_LESS_COMPARISON)
      bResult = !(lLeft < lRight);
  else
    if(cOperator.m_cSymbols == EXPRESSION_NOT_GREATHER_COMPARISON)
      bResult = !(lLeft > lRight);

	cOperator.Clear();
	cOperator.m_cSymbols = EXPRESSION_DECIMAL;
	cOperator.m_cValue.SetTypedValue(bResult?_XT("1"):_XT("0"), tFieldTypeTinyInt);
	return true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// das Ergebnis steht im Operator
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CExpression::EvaluateStringComparision(CProductionToken& cOpLeft,
    CProductionToken& cOpRight, CProductionToken& cOperator)
{
  
  xtstring sLeft, sRight;
  cOpLeft.GetValue(sLeft);
  cOpRight.GetValue(sRight);
  
  sLeft.MakeLower();
  sRight.MakeLower();

 // wir prüfen, ob wir '*' im String haben
 // und kürzen, dann die beiden Strings auf Länge vor '*'
  size_t LeftPos = sLeft.find(EXPRESSION_WILDSTAR);
  size_t RightPos = sRight.find(EXPRESSION_WILDSTAR);
  
  if(LeftPos != xtstring::npos && sRight.length() > LeftPos)
  {
    sRight.erase(LeftPos, sRight.length() - LeftPos);
    sLeft.erase(LeftPos, sLeft.length() - LeftPos);
  }
  if(RightPos != xtstring::npos && sLeft.length() > RightPos)
  {
    sLeft.erase(RightPos, sLeft.length()- RightPos);
    sRight.erase(RightPos, sRight.length()- RightPos);
  }

	bool bResult;
  if(cOperator.m_cSymbols == EXPRESSION_EQUAL_EQUAL_COMPARISON)
    bResult = sLeft == sRight;
  else
  if(cOperator.m_cSymbols == EXPRESSION_LESS_EQUAL_COMPARISON)
    bResult = sLeft <= sRight;
  else
  if(cOperator.m_cSymbols == EXPRESSION_GREATHER_EQUAL_COMPARISON)
    bResult = sLeft >= sRight;
  else
  if(cOperator.m_cSymbols == EXPRESSION_GREATHER_COMPARISON)
    bResult = sLeft > sRight;
  else
  if(cOperator.m_cSymbols == EXPRESSION_LESS_COMPARISON)
    bResult = sLeft < sRight;
  else
  if(cOperator.m_cSymbols == EXPRESSION_NOT_EQUAL_COMPARISON)
    bResult = sLeft != sRight;
  else
  if(cOperator.m_cSymbols == EXPRESSION_NOT_LESS_COMPARISON)
    bResult = !(sLeft < sRight);
  else
  if(cOperator.m_cSymbols == EXPRESSION_NOT_GREATHER_COMPARISON)
    bResult = !(sLeft > sRight);

	cOperator.Clear();
	cOperator.m_cSymbols = EXPRESSION_DECIMAL;
	cOperator.m_cValue.SetTypedValue(bResult?_XT("1"):_XT("0"), tFieldTypeTinyInt);
	return true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// das Ergebnis steht im Operator
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CExpression::EvaluateLogical(CProductionToken& cOpLeft,
                                 CProductionToken& cOpRight, CProductionToken& cOperator)
{

	long nLeft, nRight;
	cOpLeft.GetValue(nLeft);
  
	bool bResult;

  if(cOperator.m_cSymbols == EXPRESSION_BITAND)
  {
      cOpRight.GetValue(nRight);
      bResult = (bool)(nLeft & nRight) > 0;
	}
	else
  if(cOperator.m_cSymbols == EXPRESSION_BITOR)
  {
      cOpRight.GetValue(nRight);
      bResult = (bool)(nLeft | nRight) > 0;
	}
	else
  if(cOperator.m_cSymbols == EXPRESSION_OR)
  {
    if(nLeft)
      bResult = true;
    else
    {
      cOpRight.GetValue(nRight);
      bResult = nLeft || nRight;
    }
  }  
  else
  if(cOperator.m_cSymbols == EXPRESSION_AND)
  {
    if(!nLeft)
      bResult = false;
    else
    {
      cOpRight.GetValue(nRight);
      bResult = nLeft && nRight;
    }
  }
  
	cOperator.Clear();
	cOperator.m_cSymbols = EXPRESSION_STRING;
	cOperator.m_cValue.SetTypedValue(bResult?_XT("1"):_XT("0"), tFieldTypeTinyInt);

	return true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// integrierte Funktionen
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// =UCASE(FELD) 
long CExpression::_ExprFct_ucase(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 1)
  {
    xtstring sToken = (*First).m_cValue;
    sToken.MakeUpper();
    cResult = sToken;

    return 0l;
  }
  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// =LCASE(FELD) 
long CExpression::_ExprFct_lcase(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 1)
  {
    xtstring sToken = (*First).m_cValue;
    sToken.MakeLower();
    cResult = sToken;

    return 0l;
  }
  return 1l;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// FORMAT(String, '###.###')
///////////////////////////////////////////////////////////////////////////////////////////////////
long CExpression::_FnFct_format(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 
  if(cFuncParaList.size() == 2)
  {
   // xtstring sVal = ((xtstring)(*First).m_cValue);
    CXPubVariant cVariant = ((*First).m_cValue);
    ++First;
    xtstring sMask = ((xtstring)(*First).m_cValue);
    if(!cVariant.IsEmpty())
    {
      //cVariant.SetValue(atof(sVal.c_str()));
      cResult = cVariant.GetMaskedValue(sMask);
      return 0l;
    }
    else
    {
    }
    return 0l;
  }
  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// DATE('Now', '%Y%m%d%H%M%S')
///////////////////////////////////////////////////////////////////////////////////////////////////
long CExpression::_FnFct_date(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 
  if(cFuncParaList.size() == 2)
  {
    xtstring sVal = ((xtstring)(*First).m_cValue);
    ++First;
    xtstring sFormat = ((xtstring)(*First).m_cValue);
    if(sVal == _XT("Now"))
    {
      time_t nActualTime = time(NULL);
      struct tm* pLocalTime = localtime(&nActualTime);
      char sFormatedTime[100];
      strftime(sFormatedTime, sizeof(sFormatedTime), sFormat.c_str(), pLocalTime); // "%Y%m%d%H%M%S"

      cResult = (xtstring)sFormatedTime;
      return 0l;

    }
    else
    {
    }
    return 0l;
  }
  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// LIST(FELD, Pos, 'Delimeter') # Erste Element wird in 'pos' mit 1 angegeben
//															# Element auf -1, Funktion liefert Anzahl der zu erwartenden Elemente
long CExpression::_FnFct_list(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 3)
  {
    int pos;
	 // Feld:String
    xtstring sVal = ((xtstring)(*First).m_cValue);
	 // Position
		pos = (long)(*++First).m_cValue; 
	 // Trenner
		xtstring sDelim = (xtstring)(*++First).m_cValue;

		if(pos <= 0)
		{
	   // Rückgabe aus Funktionen ist String	
			sVal.Format(_XT("%ld"), sVal.GetFieldCount(sDelim.c_str()));
			cResult = sVal;
	    return 0l;
		}
		--pos;
		if (pos >= 0 && pos < sVal.GetFieldCount(sDelim.c_str()))
		{
			xtstring sOut(sVal.GetField(sDelim.c_str(), pos));
			sOut.Trim();
			cResult = sOut;
		}
		return 0l;
	}
	return 1l;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// MID(FELD, "String", BegPos, Length)
long CExpression::_FnFct_mid(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 3)
  {
    const char *z;
    const char *z2;
    int i;
    int p1, p2, len;

    xtstring sVal = ((xtstring)(*First).m_cValue);

    z = sVal.c_str();
    if( z==0 ) return 1l;
    p1 = (long)(*++First).m_cValue; 
    p2 = (long)(*++First).m_cValue;

    for(len=0, z2=z; *z2; z2++){ if( (0xc0&*z2)!=0x80 ) len++; }
    if( p1<0 )
    {
      p1 += len;
      if( p1<0 ){
        p2 += p1;
        p1 = 0;
    }
    }else if( p1>0 ){
      p1--;
    }
    if( p1+p2>len ){
      p2 = len-p1;
    }
    for(i=0; i<p1 && z[i]; i++){
      if( (z[i]&0xc0)==0x80 ) p1++;
    }
    while( z[i] && (z[i]&0xc0)==0x80 ){ i++; p1++; }
    for(; i<p1+p2 && z[i]; i++){
      if( (z[i]&0xc0)==0x80 ) p2++;
    }
    while( z[i] && (z[i]&0xc0)==0x80 ){ i++; p2++; }
    if( p2<0 ) p2 = 0;
    
    cResult = sVal.Mid(p1,p2);
   
    return 0l;
  }
  return 1l;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TRIM(" ")
long CExpression::_FnFct_trim(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 1)
  {
//    cResult = (short)((xtstring)(*First).m_cValue).size();//strlen((*First).m_cValue.sValue.c_str());
    xtstring sStr = ((xtstring)(*First).m_cValue);
    xtstring sPar = ((xtstring)(*++First).m_cValue);
		
		sStr.Trim(sPar.c_str());
		cResult = CXPubVariant(sStr, tFieldTypeVarChar);
    return 0l;
  }
  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// LENTRIM(" ")
long CExpression::_FnFct_trimlen(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 2)
  {
    xtstring sStr = ((xtstring)(*First).m_cValue);
    xtstring sPar = ((xtstring)(*++First).m_cValue);
		
		sStr.Trim(sPar.c_str());
		cResult = (long)sStr.size();
    return 0l;
  }
  if(cFuncParaList.size() == 1)
  {
    xtstring sStr = ((xtstring)(*First).m_cValue);
    xtstring sPar = (xtstring(_XT(" ")));
		
		sStr.Trim(sPar.c_str());
		cResult = (long)sStr.size();
    return 0l;
  }

  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// STRLEN("ABCDEF")
long CExpression::_FnFct_strlen(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 1)
  {
    cResult = (short)((xtstring)(*First).m_cValue).size();//strlen((*First).m_cValue.sValue.c_str());
    return 0l;
  }
  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// =IF(B4>50, "Pass", "Fail") 
long CExpression::_ExprFct_if(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 3)
  {
    if((*First).m_cValue == true)
      cResult = (*(++First)).m_cValue;
    else
    { 
      ++First;++First;
      cResult = (*First).m_cValue;
    }
    return 0l;
  }
  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// =FIND(FELD, "STRING", BegPos, Length, NoCase) 
// Length: -1 ist gesamte LÃ¤nge
// Start: 1 Index auf Beginnposition
// NoCase: True oder False
long CExpression::_ExprFct_find(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 5)
  {
    xtstring sToken = (*First).m_cValue;
    xtstring sSearchStr = (*++First).m_cValue;
    long nStart = ((long)(*++First).m_cValue) - 1;

    if(nStart < 0)
      return 1l;

    long nLength = (long)(*++First).m_cValue;
    long nNoCase = (long)(*++First).m_cValue;

    xtstring sCmpToken;
    if(nLength == -1)
      sCmpToken = sToken.Mid(nStart);     
    else
      sCmpToken = sToken.Mid(nStart, nLength);     

    cResult.SetTypedValue(_XT("0"), tFieldTypeTinyInt);
    if(nNoCase == 0)
    {
      if(sCmpToken.FindNoCase(sSearchStr.c_str()) != xtstring::npos)
        cResult.SetTypedValue(_XT("1"), tFieldTypeTinyInt);
    }
    else
    {
      if(sCmpToken.Find(sSearchStr.c_str()) != xtstring::npos)
        cResult.SetTypedValue(_XT("1"), tFieldTypeTinyInt);
   }

    return 0l;
  }
  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// =UTF8(FELD, 0) convert to system lokale
long CExpression::_ExprFct_utf8(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 3)
  {

    xtstring sToken = (*First).m_cValue;
    xtstring sT1 = (*++First).m_cValue;
    xtstring sT2 = (*++First).m_cValue;
    sToken.ReplaceAll(sT1, sT2);
    cResult = sToken;

    return 0l;
  }
  return 1l;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// =REPLACE(FELD, "+", "") 
long CExpression::_ExprFct_replace(PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
  PythonFunctionParameterListIterator First = cFuncParaList.begin(); 

  if(cFuncParaList.size() == 3)
  {

    xtstring sToken = (*First).m_cValue;
    xtstring sT1 = (*++First).m_cValue;
    xtstring sT2 = (*++First).m_cValue;
    sToken.ReplaceAll(sT1, sT2);
    cResult = sToken;

    return 0l;
  }
  return 1l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// das Funktionsergebnis steht im cFunctionOperanden
///////////////////////////////////////////////////////////////////////////////////////////////////

long CExpression::EvaluateArray(CProductionToken& cFunctionOperand)
{
  CXPubVariant cPubResult;

  // Wertberechnung im Index auflösen, auch komplexe Berechnungen sind möglich
  PythonFunctionParameterListIterator itList = cFunctionOperand.m_cFuncParaList.begin(); 
  if(itList != cFunctionOperand.m_cFuncParaList.end())
  {
    map<xtstring, CProductionVariable>::iterator cIt; 
    CPythonFunctionParameters& cPara = (*itList);
    if(cPara.m_cFieldName.length() > 0 && cPara.m_cFieldName.find(_XT(",")) != xtstring::npos)
    {
      CExpression cExpress(m_pXPubPythonState, m_pVarValueCallBack);
      cPara.m_cFieldName.ReplaceAll(_XT(","), _XT(";"));
      cPara.m_cSymbolName.ReplaceAll(_XT(" "), _XT(";"));
      cExpress.Set(xtstring(_XT("1")),cPara.m_cSymbolName, cPara.m_cFieldName);
      // Variablen der neuen Instanz übergeben 
      cExpress.m_lstVariable = m_lstVariable;
      cExpress.Evaluate(m_bConditionEvaluated, cPubResult);
      cPara.m_cValue = cPubResult;
    }
  // hier haben wir den einfachen ARRAY-Fall z.B. "Field[1]"
    else
     cPubResult = cPara.m_cValue;
  
    xtstring sArrayName(cFunctionOperand.m_cFunctionName);
    xtstring sIndex = cPubResult.GetTypedValue(tFieldTypeSmallInt);
		if(sIndex.empty())
		{
			cPubResult.Clear();
			if(!GetVariable(cPara.m_cFieldName, cPubResult))
				throw(ERR_EXPR_VARIABLEUNASSIGNED);

	    xtstring sVarValue = cPubResult.GetTypedValue(tFieldTypeVarWChar);
			sArrayName += _XT("[") + sVarValue + _XT("]");
		}
		else
			sArrayName += _XT("[") + sIndex + _XT("]");

    cPubResult.Clear();
    if(!GetVariable(sArrayName, cPubResult))
      throw(ERR_EXPR_VARIABLEUNASSIGNED);

    cFunctionOperand.Clear();
    cFunctionOperand.m_cSymbols = EXPRESSION_STRING;
    cFunctionOperand.m_cValue = cPubResult;
    return 0l;

  }
  throw(ERR_EXPR_MALFORMEDXML);
  return 1l;

}

long CExpression::EvaluateFunction(CProductionToken& cFunctionOperand)
{

// Parameterliste auflösen
	PythonFunctionParameterListIterator itList = cFunctionOperand.m_cFuncParaList.begin(); 
	for(; itList != cFunctionOperand.m_cFuncParaList.end(); itList++)
	{
		map<xtstring, CProductionVariable>::iterator cIt; 
	  CPythonFunctionParameters& cPara = (*itList);
		if(cPara.m_cFieldName.length() > 0)
		{
    // Ist Parameter Expression 
      // wir prüfen, ob Kommas da sind und ob wir mehrere Symbole haben(Symbole sind durch Leerstelle getrennt)
      if(cPara.m_cSymbolName.find(_XT(" ")) != xtstring::npos && cPara.m_cFieldName.find(_XT(",")) != xtstring::npos)
      {
        CExpression cExpress(m_pXPubPythonState, m_pVarValueCallBack);
        cPara.m_cFieldName.ReplaceAll(_XT(","), _XT(";"));
        cPara.m_cSymbolName.ReplaceAll(_XT(" "), _XT(";"));
        cExpress.Set(xtstring(_XT("1")),cPara.m_cSymbolName, cPara.m_cFieldName);
        CXPubVariant cPubResult;
       // Variablen der neuen Instanz übergeben 
        cExpress.m_lstVariable = m_lstVariable;
        cExpress.Evaluate(m_bConditionEvaluated, cPubResult);
        cPara.m_cValue = cPubResult;
      }
    
    // Identifier auflösen
    // MAK: 25.01.2005
      cIt = m_lstVariable.find(cPara.m_cFieldName);
      if(cIt != m_lstVariable.end())
      {
        GetVariable(cPara.m_cFieldName, cPara.m_cValue);
      }

		}
	}

	xtstring sLibrary(cFunctionOperand.m_cFunctionName);
	size_t nSyPos = sLibrary.find(_XT("."));
	if(nSyPos != xtstring::npos)
		sLibrary.erase(nSyPos, sLibrary.length() - nSyPos);

	xtstring sFunction(cFunctionOperand.m_cFunctionName);
	nSyPos = sFunction.find(_XT("."));
	if(nSyPos != xtstring::npos)
		sFunction.erase(0, nSyPos+1);

	CXPubVariant cResult;

  long lResult = 0;

  if(sFunction == _XT("IF"))
    lResult = _ExprFct_if(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("FIND"))
    lResult = _ExprFct_find(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("STRLEN"))
    lResult = _FnFct_strlen(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("LIST"))
    lResult = _FnFct_list(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("MID"))
    lResult = _FnFct_mid(cFunctionOperand.m_cFuncParaList, cResult);
  else 
  if(sFunction == _XT("TRIMLEN"))
    lResult = _FnFct_trimlen(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("TRIM"))
    lResult = _FnFct_trim(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("DATE"))
    lResult = _FnFct_date(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("FORMAT"))
    lResult = _FnFct_format(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("REPLACE"))
    lResult = _ExprFct_replace(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("UCASE"))
    lResult = _ExprFct_ucase(cFunctionOperand.m_cFuncParaList, cResult);
  else
  if(sFunction == _XT("LCASE"))
    lResult = _ExprFct_lcase(cFunctionOperand.m_cFuncParaList, cResult);
  else
  {
    lResult = m_cPython.Function(m_pXPubPythonState,
                                 sLibrary,
                                 sFunction, 
                                 cFunctionOperand.m_cFuncParaList,
                                 cResult);
    if(lResult != 0)
    {
      m_lastError = m_cPython.GetLastError();
    }
  }
			//MessageBox(0, xtstring(cResult).c_str(), sFunction.c_str(),0);

	cFunctionOperand.Clear();
	cFunctionOperand.m_cSymbols = EXPRESSION_STRING;
	cFunctionOperand.m_cValue = cResult;
	return lResult;

}

bool CExpression::EvaluateStringConcatination(CProductionToken& cOpLeft,
																	CProductionToken& cOpRight, CProductionToken& cOperator)
{
	cOperator.Clear();

  xtstring sLeftVal, sRightVal;
  cOpLeft.GetValue(sLeftVal);
  cOpRight.GetValue(sRightVal);

  cOperator.m_cValue.SetTypedValue(sLeftVal+sRightVal, CXPubVariant::GetDefaultFieldTypeOfGroup(tFieldTypeGroupText));//tFieldTypeVarChar);
	cOperator.m_cSymbols = EXPRESSION_STRING;

	return 0;

}

long CExpression::Evaluate(bool bCondition,
                           CXPubVariant& cResult)
{
  // bCondition = true --> Bedingung
  // bCondition = false --> Zuweisung
  m_bConditionEvaluated = bCondition;

	long lReturn = EXPRESSION_ERROR_UNDEFINED;
	cResult.Clear(); // Rückgabestring leeren

 // Liste duplizieren
  list<CProductionToken> prodlist; 
  for(int i = 0; i < (int)m_prodlist.size(); i++)
	{ 
		prodlist.push_back(m_prodlist[i]);
	}

 // iterator für Liste erzeugen
  list<CProductionToken>::iterator itList = prodlist.begin(); 
  list<CProductionToken>::iterator itListEnd = prodlist.end(); 
  --itListEnd;	
  // unused. CProductionToken& cEndCheckOp = *(itListEnd);

////////////////////////////////////////////////////////////////////////////////////////
//
// einfache Zuweisung oder Funktionsauswertung aus einem Operanden 
//
	if(prodlist.size() == 1)
	{
		CProductionToken& cOperand = *(itList);
		
		if(IsFunction(cOperand.m_cSymbols))
		{
			lReturn = EvaluateFunction(cOperand);
			if(lReturn != EXPRESSION_ERROR_NONE)
      {
        throw(ERR_EXPR_SCRIPTING_FUNCTIONCALL_FAILED);
		  }
    	cResult = cOperand.m_cValue;
		}
		else
    if(IsArray(cOperand.m_cSymbols))
    {
      lReturn = EvaluateArray(cOperand);
      if(lReturn != EXPRESSION_ERROR_NONE)
      {
        throw(ERR_EXPR_MALFORMEDXML);
      }
      cResult = cOperand.m_cValue;
    }
    else
		if(IsIdentifier(cOperand.m_cSymbols) || IsOperand(cOperand.m_cSymbols))
		{
			cOperand.GetValue(cResult);
			lReturn = EXPRESSION_ERROR_NONE;
		}
		else 
		{
			throw(ERR_EXPR_MALFORMEDXML);
		}
		return lReturn;
	}

//////////////////////////////* Ende der einfachen Zuweisung *///////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//
// hier verarbeiten wir komplexen Bedingungen und Zuweisungen
//
////////////////////////////////////////////////////////////////////////////////////////

	while( prodlist.size() >= 3 )
  {
    if(itList == prodlist.end())
      break;
   
   // zur weiteren Untersuchung rausextrahieren
    CProductionToken& cOpLeft = *(itList);
    CProductionToken& cOpRight = *(++itList);
    CProductionToken& cOperator = *(++itList);

  //////////////////////////////////////////////////////////////////////////
  // Optimierung bei && und ||

   if(itList != itListEnd)
    {
      CProductionToken& cCheckOp = *(++itList);
      if(itList == itListEnd && IsLogical(cCheckOp.m_cSymbols) && !IsBitLogical(cCheckOp.m_cSymbols))
      {
        long nValue;
        list<CProductionToken>::iterator itBegList = prodlist.begin(); 
        CProductionToken& cBegOp = *(itBegList);
        cBegOp.GetValue(nValue);
        if( (cCheckOp.m_cSymbols == EXPRESSION_AND)
						&& nValue == 0)
        {
          cResult.Clear();
          cResult.SetTypedValue(_XT("0"), tFieldTypeTinyInt);
          lReturn = EXPRESSION_ERROR_NONE;
          return lReturn;
        }
        else
        if( (cCheckOp.m_cSymbols == EXPRESSION_OR)
							&& nValue == 1)
        {
          cResult.Clear();
          cResult.SetTypedValue(_XT("1"), tFieldTypeTinyInt);
          lReturn = EXPRESSION_ERROR_NONE;
          return lReturn;
        }
      }
      --itList;
    }
  //////////////////////////////////////////////////////////////////////////

		bool b1 = IsOperand(cOpLeft.m_cSymbols);
		bool b2 = IsOperand(cOpRight.m_cSymbols);

// Operation herausfinden
		// unused. bool b3 = !IsOperand(cOperator.m_cSymbols);
		bool b4 = IsLogical(cOperator.m_cSymbols);      // "and" oder "or"
    bool b5 = IsComparison(cOperator.m_cSymbols);   // "==", "<="
    bool b6 = IsMathematical(cOperator.m_cSymbols); // "-", "+", "%"

// das können wir nun verarbeiten
    if(b1 && b2 && (b4||b5||b6)) 
    {
     
// Funktionen auflösen
   // Das Ergebnis wird in Übergabeparameter zurückgeschrieben  	
	
			if(IsFunction(cOpLeft.m_cSymbols))	
      {
				if(EvaluateFunction(cOpLeft) != EXPRESSION_ERROR_NONE)
        {
          throw(ERR_EXPR_SCRIPTING_FUNCTIONCALL_FAILED);
        }
      }

			if(IsFunction(cOpRight.m_cSymbols))	
      {
        if(EvaluateFunction(cOpRight) != EXPRESSION_ERROR_NONE)
        {
          throw(ERR_EXPR_SCRIPTING_FUNCTIONCALL_FAILED);
        }
      }
		
	 // folgende Dinge müssen noch getestet werden.
			// a) Parameter double in Python
		  // b) Rückgabe double aus Pyhton

   // kann erst nach Funktionsauflösung bestimmt werden, da evtl. String aus Funktion zurückkommt
			bool b7 = IsStringConcatination(cOpLeft, cOpRight, cOperator); // e.g. 'Serbrief' + 1 Ergebnis 'Serbrief1'
			bool b8 = b5 && (IsTextType(cOpLeft) || IsTextType(cOpRight));// Stringvergleich:
			b6 = b7?false:b6;		// Stringverkettung: wenn b7 dann haben wir stringverkettung '+'	
			b5 = b8?false:b5;		// Stringvergleich: wenn b8, und einer der Operanden string ist	

/////////////////////////////////////////////////////////////////////////////////////////////
// Operation evaluieren
//    Das Ergebnis steht nach 'Evaluate*' im Operator und dieser wird aus Liste nicht entfernt

			bool bReturnEval=false;

			if(b4)
        bReturnEval = EvaluateLogical(cOpLeft, cOpRight, cOperator);
     
			else if(b5) // math. Vergleich
				bReturnEval = EvaluateMathComparision(cOpLeft, cOpRight, cOperator);

			else if(b6) // mathematisch
				bReturnEval = EvaluateMathematical(cOpLeft, cOpRight, cOperator);

			else if(b7) // Stringverkettung
				bReturnEval = EvaluateStringConcatination(cOpLeft, cOpRight, cOperator);
			
			else if(b8)
				bReturnEval = EvaluateStringComparision(cOpLeft, cOpRight, cOperator);

/////////////////////////////////////////////////////////////////////////////////////////////

     // Operanden löschen
      DeleteID(prodlist, cOpLeft.m_nID);
      DeleteID(prodlist, cOpRight.m_nID);

   // Ergebnis rausschreiben, wenn nur noch ein Operand übrig ist.
			if(prodlist.size() == 1)
			{
				cResult.Clear();
        cResult = cOperator.m_cValue;
				
				if(!cResult.IsEmpty())
					lReturn = EXPRESSION_ERROR_NONE;
				else
					throw(ERR_EXPR_VARIABLEUNASSIGNED);
			}
			
			if(prodlist.size() == 2)
			{
				throw(ERR_EXPR_MALFORMEDXML);
			}

     // wir beginnen wieder vom Beginn der Liste
      itList = prodlist.begin(); 
    }
    else
    {
      --itList; // um 1 zurücksetzen, da wir sonst nicht alle verarbeiten
    }

  }
  return lReturn;

}

bool CExpression::Set(const xtstring& Condition, const xtstring& Symbols, const xtstring& Values)
{

	if(Condition.begin() == Condition.end())
		return false;

	if(Symbols.begin() == Symbols.end() ||
    Values.begin() == Values.end())
    return false;

  if(m_prodlist.size() || m_lstVariable.size())
  { 
    m_prodlist.empty();
    m_lstVariable.clear();
  }

// condition für Debug in Klasse ablegen
  m_cCondition = Condition;

 // gesamte Bedingung zerlegen
  vector<xtstring> vSymbol;
  Tokenize(vSymbol, Symbols, ';');

  xtstring slValues(Values);
  slValues.Trim();

  vector<xtstring> vValues;
  Tokenize(vValues, slValues, ';');

 // Elemente der Bedingung für Datenzuordnung vorbereiten
  // unused. int j=0;
  for (size_t i=0; i<vSymbol.size() && i<vValues.size();i++)
  {
 		xtstring sValues(vValues[i]), sSymbol(vSymbol[i]);

    if(IsString(sSymbol))
      sValues.Trim(_XT("'")); // die quotes entfernen

    CProductionToken cProd(this, sValues, sSymbol, i); // i=ID zur späteren Identifizierung
		
	 // Funktionen und Parameter verarbeiten
  	if(IsFunction(sSymbol) || IsArray(sSymbol)) // ist es eine Funktion mit Parametern?
		{
			size_t nSympos = sSymbol.find(_XT("["));
			size_t nValpos = sValues.find(_XT("["));

		 // die folgende Bedingung wird war, wenn wir Funktion mit Array verarbeiten!
		 // das funktionsende wird nicht korrekt gefunden
			if(nSympos == xtstring::npos || nValpos == xtstring::npos)
				return false;

			xtstring sFuncPara(sValues);
			sFuncPara.erase(0, nValpos+1);

		 // Funktionname
			xtstring sFuncName(sValues);
			sFuncName.erase(nValpos, sFuncName.length() - nValpos);

		 // Symbol Funktionsparameter
			xtstring sFuncSymPara(sSymbol);
			sFuncSymPara.erase(0, nSympos+1);

			sFuncPara.Trim(_XT("]"));
			sFuncSymPara.Trim(_XT("]"));

		 // Parameter Value/Identifier zerlegen
      vector<xtstring> vTokValue;
      Tokenize(vTokValue, sFuncPara, '#');

		 // Parameter Symbole "IDENTIFIER" zerlegen
      vector<xtstring> vTokSymbole;
      Tokenize(vTokSymbole, sFuncSymPara, '#');

     // Parameter verarbeiten und in Liste eintragen
			for (size_t i=0; i<vTokValue.size() && i<vTokSymbole.size();i++)
			{
				xtstring sValuePar(vTokValue[i]);
				xtstring sSymbPara(vTokSymbole[i]);
				
        vector<xtstring> vParSymbole;
        Tokenize(vParSymbole, sSymbPara, ' ');

        vector<xtstring> vValuePar;
        Tokenize(vValuePar, sValuePar, ',');

      // der folgende loop extrahiert die Variablen, damit diese zugewiesen werden können
        for (size_t i=0; i<vValuePar.size() && i<vParSymbole.size(); i++)
        {
          xtstring sValuePar(vValuePar[i]);
          xtstring sSymbPara(vParSymbole[i]);
          sValuePar.Trim();sSymbPara.Trim();

          if(IsIdentifier(sSymbPara))
          {
            // hier ist eine Variable
            CProductionVariable cProdVariable;
            cProdVariable.m_cVarName = sValuePar;
            m_lstVariable[sValuePar] = cProdVariable;
          }
        }
 
				CPythonFunctionParameters cFuncPara;
				cFuncPara.Add(sValuePar, sSymbPara);
									
				if(IsString(sSymbPara))
        {
          sValues.Trim();
          sValuePar.Trim(_XT("'")); // die quotes entfernen
          cFuncPara.SetValue(CXPubVariant(sValuePar, tFieldTypeVarChar)); 
        }

				if(IsDecimal(sSymbPara))
					cFuncPara.SetValue(CXPubVariant(sValuePar, tFieldTypeInteger)); 

				if(IsFloat(sSymbPara))
					cFuncPara.SetValue(CXPubVariant(sValuePar, tFieldTypeDouble)); 

		  	cProd.m_cFuncParaList.push_back(cFuncPara);
			}

			cProd.m_cFunctionName = sFuncName;
      if(IsFunction(sSymbol))
        cProd.m_cSymbols = EXPRESSION_FUNCTION;
      if(IsArray(sSymbol))
        cProd.m_cSymbols = EXPRESSION_ARRAY;

		}
		else
    if(IsIdentifier(sSymbol))
    {
      CProductionVariable cProdVariable;
      cProdVariable.m_cVarName = sValues;
      m_lstVariable[sValues] = cProdVariable;
    }
    m_prodlist[(int)i] = cProd;
  }
  return m_prodlist.size() > 0;

}

// Member operator "->"
bool CExpression::IsIdentifier(const xtstring& cIdentifier)
{
  return cIdentifier == EXPRESSION_IDENTIFIER; //IsArray(cIdentifier -> Entfernt, da es nun über EvaluateArray läuft
}

bool CExpression::IsMember(const xtstring& cIdentifier)
{
  return (cIdentifier == EXPRESSION_ARROW ||
    cIdentifier == EXPRESSION_ARROWSTAR ||
    cIdentifier == EXPRESSION_DOT ||
    cIdentifier == EXPRESSION_SCOPERES ||
    cIdentifier == EXPRESSION_DOTSTAR );
}

bool CExpression::IsFloat(const xtstring& cIdentifier)
{
  return (cIdentifier == EXPRESSION_FLOAT0 || cIdentifier == EXPRESSION_FLOAT1 
            || cIdentifier == EXPRESSION_FLOAT2);
}

bool CExpression::IsDecimal(const xtstring& cIdentifier)
{
	return (cIdentifier == EXPRESSION_DECIMAL);
}

bool CExpression::IsString(const xtstring& cIdentifier)
{
	return (cIdentifier == EXPRESSION_STRING);
}

bool CExpression::IsOperand(const xtstring& cIdentifier)
{

  bool bRetf = IsExpression(cIdentifier) || IsIdentifier(cIdentifier)
    || IsDecimal(cIdentifier) || IsFloat(cIdentifier) || IsFunction(cIdentifier)
    || cIdentifier == EXPRESSION_IDTRUE || cIdentifier == EXPRESSION_IDFALSE
    || cIdentifier == EXPRESSION_IDNULL || cIdentifier == EXPRESSION_STRING
		|| cIdentifier == EXPRESSION_EMPTY_SQSTRING || cIdentifier == EXPRESSION_EMPTY_DQSTRING;

  return bRetf;
} 

// Ergebnis aus Berechnung
bool CExpression::IsExpression(const xtstring& cIdentifier)
{
  return (cIdentifier == EXPRESSION_EXPRESSION);
}

bool CExpression::IsFunction(const xtstring& cIdentifier)
{
  return (cIdentifier.find(EXPRESSION_FUNCTION) != xtstring::npos);
}

bool CExpression::IsArray(const xtstring& cIdentifier)
{
  return (cIdentifier.find(EXPRESSION_ARRAY) != xtstring::npos);
}

bool CExpression::IsBitLogical(const xtstring& cIdentifier)
{
  return ( cIdentifier == EXPRESSION_BITOR || cIdentifier == EXPRESSION_BITAND);
}
bool CExpression::IsLogical(const xtstring& cIdentifier)
{
  return (cIdentifier == EXPRESSION_OR || cIdentifier == EXPRESSION_AND ||
		cIdentifier == EXPRESSION_BITOR || cIdentifier == EXPRESSION_BITAND);
}

bool CExpression::IsComparison(const xtstring& cIdentifier)
{
  return (
    cIdentifier == EXPRESSION_NOT_LESS_COMPARISON ||
    cIdentifier == EXPRESSION_NOT_GREATHER_COMPARISON  ||
    cIdentifier == EXPRESSION_EQUAL_EQUAL_COMPARISON ||
    cIdentifier == EXPRESSION_LESS_EQUAL_COMPARISON  ||
    cIdentifier == EXPRESSION_GREATHER_EQUAL_COMPARISON  ||
    cIdentifier == EXPRESSION_GREATHER_COMPARISON  ||
    cIdentifier == EXPRESSION_LESS_COMPARISON  ||
    cIdentifier == EXPRESSION_NOT_EQUAL_COMPARISON);
}

// Sonderfall:
// Wenn bei einer Addition einer von zwei Operanden ein String ist, dann handelt es sich um
// eine Verkettung von Strings
bool CExpression::IsStringConcatination(CProductionToken& cOpLeft, CProductionToken& cOpRight, 
																				CProductionToken& cOperator)
{
  if( IsString(cOpLeft.m_cSymbols) || IsString(cOpRight.m_cSymbols) )
  {
    if(cOperator.m_cSymbols == EXPRESSION_ADDITION)
      return true;
  }

  cOpLeft.GetIdentifier();
  cOpRight.GetIdentifier();

	bool bResult = false;
	if(CXPubVariant::GetGroupOfFieldType(cOpRight.m_cValue.GetType()) == tFieldTypeGroupText ||
		CXPubVariant::GetGroupOfFieldType(cOpLeft.m_cValue.GetType()) == tFieldTypeGroupText )
	{
		if(cOperator.m_cSymbols == EXPRESSION_ADDITION)
			bResult = true;
	}	
	return bResult;
}

bool CExpression::IsMathematical(const xtstring& cIdentifier)
{
  return (cIdentifier == _XT("%") || cIdentifier == EXPRESSION_ADDITION || cIdentifier == EXPRESSION_SUBTRACTION
		|| cIdentifier == EXPRESSION_MULITPLICATION || cIdentifier == EXPRESSION_DIVISION);
}

bool CExpression::IsTrue(xtstring cResult)
{
	cResult.MakeUpper();
	return (cResult == EXPRESSION_IDTRUE || cResult == _XT("1"));
}

bool CExpression::IsFalse(xtstring cResult)
{
	cResult.MakeUpper();
	return (cResult == EXPRESSION_IDFALSE || cResult == _XT("0"));
}

bool CExpression::IsUnaer(const xtstring& cIdentifier, xtstring& cSymbolName)
{
  if(cIdentifier.length() > 1) // unaer hat immer Länge 1, Achtung "!=" usw.
    return false;

  bool bRetf = cIdentifier == EXPRESSION_NEGATE || cIdentifier == EXPRESSION_COMPLEMENT 
    || cIdentifier == EXPRESSION_INCREMENT || cIdentifier == EXPRESSION_DECREMENT;

  if(cIdentifier == EXPRESSION_NEGATE)
    cSymbolName = EXPRESSION_NEG;

  if(cIdentifier == EXPRESSION_COMPLEMENT)
    cSymbolName = EXPRESSION_COMP;

  if(cIdentifier == EXPRESSION_INCREMENT)
    cSymbolName = EXPRESSION_INCR;

  if(cIdentifier == EXPRESSION_DECREMENT)
    cSymbolName = EXPRESSION_DECR;

  return bRetf;
} 
