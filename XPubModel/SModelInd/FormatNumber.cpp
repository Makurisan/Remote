//============================================================================

#include "FormatNumber.h"

#include "STools/GlobalSettings.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CFormatNumber::CFormatNumber(CGS_DateTimeNumber* pGSettings)
{
	m_bLeadingZeroes = false;
	m_nLeadingZeroes = 0;
	
	m_bPrecision = true;					// Nachkommastellen
	m_nPrecision = 2;							// Anzahl der Nachkommastellen
	m_bPrecisionZeroes = true;		// Nachkommastellen mit 0 auf Genauigkeit auffüllen
	m_bGroupSeparator = false;		// Tausenderpunkte nicht darstellen
	m_nDigitsInGroup = 3;					// Anzahl Zeichen in Tausendergruppe
	m_bEmpty = true;							// leer, wenn Null
	m_bLeadingSign = false;
	m_bShowResult = false;				// Anzeigen, auch wenn 0, dazu muss in Maske vor Dezimalzeichen eine "0" definiert sein

	m_sPrefix = _XT("");
	m_sSuffix = _XT("");

  if (!pGSettings)
  {
	  m_sNegativeSign = DEFAULT_NEGATIVESIGN;
	  m_sDecimalPoint = DEFAULT_DECIMALSYMBOL;
	  m_sGroupSeparator = DEFAULT_DIGITGROUPINGSYMBOL;
  }
  else
  {
	  m_sNegativeSign = DEFAULT_NEGATIVESIGN;
	  m_sDecimalPoint = pGSettings->GetDecimalSymbol();
	  m_sGroupSeparator = pGSettings->GetDigitGroupingSymbol();
  }
/*
// wir müssen noch die Definition aus XPublisher Eigenschaften auslesen
#ifdef WIN32
	// Get the system's negative sign
	if (::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SNEGATIVESIGN, NULL, 0))
		::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SNEGATIVESIGN, m_cNegativeSign, sizeof(m_cNegativeSign));

	// Get the system's decimal point	
	if (::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, NULL, 0))
		::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, m_cDecimalPoint, sizeof(m_cDecimalPoint));

	// Get the system's group separator
	if (::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, NULL, 0))
		::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, m_cGroupSeparator, sizeof(m_cGroupSeparator));

	char szValue[10];
	if (::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONGROUPING, szValue, 0))
	{
		::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONGROUPING, szValue, sizeof(szValue));
		m_nDigitsInGroup = atoi(szValue);
	}

#else
  #error "unexpected error"
#endif
*/
}

/////////////////////////////////////////////////////////////////////////////////
//
CFormatNumber::~CFormatNumber()
{

}

