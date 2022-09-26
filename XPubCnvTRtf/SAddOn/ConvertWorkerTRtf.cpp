#include <assert.h>

#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"

#ifdef __unix__
#else // __unix__
#include "windows.h"
#include "wingdi.h"
#include "Shlwapi.h"
#endif // __unix__

#include "SAddOnManager/XPubAddOnManager.h"

#include "SAddOnManager/ConvertWorker.h"
#include "SAddOn/ConvertWorkerFTag.h"

#include "ter.h"
#include "ter_cmd.h"

#include "ConvertWorkerTRtf.h"

#define GETPARAM1(tag, val1) 		\
	CConvertWorkerFromTag::GetParameterValues(cInTag, tag, val1, cParValue[1], cParValue[2], cParValue[3], 1)

#define GETPARAM(tag, val1) 		\
  CConvertWorkerFromTag::GetParameterValues(cInTag, tag, val1, cParValue[1], cParValue[2], cParValue[3], 0)

#if !defined(_LIB)
XPUBCNVTRTF_EXPORTIMPORT CConvertToAddOn* RegisterAddOnManager(CXPubAddOnManager* pManager);
XPUBCNVTRTF_EXPORTIMPORT bool UnregisterAddOnManager(CConvertToAddOn* pCnvAddOn);
XPUBCNVTRTF_EXPORTIMPORT int GetCnvToAddOnVersion();
XPUBCNVTRTF_EXPORTIMPORT CConvertWorkerTo* CreateConvertWorkerTo_(CConvertToAddOn* pCnvAddOn, HWND hWndParent);
XPUBCNVTRTF_EXPORTIMPORT CConvertWorkerTo* CreateConvertWorkerToClone_(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);
XPUBCNVTRTF_EXPORTIMPORT bool ReleaseConvertWorkerTo_(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);
#endif // !defined(_LIB)

#if !defined(_LIB)
CConvertToAddOn* RegisterAddOnManager(CXPubAddOnManager* pManager)
{
  CConvertToAddOn* pNew = new CConvertToRtfAddOn(pManager);
  if (!pNew)
    throw(ERR_NEW_PROBLEM);
  return pNew;
}

bool UnregisterAddOnManager(CConvertToAddOn* pCnvAddOn)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_UNREGISTER_ADDON_WITH_NULL_POINTER);
    return false;
  }

  pCnvAddOn->DestroyAndRemoveAllConvertWorkers();
  delete pCnvAddOn;
  pCnvAddOn = NULL;
  return true;
}

int GetCnvToAddOnVersion()
{
  return ConvertWorkerTRtf_Version;
}

CConvertWorkerTo* CreateConvertWorkerTo_(CConvertToAddOn* pCnvAddOn, HWND hWndParent)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerTo(hWndParent);
}

CConvertWorkerTo* CreateConvertWorkerToClone_(CConvertToAddOn* pCnvAddOn,
                                              CConvertWorkerTo* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKERCLONE_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerToClone(pConvertWorker);
}

bool ReleaseConvertWorkerTo_(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_RELEASEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->ReleaseConvertWorkerTo(pConvertWorker);
}
#endif // !defined(_LIB)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CConvertToRtfAddOn::CConvertToRtfAddOn(CXPubAddOnManager* pAddOnManager)
                   :CConvertToAddOn(pAddOnManager)
{
#ifdef WIN32
  ::TerSetInitTypeface((LPBYTE)"Arial");
#endif
}

CConvertToRtfAddOn::~CConvertToRtfAddOn()
{
}

CConvertWorkerTo* CConvertToRtfAddOn::CreateConvertWorkerTo(HWND hWndParent)
{
  CConvertWorkerTo* pWorker = new CConvertWorkerToRtf(this, hWndParent);
	if (!pWorker || !::IsWindow(((CConvertWorkerToRtf*)pWorker)->GetHWnd()) )
  {
    throw(ERR_ADDON_NEWWORKER_NOT_CREATED);
    return pWorker;
  }

  if (AddConvertWorkerTo(pWorker))
    return pWorker;

  delete pWorker;
  pWorker = NULL;
  throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);
  return pWorker;
}

CConvertWorkerTo* CConvertToRtfAddOn::CreateConvertWorkerToClone(CConvertWorkerTo* pConvertWorker)
{
  if (!pConvertWorker)
  {
    throw(ERR_ADDON_CREATEWORKERCLONE_FROM_NULL_POINTER);
    return NULL;
  }

  CConvertWorkerToRtf* pWorker = (CConvertWorkerToRtf*)((CConvertWorkerToRtf*)pConvertWorker)->Clone();
  if (AddConvertWorkerTo(pWorker))
    return pWorker;

  delete pWorker;
  pWorker = NULL;
  throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);
  return pWorker;
}

