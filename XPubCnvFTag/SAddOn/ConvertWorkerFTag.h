#if !defined(_CONVERTWORKERFTAG_H_)
#define _CONVERTWORKERFTAG_H_

#define ConvertWorkerFTag_Version   0

#include "CnvFTagExpImp.h"

// todo
// 1. SetVersion "1.6", "1.5"
// 2. Font Embedding HOST, Acrobat
// 3. Absatzabstand
// 4. Abstand zwischen Zeichen, Laufweite
// 5. Absatzkontrolle bei PDF
// 6. Interner Umbau auf Strukturen bei Paragraph

class CConvertWorkerFromTag;


class CConvertFromTagAddOn : public CConvertFromAddOn
{
  friend class CConvertWorkerFromTag;
public:
	CConvertFromTagAddOn(CXPubAddOnManager* pAddOnManager);
	~CConvertFromTagAddOn();

	virtual CConvertWorkerFrom* CreateConvertWorkerFrom();
	virtual CConvertWorkerFrom* CreateConvertWorkerFromClone(CConvertWorkerFrom* pConvertWorker);
	virtual bool ReleaseConvertWorkerFrom(CConvertWorkerFrom* pConvertWorker);
};


#define    MmToInches(x)       (((float)(x))/(float)25.4)
#define    InchesToMm(x)       (((float)(x))*(float)25.4)

typedef struct
{
//  int nPaperSize;
  XTCHAR    *szPaperName;
  float nPaperWidth;
  float nPaperHeight;
} _DefDocumentsPaper;

static _DefDocumentsPaper _dcDocuments[] = 
{
  {/* 0*/  /*DMPAPER_LETTER,      */ _XT("Letter"), InchesToMm(8.5),  InchesToMm(11) },
  {/* 1*/  /*DMPAPER_LETTER,      */ _XT("Letter"), InchesToMm(8.5),  InchesToMm(11) },
  {/* 2*/  /*DMPAPER_LEGAL,       */ _XT("Legal"), InchesToMm(8.5),  InchesToMm(14) },
  {/* 3*/  /*DMPAPER_LEDGER,      */ _XT("Ledger"), InchesToMm(17),  InchesToMm(11) },
  {/* 4*/  /*DMPAPER_TABLOID,     */ _XT("Tabloid"), InchesToMm(11),  InchesToMm(17) },
  {/* 5*/  /*DMPAPER_STATEMENT,   */ _XT("Statement"), InchesToMm(5.5),  InchesToMm(8.5) },
  {/* 6*/  /*DMPAPER_EXECUTIVE,   */ _XT("Executive"), InchesToMm(7.25),  InchesToMm(10.5) },
  {/* 7*/  /*DMPAPER_A3,          */ _XT("A3"), 297,  420 },
  {/* 8*/  /*DMPAPER_A4,          */ _XT("A4"), 210,  297 },
  {/* 9*/  /*DMPAPER_A5,          */ _XT("A5"), 148,  210 },
  {/*10*/  /*DMPAPER_B4,          */ _XT("B4"), 250,  354 },
  {/*11*/  /*DMPAPER_B5,          */ _XT("B5"), 182,  257 },
  {/*12*/  /*DMPAPER_ENV_10,      */ _XT("Envelope 10"), InchesToMm(4.5),  InchesToMm(9.5) },
  {/*13*/  /*DMPAPER_ENV_DL,      */ _XT("Envelope DL"), 110,  220 },
  {/*14*/  /*DMPAPER_ENV_C5,      */ _XT("Envelope C5"), 162,  229 },
  {/*15*/  /*DMPAPER_ENV_MONARCH, */ _XT("Envelope Monarch"), InchesToMm(3.875),  InchesToMm(7.5) },
};

#define RGBDEF(r,g,b)          ((COLORDEF)((0xff & (r)|((0xff & (g))<<8))|((0xff & (b))<<16)))

typedef struct
{
  _ColorDef crCMYK;
  _ColorDef crRGB;
  XTCHAR    *szEN_Name; // englischer Name
  XTCHAR    *szGE_Name; // deutscher Name
} _ColourTableEntry;

class TableGridCoord
{
public:

	TableGridCoord(long topRow, long leftCol, long bottomRow, long right);
	TableGridCoord(long Row, long Col);
	
	void Set(long topRow, long leftCol, long bottomRow, long right);

protected:
	long Row;
	long Col;
	long bottomRow;
	long right;

};


// Die Namen muessen mit kleinen Buchstaben
static _ColourTableEntry _crColours[] = 
{
  { _ColorDef(75,68,67,90),		RGBDEF(0x00, 0x00, 0x00),     _XT("black")   ,    _XT("Schwarz") },

  { _ColorDef(29,100,100,38), RGBDEF(0x80, 0x00, 0x00),     _XT("maroon")  ,    _XT("Dunkelrot") },
  { _ColorDef(61,100,14,3),		RGBDEF(0x80, 0x00, 0x80),     _XT("purple")  ,    _XT("Violett") },
  { _ColorDef(25,20,20,0),		RGBDEF(0xC0, 0xC0, 0xC0),     _XT("silver")  ,    _XT("Hellgrau") },
  { _ColorDef(87,24,100,13),	RGBDEF(0x00, 0x80, 0x00),     _XT("green")   ,    _XT("Grün") },
  { _ColorDef(100,98,14,17),	RGBDEF(0x00, 0x00, 0x80),     _XT("navy")    ,    _XT("Blau") },

  { _ColorDef(52,43,43,8),		RGBDEF(0x80, 0x80, 0x80),     _XT("gray")    ,    _XT("Dunkelgrau") },
  { _ColorDef(0,0,0,0),				RGBDEF(0xFF, 0xFF, 0xFF),     _XT("white")   ,    _XT("Weiß") },
  { _ColorDef(0,99,100,0),		RGBDEF(0xFF, 0x00, 0x00),     _XT("red")     ,    _XT("Rot") },
  { _ColorDef(0,63,38,0),			RGBDEF(0xFF, 0x80, 0x80),     _XT("fuchsia") ,    _XT("Rosa") },
  { _ColorDef(63,0,100,0),		RGBDEF(0x00, 0xFF, 0x00),     _XT("lime")    ,    _XT("Hellgrün") },
  { _ColorDef(6,0,97,0),			RGBDEF(0xFF, 0xFF, 0x00),     _XT("yellow")  ,    _XT("Gelb") },
  { _ColorDef(88,77,0,0),			RGBDEF(0x00, 0x00, 0xFF),     _XT("blue")    ,    _XT("Hellblau") },
  { _ColorDef(52,0,13,0),			RGBDEF(0x00, 0xFF, 0xFF),     _XT("aqua")    ,    _XT("Türkis") },

  { _ColorDef(71,65,64,68),		RGBDEF(0x28, 0x28, 0x28),     _XT("gray-80"),     _XT("Grau-80") },
  { _ColorDef(60,51,51,20),		RGBDEF(0x66, 0x66, 0x66),     _XT("gray-50"),     _XT("Grau-50") },
  { _ColorDef(52,43,43,8),		RGBDEF(0x7F, 0x7F, 0x7F),     _XT("gray-40"),     _XT("Grau-40") },
  { _ColorDef(25,20,20,0),		RGBDEF(0xC0, 0xC0, 0xC0),     _XT("gray-25"),     _XT("Grau-25") },
  { _ColorDef(4,3,3,0),				RGBDEF(0xF0, 0xF0, 0xF0),     _XT("gray-10"),     _XT("Grau-10") }

};

#define TAG_PAGE_TOP_MARGIN            1134  // <-- Margin in twips (2 cm)
#define TAG_PAGE_BOTTOM_MARGIN         1134  // <--
#define TAG_PAGE_LEFT_MARGIN           1134  // <--
#define TAG_PAGE_RIGHT_MARGIN          1134  // <--

