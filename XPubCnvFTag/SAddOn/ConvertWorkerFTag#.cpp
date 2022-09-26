#if defined(LINUX_OS)
#include <math.h>
#elif defined(WIN32)
//#include <windows.h>
#include <tchar.h>
#include <string.h>
#elif defined(MAC_OS_MWE)
#include <math.h>
#else
#error "not implemented"
#endif

#include <list>
#include <vector>
#include <map>
#include <assert.h>
#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"

#include "XPicture.h" 
 
#include "SAddOnManager/XPubAddOnManager.h"
#include "ConvertWorkerFTag.h"

#define GETPARAM1(tag, val1)    \
  GetParameterValues(cInTag, tag, val1, cParValue[1], cParValue[2], cParValue[3], 1)
#define GETPARAM2(tag, val1, val2)    \
  GetParameterValues(cInTag, tag, val1, val2, cParValue[2], cParValue[3], 2)
#define GETPARAM4(tag, val1, val2, val3, val4)    \
  GetParameterValues(cInTag, tag, val1, val2, val3, val4, 4)
#define GETPARAM(tag, val1)     \
  GetParameterValues(cInTag, tag, val1, cParValue[1], cParValue[2], cParValue[3], 0)

#define CHECK_FONTSIZE(point) (point>0?point:TAG_DEFAULT_CONVERT_FONT_SIZE)
#define CHECK_FONTNAME(name) (name==_XT("")?TAG_DEFAULT_CONVERT_FONT:name)

// für Gültigkeiten bei TAG
#define VALID_TRUE    1
#define VALID_FALSE   2
#define VALID_ERROR   0


// Unter LINUX waren die Namen den folgenden Funktionen
// in SO mit verrueckten Namen. Ich habe nur diese Moeglichkeit gefunden,
// dass die Namen in SO unveraendert sind.
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
extern "C"
{
#endif

#if defined(MAC_OS_MWE)
#pragma export on
#endif

CConvertFromAddOn* RegisterAddOnManager(CXPubAddOnManager* pManager);
bool UnregisterAddOnManager(CConvertFromAddOn* pCnvAddOn);
int GetCnvFromAddOnVersion();
CConvertWorkerFrom* CreateConvertWorkerFrom_(CConvertFromAddOn* pCnvAddOn);
CConvertWorkerFrom* CreateConvertWorkerFromClone_(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker);
bool ReleaseConvertWorkerFrom_(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker);
                          
#if defined(MAC_OS_MWE)
#pragma export off
#endif

CConvertFromAddOn* RegisterAddOnManager(CXPubAddOnManager* pManager)
{
  CConvertFromAddOn* pNew = new CConvertFromTagAddOn(pManager);
  if (!pNew)
    throw(ERR_NEW_PROBLEM);
  return pNew;
}

bool UnregisterAddOnManager(CConvertFromAddOn* pCnvAddOn)
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

int GetCnvFromAddOnVersion()
{
  return ConvertWorkerFTag_Version;
}

CConvertWorkerFrom* CreateConvertWorkerFrom_(CConvertFromAddOn* pCnvAddOn)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerFrom();
}

CConvertWorkerFrom* CreateConvertWorkerFromClone_(CConvertFromAddOn* pCnvAddOn,
                                                  CConvertWorkerFrom* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKERCLONE_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerFromClone(pConvertWorker);
}

bool ReleaseConvertWorkerFrom_(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_RELEASEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->ReleaseConvertWorkerFrom(pConvertWorker);
}

#if defined(LINUX_OS) || defined(MAC_OS_MWE)
}
#endif









xtstring CConvertWorkerFromTag::m_sTxtFrame = _XT("");
int CConvertWorkerFromTag::m_nTxtFrame = 0;
TagVariableList CConvertWorkerFromTag::m_TagVarList;
TagStringList CConvertWorkerFromTag::m_TagOversetList;
TagStringList CConvertWorkerFromTag::m_TagLibraryList;
TagStringList CConvertWorkerFromTag::m_TagPDFTableList;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CConvertFromTagAddOn::CConvertFromTagAddOn(CXPubAddOnManager* pAddOnManager)
                     :CConvertFromAddOn(pAddOnManager)
{
  

}

CConvertFromTagAddOn::~CConvertFromTagAddOn()
{
}

CConvertWorkerFrom* CConvertFromTagAddOn::CreateConvertWorkerFrom()
{
  CConvertWorkerFrom* pWorker = new CConvertWorkerFromTag(this);
  if (!pWorker)
  {
    throw(ERR_ADDON_NEWWORKER_NOT_CREATED);
    return pWorker;
  }

  if (AddConvertWorkerFrom(pWorker))
    return pWorker;

  delete pWorker;
  pWorker = NULL;
  throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);
  return pWorker;
}

CConvertWorkerFrom* CConvertFromTagAddOn::CreateConvertWorkerFromClone(CConvertWorkerFrom* pConvertWorker)
{
  if (!pConvertWorker)
  {
    throw(ERR_ADDON_CREATEWORKERCLONE_FROM_NULL_POINTER);
    return NULL;
  }

  CConvertWorkerFromTag* pWorker = (CConvertWorkerFromTag*)((CConvertWorkerFromTag*)pConvertWorker)->Clone();
  if (AddConvertWorkerFrom(pWorker))
    return pWorker;

  delete pWorker;
  pWorker = NULL;
  throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);
  return pWorker;
}

