// Expression.h: interface for the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPRESSION_H__D68B21F6_1D11_44D3_A5AB_1D16F4C895E8__INCLUDED_)
#define AFX_EXPRESSION_H__D68B21F6_1D11_44D3_A5AB_1D16F4C895E8__INCLUDED_


#include "SModelInd/ModelExpImp.h"
#include "SModelInd/XPubVariant.h"
#include "XPubPythonWrapper.h"



#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


#define EXPRESSION_EMPTY_SQSTRING	_XT("SQ") // wird bei '' gesetzt, wenn String leer ist
#define EXPRESSION_EMPTY_DQSTRING	_XT("DQ") // wird bei "" gesetzt, wenn String leer ist
#define EXPRESSION_STRING				  _XT("STRING")
#define EXPRESSION_IDENTIFIER			_XT("IDENTIFIER")
#define EXPRESSION_DECIMAL					_XT("DECIMAL")
#define EXPRESSION_FLOAT0						_XT("FLOATINGconstant0")
#define EXPRESSION_FLOAT1						_XT("FLOATINGconstant1")
#define EXPRESSION_FLOAT2						_XT("FLOATINGconstant2")
#define EXPRESSION_HEX							_XT("HEXADECIMAL")
#define EXPRESSION_FUNCTION				_XT("FUNCTION")
#define EXPRESSION_ARRAY       _XT("ARRAY")
#define EXPRESSION_EXPRESSION  _XT("EXPRESSION")
#define EXPRESSION_PARSEERROR  _XT("ERROR")
#define EXPRESSION_IDTRUE      _XT("TRUE")
#define EXPRESSION_IDFALSE     _XT("FALSE")
#define EXPRESSION_IDNULL      _XT("NULL")
#define EXPRESSION_NEG         _XT("NEG")
#define EXPRESSION_COMP        _XT("COMP")

#define EXPRESSION_INCR        _XT("INCR")
#define EXPRESSION_DECR        _XT("DECR")
#define EXPRESSION_ARROW       _XT("ARROW")
#define EXPRESSION_ARROWSTAR   _XT("ARROWSTAR")
#define EXPRESSION_DOTSTAR     _XT("DOTSTAR")
#define EXPRESSION_DOT         _XT("DOT")
#define EXPRESSION_SCOPERES    _XT("SCOPERES")

#define EXPRESSION_OR            _XT("||")
#define EXPRESSION_AND           _XT("&&")
#define EXPRESSION_BITOR         _XT("|")
#define EXPRESSION_BITAND        _XT("&")

#define EXPRESSION_NEGATE        _XT("!")
#define EXPRESSION_COMPLEMENT    _XT("~")
#define EXPRESSION_INCREMENT   _XT("++")
#define EXPRESSION_DECREMENT   _XT("--")

#define EXPRESSION_ADDITION				_XT("+")
#define EXPRESSION_SUBTRACTION			_XT("-")
#define EXPRESSION_MULITPLICATION  _XT("*")
#define EXPRESSION_DIVISION				_XT("/")

#define EXPRESSION_FIELD_ARROW       _XT("->")
#define EXPRESSION_FIELD_ARROWSTAR   _XT("->*")
#define EXPRESSION_FIELD_DOTSTAR     _XT(".*")
#define EXPRESSION_FIELD_DOT         _XT(".")
#define EXPRESSION_FIELD_SCOPERES    _XT("::")

#define EXPRESSION_EQUAL_EQUAL_COMPARISON        _XT("==")
#define EXPRESSION_LESS_EQUAL_COMPARISON         _XT("<=")
#define EXPRESSION_GREATHER_EQUAL_COMPARISON     _XT(">=")
#define EXPRESSION_GREATHER_COMPARISON           _XT(">")
#define EXPRESSION_LESS_COMPARISON               _XT("<")

#define EXPRESSION_NOT_EQUAL_COMPARISON          _XT("!=")
#define EXPRESSION_NOT_LESS_COMPARISON           _XT("!<")
#define EXPRESSION_NOT_GREATHER_COMPARISON       _XT("!>")

#define EXPRESSION_WILDSTAR          _XT("*")

class CExpression;

// hieraus entsteht die Liste der Variablen, die zu befüllen sind
class XPUBMODEL_EXPORTIMPORT CProductionVariable{

public:
	CProductionVariable();	