// PDF Version
#define TAG_PDF_VERSION		_XT("Version")	// 1.5, 1.6 = Acrobat 7

// Farbe
#define TAG_RGB		_XT("RGB(")		// Im Moment nur RGB
#define TAG_CMYK	_XT("CMYK(")		// 

// Schrift Defaults
#define TAG_DEFAULT_CONVERT_FONT        _XT("Arial")
#define TAG_DEFAULT_CONVERT_FONT_SIZE   11
#define TAG_DEFAULT_BLACK               RGBDEF(0x00, 0x00, 0x00) // black
#define TAG_DEFAULT_WHITE               RGBDEF(0xFF, 0xFF, 0xFF) // white
#define TAG_DEFAULT_GRAY								RGBDEF(0xC0, 0xC0, 0xC0) // gray-25

// Format
#define TAG_TEXT_NORMAL         0   // normal
#define TAG_TEXT_BOLD           1   // Fett an
#define TAG_TEXT_BOLD_END       2   // Fett aus
#define TAG_TEXT_ITALIC         3   // Kursiv an
#define TAG_TEXT_ITALIC_END     4   // Kursiv aus
#define TAG_TEXT_UNDERLINE      5   // Unterstrichen an
#define TAG_TEXT_UNDERLINE_END  6   // Unterstrichen aus
#define TAG_TEXT_UL_DOUBLE      7   // Doppelt an
#define TAG_TEXT_UL_DOUBLE_END  8   // Doppelt aus
#define TAG_TEXT_STRIKE					9   // Durchgestrichen an
#define TAG_TEXT_STRIKE_END		    10 // Durchgestrichen aus
#define TAG_TEXT_UNDERLINE_COLOR  11 // Underline-Color

// Ausrichtung
#define TAG_TEXT_TEXTZ  1 // <TextZ> 
#define TAG_TEXT_TEXTL  2 // <TextL>
#define TAG_TEXT_TEXTR  3 // <TextR>
#define TAG_TEXT_TEXTB  4 // <TextB>
#define TAG_TEXT_TEXTBL 5 // <TextBL>
#define TAG_TEXT_TEXTBR 6 // <TextBR>
#define TAG_TEXT_TEXTBZ 7 // <TextBZ>

// Fitting
#define TAG_IMAGE_FITCONTPROP				1	// kFitContentPropCmdBoss:fits content to just barely fill the frame (kFalse) 
#define TAG_IMAGE_FITFILLCONTPROP		2	// kFitContentPropCmdBoss:stretches larger than frame to fill all white space (kTrue)
#define TAG_IMAGE_FITFRAMETOCONTENT	3	// kFitFrameToContentCmdBoss
#define TAG_IMAGE_FITCONTENTTOFRAME	4	// kFitContentToFrameCmdBoss
#define TAG_IMAGE_FITCENTERCONTENT	5	// kCenterContentInFrameCmdBoss
#define TAG_IMAGE_FITFRAMERIGHTTOCONTENT	6	// kFitFrameRightToContentCmdBoss

#define TAG_INLINE_TYPETEXT	  		1	// 
#define TAG_INLINE_TYPEGRAFIC	   	2	// kEPSItem, kWMFItem, kImageItem

#define TAG_TEXT_TEXTZ  1 // <TextZ> 
#define TAG_TEXT_TEXTL  2 // <TextL>
#define TAG_TEXT_TEXTR  3 // <TextR>
#define TAG_TEXT_TEXTB  4 // <TextB>
#define TAG_TEXT_TEXTBL 5 // <TextBL>
#define TAG_TEXT_TEXTBR 6 // <TextBR>
#define TAG_TEXT_TEXTBZ 7 // <TextBZ>

// Section Cursor Position 
#define TAG_CURSOR_HOME   1 // Begin der Section/Positionsrahmen/Header, Footer
#define TAG_CURSOR_END    2 // Ende der Section/Positionsrahmen/Header, Footer

// Kopf/Fuss Body
#define TAG_TEXT_HEADPOS    1 // Header
#define TAG_TEXT_FOOTERPOS  2 // Footer
#define TAG_TEXT_BODYPOS    3 // Maintext

// Layer Definitionen
// TAG_LAYER_INTENT	  "intent="	  // PDF7: View, Reference
#define TAG_LAYER_INTENT_FLAG_VIEW 0
#define TAG_LAYER_INTENT_FLAG_REFERENCE 1
// Sonderfall Indesign: Wir bilden Guide locked für 
#define TAG_LAYER_INTENT_GUIDE_OFF 0
#define TAG_LAYER_INTENT_GUIDE_ON	1

// TAG_LAYER_DEFSTATE	/ TAG_LAYER_LOCKED
#define TAG_LAYER_FLAG_ON   1
#define TAG_LAYER_FLAG_OFF  0
// TAG_LAYER_VISIBLE, TAG_LAYER_PRINT, TAG_LAYER_EXPORT
#define TAG_LAYER_INITIALSTATE_FLAG_VISIBLE_WHEN_ON	0 // Visible when on
#define TAG_LAYER_INITIALSTATE_FLAG_NEVER_VISIBLE	  1 // Never Visible 
#define TAG_LAYER_INITIALSTATE_FLAG_ALWAYS_VISIBLE	2 // Always Visible

// Tabelle und Anfügeoperation
#define TAG_TABLE_ROW_APPEND_END        1 // am Ende hinzufügen
#define TAG_TABLE_ROW_APPEND_CURRENT    2 // or der aktuellen Zeile

#define TAG_TABLE_VERTALIGN_CENTER  1 // Center alignment
#define TAG_TABLE_VERTALIGN_BOTTOM  2 // Bottom alignment
#define TAG_TABLE_VERTALIGN_TOP     3 // Align base line of the text

#define TAG_TABLE_ROTATE_HORZ       0 // normale Darstellung in Zelle
#define TAG_TABLE_ROTATE_TOPTOBOT   1 // von oben nach unten
#define TAG_TABLE_ROTATE_BOTTOTOP   2 // von unten nach oben

// Tabulatoren
#define TAG_TABULATOR_LEFT        1 // links ausgerichteter Tabulator
#define TAG_TABULATOR_RIGHT       2 // rechts ausgerichteter Tabulator
#define TAG_TABULATOR_CENTER      3 // zentrierter Tabulator
#define TAG_TABULATOR_DECIMAL     4 // dezimaler Tabulator
#define TAG_TABULATOR_DEFAULT     5 // Standardtabulator
#define TAG_TABULATOR_CLEARALL    6 // alle Tabs löschen
#define TAG_TABULATOR_ALIGN_CHAR  7 // dezimaler Tab ausgerichtet an Zeichen ','
#define TAG_TABULATOR_LEADER      8 // Tabulator führend mit folgenden Zeichen auffüllen
#define TAG_TABULATOR_DECIMAL_COMMA       1 // dezimaler Tabausrichtungspunkt ist Komma
#define TAG_TABULATOR_DECIMAL_POINT       2 // dezimaler Tabausrichtungspunkt ist Punkt

// Paraframe Positionen
#define TAG_TEXTFRAME_PAGE      0  // object y position relative to the top of the page
#define TAG_TEXTFRAME_MARG      1  // object y position relative to the top margin
#define TAG_TEXTFRAME_PARA      2  // object y position relative to the anchor paragraph

#define TAG_TEXTFRAME_VERTALIGN_FLAG_CENTER  1 // Center alignment
#define TAG_TEXTFRAME_VERTALIGN_FLAG_BOTTOM  2 // Bottom alignment
#define TAG_TEXTFRAME_VERTALIGN_FLAG_TOP     3 // Align base line of the contents
#define TAG_TEXTFRAME_VERTALIGN_FLAG_JUSTIFY 4 // Align vertical justify
 
