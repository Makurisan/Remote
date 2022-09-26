#if !defined(_FORMATNUMBER_H_)
#define _FORMATNUMBER_H_

#include "ModelExpImp.h"
#include "../SSTLdef/STLdef.h"
#include "ModelDef.h"
#include "ModelInd.h"
#include <time.h>
#include <assert.h>



class CGS_DateTimeNumber;


class XPUBMODEL_EXPORTIMPORT CFormatNumber  
{
public:
	CFormatNumber(CGS_DateTimeNumber* pGSettings);

	virtual ~CFormatNumber();

	bool IsLeadingSign();

	bool IsLeadingZeroes();
	size_t  SizeLeadingZeroes();
	
	bool IsPrecision();
	size_t  SizePrecision();

	bool IsGroupSeparator();
	size_t  SizeGroupSeparator();

	bool IsPrecisionZeroes();
	bool IsEmpty();

	void FormatValue(double value, xtstring& strFormat);
	bool SetMask(const xtstring& sMask);

private:

	bool m_bLeadingSign;				// führendes + -
	bool m_bLeadingZeroes;			// führende O
	size_t  m_nLeadingZeroes;		// Feldbreite
	bool m_bPrecision;					// Nachkommastellen
	size_t  m_nPrecision;				// Anzahl der Nachkommastellen
	bool m_bPrecisionZeroes;		// Nachkommastellen mit 0 auf Genauigkeit auffüllen
	bool m_bGroupSeparator;			// Tausenderpunkte darstellen
	size_t m_nDigitsInGroup;		// Anzahl Zeichen in Tausendergruppe
	bool m_bEmpty;							// leer, wenn Null
	bool m_bShowResult;					// Ergebnis anzeigen, wenn 0

	xtstring m_sPrefix;					// e.g  "$"
	xtstring m_sSuffix;					// e.g. "###%"
	xtstring m_sMask;						// die Maske		

	xtstring m_sNegativeSign;
	xtstring m_sDecimalPoint;
	xtstring m_sGroupSeparator;

};

inline bool CFormatNumber::IsLeadingSign()
{ return m_bLeadingSign;};

inline bool CFormatNumber::IsLeadingZeroes()
{ return m_bLeadingZeroes;};

inline size_t CFormatNumber::SizeLeadingZeroes()
{ return m_nLeadingZeroes;};

inline bool CFormatNumber::IsPrecision()
{ return m_bPrecision;};

inline size_t CFormatNumber::SizePrecision()
{ return m_nPrecision;};

inline bool CFormatNumber::IsPrecisionZeroes()
{ return m_bPrecisionZeroes;};

inline bool CFormatNumber::IsGroupSeparator()
{ return m_bGroupSeparator;};

inline size_t CFormatNumber::SizeGroupSeparator()
{ return m_nDigitsInGroup;};

inline bool CFormatNumber::IsEmpty()
{ return m_bEmpty;};


#endif // !defined(_FORMATNUMBER_H_)