bool CConvertFromTagAddOn::ReleaseConvertWorkerFrom(CConvertWorkerFrom* pWorker)
{
  if (!pWorker)
  {
    throw(ERR_ADDON_RELEASEWORKER_FROM_NULL_POINTER);
    return false;
  }

  if (!RemoveConvertWorkerFrom(pWorker))
    return false;

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CHECKCALLPOINTER()   assert(pCW);\
                        if (!pCW)\
                          return false;\
                        assert(pCW->m_pConvertTo);\
                        if (!pCW->m_pConvertTo)\
                          return false;

typedef bool (*pProcessFkt) (CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken);

typedef struct
{
  XTCHAR* szTag;      // zu suchendes Element
  TTypeOfCase nCase;
  pProcessFkt pAnFkt;
} _TagTokens;

static _TagTokens _cTagTokens[]= 
{
  // Tabelle
  { TAG_TABLE,    tCASE_TABLE,      CConvertWorkerFromTag::Table },
  { TAG_ENDTABLE, tCASE_TABLE_END,  CConvertWorkerFromTag::Table },

// Paragraph
  { TAG_PARAGRAPHBREAK,   tCASE_PARABREAK,          CConvertWorkerFromTag::Paragraph },
  { TAG_LINEBREAK,        tCASE_LINE,               CConvertWorkerFromTag::Paragraph },
  { TAG_PARAGRAPH,        tCASE_PARAGRAPH,          CConvertWorkerFromTag::Paragraph },
  { TAG_PARA_PAGEBREAK,   tCASE_PARAGRAPH_PAGEBREAK,CConvertWorkerFromTag::Paragraph },
  { TAG_BEGINBODY,        tCASE_NONE,               CConvertWorkerFromTag::BeginPageBody },

// Grafik
  { TAG_IMAGE_SYMBOL,    tCASE_GRAFIC_NORMAL,    CConvertWorkerFromTag::Image },
  { TAG_IMAGE,           tCASE_GRAFIC_BAUSTEIN,  CConvertWorkerFromTag::Image },
        
// Font
  { TAG_FONT,          tCASE_FONT,              CConvertWorkerFromTag::Font },
  { TAG_FONT_END,      tCASE_FONT_END,          CConvertWorkerFromTag::Font },

// Paragraph Style
  { TAG_STYLE,          tCASE_STYLE,                CConvertWorkerFromTag::Styles },
  { TAG_STYLE_END,      tCASE_STYLE_END,            CConvertWorkerFromTag::Styles },

// Format
  { TAG_FONT_BOLD,          tCASE_BOLD,             CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_BOLD_END,      tCASE_BOLD_END,         CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_ITALIC,        tCASE_ITALIC,           CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_ITALIC_END,    tCASE_ITALIC_END,       CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_UNDERLINE,     tCASE_UNDERLINE,        CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_UNDERLINE_END, tCASE_UNDERLINE_END,    CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_UL_DOUBLE,     tCASE_UL_DOUBLE,        CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_UL_DOUBLE_END, tCASE_UL_DOUBLE_END,    CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_UL_STRIKE,     tCASE_STRIKE,           CConvertWorkerFromTag::FontAttribute },
  { TAG_FONT_UL_STRIKE_END, tCASE_STRIKE_END,       CConvertWorkerFromTag::FontAttribute },
  
// Tabulator
  { TAG_TAB_DEFAULT,  tCASE_TABDEFAULTS,  CConvertWorkerFromTag::Tabulator },
  { TAG_TAB,          tCASE_TAB,          CConvertWorkerFromTag::Tabulator },
  { TAG_TAB_SIMPLE,   tCASE_TABSIMPLE,    CConvertWorkerFromTag::Tabulator },
  
// Umschaltung in Kopf/Fussbereiche
  { TAG_HEADER,       tCASE_HEADER,       CConvertWorkerFromTag::SwitchHeaderFooter },
  { TAG_HEADER_END,   tCASE_HEADER_END,   CConvertWorkerFromTag::SwitchHeaderFooter },
  { TAG_FOOTER,       tCASE_FOOTER,       CConvertWorkerFromTag::SwitchHeaderFooter },
  { TAG_FOOTER_END,   tCASE_FOOTER_END,   CConvertWorkerFromTag::SwitchHeaderFooter },

// Kopf/Fuß Abstand vom Blattrand
  { TAG_FOOTER_MARGIN,  tCASE_FOOTER_MARGIN,  CConvertWorkerFromTag::SetHeaderFooterPos },
  { TAG_HEADER_MARGIN,  tCASE_HEADER_MARGIN, CConvertWorkerFromTag::SetHeaderFooterPos },

// Dokumentsprungfunktionen
  { TAG_ENDPOS,  tCASE_CURSOR_END,      CConvertWorkerFromTag::CursorPosition },
  { TAG_HOMEPOS,  tCASE_CURSOR_HOME,    CConvertWorkerFromTag::CursorPosition },

// Checkbox
  { TAG_CHECKBOX, tCASE_NONE,  CConvertWorkerFromTag::Checkbox },

// Box
  { TAG_BOX,    tCASE_BOX,    CConvertWorkerFromTag::Box },
  { TAG_BOXEND, tCASE_BOXEND, CConvertWorkerFromTag::Box },

// Positionsrahmen
  { TAG_TEXTFRAME_BREAK, tCASE_POSFRAME_BREAK,  CConvertWorkerFromTag::TextFrame },
  { TAG_POSFRAME_BEGIN,  tCASE_POSFRAME,        CConvertWorkerFromTag::TextFrame },
  { TAG_POSFRAME_END,    tCASE_POSFRAME_END,    CConvertWorkerFromTag::TextFrame },
  { TAG_TEXTFRAME_BEGIN, tCASE_POSFRAME,        CConvertWorkerFromTag::TextFrame },
  { TAG_TEXTFRAME_END,   tCASE_POSFRAME_END,    CConvertWorkerFromTag::TextFrame },

// linie
  { TAG_LINE,    tCASE_NONE,  CConvertWorkerFromTag::Line },

// Template
  { TAG_TEMPLATE,    tCASE_NONE,  CConvertWorkerFromTag::LoadTemplate },

// Dokument
  { TAG_PAGE_MARGINS,       tCASE_NONE, CConvertWorkerFromTag::PageMargin },
  { TAG_RTF_DOC_SETTINGS,   tCASE_NONE, CConvertWorkerFromTag::DocumentInfo },
  { TAG_DOCUMENT,           tCASE_NONE, CConvertWorkerFromTag::Document },

// Gesamtseitenzahl, aktuelle Seite über ein Feld ausgeben
  { TAG_PAGES,      tCASE_NONE,  CConvertWorkerFromTag::SetPageCount },
  { TAG_PAGENUMBER, tCASE_NONE,  CConvertWorkerFromTag::SetCurrentPage },

// Seite/Abschnitt umbrechen
  { TAG_PAGEBREAK,      tCASE_PAGEBREAK,            CConvertWorkerFromTag::InsertPageBreak },

  { TAG_FILLPAGEBREAK, tCASE_FILLPAGEBREAK,  CConvertWorkerFromTag::InsertPageBreak },
  { TAG_GOWRITEPOSITION, tCASE_NONE,  CConvertWorkerFromTag::GoWritePosition },

  { TAG_SECTIONBREAK, tCASE_NONE,  CConvertWorkerFromTag::InsertSectionBreak },
  { TAG_COLUMNBREAK,  tCASE_NONE,  CConvertWorkerFromTag::InsertColumnBreak },
  { TAG_SECTION,      tCASE_NONE,  CConvertWorkerFromTag::EditSection },

// Kannseite
  { TAG_NOPAGEBREAK,      tCASE_NOPAGEBREAK,      CConvertWorkerFromTag::CanPageBreak },
  { TAG_NOPAGEBREAK_END,  tCASE_NOPAGEBREAK_END,  CConvertWorkerFromTag::CanPageBreak },
  
// ein einfaches Seriendruckfeld
  { TAG_MERGEFIELD, tCASE_NONE,       CConvertWorkerFromTag::MergeField },
  { TAG_FIELD, tCASE_NONE,            CConvertWorkerFromTag::Field },
  { TAG_FORMFIELD, tCASE_NONE,        CConvertWorkerFromTag::FormField },

// Kennwort der PDF-Datei
  { TAG_PDF_PASSWORD, tCASE_NONE,     CConvertWorkerFromTag::Password },

// Layer
  { TAG_LAYER_BEGIN, tCASE_LAYER,     CConvertWorkerFromTag::Layer },
  { TAG_LAYER_END, tCASE_LAYER_END,   CConvertWorkerFromTag::Layer },

// Lesezeichen
  { TAG_BOOKMARK, tCASE_NONE,  CConvertWorkerFromTag::InsertBookmark },
  { TAG_PDF_LINK_BEGIN,     tCASE_LINK,     CConvertWorkerFromTag::InsertLink },
  { TAG_PDF_LINK_END,       tCASE_LINK_END, CConvertWorkerFromTag::InsertLink },
  { TAG_ANCHOR_BEGIN,       tCASE_NONE,     CConvertWorkerFromTag::InsertAnchor },

// Pfadsuche bei Template & Images
  { TAG_SEARCHPATH,       tCASE_NONE,     CConvertWorkerFromTag::RootPath },

  { TAG_METRICS,     tCASE_NONE,     CConvertWorkerFromTag::Metrics },
  { TAG_CLASS,       tCASE_NONE,     CConvertWorkerFromTag::ClassName },

// Library liegt in ProcessTags
  //{ TAG_LIBRARY,      tCASE_NONE,       CConvertWorkerFromTag::Library },
 
};

CConvertWorkerFromTag::CConvertWorkerFromTag(CConvertFromTagAddOn* pAddOn)
                      :CConvertWorkerFrom(tDataFormatST_Tag)
{
  m_TagVarList.clear();
  m_TagOversetList.clear();
  m_TagLibraryList.clear();
  m_TagPDFTableList.clear();
  m_pAddOn = pAddOn;

  m_nWorkModus = tWM_Work;
  m_nActiveUnitOfLength = tAUOL_Default;
}

CConvertWorkerFromTag::~CConvertWorkerFromTag()
{
}

CConvertWorkerFrom* CConvertWorkerFromTag::Clone()
{
  CConvertWorkerFromTag* pNew = new CConvertWorkerFromTag(m_pAddOn);
  if (!pNew)
  {
    throw(ERR_ADDON_NEWWORKER_FOR_CLONE_NOT_CREATED);
    return NULL;
  }

  return pNew;
}

_ColorDef CConvertWorkerFromTag::GetColor(xtstring& sColorName, COLORDEF clrfDefault /*= TAG_DEFAULT_BLACK*/)
{
  _ColorDef ColorType;

  // RGB-Wert wurde gesetzt
  int nFieldCnt = sColorName.GetFieldCount(_XT(","));

  if(nFieldCnt == 3 && sColorName.FindNoCase(TAG_RGB) != xtstring::npos)
  {
		sColorName = sColorName.Mid(strlen(TAG_RGB));    

		ColorType.ct = tColorRGB;
		ColorType.rgb.r = atol(sColorName.GetField(_XT(","),0).c_str());
		ColorType.rgb.g = atol(sColorName.GetField(_XT(","),1).c_str());
		ColorType.rgb.b = atol(sColorName.GetField(_XT(","),2).c_str());
		return ColorType;
  }
  if(nFieldCnt == 4 && sColorName.FindNoCase(TAG_CMYK) != xtstring::npos)
  {
		sColorName = sColorName.Mid(strlen(TAG_CMYK));    

		ColorType.ct = tColorCMYK;
		ColorType.cmyk.c = atof(sColorName.GetField(_XT(","),0).c_str());
		ColorType.cmyk.m = atof(sColorName.GetField(_XT(","),1).c_str());
		ColorType.cmyk.y = atof(sColorName.GetField(_XT(","),2).c_str());
		ColorType.cmyk.k = atof(sColorName.GetField(_XT(","),3).c_str());
		return ColorType;
  } 

  // trim left  
  while (sColorName.size()
          && (sColorName.at(0) == ' '
              || sColorName.at(0) == '\t'))
    sColorName.erase(sColorName.begin(), sColorName.begin() + 1);

  for (int i = 0; i < sizeof(_crColours) / sizeof(_ColourTableEntry); i++)
  {
    if (_tcsnicmp(sColorName.c_str(), _crColours[i].szEN_Name, sColorName.size()) == 0 ||
        _tcsnicmp(sColorName.c_str(), _crColours[i].szGE_Name, sColorName.size()) == 0)
		{
			ColorType.ct = tColorRGB;
      ColorType.rgb.r = GetRValue(_crColours[i].crRGB);
      ColorType.rgb.g = GetGValue(_crColours[i].crRGB);
      ColorType.rgb.b = GetBValue(_crColours[i].crRGB);
      return ColorType;
		}
  }
  return ColorType;
}

void CConvertWorkerFromTag::CleanUpTAGStreamBefore(xtstring& sDataStream)
{
// wir dürfen das Ersetzen erst nach der Verarbeitung der Tokens machen
  sDataStream.ReplaceAll(_XT("\\<"), _XT("<"));
  sDataStream.ReplaceAll(_XT("\\>"), _XT(">"));
  sDataStream.ReplaceAll(_XT("\\\""), _XT("\""));
}

void CConvertWorkerFromTag::CleanUpTAGStreamAfter(xtstring& sDataStream)
{

  sDataStream.ReplaceAll(_XT("&lt;"), _XT("<"));
  sDataStream.ReplaceAll(_XT("&gt;"), _XT(">"));
  sDataStream.ReplaceAll(_XT("&amp;"), _XT("&"));
  sDataStream.ReplaceAll(_XT("&apos;"), _XT("'"));
  sDataStream.ReplaceAll(_XT("&quot;"), _XT("\""));

  sDataStream.ReplaceAll(_XT("&#223;"), _XT("ß"));
  sDataStream.ReplaceAll(_XT("&#214;"), _XT("Ö"));
  sDataStream.ReplaceAll(_XT("&#196;"), _XT("Ä"));
  sDataStream.ReplaceAll(_XT("&#228;"), _XT("ä"));
  sDataStream.ReplaceAll(_XT("&#246;"), _XT("ö"));
  sDataStream.ReplaceAll(_XT("&#220;"), _XT("Ü"));
  sDataStream.ReplaceAll(_XT("&#252;"), _XT("ü"));

  sDataStream.ReplaceAll(_XT("\\n"), _XT(""));
  sDataStream.ReplaceAll(_XT("\\t"), _XT(""));
  sDataStream.ReplaceAll(_XT("\x0a"), _XT(""));
  sDataStream.ReplaceAll(_XT("\x0d"), _XT(""));
  sDataStream.ReplaceAll(_XT("\x09"), _XT(""));

}

bool CConvertWorkerFromTag::ConvertAndAdd(const xtstring& sDataStream,
                                          const xtstring& sXPubConst,
                                          TWorkPaperCopyType eWorkPaperCopyType)
{
  m_bOnlyFreewareKeywordsUsed = true;
// MAK
// Der Kannseitenumbruch wird normalerweise am Ende eines Bausteines getestet.
// Für das Testen im Testanwendung ist es aber besser dies in Dialog zuzulassen
#define xCHECK_CANPAGEBREAK

  assert(m_pConvertTo);
  if (!m_pConvertTo)
    return false;

  xtstring sStream, sInsParStream;
  sStream = sDataStream;
  CleanUpTAGStreamBefore(sStream);

  m_pConvertTo->PositionCursor(sXPubConst, eWorkPaperCopyType);

  xtstring sText;
  size_t nLength = sStream.size();
  if (m_pConvertProgress)
    m_pConvertProgress->StartProgress();

  for (int nI = 0; nI < 2; nI++)
  {
    size_t nPrLineCount = 0;
    // nI == 0 --> tWM_PreProcess
    // nI == 1 --> tWM_Work
    if (nI == 0)
    {
      m_nWorkModus = tWM_PreProcess;
      m_pConvertTo->StyleEvent(tSE_StartPreProcess, 0);
    }
    else if (nI == 1)
    {
      m_nWorkModus = tWM_Work;
      m_pConvertTo->StyleEvent(tSE_StartWork, 0);
    }
    else
    {
      assert(false);
      break;
    }
    size_t nIndex;
    for (nIndex = 0; nIndex < nLength; nIndex++)
    {
      // wenn "<<" dann "<" ausgeben.
      if (sStream[nIndex] == _XT('\\') && sStream[nIndex+1] == _XT('<'))
      { 
        nPrLineCount+=2; ++nIndex;
      }      
      else if (sStream[nIndex] == _XT('<'))
      {
        // aufgelaufenes verarbeiten  
        if (nPrLineCount)
        {
          sInsParStream = sStream.Mid(nIndex - nPrLineCount, nPrLineCount);
          CleanUpTAGStreamAfter(sInsParStream);
          if (m_nWorkModus == tWM_Work && sInsParStream.size())
          {
            if (m_bRestrictFeatures)
            {
              xtstring sNewStream;
              for (size_t nI = 0; nI < sInsParStream.size(); nI++)
              {
                if (nI % 3 == 2)
                  sNewStream += _XT('-');
                else
                  sNewStream += sInsParStream[nI];
              }
              if (!m_pConvertTo->InsertTextFromMemory(sNewStream, CNV_TEXTFORMAT_TEXT))
                return false;
            }
            else
            {
              if (!m_pConvertTo->InsertTextFromMemory(sInsParStream, CNV_TEXTFORMAT_TEXT))
                return false;
            }
          }
          nPrLineCount = 0;
#ifdef CHECK_CANPAGEBREAK
          m_pConvertTo->CheckCanPageBreak();
#endif
        }

        // Ende des Tags suchen und nIndex erhöhen  
        size_t nEndPos = sStream.find(TAG_CLOSETAG, nIndex);
        if (nEndPos != xtstring::npos) 
        {
          bool bError = false;  

          xtstring sInStream = sStream.Mid(nIndex, nEndPos - nIndex + 1);    
          // in zu verarbeitetendem TAG darf kein Beginntag kommen 
          size_t nInTagPos = sInStream.find(TAG_BEGINTAG, 1);
          // wenn Beginntag gefunden, dann Text einfach ausgeben
          if (nInTagPos != xtstring::npos) 
            bError = true;  

          if (m_nWorkModus == tWM_Work && bError)
          {
            if (m_bRestrictFeatures)
            {
              xtstring sNewStream;
              for (size_t nI = 0; nI < sInsParStream.size(); nI++)
              {
                if (nI % 3 == 2)
                  sNewStream += _XT('-');
                else
                  sNewStream += sInsParStream[nI];
              }
              m_pConvertTo->InsertTextFromMemory(sNewStream, CNV_TEXTFORMAT_TEXT);
            }
            else
            {
              m_pConvertTo->InsertTextFromMemory(sInStream, CNV_TEXTFORMAT_TEXT);
            }
          }

          // Tag verarbeiten  
          if(!bError && !ProcessTags(sInStream))
          {
            xtstring sToken = sStream.Mid(nIndex, nEndPos - nIndex + 1);
          // Library: Tagbereich wird in Variable abgelegt

            // mit nIndex über Tag positionieren  
            nIndex += nEndPos-nIndex; 

            if(_tcsnicmp(sToken.c_str(), TAG_LIBRARY, strlen(TAG_LIBRARY)) == 0)
            {
              size_t nLibEndPos = sStream.find(TAG_LIBRARY_END, nIndex);
              if(nLibEndPos != -1) 
              {
                xtstring sValueToken = sStream.Mid(nIndex + 1, (nLibEndPos - 1) - nIndex);
                Library(this, sToken, sValueToken);
                nEndPos = nLibEndPos + strlen(TAG_LIBRARY_END) - 1;
              }
              else
                Library(this, sToken, xtstring(_XT("")));
            }
          }

          // mit nIndex über Tag positionieren  
          nIndex += nEndPos-nIndex; 
          if (m_pConvertProgress)
          {
            // progress ist von 0 bis 1000
            if (nI == 0)
              m_pConvertProgress->StepProgress((int)(nIndex * 500 / nLength));
            else
              m_pConvertProgress->StepProgress((int)(500 + nIndex * 500 / nLength));
          }

#ifdef CHECK_CANPAGEBREAK
          m_pConvertTo->CheckCanPageBreak();
#endif
        }
      }
      else
        nPrLineCount++;
    }
    // Rest verarbeiten 
    if (m_nWorkModus == tWM_Work && nPrLineCount)
    {
      sInsParStream = sStream.Mid(nIndex - nPrLineCount, nPrLineCount);
      CleanUpTAGStreamAfter(sInsParStream);
      {
        if (m_bRestrictFeatures)
        {
          xtstring sNewStream;
          for (size_t nI = 0; nI < sInsParStream.size(); nI++)
          {
            if (nI % 3 == 2)
              sNewStream += _XT('-');
            else
              sNewStream += sInsParStream[nI];
          }
          if (!m_pConvertTo->InsertTextFromMemory(sNewStream, CNV_TEXTFORMAT_TEXT))
            return false;
        }
        else
        {
          if (!m_pConvertTo->InsertTextFromMemory(sInsParStream, CNV_TEXTFORMAT_TEXT))
            return false;
        }
      }
    }
  }
  if (m_pConvertProgress)
    m_pConvertProgress->EndProgress();

// Kannseitenumbruch kontrollieren
#ifdef CHECK_CANPAGEBREAK
  m_pConvertTo->CheckCanPageBreak();
#endif

  return true;
}

bool CConvertWorkerFromTag::ProcessTags(const xtstring& cTag)
{
  assert(m_pConvertTo);
  if (!m_pConvertTo)
    return false;

  bool bRetf = false;

  for (int j = 0; j < sizeof(_cTagTokens) / sizeof(_TagTokens); j++)
  {
    // die Elemente mit Wert haben im Array kein ">"
    xtstring cElement = _cTagTokens[j].szTag;
    size_t nValElem = cElement.find(TAG_CLOSETAG);

    int cCmp = _tcsnicmp(cTag.c_str(), cElement.c_str(), cElement.size());

    if (cCmp == 0 && nValElem != -1)
    {
      if (_cTagTokens[j].pAnFkt != 0)
      {
        //TRACE("%s\n", cTag);
        bRetf = _cTagTokens[j].pAnFkt(this, cElement, &_cTagTokens[j]);
      }
      break;
    }
    if (cCmp == 0 && nValElem == -1)
    {
      if (_cTagTokens[j].pAnFkt != 0)
        bRetf = _cTagTokens[j].pAnFkt(this, cTag, &_cTagTokens[j]);
      break;
    }
  }
  return bRetf;
}

bool CConvertWorkerFromTag::Library(CConvertWorkerFromTag* pCW, const xtstring& cInTag, const xtstring& sStream)
{
  CHECKCALLPOINTER()

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (m_pKeywords && m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_LIBRARY;
    CheckKeyword(sKeyword);
  }

  if (m_nWorkModus != tWM_Work)
    return true;

  xtstring cParValue[4];
  xtstring sLibraryName;

  if (cInTag.FindNoCase(TAG_LIBRARY_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (m_pKeywords && m_bOnlyFreewareKeywordsUsed)
      CheckParameter(sKeyword, TAG_LIBRARY_NAME);
    GETPARAM1(TAG_LIBRARY_NAME, cParValue[0]);
    sLibraryName = cParValue[0];
    if(!sStream.empty())
      m_TagLibraryList[sLibraryName] = sStream;
  }

  int nMode = 0;
  if (cInTag.FindNoCase(TAG_LIBRARY_MODE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (m_pKeywords && m_bOnlyFreewareKeywordsUsed)
      CheckParameter(sKeyword, TAG_LIBRARY_MODE);
    GETPARAM1(TAG_LIBRARY_MODE, cParValue[0]);
    nMode = atoi(cParValue[0].c_str()); 
   
    // bedingt die übergelaufenen Felder ausgeben
    if (sLibraryName == _XT("OvTableIf") && m_TagOversetList.size()) // OvTableLine
    {
    // der Kopf, OvTableBegin
      ConvertAndAdd(m_TagLibraryList[xtstring(_XT("OvTableBegin"))], _XT(""), tCopyType_ActualPosition);

    // die einzelnen Zeilen
      map<xtstring, xtstring>::iterator cIt; 
      cIt = m_TagOversetList.begin(); 
      for (; cIt != m_TagOversetList.end(); ++cIt)
      {
        xtstring sLineData, sOutData;
        sLineData = m_TagLibraryList[xtstring(_XT("OvTableLine"))];
        sOutData.Format(sLineData.c_str(), ((*cIt).first).c_str(), ((*cIt).second).c_str());
        ConvertAndAdd(sOutData, _XT(""), tCopyType_ActualPosition);
      }
    // der Rumpf
      ConvertAndAdd(m_TagLibraryList[xtstring(_XT("OvTableEnd"))], _XT(""), tCopyType_ActualPosition);

    }
    else if(nMode && !sLibraryName.empty())
    { 
      ConvertAndAdd(m_TagLibraryList[sLibraryName], _XT(""), tCopyType_ActualPosition);
    }
  }

  return true;

}

bool CConvertWorkerFromTag::BeginPageBody(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_BEGINBODY;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring cParValue[4];
  int nPos = 0;
  if (cInTag.FindNoCase(TAG_BEGINBODY_POS) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_BEGINBODY_POS);
    GETPARAM1(TAG_BEGINBODY_POS, cParValue[0]);
    nPos = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }
  pCW->m_pConvertTo->BeginPageBody(nPos);

  return true;

}

bool CConvertWorkerFromTag::Paragraph(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_PARABREAK)
      sKeyword = TAG_PARAGRAPHBREAK;
    else if (lpLitToken->nCase == tCASE_LINE)
      sKeyword = TAG_LINEBREAK;
    else if (lpLitToken->nCase == tCASE_PARAGRAPH)
      sKeyword = TAG_PARAGRAPH;
    else if (lpLitToken->nCase == tCASE_PARAGRAPH_PAGEBREAK)
      sKeyword = TAG_PARA_PAGEBREAK;
    pCW->CheckKeyword(sKeyword);
  }

  xtstring cParValue[4];

  if (pCW->m_nWorkModus == tWM_Work
      && lpLitToken->nCase == tCASE_PARABREAK)
  {
    pCW->m_pConvertTo->InsertParagraph();
  //  SetDefaults(pCW);
    return true;
  }
     
  if (pCW->m_nWorkModus == tWM_Work
      && lpLitToken->nCase == tCASE_LINE)
  {
    return pCW->m_pConvertTo->InsertLine();
  }

  if (lpLitToken->nCase == tCASE_PARAGRAPH)
  {
    if (cInTag.FindNoCase(TAG_PARA_PAGEBREAK) != xtstring::npos)
    {
      GETPARAM1(TAG_PARA_PAGEBREAK, cParValue[0]);
      int nPageBreakBefore = atol(cParValue[0].c_str());
      if (pCW->m_nWorkModus == tWM_Work)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_PARA_PAGEBREAK);

        pCW->m_cStyleManager.SetPageBreakBefore(nPageBreakBefore);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        pCW->m_pConvertTo->ParagraphPageBreak(nPageBreakBefore);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
        pCW->m_cStyleManager.GetActualStyleToSet()->SetPageBreakBefore(nPageBreakBefore);
    }

    if (cInTag.FindNoCase(TAG_PARA_KEEPNEXT) != xtstring::npos)
    {
      GETPARAM1(TAG_PARA_KEEPNEXT, cParValue[0]);
      int nKeepNext = atol(cParValue[0].c_str());
      if (pCW->m_nWorkModus == tWM_Work)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_PARA_KEEPNEXT);

        pCW->m_cStyleManager.SetParagraphKeepNext(nKeepNext);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        pCW->m_pConvertTo->SetParagraphControl(-1, nKeepNext, -1);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
        pCW->m_cStyleManager.GetActualStyleToSet()->SetParagraphControl(-1, nKeepNext, -1);
    }

    if (cInTag.FindNoCase(TAG_PARA_KEEP) != xtstring::npos)
    {
      GETPARAM1(TAG_PARA_KEEP, cParValue[0]);
      int nKeep = atol(cParValue[0].c_str());
      if (pCW->m_nWorkModus == tWM_Work)
      {
       // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_PARA_KEEP);

        pCW->m_cStyleManager.SetParagraphKeep(nKeep);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        pCW->m_pConvertTo->SetParagraphControl(nKeep,-1,-1);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
        pCW->m_cStyleManager.GetActualStyleToSet()->SetParagraphControl(nKeep, -1, -1);
    }

    if (cInTag.FindNoCase(TAG_PARA_WIDOW) != xtstring::npos)
    {
      GETPARAM1(TAG_PARA_WIDOW, cParValue[0]);
      int nWidow = atol(cParValue[0].c_str());
      if (pCW->m_nWorkModus == tWM_Work)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_PARA_WIDOW);

        pCW->m_cStyleManager.SetParagraphWindowOrphan(nWidow);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        pCW->m_pConvertTo->SetParagraphControl(-1, -1, nWidow);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
        pCW->m_cStyleManager.GetActualStyleToSet()->SetParagraphControl(-1, -1, nWidow);
    }

    // Textausrichtung
    Alignment(pCW, sKeyword, cInTag, lpToken);

    int nLeft = -1;
    if (cInTag.FindNoCase(TAG_PARA_LEFT) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_PARA_LEFT);

      GETPARAM1(TAG_PARA_LEFT, cParValue[0]);
      nLeft = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

    int nRight = -1;
    if (cInTag.FindNoCase(TAG_PARA_RIGHT) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_PARA_RIGHT);

      GETPARAM1(TAG_PARA_RIGHT, cParValue[0]);
      nRight = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

    int nIdent = -1;
    if (cInTag.FindNoCase(TAG_PARA_IDENT) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_PARA_IDENT);

      GETPARAM1(TAG_PARA_IDENT, cParValue[0]);
      nIdent = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

    if (nLeft != -1 || nRight != -1 || nIdent != -1)
    {
      if (pCW->m_nWorkModus == tWM_Work)
      {
        pCW->m_cStyleManager.SetParagraphIndents(nLeft, nRight, nIdent);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        pCW->m_pConvertTo->SetParagraphIndents(nLeft, nRight, nIdent);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess
              && pCW->m_cStyleManager.GetActualStyleToSet())
        pCW->m_cStyleManager.GetActualStyleToSet()->SetParagraphIndents(nLeft, nRight,  nIdent);
    }

// Abstände vor und nach dem Absatz
    int nSpaceBefore = 0;
    if ( cInTag.FindNoCase(TAG_PARA_SPACE_BEFORE) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_PARA_SPACE_BEFORE);
      GETPARAM1(TAG_PARA_SPACE_BEFORE, cParValue[0]);
      nSpaceBefore = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

    int nSpaceAfter = 0;
    if ( cInTag.FindNoCase(TAG_PARA_SPACE_AFTER) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_PARA_SPACE_AFTER);
      GETPARAM1(TAG_PARA_SPACE_AFTER, cParValue[0]);
      nSpaceAfter = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }
    int nSpaceBetween = 0;
    if ( cInTag.FindNoCase(TAG_PARA_SPACE_LINE) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_PARA_SPACE_LINE);
      GETPARAM1(TAG_PARA_SPACE_LINE, cParValue[0]);
      nSpaceBetween = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

    int nLineSpacing = 0;
    // Wenn die Kontrolle da ist, dann kann ich nie die Werte auf 0 setzen.
    //if (nSpaceBefore != 0 || nSpaceAfter != 0 || nSpaceBetween != 0)
    if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
      pCW->m_pConvertTo->SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
    }
    else if (pCW->m_nWorkModus == tWM_StyleInPreProcess
            && pCW->m_cStyleManager.GetActualStyleToSet())
      pCW->m_cStyleManager.GetActualStyleToSet()->SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
  } 
  return true;
}

bool CConvertWorkerFromTag::SetDefaults(CConvertWorkerFromTag* pCW)
{
  CHECKCALLPOINTER()
 
  pCW->m_pConvertTo->SetDefaults();
  return true;
}
 