#define TAG_TEXTFRAME_HORALIGN_FLAG_LEFT     1 // Left alignment
#define TAG_TEXTFRAME_HORALIGN_FLAG_MIDDLE  2 // Middle alignment
#define TAG_TEXTFRAME_HORALIGN_FLAG_RIGHT   3 // Right base line of the contents

// PDF unterstützt 1 und 2, RTF keines
#define TAG_TEXTFRAME_WRAP_C_NOTEXT      1 // idNone: no text wrap (default)
#define TAG_TEXTFRAME_WRAP_C_AROUNDBOX   2 // idBoundingBoxTextWrap: wrap around bounding box
#define TAG_TEXTFRAME_WRAP_C_CONTOUR     3 // idContour: wrap around object contour
#define TAG_TEXTFRAME_WRAP_C_JUMPOBJECT  4 // idJumpObjectTextWrap: jump object
#define TAG_TEXTFRAME_WRAP_C_JUMPNEXT    5 // idNextColumnTextWrap: jump next colum

#define TAG_SECTION_LANDSCAPE   0 // Querformat
#define TAG_SECTION_PORTRAIT    1 // Hochformat

#define TAG_TEMPLATE_APPEND_MODE	0					// hängt die Seite ans Ende an
#define TAG_TEMPLATE_REPEAT_MODE	-1				// wiederholt die Seite bei jeder neuen Seite innerhalb der aktuellen Section
#define TAG_TEMPLATE_OVERLAY_MODE	-2				// lagert die Seite über die angegebene Seite

// Name der Einstellungsdatei
#define TAG_IMAGE_IC_INI        xtstring(_XT("ic32.ini"))

#define TAG_CLOSETAG     ">"									// Endetag
#define TAG_BEGINTAG     "<"									// Beginntag
#define TAG_TRUE				"True"								// True
#define TAG_FALSE				"False"								// False

#define TAG_BOOKMARK				"<bookmark"				// Lesezeichen
#define TAG_BOOKMARK_TEXT			 "name="				// Text der Lesezeichen
#define TAG_BOOKMARK_LEVEL		 "level="				// Gliederungsstufe der Lesezeichen, 1, 1.1, 1.2
#define PDF_BOOKMARK_SYMBOL		"anchor="				// Symbolischer Name auf den durch Link referenziert wird
#define TAG_BOOKMARK_ZOOM			 "zoom="				// Zoom bei Lesezeichen
#define TAG_BOOKMARK_IGNOREPOS "ignore="			// Position ignorieren und auf 0,0 Seite zeigen 
#define TAG_BOOKMARK_FORMAT		 "format="			// bold, italic, beides
#define TAG_BOOKMARK_FORMAT_BOLD		"bold"		// siehe format
#define TAG_BOOKMARK_FORMAT_ITALIC	"italic"	// 
#define TAG_BOOKMARK_FORMAT_BOTH		"both"		// 
#define TAG_BOOKMARK_COLOR		 "color="	      // Farbe des Lesezeichens

#define PDF_BOOKMARK_NORMAL         0
#define PDF_BOOKMARK_ITALIC         1//1.bit
#define PDF_BOOKMARK_BOLD           2//2.bit
#define PDF_BOOKMARK_ITALIC_BOLD    3//1. and 2. bit

// Anchor
#define TAG_ANCHOR_BEGIN		"<anchor"			// Anker für Referenz oder Lesezeichen
#define TAG_ANCHOR_NAME			"name="				// Der Name des Ankers
#define TAG_ANCHOR_END		  "</anchor"		// Anker Ende

// Metrische Angaben
#define TAG_METRICS		"<metrics"				// erlaubt die Auswahl eines bestimmten Maßsystems
#define TAG_METRICS_NAME	"name="				// Name, vordefiniert
#define TAG_METRICS_INCH	"Inch"				// Namen
#define TAG_METRICS_MM		"Millimeter"	// Namen
#define TAG_METRICS_PICA	"Pica"				// Namen

// LINK
#define TAG_PDF_LINK_BEGIN		"<reference"				// Referenz
#define TAG_PDF_LINK_END		  "</reference"				// Referenz Ende
#define TAG_PDF_LINK_NAME			 "name="				// Referenz auf  z.B. "www.x-publisher.com" oder auf Anchor
#define TAG_PDF_LINK_SYMBOL		 "anchor="			// Anker-Name auf den im Dokument referenziert wird
#define TAG_PDF_LINK_PAGE			 "pagenbr="			// Seite auf die referenziert wird
#define TAG_PDF_LINK_ZOOM			 "zoom="				// Zoom bei Link

// ROOT-PATHs
#define TAG_SEARCHPATH		"<searchpath"				// globale Pfade
#define TAG_SEARCHPATH_PATH		"names=" // Pfad zur Suche 

// Select Style
#define TAG_STYLE						"<style"		// Paragraph Stylename
#define TAG_STYLE_END				"</style>"	// Paragraph Style Definition Ende
#define TAG_STYLE_TYPE  		"type="			// Type des Styles "Paragraph", "Object", "Table", "TextFrame"
#define TAG_STYLE_SELECT		"select="		// "selektiert ein angelegtes Style
#define TAG_STYLE_NAME			"name="			// "Normal" ist Defaultstyle des Dokumentes
#define TAG_STYLE_PERMANENT	"permanent="// immer anlegen, nur wenn benutzt
#define TAG_STYLE_NEXTSTYLE	"nextstyle="// Name des Folgestyle für nächsten Absatz
#define TAG_STYLE_BASEDON 	"basedon="  // Style basiert auf
#define TAG_STYLE_TYPE_CHARACTER  "Character"
#define TAG_STYLE_TYPE_PARAGRAPH  "Paragraph"

#define TAG_CLASS				"<class"				// class
#define TAG_CLASS_NAME		"name="				// name z.B. kTextAttrSpaceAfterBoss
#define TAG_CLASS_TYPE		"type="				// type: bool, real, int32, int16
#define TAG_CLASS_VALUE	  "value="			// Wert 

#define TAG_FONT				"<font"					// Font
#define TAG_FONT_SIZE			"size="				// Schrifthöhe
#define TAG_FONT_NAME			"name="				// Schriftname
#define TAG_FONT_COLOR	  "color="			// Textfarbe
#define TAG_FONT_BGCOLOR	"bgcolor="		// Hintergrundfarbe des Textes
#define TAG_FONT_CODEPAGE	"codepage="		// Codepage des Textes
#define TAG_FONT_END		"</font>"			  // auf Standard Schriftname zurück

#define TAG_FONT_BOLD					"<bold>"			 
#define TAG_FONT_BOLD_END			"</bold>"			 
#define TAG_FONT_ITALIC				"<italic>"			 
#define TAG_FONT_ITALIC_END		"</italic>"			 

#define TAG_FONT_UNDERLINE			"<underline>"	// einfach Unterstrichen		 
#define TAG_FONT_UNDERLINE_END	"</underline>"			 
#define TAG_FONT_UL_DOUBLE			"<double>"		// doppelt Unterstrichen			 
#define TAG_FONT_UL_DOUBLE_END	"</double>"			 
#define TAG_FONT_UL_STRIKE			"<strike>"		// Durchgestrichen	 
#define TAG_FONT_UL_STRIKE_END	"</strike>"			 

#define TAG_PAGEBREAK       "<pagebreak>"             // Seitenumbruch MUSS
#define TAG_FILLPAGEBREAK   "<fillpagebreak>"   // Seitenumbruch bei PDF-Formularen bis letzte Seite auffüllen
#define TAG_GOWRITEPOSITION "<gowriteposition>" // Seitenumbruch auffüllen und zu Schreibposition oder auf neue Seite springen
#define TAG_NOPAGEBREAK    "<nopagebreak>"		  // keinen Umbruch durchführen 
#define TAG_NOPAGEBREAK_END "</nopagebreak>"	  // Ende von keinen Umbruch durchführen