 void GetVariableEntityName(xtstring& cEntityName);   // e.g. "Entitity" from "Entitity.FieldName"
 void GetVariableFieldName(xtstring& cFieldName);			// e.g. "FieldName" from "Entitity.FieldName"
 void SetFieldValue(const CXPubVariant& cValue);			// the value of the field

public:

// Variablenname
 xtstring m_cVarName;				// Name der Variable
 CXPubVariant m_sValue;			// Wert

protected:
  bool m_bInit;

};

// das sind die zu verarbeitenden Elemente(Operator, Operand) der Bedingung
// die Liste wird in ::Set erzeugt
class XPUBMODEL_EXPORTIMPORT CProductionToken{

  friend class CExpression;

 public:

	CProductionToken(CExpression *pExpression, const xtstring& cValues, const xtstring& cSymbols, const size_t& nID);
	CProductionToken();

	void Clear();

	void GetValue(long& lValue);
	void GetValue(double& lValue);
	void GetValue(xtstring& sValue);
	void GetValue(CXPubVariant& cResult);

  void GetIdentifier();

 protected:

// zwischen set() und Evaluate() wird eine Liste befüllt
// in Evaluate werden die Variablen aus System zugeordnet
	bool GetVariable();

	CXPubVariant m_cValue;	// Wert

// Variablenname und Symbolname
	xtstring m_cVariableName;
	xtstring m_cSymbols;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// spezielle Attribute für Funktionen
	xtstring m_cFunctionName;	// Funktion
	PythonFunctionParameterList m_cFuncParaList; 

// Verweis auf CExpression
	CExpression *m_pExpression;

private:
// eindeutige Token ID, zur Löschung von Elementen
  size_t  m_nID; 

};

typedef map<int, CProductionToken > ProductionList;
typedef map<int, int > IdentifierList;
typedef list<CProductionToken> ProductionTokenList;
typedef map<xtstring, CProductionVariable> ProductionVariableList;
class CVisibilityPreprocess;

class XPUBMODEL_EXPORTIMPORT CVariableValueCallBack
{
public:
  // constructor / destructor
  CVariableValueCallBack(){m_pOutItem = 0;m_pLogItem = 0;};
  virtual ~CVariableValueCallBack(){};

  // Funktion, mit welcher kann CExpression nach Werten fragen.
  // Wenn diese Funktion 'false' liefert, es muss alles abgebrochen und beendet werden.
  // Was betrifft Fehlermeldungen, es bleibt wie vorher, CExpression muss
  // gar keine Meldung machen. (Exceptions kann man weiterhin in 'Evaluate' benutzen.)
  // Diese Funktion ruft die untere rein virtuelle Funktion.
  virtual bool GetValue_CB(const xtstring& sEntityName, const xtstring& sFieldName, CXPubVariant& cValue) = 0;

protected:
  // Diese Funktion ist von CDataPoolLevel verlangt. Es ist so, dass zu einem Fehler
  // mehrere Texte ausgegeben werden koennen, aber immer in "einer Zeile", dadurch
  // muessen diese zwei Variablen auch da benutzt werden, um ein Fehler 
  void SetMSGItemsInVariableValueCallBack(OUT_MESSAGE_ITEM* pOutItem, LOG_MESSAGE_ITEM* pLogItem)
  {m_pOutItem = pOutItem; m_pLogItem = pLogItem;};
  void SetMSGTraceInfoInVariableValueCallBack(xtstring* pTraceInfo)
  {m_pTraceInfo = pTraceInfo;};
  void SetMSGVisibilityInVariableValueCallBack(CVisibilityPreprocess* pVisibility)
  {m_pVisibility = pVisibility;}

protected:
/*
  // Diese Funktion ist in abgeleitete Klasse - CDataPoolLevel.
  // Weil diese Funktion schon dort vorhanden ist und auch fuer andere Zwecke
  // benutzt ist, diese Funktionsdeklaration ist einfach uebernommen
  virtual bool GetValue_CB(const xtstring& sEntityName,
                        const xtstring& sMiddlePartName,
                        xtstring& sFieldName,
                        CXPubVariant& cValue,
                        OUT_MESSAGE_ITEM& nOutItem,
                        LOG_MESSAGE_ITEM& nLogItem) = 0;
*/
  // Wie oben erwaehnt, wegen mehreren Fehlertexten, die in "einer Zeile"
  // dargestellt sind.
  OUT_MESSAGE_ITEM* m_pOutItem;
  LOG_MESSAGE_ITEM* m_pLogItem;