bool CConvertWorkerFromTag::Image(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{ 
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_GRAFIC_NORMAL)
      sKeyword = TAG_IMAGE_SYMBOL;
    else if (lpLitToken->nCase == tCASE_GRAFIC_BAUSTEIN)
      sKeyword = TAG_IMAGE;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // Grafikbaustein über INI-Datei ausgeben
  if (lpLitToken->nCase == tCASE_GRAFIC_NORMAL)
  {
    xtstring cSymbol = cInTag.Mid(1, _tcslen(lpLitToken->szTag));
    xtstring cImageName = pCW->GetKeyStrFromIni(xtstring(_XT("SYMBOLS")), cSymbol, TAG_IMAGE_IC_INI);

    if (cImageName.size() > 0)
    {
      xtstring sPath;
      pCW->GetModulePath(sPath);
      sPath += cImageName;

      xtstring cTempPath;
      xtstring sType(_XT(".png"));
      if( pCW->GetTemporaryFileName(sPath, cTempPath, sType) )
      {
        CXPubPicture cPicture;
        cPicture.Resize(sPath, cTempPath, 100);
      }
      if(pCW->IsPathFile(cTempPath.c_str()))   
        pCW->m_pConvertTo->InsertImage(cTempPath, -1, -1, 100, false);
      
      return true;
    }
  } 
  else if (lpLitToken->nCase == tCASE_GRAFIC_BAUSTEIN)
  {

    xtstring cParValue[4];
    xtstring sPath;

    xtstring cGraficName;
    if ( cInTag.FindNoCase(TAG_IMAGE_NAME) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_IMAGE_NAME);
      GETPARAM1(TAG_IMAGE_NAME, cParValue[0]);
      cGraficName = cParValue[0];
    }
    if(cGraficName.size() <= 0)
      return false;
 
    sPath = cGraficName;
  // check if image file exist
    if(!pCW->IsPathFile(sPath.c_str()))
    {
      xtstring sRootPicturePath;
      pCW->GetRootPicturePath(sRootPicturePath);
      if (sRootPicturePath.size())
        sPath = sRootPicturePath + cGraficName;
      if(!pCW->IsPathFile(sPath.c_str()))
        return false;
    }

    bool bShrink = false;
    if ( cInTag.FindNoCase(TAG_IMAGE_SHRINK) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_IMAGE_SHRINK);
      GETPARAM1(TAG_IMAGE_SHRINK, cParValue[0]);
      bShrink = cParValue[0] == _XT("True")?true:false;
    }



    int nWidth = -1;
    if ( cInTag.FindNoCase(TAG_IMAGE_WIDTH) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_IMAGE_WIDTH);
      GETPARAM1(TAG_IMAGE_WIDTH, cParValue[0]);
      nWidth = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

    int nDPI = 96; // Standardauflösung
    if ( cInTag.FindNoCase(TAG_IMAGE_DPI) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_IMAGE_DPI);
      GETPARAM1(TAG_IMAGE_DPI, cParValue[0]);
      nDPI = atol(cParValue[0].c_str());
    }

    int nHeight = -1;
    if ( cInTag.FindNoCase(TAG_IMAGE_HEIGHT) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_IMAGE_HEIGHT);
      GETPARAM1(TAG_IMAGE_HEIGHT, cParValue[0]);
      nHeight = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

// wenn Zoom angegeben ist, dann setzen wir "nWidth = nHeight = -1;"
    int nZoom = -1;
    if ( cInTag.FindNoCase(TAG_IMAGE_ZOOM) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_IMAGE_ZOOM);
      GETPARAM1(TAG_IMAGE_ZOOM, cParValue[0]);
      nZoom = atol(cParValue[0].c_str());
      nWidth = nHeight = -1;
    }

    bool bThumbnail = false;
    if ( cInTag.FindNoCase(TAG_IMAGE_THUMBNAIL) != xtstring::npos)
    {
      bThumbnail = true;
    }

// Resize nur, wenn shrink auf "True" steht
    xtstring cTempPath(sPath);
    xtstring sType = _XT(".png");
    if((bShrink || bThumbnail) && pCW->GetTemporaryFileName(sPath, cTempPath, sType) )
    {
      CXPubPicture cPicture;
      if(bThumbnail == false)
      {
        if(nZoom != -1)
          cPicture.Resize(sPath, cTempPath, nZoom);
        else // nWidth oder nHeight kann auf -1 stehen, es wird auch nur nDPI transformiert
          cPicture.Resize(sPath, cTempPath, nWidth, nHeight, nDPI, nDPI);
      }
      else
      {
        if(nZoom != -1 && cPicture.BuildThumbnailSmall(sPath, nZoom, 0, 0))
        {
          cPicture.BuildSmallFileName(sPath, cTempPath);
        }
        else
          return false;
      }
      nZoom = 100; // wir nehmen nun die Größe von Original

    }
// Ist Grafik vorhanden?
    if(pCW->IsPathFile(cTempPath.c_str()))   
    {
      pCW->m_pConvertTo->InsertImage(cTempPath, (float)nWidth, (float)nHeight, nZoom, bShrink);
    }
    return true;
  }
  return false;
}

bool CConvertWorkerFromTag::Styles(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_STYLE)
      sKeyword = TAG_STYLE;
    else if (lpLitToken->nCase == tCASE_STYLE_END)
      sKeyword = TAG_STYLE_END;
    pCW->CheckKeyword(sKeyword);
  }

  xtstring cParValue[4];

  if (lpLitToken->nCase == tCASE_STYLE_END)
  {
    // Style Block Ende
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess)
    {
      pCW->m_nWorkModus = tWM_PreProcess;
      pCW->m_pConvertTo->CreateDefinedStyle(pCW->m_cStyleManager.GetActualStyleToSet());
      pCW->m_cStyleManager.SetNoActualStyleToSet();
    }
    else if (pCW->m_nWorkModus == tWM_StyleInWork)
    {
      pCW->m_nWorkModus = tWM_Work;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      // character style beenden
      size_t nRemovePos;
      CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_StyleElement);
      if (pItemToRemove)
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
      pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_StyleElement, nRemovePos);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackRemove, pCW->m_cStyleManager.GetActualFormatInStack());
    }
    return true;
  }

  // Es besteht Frage, was groessere Prioritaet hat - Select, oder Name?
  if (cInTag.FindNoCase(TAG_STYLE_SELECT) != xtstring::npos)
  {
    if (pCW->m_nWorkModus == tWM_Work)
    {
      // jetzt muessen wir dem Style "aktivieren"
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_STYLE_SELECT);

      GETPARAM1(TAG_STYLE_SELECT, cParValue[0]);
      xtstring sStyleName = cParValue[0].c_str();

      CXPubParagraphStyle::TStyleType nStyleType = CXPubParagraphStyle::tST_ParagraphType;
      {
        // es kann sein, dass auch type uebergeben wurde
        xtstring sType = _XT(""); //
        if (cInTag.FindNoCase(TAG_STYLE_TYPE) != xtstring::npos)
        {
          // parameter freeware kontrolle
          if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
            pCW->CheckParameter(sKeyword, TAG_STYLE_TYPE);

          GETPARAM1(TAG_STYLE_TYPE, cParValue[0]);
          sType =  cParValue[0].c_str();
          if (0 == sType.CompareNoCase(TAG_STYLE_TYPE_CHARACTER))
            nStyleType = CXPubParagraphStyle::tST_CharacterType;
        }
      }

      // Wir muessen dem Style kontrollieren.
      // Falls style nicht existiert, selektieren wir default
      // Falls style 'paragpraph' ist -> select
      // Falls style 'character' ist ->FormatStack
      CXPubParagraphStyle* pSelStyle = pCW->m_cStyleManager.GetStyle(sStyleName);
      if (!pSelStyle)
      {
        if (nStyleType == CXPubParagraphStyle::tST_ParagraphType)
        {
          pCW->m_cStyleManager.SetActualStyle(sStyleName);
          pCW->m_pConvertTo->StyleEvent(tSE_FormatStackStart, pCW->m_cStyleManager.GetActualFormatInStack());
        }
        else
        {
          size_t nRemovePos;
          // character style beenden
          CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_StyleElement);
          if (pItemToRemove)
            pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
          pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_StyleElement, nRemovePos);
          // Es kann sein, dass in diesem Moment in nRemovePos -1 ist,
          // weil kein vorheriges CharacterStyle in FormatStack war.
          pCW->m_cStyleManager.CreateNewItemInFormatStack(nRemovePos);
          pCW->m_cStyleManager.GetItemInFormatStack(nRemovePos)->SetCharacterStyle(0);
          pCW->m_cStyleManager.GetItemInFormatStack(nRemovePos)->SetStyleName(sStyleName);
          pCW->m_cStyleManager.GetItemInFormatStack(nRemovePos)->SetStyleType(CXPubParagraphStyle::tST_CharacterType);
          pCW->m_cStyleManager.RecopyProperties(nRemovePos);
          if (nRemovePos == -1)
            pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetActualFormatInStack());
          else
          {
            for (size_t nI = nRemovePos; nI < pCW->m_cStyleManager.CountOfFormatsInFormatStack(); nI++)
              pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetFormatInStack(nI));
          }
        }
      }
      else
      {
        if (pSelStyle->GetStyleType() == CXPubParagraphStyle::tST_ParagraphType)
        {
          pCW->m_cStyleManager.SetActualStyle(sStyleName);
          pCW->m_pConvertTo->StyleEvent(tSE_FormatStackStart, pCW->m_cStyleManager.GetActualFormatInStack());
        }
        else if (pSelStyle->GetStyleType() == CXPubParagraphStyle::tST_CharacterType)
        {
          size_t nRemovePos;
          // character style beenden
          CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_StyleElement);
          if (pItemToRemove)
            pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
          pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_StyleElement, nRemovePos);
          // Es kann sein, dass in diesem Moment in nRemovePos -1 ist,
          // weil kein vorheriges CharacterStyle in FormatStack war.
          pCW->m_cStyleManager.CreateNewItemInFormatStack(nRemovePos);
          pCW->m_cStyleManager.GetItemInFormatStack(nRemovePos)->SetCharacterStyle(pSelStyle);
          pCW->m_cStyleManager.RecopyProperties(nRemovePos);
          if (nRemovePos == -1)
            pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetActualFormatInStack());
          else
          {
            for (size_t nI = nRemovePos; nI < pCW->m_cStyleManager.CountOfFormatsInFormatStack(); nI++)
              pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetFormatInStack(nI));
          }
        }
      }
    }
    return true;
  }

  if (cInTag.FindNoCase(TAG_STYLE_NAME) != xtstring::npos)
  {
    if (pCW->m_nWorkModus == tWM_PreProcess)
    {
      // jetzt muessen wir Style anlegen und fuer SET "aktivieren"
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_STYLE_NAME);

      GETPARAM1(TAG_STYLE_NAME, cParValue[0]);
      xtstring sStyleName = cParValue[0].c_str();
      bool bStyleIsPermanent = true;
      if (cInTag.FindNoCase(TAG_STYLE_PERMANENT) != xtstring::npos)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_STYLE_PERMANENT);

        GETPARAM1(TAG_STYLE_PERMANENT, cParValue[0]);
        bStyleIsPermanent = atoi(cParValue[0].c_str()) != 0;
      }


      xtstring sType = TAG_STYLE_TYPE_PARAGRAPH; // default type is paragraph style
      if (cInTag.FindNoCase(TAG_STYLE_TYPE) != xtstring::npos)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_STYLE_TYPE);

        GETPARAM1(TAG_STYLE_TYPE, cParValue[0]);
        sType =  cParValue[0].c_str();
      }

      xtstring sNextStyle = _XT(""); // empty means the same style
      if (cInTag.FindNoCase(TAG_STYLE_NEXTSTYLE) != xtstring::npos)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_STYLE_NEXTSTYLE);

        GETPARAM1(TAG_STYLE_NEXTSTYLE, cParValue[0]);
        sNextStyle =  cParValue[0].c_str();
      }

      xtstring sBasedOn = _XT(""); // empty means the same style
      if (cInTag.FindNoCase(TAG_STYLE_BASEDON) != xtstring::npos)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_STYLE_BASEDON);

        GETPARAM1(TAG_STYLE_BASEDON, cParValue[0]);
        sBasedOn =  cParValue[0].c_str();
      }

      if (sType.size())
      {
        bool bCreate = false;
        CXPubParagraphStyle::TStyleType nStyleType;
        if (0 == sType.CompareNoCase(TAG_STYLE_TYPE_PARAGRAPH))
        {
          bCreate = true;
          nStyleType = CXPubParagraphStyle::tST_ParagraphType;
        }
        if (0 == sType.CompareNoCase(TAG_STYLE_TYPE_CHARACTER))
        {
          bCreate = true;
          nStyleType = CXPubParagraphStyle::tST_CharacterType;
        }
        if (bCreate)
        {
          // Folge Funktion ist so schlau, dass die kontrolliert, ob der Name schon existiert.
          // Falls ja, dann kriegen wir eigentlich dem altem (bereits existierendem) Style.
          // pNewStyle brauchen wir eigentlich nicht
          CXPubParagraphStyle* pNewStyle = pCW->m_cStyleManager.CreateNewStyle(sStyleName, sBasedOn);
          pNewStyle->SetStyleIsPermanent(bStyleIsPermanent);
          pNewStyle->SetStyleType(nStyleType);
          // Folge Funktion ist genau so schlau, wie die vorherige
          pCW->m_cStyleManager.SetActualStyleToSet(sStyleName);
          pNewStyle->SetNextStyle(sNextStyle);
        }
      }

      // Modus umschalten
      pCW->m_nWorkModus = tWM_StyleInPreProcess;
    }
    if (pCW->m_nWorkModus == tWM_Work)
      pCW->m_nWorkModus = tWM_StyleInWork;
    return true;
  }
/*
  if (cInTag.FindNoCase(TAG_STYLE_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_STYLE_NAME);
    GETPARAM1(TAG_STYLE_NAME, cParValue[0]);
    xtstring sStName = cParValue[0].c_str();
    if( sStName.FindNoCase(TAG_STYLE_BASIC) != xtstring::npos )
    { 
      if ( cInTag.FindNoCase(TAG_STYLE_FONTSIZE) != xtstring::npos)
      { 
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_STYLE_FONTSIZE);
        GETPARAM1(TAG_STYLE_FONTSIZE, cParValue[0]);
        int wPoints = atoi(cParValue[0].c_str());
        if(wPoints > 0)
        {
          pCW->m_pConvertTo->SetFontSize(CHECK_FONTSIZE(wPoints));
          pCW->m_pConvertTo->SetFontSizeDefault(wPoints);
        }
      }
      if ( cInTag.FindNoCase(TAG_STYLE_FONTNAME) != xtstring::npos)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_STYLE_FONTNAME);
        GETPARAM1(TAG_STYLE_FONTNAME, cParValue[0]);
        if(cParValue[0].size() > 0)
        {
          pCW->m_pConvertTo->SetFontName(cParValue[0]);
          pCW->m_pConvertTo->SetFontNameDefault(cParValue[0]);
        }
      }
      if ( cInTag.FindNoCase(TAG_STYLE_CODEPAGE) != xtstring::npos)
      {
        // parameter freeware kontrolle
        if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
          pCW->CheckParameter(sKeyword, TAG_STYLE_CODEPAGE);
        GETPARAM1(TAG_STYLE_CODEPAGE, cParValue[0]);

        BOOL bSetCodePage = TRUE;

#if defined(WIN32)

      //Prüfung, ob es die Codepage gibt? 
        CPINFOEX cpInfo;
        cpInfo.MaxCharSize = MAX_DEFAULTCHAR;
        ::GetCPInfoEx(atol(cParValue[0].c_str()), 0, &cpInfo);
        if(::GetLastError() == ERROR_INVALID_PARAMETER) // If the specified code page is not installed or not available
          bSetCodePage = FALSE;

#endif 
        if (bSetCodePage)
        {  
          pCW->m_pConvertTo->SetCodePageDefault(cParValue[0]);
          pCW->m_pConvertTo->SetCodePage(pCW->m_pConvertTo->GetCodePageDefault());

        // style setzt auf alle Defaults
          pCW->m_pConvertTo->SetFontName(pCW->m_pConvertTo->GetFontNameDefault());
          pCW->m_pConvertTo->SetFontSize(pCW->m_pConvertTo->GetFontSizeDefault());
        }
      }
      return true;
    }
  }
*/
  return false;
}

bool CConvertWorkerFromTag::Font(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_FONT)
      sKeyword = TAG_FONT;
    else if (lpLitToken->nCase == tCASE_FONT_END)
      sKeyword = TAG_FONT_END;
    pCW->CheckKeyword(sKeyword);
  }

  xtstring cParValue[4];

  if (pCW->m_nWorkModus == tWM_Work
      && lpLitToken->nCase == tCASE_FONT_END)
  {
    size_t nRemovePos;
    CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_FontAll);
    if (pItemToRemove)
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
    pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_FontAll, nRemovePos);
    pCW->m_pConvertTo->StyleEvent(tSE_FormatStackRemove, pCW->m_cStyleManager.GetActualFormatInStack());
    return true;
  }

  // FormatStack
  if (pCW->m_nWorkModus == tWM_Work)
  {
    size_t nPos = -1;
    pCW->m_cStyleManager.CreateNewItemInFormatStack(nPos);
  }

  if (cInTag.FindNoCase(TAG_FONT_CODEPAGE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FONT_CODEPAGE);

    GETPARAM1(TAG_FONT_CODEPAGE, cParValue[0]);
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
      pCW->m_cStyleManager.GetActualStyleToSet()->SetCodePage(_ttoi(cParValue[0].c_str()));
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.GetActualFormatInStack()->SetCodePage(_ttoi(cParValue[0].c_str()));
      pCW->m_pConvertTo->SetCodePage(cParValue[0]);
    }
  }

  if (cInTag.FindNoCase(TAG_FONT_SIZE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FONT_SIZE);

    GETPARAM1(TAG_FONT_SIZE, cParValue[0]);
    int wPoints = atoi(cParValue[0].c_str());
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
      pCW->m_cStyleManager.GetActualStyleToSet()->SetFontSize(CHECK_FONTSIZE(wPoints));
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.GetActualFormatInStack()->SetFontSize(CHECK_FONTSIZE(wPoints));
      pCW->m_pConvertTo->SetFontSize(CHECK_FONTSIZE(wPoints));
    }
  }

  if (cInTag.FindNoCase(TAG_FONT_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FONT_NAME);

    GETPARAM1(TAG_FONT_NAME, cParValue[0]);
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
      pCW->m_cStyleManager.GetActualStyleToSet()->SetFontName(cParValue[0]);
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.GetActualFormatInStack()->SetFontName(CHECK_FONTNAME(cParValue[0]));
      pCW->m_pConvertTo->SetFontName(cParValue[0]);
    }
  }

  if (cInTag.FindNoCase(TAG_FONT_COLOR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FONT_COLOR);

    GETPARAM(TAG_FONT_COLOR, cParValue[0]);
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
      pCW->m_cStyleManager.GetActualStyleToSet()->SetFontColor(pCW->GetColor(cParValue[0], TAG_DEFAULT_BLACK));
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.GetActualFormatInStack()->SetFontColor(pCW->GetColor(cParValue[0], TAG_DEFAULT_BLACK));
      pCW->m_pConvertTo->SetTextColor(pCW->GetColor(cParValue[0], TAG_DEFAULT_BLACK));
    }
  }

  if (cInTag.FindNoCase(TAG_FONT_BGCOLOR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FONT_BGCOLOR);

    GETPARAM(TAG_FONT_BGCOLOR, cParValue[0]);
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
      pCW->m_cStyleManager.GetActualStyleToSet()->SetFontBackColor(pCW->GetColor(cParValue[0], TAG_DEFAULT_WHITE));
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.GetActualFormatInStack()->SetFontBackColor(pCW->GetColor(cParValue[0], TAG_DEFAULT_WHITE));
    }
  }

  if (pCW->m_nWorkModus == tWM_Work)
    pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetActualFormatInStack());

  return true;
}