#define TAG_SECTION					"<section "				// Abschnittwechsel konfigurieren
#define TAG_SECTION_NUMCOL		"numcols="			// Spalten
#define TAG_SECTION_WIDTH			"colspace="			// Breite zwischen den Spalten
#define TAG_SECTION_PAGENBR		"pagenbr="			// Seitenzahl für die erste Seite der Sektion

#define TAG_SECTIONBREAK    "<sectionbreak>" // Abschnittwechsel einfügen
#define TAG_COLUMNBREAK     "<columnbreak>"  // Spaltenwechsel
#define TAG_PARAGRAPHBREAK  "<p>"             // Paragraph
#define TAG_LINEBREAK       "<l>"		          // Zeilenumbruch, Paragraphformat bleibt erhalten
#define TAG_BEGINBODY       "<pagebody "      // bei angegebener Stelle auf Seite beginnen zu schreiben
#define TAG_BEGINBODY_POS	  "pos="            // Position ab der das Schreiben auf Seite beginnen soll

// Template
#define TAG_TEMPLATE	    "<template "    // pdf Datei als Unterlage laden
#define TAG_TEMPLATE_NAME	      "name="       // die PDF Datei	
#define TAG_TEMPLATE_LAYER	    "layer="      // Layer des Templates
#define TAG_TEMPLATE_OPEN_PWD	  "openpwd="    // Kennwort um PDF-Datei zu öffnen
#define TAG_TEMPLATE_USER_PWD	  "userpwd="    // Kennwort um PDF-Datei zu bearbeiten
#define TAG_TEMPLATE_MODE	      "mode="       // gibt an wie die Datei geladen werden soll
#define TAG_TEMPLATE_MODE_OVERLAY	"overlay"   // lagert die Seite über die angegebene Seite
#define TAG_TEMPLATE_MODE_APPEND	"append"    // hängt die Seite ans Ende an
#define TAG_TEMPLATE_MODE_REPEAT	"repeat"    // wiederholt die Seite bei jeder neuen Seite innerhalb der aktuellen Section

// Paragraphen Steuerung

#define TAG_PARAGRAPH       "<par "					 // Absatz
#define TAG_PARA_LEFT       "left="          // linker Einzug
#define TAG_PARA_RIGHT      "right="         // rechter Einzug
#define TAG_PARA_IDENT      "indent="        // hängender Einzug

#define TAG_PARA_SPACE_AFTER    "spaceafter="		// Space before the first line of the paragraph in twips
#define TAG_PARA_SPACE_BEFORE   "spacebefore="	// Space after the last line of the paragraph in twips
#define TAG_PARA_SPACE_BETWEEN  "spacebetween="	// Minimum space between the lines of the paragraph
#define TAG_PARA_SPACE_LINE	    "linespace="	  // You can use this argument to specify the extra
																							  // line space in percentage of the current line height
#define TAG_PARA_ALIGN      "align="         // Ausrichtung des Paragraphs
 #define TAG_PARAGRAPH_LEFT		"left"				 // links(default)
 #define TAG_PARAGRAPH_RIGHT  "right"				 // rechts
 #define TAG_PARAGRAPH_CENTER "center"			 // zentriert
 #define TAG_PARAGRAPH_BLOCK	"justified"		 // Blocksatz
#define TAG_PARA_PAGEBREAK    "pagebreak="   // Seitenumruch vor Absatz einfügen, wenn nicht auf neuer Seite
#define TAG_PARA_WIDOW				"widow="			 // widow/orphon
#define TAG_PARA_KEEP					"keep="				 // Absatz über Seitenumbruch zusammenhalten
#define TAG_PARA_KEEPNEXT			 "keepnext="	 // haltet Absatz mit Folgeabsatz zusammen

// Paragraph
#define TAG_PAGES           "<pagenbrs>"   // gesamte Seitenzahl
#define TAG_PAGENUMBER      "<pagenbr>"    // aktuelle Seite

#define TAG_HEADER          "<header>"      // Kopf
#define TAG_HEADER_END      "</header>"     // Ende Kopf
#define TAG_HEADER_MARGIN   "<header"				// Kopfbeginn vom oberen Blattrand
#define TAG_HEADER_POS      "top="					// Angabe des Abstandes

#define TAG_FOOTER          "<footer>"      // Fusszeile
#define TAG_FOOTER_END      "</footer>"     // Ende Fusszeile
#define TAG_FOOTER_MARGIN   "<footer"				// Fuss beginn vom unteren Blattrand
#define TAG_FOOTER_POS      "bottom="					// Angabe des Abstandes

// Checkbox
#define TAG_CHECKBOX          "<checkbox "  // Checkbox
#define TAG_CHECKBOX_VALUE		"value="			// Value kann "True" oder "False" sein		
#define CHECKBOX_YES					"chkboxYes.jpg"
#define CHECKBOX_NO						"chkboxNo.jpg"

// Import: snippets, pdf, xml
#define TAG_IMPORT							"<import"				// verschiedenste Typen

// Inlines
#define TAG_INLINE							"<inline"				// Grafik
#define TAG_INLINE_FITTING			"fitting="			// 
#define TAG_INLINE_ID  					"id="						// ElementID
#define TAG_INLINE_NAME					"name="					// sym. Name
#define TAG_INLINE_FILENAME			"file="					// Grafikdatei
#define TAG_INLINE_WIDTH				"width="				// Breite des Bildes
#define TAG_INLINE_HEIGHT			  "height="				// Höhe des Bildes
#define TAG_INLINE_XSCALE			  "xscale="				// X-Scale des Bildes
#define TAG_INLINE_YSCALE			  "yscale="				// Y-Scale des Bildes
#define TAG_INLINE_NATIVE				"native="				// nativer Typ, für Erweiterungen z.B kMoviePageItemBoss, kSoundPageItemBoss
#define TAG_INLINE_TYPE					"type="				  // Grafic, Text, Movie... siehe IDataLinkReference
#define TAG_INLINE_TYPE_GRAFIC	"Grafic"				// kEPSItem, kWMFItem, kImageItem
#define TAG_INLINE_TYPE_TEXT  	"Text"					// kTextStoryBoss

// Grafik
#define TAG_IMAGE_SYMBOL			"<sym"					// Symbolische Grafik über ic32.ini
#define TAG_IMAGE							"<image"				// Grafik
#define TAG_IMAGE_ID  				"id="						// ElementID
#define TAG_IMAGE_SYMNAME			"name="					// sym. Name
#define TAG_IMAGE_FILENAME		"file="					// Grafikdatei
#define TAG_IMAGE_ZOOM				"zoom="					// Grafik Skalierungsfaktor in %
#define TAG_IMAGE_WIDTH				"width="				// Breite des Bildes
#define TAG_IMAGE_HEIGHT			"height="				// Höhe des Bildes
#define TAG_IMAGE_SHRINK			"shrink="				// Grafikdatei auf angegegeben Wert verkleinern
#define TAG_IMAGE_THUMBNAIL		"thumbnail"			// Grafikdatei als Thumbnail, Wert bei width ist size der Thumbnail
#define TAG_IMAGE_DPI					"dpi="					// dpi bei shrink
#define TAG_IMAGE_FITTING			"fitting="			// 
// Indesign
#define TAG_IMAGE_FIT_CONTPROP				"FitContentProp"		// kFitContentPropCmdBoss:fits content to just barely fill the frame (kFalse) 
#define TAG_IMAGE_FIT_FILLCONTPROP		"FillContentProp"		// kFitContentPropCmdBoss:stretches larger than frame to fill all white space (kTrue)
#define TAG_IMAGE_FIT_FRAMETOCONTENT	"FitFrameToContent"	// kFitFrameToContentCmdBoss
#define TAG_IMAGE_FIT_FRAMERIGHTTOCONTENT	"FitFrameRightToContent"	// kFitFrameToContentCmdBoss Ausrichtung rechts
#define TAG_IMAGE_FIT_CONTENTTOFRAME	"FitContentToFrame"	// kFitContentToFrameCmdBoss
#define TAG_IMAGE_FIT_CENTERCONTENT		"CenterContent"		// kCenterContentInFrameCmdBoss