bool CConvertToRtfAddOn::ReleaseConvertWorkerTo(CConvertWorkerTo* pWorker)
{
  if (!pWorker)
  {
    throw(ERR_ADDON_RELEASEWORKER_FROM_NULL_POINTER);
    return NULL;
  }

  if (!RemoveConvertWorkerTo(pWorker))
    return false;

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CCanPageBreak::Set(HWND hWnd)
{ 
	nCurLine=0;nCurCol=0;
	BOOL bReturn = ::GetTerCursorPos(hWnd,&nCurLine,&nCurCol); 
	return bReturn;
}

BOOL CCanPageBreak::GetPage(HWND hWnd, INT& nPage)
{ 
	nPage = ::TerPageFromLine(hWnd, nCurLine);
	return nPage != -1;
}

BOOL CCanPageBreak::GetCurrentPage(HWND hWnd, INT& nPage)
{ 
	//	int nlocCurLine = ::TerGetParam(hWnd, TP_CUR_LINE);
	long nlocCurLine; int nlocCurCol=0;

	BOOL bReturn = ::GetTerCursorPos(hWnd,&nlocCurLine,&nlocCurCol); 
	nPage = ::TerPageFromLine(hWnd, nlocCurLine);
	return nPage != -1;
}

BOOL CCanPageBreak::IsSet()
{
	return nCurLine > 0;
}

void CCanPageBreak::Clear()
{
	nCurLine=0;nCurCol=0;
}

void CCanPageBreak::CheckPageBreak(HWND hWnd)
{
	if(IsSet())
	{
		// sonst werden die Seitenangaben nicht korrekt gemeldet
		::TerRepaginate(hWnd, FALSE);
		INT nPage, nBePage;
		if(GetCurrentPage(hWnd, nBePage) && GetPage(hWnd, nPage))
		{
			if(nPage+1 == nBePage)
			{
				PageBreak(hWnd);
				Clear();
			}
			else
				if(nPage != nBePage) // unnötige Prüfungen abschalten
					Clear();
		}
	}
}

void CCanPageBreak::PageBreak(HWND hWnd)
{
	// aktuelle absolute Position merken
	int CurCol=-1; long CurLine;
	BOOL bRetf = ::GetTerCursorPos(hWnd, &CurLine, &CurCol); 

	// auf Kannumbruch setzen
	::SetTerCursorPos(hWnd, nCurLine, nCurCol, FALSE);

// bis Lösung von TE kommt MAK:
  int nID = ::TerGetTableId(hWnd, -1);
  if(nID != -1)
  {
//    BYTE text[MAX_WIDTH];
//    ::TerGetLine(hWnd,nCurLine-1,text,NULL); 
  }

	bRetf = ::TerPageBreak(hWnd, FALSE);

  INT TotalPages, CurPage;
  ::TerGetPageCount(hWnd, &TotalPages, &CurPage);

  xtstring sBmk;
  sBmk.Format("$$CANPAGEBREAK_%d", CurPage);
  ::TerInsertBookmark(hWnd, -1, 0, (LPBYTE)sBmk.c_str());

	// auf vorige Position setzen, +1 für eingesetzten Pagebreak
	::SetTerCursorPos(hWnd, CurLine+1, CurCol, FALSE);

	Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CConvertWorkerToRtf::CConvertWorkerToRtf(CConvertToRtfAddOn* pAddOn, HWND hWndParent)
                    :CConvertWorkerTo(tDataFormatST_Rtf, hWndParent)
{

  m_pAddOn = pAddOn;

  m_nTableID = -1;
  m_nTableRow = -1;
  m_nTableCol = -1;

  //aus Ter_init
	//DefCellMargin=CELL_MARGIN;  // default cell margin
	//FrameDistFromMargin=FRAME_DIST_FROM_MARGIN;
	
  m_nActCharSet = DEFAULT_CHARSET;
  m_sFontName = m_sDefaults.DefFontName;
  m_sCodePage = m_sDefaults.DefCodePage;
  m_nFontSize = m_sDefaults.DefFontSize;

	LoadTerControl();

	m_hWndRTF = ::CreateWindow( TER_CLASS, "",
		WS_CHILD|WS_BORDER|TER_WORD_WRAP|TER_PAGE_MODE|TER_BORDER_MARGIN, //WS_VISIBLE|
    -2000,
    -2000,
    100,
    100,
    ::GetDesktopWindow(),
    NULL,
    NULL,
    NULL
	);

	SetDefTabulator(m_sDefaults.DefTabWidth, 0);

	::TerSetPrinter(GetHWnd(),(LPBYTE)"HP LaserJet 5P",NULL,NULL,NULL);

	::TerSetSectOrient(GetHWnd(), m_sDefaults.PaperOrient, FALSE);
	SetPaperSize(0, m_sDefaults.PageWidth, m_sDefaults.PageHeight);

	SetHeaderFooterPos(TAG_TEXT_HEADPOS, m_sDefaults.HdrMargin);
	SetHeaderFooterPos(TAG_TEXT_FOOTERPOS, m_sDefaults.FtrMargin);
	Margins(m_sDefaults.LeftMargin, m_sDefaults.RightMargin,
					m_sDefaults.TopMargin, m_sDefaults.BotMargin);

	::SetTerDefaultFont(GetHWnd(), (LPBYTE)m_sFontName.c_str(), m_nFontSize, 0, 0, FALSE); 
	::ClearAllTabs(GetHWnd(), FALSE);
	::TerSetFlags4(GetHWnd(), TRUE, TFLAG4_SAVE_BMP_AS_PNG);
	::TerSetDefLang(GetHWnd(), m_sDefaults.DefLang);

	//::TerSetNewFrameDim(GetHWnd(),x,y,width,height,TRUE);

	m_cBookList.clear(); // Liste der Lesezeichen

}

CConvertWorkerToRtf::~CConvertWorkerToRtf()
{
	if(::IsWindow(m_hWndRTF))
		::DestroyWindow(m_hWndRTF);
}

CConvertWorkerTo* CConvertWorkerToRtf::Clone()
{
  CConvertWorkerToRtf* pNew = new CConvertWorkerToRtf(m_pAddOn, m_hWndParent);
  if (!pNew)
  {
    throw(ERR_ADDON_NEWWORKER_FOR_CLONE_NOT_CREATED);
    return NULL;
  }

  return pNew;
}

bool CConvertWorkerToRtf::IsDataFormatAvailable(TDataFormatForStoneAndTemplate nOutputDataFormat)
{
  return (nOutputDataFormat == tDataFormatST_Rtf);
}

long CConvertWorkerToRtf::MMToTwips(float nMM)
{
//	return ((long)((((float)nMM)*1440*10)/254));
// besser
	long nRet = 0;
	if ( nMM <= ( LONG_MAX / 567 ) && nMM >= ( LONG_MIN / 567 ) )
		nRet = nMM * 567 / 10;
	return nRet;
}

bool CConvertWorkerToRtf::PositionCursor(const xtstring& sXPubConst, TWorkPaperCopyType eWorkPaperCopyType)
{

	// Einfügemarke an richtige Position setzen
	if ( eWorkPaperCopyType == tCopyType_ReplaceSingle
		|| eWorkPaperCopyType == tCopyType_ReplaceMultiple
		|| eWorkPaperCopyType == tCopyType_InsertSingle 
		|| eWorkPaperCopyType == tCopyType_InsertMultiple
		)
	{
		LONG lBeginPos=0, lEndPos=0;
		LONG FindPos=0;

// When the SRCH_SEARCH flag is specified, the return value indicates the absolute character position 
		FindPos = ::TerSearchReplace2(GetHWnd(), (LPBYTE)sXPubConst.c_str(), (LPBYTE)"", SRCH_SEARCH|
																	SRCH_SKIP_HIDDEN_TEXT|SRCH_NO_REPLACE_PROT_TEXT, FindPos, lEndPos);
		if(FindPos!=-1)
		{ 
			if( (eWorkPaperCopyType == tCopyType_ReplaceSingle ||
			eWorkPaperCopyType == tCopyType_ReplaceMultiple) )
			{ 
//				int CurCol=-1;long AbsPos;
//				::GetTerCursorPos(GetHWnd(),&FindPos,&CurCol); 

			// SRCH_NO_REPLACE_PROT_TEXT: Do not replace protected text.
// Achtung!
//   BOOL ReplaceTextString(PTERWND w,LPBYTE
//   Diese Funktion löscht ein Zeichen mehr als erwartet
//   Wenn wir nun lEndPos-1 machen, dann geht es richtig

				lEndPos=sXPubConst.size() + FindPos;
				BOOL bRetf = ::TerSearchReplace2(GetHWnd(), (LPBYTE)sXPubConst.c_str(), (LPBYTE)"", SRCH_REPLACE|
																	SRCH_SKIP_HIDDEN_TEXT|SRCH_NO_REPLACE_PROT_TEXT, FindPos, lEndPos-1);
			
				::SetTerCursorPos(GetHWnd(), FindPos, -1, FALSE);

//				long newAbsPos;
//				::GetTerCursorPos(GetHWnd(),&newAbsPos,&CurCol); 
//				if(AbsPos == newAbsPos && bRetf)
//					::SetTerCursorPos(GetHWnd(), AbsPos+1, -1, FALSE);
			}
			else
			{ 
				if(FindPos != -1)
					::SetTerCursorPos(GetHWnd(), FindPos, -1, FALSE);

//					GetControl().SetSel(FindPos, FindPos);
//					DeselectTerText(GetHWnd(), FALSE);
			}
		}
	}
	int nSection = -1;
	if ( eWorkPaperCopyType == tCopyType_InsertFront)
		::TerPosBodyText(GetHWnd(), nSection, POS_BEG, FALSE);

	if ( eWorkPaperCopyType == tCopyType_InsertBack)
		::TerPosBodyText(GetHWnd(), nSection, POS_END, FALSE);

// do nothing
	// if ( eWorkPaperCopyType == tCopyType_ActualPosition)

	return true;
}


bool CConvertWorkerToRtf::PrintDataStream(const xtstring& squeuename, bool bSelectOrPrint)
{
	bool bReturn = TRUE;

	if(bSelectOrPrint)
	{
//		bReturn = ::TerPrinterOptions(GetHWnd());
		//::SendMessage(GetHWnd(), WM_COMMAND, ID_PRINT_OPTIONS, 0l);
	}
	else
	{
		::TerDocName(GetHWnd(), FALSE, (LPBYTE)squeuename.c_str());
//special behaviour are implemented
		::TerPrint2(GetHWnd(),FALSE,-1,-1,0,TRUE);
		//::SendMessage(GetHWnd(), WM_COMMAND, ID_PRINT, 0l);
	}
	return bReturn;
}

bool CConvertWorkerToRtf::SaveDataStream(const xtstring& sfilename)
{
	BOOL bReturn = TRUE;

 // create file
  FILE* fp = NULL;

	fp = fopen((const char*)sfilename.c_str(), _XT("wb"));
  if(!fp)
 		return FALSE;
  else
  {
		xtstring sCtrlText;
		GetControlText(sCtrlText, SAVE_RTF);
		fwrite(sCtrlText.c_str(), (size_t)sCtrlText.size(), 1, fp);
		fclose(fp);
  }
 	return TRUE;

}

bool CConvertWorkerToRtf::GetControlText(xtstring& sCtrlText, int nFormat)
{
	::TerSetOutputFormat(GetHWnd(), nFormat);

	LONG size = 0l;
	HANDLE hHandle = ::GetTerBuffer(GetHWnd(), &size);
	if(hHandle)
	{
		LPBYTE pText=(LPBYTE)GlobalLock(hHandle);
		// Inhalt aus Control an aktuelle Position einfügen
		sCtrlText = (const char*)pText;

		::GlobalUnlock(hHandle);
		::GlobalFree(hHandle);
	}
	return FALSE;
}

bool CConvertWorkerToRtf::GetDataStream(TDataFormatForStoneAndTemplate nOutputDataFormat, char *pOutput, int &nSize)
{
	SetBookmarkFromList(); //MAK: mehrmaliges Aufrufen macht nichts

	xtstring sHeCtrlText;
	GetControlText(sHeCtrlText, SAVE_RTF);
	if(nSize == 0)
	{
		nSize = sHeCtrlText.size();
		return true;
	}

	if(nSize > sHeCtrlText.size())
	{
		memcpy(pOutput, sHeCtrlText.c_str(), sHeCtrlText.size());
		nSize = sHeCtrlText.size();
		return true;
	}
  return false;
}

xtstring CConvertWorkerToRtf::GetDataStream(TDataFormatForStoneAndTemplate nOutputDataFormat)
{
	SetBookmarkFromList(); //MAK: mehrmaliges Aufrufen macht nichts

	xtstring sHeCtrlText;
  GetControlText(sHeCtrlText, SAVE_RTF);
  return sHeCtrlText;
}

int CConvertWorkerToRtf::GetCurSecId()
{
	int nCurLine = ::TerGetParam(GetHWnd(), TP_CUR_LINE);
	int nSecId = ::TerGetSectFromLine(GetHWnd(), nCurLine);	
	return nSecId;
}

int CConvertWorkerToRtf::GetSectionNumber()
{
	int nSecId = GetCurSecId();
	return ::TerGetPageSect(GetHWnd(), nSecId);
}

inline int CConvertWorkerToRtf::GetTableSelection(int wRow, int wCol)
{
 // -1 auflösen
	wRow = _GET_ROW(wRow);
	wCol = _GET_COL(wCol);

  if (wRow > 0 && wCol > 0)
    return SEL_CELLS;

	INT nColNo, nRowNo, nTableNo;

	bool bTablePos = ::TerGetTablePos(GetHWnd(), &nTableNo, &nRowNo, &nColNo) == TRUE;

	if (wRow > 0 && wCol == 0)
	{  
  // wir müssen sicherstellen, dass der Cursor auch in wRow ist
		if(nRowNo != wRow || !bTablePos)
			::TerPosTable3(GetHWnd(), GetTableID(), wRow-1, 0, TAG_POS_END, FALSE);
		return SEL_ROWS;
	}
	if (wRow == 0 && wCol > 0)
	{  
	// wir müssen sicherstellen, dass der Cursor auch in wCol ist
		if(nColNo != wCol || !bTablePos)
			::TerPosTable3(GetHWnd(), GetTableID(), 0, wCol-1, TAG_POS_END, FALSE);
		return SEL_COLS;
	}

	if (wRow == 0 && wCol == 0)
    return SEL_ALL;

  return SEL_CELLS;
}

bool CConvertWorkerToRtf::InsertTextFromMemory(const xtstring& sInput, int nFormat/* = CNV_TEXTFORMAT_TEXT*/)
{
  assert(GetHWnd());

  bool bResult;
	INT id = ::TerGetEffectiveFont(GetHWnd());

// ergänzt um UTF8 Unterstützung
	if(isLegalUTF8String((UTF8*)sInput.c_str(), (int)sInput.size()))
	{
		xwstring sUni;
		sUni.ConvertAndAssign(sInput, UTF8_CODEPAGE);
		bResult = FALSE != ::TerInsertTextU(GetHWnd(), (WORD*)(LPCWSTR)sUni.c_str(),  id,  -1, FALSE);
	}
	else
		bResult = FALSE != ::TerInsertText(GetHWnd(), (LPBYTE)(LPCTSTR)sInput.c_str(),  id,  -1, FALSE);
  return bResult;
}

bool CConvertWorkerToRtf::InsertParagraph()
{
  xtstring s;
  s = PARA_CHAR;
  InsertTextFromMemory(s, CNV_TEXTFORMAT_ANSI);
  //TRACE("InsertTextFromMemory(s, CNV_TEXTFORMAT_ANSI);\n");

#ifdef _DEBUG
  int ParaId, CellId, ParaFID, x, y, height;
  DWORD dwflags; UINT InfoFlags;
  ::TerGetLineInfo(GetHWnd(), -1, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);
#endif // _DEBUG
	
// defaults at Paragraphcontrol
	ParagraphPageBreak(0); 
	SetParagraphControl(0,0,0);

	return true;
}

static int GetCharSet(const xtstring& sCodePage)
{
  CHARSETINFO chsi;

  DWORD dwCodePage = atol(sCodePage.c_str());
  if (::TranslateCharsetInfo((DWORD*)dwCodePage, &chsi, TCI_SRCCODEPAGE))
  {
    return chsi.ciCharset;
  }
  return DEFAULT_CHARSET;
}

bool CConvertWorkerToRtf::SetDefaults()
{
  assert(GetHWnd());
  m_nActCharSet = GetCharSet(m_sCodePage);
  //SetFont(m_sFontName, m_nFontSize);

  return true;
}

bool CConvertWorkerToRtf::InsertLine()
{
  xtstring s;
  s = LINE_CHAR;
	bool bReturn = InsertTextFromMemory(s, CNV_TEXTFORMAT_ANSI);
	return bReturn;
}

bool CConvertWorkerToRtf::SetParagraphControl(const int nKeep, const int nKeepNext, const int nWidowOrphon)
{

	BOOL bResult = TRUE;
	
	if(nWidowOrphon != -1)
		bResult &= ::TerSetPflags(GetHWnd(), PFLAG_WIDOW, (BOOL)nWidowOrphon, FALSE);
	
	if(nKeep != -1)
		bResult &= ::SetTerParaFmt(GetHWnd(), PARA_KEEP, (BOOL)nKeep, FALSE);

	if(nKeepNext != -1)
 		bResult &= ::SetTerParaFmt(GetHWnd(), PARA_KEEP_NEXT, (BOOL)nKeepNext, FALSE);

  return (bResult != FALSE);

}

bool CConvertWorkerToRtf::SetParagraphSpacing(int nSpaceBefore, int nSpaceAfter, int nSpaceBetween, int nLineSpacing)
{
  assert(GetHWnd());

  long LineNo;

	LineNo = ::TerGetParam(GetHWnd(), TP_CUR_LINE); // get the paragraph information about the style being currently edited

	int ParaId, CellId, ParaFID, x, y, height;
	DWORD dwflags; UINT InfoFlags;
	::TerGetLineInfo(GetHWnd(), LineNo, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);

  BOOL bResult;

	nLineSpacing = 0; // Set to 0 for default.
	bResult = ::TerSetParaSpacing2(GetHWnd(), nSpaceBefore, nSpaceAfter, nSpaceBetween*2, nLineSpacing, FALSE);

  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetParagraphIndents(int nLeft, int nRight, int nFirstIndent)
{
  assert(GetHWnd());

  BOOL bResult;
	bResult = ::TerSetParaIndent(GetHWnd(), nLeft, nRight, nFirstIndent, FALSE);

  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetDefTabulator(int nNewWidth, int nTabPos)
{
	BOOL bResult;
	bResult = ::TerSetDefTabWidth(GetHWnd(), nNewWidth, FALSE);
		
	return (bResult != FALSE);
}

bool CConvertWorkerToRtf::ClearTabulators()
{
	::ClearAllTabs(GetHWnd(),FALSE);
  return true;
}

bool CConvertWorkerToRtf::InsertTabulator(int nTabFlag, INT nTabPos, const xtstring& sTabDecChar, const xtstring& sTabLeader)
{
  int TabType = TAB_LEFT;
  BOOL bResult = TRUE;

  switch(nTabFlag)
  {
  case TAG_TABULATOR_LEFT: // The tab position is at the left side of text.
    TabType = TAB_LEFT;
    break;

  case TAG_TABULATOR_RIGHT: // The tab position is at the right side of text.
    TabType = TAB_RIGHT;
    break;

  case TAG_TABULATOR_CENTER: // The text is centered on the tab position.
    TabType = TAB_CENTER;
    break;

  case TAG_TABULATOR_DECIMAL: // The text is centered on the tab position.
    TabType = TAB_DECIMAL;
    break;

  case TAG_TABULATOR_CLEARALL: // Clear all Positions
    bResult = ::ClearAllTabs(GetHWnd(), FALSE);
    return (bResult != FALSE);	

  case TAG_TABULATOR_DEFAULT: // the tab has no position
    break;
  }

  bResult = ::TerSetTab(GetHWnd(), TabType, nTabPos, TAB_NONE, FALSE); 
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::GetActualFont(xtstring& sFontName, int& nFontSize, int& nStyle)
{
	BOOL bResult = TRUE;

	BYTE sTypeFace[255];
	int id =  ::TerGetEffectiveFont(GetHWnd());
	bResult = ::GetFontInfo(GetHWnd(), id, (LPBYTE)sTypeFace, &nFontSize, (UINT*)&nStyle);
	sFontName = (LPCSTR)sTypeFace;

	return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetFont(const xtstring& sFontName, int nFontSize, int nCodePage)
{

  ::TerSetCharSet(GetHWnd(), m_nActCharSet);
  ::SetTerFont(GetHWnd(), (LPBYTE)(LPCSTR)sFontName.c_str(), FALSE);
  
  if(nFontSize != -1)
    ::SetTerPointSize(GetHWnd(), nFontSize, FALSE);

  return true;
}

bool CConvertWorkerToRtf::SetFontName(const xtstring& sFontName)
{
  return SetFont(sFontName, -1, -1);
}

bool CConvertWorkerToRtf::SetFontNameDefault(const xtstring& sFontName)
{
  m_sFontName = sFontName;
  return true;
}

bool CConvertWorkerToRtf::SetFontSize(int nFontSize)
{
	::SetTerPointSize(GetHWnd(), nFontSize, FALSE);
  return true;
}

bool CConvertWorkerToRtf::SetFontSizeDefault(int nFontSize)
{
  m_nFontSize = nFontSize;
  return true;
}

bool CConvertWorkerToRtf::SetCodePageDefault(const xtstring& sCodePage)
{
  m_sCodePage = sCodePage;
  return true;
}

bool CConvertWorkerToRtf::SetCodePage(const xtstring& sCodePage)
{
  m_nActCharSet = GetCharSet(sCodePage);
  return true;
}

bool CConvertWorkerToRtf::InsertTab()
{
  xtstring s;
  s = _XT('\x09');
	InsertTextFromMemory(s, CNV_TEXTFORMAT_ANSI);
	return true;
}

bool CConvertWorkerToRtf::InsertImage(const xtstring& id, const xtstring& sName, const xtstring& cPath,  float nWidht, float nHeight,
																			int nZoom, bool bLinked/*=true*/, bool bShrink/*=false*/, int nFitting/*=0*/)
{
  if (cPath.size() && IsPathFile(cPath.c_str()))
  {
// Positionsrahmen
	// - Wenn wir in Positionsrahmen sind, dann wird durch ein großes Bild der Positionsrahmen vergrößert.
	// - Ist das Bild kleiner als der Positionsrahmen wird der Positionsrahmen verkleinert.
	// --> Wir lösen das, indem der Positionsrahmen nachkorrigiert wird.

		int ParaId, CellId, ParaFID, x, y, height, width;
		DWORD dwflags; UINT InfoFlags;
		::TerGetLineInfo(GetHWnd(), -1, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);
		if(ParaFID > 0)
			::TerGetFrameSize(GetHWnd(), ParaFID, &x, &y, &width, &height);

		bool bInsert=FALSE;
    int id = ::TerInsertPictureFile(GetHWnd(), (LPBYTE)(LPCSTR)cPath.c_str(), TRUE, ALIGN_BOT, bInsert);

		UINT style;
		INT align, aux;
		RECT rect;
  
		BOOL bReturn = true;
	// Größe auslesen	
		bReturn = ::TerGetPictInfo(GetHWnd(), id, &style, &rect, &align, &aux);

		if(nZoom != -1)
		{
			nWidht = (float)Width(rect)/(float)100*(float)nZoom;
			nHeight = (float)Height(rect)/(float)100*(float)nZoom;
			::TerSetPictSize(GetHWnd(), id, nWidht, nHeight);
		}
		else
		{
			if( nHeight == -1)
				nHeight = nWidht / ((float)Width(rect)/(float)Height(rect));

			if( nWidht == -1)
				nWidht = nHeight / ((float)Height(rect)/(float)Width(rect));
		
			::TerSetPictSizeTwips(GetHWnd(), id, nWidht, nHeight);
		}
  // Bild einsetzen
		::TerInsertObjectId(GetHWnd(), id, TRUE);

		if(id)
		{
		// Einfügemarke "absolut" hinter das Bild setzen
			int CurCol=-1;long CurLine;
			::GetTerCursorPos(GetHWnd(),&CurLine,&CurCol); 
			::SetTerCursorPos(GetHWnd(), ++CurLine, CurCol, FALSE);
		}

// Positionsrahmen nachkorrigieren
		if(ParaFID > 0)
    { // TerMoveParaFrame
      // The y position is relative to the top of the page when the page header/footer is visible.
      // Otherwise it is relative to the top margin of the page.
      // The x location is relative to the left margin of the page.
      int left,right,top,bottom,header,footer;
      if(::TerGetMarginEx(GetHWnd(), SECT_CUR, &left,&right,&top,&bottom,&header,&footer))
      {// M
        ::TerMoveParaFrame3(GetHWnd(), ParaFID, x-left, y-top, width, height, 1);
      }
    }

		if(bShrink)
			unlink(cPath.c_str());

    return (bReturn != FALSE);
  }
  return false;
}

bool CConvertWorkerToRtf::SetUnderLineColor(_ColorDef color)
{
  BOOL bReturn = FALSE;
	
  bReturn = ::TerSetUlineColor(GetHWnd(), FALSE, color, FALSE);
	return (bReturn != FALSE);
}

bool CConvertWorkerToRtf::SetFontAttribute(int nAttrib)
{
  BOOL bReturn = FALSE;
  switch (nAttrib)
  {
  case TAG_TEXT_BOLD:
    bReturn = ::SetTerCharStyle(GetHWnd(), BOLD, TRUE, FALSE);
    break;
  case TAG_TEXT_BOLD_END:
    bReturn = ::SetTerCharStyle(GetHWnd(), BOLD, FALSE, FALSE);
    break;
  case TAG_TEXT_ITALIC: 
    bReturn = ::SetTerCharStyle(GetHWnd(), ITALIC, TRUE, FALSE);
    break;
  case TAG_TEXT_ITALIC_END:
    bReturn = ::SetTerCharStyle(GetHWnd(), ITALIC, FALSE, FALSE);
    break;
  case TAG_TEXT_UNDERLINE:		
    bReturn = ::SetTerCharStyle(GetHWnd(), ULINE, TRUE, FALSE);
    break;
  case TAG_TEXT_UNDERLINE_END:		
    bReturn = ::SetTerCharStyle(GetHWnd(), ULINE, FALSE, FALSE);
    break;
  case TAG_TEXT_UL_DOUBLE:		
    bReturn = ::SetTerCharStyle(GetHWnd(), ULINED, TRUE, FALSE);
    break;
  case TAG_TEXT_UL_DOUBLE_END:		
    bReturn = ::SetTerCharStyle(GetHWnd(), ULINED, FALSE, FALSE);
    break;
	case TAG_TEXT_STRIKE:		
		bReturn = ::SetTerCharStyle(GetHWnd(), STRIKE, TRUE, FALSE);
		break;
	case TAG_TEXT_STRIKE_END:		
		bReturn = ::SetTerCharStyle(GetHWnd(), STRIKE, FALSE, FALSE);
		break;

  default:
    bReturn = ::SetTerCharStyle(GetHWnd(), ULINED|ULINE|ITALIC|BOLD, FALSE, FALSE);
    break;
  }		
  return (bReturn != FALSE);
}

bool CConvertWorkerToRtf::SetTextColor(_ColorDef color)
{
  BOOL bResult = ::SetTerColor(GetHWnd(), color, FALSE);
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetHeaderFooterPos(int nCase, int nPosition)
{
  assert(GetHWnd());

  BOOL bResult = FALSE;
  int nSecId = GetCurSecId();

  switch(nCase)
  {
  case TAG_TEXT_HEADPOS: // The distance of the header text from the top of the page
    bResult = ::TerSetMarginEx(GetHWnd(), nSecId, -1, -1, -1, -1, nPosition, -1, FALSE);
    break;
  case TAG_TEXT_FOOTERPOS: // The distance of the footer text from the bottom of the page
    bResult = ::TerSetMarginEx(GetHWnd(), nSecId, -1, -1, -1, -1, -1, nPosition, FALSE);
    break;
  default:
    assert(0);
    break;
  }		
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SwitchHeaderFooter(int nCase, int nPos/*=CURSOR_END*/)
{
  assert(GetHWnd());
  INT Pos = nPos == TAG_CURSOR_HOME ? POS_BEG : POS_END;

  int nSecNbr = GetSectionNumber();

  switch(nCase)
  {
  case TAG_TEXT_HEADPOS:
    //::TerPosHdrFtrEx(hWnd,section,HdrFtrId,pos,repaint)
    //TRACE("::TerPosHdrFtr (GetHWnd(), %ld, TRUE, %ld, FALSE);\n", nSecNbr, Pos);
    return (::TerPosHdrFtr(GetHWnd(), nSecNbr, TRUE, Pos, FALSE) != FALSE);
    break;
  case TAG_TEXT_FOOTERPOS: 
    //TRACE("::TerPosHdrFtr (GetHWnd(), %ld,  FALSE, %ld, FALSE);\n", nSecNbr, Pos);
    return (::TerPosHdrFtr(GetHWnd(), nSecNbr,  FALSE, Pos, FALSE) != FALSE);
    break;
  case TAG_TEXT_BODYPOS: 
	 // wenn wir einen Sectionbreak einfügen, dann dürfen wir nicht im Editmodus bei
	 // Header/Footer sein
		ToggleEditHdrFtr();
   // den Cursor in der aktuellen Sektion ans Ende setzen 
		return SetCursorPosition(TAG_CURSOR_END, 0);
    break;
  default:
    assert(0);
    break;
  }		
  return false;
}

bool CConvertWorkerToRtf::ToggleEditHdrFtr()
{
	LRESULT lResult;
	lResult = ::SendMessage(GetHWnd(), WM_COMMAND, ID_EDIT_HDR_FTR, 0l);
	return (lResult == 1);
}

bool CConvertWorkerToRtf::TextAlignment(int nAlign)
{
  BOOL bReturn = FALSE;
  switch(nAlign)
  {
  case TAG_TEXT_TEXTZ: // Set centered paragraphs.
    bReturn = ::SetTerParaFmt(GetHWnd(), CENTER, TRUE, FALSE);
    break;
  case TAG_TEXT_TEXTL: // Set left-aligned paragraphs.
    bReturn = ::SetTerParaFmt(GetHWnd(), LEFT, TRUE, FALSE);
    break;
  case TAG_TEXT_TEXTR: // Set right-aligned paragraphs.
    bReturn = ::SetTerParaFmt(GetHWnd(), RIGHT_JUSTIFY, TRUE, FALSE);
    break;
  case TAG_TEXT_TEXTB: // Set to block formatted paragraphs 
    bReturn = ::SetTerParaFmt(GetHWnd(), JUSTIFY, TRUE, FALSE);
    break;
  }		
  return (bReturn != FALSE);
}

bool CConvertWorkerToRtf::SetCursorPosition(int nCase, int nSection /*= 0*/)
{
  assert(GetHWnd());

  BOOL bResult = FALSE;
  int nSecNbr = GetSectionNumber(); // current section number

//  TerPosBodyText berücksichtigt Header und Footer

  switch (nCase)
  {
  case TAG_CURSOR_HOME:
    bResult = ::TerPosBodyText (GetHWnd(), nSecNbr, POS_BEG, FALSE);
    //TRACE("::TerPosBodyText (GetHWnd(), %ld, POS_BEG, FALSE);\n", nSecNbr);
    break;
  case TAG_CURSOR_END: 
    bResult = ::TerPosBodyText (GetHWnd(), nSecNbr, POS_END, FALSE);
    //TRACE("::TerPosBodyText (GetHWnd(), %ld, POS_END, FALSE);\n", nSecNbr);
    break;
  default:
    assert(0);
    break;
  }	

  // Mit dem folgendem code wird geprüft, ob wir auf einem Objekt oder im Haupttext stehen
  // wir iterieren bis ParaFID auf 0 ist, dann handelt es sich um Text LFLAG_
  INT ParaId, CellId, ParaFID, x, y, height;
  DWORD dwflags; UINT InfoFlags;
  bResult &= ::TerGetLineInfo(GetHWnd(), -1, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);
	
  int nCurLine = ::TerGetParam(GetHWnd(), TP_CUR_LINE);
  int nLines = ::TerGetParam(GetHWnd(), TP_TOTAL_LINES);
  int nCurSect = ::TerGetSectFromLine(GetHWnd(), nCurLine);

  // wir müssen in den Text positionieren, wenn erste Zeile Positionsrahmen
  if (ParaFID != 0 && nCase == TAG_CURSOR_HOME)
  {
    int nActCurSect = nCurSect;
    while(nCurSect == nActCurSect && nActCurSect != -1 && ParaFID != 0 && nCurLine < nLines)
    {
      nActCurSect = ::TerGetSectFromLine(GetHWnd(), ++nCurLine);
      ::TerGetLineInfo(GetHWnd(), -1, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);
    }
  }
  if (ParaFID != 0 && nCase == TAG_CURSOR_END)
  {
    nCurLine;
  }
  return (bResult != FALSE);
}

// Wenn man in Seitenrand mit der Linie möchte, dann einfach negative Werte bei x,y angeben
bool CConvertWorkerToRtf::Line(int nX1, int nY1, int nX2, int nY2, int nWidth, _ColorDef LineColor)
{
	BOOL bResult = FALSE;
	assert(GetHWnd());
	int nFrameID = ::TerInsertLineObject(GetHWnd(), nX1, nY1, nX2, nY2);
	if(nFrameID != 0)
	{		
		int LineType=DOB_LINE_NONE;
		BOOL FillSolid=FALSE;			// TRUE to fill the background, FALSE to leave the text box transparent
		int ZOrder = -1; //-1 bringt linie hinter anderen Objekten
		bResult = ::TerSetObjectAttribEx(GetHWnd(), nFrameID, LineType, nWidth, LineColor, FALSE, 0, ZOrder );
	}
	return (bResult != FALSE);
}

bool CConvertWorkerToRtf::InsertTextFrameColumns(const xtstring& sName, const int nColumns, const double nWidth, const double nGutter)
{
  return false;
}

int CConvertWorkerToRtf::CreateTextFrame(const xtstring& id, const xtstring& sName, const xtstring& sJoin, const xtstring& sLibrary, int nLeft, int nTop, int nWidth, int nHeight, int nVertAlign, int nHorzAlign, 
     int nType/*=POSFRAME_PAGE*/, int LineThickness/*=-1*/, _ColorDef LineColor/*-1*/, _ColorDef FillColor/*-1*/,
     int nWrap/*=0*/, int nTextDirection/*=0*/)
{
  assert(GetHWnd());
// DOB_TEXT_BOX: wenn wir die Textbox nehmen, dann kommt bei RTF das shape unter WordXP falsch an
	int nBase=BASE_PAGE;

	if(nType == TAG_TEXTFRAME_PAGE)
		nBase=BASE_PAGE;
	if(nType == TAG_TEXTFRAME_MARG)
		nBase=BASE_MARG;
		
  BOOL bResult = FALSE;
  int nFrameID = ::TerInsertDrawObject(GetHWnd(), DOB_TEXT_BOX, nLeft, nTop, nWidth, nHeight, nBase);
	if(nFrameID != 0) 
	{		
		int LineType=DOB_LINE_NONE;
		BOOL FillSolid=FALSE;			// TRUE to fill the background, FALSE to leave the text box transparent
		
		if(FillColor != -1) 
			FillSolid=TRUE;	
		
		if(LineThickness <= 0)
			LineThickness = 0;
		else
		{
			LineType = DOB_LINE_SOLID;
		}
   // bei -1 wird Positionsrahmen hinter Text gestellt, das haben wir aus ter_RTF3.c(fBehindDocument)
		int ZOrder = -1; //-9999;
		BOOL bResult = ::TerSetObjectAttribEx(GetHWnd(), nFrameID, LineType, LineThickness, LineColor, FillSolid, FillColor, ZOrder);
		
	// This function sets the vertical base position for a frame or a drawing object.
		int base = BASE_PARA; 

		if(nType == TAG_TEXTFRAME_MARG)
			base = BASE_MARG;
		if(nType == TAG_TEXTFRAME_PAGE)
			base = BASE_PAGE;
 
	// der folgende Befehl klappt nicht! Wir machen es in TerInsertDrawObject
	 
	  bResult = ::TerSetFrameYBase(GetHWnd(), nFrameID, base);
/*
   int left,right,top,bottom,header,footer;
   if(::TerGetMarginEx(GetHWnd(), SECT_CUR, &left,&right,&top,&bottom,&header,&footer))
   {
   // ::TerMoveParaFrame2(GetHWnd(), nFrameID, nLeft-left, nTop-top, -1, -1, -1);
   }
*/
	 int direction = TEXT_HORZ;

	 if(nTextDirection == TAG_TABLE_ROTATE_TOPTOBOT)
		direction = TEXT_TOP_TO_BOT;

	 if(nTextDirection == TAG_TABLE_ROTATE_BOTTOTOP)
		direction = TEXT_BOT_TO_TOP;

	 ::TerRotateFrameText(GetHWnd(), FALSE, -1 * nFrameID, direction, FALSE);

	 	// select "Normal"
	 //CXPubParagraphStyle* pStyle = m_pBridgeToStyleManager->GetActualFormatInStack();
	 //SelectActualStyle(pStyle, xtstring(_XT("Normal")));

	}
  return (bResult != FALSE);

}

bool CConvertWorkerToRtf::Margins(int nLeft, int nRight, int Top, int Button)
{
// SECT_CUR, SECT_ALL  bei secID möglich

  int nSecId = GetCurSecId();
  BOOL bResult = ::TerSetMarginEx(GetHWnd(), nSecId, nLeft, nRight, Top, Button, -1, -1, FALSE); 

	return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetSectionOrientation(int nOrient/*=SECTION_PORTRAIT*/)
{
  return (::TerSetSectOrient(GetHWnd(),
             nOrient == TAG_SECTION_PORTRAIT ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE, FALSE) != FALSE);
}

bool CConvertWorkerToRtf::SetPaperSize(int nDmConstant, int nWidth/*=0*/, int nLength/*=0*/)
{
  // nDmConstant = DMPAPER_ constant from SDK e.g. DMPAPER_A4
  //BOOL bResult = ::TerSetPaperEx(GetHWnd(), nDmConstant, nWidth, nLength, -1, FALSE);

	BOOL bResult = ::TerSetSectPageSize(GetHWnd(), SECT_CUR, nDmConstant, nWidth, nLength, TRUE);
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetDocumentInfo(const TDocumentProperty nTypeOfProperty, const xtstring& sValue)
{
  int nProperty;

  if (nTypeOfProperty == eDoc_Title)
    nProperty = INFO_TITLE;
  else if (nTypeOfProperty == eDoc_Subject)
    nProperty = INFO_SUBJECT;
  else if (nTypeOfProperty == eDoc_Author)
    nProperty = INFO_AUTHOR;
  else if (nTypeOfProperty == eDoc_Keywords)
    nProperty = INFO_KEYWORDS;
  else if (nTypeOfProperty == eDoc_Comment)
    nProperty = INFO_COMMENT;
  else if (nTypeOfProperty == eDoc_Operator)
    nProperty = INFO_OPERATOR;
  else if (nTypeOfProperty == eDoc_Manager)
    nProperty = INFO_MANAGER;
  else if (nTypeOfProperty == eDoc_Category)
    nProperty = INFO_CATEGORY;
  else
    return true;

  return SetDocumentInfo(nProperty, sValue);
}

bool CConvertWorkerToRtf::SetDocumentInfo(const int nProperty, const xtstring& sValue)
{
  BOOL bResult = ::TerSetRtfDocInfo(GetHWnd(), nProperty, (BYTE*)(LPCSTR)sValue.c_str());
	return (bResult != FALSE);
}

bool CConvertWorkerToRtf::DocumentInfo(const xtstring& cTag)
{
	xtstring cParValue[4];
	xtstring cInTag(cTag);

	struct TerDocumentInfo {
		char	*constante;
		int   methodval;
	};

	TerDocumentInfo docinfo[] = {
		{TAG_RTF_DOC_INFO_TITLE, INFO_TITLE},
		{TAG_RTF_DOC_INFO_SUBJECT, INFO_SUBJECT},
		{TAG_RTF_DOC_INFO_AUTHOR, INFO_AUTHOR },
		{TAG_RTF_DOC_INFO_MANAGER, INFO_MANAGER},
		{TAG_RTF_DOC_INFO_COMPANY, INFO_COMPANY},
		{TAG_RTF_DOC_INFO_OPERATOR, INFO_OPERATOR},
		{TAG_RTF_DOC_INFO_CATEGORY, INFO_CATEGORY},
		{TAG_RTF_DOC_INFO_KEYWORDS, INFO_KEYWORDS},
		{TAG_RTF_DOC_INFO_COMMENT, INFO_COMMENT},
		{TAG_RTF_DOC_INFO_DOCCOMM, INFO_DOCCOMM}
	};

	bool bReturn = TRUE;
	for (size_t nI = 0; nI < sizeof (docinfo) / sizeof(TerDocumentInfo); nI++)
	{
		if (cInTag.FindNoCase(docinfo[nI].constante) != xtstring::npos)
		{	
			GETPARAM1(xtstring(docinfo[nI].constante), cParValue[0]);
			bReturn &= SetDocumentInfo(docinfo[nI].methodval, cParValue[0]);
		}
	}
	return (bReturn != FALSE);
}

bool CConvertWorkerToRtf::CreateDefinedStyle(CXPubParagraphStyle* pStyle)
{
  bool bRet = CConvertWorkerTo::CreateDefinedStyle(pStyle);

  bool bRegisterNow = false;
  if (pStyle->GetStyleIsPermanent()
      && (pStyle->GetStyleID() == -1))
    bRegisterNow = true;

	if(pStyle->IsDefaultStyle())
	{
		pStyle->SetStyleID(0);
		bRegisterNow = true;
	}

  if (bRegisterNow)
  {
		BOOL bCreating = FALSE;
		bCreating = TRUE;

// die gesetzten Styles ans Ter übertragen
		int nType = SSTYPE_PARA;
		if(pStyle->GetStyleType() == CXPubParagraphStyle::tST_CharacterType)
			nType = SSTYPE_CHAR;

		BOOL bBeginRecording = TRUE;
		int nStyleID = ::TerEditStyle(GetHWnd(),
                                      bBeginRecording,
                                      (LPBYTE)pStyle->GetStyleName().c_str(),
                                      bCreating,
                                      nType,
                                      FALSE);
		if(nStyleID == -1)
		{
			::TerCancelEditStyle(GetHWnd());
			return FALSE;
		}

		pStyle->SetStyleID(nStyleID);

	// Char&Par Styles
		SetFont(pStyle->GetFontName(), pStyle->GetFontSize(), pStyle->GetCodePage());
		SetTextColor(pStyle->GetFontColor());
		SetUnderLineColor(pStyle->GetFontColor());
		SetFontAttribute(pStyle->GetBold() ? TAG_TEXT_BOLD : TAG_TEXT_BOLD_END);
		SetFontAttribute(pStyle->GetUnderline() ? TAG_TEXT_UNDERLINE : TAG_TEXT_UNDERLINE_END);
		SetFontAttribute(pStyle->GetItalic() ? TAG_TEXT_ITALIC : TAG_TEXT_ITALIC_END);
		SetFontAttribute(pStyle->GetDoubleUnderline() ? TAG_TEXT_UL_DOUBLE : TAG_TEXT_UL_DOUBLE_END);
		SetFontAttribute(pStyle->GetStrikeOut() ? TAG_TEXT_STRIKE : TAG_TEXT_STRIKE_END);

		if(pStyle->GetStyleType() == CXPubParagraphStyle::tST_ParagraphType)
		{
		// Pagebreak before
			int nPageBreakBefore;
			pStyle->GetPageBreakBefore(nPageBreakBefore);
			ParagraphPageBreak(nPageBreakBefore);

		// Linespace
      int nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing;
      pStyle->GetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
      SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);

		// Paragraph
			int nLeft, nRight, nIdent;
			pStyle->GetParagraphIndents(nLeft, nRight, nIdent);
			SetParagraphIndents(nLeft, nRight, nIdent);

		// Pagination
			int nKeep, nKeepNext, nWidowOrphon;
			pStyle->GetParagraphControl(nKeep, nKeepNext, nWidowOrphon);
			SetParagraphControl(nKeep, nKeepNext, nWidowOrphon);

		// Textalignmen
      int nTextAlign;
      pStyle->GetTextAlignment(nTextAlign);
      TextAlignment(nTextAlign);

		// Tabstop
			if (pStyle->CountOfTabStops() == 0)
				ClearTabulators();
			else
			{
				for(int i=0; i < pStyle->CountOfTabStops(); i++)
				{
					CXPubTabStop* pTabStop = pStyle->GetTabStop(i);
					if(pTabStop)
						InsertTabulator(pTabStop->GetTabFlag(),pTabStop->GetTabPos(), 
													pTabStop->GetTabDecChar(), pTabStop->GetTabLeader());
				}
			}
		}
	// Ende der Aufzeichnung
		bBeginRecording = FALSE;
		::TerEditStyle(GetHWnd(),
                    bBeginRecording,
                    (LPBYTE)pStyle->GetStyleName().c_str(),
                    bCreating,
                    nType,
                    FALSE);

		if(pStyle->GetStyleType() == CXPubParagraphStyle::tST_ParagraphType)
		{
			// next style
			xtstring sNextStyle = pStyle->GetNextStyle();
			if(sNextStyle.size())
			{
			// Routine gibt styleID: style muss vorhanden sein	
				int nStylID = ::TerEditStyle(GetHWnd(), TRUE, (LPBYTE)sNextStyle.c_str(),
												FALSE,SSTYPE_PARA, FALSE);
				::TerEditStyle(GetHWnd(), FALSE, (LPBYTE)sNextStyle.c_str(),
												FALSE,SSTYPE_PARA, FALSE);
			// nach Enter den folgenden Stil selektieren	
				::TerSetStyleParam(GetHWnd(), pStyle->GetStyleID(), SSINFO_NEXT, nStylID, NULL, FALSE);
			}
		}
  }
  return bRet;
}

void CConvertWorkerToRtf::StyleEvent(TStyleEvent nStyleEvent, CXPubParagraphStyle* pStyle)
{
  CConvertWorkerTo::StyleEvent(nStyleEvent, pStyle);

  if (nStyleEvent == tSE_StartWork)
  {
		CXPubParagraphStyle* pActualStyle = m_pBridgeToStyleManager->GetActualStyle();
    if (pActualStyle && pActualStyle->IsDefaultStyle())
		{
		 // Style "Normal" kreieren und einstellen
			CreateDefinedStyle(pActualStyle);
			::TerSelectParaStyle(GetHWnd(), 0, FALSE);
		}
	}
  if (nStyleEvent == tSE_FormatStackStart)
  {
    if (m_pBridgeToStyleManager && m_pBridgeToStyleManager->GetActualStyle())
    {
      CXPubParagraphStyle* pActualStyle = m_pBridgeToStyleManager->GetActualStyle();
      if (pActualStyle->GetStyleID() == -1)
      {
        // es war noch nicht registriert
        bool bTemp = pActualStyle->GetStyleIsPermanent();
        pActualStyle->SetStyleIsPermanent(true);
        CreateDefinedStyle(pActualStyle);
        pActualStyle->SetStyleIsPermanent(bTemp);
      }

      // Sonderfall wenn "Normal"
      if(pActualStyle->IsDefaultStyle())
      {
				if(pActualStyle->GetStyleType() == CXPubParagraphStyle::tST_ParagraphType)
				{
					::TerSelectParaStyle(GetHWnd(), pActualStyle->GetStyleID(), FALSE);

					SetFont(pStyle->GetFontName(), pStyle->GetFontSize(), pStyle->GetCodePage());
					SetTextColor(pStyle->GetFontColor());
					SetUnderLineColor(pStyle->GetFontColor());
					SetFontAttribute(pStyle->GetBold() ? TAG_TEXT_BOLD : TAG_TEXT_BOLD_END);
					SetFontAttribute(pStyle->GetUnderline() ? TAG_TEXT_UNDERLINE : TAG_TEXT_UNDERLINE_END);
					SetFontAttribute(pStyle->GetItalic() ? TAG_TEXT_ITALIC : TAG_TEXT_ITALIC_END);
					SetFontAttribute(pStyle->GetDoubleUnderline() ? TAG_TEXT_UL_DOUBLE : TAG_TEXT_UL_DOUBLE_END);
					SetFontAttribute(pStyle->GetStrikeOut() ? TAG_TEXT_STRIKE : TAG_TEXT_STRIKE_END);
				}
      }
      else if (pActualStyle->GetStyleID() != -1)
      {
				if(pActualStyle->GetStyleType() == CXPubParagraphStyle::tST_ParagraphType)
				{
					::TerSelectParaStyle(GetHWnd(), pActualStyle->GetStyleID(), FALSE);
				}
				// Tabulator wird hier automatisch von TE gesetzt
      }
    }
  }
  else if (nStyleEvent == tSE_FormatStackAdd
        || nStyleEvent == tSE_FormatStackRemove
        || nStyleEvent == tSE_FormatStackSegmentRemoved)
  {
    if (m_pBridgeToStyleManager && m_pBridgeToStyleManager->GetActualFormatInStack())
    {			
			assert(pStyle);

			if(nStyleEvent == tSE_FormatStackAdd && pStyle->GetStyleType() == CXPubParagraphStyle::tST_CharacterType)
			{
				if(pStyle->GetStyleID() != -1)
					::TerSelectCharStyle(GetHWnd(), pStyle->GetStyleID(), FALSE);

				return;
			}
			else
			{
				SetFont(pStyle->GetFontName(), pStyle->GetFontSize(), pStyle->GetCodePage());
				SetTextColor(pStyle->GetFontColor());
				SetUnderLineColor(pStyle->GetFontColor());
				SetFontAttribute(pStyle->GetBold() ? TAG_TEXT_BOLD : TAG_TEXT_BOLD_END);
				SetFontAttribute(pStyle->GetUnderline() ? TAG_TEXT_UNDERLINE : TAG_TEXT_UNDERLINE_END);
				SetFontAttribute(pStyle->GetItalic() ? TAG_TEXT_ITALIC : TAG_TEXT_ITALIC_END);
				SetFontAttribute(pStyle->GetDoubleUnderline() ? TAG_TEXT_UL_DOUBLE : TAG_TEXT_UL_DOUBLE_END);
				SetFontAttribute(pStyle->GetStrikeOut() ? TAG_TEXT_STRIKE : TAG_TEXT_STRIKE_END);
			}
    }
  }
  else if (nStyleEvent == tSE_FormatStackFormatToRemove)
  {
    assert(pStyle);

		if(nStyleEvent == tSE_FormatStackFormatToRemove && pStyle->GetStyleType() == CXPubParagraphStyle::tST_CharacterType)
		{
			// "Default Paragraph Font" herausfinden und selektieren
			// das TE definiert einen Default Par und Default Char Style
			int nStylID = ::TerEditStyle(GetHWnd(), TRUE, (LPBYTE)"Default Paragraph Font",
											FALSE,SSTYPE_CHAR, FALSE);
			::TerEditStyle(GetHWnd(), FALSE, (LPBYTE)"Default Paragraph Font",
											FALSE,SSTYPE_CHAR, FALSE);
			if(nStylID != -1)
				::TerSelectCharStyle(GetHWnd(), nStylID, FALSE);
			
			return;
		}
  }
  else if (nStyleEvent == tSE_FormatStackParaChanged)
  {
    if (m_pBridgeToStyleManager && m_pBridgeToStyleManager->GetActualFormatInStack())
    {
      CXPubParagraphStyle* pStyle = m_pBridgeToStyleManager->GetActualFormatInStack();
 
		// Paragraph
			int nLeft, nRight, nIdent;
			pStyle->GetParagraphIndents(nLeft, nRight, nIdent);
			SetParagraphIndents(nLeft, nRight, nIdent);

		 // Pagination
      int nKeep, nKeepNext, nWidowOrphon;
      pStyle->GetParagraphControl(nKeep, nKeepNext, nWidowOrphon);
      SetParagraphControl(nKeep, nKeepNext, nWidowOrphon);

      int nPageBreakBefore;
      pStyle->GetPageBreakBefore(nPageBreakBefore);
      ParagraphPageBreak(nPageBreakBefore);

      int nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing;
      pStyle->GetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
      SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);

      int nTextAlign;
      pStyle->GetTextAlignment(nTextAlign);
      TextAlignment(nTextAlign);

      if (pStyle->CountOfTabStops() == 0)
        ClearTabulators();
      else
      {
        for(int i=0; i < pStyle->CountOfTabStops(); i++)
        {
          CXPubTabStop* pTabStop = pStyle->GetTabStop(i);
          if(pTabStop)
            InsertTabulator(pTabStop->GetTabFlag(),pTabStop->GetTabPos(), 
                          pTabStop->GetTabDecChar(), pTabStop->GetTabLeader());
        }
      }
      int nTabDefaultPos, nTabDefaultDecimalChar;
      pStyle->GetTabDefaults(nTabDefaultPos, nTabDefaultDecimalChar);
      SetDefTabulator(nTabDefaultPos, nTabDefaultDecimalChar);
    }
  }
}

//SYMBOL 43 \a \f Wingdings \h \* MERGEFORMAT
//{LINK Excel.Sheet.8 "C:\\My Documents\\BOOK2.XLS" "Sheet1!R1C1" \a \r}

bool CConvertWorkerToRtf::Field(const xtstring& cTag)
{
	xtstring cInTag(cTag);
	BOOL bReturn = FALSE;

	int nPos;
	xtstring sFieldName, sFieldValue;
  xtstring cParValue[4];

	int nCurLine = ::TerGetParam(GetHWnd(), TP_CUR_LINE);

  if((nPos=cInTag.FindNoCase(TAG_FIELD_DATE)) != -1)
  {			
    GETPARAM(TAG_FIELD_DATE, cParValue[0]);

    sFieldName.Format(_XT("DATE \\@ \"%s\"  \\* MERGEFORMAT"), cParValue[0].c_str()); // z.B. "dddd, d. MMMM yyyy"
  }

	if((nPos=cInTag.FindNoCase(TAG_FIELD_NAME)) != -1)
	{			
		sFieldName = cInTag.Mid(nPos+strlen(TAG_FIELD_NAME));
    sFieldName.ReplaceAll(xtstring(_XT("'")), xtstring(_XT("\"")));
    sFieldName.Trim(">");
    sFieldName.Trim("\"");
	}
// jetzt auswerten
	if((nPos=cInTag.FindNoCase(TAG_FIELD_SYMBOL)) != -1)
	{			
		sFieldName = cInTag.Mid(nPos+strlen(TAG_FIELD_SYMBOL));
		sFieldName.Trim(_XT(">"));

		if ( sFieldName.GetFieldCount(_XT(' ')) <= 2)
			sFieldName = _XT("SYMBOL ") + sFieldName + _XT(" \\a \\f Wingdings \\h ");

		sFieldName += _XT(" \\* MERGEFORMAT");

	}
	else
	if((nPos=cInTag.FindNoCase(TAG_FIELD_HYPERL)) != -1)
	{		
// wir müssen noch unterstrichen und blau einsetzen
		SetTextColor(RGB(0,0,255));

		sFieldName = cInTag.Mid(nPos+strlen(TAG_FIELD_HYPERL));
		sFieldName.Trim(_XT(">"));

		sFieldValue = _XT("") + sFieldName + _XT(""); // sichtbare Form
		sFieldName = _XT("HYPERLINK ") + sFieldName + _XT("\\* MERGEFORMAT");
	
		SetTextColor(RGB(0,0,0));
	}
	else
	if((nPos=cInTag.FindNoCase(TAG_FIELD_LINK)) != -1)
	{			
		sFieldName = cInTag.Mid(nPos+strlen(TAG_FIELD_LINK));
		sFieldName.Trim(_XT(">"));

		sFieldValue = _XT(""); // sichtbare Form
		sFieldName = _XT("LINK ") + sFieldName + _XT("\\b \\* MERGEFORMAT");

		//int id = ::TerInsertOleFile(GetHWnd(), (LPBYTE)sFieldName.c_str(), TRUE, FALSE, -1, -1);
 
		//UINT style; INT align, aux; RECT rect;
		//BOOL  bReturn = ::TerGetPictInfo(GetHWnd(), id, &style, &rect, &align, &aux);
		//return 0;
	}
	else
	if((nPos=cInTag.FindNoCase(TAG_FIELD_FORMEL)) != -1)
	{			
		sFieldName = cInTag.Mid(nPos+strlen(TAG_FIELD_FORMEL));
		sFieldName.Trim(">");
		sFieldValue = _XT("*******");
		sFieldName = _XT("=") + sFieldName;
		sFieldName += " \\* MERGEFORMAT";
	}
	else
	if( sFieldName.FindNoCase("SECTIONPAGES") != -1)
	{
		sFieldValue = _XT("*");
		sFieldName = _XT("SECTIONPAGES \\* MERGEFORMAT");
	}
	else
	if( sFieldName.FindNoCase("SECTION") != -1)
	{
		sFieldValue = _XT("*");
		sFieldName = _XT("SECTION \\* MERGEFORMAT");
	}
	else
	if( sFieldName.FindNoCase("NUMPAGES") != -1)
	{
		sFieldValue = _XT("*");
		sFieldName = _XT("NUMPAGES \\* MERGEFORMAT");
	}
	else
	if( sFieldName.FindNoCase("PAGE") != -1)
	{
		::TerRepaginate(GetHWnd(), FALSE); 

		int nCurLine = ::TerGetParam(GetHWnd(), TP_CUR_LINE);
		int nPage = ::TerPageFromLine(GetHWnd(),nCurLine) + 1;

		char TempString[10];
		itoa(nPage,TempString,10);
 
		sFieldValue = _XT(TempString);
		sFieldName = _XT("PAGE \\* MERGEFORMAT");

	}
	else
	if( sFieldName.FindNoCase("DOCPROPERTY  Author") != -1)
	{
		sFieldValue = _XT("Author");
		sFieldName = _XT("DOCPROPERTY  Author \\* MERGEFORMAT");
	}
	if((nPos=cInTag.FindNoCase(TAG_FIELD_VALUE)) != -1)
	{
		nPos=cInTag.FindNoCase(TAG_FIELD_NAME);
		if ( sFieldName.GetFieldCount(_XT(',')) > 1)
		{
			sFieldName = cInTag.GetField(_XT(','), 0);
			sFieldName = sFieldName.Mid(nPos+strlen(TAG_FIELD_NAME));

		}
		nPos=cInTag.FindNoCase(TAG_FIELD_VALUE);
		sFieldValue = cInTag.Mid(nPos+strlen(TAG_FIELD_VALUE));
		sFieldValue.Trim(_XT(">"));
	}

	// aktuellen Font und Size auslesen
	xtstring sTypeFace;
	int nPointSize;
	int styles;
	GetActualFont(sTypeFace, nPointSize, styles);

	bReturn = ::TerInsertField(GetHWnd(), (LPBYTE)sFieldName.c_str(), (LPBYTE)sFieldValue.c_str(), FALSE);

  // auf Font zurückschalten
	SetFont(sTypeFace, nPointSize);

	return (bReturn != FALSE);

}

/*

bei Type ist implementiert:
0 Regular text
1 Number:"#.##0,00 €;(#.##0,00 €)"

// folgende noch nicht
2 Date
3 Current date: dd.MM.yyyy
4 Current time
5 Calculation

*/

bool CConvertWorkerToRtf::FormField(const xtstring& cName, const xtstring& cValue, const xtstring& cType,
                                    const xtstring& cMask, const int nLength, const xtstring& sFontName, const int nFontSize, const int nCodePage)
{

return false;

/*
	xtstring cInTag(cTag);
	xtstring sFieldName, sFieldValue;
	xtstring sFieldLength;

	int nPos;
	BOOL bReturn = FALSE;

	if((nPos=cInTag.FindNoCase(TAG_FORMFIELD_NAME)) != -1)
	{			
		sFieldName = cInTag.Mid(nPos+strlen(TAG_FORMFIELD_NAME));
		nPos=sFieldName.FindNoCase(" ");
		if ( nPos != -1)
		{
			sFieldName = sFieldName.Mid(0, nPos-1);
		}
		sFieldName.Trim(_XT(">"));
		//sFieldName += "{\\*\\ffformat #.##0,00 \'80\'3b(#.##0,00 \'80)}";
  
		nPos=cInTag.FindNoCase(TAG_FORMFIELD_VALUE);
		if(nPos != -1)
		{	
			sFieldValue = cInTag.Mid(nPos+strlen(TAG_FORMFIELD_VALUE)+1);
			nPos=sFieldValue.FindNoCase("\"");
			if(nPos != -1)
				sFieldValue = sFieldValue.Mid(0, nPos);
			sFieldValue.Trim(_XT(">"));
			sFieldValue.Trim(_XT(" "));
		}

		nPos=cInTag.FindNoCase(TAG_FORMFIELD_LENGTH);
		if(nPos != -1)
		{	
			sFieldLength = cInTag.Mid(nPos+strlen(TAG_FORMFIELD_LENGTH));
			nPos=sFieldLength.FindNoCase(" ");
			if(nPos != -1)
				sFieldLength = sFieldLength.Mid(0, nPos-1);
			
			sFieldLength.Trim(_XT(">"));
			sFieldLength.Trim(_XT(","));
		}

		xtstring sInpMask(_XT(""));
		nPos=cInTag.FindNoCase(TAG_FORMFIELD_MASK);
		if(nPos != -1)
		{	
			sInpMask = cInTag.Mid(nPos+strlen(TAG_FORMFIELD_MASK)+1);
			nPos=sInpMask.FindNoCase("\"");
			if(nPos != -1)
				sInpMask = sInpMask.Mid(0, nPos);

			sInpMask.Trim(_XT(">"));
			sInpMask.Trim(_XT(","));
		}


		xtstring sType(_XT("0"));
		nPos=cInTag.FindNoCase(TAG_FORMFIELD_TYPE);
		if(nPos != -1)
		{	
			sType = cInTag.Mid(nPos+strlen(TAG_FORMFIELD_TYPE));
			nPos=sType.FindNoCase(" ");
			if(nPos != -1)
				sType = sType.Mid(0, nPos);

			sType.Trim(_XT(">"));
			sType.Trim(_XT(","));
		}


		int border = TRUE;
		int color=0;

		xtstring sTypeFace;
		int nPointSize;
		int style;

		int MaxChars=12;
	
   // zur Sicherheit auf Defaulttextlänge setzen
		if(MaxChars < sFieldValue.length())
			MaxChars=sFieldValue.length();
		
		GetActualFont(sTypeFace, nPointSize, style);

		HWND hWnd=(HWND)0; 
		UINT typetxt = atoi(sType.c_str()); // default text input formfield

		int id = ::TerInsertTextInputField(GetHWnd(), (LPBYTE)sFieldName.c_str(), (LPBYTE)sFieldValue.c_str(), MaxChars,
													border, (LPBYTE)sTypeFace.c_str(), nPointSize*20, style,
													color, TRUE, typetxt, &hWnd, (LPBYTE)sInpMask.c_str(), (LPBYTE)"Edit", FALSE);

//
//		int width = MMToTwips(20); // Breite des controls
//		bReturn = ::TerSetTextFieldInfo(GetHWnd(),id, (LPBYTE)sFieldValue.c_str(), MaxChars, 
//													width, (LPBYTE)sTypeFace.c_str(), nPointSize*20, style);

		if(id)
		{
			// Einfügemarke "absolut" hinter das Eingabefeld setzen
			int CurCol=-1;long CurLine;
			::GetTerCursorPos(GetHWnd(),&CurLine,&CurCol); 
			::SetTerCursorPos(GetHWnd(), ++CurLine, CurCol, FALSE);
		}

	
	}	
	return (bReturn != FALSE);
*/
}

// Seriendruckfelder in RTF für WORD anlegen
bool CConvertWorkerToRtf::MergeField(const xtstring& cTag)
{
	xtstring cInTag(cTag);
	xtstring cParValue[4];
	BOOL bReturn = FALSE;

	if( cInTag.FindNoCase(TAG_FIELD_NAME) != xtstring::npos)
	{
		GETPARAM1(TAG_FIELD_NAME, cParValue[0]);
	
		xtstring sFieldName, sFieldValue;

		if( cParValue[0].FindNoCase("NEXT") != xtstring::npos)
		{
			sFieldValue = _XT("«Nächster Datensatz»");
			sFieldName = _XT("NEXT \\* MERGEFORMAT");
		}
		else
		{
			sFieldValue = _XT("«")+ cParValue[0] + _XT("»");
			sFieldName = _XT("MERGEFIELD ") + cParValue[0] + (" \\* MERGEFORMAT");
		}
		bReturn = ::TerInsertField(GetHWnd(), (LPBYTE)sFieldName.c_str(), (LPBYTE)sFieldValue.c_str(), FALSE);
	}
	return (bReturn != FALSE);
}

bool CConvertWorkerToRtf::InsertAnchor(const xtstring& sAnchorName)
{
	BOOL bReturn = FALSE;

	int CurCol=-1;long CurLine=0;
	bReturn = ::GetTerCursorPos(GetHWnd(),&CurLine,&CurCol); 
	m_cAnchorList[sAnchorName] = CurLine;

	return (bReturn != FALSE);
}

// die Lesezeichen werden bei RTF am Schluß gesetzt, da sich ein Lesezeichen die aktuelle Position im TE holt
bool CConvertWorkerToRtf::InsertBookmark(xtstring& sLevel, xtstring& sText, xtstring& sSymbol, int nZoom, bool bIgnorePos, int nColor, int nStyle)
{
	BOOL bReturn = FALSE;

	int CurCol=-1;long CurLine=0;
	bReturn = ::GetTerCursorPos(GetHWnd(),&CurLine,&CurCol); 

// wir erlauben Bookmark mit oder ohne Anker
	if(sSymbol.size())
		m_cBookList[sSymbol] = sText;
	else
	{
		xtstring sFmt;
		sFmt.Format(_XT("%ld"), CurLine);
		m_cBookList[sText] = sFmt;
	}
	return (bReturn != FALSE);
}

// TODO: Fehlerausgabe bei Nicht-Gesetzten Textmarken
// Im Moment bricht Routine ab, wenn Textmarke nicht gesetzt werden konnte
bool CConvertWorkerToRtf::SetBookmarkFromList()
{
	int id = 0;
	
	if(m_cAnchorList.size())
	{
		map<xtstring, xtstring>::iterator cIt; 
		cIt = m_cBookList.begin(); 
		for (; cIt != m_cBookList.end(); ++cIt)
		{
			int col=-1;long line=0;
			xtstring sBookName = (*cIt).second.c_str();
			xtstring sAnchor = (*cIt).first.c_str();

			map<xtstring, long>::iterator cInt; 
			cInt = m_cAnchorList.find(sAnchor);
			if(cInt != m_cAnchorList.end())
			{
	// The name may consist of regular alphabetic characters, but it may not contain spaces.
				sBookName.ReplaceAll(_XT(" "), _XT("_"));
				::TerAbsToRowCol(GetHWnd(), (*cInt).second, &line, &col);
				id &= ::TerInsertBookmark(GetHWnd(), line, col, (LPBYTE)sBookName.c_str());
				if(!id)
					break;
			}
		}
	}
	else
	{
		map<xtstring, xtstring>::iterator cIt; 
		cIt = m_cBookList.begin(); 
		for (; cIt != m_cBookList.end(); ++cIt)
		{
			long lCurLine = atol((*cIt).first.c_str());
			int col=-1;long line=0;
			xtstring sBookName = (*cIt).second.c_str();
	// The name may consist of regular alphabetic characters, but it may not contain spaces.
			sBookName.ReplaceAll(_XT(" "), _XT("_"));
			::TerAbsToRowCol(GetHWnd(), lCurLine, &line, &col);
			id &= ::TerInsertBookmark(GetHWnd(), line, col, (LPBYTE)sBookName.c_str());
			if(!id)
				break;
		}
	}
	return (id != 0);

}

bool CConvertWorkerToRtf::SetPageCount(int nCount)
{
  LRESULT lResult;
  lResult = ::SendMessage(GetHWnd(), WM_COMMAND, ID_INSERT_PAGE_COUNT, 0l);
  return (lResult == 1);
}

bool CConvertWorkerToRtf::SetPageNumberFormat(int nFormat)
{
  LRESULT lResult;
/*
  NBR_DEC // Decimal number
  NBR_UPR_ROMAN // Uppercase roman letters
  NBR_LWR_ROMAN // Lowercase roman letters
  NBR_UPR_ALPHA // Uppercase alphabets
  NBR_LWR_ALPHA // Lowercase alphabets
*/ 
  lResult = ::TerSetPageNumFmt(GetHWnd(), SECT_CUR, NBR_DEC);
  return (lResult == 1);
}

bool CConvertWorkerToRtf::SetCurrentPage(int nCount)
{
  LRESULT lResult;
	lResult = ::SendMessage(GetHWnd(), WM_COMMAND, ID_INSERT_PAGE_NUMBER,0l);
  return (lResult == 1);
}

bool CConvertWorkerToRtf::InsertPageBreak()
{
	BOOL bReturn = ::TerPageBreak(GetHWnd(), FALSE);
// Kannumbruch löschen
	m_nCanPageBreak.Clear();

	return (bReturn != FALSE);
}

bool CConvertWorkerToRtf::ClearParagraphEndofDocument()
{
/*
  struct StrTerField afield;
  ::GetTerFields(GetHWnd(),&afield);
*/

  INT TotalPages, CurPage;
  BYTE text1[MAX_WIDTH], text2[MAX_WIDTH];

  if(::TerGetPageCount(GetHWnd(), &TotalPages, &CurPage))
  {
    int nLines = ::TerGetParam(GetHWnd(), TP_TOTAL_LINES );
    
    int bLine1 = ::TerGetLine(GetHWnd(),--nLines,text1,NULL);
    int bLine2 =::TerGetLine(GetHWnd(),--nLines,text2,NULL);

    if(bLine1 != -1 && bLine2 != -1 && text1[0] == text2[0] && text2[0] == PARA_CHAR)
    {  
      ::SendMessage(GetHWnd(), WM_COMMAND, ID_DEL_LINE, 0l);
    }

/*
    long nFirstLine = ::TerGetPageFirstLine(GetHWnd(), TotalPages-1);
    if(::TerGetLine(GetHWnd(),nFirstLine,text,NULL) != -1)
    {
      
      int nPage = ::TerPageFromLine(GetHWnd(),nFirstLine);

      while(::TerPageFromLine(GetHWnd(), ++nFirstLine) != -1)
      {
        ::TerGetLine(GetHWnd(),nFirstLine,text,NULL);
      }
    }    
*/
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////
//

bool CConvertWorkerToRtf::ParagraphPageBreak(const int nInsert)
{
	::TerSetPflags(GetHWnd(),PFLAG_PAGE_BREAK,nInsert,FALSE);
  return true;
}

bool CConvertWorkerToRtf::InsertSectionBreak()
{
/*

// TerSetSectOrient(HWND,int,BOOL);
	INT nSec = GetSectionNumber();

	int nCurLine = ::TerGetParam(GetHWnd(), TP_CUR_LINE);
	int nPage = ::TerPageFromLine(GetHWnd(),nCurLine);

	int nSecId = ::TerGetSectFromLine(GetHWnd(), nCurLine);	
*/

	BOOL bReturn = ::TerSectBreak(GetHWnd(), FALSE);


// wir bekommen sonst nicht korrekte Seitenzahl	
	//bReturn &= ::TerRepaginate(GetHWnd(), FALSE); 


/*
	int NewnCurLine = ::TerGetParam(GetHWnd(), TP_CUR_LINE);
	nSecId = ::TerGetSectFromLine(GetHWnd(), nCurLine);	
	int nPages = ::TerGetParam(GetHWnd(), TP_TOTAL_PAGES);
	int NewnCurLine = ::TerGetPageFirstLine(GetHWnd(), nPages);
	
	int nSecId = ::TerGetPageSect(GetHWnd(), nPages);

//SetCursorPosition(CURSOR_END);
//	::TerPosBodyText (GetHWnd(), nSec+1, POS_BEG, FALSE);
//	int NewnCurLine = ::TerGetParam(GetHWnd(), TP_CUR_LINE);

	nPage = ::TerPageFromLine(GetHWnd(),NewnCurLine+2);
	nSec = GetSectionNumber();
	
*/

	return (bReturn != FALSE);
}
bool CConvertWorkerToRtf::InsertColumnBreak()
{
  BOOL bReturn = ::TerColBreak(GetHWnd(), FALSE);
  return (bReturn != FALSE);
}

bool CConvertWorkerToRtf::CanPageBreak(bool bDelete)
{
	if(bDelete)
		m_nCanPageBreak.Clear();
	else
		m_nCanPageBreak.Set(GetHWnd());
	return true;
}

bool CConvertWorkerToRtf::CheckCanPageBreak()
{
	m_nCanPageBreak.CheckPageBreak(GetHWnd());
	return true;
}

bool CConvertWorkerToRtf::EditSection(int NumCols, int ColSpace, int FirstPageNo)
{
	// TRUE to begin the section on a new page.	
	BOOL NewPage = TRUE;
	BOOL bResult = ::TerSetSectEx(GetHWnd(), NumCols, ColSpace, NewPage, FirstPageNo);

// Border um die Section
	//int nSecId = GetCurSecId();
	//::TerSetSectBorder(GetHWnd(), SECT_CUR, BRDRTYPE_SINGLE, /*width*/ 10, /*space*/700, RGB(0,0,0), FALSE);

	return (bResult != FALSE);
}

int CConvertWorkerToRtf::CreateTable(const xtstring& id, const xtstring& sName, int wRows, int wColumns,
												int numHeaderRows, int numFooterRows, double spacebefore, double spaceafter)
{
  static int nTableId=0;
  nTableId++;

 // letzte Position merken
  map<int, CTableStackItem>::iterator cIt; 
  cIt = m_lTableStack.find(GetTableID());
  if(cIt != m_lTableStack.end())
  {
    (*cIt).second.m_nTableRow = GetTableRow();
    (*cIt).second.m_nTableCol = GetTableCol();
    (*cIt).second.m_nTableHdrRows = numHeaderRows;
  } 

	BOOL bResult = ::TerCreateTable(GetHWnd(), wRows, wColumns, FALSE);
  bResult &= ::TerSetTableId(GetHWnd(), -1/*current table*/, nTableId);
 
 // set the table header row if table has more lines than the header
 // if not set, the things are done inside AddRow
	if(numHeaderRows && wRows > numHeaderRows)
		SetTableHeaderRow(nTableId, 1, numHeaderRows, 2);

  CTableStackItem cTableStack; 
  cTableStack.m_nTableHdrRows = numHeaderRows;
  cTableStack.m_nTableCol = m_nTableCol;
  cTableStack.m_nTableRow = m_nTableRow;
  cTableStack.m_nTableID = nTableId;

  m_lTableStack[nTableId] = cTableStack;

  return (bResult ? nTableId : -1);
 
 /*
   BOOL bR = ::TerPosBookmark(GetHWnd(), (LPBYTE)"$$TABLEOUTERPOS", FALSE);

  if(!::TerGetTableLevel(GetHWnd(), -1/cell id instead of a line /))
  {
    LONG nCurLine = -1; // current line
    nCurLine=0;INT nCurCol=-1;
    ::GetTerCursorPos(GetHWnd(),&nCurLine,&nCurCol); 
    ::TerInsertBookmark(GetHWnd(), nCurLine, -1, (LPBYTE)"$$TABLEOUTERPOS");
  }
 
	//::TerRowPositionEx(GetHWnd(), LEFT, CTxtBase::MMToTwips(10) ,TRUE, FALSE);

  int ParaId, ParaFID, x, y, height;
  DWORD dwflags;
  INT CellId = -1;
  UINT InfoFlags;
  //bResult &= ::TerGetLineInfo(GetHWnd(), nCurLine, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);

  INT TableNo, RowNo, ColNo;

  if(ParentCell == -1)
    ParentCell = 0;
  else
    ParentCell = CellId;

  bResult &= ::TerGetTablePos(GetHWnd(), &TableNo, &RowNo, &ColNo, ParentCell);

  COLORREF BackColor;
  int CellId;
  int margin = -1;
  ParentCell = -1;
  //::TerSetCellInfo2(GetHWnd(), CellId, BackColor, margin, ParentCell);
  // diese ID können wir einer Tabelle zuweisen
  //INT nTableId = ::TerGetTableId(GetHWnd(), 0);
  */	
}

bool CConvertWorkerToRtf::SetTableRowKeep(int nTableID, int wRow, int wCol, bool bSet/*=true*/)
{
	int CellId = 0; // 0 means the current row

  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);

  BOOL bResult=FALSE;

  if(wRow > 0)
  { 
    bResult &= ::TerPosTable3(GetHWnd(), nTableID, wRow - 1, 0, TAG_POS_END, FALSE);
    int ParaId, ParaFID, x, y, height;
    DWORD dwflags;
    UINT InfoFlags;
    INT nCurLine = -1; // current line
    bResult &= ::TerGetLineInfo(GetHWnd(), nCurLine, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);
  }

  bResult &= ::TerSetRowKeep(GetHWnd(), CellId, bSet, FALSE);
	return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetTableHeaderRow(int nTableID, int wFromRow, int wToRow, int nSet)
{
	// Set the header row for a table
	int CellId = 0; // 0 means the current row

  int wRow = _GET_ROW(wFromRow);
  int wCol = _GET_COL(wToRow);

  BOOL bResult=FALSE;

 // check the actual rowcount
	int nRowCount = TerGetRowCellCount(GetHWnd(), TRUE);

	if(nSet == 2)
	{
	// if we set a line which is not in TE, all header definitions disappear
		if(wFromRow >= nRowCount || wToRow < wFromRow)
			return FALSE;

		int ParaId, ParaFID, x, y, height;
		DWORD dwflags;
		UINT InfoFlags;
		INT nCurLine = -1; // current line
		INT nOldCurLine = -1; // current line

	// save the actual current line
		INT nTable;
		bResult = TerGetTablePos(GetHWnd(), &nTable, &wRow, &wCol);

		for(int i=wFromRow; i <= wToRow; i++)
		{	
		// set to the line specified
			bResult &= ::TerPosTable3(GetHWnd(), nTableID, i - 1, 0, TAG_POS_END, FALSE);
		// set the header
			bResult &= ::TerSetHdrRow(GetHWnd(), 0/*current row*/, TRUE, FALSE);
		}

	// restore to the line
		bResult &= ::TerPosTable3(GetHWnd(), nTableID, wRow, wCol, TAG_POS_END, FALSE);

		return bResult == TRUE;
	}

  if(wRow > 0 && nSet == 1)
  { 
   bResult &= ::TerPosTable3(GetHWnd(), nTableID, wRow - 1, wCol - 1, TAG_POS_END, FALSE);
   int ParaId, ParaFID, x, y, height;
   DWORD dwflags;
   UINT InfoFlags;
   INT nCurLine = -1; // current line
   bResult &= ::TerGetLineInfo(GetHWnd(), nCurLine, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);
	}
	if(GetTableSelection(wRow, wCol) == SEL_ALL || GetTableSelection(wRow, wCol) == SEL_ROWS)
		bResult = ::TerSetHdrRow2(GetHWnd(), GetTableSelection(wRow, wCol), nSet, FALSE);
	else
		bResult = ::TerSetHdrRow(GetHWnd(), CellId, nSet, FALSE);

	return bResult == TRUE;

}

bool CConvertWorkerToRtf::CellBackgroundColor(int nTableID, int wRow, int wCol, _ColorDef crBkColor)
{
// Bis TE das behoben hat
	//if(crBkColor == 0) crBkColor = 1;

  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);
  BOOL bResult = TRUE; // 
	
	int CellId = 0; // 0 means the current row

	if(wRow > 0 && wCol > 0)
  { 
		bResult &= ::TerPosTable3(GetHWnd(), nTableID, wRow - 1, wCol - 1, POS_END , FALSE);
		int ParaId, ParaFID, x, y, height;
		DWORD dwflags;
		UINT InfoFlags;
		INT nCurLine = -1; // current line
		bResult &= ::TerGetLineInfo(GetHWnd(), nCurLine, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);

		int margin = -1;
		int ParentCell = -1;
		bResult &= ::TerSetCellInfo2(GetHWnd(), CellId, crBkColor, margin, ParentCell);

	}
	else
		bResult &= ::TerCellColor(GetHWnd(), GetTableSelection(wRow, wCol), crBkColor, TRUE);

	//::SaveTerFile(GetHWnd(), (LPBYTE)"e:\\$$TER.SSE");
	//SaveDataStream(xtstring("e:\\$$TER.rtf"));

  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetTableBackgroundPicture(int nTableID, int wRow, int indent, const xtstring& cFile)
{
// leider im RTF nicht unterstützt
	return false;
}

// Position a table row.
bool CConvertWorkerToRtf::SetRowPosition(int nTableID, int wRow, int indent)
{
	BOOL bResult = FALSE;

	wRow = _GET_ROW(wRow);
	
	BOOL bAllRows = (wRow==0)?TRUE:FALSE;

	UINT JustFlag=LEFT;
	bResult = ::TerRowPositionEx(GetHWnd(), JustFlag, indent ,bAllRows, FALSE);

	return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetCellBorderWidth(int nTableID, int wRow, int wCol, int nLeft, int nTop, int nRight, int nButton)
{
  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);
  BOOL bResult = ::TerCellBorder(GetHWnd(), GetTableSelection(wRow, wCol), nTop, nButton, nLeft, nRight, FALSE);
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetCellRotate(int nTableID, int wRow, int wCol, int nDirection)
{
  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);
  BOOL bResult = ::TerCellRotateText(GetHWnd(), GetTableSelection(wRow, wCol), nDirection, FALSE);
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetCursorAfterTable(int nTableID)
{
  BOOL bResult = ::TerPosAfterTable(GetHWnd(), FALSE, FALSE);

// gerade aktive Tabelle beenden und aus Stack entfernen
  map<int, CTableStackItem>::iterator cIt; 
  cIt = m_lTableStack.find(GetTableID());

  if (cIt != m_lTableStack.end())
  {
    m_lTableStack.erase(cIt);

// die letzte Tabelle aktivieren
    map<int, CTableStackItem>::reverse_iterator cRIt; 
    cRIt = m_lTableStack.rbegin();
    if (cRIt != m_lTableStack.rend())
    {
      SetTableID((*cRIt).second.m_nTableID);
      SetTableRow((*cRIt).second.m_nTableRow);
      SetTableCol((*cRIt).second.m_nTableCol);
    }
  }
  return (bResult != FALSE);
}

inline int CConvertWorkerToRtf::GetTableID()
{
  return m_nTableID;
}

inline bool CConvertWorkerToRtf::SetTableID(int nTableID)
{
  m_nTableID = nTableID;
  return true;
}

inline bool CConvertWorkerToRtf::SetTableRow(int nTableRow)
{ 
	m_nTableRow = _GET_ROW(nTableRow);
	return true;
}

inline bool CConvertWorkerToRtf::SetTableCol(int nTableCol)
{
	m_nTableCol = _GET_COL(nTableCol);
	return true;
}

int CConvertWorkerToRtf::GetTableRow()
{
	return m_nTableRow;
}

int CConvertWorkerToRtf::GetTableCol()
{
	return m_nTableCol;
}

bool CConvertWorkerToRtf::GetRowAndCols(int nTableID, int& wRows, int& wCols)
{
  wRows = ::TerGetRowCellCount(GetHWnd(), TRUE);
  wCols = ::TerGetRowCellCount(GetHWnd(), FALSE);
  //ASSERT(wRows > 0 && wCols > 0);
  return (wRows > 0 && wCols > 0);
}

bool CConvertWorkerToRtf::InsertTableRow(int nTableID, int nInsert, int wLines/*=1*/)
{
  BOOL bRetf = TRUE;
  for (int i = 0; i < wLines && bRetf; i++)
  {
    bRetf = ::TerInsertTableRow(GetHWnd(), nInsert == TAG_TABLE_ROW_APPEND_CURRENT ? TRUE : FALSE, FALSE);
    if(bRetf)
    {
			// first set the default for a new row in the table
      SetTableRowKeep(nTableID, m_nTableRow+1, 1, false);
			SetTableHeaderRow(nTableID, m_nTableRow+1, 1, 0);

			map<int, CTableStackItem>::iterator cIt; 
			cIt = m_lTableStack.find(nTableID);
			if(cIt != m_lTableStack.end())
			{
				int numHeaderRows = (*cIt).second.m_nTableHdrRows;
				if(numHeaderRows)
				{
					int nRowCount = TerGetRowCellCount(GetHWnd(), TRUE);
					if(nRowCount > numHeaderRows)
					{
						(*cIt).second.m_nTableHdrRows = 0;
						SetTableHeaderRow(nTableID, 1, numHeaderRows, 2);
					}
				}
			}

    }
  }
  return (bRetf != FALSE);
}

bool CConvertWorkerToRtf::SetTableCellActivate(int nTableID, int wRow, int wCol, int nPos/*=TAG_POS_END*/)
{
  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);

  if (nPos == TAG_POS_END)
    nPos = POS_END;
  else
    nPos = TAG_POS_BEG;

	if( (wRow-1) < 0)
    wRow = 1;

  if( (wCol-1) < 0)
    wCol = 1;

	/*
	POS_END: since the nested table starts right at the 
	beginning of the parent cell.  So there is no parent cell line to position 
	at.  Also, a cell is guaranteed to have a ending line of its own to contain 
	the cell delimiter line.
	*/

	BOOL bResult = FALSE;
  int CellId = 0; // 0 for default

	// wenn in geschachtelte Zelle geschaltet werden soll, dann müssen wir TAG_POS_END setzen, da wir sonst 
	// nicht in der inneren Tabelle sind
//	if (wNestRow != -2 && wNestCol != -2) 
//		bResult = ::TerPosTable3(GetHWnd(), nTableID, wRow - 1, wCol - 1, TAG_POS_END, FALSE);
//	else	
		bResult = ::TerPosTable3(GetHWnd(), nTableID, wRow - 1, wCol - 1, nPos, FALSE);

  int ParaId, ParaFID, x, y, height;
  DWORD dwflags;
  UINT InfoFlags;
  INT nCurLine = -1; // current line
  bResult &= ::TerGetLineInfo(GetHWnd(), nCurLine, &ParaId, &CellId, &ParaFID, &x, &y, &height, &dwflags, &InfoFlags);

/*
  // in die eingelagerte Tabelle schalten
  if (wNestRow != -2 && wNestCol != -2)
    bResult &= ::TerPosTable3(GetHWnd(), nTableID, wNestRow - 1, wNestCol - 1, nPos, FALSE);
*/

  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetRowHeight(int nTableID, int wRow, int wCol, double fRowHeight, long nPos/*=-1*/)
{
  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol); 
	GetTableSelection(wRow, 0);

	BOOL AllRows = FALSE;
	if(wRow == 0)
		AllRows = TRUE;		

  BOOL bResult = ::TerRowHeight(GetHWnd(), MMToTwips(fRowHeight), AllRows, FALSE);

// eingestellte Zelle wieder zurückstellen
	SetTableCellActivate(nTableID, wRow, wCol, TAG_POS_END);

  return (bResult != FALSE);
}
 
bool CConvertWorkerToRtf::SetCellWidth(int nTableID, int wRow, int wCol, double fWidth, long nPos/*=-1*/)
{
  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);	
  BOOL bResult = ::TerCellWidth(GetHWnd(), GetTableSelection(wRow, wCol), MMToTwips(fWidth), -1, FALSE);
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SetCellBorderColor(int nTableID, int wRow, int wCol, _ColorDef cClrfLeft,
                                             _ColorDef cClrfTop, _ColorDef cClrfRight, _ColorDef cClrfButtom)
{
  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);

  BOOL bResult = ::TerCellBorderColor(GetHWnd(), GetTableSelection(wRow, wCol), cClrfTop, cClrfButtom, cClrfLeft, cClrfRight, FALSE);

  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::CellMargin(int nTableID, int wRow, int wCol, int nLeft, int nTop, int nRight, int nButton)
{
  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);

  int lMargin = nLeft;
  BOOL bResult = ::TerCellWidth(GetHWnd(), GetTableSelection(wRow, wCol), -1, lMargin, FALSE);
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::CellVerticalAlign(int nTableID, int wRow, int wCol, unsigned int nAlign)
{
  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);

  long lVertAlign = CFLAG_VALIGN_BASE;

  switch(nAlign)
  {
  case TAG_TABLE_VERTALIGN_CENTER: // Center alignment
    lVertAlign = CFLAG_VALIGN_CTR; break;
  case TAG_TABLE_VERTALIGN_BOTTOM: // Bottom alignment
    lVertAlign = CFLAG_VALIGN_BOT; break;
  case TAG_TABLE_VERTALIGN_TOP:  // Align base line of the text
    lVertAlign = CFLAG_VALIGN_BASE; break;
  default:
    assert(0);
    return FALSE; break;
  }		

  BOOL bResult = ::TerCellVertAlign(GetHWnd(), GetTableSelection(wRow, wCol), lVertAlign, FALSE);
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::DeleteColumn(int nTableID, int wRow, int wCol)
{
// hier wird nicht geprüft, ob es die Zelle noch gibt?!
	BOOL bResult = ::TerDeleteCells(GetHWnd(), GetTableSelection(wRow, wCol), FALSE);

// wo befindet sich aktuelle col nach löschen
	//INT nColNo, nRowNo, nTableNo;
	//bool bTablePos = ::TerGetTablePos(GetHWnd(), &nTableNo, &nRowNo, &nColNo) == TRUE;

// nach dem löschen einer Zelle wird die Zelle vor der aktuellen aktiv
	return (bResult != FALSE);
}

bool CConvertWorkerToRtf::SplitColumn(int nTableID, int wRow, int wCol)
{
  BOOL bResult = ::TerTableSplitCell(GetHWnd());
  //BOOL bResult = ::SendMessage(GetHWnd(), WM_COMMAND, ID_TABLE_SPLIT_CELL, 0l);
  return (bResult != FALSE);
}

bool CConvertWorkerToRtf::InsertColumn(int nTableID, int wRow, int wCol)
{
  BOOL bResult = ::TerInsertTableCol(GetHWnd(), TRUE, FALSE, FALSE);
	return (bResult != FALSE);
}

void CConvertWorkerToRtf::DetachStatic()
{ 
//	CMailMerge::DetachStatic();
}

// schnelle Routine zum Tabellenkreieren
/*
int PrevCell=0;
int row=0, col=0;
int NewRow, CellId;
CString string;
CHAR RowStr[10];

for ( row=0;row<3000;row++)
{
for ( col=0;col<20;col++)
{
if (col==0) NewRow=TRUE;
else
NewRow=FALSE;
CellId=::TerCreateCellId(GetHWnd(),NewRow,PrevCell,0,0,0,
2000,0,0,0,0,0,1,1,0);
string = CString(itoa(row, RowStr, 10)) + CString((CHAR)CELL_CHAR, 1);   // append cell delimiter to the cell text
::TerAppendTextEx(GetHWnd(),(LPBYTE)(LPCSTR)string,-1,-1,CellId,-1,FALSE);
PrevCell=CellId;
}
string = CString((CHAR)ROW_CHAR, 1);    // insert a row delimiter
::TerAppendTextEx(GetHWnd(),(LPBYTE)(LPCSTR)string,-1,-1,CellId,-1,FALSE);
}
*/