bool CConvertWorkerFromTag::FontAttribute(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_BOLD)
      sKeyword = TAG_FONT_BOLD;
    else if (lpLitToken->nCase == tCASE_BOLD_END)
      sKeyword = TAG_FONT_BOLD_END;
    else if (lpLitToken->nCase == tCASE_ITALIC)
      sKeyword = TAG_FONT_ITALIC;
    else if (lpLitToken->nCase == tCASE_ITALIC_END)
      sKeyword = TAG_FONT_ITALIC_END;
    else if (lpLitToken->nCase == tCASE_UNDERLINE)
      sKeyword = TAG_FONT_UNDERLINE;
    else if (lpLitToken->nCase == tCASE_UNDERLINE_END)
      sKeyword = TAG_FONT_UNDERLINE_END;
    else if (lpLitToken->nCase == tCASE_UL_DOUBLE)
      sKeyword = TAG_FONT_UL_DOUBLE;
    else if (lpLitToken->nCase == tCASE_UL_DOUBLE_END)
      sKeyword = TAG_FONT_UL_DOUBLE_END;
    else if (lpLitToken->nCase == tCASE_STRIKE)
      sKeyword = TAG_FONT_UL_STRIKE;
    else if (lpLitToken->nCase == tCASE_STRIKE_END)
      sKeyword = TAG_FONT_UL_STRIKE_END;
    pCW->CheckKeyword(sKeyword);
  }

  switch (lpLitToken->nCase)
  {
  case tCASE_BOLD:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetBold(true);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nPos = -1;
      pCW->m_cStyleManager.CreateNewItemInFormatStack(nPos);
      pCW->m_cStyleManager.GetActualFormatInStack()->SetBold(true);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_BOLD);
    }
    return true;
    break;
  case tCASE_BOLD_END:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetBold(false);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nRemovePos;
      CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_Bold);
      if (pItemToRemove)
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
      pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_Bold, nRemovePos);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackRemove, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_BOLD_END);
    }
    return true;
    break;
  case tCASE_ITALIC: 
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetItalic(true);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nPos = -1;
      pCW->m_cStyleManager.CreateNewItemInFormatStack(nPos);
      pCW->m_cStyleManager.GetActualFormatInStack()->SetItalic(true);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_ITALIC);
    }
    return true;
    break;
  case tCASE_ITALIC_END:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetItalic(false);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nRemovePos;
      CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_Italic);
      if (pItemToRemove)
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
      pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_Italic, nRemovePos);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackRemove, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_ITALIC_END);
    }
    return true;
    break;
  case tCASE_UNDERLINE:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetUnderline(true);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nPos = -1;
      pCW->m_cStyleManager.CreateNewItemInFormatStack(nPos);
      pCW->m_cStyleManager.GetActualFormatInStack()->SetUnderline(true);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_UNDERLINE);
    }
    return true;
    break;
  case tCASE_UNDERLINE_END:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetUnderline(false);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nRemovePos;
      CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_Underline);
      if (pItemToRemove)
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
      pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_Underline, nRemovePos);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackRemove, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_UNDERLINE_END);
    }
    return true;
    break;
  case tCASE_UL_DOUBLE:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetDoubleUnderline(true);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nPos = -1;
      pCW->m_cStyleManager.CreateNewItemInFormatStack(nPos);
      pCW->m_cStyleManager.GetActualFormatInStack()->SetDoubleUnderline(true);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_UL_DOUBLE);
    }
    return true;
    break;
  case tCASE_UL_DOUBLE_END:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetDoubleUnderline(false);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nRemovePos;
      CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_DoubleUnderline);
      if (pItemToRemove)
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
      pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_DoubleUnderline, nRemovePos);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackRemove, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_UL_DOUBLE_END);
    }
    return true;
    break;
  case tCASE_STRIKE:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetStrikeOut(true);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nPos = -1;
      pCW->m_cStyleManager.CreateNewItemInFormatStack(nPos);
      pCW->m_cStyleManager.GetActualFormatInStack()->SetStrikeOut(true);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackAdd, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_STRIKE);
    }
    return true;
    break;
  case tCASE_STRIKE_END:
    if (pCW->m_nWorkModus == tWM_StyleInPreProcess
        && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetStrikeOut(false);
      return true;
    }
    else if (pCW->m_nWorkModus == tWM_Work)
    {
      size_t nRemovePos;
      CXPubParagraphStyle* pItemToRemove = pCW->m_cStyleManager.GetItemForRelease(CXPubParagraphStyle::tCP_StrikeOut);
      if (pItemToRemove)
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackFormatToRemove, pItemToRemove);
      pCW->m_cStyleManager.ReleaseItemInFormatStack(CXPubParagraphStyle::tCP_StrikeOut, nRemovePos);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackRemove, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_STRIKE_END);
    }
    return true;
    break;
  default:
    if (pCW->m_nWorkModus == tWM_Work)
      return pCW->m_pConvertTo->SetFontAttribute(TAG_TEXT_NORMAL);
    break;
  }
  return false;
}

bool CConvertWorkerFromTag::Tabulator(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_TABDEFAULTS)
      sKeyword = TAG_TAB_DEFAULT;
    else if (lpLitToken->nCase == tCASE_TAB)
      sKeyword = TAG_TAB;
    else if (lpLitToken->nCase == tCASE_TABSIMPLE)
      sKeyword = TAG_TAB_SIMPLE;
    pCW->CheckKeyword(sKeyword);
  }

  xtstring cParValue[4];
  
  if (lpLitToken->nCase == tCASE_TABDEFAULTS) 
  {
    // In diesem IF geht um DEFAULT Werte fuer TABS

    int nTabDecimal = TAG_TABULATOR_DECIMAL_COMMA;
    if (cInTag.FindNoCase(TAG_TAB_DEFAULT_SEPERATOR) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_TAB_DEFAULT_SEPERATOR);

      GETPARAM1(TAG_TAB_DEFAULT_SEPERATOR, cParValue[0]);
      if (cParValue[0].FindNoCase(TAG_TAB_SEPERATOR_COMMA) == xtstring::npos)
        nTabDecimal = TAG_TABULATOR_DECIMAL_POINT; 
    }

    xtstring sDefTabPos(TAG_TAB_DEFAULT_TAPOS_VALUE); 
    if (cInTag.FindNoCase(TAG_TAB_DEFAULT_TAPOS) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_TAB_DEFAULT_TAPOS);

      GETPARAM1(TAG_TAB_DEFAULT_TAPOS, cParValue[0]);
      sDefTabPos = cParValue[0];
    }

    int nDefTabPos = pCW->ConvertUnitOfLength(sDefTabPos.c_str());
    if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.SetTabDefaults(nDefTabPos, nTabDecimal);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->SetDefTabulator(nDefTabPos, nTabDecimal);
    }
    else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      pCW->m_cStyleManager.GetActualStyleToSet()->SetTabDefaults(nDefTabPos, nTabDecimal);
      return true;
    }
    return false;
  }

  if (lpLitToken->nCase == tCASE_TABSIMPLE)
  {
    // In diesem IF geht um TAB einfuegen.

    if (pCW->m_nWorkModus == tWM_Work)
      return pCW->m_pConvertTo->InsertTab();
    // Wenn wir nicht arbeiten, dann einfach TRUE
    return true;
  }

  // muss als erstes ausgewertet werden, sonst werden Folgeparameter überschrieben
  if (cInTag.FindNoCase(TAG_TAB_CLEAR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TAB_CLEAR);

    GETPARAM1(TAG_TAB_CLEAR, cParValue[0]);
    if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.ClearTabStops();
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
      pCW->m_pConvertTo->InsertTabulator(TAG_TABULATOR_CLEARALL, 0, xtstring(_XT("")), xtstring(_XT("")));
    }
    else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
      pCW->m_cStyleManager.GetActualStyleToSet()->RemoveAllTabStops();
  }

  int nTabFlag = TAG_TABULATOR_DEFAULT;
  if (cInTag.FindNoCase(TAG_TAB_ALIGN) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TAB_ALIGN);

    GETPARAM1(TAG_TAB_ALIGN, cParValue[0]);
    if (cParValue[0].FindNoCase(TAG_TAB_LEFT) != xtstring::npos)
      nTabFlag = TAG_TABULATOR_LEFT;
    if (cParValue[0].FindNoCase(TAG_TAB_RIGHT) != xtstring::npos)
      nTabFlag = TAG_TABULATOR_RIGHT;
    if (cParValue[0].FindNoCase(TAG_TAB_CENTER) != xtstring::npos)
      nTabFlag = TAG_TABULATOR_CENTER;
    if (cParValue[0].FindNoCase(TAG_TAB_DECIMAL) != xtstring::npos)
      nTabFlag = TAG_TABULATOR_DECIMAL;
  }

  xtstring sTabDecChar = _XT(""); // dezimales Ausrichtungszeichen
  if (cInTag.FindNoCase(TAG_TAB_DECCHAR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TAB_DECCHAR);

    GETPARAM1(TAG_TAB_DECCHAR, cParValue[0]);
    sTabDecChar = cParValue[0];
  }

  xtstring sTabLeader = _XT(""); // aufzufüllende Zeichen vor Tab
  if (cInTag.FindNoCase(TAG_TAB_LEADER) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TAB_LEADER);

    GETPARAM1(TAG_TAB_LEADER, cParValue[0]);
    sTabLeader = cParValue[0];
 }

  if (cInTag.FindNoCase(TAG_TAB_POSITION) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TAB_POSITION);

    GETPARAM1(TAG_TAB_POSITION, cParValue[0]);
    int nTabPos = pCW->ConvertUnitOfLength(cParValue[0].c_str());

    if (pCW->m_nWorkModus == tWM_Work)
    {
      pCW->m_cStyleManager.AddTabStop(nTabFlag, nTabPos, sTabDecChar, sTabLeader);
      pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
      return pCW->m_pConvertTo->InsertTabulator(nTabFlag, nTabPos, sTabDecChar, sTabLeader);
    }
    else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
      pCW->m_cStyleManager.GetActualStyleToSet()->AddTabStop(nTabFlag, nTabPos, sTabDecChar, sTabLeader);
  }
  return false;
}

bool CConvertWorkerFromTag::SetHeaderFooterPos(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_FOOTER_MARGIN)
      sKeyword = TAG_FOOTER_MARGIN;
    else if (lpLitToken->nCase == tCASE_HEADER_MARGIN)
      sKeyword = TAG_HEADER_MARGIN;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  xtstring cParValue[4];

  if (lpLitToken->nCase == tCASE_HEADER_MARGIN)
  {
    if ( cInTag.FindNoCase(TAG_HEADER_POS) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_HEADER_POS);

      GETPARAM1(TAG_HEADER_POS, cParValue[0]);
      int nPos = pCW->ConvertUnitOfLength(cParValue[0].c_str());
      bool bReturn = pCW->m_pConvertTo->SetHeaderFooterPos(TAG_TEXT_HEADPOS, nPos);
      return bReturn & pCW->m_pConvertTo->SwitchHeaderFooter(TAG_TEXT_HEADPOS, TAG_CURSOR_END);
    }
  }

  if (lpLitToken->nCase == tCASE_FOOTER_MARGIN)
  {
    if ( cInTag.FindNoCase(TAG_FOOTER_POS) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_FOOTER_POS);

      GETPARAM1(TAG_FOOTER_POS, cParValue[0]);
      int nPos = pCW->ConvertUnitOfLength(cParValue[0].c_str());
      bool bReturn = pCW->m_pConvertTo->SetHeaderFooterPos(TAG_TEXT_FOOTERPOS, nPos);
      return bReturn & pCW->m_pConvertTo->SwitchHeaderFooter(TAG_TEXT_FOOTERPOS, TAG_CURSOR_END);
    }
  }
  return false;

}

bool CConvertWorkerFromTag::SwitchHeaderFooter(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_HEADER)
      sKeyword = TAG_HEADER;
    else if (lpLitToken->nCase == tCASE_HEADER_END)
      sKeyword = TAG_HEADER_END;
    else if (lpLitToken->nCase == tCASE_FOOTER)
      sKeyword = TAG_FOOTER;
    else if (lpLitToken->nCase == tCASE_FOOTER_END)
      sKeyword = TAG_FOOTER_END;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  bool bRet = false;
  switch (lpLitToken->nCase)
  {
  case tCASE_HEADER:
    bRet = pCW->m_pConvertTo->SwitchHeaderFooter(TAG_TEXT_HEADPOS, TAG_CURSOR_END);
    break;
  case tCASE_FOOTER:
    bRet = pCW->m_pConvertTo->SwitchHeaderFooter(TAG_TEXT_FOOTERPOS, TAG_CURSOR_END);
    break;
  case tCASE_HEADER_END:
  case tCASE_FOOTER_END:
    bRet = pCW->m_pConvertTo->SwitchHeaderFooter(TAG_TEXT_BODYPOS, TAG_CURSOR_END);
    break;
  default:
    return false;
  }

  // Style Manager funktionalitaet
  switch (lpLitToken->nCase)
  {
  case tCASE_HEADER:
  case tCASE_FOOTER:
    // neuem Segment in FormatStack erzeugen
    // Segment faengt mit dem Default Style
    pCW->m_cStyleManager.CreateNewSegmentInFormatStack();
    // Jetzt muessen wir aber noch explizit bekannt geben, dass Style geaendert wurde.
    pCW->m_cStyleManager.BackUpStyle();
    pCW->m_cStyleManager.SetActualStyle(DEFAULT_STYLE_NAME);
    pCW->m_pConvertTo->StyleEvent(tSE_FormatStackStart, pCW->m_cStyleManager.GetActualFormatInStack());
    break;
  case tCASE_HEADER_END:
  case tCASE_FOOTER_END:
    // Style restore
    pCW->m_cStyleManager.RestoreStyle();
    // ganzen Segment entfernen
    pCW->m_cStyleManager.ReleaseSegmentInFormatStack();
    // und bekannt geben
    pCW->m_pConvertTo->StyleEvent(tSE_FormatStackSegmentRemoved, pCW->m_cStyleManager.GetActualFormatInStack());
    break;
  }
  return false;
}