// Dokumentfunktion
#define TAG_ENDPOS            "<bodyend>"		  // setzt die Einfügemarke ans Dokumentende
#define TAG_HOMEPOS           "<bodybegin>"   // setzt die Einfügemarke ans Dokumentende

// Tabellenfunktionen
#define TAG_ENDTABLE          "</table>"        // an das Tabellenende springen
#define TAG_TABLE             "<table"          // TabellenFunktionen

// die folgenden sind kombiniert mit "<Tabelle ..."
#define TAG_TABLE_ID						"id="						// ElementID
#define TAG_TABLE_NAME					"name="				  // Name der Tabelle
#define TAG_TABLE_DEFINITION		"create="				// Definition der Tabelle
#define TAG_TABLE_CLEAR					"clear="				// spezielle Tabellenverarbeitung
#define TAG_TABLE_CURRENT       "rowcol="				// macht Zelle aktiv
#define TAG_TABLE_BKGR_COLOR    "cellcolor="	  // Hintergrundfarbe einer Zelle der Tabelle
#define TAG_TABLE_BORDER_COLOR  "bordercolor="  // Borderfarbe einer Zelle der Tabelle
#define TAG_TABLE_BORDER_WIDTH  "borderwidth="  // Rahmenstärke
#define TAG_TABLE_CELL_WIDTH		"cellwidth="		// Zellbreite
#define TAG_TABLE_ROW_HEIGHT		"rowheight="		// minimale Zeilenhöhe festlegen
#define TAG_TABLE_ROW_HEIGHT_EXACTLY	"exactly"		// exakte Einstellung
#define TAG_TABLE_ROW_HEIGHT_ATLEAST	"atleast"		// minimale Einstellung
#define TAG_TABLE_ROWPOSITION	  "rowposition="	// Beginnposition der Tabellenzeile vom linken Blattrand
#define TAG_TABLE_BG_IMAGE  		"bgimage="			// fügt eine Grafik in Zellhintergrund ein
#define TAG_TABLE_CELLMARGIN	  "cellmargin="		// Abstand des Inhaltes einer Zelle von den Rändern
#define TAG_TABLE_CELLALIGN			"valign="				// Ausrichtung des Textes in der Zelle
 #define TAG_TABLE_ALIGN_TOP		"top"						// Ausrichtung "Oben"
 #define TAG_TABLE_ALIGN_CENTER	"middle"				// Ausrichtung "Vertikal"
 #define TAG_TABLE_ALIGN_BOTTOM	"bottom"				// Ausrichtung "Unten"
 #define TAG_TABLE_DELETECOLUMN	"delete"				// löscht die aktuelle Zelle
#define TAG_TABLE_INSERTCOLUMN	"addcell"				// fügt eine neue Zelle an aktueller Position ein
#define TAG_TABLE_SPLITCOLUMN	  "splitcell"		  // teilt die gerade aktive Zelle
#define TAG_TABLE_MERGECOLUMNS  "mergecells="		// merged Zellen zusammen
#define TAG_TABLE_HEADERROW			"header="			// definiert die Zeile als Tabellenkopf, mehrere Zeilen sind möglich
#define TAG_TABLE_HEADERROW_TRUE	"header"			// definiert die Zeile als Tabellenkopf, mehrere Zeilen sind möglich
#define TAG_TABLE_FOOTERROW			"footer"				// definiert die Zeile als Tabellenfuss, mehrere Zeilen sind möglich
#define TAG_TABLE_SPACE 				"space="				// erlaubt die Tablespace Definition: "=before,after"
#define TAG_TABLE_KEEPROW				"keeprow="			// hält die Zeile mit weiteren Zeilen zusammen, TRUE/FALSE
#define TAG_TABLE_KEEPROW_TRUE	"keeprow"				// "" direktes Setzen
#define TAG_TABLE_SKIP					"skip"					// intern gespeicherte aktuelle Zeile oder Spalte erhöhen
#define TAG_TABLE_ADDLINE				"addrow"				// Zeile hinzufügen
#define TAG_TABLE_ADDLINE_MORE	"addrow="				// Zeilen hinzufügen
#define TAG_TABLE_CELLROTATE	  "rotate="				// Zelle rotieren
#define TAG_TABLE_CELLROTATE_HORZ	    "horizontal"	// horizontaler Text
#define TAG_TABLE_CELLROTATE_TOPBOT	  "topbottom"		// Oben nach Unten
#define TAG_TABLE_CELLROTATE_BOTTOP	  "bottomtop"		// Unten nach Oben

#define TAG_RTF_DOC_SETTINGS      "<information"  // Dokumenteigenschaften
// RTF:Dokumentinfo  nur bei RTFAddOn
#define TAG_RTF_DOC_INFO_TITLE      "title="      // Document title
#define TAG_RTF_DOC_INFO_SUBJECT    "subject="    // Subject
#define TAG_RTF_DOC_INFO_AUTHOR     "author="     // Author
#define TAG_RTF_DOC_INFO_MANAGER    "manager="    // Manager
#define TAG_RTF_DOC_INFO_COMPANY    "company="    // Company
#define TAG_RTF_DOC_INFO_OPERATOR   "operator="   // Operator
#define TAG_RTF_DOC_INFO_CATEGORY   "category="   // Category
#define TAG_RTF_DOC_INFO_KEYWORDS   "keywords="   // Keywords
#define TAG_RTF_DOC_INFO_COMMENT    "comment="    // Comment
#define TAG_RTF_DOC_INFO_DOCCOMM    "addComment=" // Additional comment

// PDF:Dokumentinfo nur bei PDFAddon
#define TAG_PDF_DOC_INFO_TITLE      "title="      // Document title
#define TAG_PDF_DOC_INFO_SUBJECT    "subject="    // Subject
#define TAG_PDF_DOC_INFO_KEYWORDS   "keywords="   // Keywords
#define TAG_PDF_DOC_INFO_AUTHOR     "author="     // Author
#define TAG_PDF_DOC_INFO_CREATOR    "creator="    // Creator
#define TAG_PDF_DOC_INFO_PRODUCER   "producer="   // Producer
#define TAG_PDF_DOC_INFO_CREATEDATE "created="    // CreateDate
#define TAG_PDF_DOC_INFO_MODDATE    "moddate="    // mod. Date

#define TAG_PDF_PASSWORD				"<password"				// Kennwort der PDF-Datei
#define TAG_PDF_PASSWORD_MASTER	    "master="			// Master-Kennwort
#define TAG_PDF_PASSWORD_USER				"user="				// User-Kennwort
#define TAG_PDF_PASSWORD_PERMISSION "permission=" // Permissions

#define TAG_PDF_PASSWORD_PERMISSION_ANNOT			"annotation"	// Annotation
#define TAG_PDF_PASSWORD_PERMISSION_ASSEM			"assemble"		// Assemble
#define TAG_PDF_PASSWORD_PERMISSION_COPY			"copy"				// Copy
#define TAG_PDF_PASSWORD_PERMISSION_EXTRACT		"extract"			// Extract
#define TAG_PDF_PASSWORD_PERMISSION_FORM			"form"				// Form
#define TAG_PDF_PASSWORD_PERMISSION_MODIFY		"modify"			// Modify
#define TAG_PDF_PASSWORD_PERMISSION_PRINTLOW	"printlow"		// PrintLow
#define TAG_PDF_PASSWORD_PERMISSION_PRINTHIGH	"printhigh"		// PrintHigh