  xtstring*               m_pTraceInfo;
  CVisibilityPreprocess*  m_pVisibility;
};

class XPUBMODEL_EXPORTIMPORT CExpression  
{

	friend class CProductionToken;

public:
	
	CExpression(XPubPythonState* pXPubPythonState, CVariableValueCallBack* pVarValue);
  virtual ~CExpression();

  bool Set(const xtstring& Condition, const xtstring& Symbols, const xtstring& Values);
  long Evaluate(bool bCondition, CXPubVariant& cResult);
  bool GetVariable(xtstring& sVariableName, CXPubVariant& sValue); // Identifier besorgen
  void GetErrorInfo(xtstring& ErrrorMessage){ErrrorMessage = m_lastError;}

// der gesamte Bedingungsausdruck aufbereitet in einzelne Tokens
	ProductionList m_prodlist; 

// alle zu befüllende Variable
	ProductionVariableList  m_lstVariable;

protected:
// integrierte Funktionen
  long _ExprFct_if(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _FnFct_strlen(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _FnFct_trim(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _FnFct_trimlen(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _FnFct_mid(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _FnFct_list(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _FnFct_date(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _FnFct_format(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _ExprFct_replace(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _ExprFct_find(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _ExprFct_lcase(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _ExprFct_ucase(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);
  long _ExprFct_utf8(PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);

protected:
  bool IsTextType(CProductionToken& cOperand);

  long EvaluateArray(CProductionToken& cOperand);
	long EvaluateFunction(CProductionToken& cOperand);
	bool EvaluateMathComparision(CProductionToken& cOpLeft ,
		CProductionToken& cOpRight, CProductionToken& cOperator);
	bool EvaluateMathematical(CProductionToken& cOpLeft ,
		CProductionToken& cOpRight, CProductionToken& cOperator);
  bool EvaluateStringComparision(CProductionToken& cOpLeft,
    CProductionToken& cOpRight, CProductionToken& cOperator);
  bool EvaluateLogical(CProductionToken& cOpLeft,
    CProductionToken& cOpRight, CProductionToken& cOperator);  
	bool EvaluateStringConcatination(CProductionToken& cOpLeft ,
		CProductionToken& cOpRight, CProductionToken& cOperator);

  bool DeleteID(ProductionTokenList& ProdTokenList, size_t& nID);

  bool IsArray(const xtstring& cIdentifier);
  bool IsOperand(const xtstring& cIdentifier);
  bool IsLogical(const xtstring& cIdentifier);
  bool IsBitLogical(const xtstring& cIdentifier);
  bool IsExpression(const xtstring& cIdentifier);
  bool IsFunction(const xtstring& cIdentifier);
  bool IsError(const xtstring& cIdentifier);
  bool IsUnaer(const xtstring& cIdentifier, xtstring& cSymbolName);
  bool IsMember(const xtstring& cIdentifier);
  bool IsIdentifier(const xtstring& cIdentifier);
  bool IsComparison(const xtstring& cIdentifier);
  bool IsMathematical(const xtstring& cIdentifier);
	bool IsStringConcatination(CProductionToken& cOpLeft, CProductionToken& cOpRight,
														CProductionToken& cOperator);
	bool IsString(const xtstring& cIdentifier);
	bool IsDecimal(const xtstring& cIdentifier);
	bool IsFloat(const xtstring& cIdentifier);

// für Rückgabe testen
	bool IsTrue(xtstring cResult);
	bool IsFalse(xtstring cResult);

protected:

  bool m_bParseError; 
// die ursprüngliche Bedingung
	xtstring m_cCondition;  
// Referenz auf Python
	CXPubPythonWrapperInterface m_cPython;
	XPubPythonState*            m_pXPubPythonState;
// Error
  xtstring m_lastError;
// Referenz auf VariableValue
  CVariableValueCallBack* m_pVarValueCallBack;

  bool m_bConditionEvaluated;
};

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(AFX_EXPRESSION_H__D68B21F6_1D11_44D3_A5AB_1D16F4C895E8__INCLUDED_)