bool CConvertWorkerFromTag::Alignment(CConvertWorkerFromTag* pCW,
                                      const xtstring& sKeyword,
                                      const xtstring& cInTag,
                                      void* lpToken)
{
  CHECKCALLPOINTER()

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 
  xtstring cParValue[4];

  if (cInTag.FindNoCase(TAG_PARA_ALIGN) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_PARA_ALIGN);

    GETPARAM1(TAG_PARA_ALIGN, cParValue[0]);

    if (cParValue[0].FindNoCase(TAG_PARAGRAPH_LEFT) != xtstring::npos)
    {
      if (pCW->m_nWorkModus == tWM_Work)
      {
        pCW->m_cStyleManager.SetTextAlignment(TAG_TEXT_TEXTL);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        return pCW->m_pConvertTo->TextAlignment(TAG_TEXT_TEXTL);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
      {
        pCW->m_cStyleManager.GetActualStyleToSet()->SetTextAlignment(TAG_TEXT_TEXTL);
        return true;
      }
    }

    if (cParValue[0].FindNoCase(TAG_PARAGRAPH_RIGHT) != xtstring::npos)
    {
      if (pCW->m_nWorkModus == tWM_Work)
      {
        pCW->m_cStyleManager.SetTextAlignment(TAG_TEXT_TEXTR);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        return pCW->m_pConvertTo->TextAlignment(TAG_TEXT_TEXTR);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
      {
        pCW->m_cStyleManager.GetActualStyleToSet()->SetTextAlignment(TAG_TEXT_TEXTR);
        return true;
      }
    }

    if (cParValue[0].FindNoCase(TAG_PARAGRAPH_CENTER) != xtstring::npos)
    {
      if (pCW->m_nWorkModus == tWM_Work)
      {
        pCW->m_cStyleManager.SetTextAlignment(TAG_TEXT_TEXTZ);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        return pCW->m_pConvertTo->TextAlignment(TAG_TEXT_TEXTZ);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
      {
        pCW->m_cStyleManager.GetActualStyleToSet()->SetTextAlignment(TAG_TEXT_TEXTZ);
        return true;
      }
    }
  
    if (cParValue[0].FindNoCase(TAG_PARAGRAPH_BLOCK) != xtstring::npos)
    {
      if (pCW->m_nWorkModus == tWM_Work)
      {
        pCW->m_cStyleManager.SetTextAlignment(TAG_TEXT_TEXTB);
        pCW->m_pConvertTo->StyleEvent(tSE_FormatStackParaChanged, pCW->m_cStyleManager.GetActualFormatInStack());
        return pCW->m_pConvertTo->TextAlignment(TAG_TEXT_TEXTB);
      }
      else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
      {
        pCW->m_cStyleManager.GetActualStyleToSet()->SetTextAlignment(TAG_TEXT_TEXTB);
        return true;
      }
    }

  }
  return false;
}

bool CConvertWorkerFromTag::TableCellActivate(CConvertWorkerFromTag* pCW, bool bDefault, int wRow, int wCol)
{
  CHECKCALLPOINTER()

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  pCW->m_pConvertTo->SetTableCellActivate(pCW->m_pConvertTo->GetTableID(), wRow, wCol, TAG_POS_END);
  
  if(bDefault)
    SetDefaults(pCW);
  return true;
}

inline bool CConvertWorkerFromTag::GetRowCol(CConvertWorkerFromTag* pCW, xtstring& sRow, xtstring& sCol, int& wRow, int& wCol)
{
// #define GET_ROW(x)   (x == 0xffff)?m_nTableRow:x

  if(sRow.FindNoCase(TAG_TABLE_SKIP) != xtstring::npos)
  { 
    wRow = pCW->m_pConvertTo->GetTableRow() + 1;
    pCW->m_pConvertTo->SetTableRow(wRow);
  }
  else
    wRow = atoi(sRow.c_str());

  if(sCol.FindNoCase(TAG_TABLE_SKIP) != xtstring::npos)
  { 
    wCol = pCW->m_pConvertTo->GetTableCol() + 1;
    pCW->m_pConvertTo->SetTableCol(wCol);
  }
  else
    wCol = atoi(sCol.c_str());

  return true;

}

bool CConvertWorkerFromTag::GetRowCol(CConvertWorkerFromTag* pCW,
                                      const xtstring& sKeyword,
                                      const xtstring& cInTag,
                                      int& wRow, int& wCol)
{
  xtstring cParValue[4];

  if (cInTag.FindNoCase(TAG_TABLE_CURRENT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_CURRENT);
    GETPARAM2(TAG_TABLE_CURRENT, cParValue[0], cParValue[1]);
    GetRowCol(pCW, cParValue[0], cParValue[1], wRow, wCol);
  }
  else
  {
    wRow = wCol = -1;
  }

/*
  if ( cInTag.FindNoCase(TAG_TABLE_NESTED) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_NESTED);
    GETPARAM2(TAG_TABLE_NESTED, cParValue[0], cParValue[1]);
    GetRowCol(pCW, cParValue[0], cParValue[1], wNestRow, wNestCol);
  }
  else
  {
    wNestRow = wNestCol = -2;
  }
*/
  return true;
}

bool CConvertWorkerFromTag::Table(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_TABLE)
      sKeyword = TAG_TABLE;
    else if (lpLitToken->nCase == tCASE_TABLE_END)
      sKeyword = TAG_ENDTABLE;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  int nRow = pCW->m_pConvertTo->GetTableRow();
  int nCol = pCW->m_pConvertTo->GetTableCol();

  xtstring cParValue[4];
  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 
  
// es gibt eine automatische Zeilensteuerung d.h. wenn User mit 
// TAG_TABLE_ADDLINE eine Zeile in Tabelle einfügt, dann wird intern
// die Zeile gemerkt. Solange User nun nicht Tabellenbefehl mit 
// "Aktiv=" >= 0(Row) auswählt, muss man für Tabellenbefehle keine
// Zeile auswählen.
  
  if (cInTag.FindNoCase(TAG_TABLE_DEFINITION) != xtstring::npos
      || pCW->m_pConvertTo->GetTableID() < 0)
  {  
      pCW->m_pConvertTo->SetTableRow(-1);
  }

// Achtung, wir dürfen keine Befehle vor TAG_TABLE_CURRENT setzen
// Beispiel: <table rowcol="1,1" irgendetwas>

// Zelle aktivieren
  if ( cInTag.FindNoCase(TAG_TABLE_CURRENT) != xtstring::npos)
  {
    GetRowCol(pCW, sKeyword, cInTag, nRow, nCol);
   
   // Ablauf von SetDefaults darf eigentlich nicht bei  
    bool bDefault = true;
    if (cInTag.FindNoCase(TAG_TABLE_CELL_WIDTH) != xtstring::npos || 
      cInTag.FindNoCase(TAG_TABLE_ROW_HEIGHT) != xtstring::npos ||
      cInTag.FindNoCase(TAG_TABLE_BORDER_WIDTH) != xtstring::npos ||
      cInTag.FindNoCase(TAG_TABLE_BORDER_COLOR) != xtstring::npos ||
      cInTag.FindNoCase(TAG_TABLE_ROWPOSITION) != xtstring::npos ||
      cInTag.FindNoCase(TAG_TABLE_BKGR_COLOR) != xtstring::npos ||
      cInTag.FindNoCase(TAG_TABLE_CELLALIGN) != xtstring::npos
      )
      bDefault = false;
    TableCellActivate(pCW, bDefault, nRow, nCol);  
     
    pCW->m_pConvertTo->SetTableCol(nCol);
    pCW->m_pConvertTo->SetTableRow(nRow);
}

 // eine Tabellenzeile einfügen
  if (cInTag.FindNoCase(TAG_TABLE_INSERTCOLUMN) != xtstring::npos)
  {
    if(pCW->m_pConvertTo->InsertColumn(pCW->m_pConvertTo->GetTableID(), nRow, nCol))
    {
    }
  }

  if (cInTag.FindNoCase(TAG_TABLE_ADDLINE) != xtstring::npos)
  {
    int wRows = 1, wCols = 1; // wir aktivieren die Tabelle 
    // Wieviele Zeilen haben wir momentan?  
    if (pCW->m_pConvertTo->GetRowAndCols(pCW->m_pConvertTo->GetTableID(), wRows, wCols))
    {
      if (pCW->m_pConvertTo->InsertTableRow(pCW->m_pConvertTo->GetTableID(), TAG_TABLE_ROW_APPEND_END, 1))
      {
        pCW->m_pConvertTo->SetTableRow(wRows + 1);
        pCW->m_pConvertTo->SetTableCol(1);

        nRow = pCW->m_pConvertTo->GetTableRow();
        nCol = pCW->m_pConvertTo->GetTableCol();

        TableCellActivate(pCW, true, pCW->m_pConvertTo->GetTableRow(), pCW->m_pConvertTo->GetTableCol());
      }
    }
  }

// Zeile als Kopf definieren, wird bei Seitenumbruch wiederholt
    if (cInTag.FindNoCase(TAG_TABLE_HEADERROW) != xtstring::npos)
    {  
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_TABLE_HEADERROW);
      GETPARAM1(TAG_TABLE_HEADERROW, cParValue[0]);
      bool bSet = atol(cParValue[0].c_str()) != 0;
      pCW->m_pConvertTo->SetTableHeaderRow(pCW->m_pConvertTo->GetTableID(), nRow, nCol, bSet);
    }
    else
    if (cInTag.FindNoCase(TAG_TABLE_HEADERROW_TRUE) != xtstring::npos)
      pCW->m_pConvertTo->SetTableHeaderRow(pCW->m_pConvertTo->GetTableID(), nRow, nCol, true);


// Zeile mit weiteren Zeilen zusammenhalten
    if (cInTag.FindNoCase(TAG_TABLE_KEEPROW) != xtstring::npos)
    {    
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_TABLE_KEEPROW);
      GETPARAM1(TAG_TABLE_KEEPROW, cParValue[0]);
      bool bSet = atol(cParValue[0].c_str()) != 0;
      pCW->m_pConvertTo->SetTableRowKeep(pCW->m_pConvertTo->GetTableID(), nRow, nCol, bSet);
    }
    else
    if (cInTag.FindNoCase(TAG_TABLE_KEEPROW_TRUE) != xtstring::npos)
      pCW->m_pConvertTo->SetTableRowKeep(pCW->m_pConvertTo->GetTableID(), nRow, nCol, true);

// Tabelle anlegen
  if (cInTag.FindNoCase(TAG_TABLE_DEFINITION) != xtstring::npos)
  {    
   // Absatz auf Standard zurückstellen
    pCW->m_pConvertTo->SetParagraphIndents(0,0,0);
    pCW->m_pConvertTo->TextAlignment(TAG_TEXT_TEXTL);
 
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_DEFINITION);
    GETPARAM2(TAG_TABLE_DEFINITION, cParValue[0], cParValue[1]);

    int nRowCnt = atoi(cParValue[0].c_str());
    int nColCnt = atoi(cParValue[1].c_str());

    int nTableID = pCW->m_pConvertTo->CreateTable(nRowCnt, nColCnt);
    pCW->m_pConvertTo->SetTableID(nTableID);
    pCW->m_pConvertTo->SetTableRow(1);
    pCW->m_pConvertTo->SetTableCol(1);

   // Zelle 1,1 aktivieren und Font setzen
    //TableCellActivate(pCW, true, 1, 1);

  }

// minimale Tabellenzeilenhöhe festlegen
  if (cInTag.FindNoCase(TAG_TABLE_ROW_HEIGHT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_ROW_HEIGHT);
    GETPARAM1(TAG_TABLE_ROW_HEIGHT, cParValue[0]);
    
    double fRowHeight = 0;

    nMinHeight = pCW->ConvertUnitOfLength(cParValue[0].c_str());

    pCW->m_pConvertTo->SetRowHeight(pCW->m_pConvertTo->GetTableID(),
                                           nRow, nCol, fRowHeight, -1);
  }

// Breite der Tabellenzelle festlegen
  if (cInTag.FindNoCase(TAG_TABLE_CELL_WIDTH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_CELL_WIDTH);
    GETPARAM1(TAG_TABLE_CELL_WIDTH, cParValue[0]);
    
    long lxExt = 0;
    if ( cParValue[0].size() == 0 || cParValue[0] == _XT("-1") )
      lxExt = -1;
    else
      lxExt = pCW->ConvertUnitOfLength(cParValue[0].c_str());

    pCW->m_pConvertTo->SetCellWidth(pCW->m_pConvertTo->GetTableID(),
                                           nRow, nCol, lxExt, -1);
  }

// Tabellenzelle mit Rahmen belegen
  if (cInTag.FindNoCase(TAG_TABLE_BORDER_WIDTH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_BORDER_WIDTH);
    GETPARAM4(TAG_TABLE_BORDER_WIDTH, cParValue[0], cParValue[1], cParValue[2], cParValue[3]);

    int nLeft = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    int nTop = pCW->ConvertUnitOfLength(cParValue[1].c_str());
    int nRight = pCW->ConvertUnitOfLength(cParValue[2].c_str());
    int nButton = pCW->ConvertUnitOfLength(cParValue[3].c_str());

    nLeft = (nLeft == -1) ? 0 : nLeft;
    nTop = (nTop == -1) ? 0 : nTop;
    nRight = (nRight == -1) ? 0 : nRight;
    nButton = (nButton == -1) ? 0 : nButton;

    pCW->m_pConvertTo->SetCellBorderWidth(pCW->m_pConvertTo->GetTableID(),
                                                 nRow, nCol, nLeft, nTop, nRight, nButton);
  }

 // Rotieren von Text in Zelle
  if (cInTag.FindNoCase(TAG_TABLE_CELLROTATE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_CELLROTATE);
    GETPARAM1(TAG_TABLE_CELLROTATE, cParValue[0]);
    
    int nDirection = TAG_TABLE_ROTATE_HORZ;
    
    if (cParValue[0].FindNoCase(TAG_TABLE_CELLROTATE_TOPBOT) != xtstring::npos)
        nDirection = TAG_TABLE_ROTATE_TOPTOBOT;
    
    if (cParValue[0].FindNoCase(TAG_TABLE_CELLROTATE_BOTTOP) != xtstring::npos)
      nDirection = TAG_TABLE_ROTATE_BOTTOTOP;

    pCW->m_pConvertTo->SetCellRotate(pCW->m_pConvertTo->GetTableID(), nRow, nCol, nDirection);

  }

// Tabellenborder mit Farbe belegen
  if (cInTag.FindNoCase(TAG_TABLE_BORDER_COLOR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_BORDER_COLOR);
    GETPARAM(TAG_TABLE_BORDER_COLOR, cParValue[0]);
    int nFieldCount = cParValue[0].GetFieldCount(',');
    if( nFieldCount == 4 || nFieldCount == 3 || nFieldCount == 1)
    {
      _ColorDef AllClrf = pCW->GetColor(cParValue[0], TAG_DEFAULT_BLACK);
      pCW->m_pConvertTo->SetCellBorderColor(pCW->m_pConvertTo->GetTableID(),
                          nRow, nCol, AllClrf, AllClrf, AllClrf, AllClrf);
    }
    else
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_TABLE_BORDER_COLOR);
      GETPARAM4(TAG_TABLE_BORDER_COLOR, cParValue[0], cParValue[1], cParValue[2], cParValue[3]);

      COLORREF nLeft = pCW->GetColor(cParValue[0], (COLORDEF)-1);
      COLORREF nTop = pCW->GetColor(cParValue[1], (COLORDEF)-1);
      COLORREF nRight = pCW->GetColor(cParValue[2], (COLORDEF)-1);
      COLORREF nBottom = pCW->GetColor(cParValue[3], (COLORDEF)-1);
   
      pCW->m_pConvertTo->SetCellBorderColor(pCW->m_pConvertTo->GetTableID(),
                                            nRow, nCol, nLeft, nTop, nRight, nBottom);

    }
  }

// Tabellenzelle mit Hintergrund belegen
  if (cInTag.FindNoCase(TAG_TABLE_BKGR_COLOR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_BKGR_COLOR);
    GETPARAM(TAG_TABLE_BKGR_COLOR, cParValue[0]);

    pCW->m_pConvertTo->CellBackgroundColor(pCW->m_pConvertTo->GetTableID(),
                                                  nRow, nCol, pCW->GetColor(cParValue[0], TAG_DEFAULT_WHITE));
  }

  // linke Beginnposition der Tabelle
  if (cInTag.FindNoCase(TAG_TABLE_ROWPOSITION) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_ROWPOSITION);
    GETPARAM1(TAG_TABLE_ROWPOSITION, cParValue[0]);

    long lIdent = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    pCW->m_pConvertTo->SetRowPosition(pCW->m_pConvertTo->GetTableID(), nRow, lIdent);

  }

// Margin der Zelle
  if (cInTag.FindNoCase(TAG_TABLE_CELLMARGIN) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_CELLMARGIN);
    GETPARAM4(TAG_TABLE_CELLMARGIN, cParValue[0], cParValue[1], cParValue[2], cParValue[3]);

    int nLeft = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    int nTop = pCW->ConvertUnitOfLength(cParValue[1].c_str());
    int nRight = pCW->ConvertUnitOfLength(cParValue[2].c_str());
    int nButton = pCW->ConvertUnitOfLength(cParValue[3].c_str());

    pCW->m_pConvertTo->CellMargin(pCW->m_pConvertTo->GetTableID(), nRow, nCol, nLeft, nTop, nRight, nButton);
  }

// Text in der Tabellenzelle ausrichten
  if (cInTag.FindNoCase(TAG_TABLE_CELLALIGN) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_CELLALIGN);
    GETPARAM1(TAG_TABLE_CELLALIGN, cParValue[0]);

    long lVertAlign = TAG_TABLE_VERTALIGN_TOP;
    if (cParValue[0].FindNoCase(TAG_TABLE_ALIGN_TOP) != xtstring::npos)
      lVertAlign = TAG_TABLE_VERTALIGN_TOP;
    else
    if (cParValue[0].FindNoCase(TAG_TABLE_ALIGN_CENTER) != xtstring::npos)
      lVertAlign = TAG_TABLE_VERTALIGN_CENTER;
    else
    if (cParValue[0].FindNoCase(TAG_TABLE_ALIGN_BOTTOM) != xtstring::npos)
      lVertAlign = TAG_TABLE_VERTALIGN_BOTTOM;

    pCW->m_pConvertTo->CellVerticalAlign(pCW->m_pConvertTo->GetTableID(),
                                                nRow, nCol, lVertAlign);
  }

// Tabellenzelle aktivieren und löschen
  if (cInTag.FindNoCase(TAG_TABLE_DELETECOLUMN) != xtstring::npos)
  {
    pCW->m_pConvertTo->DeleteColumn(pCW->m_pConvertTo->GetTableID(), nRow, nCol);
  }

  if (cInTag.FindNoCase(TAG_TABLE_SPLITCOLUMN) != xtstring::npos)
  {
    pCW->m_pConvertTo->SplitColumn(pCW->m_pConvertTo->GetTableID(), nRow, nCol);
  }

  if (cInTag.FindNoCase(TAG_ENDTABLE) != xtstring::npos)
  { 
    pCW->m_pConvertTo->SetCursorAfterTable(pCW->m_pConvertTo->GetTableID());
  }

  if (cInTag.FindNoCase(TAG_TABLE_BG_IMAGE) != xtstring::npos)
  { 
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_BG_IMAGE);
    GETPARAM1(TAG_TABLE_BG_IMAGE, cParValue[0]);

    xtstring sImagePath=cParValue[0];
    if(!pCW->IsPathFile(sImagePath.c_str()))
    {
      xtstring sRootPicturePath;
      pCW->GetRootPicturePath(sRootPicturePath);
      if (sRootPicturePath.size())
        sImagePath=sRootPicturePath+sImagePath;
    }
      
    if(pCW->IsPathFile(sImagePath.c_str()))
      pCW->m_pConvertTo->SetTableBackgroundPicture(pCW->m_pConvertTo->GetTableID(), nRow, nCol, sImagePath);
  
  }
  return false;

}

bool CConvertWorkerFromTag::CursorPosition(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_CURSOR_END)
      sKeyword = TAG_ENDPOS;
    else if (lpLitToken->nCase == tCASE_CURSOR_HOME)
      sKeyword = TAG_HOMEPOS;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

// !Achtung, wenn Sectionunterstützung implementiert ist
  switch(lpLitToken->nCase)
  {
  case tCASE_CURSOR_HOME: // Set centered paragraphs.
    return pCW->m_pConvertTo->SetCursorPosition(TAG_CURSOR_HOME, 0);
    break;
  case tCASE_CURSOR_END: // Set centered paragraphs.
    return pCW->m_pConvertTo->SetCursorPosition(TAG_CURSOR_END, 0);
    break;
  default:
    return false;
  }   
  return false;
}

bool CConvertWorkerFromTag::Checkbox(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_CHECKBOX;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  xtstring cParValue[4];
  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 

  if ( cInTag.FindNoCase(TAG_CHECKBOX_VALUE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_CHECKBOX_VALUE);
    GETPARAM1(TAG_CHECKBOX_VALUE, cParValue[0]);

    xtstring cSymBitmap;
    if (cParValue[0] == TAG_TRUE)
      cSymBitmap = CHECKBOX_YES;
    else
      cSymBitmap = CHECKBOX_NO;

    xtstring sPath; 
    pCW->GetModulePath(sPath);
    sPath += cSymBitmap;
    if(!pCW->IsPathFile(sPath.c_str()))
    {
      pCW->m_pConvertTo->InsertTextFromMemory(_XT("<Error: missing 'chkboxyes(no).jpg'>"), CNV_TEXTFORMAT_TEXT);
      return false;
    }
    if(pCW->m_pConvertTo->GetFormaterType() == PDF_Formater)
    {
      return pCW->m_pConvertTo->InsertImage(sPath, -1, -1, 100, false);
    }
    else
    {
      if (cParValue[0].FindNoCase(TAG_TRUE) != xtstring::npos)
        return pCW->m_pConvertTo->Field(xtstring("<field symbol=254>"));
      else
        return pCW->m_pConvertTo->Field(xtstring("<field symbol=111>"));
    }
  }
  return false;
}