// Blattränder definieren
#define TAG_PAGE_MARGINS   "<margins "		 // Blattrand definieren
#define TAG_MARGIN_LEFT     "left="        // oberer Blattrand
#define TAG_MARGIN_RIGHT    "right="       // rechter
#define TAG_MARGIN_TOP      "top="         // oberer
#define TAG_MARGIN_BUTTON   "bottom="      // unterer

#define TAG_MODULE					 "<module "		// Modul
#define TAG_MODULE_ID        "id="				// ElementID
#define TAG_MODULE_FUNCTION	 "function="	// Name der Funktion
#define TAG_MODULE_PARAMETER "parameter="	// Parameter der Funktion
#define TAG_MODULE_LAYER		 "layer="			// Layer für anzuwendede Funktion

// Asset
#define TAG_ASSET					 "<asset "		// Asset
#define TAG_ASSET_ID         "id="			// ElementID
#define TAG_ASSET_NAME	     "name="		// Name des Layers
#define TAG_ASSET_FILENAME	 "file="		// Dateiname des Asset, könnte auch Snippet sein

#define TAG_PAGE					 "<pages"				// Page 
#define TAG_PAGE_JOIN			 "join="				// Nachfolgeer ist "Vorgänger, Nachfolger"
#define TAG_PAGE_ASSET		 "asset="				// das Asset aus Library
#define TAG_PAGE_LIB			 "lib="				  // die Library
#define TAG_PAGE_REPEAT		 "repeat="	    // Wiederholung bis Überlauf ok
#define TAG_PAGE_OVERFLOW	 "overset="	    // Element, welches auf Überlauf kontrolliert wird

#define TAG_SPREAD         "<spreads"			//  
#define TAG_PAGE_CREATE    "create="			//  
#define TAG_PAGE_MOVE      "move="				//  
#define TAG_PAGE_DELETE    "delete"				//  

#define TAG_DOCUMENT          "<document"     // Dokumentdefinition 
#define TAG_DOCUMENT_ID         "id="					//  ElementID
#define TAG_DOCUMENT_ORIENT     "orientation="//  Ausrichtung: Hochformat, Querformat
#define TAG_DOCUMENT_FORMAT     "format="     //  DINA4, DINA5
#define TAG_DOCUMENT_LANDSCAPE  "landscape"   //  Querformat
#define TAG_DOCUMENT_PORTRAIT   "portrait"    //  Hochformat
#define TAG_DOCUMENT_WIDTH      "width="      //  Blattbreite
#define TAG_DOCUMENT_HEIGHT     "height="     //  Blatthöhe


// Layer: nicht Indesign
#define TAG_LAYER_BEGIN   "<layer "		  // Beginn des Layers
#define TAG_LAYER_END     "</layer>"	  // Ende des Layers
#define TAG_LAYER_NAME	    "name="			// Name des Layers
#define TAG_LAYER_ID  	    "id="				// ElementID
#define TAG_LAYER_INTENT	  "intent="	  // PDF7: View, Reference
#define TAG_LAYER_INTENT_VIEW       "View"      // View
#define TAG_LAYER_INTENT_REFERENCE  "Reference" // Reference

// LayerCS:  Indesign
#define TAG_LAYER_CS_SHOW	  "show="				 // 0/1
#define TAG_LAYER_CS_GUIDE	"guide="			 // 0/1
#define TAG_LAYER_CS_LOCK	  "lock="				 // 0/1
#define TAG_LAYER_CS_ACTIVATE	"active="	   // Eingabefocus: nur ein Layer kann zugleich aktiv sein
#define TAG_LAYER_CS_COPYFROM	"copyfrom="	 // Wenn nicht vorhanden erstellen aus dieser Angabe
#define TAG_LAYER_CS_DELETE	"delete="			 // Layer vorher löschen
#define TAG_LAYER_CS_MOVETO	"move="				 // Layer in Liste an angegebene Position verschieben, -1=Ende, 0=Anfang

#define TAG_LAYER_DEFSTATE	"defstate="	// Default state: visible, invisible
#define TAG_LAYER_LOCKED	  "locked="	  // PDF7: ON/OFF
#define TAG_LAYER_ON        "on"        //
#define TAG_LAYER_OFF       "off"       // Default ist OFF

#define TAG_LAYER_VISIBLE	  "visible="	// Visible when on, Never Visible, Always Visible
#define TAG_LAYER_PRINT	    "print="	  // Visible when on, Never Visible, Always Visible
#define TAG_LAYER_EXPORT	  "export="	  // Visible when on, Never Visible, Always Visible
#define TAG_LAYER_INITIALSTATE_VISIBLE_WHEN_ON	"visiblewhenon"  // Visible when on
#define TAG_LAYER_INITIALSTATE_NEVER_VISIBLE	  "nevervisible"   // Never Visible 
#define TAG_LAYER_INITIALSTATE_ALWAYS_VISIBLE	  "alwaysvisible" // Always Visible

#define TAG_LAYER_GROUP	    "group="	  // Gruppe für Radiobutton verhalten
#define TAG_LAYER_PARENT	  "parent="	  // Gruppe für SubLayer, nur einstufig
#define TAG_LAYER_COLOR	    "color="	  // Indesign: Layerfarbe

// rectangle
#define TAG_RECTANGLE					"<rectangle"				// Rechteck
#define TAG_RECTANGLE_ID  				"id="						// ElementID
#define TAG_RECTANGLE_SYMNAME			"name="					// sym. Name
#define TAG_RECTANGLE_LEFT				"left="					// X, linke Position
#define TAG_RECTANGLE_TOP					"top="					// Y, obere Position 
#define TAG_RECTANGLE_WIDTH				"width="				// Breite in mm
#define TAG_RECTANGLE_HEIGHT			"height="				// Höhe in mm
#define TAG_RECTANGLE_BORDERWIDTH	"borderwidth="  // Linienstärke
#define TAG_RECTANGLE_BORDERCOLOR	"bordercolor="  // Linienfarbe
#define TAG_RECTANGLE_FILLCOLOR		"bgcolor="			// Füllfarbe
#define TAG_RECTANGLE_TINT				"tint="					// Deckkraft
#define TAG_RECTANGLE_ARRANGE			"arrange="			// Anordnung: Front, Forward, Backward, Back

// Textrahmen
// ALT: wird nicht unterstützt
#define TAG_POSFRAME_BEGIN     "<posframe "		// Positionsrahmen
#define TAG_TEXTFRAME_BREAK     "<breakframe>"	// ALT: Wechsel in nächsten Positionsrahmen der Kette 
#define TAG_POSFRAME_END       "</posframe>"	// ALT: Positionsrahmen Ende

// Textrahmen
#define TAG_TEXTFRAME_BEGIN     "<textframe "		// Positionsrahmen
#define TAG_TEXTFRAME_END       "</textframe>"	// Endpositionsrahmen
#define TAG_TEXTFRAME_NAME			 "name="				// Name des Positionsrahmens
#define TAG_TEXTFRAME_ID  			 "id="					// ElementID
#define TAG_TEXTFRAME_JOIN			 "join="				// Nachfolgeer ist 
#define TAG_TEXTFRAME_LIBRARY		 "library="			// set von Befehlen für mehrfache gleichnamige XML-Tags
#define TAG_TEXTFRAME_LEFT			 "left="				// X, linke Position
#define TAG_TEXTFRAME_TOP			 "top="					// Y, obere Position 
#define TAG_TEXTFRAME_WIDTH		 "width="				// Breite in mm
#define TAG_TEXTFRAME_HEIGHT		 "height="					// Höhe in mm
#define TAG_TEXTFRAME_BORDERWIDTH	"borderwidth="  // Linienstärke
#define TAG_TEXTFRAME_BORDERCOLOR	"bordercolor="  // Linienfarbe
#define TAG_TEXTFRAME_FILLCOLOR		"bgcolor="    // Füllfarbe
#define TAG_TEXTFRAME_YPOS					"ypos="			// y-Position
#define TAG_TEXTFRAME_YPOS_MARGIN	"margin"		// relativ zu Rand
#define TAG_TEXTFRAME_YPOS_PARA		"paragraph"	// relativ zu Paragraph
#define TAG_TEXTFRAME_YPOS_PAGE		"page"			// relativ zu Seite