/////////////////////////////////////////////////////////////////////////////////
// "leer, wenn 0", wird über eine Null Maske definiert
//
// Masken:
//    ##.##0,00				; Tausender Punkte, feste Anzahl Nachkommastellen
//    ##.##0,00 			; wie oben Plus Eurozeichen
//   $##.##0,00
//  $+##.##0,00			  ; wie oben plus Vorzeichen auch bei positiven Werten
//         0					; auch 0 als Ergebnis wird ausgeben
//				 0,00				; bei Ergebnis 0 wird dieses ausgeben	
//
bool CFormatNumber::SetMask(const xtstring& sMask)
{

 // maske speichern
	m_sMask = sMask;

 // Tausend Seperator
  if (sMask.find(m_sGroupSeparator) != xtstring::npos)
		m_bGroupSeparator = true;

 // präfix
	size_t nPos;
	if((nPos = sMask.find(_XT("#"))) != xtstring::npos && nPos != 0)
	{
		m_sPrefix += sMask.substr(0, nPos); 
		m_sPrefix.TrimRight(_XT("+"));
	}

 // suffix  
	if((nPos = sMask.find_last_of(_XT("0"))) != xtstring::npos || (nPos = sMask.find_last_of(_XT("#"))) != xtstring::npos) 
	{
		if(nPos+1 < sMask.size())
	 		m_sSuffix += sMask.substr(nPos+1); 
	}

 // über System auslesen
	xtstring sHash = _XT("#") + m_sDecimalPoint;
	xtstring sZero = _XT("0") + m_sDecimalPoint;

 // Nachkommastellen mit 0 auf Genauigkeit auffüllen
	if(sMask.find(sZero) != xtstring::npos) 
		m_bPrecisionZeroes = true;

 // Anzahl der Nachkommastellen herausfinden
	m_nPrecision = 0;

	if( (nPos = m_sMask.find(sHash)) != xtstring::npos || (nPos = m_sMask.find(sZero)) != xtstring::npos)
	{
		size_t nLPos;		
		if((nLPos = m_sMask.find_last_of(_XT("0"))) != xtstring::npos || (nLPos = m_sMask.find_last_of(_XT("#"))) != xtstring::npos) 
			m_nPrecision = (int) nLPos - (nPos+1);

		if( sMask.find(sZero) != xtstring::npos )
			m_bShowResult=true;

	}
	else
	{
		if( sMask.find_last_of(_XT("0")) != xtstring::npos )
			m_bShowResult=true;
	}

 // führende Plus oder Minus
	if(sMask.find(_XT("+")) != xtstring::npos) // führendes + -
		m_bLeadingSign = true;

	return true;
}

void CFormatNumber::FormatValue(double value, xtstring& strFormatString)
{
	char strFormat[32] = _XT("%");
	char sOutputString[100];

	if(value == 0.0 && SizePrecision() == 0)
	{
		strFormatString = _XT("");
		return;
	}

	if(IsLeadingSign()) // + or -
	{
		if(value > 0.0)
			strcat(strFormat, _XT("+"));
	}

	if(IsLeadingZeroes() && SizeLeadingZeroes() > 0) // Breite vorgegeben mit Nullen
	{
		char strVkst[10];
		sprintf(strVkst, _XT("%d"), SizeLeadingZeroes());
		strcat(strFormat, _XT("0"));
		strcat(strFormat, strVkst);
	}

	if(IsPrecision())
	{
		char strNkst[10];
		if(IsPrecisionZeroes())
			sprintf(strNkst,_XT("0.%d"), SizePrecision());
		else
			sprintf(strNkst,_XT(".%d"), SizePrecision());

		strcat(strFormat, strNkst);
	}

	strcat(strFormat, _XT("lf"));
	sprintf(sOutputString, strFormat, value);
	
	strFormatString = sOutputString;
	
	if(!m_bShowResult && atof(sOutputString) == 0.0)
	{
		strFormatString = _XT("");
		return;
	}

	if(strFormatString.find(_XT(".")) != xtstring::npos && m_sDecimalPoint != _XT(".") )
		strFormatString.ReplaceAll(_XT("."), m_sDecimalPoint);

// Tausenderpunkte einfügen
	if(IsGroupSeparator() && strFormatString.size())
	{
		size_t nDot = strFormatString.find(m_sDecimalPoint);
		size_t nLen = nDot != xtstring::npos ? nDot : strFormatString.size();
		nLen = (strFormatString[0] == '-' || strFormatString[0] == '+' )?--nLen:nLen;
		if(nLen > SizeGroupSeparator())
		{
			int nStartPos = strFormatString[0] == '-' || strFormatString[0] == '+'?1:0;
			for (int iPos = nLen - (SizeGroupSeparator() + 1); iPos >= nStartPos; iPos -= SizeGroupSeparator())
				strFormatString.insert(iPos + 1, m_sGroupSeparator);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
// Nachbearbeitung

	if (m_sPrefix.size())
  {
    xtstring s;
		s = m_sPrefix;
    s += strFormatString;
    strFormatString = s;
  }

	if (m_sSuffix.size())
  {
		strFormatString = strFormatString;
    strFormatString += m_sSuffix;
  }

}