bool CConvertWorkerFromTag::Box(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_BOX)
      sKeyword = TAG_BOX;
    else if (lpLitToken->nCase == tCASE_BOXEND)
      sKeyword = TAG_BOXEND;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  if (lpLitToken->nCase == tCASE_BOX)
  {
    xtstring cParValue[4];
    xtstring sColor(BOX_BKGRND_DEFAULT);
    COLORDEF clrf = pCW->GetColor(sColor, TAG_DEFAULT_WHITE);
    if ( cInTag.FindNoCase(TAG_BOX_BGCOLOR) != xtstring::npos)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_BOX_BGCOLOR);
      GETPARAM(TAG_BOX_BGCOLOR, cParValue[0]);
      clrf = pCW->GetColor(cParValue[0], TAG_DEFAULT_WHITE);
    }

    int nTableID = pCW->m_pConvertTo->CreateTable(1, 1);
    pCW->m_pConvertTo->SetTableID(nTableID);
    pCW->m_pConvertTo->SetTableRow(1);
    pCW->m_pConvertTo->SetTableCol(1);

    nTableID = pCW->m_pConvertTo->GetTableID(); 
    pCW->m_pConvertTo->CellBackgroundColor(pCW->m_pConvertTo->GetTableID(), 1, 1, clrf);

    // Zelle mit Rahmen versehen
    pCW->m_pConvertTo->SetCellBorderWidth(pCW->m_pConvertTo->GetTableID(), 1, 1, 10, 10, 10, 10);

  }
  else
  {
    if (lpLitToken->nCase == tCASE_BOXEND)
    {
      return pCW->m_pConvertTo->SetCursorAfterTable(-1);
    }
  }
  return false;
}

bool CConvertWorkerFromTag::Metrics(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  xtstring cParValue[4];
  xtstring sName;
  xtstring sKeyword = TAG_METRICS;

  if (cInTag.FindNoCase(TAG_METRICS_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_METRICS_NAME);

    GETPARAM1(TAG_METRICS_NAME, cParValue[0]);
    sName = cParValue[0];

		if (sName == TAG_METRICS_INCH)
      pCW->m_nActiveUnitOfLength = tAUOL_Inch;
		else if (sName == TAG_METRICS_MM)
      pCW->m_nActiveUnitOfLength = tAUOL_Milimeter;
		else if (sName == TAG_METRICS_PICA)
      pCW->m_nActiveUnitOfLength = tAUOL_Pica;
    else
      pCW->m_nActiveUnitOfLength = tAUOL_Default;
	}

	return false;

}

bool CConvertWorkerFromTag::ClassName(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  xtstring cParValue[4];
  xtstring sName, sType, sValue;
  xtstring sKeyword = TAG_CLASS;

  if (cInTag.FindNoCase(TAG_CLASS_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_CLASS_NAME);

    GETPARAM1(TAG_CLASS_NAME, cParValue[0]);
    sName = cParValue[0];

    GETPARAM1(TAG_CLASS_TYPE, cParValue[0]);
    sType = cParValue[0];

    GETPARAM1(TAG_CLASS_VALUE, cParValue[0]);
    sValue = cParValue[0];

		// das muss ja in Stylemanager gesetzt werden
    //pCW->m_pConvertTo->SetClassName(sName, sType, sValue);
    if (pCW->m_nWorkModus == tWM_Work)
    {
      // da machen wir nichts
    }
    else if (pCW->m_nWorkModus == tWM_StyleInPreProcess && pCW->m_cStyleManager.GetActualStyleToSet())
    {
      if (sName.size())
      {
        CXPubClassProperty cClass;
        cClass.SetName(sName);
        cClass.SetType(_ttoi(sType.c_str()));
        cClass.SetValue(sValue);
        pCW->m_cStyleManager.GetActualStyleToSet()->AddClassProperty(cClass);
      }
    }
	}
	return false;

}

bool CConvertWorkerFromTag::RootPath(CConvertWorkerFromTag* pCW,
                                     const xtstring& cInTag,
                                     void* lpToken)
{
  CHECKCALLPOINTER()

  xtstring cParValue[4];
  xtstring sName;
  xtstring sKeyword = TAG_SEARCHPATH;

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  if ( cInTag.FindNoCase(TAG_SEARCHPATH_PATH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_SEARCHPATH_PATH);

    GETPARAM1(TAG_SEARCHPATH_PATH, cParValue[0]);
    sName = cParValue[0];
    pCW->m_pConvertTo->SetRootPicturePath(sName);
    pCW->SetRootPicturePath(sName);
  }

  if ( cInTag.FindNoCase(TAG_SEARCHPATH_PATH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_SEARCHPATH_PATH);

    GETPARAM1(TAG_SEARCHPATH_PATH, cParValue[0]);
    sName = cParValue[0];
    pCW->m_pConvertTo->SetRootTemplatePath(sName);
    pCW->SetRootTemplatePath(sName);
  }
  return false;

}

bool CConvertWorkerFromTag::LoadTemplate(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_TEMPLATE;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 
  xtstring cParValue[4];

  xtstring sName;
  if ( cInTag.FindNoCase(TAG_TEMPLATE_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEMPLATE_NAME);
    GETPARAM1(TAG_TEMPLATE_NAME, cParValue[0]);

    sName = cParValue[0];
    if(!pCW->IsPathFile(sName.c_str()))
    {
      xtstring sRootTemplatePath;
      pCW->GetRootTemplatePath(sRootTemplatePath);
      if (sRootTemplatePath.size())
        pCW->SetPathCombine(sName, sRootTemplatePath, sName);
      if(!pCW->IsPathFile(sName.c_str()))
        return false;
    }
  }

  int nMode=TAG_TEMPLATE_APPEND_MODE; //append: 0 = Angegebenes PDF wird am Ende eingefügt
  if ( cInTag.FindNoCase(TAG_TEMPLATE_MODE) != xtstring::npos)
  {
    //repeat: -1 =  PDF wird auf jeder Seite ausgegeben
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEMPLATE_MODE);
    GETPARAM1(TAG_TEMPLATE_MODE, cParValue[0]);
    
    if (cParValue[0].FindNoCase(TAG_TEMPLATE_MODE_APPEND) != xtstring::npos)
    {
      nMode = TAG_TEMPLATE_APPEND_MODE;
    }
    else if (cParValue[0].FindNoCase(TAG_TEMPLATE_MODE_REPEAT) != xtstring::npos)
    {
      nMode = TAG_TEMPLATE_REPEAT_MODE;
    }
    else if (cParValue[0].FindNoCase(TAG_TEMPLATE_MODE_OVERLAY) != xtstring::npos)
    {
      nMode = TAG_TEMPLATE_OVERLAY_MODE;
    }
    else
    {
      nMode = atol(cParValue[0].c_str());
      if (nMode == TAG_TEMPLATE_APPEND_MODE || nMode != TAG_TEMPLATE_REPEAT_MODE)
        nMode=TAG_TEMPLATE_APPEND_MODE;
    }
  }

  xtstring sLayer;
  if ( cInTag.FindNoCase(TAG_TEMPLATE_LAYER) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEMPLATE_LAYER);

    GETPARAM1(TAG_TEMPLATE_LAYER, cParValue[0]);
    sLayer = cParValue[0].c_str();
  }

  xtstring sOwnerPwd;
  if (cInTag.FindNoCase(TAG_TEMPLATE_OPEN_PWD) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEMPLATE_OPEN_PWD);

    GETPARAM1(TAG_TEMPLATE_OPEN_PWD, cParValue[0]);
    sOwnerPwd = cParValue[0].c_str();
  }

  return pCW->m_pConvertTo->LoadTemplate(sName, sLayer, sOwnerPwd, nMode);

}

bool CConvertWorkerFromTag::Line(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_LINE;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 
  xtstring cParValue[4];
  int nX1=-1, nX2=-1, nY1=-1, nY2=-1;

  // X1 
  if ( cInTag.FindNoCase(TAG_LINE_X1) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LINE_X1);
    GETPARAM1(TAG_LINE_X1, cParValue[0]);
    nX1 = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }
  // X2
  if ( cInTag.FindNoCase(TAG_LINE_X2) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LINE_X2);
    GETPARAM1(TAG_LINE_X2, cParValue[0]);
    nX2 = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }
  // Y1
  if ( cInTag.FindNoCase(TAG_LINE_Y1) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LINE_Y1);
    GETPARAM1(TAG_LINE_Y1, cParValue[0]);
    nY1 = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }
  // Y2
  if ( cInTag.FindNoCase(TAG_LINE_Y2) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LINE_Y2);
    GETPARAM1(TAG_LINE_Y2, cParValue[0]);
    nY2 = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }
  // width
  int nWidth=20;
  if ( cInTag.FindNoCase(TAG_LINE_WIDTH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LINE_WIDTH);
    GETPARAM1(TAG_LINE_WIDTH, cParValue[0]);
    nWidth = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }
  // color
  COLORREF LineColor=TAG_DEFAULT_BLACK;
  if ( cInTag.FindNoCase(TAG_LINE_COLOR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LINE_COLOR);
    GETPARAM(TAG_LINE_COLOR, cParValue[0]);
    LineColor = pCW->GetColor(cParValue[0]);
  }
  //<line x1="10" y1="10"  x2="5" y2="5"> 

  return pCW->m_pConvertTo->Line(nX1, nY1, nX2, nY2, nWidth, LineColor);
}

bool CConvertWorkerFromTag::TextFrame(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_POSFRAME_BREAK)
      sKeyword = TAG_TEXTFRAME_BREAK;
    else if (lpLitToken->nCase == tCASE_POSFRAME)
      sKeyword = TAG_TEXTFRAME_BEGIN;
    else if (lpLitToken->nCase == tCASE_POSFRAME_END)
      sKeyword = TAG_TEXTFRAME_END;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  xtstring cParValue[4];

  // Name der Textbox
  if (cInTag.FindNoCase(TAG_TEXTFRAME_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_NAME);
    GETPARAM1(TAG_TEXTFRAME_NAME, cParValue[0]);
    m_sTxtFrame = cParValue[0];
  }
  else
  {
    XTCHAR buff[100];
    // ltoa((long) ++m_nTxtFrame, buff, 10);
    _stprintf(buff, "%ld", ++m_nTxtFrame);
    m_sTxtFrame = _XT("$$XPub");
    m_sTxtFrame += buff;
  }

  if (cInTag.FindNoCase(TAG_TEXTFRAME_OVERSET) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_OVERSET);

    GETPARAM1(TAG_TEXTFRAME_OVERSET, cParValue[0]);
    xtstring sVariableName = cParValue[0]; // Variablenname heraustrennen

    CXPubVariant sVarValue((bool)false);
    if (pCW->m_pConvertTo->CheckTextFrameOverset(m_sTxtFrame)) // Überlauf
      sVarValue = (bool)true;

    map<xtstring, CTagVariable>::iterator cIt; 
    cIt = m_TagVarList.find(sVariableName);
    if (cIt != m_TagVarList.end()) // Variable ist vorhanden, Wert setzen
    {
      (*cIt).second.SetValue(sVarValue);
    }
    else // Variable ist nicht vorhanden, Wert anhängen
    {
      m_TagVarList[sVariableName] = sVarValue;
    }
    return true;
  }
 
  if (lpLitToken->nCase == tCASE_POSFRAME_BREAK)
  {  
    return pCW->m_pConvertTo->InsertPosFrameBreak();
  }

  if (lpLitToken->nCase == tCASE_POSFRAME_END)
  {
    // Style restore
    pCW->m_cStyleManager.RestoreStyle();
    // ganzen Segment entfernen
    pCW->m_cStyleManager.ReleaseSegmentInFormatStack();
    // und bekannt geben
    pCW->m_pConvertTo->StyleEvent(tSE_FormatStackSegmentRemoved, pCW->m_cStyleManager.GetActualFormatInStack());

    return pCW->m_pConvertTo->SetCursorPosition(TAG_CURSOR_END, 0);
  }

  // neuem Segment in FormatStack erzeugen
  // Segment faengt mit dem Default Style
  // Spaeter muss man aber auch SelectStyle machen.
  pCW->m_cStyleManager.CreateNewSegmentInFormatStack();

  int nLeft = -1, nTop = -1, nWidth = -1, nHeight = -1;

  int nWrap = TAG_TEXTFRAME_WRAP_C_NOTEXT; // idNone: no text wrap (default)
  if (cInTag.FindNoCase(TAG_TEXTFRAME_WRAP) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_WRAP);
    GETPARAM1(TAG_TEXTFRAME_WRAP, cParValue[0]);

    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_WRAP_NOTEXT) != xtstring::npos)
      nWrap = TAG_TEXTFRAME_WRAP_C_NOTEXT;
    else
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_WRAP_AROUNDBOX) != xtstring::npos)
      nWrap = TAG_TEXTFRAME_WRAP_C_AROUNDBOX;
    else
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_WRAP_CONTOUR) != xtstring::npos)
      nWrap = TAG_TEXTFRAME_WRAP_C_CONTOUR;
    else
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_WRAP_JUMPOBJECT) != xtstring::npos)
      nWrap = TAG_TEXTFRAME_WRAP_C_JUMPOBJECT;
    else
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_WRAP_JUMPNEXT) != xtstring::npos)
      nWrap = TAG_TEXTFRAME_WRAP_C_JUMPNEXT;
  }

  xtstring sJoin;
  if ( cInTag.FindNoCase(TAG_TEXTFRAME_JOIN) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_JOIN);

    GETPARAM1(TAG_TEXTFRAME_JOIN, cParValue[0]);
    sJoin = cParValue[0];
  }
  
  // Left
  if ( cInTag.FindNoCase(TAG_TEXTFRAME_LEFT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_LEFT);

    GETPARAM1(TAG_TEXTFRAME_LEFT, cParValue[0]);
    nLeft = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  // Top
  if ( cInTag.FindNoCase(TAG_TEXTFRAME_TOP) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_TOP);

    GETPARAM1(TAG_TEXTFRAME_TOP, cParValue[0]);
    nTop = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  // Width
  if ( cInTag.FindNoCase(TAG_TEXTFRAME_WIDTH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_WIDTH);

    GETPARAM1(TAG_TEXTFRAME_WIDTH, cParValue[0]);
    nWidth = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  // Height
  if ( cInTag.FindNoCase(TAG_TEXTFRAME_HEIGHT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_HEIGHT);

    GETPARAM1(TAG_TEXTFRAME_HEIGHT, cParValue[0]);
    nHeight = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  int LineThickness=0;  
  if ( cInTag.FindNoCase(TAG_TEXTFRAME_BORDERWIDTH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_BORDERWIDTH);

    GETPARAM1(TAG_TEXTFRAME_BORDERWIDTH, cParValue[0]);
    LineThickness = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  COLORREF LineColor = (COLORDEF)-1;
  if (LineThickness != 0)
    LineColor = TAG_DEFAULT_BLACK;

  if ( cInTag.FindNoCase(TAG_TEXTFRAME_BORDERCOLOR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_BORDERCOLOR);

    GETPARAM(TAG_TEXTFRAME_BORDERCOLOR, cParValue[0]);
    LineColor = pCW->GetColor(cParValue[0], TAG_DEFAULT_BLACK);
  }
  COLORREF FillColor = (COLORDEF)-1;
  if ( cInTag.FindNoCase(TAG_TEXTFRAME_FILLCOLOR) != xtstring::npos)
  {
   // wir holen hier den gesamten Parameter, um ihn in GetColor zu zerlegen
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_FILLCOLOR);

    GETPARAM(TAG_TEXTFRAME_FILLCOLOR, cParValue[0]); 
    FillColor = pCW->GetColor(cParValue[0], TAG_DEFAULT_WHITE); 
  } 

  int nHorzAlign = TAG_TEXTFRAME_HORALIGN_FLAG_LEFT;
  if (cInTag.FindNoCase(TAG_TEXTFRAME_HORALIGN) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_HORALIGN);

    GETPARAM1(TAG_TEXTFRAME_HORALIGN, cParValue[0]);

    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_HORALIGN_LEFT) != xtstring::npos)
      nHorzAlign = TAG_TEXTFRAME_HORALIGN_FLAG_LEFT;
    else
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_HORALIGN_MIDDLE) != xtstring::npos)
      nHorzAlign = TAG_TEXTFRAME_HORALIGN_FLAG_MIDDLE;
    else
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_HORALIGN_RIGHT) != xtstring::npos)
      nHorzAlign = TAG_TEXTFRAME_HORALIGN_FLAG_RIGHT;
  }

  int nVertAlign = TAG_TEXTFRAME_VERTALIGN_FLAG_TOP;
  if (cInTag.FindNoCase(TAG_TEXTFRAME_VERTALIGN) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_VERTALIGN);

    GETPARAM1(TAG_TEXTFRAME_VERTALIGN, cParValue[0]);

    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_VERTALIGN_TOP) != xtstring::npos)
      nVertAlign = TAG_TEXTFRAME_VERTALIGN_FLAG_TOP;
    else
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_VERTALIGN_CENTER) != xtstring::npos)
      nVertAlign = TAG_TEXTFRAME_VERTALIGN_FLAG_CENTER;
    else
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_VERTALIGN_BOTTOM) != xtstring::npos)
      nVertAlign = TAG_TEXTFRAME_VERTALIGN_FLAG_BOTTOM;
  }

  int nBase = TAG_TEXTFRAME_MARG;
  if ( cInTag.FindNoCase(TAG_TEXTFRAME_YPOS) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_YPOS);

    GETPARAM1(TAG_TEXTFRAME_YPOS, cParValue[0]);

    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_YPOS_MARGIN) != xtstring::npos)
      nBase = TAG_TEXTFRAME_MARG;
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_YPOS_PARA) != xtstring::npos)
      nBase = TAG_TEXTFRAME_PARA;
    if (cParValue[0].FindNoCase(TAG_TEXTFRAME_YPOS_PAGE) != xtstring::npos)
      nBase = TAG_TEXTFRAME_PAGE;
  }

// Rotieren von Text in Zelle
  int nDirection = 0;//TAG_TABLE_ROTATE_HORZ;
  if (cInTag.FindNoCase(TAG_TABLE_CELLROTATE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TABLE_CELLROTATE);

    GETPARAM1(TAG_TABLE_CELLROTATE, cParValue[0]);

    nDirection = atoi(cParValue[0].c_str());
/*
    if (cParValue[0].FindNoCase(TAG_TABLE_CELLROTATE_TOPBOT) != xtstring::npos)
      nDirection = TAG_TABLE_ROTATE_TOPTOBOT;

    if (cParValue[0].FindNoCase(TAG_TABLE_CELLROTATE_BOTTOP) != xtstring::npos)
      nDirection = TAG_TABLE_ROTATE_BOTTOTOP;
*/
  }
  