#define TAG_TEXTFRAME_GROWING			    "growing="	// wrapping des Textframes
#define TAG_TEXTFRAME_GROWING_NO	      "nogrow"	// (default) nicht expandieren, overset wird gesetzt
#define TAG_TEXTFRAME_GROWING_YES	      "grow"	  // automatisch expandieren

#define TAG_TEXTFRAME_WRAP			      "wrap="			// wrapping des Textframes
#define TAG_TEXTFRAME_WRAP_NOTEXT	      "nowrap"	// idNone: no text wrap (default)
#define TAG_TEXTFRAME_WRAP_AROUNDBOX	  "around"	// idBoundingBoxTextWrap: wrap around bounding box
#define TAG_TEXTFRAME_WRAP_CONTOUR	    "contour"	// idContour: wrap around object contour
#define TAG_TEXTFRAME_WRAP_JUMPOBJECT	  "jobject"	// idJumpObjectTextWrap: jump object
#define TAG_TEXTFRAME_WRAP_JUMPNEXT	    "jnext"	  // idNextColumnTextWrap: jump next colum

#define TAG_TEXTFRAME_VERTALIGN			"valign="			// vertikale Ausrichtung der Inhalte in Posframe
#define TAG_TEXTFRAME_VERTALIGN_TOP		  "top"				// Ausrichtung "Oben"
#define TAG_TEXTFRAME_VERTALIGN_CENTER	  "middle"		// Ausrichtung "Vertikal"
#define TAG_TEXTFRAME_VERTALIGN_BOTTOM	  "bottom"		// Ausrichtung "Unten"
#define TAG_TEXTFRAME_VERTALIGN_JUSTIFY	"justify"		// Ausrichtung vertikaler "Blocksatz"

#define TAG_TEXTFRAME_HORALIGN      "align="       // horizontale Ausrichtung der Inhalte in Posframe
#define TAG_TEXTFRAME_HORALIGN_LEFT    "left"
#define TAG_TEXTFRAME_HORALIGN_MIDDLE  "middle"
#define TAG_TEXTFRAME_HORALIGN_RIGHT   "right"

#define TAG_TEXTFRAME_COLUMNS_COUNT      "columns="    // Anzahl der Spalten
#define TAG_TEXTFRAME_COLUMNS_WIDTH      "colwidth="   // Spaltenbreite
#define TAG_TEXTFRAME_COLUMNS_GUTTER     "gutter="     // Stegbreite

#define TAG_TEXTFRAME_OVERSET           "overset="    // V3: Überlauf von Text?, in Variable True/False speichern
#define TAG_TEXTFRAME_EMPTY             "empty="      // V3: Leerer Rahmen?, in Variable True/False speichern
#define TAG_TEXTFRAME_VALID             "valid="      // V3: Textrahmen nur ausgeben, wenn Wert 1 oder True ist

#define TAG_LINE			  "<line "			// Linie
#define TAG_LINE_X1			"x1="					// x1
#define TAG_LINE_X2			"x2="					// x2
#define TAG_LINE_Y1			"y1="					// y1
#define TAG_LINE_Y2			"y2="					// y2
#define TAG_LINE_WIDTH	"thick="			// Strichstärke
#define TAG_LINE_COLOR	"color="			// color

#define TAG_BOX							"<box"         // Box
#define TAG_BOXEND					"</box>"        // Boxende
#define TAG_BOX_BGCOLOR		  "bgcolor="      // Hintergrundfarbe
#define BOX_BKGRND_DEFAULT	"gray-10"				// Defaulthintergrund der Box

// Mergefields
#define TAG_MERGEFIELD	 "<mergefield"    // Mergefield
#define TAG_FIELD_NAME   "name="          // Name des Feldes
#define TAG_FIELD_VALUE  "value="         // Wert des Feldes

// Tagbibliothek
#define TAG_LIBRARY				 "<library"		  // Field
#define TAG_LIBRARY_NAME   "name="        // Name der Tagsammlung
#define TAG_LIBRARY_MODE   "mode="        // Modus, 1=Ausgeben, 0=Sammeln(default), 
#define TAG_LIBRARY_END    "</library>"   // Ende der Sammlung

// EditField
#define TAG_EDITFIELD				 "<editfield"	// EditField: PDF
#define TAG_EDITFIELD_NAME   "name="      // Name des Feldes
#define TAG_EDITFIELD_VALUE  "value="     // Wert des Feldes

// Fields
#define TAG_FIELD				 "<field"					// Field
#define TAG_FIELD_DATE   "date="				  // Datumsfeld
#define TAG_FIELD_FORMEL "formel="				// Formelfeld
#define TAG_FIELD_SYMBOL "symbol="				// Symbol: {SYMBOL 43 \a \f Wingdings \h \* MERGEFORMAT}
#define TAG_FIELD_HYPERL "hyperlink="		  // http: {HYPERLINK "www.google.de" \* MERGEFORMAT}
#define TAG_FIELD_LINK   "link="		      // {LINK Excel.Sheet.8 "C:\\My Documents\\BOOK2.XLS" "Sheet1!R1C1" \a \r}
#define TAG_FIELD_SYMBOL_FONT "font="			// Font des Symbols

// Formfields
#define TAG_FORMFIELD					"<formfield"	 // 
#define TAG_FORMFIELD_LENGTH	"length="			 // 
#define TAG_FORMFIELD_MASK		"mask=" 			 // 
#define TAG_FORMFIELD_TYPE		"type="				 // 
#define TAG_FORMFIELD_VALUE		"value="			 // 
#define TAG_FORMFIELD_NAME		"name="			   // 
#define TAG_FORMFIELD_VALID		"valid="			 // nur ausgeben, wenn Wert 1 ist
#define TAG_FORMFIELD_NOVALID	"novalid="		 // die negierte Form des oberen
#define TAG_FORMFIELD_OVERSET	"overset="		 // Überlauf von Text?, in Variable True/False speichern
#define TAG_FORMFIELD_ALTERNATE	"alternate=" // Kommt es zu Überlauf wird dieser Text im Formfeld ausgegeben
#define TAG_FORMFIELD_FONTNAME	"fontname="	 // Font des Ausgabefeldes
#define TAG_FORMFIELD_FONTSIZE	"fontsize="	 // Fonthöhe des Formfeldes
#define TAG_FORMFIELD_POSITION	"position="  // PDF: X,Y Position des angegeben Formularfeldes
#define TAG_FORMFIELD_CODEPAGE	"codepage="  // PDF: Codepage des Formularfelds

// Tabulatoren
#define TAG_TAB_DEFAULT             "<tabdef"    // Default für Standard-Tabulator setzen
#define TAG_TAB_DEFAULT_SEPERATOR   "decimal="    // Default für Tabulator(Komma oder Punkt) setzen
#define TAG_TAB_SEPERATOR_COMMA     "comma"    // Komma als Trennzeichen
#define TAG_TAB_SEPERATOR_POINT     "dot"      // Punkt als Trennzeichen
#define TAG_TAB_DEFAULT_TAPOS       "pos="        // Defaultposition bei TAB
#define TAG_TAB_DEFAULT_TAPOS_VALUE "12.5"      // Defaultpositionswert bei TAB

