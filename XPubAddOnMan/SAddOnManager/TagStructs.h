#if !defined(_TAGSTRUCTS_H_)
#define _TAGSTRUCTS_H_


#define ADD_TO_ICONINDEX_FOR_FREEWARE   4

// nAvailableForTargetFormat;  // PDF, RTF, InDesign
#define TF__PDF     0x0001  // PDF
#define TF__RTF     0x0002  // RTF
#define TF__IND     0x0004  // InDesign
#define TF_P    TF__PDF
#define TF_R    TF__RTF
#define TF_I    TF__IND
#define TF_PR   TF__PDF | TF__RTF
#define TF_PI   TF__PDF | TF__IND
#define TF_RI   TF__RTF | TF__IND
#define TF_PRI  TF__PDF | TF__RTF | TF__IND

struct TagHintsLists
{
  LPCTSTR lpszHintsList;
  int nIconIndex;         // 0 - kein Bild
  int nAvailableForTargetFormat;  // PDF, RTF, InDesign
  LPCTSTR lpszTooltip;
};
struct TagParameters
{
  LPCTSTR lpszParameter;
  bool bFreeWare;         // true - in FreeWare benutzen
  int nIconIndex;         // 0 - kein Bild
  bool bParOnly;          // bei true nur Parameter und kein "="
  bool bOnlyDefinedVals;  // bei true sind nur Werte von Liste akzeptiert
  int nCountOfFields;     // bei > 0 muss so viel Feldern in Wert vorhenden sein
	int nType;							// Type der Werte, 0=keine Prüfung, 1=Vaue, 2=String, 3=FileName
  TagHintsLists* hints;
  int nAvailableForTargetFormat;  // PDF, RTF, InDesign
  LPCTSTR lpszTooltip;
};
struct TagKeywords
{
  LPCTSTR lpszKeyword;
  bool bFreeWare;         // true - in FreeWare benutzen
  int nIconIndex;         // 0 - kein Bild
  TagParameters* par;     // wenn kein Keyword gesetzt ist, dann schließen wir Klammer zu e.g. "<bold>"
  int nAvailableForTargetFormat;  // PDF, RTF, InDesign
  LPCTSTR lpszTooltip;
};



#endif // _TAGSTRUCTS_H_