// einer von beiden Werten muss gesetzt sein
// MAK: muss evtl. noch in PDF/RTF als Standard verlagert werden
  if ((nWidth <= 0 && nHeight <= 0) || nWidth <= 0)
    nWidth = pCW->ConvertUnitOfLength((float)30);

  int nTxtFrameID;
  bool bReturn = 0 != (nTxtFrameID = pCW->m_pConvertTo->CreateTextFrame(m_sTxtFrame, sJoin,
            nLeft, nTop, nWidth, nHeight, nVertAlign, nHorzAlign,
            nBase, LineThickness, LineColor, FillColor, nWrap, nDirection));

  if (cInTag.FindNoCase(TAG_TEXTFRAME_COLUMNS_COUNT) != xtstring::npos && !m_sTxtFrame.empty())
  {
    int nColumns = - 1;
    int nGutter = - 1;
    int nWidth = -1;

    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_COLUMNS_COUNT);

    GETPARAM1(TAG_TEXTFRAME_COLUMNS_COUNT, cParValue[0]);
    nColumns = atol(cParValue[0].c_str());

    if (cInTag.FindNoCase(TAG_TEXTFRAME_COLUMNS_WIDTH) != xtstring::npos && !m_sTxtFrame.empty())
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_COLUMNS_WIDTH);

      GETPARAM1(TAG_TEXTFRAME_COLUMNS_WIDTH, cParValue[0]);
      nWidth = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

    if (cInTag.FindNoCase(TAG_TEXTFRAME_COLUMNS_GUTTER) != xtstring::npos && !m_sTxtFrame.empty())
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_TEXTFRAME_COLUMNS_GUTTER);

      GETPARAM1(TAG_TEXTFRAME_COLUMNS_GUTTER, cParValue[0]);
      nGutter = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    }

    pCW->m_pConvertTo->InsertTextFrameColumns(m_sTxtFrame, nColumns, nWidth, nGutter);
  }

  // Wenn wir da sind, wir haben TextFrame angelegt.
  // In FormatStack haben wir ganz am Anfang alles erledigt.
  // Jetzt muessen wir aber noch explizit bekannt geben, dass Style geaendert wurde.
  pCW->m_cStyleManager.BackUpStyle();
  pCW->m_cStyleManager.SetActualStyle(DEFAULT_STYLE_NAME);
  pCW->m_pConvertTo->StyleEvent(tSE_FormatStackStart, pCW->m_cStyleManager.GetActualFormatInStack());

  return bReturn;
}

bool CConvertWorkerFromTag::PageMargin(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_PAGE_MARGINS;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 
  xtstring cParValue[4];

  int nLeft = -1;
  int nTop = -1;
  int nRight = -1;
  int nButton = -1;

  if ( cInTag.FindNoCase(TAG_MARGIN_LEFT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_MARGIN_LEFT);
    GETPARAM1(TAG_MARGIN_LEFT, cParValue[0]);
    nLeft = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  if ( cInTag.FindNoCase(TAG_MARGIN_RIGHT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_MARGIN_RIGHT);
    GETPARAM1(TAG_MARGIN_RIGHT, cParValue[0]);
    nRight = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  if ( cInTag.FindNoCase(TAG_MARGIN_TOP) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_MARGIN_TOP);
    GETPARAM1(TAG_MARGIN_TOP, cParValue[0]);
    nTop = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  if ( cInTag.FindNoCase(TAG_MARGIN_BUTTON) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_MARGIN_BUTTON);
    GETPARAM1(TAG_MARGIN_BUTTON, cParValue[0]);
    nButton = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }
 
  return pCW->m_pConvertTo->Margins(nLeft, nRight, nTop, nButton);
}

bool CConvertWorkerFromTag::Document(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_DOCUMENT;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 
  xtstring cParValue[4];

  if (cInTag.FindNoCase(TAG_DOCUMENT_ORIENT) != xtstring::npos)
  {   
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_DOCUMENT_ORIENT);
    GETPARAM1(TAG_DOCUMENT_ORIENT, cParValue[0]);
    xtstring cValue = cParValue[0];

    int nOrient = TAG_SECTION_PORTRAIT;
    if (cValue.FindNoCase(TAG_DOCUMENT_LANDSCAPE) != xtstring::npos)
      nOrient = TAG_SECTION_LANDSCAPE;

    pCW->m_pConvertTo->SetSectionOrientation(nOrient);
  }

  if (cInTag.FindNoCase(TAG_DOCUMENT_HEIGHT) != xtstring::npos
      && cInTag.FindNoCase(TAG_DOCUMENT_WIDTH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_DOCUMENT_HEIGHT);
    GETPARAM1(TAG_DOCUMENT_HEIGHT, cParValue[0]);
    int nHeight = pCW->ConvertUnitOfLength(cParValue[0].c_str());

    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_DOCUMENT_WIDTH);
    GETPARAM1(TAG_DOCUMENT_WIDTH, cParValue[0]);
    int nWidth = pCW->ConvertUnitOfLength(cParValue[0].c_str());

    pCW->m_pConvertTo->SetPaperSize(0, nWidth, nHeight);
  }

  if (cInTag.FindNoCase(TAG_DOCUMENT_FORMAT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_DOCUMENT_FORMAT);
    GETPARAM1(TAG_DOCUMENT_FORMAT, cParValue[0]);
    int nDmConstant = 0;

    for (int i = 0; i < sizeof(_dcDocuments) / sizeof(_DefDocumentsPaper); i++)
    {
      if (cInTag.FindNoCase(_dcDocuments[i].szPaperName) != xtstring::npos)
      {     
        nDmConstant = i; break;
      }   
    }

    if(nDmConstant < sizeof(_dcDocuments) / sizeof(_DefDocumentsPaper))
    {
      pCW->m_pConvertTo->SetPaperSize(0, pCW->ConvertUnitOfLength(_dcDocuments[nDmConstant].nPaperWidth),
                      pCW->ConvertUnitOfLength(_dcDocuments[nDmConstant].nPaperHeight));
    }

  }
  return false;
}

bool CConvertWorkerFromTag::MergeField(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_MERGEFIELD;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 

  return pCW->m_pConvertTo->MergeField(cInTag);
}

bool CConvertWorkerFromTag::Field(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_FIELD;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 

  return pCW->m_pConvertTo->Field(cInTag);
}

int CConvertWorkerFromTag::CheckValidity(const xtstring& sVariableName)
{
  map<xtstring, CTagVariable>::iterator cIt; 
  cIt = m_TagVarList.find(sVariableName);
  if( cIt != m_TagVarList.end() ) // Variable ist vorhanden, Wert setzen
  {
    if( ((*cIt).second.GetValue()) == true)
      return VALID_TRUE;
    if( ((*cIt).second.GetValue()) == false)
      return VALID_FALSE;
  }
  return VALID_ERROR;

}

bool CConvertWorkerFromTag::FormField(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_FORMFIELD;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 

  xtstring cParValue[4];
  xtstring sName, sValue, sType, sMask;
  
  int nCodePage=0;
  if ( cInTag.FindNoCase(TAG_FORMFIELD_CODEPAGE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_CODEPAGE);
    GETPARAM1(TAG_FORMFIELD_CODEPAGE, cParValue[0]);
    sName = cParValue[0];
    nCodePage=atoi(sName.c_str());
  }

  if ( cInTag.FindNoCase(TAG_FORMFIELD_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_NAME);
    GETPARAM1(TAG_FORMFIELD_NAME, cParValue[0]);
    sName = cParValue[0];
  
    if(sName.find(_XT("()")) != xtstring::npos)
    {
      sName.ReplaceNoCaseAll(_XT("()"), _XT(""));

      map<xtstring, xtstring>::iterator cIt; 
      cIt = m_TagPDFTableList.find(sName);
      if( cIt != m_TagPDFTableList.end() ) // Variable ist vorhanden, Wert setzen
      {
        int nIndex = atoi(((*cIt).second).c_str());
        sValue.Format(_XT("%ld"), ++nIndex);
        m_TagPDFTableList[sName] = sValue;
        sName += sValue;
      }
      else // Variable ist nicht vorhanden, Wert anhängen
      {
        m_TagPDFTableList[sName] = _XT("1");
        sName += _XT("1");
      }
    }
  }

  if ( cInTag.FindNoCase(TAG_FORMFIELD_POSITION) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_POSITION);
    GETPARAM2(TAG_FORMFIELD_POSITION, cParValue[0], cParValue[1]);

    long lDeltaX, lDeltaY;

    lDeltaX = pCW->ConvertUnitOfLength(cParValue[0].c_str());
    lDeltaY = pCW->ConvertUnitOfLength(cParValue[1].c_str());

// wenn kein Name, dann werden alle Felder um angegebene Position verschoben  
    return pCW->m_pConvertTo->MoveFormField(sName, lDeltaX, lDeltaY);

  }

  if ( cInTag.FindNoCase(TAG_FORMFIELD_VALUE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_VALUE);
    GETPARAM(TAG_FORMFIELD_VALUE, cParValue[0]);
    sValue = cParValue[0];
    if(pCW->m_pConvertTo->GetFormaterType() == PDF_Formater)
    {
  // Zeilenumbruch im MLE
      sValue.ReplaceNoCaseAll(_XT("\n"), _XT("\\n"));
      sValue.ReplaceNoCaseAll(_XT("&quot;"), _XT("\""));
      sValue.ReplaceNoCaseAll(_XT("&gt;"), _XT(">"));
      sValue.ReplaceNoCaseAll(_XT("&lt;"), _XT("<"));
      sValue.ReplaceNoCaseAll(_XT("\x0d\x0a"), _XT("\\n"));
    }
  }

  if ( cInTag.FindNoCase(TAG_FORMFIELD_OVERSET) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_OVERSET);
    GETPARAM1(TAG_FORMFIELD_OVERSET, cParValue[0]);
    xtstring sVariableName = cParValue[0]; // Variablenname heraustrennen

    CXPubVariant sVarValue((bool)false);
    if(pCW->m_pConvertTo->CheckFormFieldOverset(sName, sValue)) // Überlauf
      sVarValue = (bool)true;
   
    map<xtstring, CTagVariable>::iterator cIt; 
    cIt = m_TagVarList.find(sVariableName);
    if( cIt != m_TagVarList.end() ) // Variable ist vorhanden, Wert setzen
    {
      (*cIt).second.SetValue(sVarValue);
    }
    else // Variable ist nicht vorhanden, Wert anhängen
    {
      m_TagVarList[sVariableName] = sVarValue;
    }
    return true;
  }

// Achtung die Folgenden müssen zusammen betrachtet werden
// wegen: "no..."
  if ( cInTag.FindNoCase(TAG_FORMFIELD_NOVALID) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_NOVALID);
    GETPARAM1(TAG_FORMFIELD_NOVALID, cParValue[0]);
    xtstring sVariableName = cParValue[0]; // Variablenname heraustrennen
    int nCheckValue = CheckValidity(sVariableName);
    if(nCheckValue == VALID_FALSE)
        return true;
    if(nCheckValue == VALID_ERROR)
      return false;
  }
  else
  if ( cInTag.FindNoCase(TAG_FORMFIELD_VALID) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_VALID);
    GETPARAM1(TAG_FORMFIELD_VALID, cParValue[0]);
    xtstring sVariableName = cParValue[0]; // Variablenname heraustrennen
    int nCheckValue = CheckValidity(sVariableName);
    if(nCheckValue == VALID_TRUE)
      return true;
    if(nCheckValue == VALID_ERROR)
      return false;
  }

  if ( cInTag.FindNoCase(TAG_FORMFIELD_TYPE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_TYPE);
    GETPARAM1(TAG_FORMFIELD_TYPE, cParValue[0]);
    sType = cParValue[0];
  }

  if ( cInTag.FindNoCase(TAG_FORMFIELD_MASK) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_MASK);
    GETPARAM1(TAG_FORMFIELD_MASK, cParValue[0]);
    sMask = cParValue[0];
  }

  int nLength = 0;
  if (cInTag.FindNoCase(TAG_FORMFIELD_LENGTH) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_LENGTH);
    GETPARAM1(TAG_FORMFIELD_LENGTH, cParValue[0]);
    nLength = atoi(cParValue[0].c_str());
  }

  int nFontsize = -1;
  if (cInTag.FindNoCase(TAG_FORMFIELD_FONTSIZE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_FONTSIZE);
    GETPARAM1(TAG_FORMFIELD_FONTSIZE, cParValue[0]);
    nFontsize = atoi(cParValue[0].c_str());
  }

  xtstring sFontName;
  if (cInTag.FindNoCase(TAG_FORMFIELD_FONTNAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_FONTNAME);
    GETPARAM1(TAG_FORMFIELD_FONTNAME, cParValue[0]);
    sFontName = cParValue[0].c_str();
  }

  if (cInTag.FindNoCase(TAG_FORMFIELD_ALTERNATE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_FORMFIELD_ALTERNATE);
    GETPARAM(TAG_FORMFIELD_ALTERNATE, cParValue[0]);
    CXPubVariant sVarValue((bool)false);
    if(pCW->m_pConvertTo->CheckFormFieldOverset(sName, sValue)) // Überlauf
    {
      sValue.ReplaceNoCaseAll(_XT("\\n"), _XT("<l>"));
      m_TagOversetList[sName] = sValue;
      sValue = cParValue[0];
    }
  }
  if(!sName.empty())
    return pCW->m_pConvertTo->FormField(sName, sValue, sType, sMask, nLength, sFontName, nFontsize, nCodePage);

  return false;
}

bool CConvertWorkerFromTag::InsertLink(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER() 

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_LINK)
      sKeyword = TAG_PDF_LINK_BEGIN;
    else if (lpLitToken->nCase == tCASE_LINK_END)
      sKeyword = TAG_PDF_LINK_END;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  if (pCW->m_pKeywords)
  {
    xtstring sKeyword = TAG_PDF_LINK_BEGIN;
    sKeyword.erase(sKeyword.begin());
    if (!pCW->KeywordIsFreeware(sKeyword))
      pCW->m_bOnlyFreewareKeywordsUsed = false;
  }

  xtstring cParValue[4];

  if(lpLitToken->nCase == tCASE_LINK_END)
  {
    return pCW->m_pConvertTo->InsertLink(xtstring(_XT("")), xtstring(_XT("")), 0, 0);
  }

  xtstring sReferenzName;
  // TAG_PDF_LINK_NAME --> "name="
  if ( cInTag.FindNoCase(TAG_PDF_LINK_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_PDF_LINK_NAME);
    GETPARAM1(TAG_PDF_LINK_NAME, cParValue[0]);
    sReferenzName = cParValue[0];
  }

  xtstring sSymbolName;
  // TAG_PDF_LINK_SYMBOL --> "anchor="
  if ( cInTag.FindNoCase(TAG_PDF_LINK_SYMBOL) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_PDF_LINK_SYMBOL);
    GETPARAM1(TAG_PDF_LINK_SYMBOL, cParValue[0]);
    sSymbolName = cParValue[0];
  }

  int nPage=0;
  // TAG_PDF_LINK_PAGE --> "pagenbr="
  if (cInTag.FindNoCase(TAG_PDF_LINK_PAGE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_PDF_LINK_PAGE);
    GETPARAM1(TAG_PDF_LINK_PAGE, cParValue[0]);
    nPage = atoi(cParValue[0].c_str());
  }

  int nZoom=0;
  // TAG_PDF_LINK_ZOOM --> "zoom="
  if (cInTag.FindNoCase(TAG_PDF_LINK_ZOOM) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_PDF_LINK_ZOOM);
    GETPARAM1(TAG_PDF_LINK_ZOOM, cParValue[0]);
    nZoom = atol(cParValue[0].c_str());
  }

  return pCW->m_pConvertTo->InsertLink(sReferenzName, sSymbolName, nPage, nZoom);

}

bool CConvertWorkerFromTag::InsertAnchor(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER() 

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_ANCHOR_BEGIN;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  if (pCW->m_pKeywords)
  {
    xtstring sKeyword = TAG_ANCHOR_BEGIN;
    sKeyword.erase(sKeyword.begin());
    if (!pCW->KeywordIsFreeware(sKeyword))
      pCW->m_bOnlyFreewareKeywordsUsed = false;
  }

  xtstring cParValue[4];

  xtstring sAnchorName;
  // TAG_ANCHOR_NAME --> "name="
  if ( cInTag.FindNoCase(TAG_ANCHOR_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_ANCHOR_NAME);
    GETPARAM1(TAG_ANCHOR_NAME, cParValue[0]);
    sAnchorName = cParValue[0];
  }
  if(sAnchorName.size())
    return pCW->m_pConvertTo->InsertAnchor(sAnchorName);
  
  return false;

}

bool CConvertWorkerFromTag::InsertBookmark(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER() 

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_BOOKMARK;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  if (pCW->m_pKeywords)
  {
    xtstring sKeyword = TAG_BOOKMARK;
    sKeyword.erase(sKeyword.begin());
    if (!pCW->KeywordIsFreeware(sKeyword))
      pCW->m_bOnlyFreewareKeywordsUsed = false;
  }

  xtstring cParValue[4];

  xtstring sBookName;
  // TAG_BOOKMARK_TEXT --> "name="
  if ( cInTag.FindNoCase(TAG_BOOKMARK_TEXT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_BOOKMARK_TEXT);
    GETPARAM1(TAG_BOOKMARK_TEXT, cParValue[0]);
    sBookName = cParValue[0];
  }

  xtstring sSymbolName;
  // PDF_BOOKMARK_SYMBOL --> "anchor="
  if ( cInTag.FindNoCase(PDF_BOOKMARK_SYMBOL) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, PDF_BOOKMARK_SYMBOL);
    GETPARAM1(PDF_BOOKMARK_SYMBOL, cParValue[0]);
    sSymbolName = cParValue[0];
  }

  int nZoom = 0;
  // TAG_BOOKMARK_ZOOM --> "zoom="
  if (cInTag.FindNoCase(TAG_BOOKMARK_ZOOM) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_BOOKMARK_ZOOM);
    GETPARAM1(TAG_BOOKMARK_ZOOM, cParValue[0]);
    nZoom = atoi(cParValue[0].c_str());
  }

  bool bIgnorePos = true;
  // TAG_BOOKMARK_IGNOREPOS --> "ignore="
  if ( cInTag.FindNoCase(TAG_BOOKMARK_IGNOREPOS) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_BOOKMARK_IGNOREPOS);
    GETPARAM1(TAG_BOOKMARK_IGNOREPOS, cParValue[0]);
    if( cParValue[0].FindNoCase(TAG_FALSE) != xtstring::npos )
      bIgnorePos = false;
  }
 
  xtstring sLevel;
  // TAG_BOOKMARK_LEVEL --> "level="
  if ( cInTag.FindNoCase(TAG_BOOKMARK_LEVEL) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_BOOKMARK_LEVEL);
    GETPARAM1(TAG_BOOKMARK_LEVEL, cParValue[0]);
    sLevel = cParValue[0];
  } 
 
  int nFormat = PDF_BOOKMARK_NORMAL;
  // TAG_BOOKMARK_FORMAT --> "format="
  if ( cInTag.FindNoCase(TAG_BOOKMARK_FORMAT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_BOOKMARK_FORMAT);
    GETPARAM1(TAG_BOOKMARK_FORMAT, cParValue[0]);
    xtstring sFormat = cParValue[0];
    if( cParValue[0].FindNoCase(TAG_BOOKMARK_FORMAT_BOLD) != xtstring::npos )
      nFormat = PDF_BOOKMARK_BOLD;
    if( cParValue[0].FindNoCase(TAG_BOOKMARK_FORMAT_ITALIC) != xtstring::npos )
      nFormat = PDF_BOOKMARK_ITALIC;
    if( cParValue[0].FindNoCase(TAG_BOOKMARK_FORMAT_BOTH) != xtstring::npos )
      nFormat = PDF_BOOKMARK_ITALIC_BOLD;
  } 

  xtstring sColor;
  // TAG_BOOKMARK_COLOR --> "color="
  if ( cInTag.FindNoCase(TAG_BOOKMARK_COLOR) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_BOOKMARK_COLOR);
    GETPARAM(TAG_BOOKMARK_COLOR, cParValue[0]);
    sColor = cParValue[0];
  }

  return pCW->m_pConvertTo->InsertBookmark(sLevel, sBookName, sSymbolName, nZoom, bIgnorePos, pCW->GetColor(sColor), nFormat);

}