#define TAG_TAB_SIMPLE		"<tab>"      // einfacher Tab
#define TAG_TAB						"<tab "			 // Tabulator
#define TAG_TAB_ALIGN			"align="		 // Ausrichtung des Tabulators
#define TAG_TAB_LEFT			  "left"		 // links ausgerichteter Tab
#define TAG_TAB_RIGHT			  "right"		 // rechts ausgerichteter Tab
#define TAG_TAB_CENTER		  "center"	 // zentrierter Tab
#define TAG_TAB_DECIMAL		  "decimal"	 // dezimaler Tab
#define TAG_TAB_POSITION	"pos="			 // Position des Tab
#define TAG_TAB_CLEAR			"clear"			 // Tabulatoren der aktuellen Zeile löschen
//nicht verwendet
#define TAG_TAB_DECCHAR  	"decchar="	 // dezimales Ausrichtungszeichen
#define TAG_TAB_LEADER  	"leader="		 // führende Zeichen 

//#define PDF_FORMTABLE			"<formtable>" // spezielle PDF-Tabelle um Index abzubilden
//#define PDF_FORMTABLEEND	"</formtable" // Tabulator

class CTagVariable{

public:
  CTagVariable(){};
  CTagVariable(const CXPubVariant& sValue)
  { m_sValue = sValue; }

  CXPubVariant& GetValue()
  { return m_sValue; }	

  void SetValue(const CXPubVariant& cValue)
  {  m_sValue = cValue; }	

  // Variablenname
  CXPubVariant m_sValue;			// Wert

};

typedef map<xtstring, CTagVariable> TagVariableList;
typedef map<xtstring, xtstring> TagStringList;

typedef enum TCWFT_WorkModus
{
  tWM_PreProcess,
  tWM_StyleInPreProcess,
  tWM_Work,
  tWM_StyleInWork,
};

class CConvertWorkerFromTag : public CConvertWorkerFrom
{
  friend class CConvertFromTagAddOn;
public:
  CConvertWorkerFromTag(CConvertFromTagAddOn* pAddOn);
  ~CConvertWorkerFromTag();

  bool SetTAGContent(const xtstring& sTAGContent);
  xtstring GetTAGContent();

  // from CConvertWorkerFrom
  virtual bool ConvertAndAdd(const xtstring& sDataStream, const xtstring& sXPubConst, TWorkPaperCopyType eWorkPaperCopyType);
	virtual bool GetLibraryStream(const xtstring& sLibraryName, xtstring& sDataStream);

protected:
  virtual CConvertWorkerFrom* Clone();

  void CleanUpTAGStreamBefore(xtstring& sDataStream);
  void CleanUpTAGStreamAfter(xtstring& sDataStream);

  // automat
	bool ProcessTags(const xtstring& cTag);

  double ConvertUnitOfLength(LPCXTCHAR lpValue);
  double ConvertUnitOfLength(double nValue);
	double MilimeterToTwips(double nValue);
	double InchToTwips(double nValue);
	double PicaToTwips(double nValue);
  static int CheckValidity(const xtstring& sVariableName);

  _ColorDef GetColor(xtstring& cColorName, COLORDEF clrfDefault = TAG_DEFAULT_BLACK);

protected:
  static int m_nTxtFrame;       // wenn kein Name vergeben intern Name zuordnen
  static xtstring m_sTxtFrame;  // Name des Textframe
  static TagVariableList m_TagVarList; // Liste der vom Anwender benutzten Variablen
  static TagStringList m_TagOversetList; // Liste der übergelaufenen Formfelder bei PDF
  static TagStringList m_TagLibraryList; // Libraryliste von TAGS
  static TagStringList m_TagPDFTableList; // Spezielles Handling bei Tabellenindizes Feldname()

  typedef enum TActiveUnitOfLength
  {
    tAUOL_Milimeter,
    tAUOL_Inch,
    tAUOL_Pica,
    tAUOL_Default = tAUOL_Milimeter,
  };
  TActiveUnitOfLength m_nActiveUnitOfLength;

public:
	static bool GetParameterValues(const xtstring& cTag, const xtstring& cParameter, xtstring& sVal1,
		xtstring& sVal2, xtstring& sVal3, xtstring& sVal4, int nTokCnt);
  
  bool Library(CConvertWorkerFromTag* pCW, const xtstring& cTag, const xtstring& sStream);
  static bool Paragraph(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool BeginPageBody(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool SetDefaults(CConvertWorkerFromTag* pCW);
	static bool Rectangle(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool Inline(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool Import(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool Image(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Font(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool FontAttribute(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Styles(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Tabulator(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool SetHeaderFooterPos(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool SwitchHeaderFooter(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Alignment(CConvertWorkerFromTag* pCW, const xtstring& sKeyword, const xtstring& cTag, void* lpToken);
  static bool TableCellActivate(CConvertWorkerFromTag* pCW, bool bDefault, int wRow, int wCol);
  static bool GetRowCol(CConvertWorkerFromTag* pCW, xtstring& sRow, xtstring& sCol, int& wRow, int& wCol);
  static bool GetRowCol(CConvertWorkerFromTag* pCW, const xtstring& sKeyword, const xtstring& cInTag, int& wRow, int& wCol);
  static bool Table(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool CursorPosition(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Checkbox(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Box(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool TextFrame(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool Line(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool PageMargin(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Module(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Asset(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Page(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Spread(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Document(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool DocumentInfo(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Password(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool MergeField(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool Field(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool FormField(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool SetPageCount(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool SetCurrentPage(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool InsertPageBreak(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool GoWritePosition(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool InsertSectionBreak(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool InsertColumnBreak(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool CanPageBreak(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
	static bool EditSection(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool InsertBookmark(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool InsertLink(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool InsertAnchor(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool LoadTemplate(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool RootPath(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Layer(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Layer_CS(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool Metrics(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);
  static bool ClassName(CConvertWorkerFromTag* pCW, const xtstring& cTag, void* lpToken);

  CConvertFromTagAddOn* m_pAddOn;
  TCWFT_WorkModus m_nWorkModus;

  void CheckKeyword(xtstring& sKeyword);
  void CheckParameter(const xtstring& sKeyword, const xtstring& sParameter);
};

inline bool CConvertWorkerFromTag::GetParameterValues(const xtstring& cTag, const xtstring& cParameter, xtstring& sVal1,
																							xtstring& sVal2, xtstring& sVal3, xtstring& sVal4, int nTokCnt)
{
	sVal1 = sVal2 = sVal3 = sVal4 = _XT("");
	size_t pos; 

	if ((pos=cTag.FindNoCase((_XT(" ") + cParameter).c_str())) != xtstring::npos)
	{  
		size_t nTokLength = cParameter.size()+1;
		// not used. bool bQuoMark = false;

		// Ende des Strings finden
		size_t nStPos=pos+nTokLength;
		if(cTag[nStPos] == '\"')
		{
			++nStPos;
			while(nStPos < cTag.size() && cTag[nStPos] != '\"')
				++nStPos;
		}
		else
		{
			++nStPos;
			while(nStPos < cTag.size() && cTag[nStPos] != ' ')
				++nStPos;
		}

		xtstring cValue;
	 // Fehler entdeckt... (Parameter ohne Zuweisung! z.B "<tab clearall>")
		if( (pos+nTokLength + (nStPos+1)-(pos+nTokLength)) <= cTag.length())
		{
			cValue = cTag.Mid(pos+nTokLength, (nStPos+1)-(pos+nTokLength));
			cValue.Trim(_XT("\""));
		}

		int nFieldCount = cValue.GetFieldCount(_XT(','));
		if ( nFieldCount > 0)
			sVal1 = cValue.GetField(_XT(','), 0);	
		if ( nFieldCount > 1)
			sVal2 = cValue.GetField(_XT(','), 1);	
		if ( nFieldCount > 2)
			sVal3 = cValue.GetField(_XT(','), 2);	
		if ( nFieldCount > 3)
			sVal4 = cValue.GetField(_XT(','), 3);	

    if(nTokCnt == 0)
      sVal1 = cValue;  

		return true;
	}
	return false;
}


#endif // !defined(_CONVERTWORKERFTAG_H_)
