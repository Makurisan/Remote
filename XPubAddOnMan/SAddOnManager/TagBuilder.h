#if !defined(_TAGBUILDER_H_)
#define _TAGBUILDER_H_

#include "AddOnManagerExpImp.h"

#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"
#include "XPubAddOnManager.h"


#define PDF_PERMISSION_NONE					    0x00	// Nichts erlauben
#define PDF_PERMISSION_PRINTING					0x01	// Drucken
#define PDF_PERMISSION_EDITING					0x02	// Ändern des Dokumentes
#define PDF_PERMISSION_COPYING					0x04	// Kopieren bzw. Entnehmen von Inhalt
#define PDF_PERMISSION_COMMENTING				0x08	// Kommentieren
#define PDF_PERMISSION_FORMINPUT				0x10	// Formularfelder ausfüllen
#define PDF_PERMISSION_CONTENTEXTRACT		0x20	// Zugriff auf Inhalt
#define PDF_PERMISSION_ASSEMBLE					0x40	// Dokumentzusammenstellung
#define PDF_PERMISSION_LOWPRINTING			0x80  // Drucken mit niedriger Auflösung

class XPUBADDONMANAGER_EXPORTIMPORT CTagBuilder
{
public:

  enum BuildFormats
  {
    RTF,
    PDF,
    INDESIGN,
    FDF,
  };

	CTagBuilder(CConvertWorkerProgress* pConvertProgress);
	~CTagBuilder(void);

	bool Attach(CXPubAddOnManager* pAddOnManager);
	void Detach();

	bool Open(BuildFormats eFormat);
	bool Close();

	bool Set(const xtstring& sInput);	// set Tag data
	bool Get(char *&pData, int& lSize);
	bool Get(xtstring& sFormatData);
	bool Save(const xtstring& sFileName);
	bool Copy(); // copy to clipboard

  void SetProgress(CConvertWorkerProgress* pConvertProgress);

	bool SetDocumentProperty(const TDocumentProperty nTypeOfProperty, const xtstring& sValue);
	bool SetPassword(const xtstring& sOwner, const xtstring& sUser, int lPermissions);
  bool GetDocumentStrings(CxtstringVector& vStringArray);
  void SetRootTemplatePath(const xtstring& sRootTemplatePath){ m_sRootTemplatePath = sRootTemplatePath;};
  void SetRootPicturePath(const xtstring& sRootPicturePath){ m_sRootPicturePath = sRootPicturePath;};

  void SetKeywords(TagKeywords* pKeywords){m_pKeywords = pKeywords;if (m_pFrom)m_pFrom->SetKeywords(m_pKeywords);};
  bool GetOnlyFreewareKeywordsUsed(){if (m_pFrom)return m_pFrom->GetOnlyFreewareKeywordsUsed(); return true;}

  void SetRestrictFeatures(bool bRestrictFeatures){m_bRestrictFeatures = bRestrictFeatures; if (m_pFrom) m_pFrom->SetRestrictFeatures(m_bRestrictFeatures);};
protected:

	CXPubAddOnManager*      m_pAddOnManager;
	CConvertWorkerProgress* m_pConvertProgress;

private:
	CConvertWorkerFrom* m_pFrom;
	CConvertWorkerTo* m_pTo;
  TagKeywords* m_pKeywords;
  bool  m_bRestrictFeatures;
	TDataFormatForStoneAndTemplate m_eDataFormat;
  xtstring m_sRootTemplatePath;
  xtstring m_sRootPicturePath;
};



#endif // _TAGBUILDER_H_