bool CConvertWorkerFromTag::Layer(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_LAYER)
      sKeyword = TAG_LAYER_BEGIN;
    else if (lpLitToken->nCase == tCASE_LAYER_END)
      sKeyword = TAG_LAYER_END;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  if (lpLitToken->nCase == tCASE_LAYER_END)
  {
    // Style restore
    pCW->m_cStyleManager.RestoreStyle();
    // ganzen Segment entfernen
    pCW->m_cStyleManager.ReleaseSegmentInFormatStack();
    // und bekannt geben
    pCW->m_pConvertTo->StyleEvent(tSE_FormatStackSegmentRemoved, pCW->m_cStyleManager.GetActualFormatInStack());

    return pCW->m_pConvertTo->LayerEnd();
  }

  // neuem Segment in FormatStack erzeugen
  // Segment faengt mit dem Default Style
  // Spaeter muss man aber auch SelectStyle machen.
  pCW->m_cStyleManager.CreateNewSegmentInFormatStack();

  xtstring cParValue[4];

  xtstring sName;
  if ( cInTag.FindNoCase(TAG_LAYER_NAME) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_NAME);

    GETPARAM(TAG_LAYER_NAME, cParValue[0]);
    sName = cParValue[0];
  }

  xtstring sGroup;
  if ( cInTag.FindNoCase(TAG_LAYER_GROUP) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_GROUP);

    GETPARAM(TAG_LAYER_GROUP, cParValue[0]);
    sGroup = cParValue[0];
  }

  xtstring sParent;
  if ( cInTag.FindNoCase(TAG_LAYER_PARENT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_PARENT);

    GETPARAM(TAG_LAYER_PARENT, cParValue[0]);
    sParent = cParValue[0];
  }

  int nIntent = TAG_LAYER_INTENT_FLAG_VIEW; // PDF7: View, Reference
  if ( cInTag.FindNoCase(TAG_LAYER_INTENT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_INTENT);

    GETPARAM(TAG_LAYER_INTENT, cParValue[0]);
    if (cParValue[0].FindNoCase(TAG_LAYER_INTENT_REFERENCE) != xtstring::npos)
      nIntent = TAG_LAYER_INTENT_FLAG_REFERENCE; 
  }

  int nDefState = TAG_LAYER_FLAG_ON;        // Default state: ON/OFF
  if ( cInTag.FindNoCase(TAG_LAYER_DEFSTATE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_DEFSTATE);

    GETPARAM(TAG_LAYER_DEFSTATE, cParValue[0]);
    if(cParValue[0].FindNoCase(TAG_LAYER_OFF) != xtstring::npos)
      nDefState = TAG_LAYER_FLAG_OFF; 
  }

  int nLocked = TAG_LAYER_FLAG_OFF;         // PDF7: ON/OFF
  if ( cInTag.FindNoCase(TAG_LAYER_LOCKED) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_LOCKED);

    GETPARAM(TAG_LAYER_LOCKED, cParValue[0]);
    if(cParValue[0].FindNoCase(TAG_LAYER_ON) != xtstring::npos)
      nLocked = TAG_LAYER_FLAG_ON; 
  }

  int nVisible = TAG_LAYER_INITIALSTATE_FLAG_VISIBLE_WHEN_ON;     // Visible when on, Never Visible, Always Visible
  if ( cInTag.FindNoCase(TAG_LAYER_VISIBLE) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_VISIBLE);

    GETPARAM(TAG_LAYER_VISIBLE, cParValue[0]);
    if (cParValue[0].FindNoCase(TAG_LAYER_INITIALSTATE_NEVER_VISIBLE) != xtstring::npos)
      nVisible = TAG_LAYER_INITIALSTATE_FLAG_NEVER_VISIBLE; 
    if (cParValue[0].FindNoCase(TAG_LAYER_INITIALSTATE_ALWAYS_VISIBLE) != xtstring::npos)
      nVisible = TAG_LAYER_INITIALSTATE_FLAG_ALWAYS_VISIBLE; 
  }

  int nPrint = TAG_LAYER_INITIALSTATE_FLAG_VISIBLE_WHEN_ON;       // Visible when on, Never Visible, Always Visible
  if ( cInTag.FindNoCase(TAG_LAYER_PRINT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_PRINT);

    GETPARAM(TAG_LAYER_PRINT, cParValue[0]);
    if (cParValue[0].FindNoCase(TAG_LAYER_INITIALSTATE_NEVER_VISIBLE) != xtstring::npos)
      nPrint = TAG_LAYER_INITIALSTATE_FLAG_NEVER_VISIBLE; 
    if (cParValue[0].FindNoCase(TAG_LAYER_INITIALSTATE_ALWAYS_VISIBLE) != xtstring::npos)
      nPrint = TAG_LAYER_INITIALSTATE_FLAG_ALWAYS_VISIBLE; 
  }

  int nExport = TAG_LAYER_INITIALSTATE_FLAG_VISIBLE_WHEN_ON;      // Visible when on, Never Visible, Always Visible
  if ( cInTag.FindNoCase(TAG_LAYER_EXPORT) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_LAYER_EXPORT);

    GETPARAM(TAG_LAYER_EXPORT, cParValue[0]);
    if (cParValue[0].FindNoCase(TAG_LAYER_INITIALSTATE_NEVER_VISIBLE) != xtstring::npos)
      nExport = TAG_LAYER_INITIALSTATE_FLAG_NEVER_VISIBLE; 
    if (cParValue[0].FindNoCase(TAG_LAYER_INITIALSTATE_ALWAYS_VISIBLE) != xtstring::npos)
      nExport = TAG_LAYER_INITIALSTATE_FLAG_ALWAYS_VISIBLE; 
  }

  bool bRetVal = pCW->m_pConvertTo->Layer(sName, sGroup, sParent, nIntent, nDefState, nLocked, nVisible, nPrint, nExport);

  // Wenn wir da sind, wir haben Layer angelegt.
  // In FormatStack haben wir ganz am Anfang alles erledigt.
  // Jetzt muessen wir aber noch explizit bekannt geben, dass Style geaendert wurde.
  pCW->m_cStyleManager.BackUpStyle();
  pCW->m_cStyleManager.SetActualStyle(DEFAULT_STYLE_NAME);
  pCW->m_pConvertTo->StyleEvent(tSE_FormatStackStart, pCW->m_cStyleManager.GetActualFormatInStack());

  return bRetVal;
}

bool CConvertWorkerFromTag::Password(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_PDF_PASSWORD;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  xtstring cParValue[4];

  xtstring sUser;
  if ( cInTag.FindNoCase(TAG_PDF_PASSWORD_USER) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_PDF_PASSWORD_USER);
    GETPARAM1(TAG_PDF_PASSWORD_USER, cParValue[0]);
    sUser = cParValue[0];
  }

  xtstring sMaster;
  if ( cInTag.FindNoCase(TAG_PDF_PASSWORD_MASTER) != xtstring::npos)
  {
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_PDF_PASSWORD_MASTER);
    GETPARAM1(TAG_PDF_PASSWORD_MASTER, cParValue[0]);
    sMaster = cParValue[0];
  }

  typedef enum
{
  ePrint        = 0x01,
  eModify       = 0x02,
  eCopy         = 0x04,
  eAnnotation   = 0x08,
  eForm         = 0x10,
  eExtract      = 0x20,
  eAssemble     = 0x40,
  ePrint2       = 0x80
} ePDFPERMISSION;

  int nPermission = 0;
  if ( cInTag.FindNoCase(TAG_PDF_PASSWORD_PERMISSION) != xtstring::npos)
  {
    for (size_t nI = 0; nI < 4; nI++)
    {
      // parameter freeware kontrolle
      if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
        pCW->CheckParameter(sKeyword, TAG_PDF_PASSWORD_PERMISSION);
      GETPARAM(TAG_PDF_PASSWORD_PERMISSION, cParValue[nI]);

      if(cParValue[nI].FindNoCase(TAG_PDF_PASSWORD_PERMISSION_ANNOT) != xtstring::npos)
        nPermission |= eAnnotation;
      if(cParValue[nI].FindNoCase(TAG_PDF_PASSWORD_PERMISSION_ASSEM) != xtstring::npos)
        nPermission |= eAssemble;
      if(cParValue[nI].FindNoCase(TAG_PDF_PASSWORD_PERMISSION_COPY) != xtstring::npos)
        nPermission |= eCopy;
      if(cParValue[nI].FindNoCase(TAG_PDF_PASSWORD_PERMISSION_EXTRACT) != xtstring::npos)
        nPermission |= eExtract;
      if(cParValue[nI].FindNoCase(TAG_PDF_PASSWORD_PERMISSION_FORM) != xtstring::npos)
        nPermission |= eForm;
      if(cParValue[nI].FindNoCase(TAG_PDF_PASSWORD_PERMISSION_MODIFY) != xtstring::npos)
        nPermission |= eModify;
      if(cParValue[nI].FindNoCase(TAG_PDF_PASSWORD_PERMISSION_PRINTLOW) != xtstring::npos)
        nPermission |= ePrint;
      if(cParValue[nI].FindNoCase(TAG_PDF_PASSWORD_PERMISSION_PRINTHIGH) != xtstring::npos)
        nPermission = nPermission | ePrint2 | ePrint;
    }

  }
  return pCW->m_pConvertTo->SetPassword(sMaster, sUser, nPermission);
}

bool CConvertWorkerFromTag::DocumentInfo(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_RTF_DOC_SETTINGS;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 

  return pCW->m_pConvertTo->DocumentInfo(cInTag);
}

bool CConvertWorkerFromTag::SetPageCount(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_PAGES;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 

  return pCW->m_pConvertTo->SetPageCount(1);
}

bool CConvertWorkerFromTag::SetCurrentPage(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_PAGENUMBER;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 
  // not used. lpToken; 
  return pCW->m_pConvertTo->SetCurrentPage(1);
}

bool CConvertWorkerFromTag::EditSection(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_SECTION;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 
  xtstring cParValue[4];

  int NumCols = -1;
  int ColSpace = 0;
  int FirstPageNo = 0;

  if (cInTag.FindNoCase(TAG_SECTION_NUMCOL) != xtstring::npos)
  { 
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_SECTION_NUMCOL);
    GETPARAM1(TAG_SECTION_NUMCOL, cParValue[0]);
    NumCols = atoi(cParValue[0].c_str());
 // default für Abstand zwischen Spalten
    ColSpace = pCW->ConvertUnitOfLength((float)12);
  }

  if (cInTag.FindNoCase(TAG_SECTION_WIDTH) != xtstring::npos)
  { 
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_SECTION_WIDTH);
    GETPARAM1(TAG_SECTION_WIDTH, cParValue[0]);
    ColSpace = pCW->ConvertUnitOfLength(cParValue[0].c_str());
  }

  if (cInTag.FindNoCase(TAG_SECTION_PAGENBR) != xtstring::npos)
  { 
    // parameter freeware kontrolle
    if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
      pCW->CheckParameter(sKeyword, TAG_SECTION_PAGENBR);
    GETPARAM1(TAG_SECTION_PAGENBR, cParValue[0]);
    FirstPageNo = atoi(cParValue[0].c_str());
  }
  return pCW->m_pConvertTo->EditSection(NumCols, ColSpace, FirstPageNo);

}

bool CConvertWorkerFromTag::CanPageBreak(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_NOPAGEBREAK)
      sKeyword = TAG_NOPAGEBREAK;
    else if (lpLitToken->nCase == tCASE_NOPAGEBREAK_END)
      sKeyword = TAG_NOPAGEBREAK_END;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // not used. _TagTokens *lpLitToken = (_TagTokens*)lpToken; 

  if (cInTag.FindNoCase(TAG_NOPAGEBREAK_END) != xtstring::npos)
    return pCW->m_pConvertTo->CanPageBreak(true);

  if (cInTag.FindNoCase(TAG_NOPAGEBREAK) != xtstring::npos)
    return pCW->m_pConvertTo->CanPageBreak(false);

  return true;
}

bool CConvertWorkerFromTag::GoWritePosition(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_GOWRITEPOSITION;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  pCW->m_pConvertTo->FillPageBreak();
  
  xtstring sValue;
  pCW->m_pConvertTo->GetFormFieldValue(xtstring(_XT("formwriteposition")), sValue);
  if(sValue.empty())
    return pCW->m_pConvertTo->InsertPageBreak();

  int nPos = pCW->ConvertUnitOfLength(sValue.c_str());
  pCW->m_pConvertTo->BeginPageBody(nPos);
    
  return true;
}

bool CConvertWorkerFromTag::InsertPageBreak(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    if (lpLitToken->nCase == tCASE_PAGEBREAK)
      sKeyword = TAG_PAGEBREAK;
    else if (lpLitToken->nCase == tCASE_FILLPAGEBREAK)
      sKeyword = TAG_FILLPAGEBREAK;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  if(lpLitToken->nCase == tCASE_FILLPAGEBREAK)
  {
    return pCW->m_pConvertTo->FillPageBreak();
  }
  else
    return pCW->m_pConvertTo->InsertPageBreak();

}

bool CConvertWorkerFromTag::InsertSectionBreak(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_SECTIONBREAK;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  // Es kommt zu Section Break
  bool bRet = pCW->m_pConvertTo->InsertSectionBreak();
  if (bRet)
  {
    pCW->m_cStyleManager.SetActualStyle(DEFAULT_STYLE_NAME);
    pCW->m_pConvertTo->StyleEvent(tSE_FormatStackStart, pCW->m_cStyleManager.GetActualFormatInStack());
  }
  return bRet;
}

bool CConvertWorkerFromTag::InsertColumnBreak(CConvertWorkerFromTag* pCW, const xtstring& cInTag, void* lpToken)
{
  CHECKCALLPOINTER()

  _TagTokens *lpLitToken = (_TagTokens*)lpToken;

  xtstring sKeyword;
  // keyword freeware kontrolle
  if (pCW->m_pKeywords && pCW->m_bOnlyFreewareKeywordsUsed)
  {
    sKeyword = TAG_COLUMNBREAK;
    pCW->CheckKeyword(sKeyword);
  }

  if (pCW->m_nWorkModus != tWM_Work)
    return true;

  return pCW->m_pConvertTo->InsertColumnBreak();
}



void CConvertWorkerFromTag::CheckKeyword(xtstring& sKeyword)
{
  if (!m_pKeywords)
    return;
  if (!m_bOnlyFreewareKeywordsUsed)
    return;

  sKeyword.Trim_AllChars(_T("< =>"));

  if (!KeywordIsFreeware(sKeyword))
    m_bOnlyFreewareKeywordsUsed = false;
}

void CConvertWorkerFromTag::CheckParameter(const xtstring& sKeyword,
                                           const xtstring& sParameter)
{
  if (!m_pKeywords)
    return;
  if (!m_bOnlyFreewareKeywordsUsed)
    return;

  xtstring sP(sParameter);
  sP.Trim_AllChars(_T("< =>"));

  if (!ParameterIsFreeware(sKeyword, sP))
    m_bOnlyFreewareKeywordsUsed = false;
}







int CConvertWorkerFromTag::ConvertUnitOfLength(LPCXTCHAR lpValue)
{
  int nConvert = 0;
  if (!lpValue)
    return nConvert;

  double nInput;
  nInput = atof(lpValue);
  return ConvertUnitOfLength(nInput);
}

int CConvertWorkerFromTag::ConvertUnitOfLength(double nValue)
{
  int nConvert = 0;

  switch (m_nActiveUnitOfLength)
  {
  case tAUOL_Milimeter:
    nConvert = MilimeterToTwips(nValue);
    break;
  case tAUOL_Inch:
    nConvert = InchToTwips(nValue);
    break;
  case tAUOL_Pica:
    nConvert = PicaToTwips(nValue);
    break;
  }
  return nConvert;
}

int CConvertWorkerFromTag::MilimeterToTwips(double nValue)
{
  return ((int) ((((double)nValue) * 1440 * 10) / (double)254));
}
int CConvertWorkerFromTag::InchToTwips(double nValue)
{
  // ins milimeter
  nValue *= 25.4;
  return ((int) ((((double)nValue) * 1440 * 10) / (double)254));
}
int CConvertWorkerFromTag::PicaToTwips(double nValue)
{
  long nRet = 0;

  if (nValue <= ( LONG_MAX / 240 ) && nValue >= ( LONG_MIN / 240 ) )
    nRet = (long)(nValue * (double)240);
  return nRet;
}



/*
int CConvertWorkerFromTag::TwipsToMM(int wTwips)
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
  INT64 nRet = wTwips;
  nRet *= 254;
  nRet /= 14400;
  return (int)nRet;
#elif defined(WIN32)
  return ::MulDiv(wTwips, 254, 14400);
#else
#error "not implemented"
#endif
}

int CConvertWorkerFromTag::MMToTwips(LPCXTCHAR lpValue)
{
  int wValue = atoi(lpValue);
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
  INT64 nRet = wValue;
  nRet *= 14400;
  nRet /= 254;
  return (int)nRet;
#elif defined(WIN32)
  return ::MulDiv(wValue, 14400, 254);
#else
#error "not implemented"
#endif
}

int CConvertWorkerFromTag::MMToTwips(int nMM)
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
  INT64 nRet = nMM;
  nRet *= 14400;
  nRet /= 254;
  return (int)nRet;
#elif defined(WIN32)
  return ::MulDiv(nMM, 14400, 254);
#else
#error "not implemented"
#endif
}

int CConvertWorkerFromTag::MMToTwips(float nMM)
{
  return ((int)((((float)nMM)*1440*10)/(float)254));
}
*/
