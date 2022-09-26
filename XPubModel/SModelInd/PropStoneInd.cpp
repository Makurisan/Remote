// PropStoneInd.cpp: implementation of the CPropStoneInd classes.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "SSTLdef/STLdef.h"
#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"

#include "XPubVariant.h"
#include "ModelInd.h"
#include "PropStoneInd.h"

#include "MPModelFieldsInd.h"
#include "MPModelStoneInd.h"

#include "STools/Utils.h"




#define PROPSTONE_COPYRIGHT_XPUBLISHER_NOTICE     _XT("'X-Publisher, Designer'")
#define PROPSTONE_COPYRIGHT_NOTICE                _XT("'XPubPDFLibrary Version 2.0'")



XPPropertyDef m_sFreeProp[] = {
  {tXPP_FreeProps_Name,         tPT_NotRemovable, tPF_NoFunction, _XT("Name"),                0},
  {tXPP_FreeProps_Value,        tPT_Normal,       tPF_NoFunction, _XT("Value"),               0},
  {tXPP_FreeProps_EndOfGroup,   tPT_Normal,       tPF_NoFunction, 0,                          0}
};

XPPropertyDef m_sExcelProp[] = {
  {tXPP_EXCP_GroupProperties,   tPT_Group,        tPF_NoFunction, _XT("Eigenschaften"),           0},
  {tXPP_EXCP_Title,             tPT_Normal,       tPF_NoFunction, _XT("Titel"),                   0},
  {tXPP_EXCP_Theme,             tPT_Normal,       tPF_NoFunction, _XT("Thema"),                   0},
  {tXPP_EXCP_Author,            tPT_Normal,       tPF_NoFunction, _XT("Autor"),                   0},
  {tXPP_EXCP_Manager,           tPT_Normal,       tPF_NoFunction, _XT("Manager"),                 0},
  {tXPP_EXCP_Firma,             tPT_Normal,       tPF_NoFunction, _XT("Firma"),                   0},
  {tXPP_EXCP_Category,          tPT_Normal,       tPF_NoFunction, _XT("Kategorie"),               0},
  {tXPP_EXCP_Keywords,          tPT_Normal,       tPF_NoFunction, _XT("Stichwörter"),             0},
  {tXPP_EXCP_Comment,           tPT_Normal,       tPF_NoFunction, _XT("Kommentare"),              0},
  {tXPP_EXCP_GroupSecurity,     tPT_Group,        tPF_NoFunction, _XT("Sicherheit"),              0},
  {tXPP_EXCP_PasswordForOpen,   tPT_Normal,       tPF_NoFunction, _XT("Kennwort zum Öffnen"),     0},
  {tXPP_EXCP_PasswordForEdit,   tPT_Normal,       tPF_NoFunction, _XT("Berechtigungskennwort"),   0},
  {tXPP_EXCP_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0}
};

XPPropertyDef m_sRTFProp[] = {
  {tXPP_RTFP_GroupProperties,   tPT_Group,        tPF_NoFunction, _XT("Eigenschaften"),           0},
  {tXPP_RTFP_Title,             tPT_Normal,       tPF_NoFunction, _XT("Titel"),                   0},
  {tXPP_RTFP_Theme,             tPT_Normal,       tPF_NoFunction, _XT("Thema"),                   0},
  {tXPP_RTFP_Author,            tPT_Normal,       tPF_NoFunction, _XT("Autor"),                   0},
  {tXPP_RTFP_Manager,           tPT_Normal,       tPF_NoFunction, _XT("Manager"),                 0},
  {tXPP_RTFP_Firma,             tPT_Normal,       tPF_NoFunction, _XT("Firma"),                   0},
  {tXPP_RTFP_Category,          tPT_Normal,       tPF_NoFunction, _XT("Kategorie"),               0},
  {tXPP_RTFP_Keywords,          tPT_Normal,       tPF_NoFunction, _XT("Stichwörter"),             0},
  {tXPP_RTFP_Comment,           tPT_Normal,       tPF_NoFunction, _XT("Kommentare"),              0},
  {tXPP_RTFP_GroupSecurity,     tPT_Group,        tPF_NoFunction, _XT("Sicherheit"),              0},
  {tXPP_RTFP_PasswordForOpen,   tPT_Normal,       tPF_NoFunction, _XT("Kennwort zum Öffnen"),     0},
  {tXPP_RTFP_PasswordForEdit,   tPT_Normal,       tPF_NoFunction, _XT("Berechtigungskennwort"),   0},
  {tXPP_RTFP_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0}
};

XPPropertyDef m_sPDFProp[] = {
  {tXPP_PDFP_DocSummary,        tPT_Group,        tPF_NoFunction, _XT("Dokumenteigenschaften"),   0},
  {tXPP_PDFP_Title,             tPT_Normal,       tPF_NoFunction, _XT("Titel"),                   0},
  {tXPP_PDFP_Subject,           tPT_Normal,       tPF_NoFunction, _XT("Thema"),                   0},
  {tXPP_PDFP_Author,            tPT_Normal,       tPF_NoFunction, _XT("Verfasser"),               0},
  {tXPP_PDFP_Keywords,          tPT_Normal,       tPF_NoFunction, _XT("Stichwörter"),             0},
  {tXPP_PDFP_Producer,          tPT_DontShow,     tPF_NoFunction, _XT("Erstellt mit"),            PROPSTONE_COPYRIGHT_XPUBLISHER_NOTICE},
  {tXPP_PDFP_Creator,           tPT_DontShow,     tPF_NoFunction, _XT("Erzeugt mit"),             PROPSTONE_COPYRIGHT_NOTICE},
//Kennwort zum Öffnen: (setzt Berechtigungskennwort voraus)
  {tXPP_PDFP_GroupSecurity,     tPT_Group,        tPF_NoFunction, _XT("Sicherheit"),              0},
  {tXPP_PDFP_UserPassword,      tPT_Normal,       tPF_NoFunction, _XT("Kennwort zum Öffnen<br><font size=1>(nur mit Berechtigungskennwort)</font>"),  0},
  {tXPP_PDFP_MasterPassword ,   tPT_Normal,       tPF_NoFunction, _XT("Berechtigungskennwort"),   0},
  {tXPP_PDFP_Edit,              tPT_Normal,       tPF_NoFunction, _XT("Ändern"),                  _XT("True")},
  {tXPP_PDFP_Copy,              tPT_Normal,       tPF_NoFunction, _XT("Kopieren"),                _XT("True")},
  {tXPP_PDFP_Commenting,        tPT_Normal,       tPF_NoFunction, _XT("Kommentieren"),            _XT("True")},
  {tXPP_PDFP_Print,             tPT_Normal,       tPF_NoFunction, _XT("Drucken<br><font size=1>(hohe Auflösung)</font>"),     _XT("True")},
  {tXPP_PDFP_LowPrinting,       tPT_Normal,       tPF_NoFunction, _XT("Drucken<br><font size=1>(niedrige Auflösung)</font>"), _XT("False")},
  {tXPP_PDFP_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0}
};

XPPropertyDef m_sUNZIPProp[] = {
  {tXPP_UNZIP_Archivename,      tPT_Normal,       tPF_NoFunction, _XT("Archivname"),              0}, // der Archivename incl. Pfad
  {tXPP_UNZIP_Files,            tPT_Normal,       tPF_NoFunction, _XT("Dateien"),                 0}, // die zu entpackenden Dateien aus Archiv
  {tXPP_UNZIP_Destination,      tPT_Normal,       tPF_NoFunction, _XT("Verzeichnis"),             0}, // das Zielverzeichnis der zu entpackenden Dateien
  {tXPP_UNZIP_Password,         tPT_Normal,       tPF_NoFunction, _XT("Kennwort"),                0}, // Kennwort
  {tXPP_UNZIP_Count,            tPT_Normal,       tPF_NoFunction, 0,                              0}
};

XPPropertyDef m_sZIPProp[] = {
  {tXPP_ZIP_Archivename,        tPT_Normal,       tPF_NoFunction, _XT("Archivname"),              0}, // der Archivename incl. Pfad
  {tXPP_ZIP_AddFiles,           tPT_Normal,       tPF_NoFunction, _XT("Dateien"),                 0}, // die hinzuzufügenden Dateien
  {tXPP_ZIP_SubDirs,            tPT_Normal,       tPF_NoFunction, _XT("Unterverzeichnisse"),      0}, // Unterverzeichnisse mit einschließen
  {tXPP_ZIP_StorePath,          tPT_Normal,       tPF_NoFunction, _XT("Pfad speichern"),          0}, // Pfade in Archiv speichern
  {tXPP_ZIP_Password,           tPT_Normal,       tPF_NoFunction, _XT("Kennwort"),                0}, // Passwort
  {tXPP_ZIP_Count,              tPT_Normal,       tPF_NoFunction, 0,                              0}
};

XPPropertyDef m_sFTPProp[] = {
  {tXPP_FTP_Server,             tPT_Normal,       tPF_NoFunction, _XT("Server"),                  0}, // Server. ftp.x-publisher.com
  {tXPP_FTP_Port,               tPT_Normal,       tPF_NoFunction, _XT("Port"),                    0}, // Port auf dem Server lauscht
  {tXPP_FTP_RemoteStartDir,     tPT_Normal,       tPF_NoFunction, _XT("Remote Startverzeichnis"), 0}, // Startverzeichnis auf Server
  {tXPP_FTP_RemoteFile,         tPT_Normal,       tPF_NoFunction, _XT("Remote Datei"),            0}, // Dateiname auf Server
  {tXPP_FTP_LocalFile,          tPT_Normal,       tPF_NoFunction, _XT("Lok. Datei"),              0}, // lokaler Dateiname mit Pfad
  {tXPP_FTP_Download,           tPT_Normal,       tPF_NoFunction, _XT("Download/Upload<br><font size=1>(True=Download)</font>"),  0}, // TRUE und FALSE für Up-/Download
  {tXPP_FTP_Username,           tPT_Normal,       tPF_NoFunction, _XT("Benutzername"),            0}, // Benutzername
  {tXPP_FTP_Password,           tPT_Normal,       tPF_NoFunction, _XT("Kennwort"),                0}, // Passwort
  {tXPP_FTP_TimeOut,            tPT_Normal,       tPF_NoFunction, _XT("Timeout"),                 0}, // Abbruch nach Ablauf der Zeit
  {tXPP_FTP_Transfermode,       tPT_Normal,       tPF_NoFunction, _XT("Transfermode<br><font size=1>(Binary/Text)</font>"), 0}, // Abbruch nach Ablauf der Zeit
  {tXPP_FTP_Count,              tPT_Normal,       tPF_NoFunction, 0,                              0}
};

XPPropertyDef m_sCMDProp[] = {
  {tXPP_CMD_CmdFile,            tPT_Normal,       tPF_NoFunction, _XT("Befehlsdatei"),            0}, // z.B. "RunWord.cmd"
  {tXPP_CMD_CurDir,             tPT_Normal,       tPF_NoFunction, _XT("Verzeichnis"),             0},
  {tXPP_CMD_CmdLine,            tPT_Normal,       tPF_NoFunction, _XT("Parameter"),               0},
  {tXPP_CMD_Count,              tPT_Normal,       tPF_NoFunction, 0,                              0}
};

XPPropertyDef m_sSMTPProp[] = {
  {tXPP_SMTP_SMTPServer,        tPT_Normal,       tPF_NoFunction, _XT("Server"),                  0}, // z.B. "smtp.1und1.com"
  {tXPP_SMTP_SMTPPort,          tPT_Normal,       tPF_NoFunction, _XT("Port<br><font size=1>(z.B. 25)</font>"), 0}, // z.B. "25"
  {tXPP_SMTP_From,              tPT_Normal,       tPF_NoFunction, _XT("Absender"),                0}, // Absender
  {tXPP_SMTP_To,                tPT_Normal,       tPF_NoFunction, _XT("Empfänger"),               0}, // Empfänger
  {tXPP_SMTP_Subject,           tPT_Normal,       tPF_NoFunction, _XT("Betreff"),                 0}, // Betreff
  {tXPP_SMTP_Filename,          tPT_Normal,       tPF_NoFunction, _XT("Messagedatei"),            0}, // Messagetextfile
  {tXPP_SMTP_User,              tPT_Normal,       tPF_NoFunction, _XT("Benutzer"),                0}, // Benutzer
  {tXPP_SMTP_Password,          tPT_Normal,       tPF_NoFunction, _XT("Kennwort"),                0}, // Passwort
  {tXPP_SMTP_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0}
};

XPPropertyDef m_sOutlookProp[] = {
  {tXPP_OUTL_From,              tPT_Normal,       tPF_NoFunction, _XT("Absender"),                0},
  {tXPP_OUTL_To,                tPT_Normal,       tPF_NoFunction, _XT("Empfänger"),               0},
  {tXPP_OUTL_Subject,           tPT_Normal,       tPF_NoFunction, _XT("Betreff"),                 0},
  {tXPP_OUTL_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0},
};
XPPropertyDef m_sEncodingProp[] = {
  {tXPP_ENCO_FileFrom,          tPT_Normal,       tPF_NoFunction, _XT("Quelldatei"),              0},
  {tXPP_ENCO_FileTo,            tPT_Normal,       tPF_NoFunction, _XT("Zieldatei"),               0},
  {tXPP_ENCO_Mode,              tPT_Normal,       tPF_NoFunction, _XT("Format<br><font size=1>(Bin64/UUEncode)</font>"),  0},
  {tXPP_ENCO_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0},
};
XPPropertyDef m_sEncryptionProp[] = {
  {tXPP_ENCR_FileFrom,          tPT_Normal,       tPF_NoFunction, _XT("Quelldatei"),              0},
  {tXPP_ENCR_FileTo,            tPT_Normal,       tPF_NoFunction, _XT("Zieldatei"),               0},
  {tXPP_ENCR_Key,               tPT_Normal,       tPF_NoFunction, _XT("Schlüssel"),               0},
  {tXPP_ENCR_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0},
};
XPPropertyDef m_sFileMoveProp[] = {
  {tXPP_FMOV_FileFrom,          tPT_Normal,       tPF_NoFunction, _XT("Quelldatei"),              0},
  {tXPP_FMOV_FileTo,            tPT_Normal,       tPF_NoFunction, _XT("Zieldatei"),               0},
  {tXPP_FMOV_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0},
};
XPPropertyDef m_sFileCopyProp[] = {
  {tXPP_FCOP_FileFrom,          tPT_Normal,       tPF_NoFunction, _XT("Quelldatei"),              0},
  {tXPP_FCOP_FileTo,            tPT_Normal,       tPF_NoFunction, _XT("Zieldatei"),               0},
  {tXPP_FCOP_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0},
};
XPPropertyDef m_sFileRemoveProp[] = {
  {tXPP_FREM_File,              tPT_Normal,       tPF_NoFunction, _XT("Dateiname"),               0},
  {tXPP_FREM_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0},
};
XPPropertyDef m_sINDDNewDocProp[] = {
  {tXPP_INDD_DocumentPreset,    tPT_Normal,       tPF_NoFunction, _XT("Dokumentvoreinstellung"),  _XT("'[Default]'")},
  {tXPP_INDD_NumberOfPages,     tPT_Normal,       tPF_NoFunction, _XT("Seitenzahl"),              _XT("1")},
  {tXPP_INDD_FacingPages,       tPT_Normal,       tPF_NoFunction, _XT("Facing Pages"),            _XT("True")},
  {tXPP_INDD_MasterTextFrame,   tPT_Normal,       tPF_NoFunction, _XT("Master Text Frame"),       _XT("False")},
  {tXPP_INDD_Page,              tPT_Group,        tPF_NoFunction, _XT("Dokumentgrößen"),          0},
  {tXPP_INDD_PageSize,          tPT_Normal,       tPF_NoFunction, _XT("Seitengröße"),             _XT("'A4'")},
  {tXPP_INDD_Width,             tPT_Normal,       tPF_NoFunction, _XT("Breite"),                  _XT("'210 mm'")},
  {tXPP_INDD_Height,            tPT_Normal,       tPF_NoFunction, _XT("Höhe"),                    _XT("'297 mm'")},
  {tXPP_INDD_Orientation,       tPT_Normal,       tPF_NoFunction, _XT("Orientierung"),            _XT("'Portrait'")},
  {tXPP_INDD_Columns,           tPT_Group,        tPF_NoFunction, _XT("Spalten"),                 0},
  {tXPP_INDD_ColumnsNumber,     tPT_Normal,       tPF_NoFunction, _XT("Zahl"),                    _XT("2")},
  {tXPP_INDD_ColumnsGutter,     tPT_Normal,       tPF_NoFunction, _XT("Stegbreite"),              _XT("'4,233 mm'")},
  {tXPP_INDD_Margins,           tPT_Group,        tPF_NoFunction, _XT("Ränder"),                  0},
  {tXPP_INDD_MarginsTop,        tPT_Normal,       tPF_NoFunction, _XT("Oben"),                    _XT("'12,7 mm'")},
  {tXPP_INDD_MarginsBottom,     tPT_Normal,       tPF_NoFunction, _XT("Unten"),                   _XT("'12,7 mm'")},
  {tXPP_INDD_MarginsInside,     tPT_Normal,       tPF_NoFunction, _XT("Links"),                   _XT("'12,7 mm'")},
  {tXPP_INDD_MarginsOutside,    tPT_Normal,       tPF_NoFunction, _XT("Rechts"),                  _XT("'12,7 mm'")},
  {tXPP_INDD_Count,             tPT_Normal,       tPF_NoFunction, 0,                              0}
};
XPPropertyDef m_sB64TProp[] = {
  {tXPP_B64T_VisibleName,       tPT_Normal,                   tPF_NoFunction,     _XT("Sichtbarer Name"), 0},
  {tXPP_B64T_DestinateFileName, tPT_Normal,                   tPF_NoFunction,     _XT("Zieldateiname"),   0},
  {tXPP_B64T_ReferenceFileName, tPT_Normal,                   tPF_NoFunction,     _XT("Dateiname"),       0},
  {tXPP_B64T_FileContent,       tPT_SetOverFunction_DontShow, tPF_SetFileContent, _XT("FileContent"),     0},
  {tXPP_B64T_FileDatum,         tPT_SetOverFunction_DontShow, tPF_SetFileDatum,   _XT("FileDatum"),       0},
  {tXPP_B64T_FileLength,        tPT_SetOverFunction_DontShow, tPF_SetFileLength,  _XT("FileLength"),      0},
  {tXPP_B64T_ShowInClient,      tPT_Normal,                   tPF_NoFunction,     _XT("Im Klient darstellen"), 0},
  {tXPP_B64T_Count,             tPT_Normal,                   tPF_NoFunction,     0,                      0},
};
XPPropertyDef m_sB64TIProp[] = {
  {tXPP_B64TI_VisibleName,      tPT_Normal,                   tPF_NoFunction,     _XT("Sichtbarer Name"), 0},
  {tXPP_B64TI_DestinateFileName,tPT_Normal,                   tPF_NoFunction,     _XT("Zieldateiname"),   0},
  {tXPP_B64TI_ReferenceFileName,tPT_Normal,                   tPF_NoFunction,     _XT("Dateiname"),       0},
  {tXPP_B64TI_FileMD5,          tPT_SetOverFunction_DontShow, tPF_SetFileMD5,     _XT("FileMD5"),         0},
  {tXPP_B64TI_FileDatum,        tPT_SetOverFunction_DontShow, tPF_SetFileDatum,   _XT("FileDatum"),       0},
  {tXPP_B64TI_FileLength,       tPT_SetOverFunction_DontShow, tPF_SetFileLength,  _XT("FileLength"),      0},
  {tXPP_B64TI_Count,            tPT_Normal,                   tPF_NoFunction,     0,                      0},
};
// XPubWizard --> InsertPointXPPropertyDef





XPPropertyGroupDef m_sPropGroups[] =
{
  {tPGMT_OrdinaryGroup, tXPP_FreeProps,               _XT("Freie Eigenschaften"),   m_sFreeProp},
  {tPGMT_OrdinaryGroup, tXPP_ExcelProps,              _XT("Excel"),                 m_sExcelProp},
  {tPGMT_OrdinaryGroup, tXPP_PDFProps,                _XT("PDF-Dokument"),          m_sPDFProp},
  {tPGMT_OrdinaryGroup, tXPP_RTFProps,                _XT("Word-Dokument"),         m_sRTFProp},
  {tPGMT_PrePostGroup,  tXPP_UNZIPProps,              _XT("UNZIP Archiv"),          m_sUNZIPProp},        // pre / post
  {tPGMT_PrePostGroup,  tXPP_ZIPProps,                _XT("ZIP Archiv"),            m_sZIPProp},          // pre / post
  {tPGMT_PrePostGroup,  tXPP_FTPTransferProps,        _XT("FTP Up-/Download"),      m_sFTPProp},          // pre / post
  {tPGMT_PrePostGroup,  tXPP_CMDProps,                _XT("Befehlsdatei(*.cmd)"),   m_sCMDProp},          // pre / post
  {tPGMT_PrePostGroup,  tXPP_SMTPProps,               _XT("SMTP"),                  m_sSMTPProp},         // pre / post
  {tPGMT_OrdinaryGroup, tXPP_OutlookProps,            _XT("Outlook"),               m_sOutlookProp},
  {tPGMT_PrePostGroup,  tXPP_EncodingProps,           _XT("Kodierung"),             m_sEncodingProp},     // pre / post
  {tPGMT_PrePostGroup,  tXPP_EncryptionProps,         _XT("Verschlüsselung"),       m_sEncryptionProp},   // pre / post
  {tPGMT_PrePostGroup,  tXPP_FileMoveProps,           _XT("Datei verschieben"),     m_sFileMoveProp},     // pre / post
  {tPGMT_PrePostGroup,  tXPP_FileCopyProps,           _XT("Datei kopieren"),        m_sFileCopyProp},     // pre / post
  {tPGMT_PrePostGroup,  tXPP_FileRemoveProps,         _XT("Datei löschen"),         m_sFileRemoveProp},   // pre / post
  {tPGMT_OrdinaryGroup, tXPP_IndesignNewDocProps,     _XT("Indesign Dokument"),     m_sINDDNewDocProp},
  {tPGMT_OrdinaryGroup, tXPP_Base64TransferProps,     _XT("Datei übertragen"),      m_sB64TProp},
  {tPGMT_OrdinaryGroup, tXPP_Base64TransferInfoProps, _XT("Datei info übertragen"), m_sB64TIProp},
// XPubWizard --> InsertPointIntoXPPropertyGroupDef
  {tPGMT_Last,          tXPP_Count,                   0,                            0}
};







//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CPropStonePropertyInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CPropStonePropertyInd::CPropStonePropertyInd(TXPubProperty nPropGroup)
                      :m_caExpression(CConditionOrAssignment::tCOAT_PropStoneProperty_Assignment, true)
{
  // nPropGroup muss Wert einer Gruppe sein
  // Gruppe ID = X * GROUPSTEP
  assert(nPropGroup % GROUPSTEP == 0);

  if (nPropGroup % GROUPSTEP != 0)
    nPropGroup = tXPP_FreeProps;

  m_pPropertiesParent = 0;
  m_nPropGroup = nPropGroup;

  m_caExpression.Reset();
  m_sInternalName.clear();
  m_nProperty = tXPP_EmptyProperty;
  m_nPropertyType = tPT_Normal;
  m_nPropertyFunction = tPF_NoFunction;
  m_xValue.Clear();
}

CPropStonePropertyInd::~CPropStonePropertyInd()
{
}

bool CPropStonePropertyInd::operator == (CPropStonePropertyInd& cProperty)
{
  if (m_caExpression != cProperty.m_caExpression)
    return false;
  if (m_sInternalName != cProperty.m_sInternalName)
    return false;
  if (m_nProperty != cProperty.m_nProperty)
    return false;
  if (m_nPropertyType != cProperty.m_nPropertyType)
    return false;
  if (m_nPropertyFunction != cProperty.m_nPropertyFunction)
    return false;
  if (!(m_xValue == cProperty.m_xValue))
    return false;
  return true;
}

void CPropStonePropertyInd::SetPropStonePropertyInd(CPropStonePropertyInd* pProperty)
{
  assert(pProperty);
  if (!pProperty)
    return;

  m_caExpression = pProperty->m_caExpression;
  m_sInternalName = pProperty->m_sInternalName;
  m_nProperty = pProperty->m_nProperty;
  m_nPropertyType = pProperty->m_nPropertyType;
  m_nPropertyFunction = pProperty->m_nPropertyFunction;
  m_xValue = pProperty->m_xValue;

  if (m_pPropertiesParent)
    m_pPropertiesParent->SetChanged();
  return;
}

bool CPropStonePropertyInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  TFieldType vt;
  xtstring sVal;
  m_xValue.Serialize(vt, sVal);

  pXMLDoc->SetAttrib(PROPSTONE_PROPERTY_INTERNALNAME, m_sInternalName.c_str());
  pXMLDoc->SetAttrib(PROPSTONE_PROPERTY_PROPERTY, m_nProperty);
  pXMLDoc->SetAttrib(PROPSTONE_PROPERTY_PROPERTYTYPE, m_nPropertyType);
  pXMLDoc->SetAttrib(PROPSTONE_PROPERTY_PROPERTYFUNCTION, m_nPropertyFunction);
  pXMLDoc->SetAttrib(PROPSTONE_PROPERTY_VALUETYPE, vt);
  pXMLDoc->SetAttrib(PROPSTONE_PROPERTY_VALUEVALUE, sVal.c_str());
  if (vt == tFieldTypeEmpty)
    m_caExpression.SaveXMLContent(pXMLDoc);

  return true;
}

bool CPropStonePropertyInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  TFieldType vt;
  xtstring sVal;

  m_sInternalName = pXMLDoc->GetAttrib(PROPSTONE_PROPERTY_INTERNALNAME);
  m_nProperty = (TXPubProperty)pXMLDoc->GetAttribLong(PROPSTONE_PROPERTY_PROPERTY);
  m_nPropertyType = (TPropertyType)pXMLDoc->GetAttribLong(PROPSTONE_PROPERTY_PROPERTYTYPE);
  m_nPropertyFunction = (TPropertyFunction)pXMLDoc->GetAttribLong(PROPSTONE_PROPERTY_PROPERTYFUNCTION);
  vt = (TFieldType)pXMLDoc->GetAttribLong(PROPSTONE_PROPERTY_VALUETYPE);
  sVal = pXMLDoc->GetAttrib(PROPSTONE_PROPERTY_VALUEVALUE);
  m_xValue.Deserialize(vt, sVal);
  if (vt == tFieldTypeEmpty)
    m_caExpression.ReadXMLContent(pXMLDoc);

  return true;
}



void CPropStonePropertyInd::SetExpression(const CConditionOrAssignment& caExpression)
{
  m_caExpression = caExpression;
  if (m_pPropertiesParent)
    m_pPropertiesParent->SetChanged();
}
void CPropStonePropertyInd::SetInternalName(const xtstring& sName)
{
  m_sInternalName = sName;
  if (m_pPropertiesParent)
    m_pPropertiesParent->SetChanged();
}
void CPropStonePropertyInd::SetProperty(TXPubProperty nProperty)
{
  m_nProperty = nProperty;
  if (m_pPropertiesParent)
    m_pPropertiesParent->SetChanged();
}
void CPropStonePropertyInd::SetPropertyType(TPropertyType nPropertyType)
{
  m_nPropertyType = nPropertyType;
  if (m_pPropertiesParent)
    m_pPropertiesParent->SetChanged();
}
void CPropStonePropertyInd::SetPropertyFunction(TPropertyFunction nPropertyFunction)
{
  m_nPropertyFunction = nPropertyFunction;
  if (m_pPropertiesParent)
    m_pPropertiesParent->SetChanged();
}
void CPropStonePropertyInd::SetValue(const CXPubVariant& xValue)
{
  m_xValue = xValue;
  if (m_pPropertiesParent)
    m_pPropertiesParent->SetChanged();
}













//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CPropStonePropertiesInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CPropStonePropertiesInd::CPropStonePropertiesInd(CSyncWPNotifiersGroup* pGroup)
                        :CModelBaseInd(tPropStoneProperties),
                         CSyncNotifier_WP(tWPNotifier_PropStoneProperties, pGroup),
                         CSyncResponse_Proj(tRO_PropStone_Property),
                         CSyncResponse_ER(tRO_PropStone_Property),
                         CSyncResponse_IFS(tRO_PropStone_Property)
{
  m_bPropertiesChanged = false;

  m_pPropStoneParent = 0;
  m_nGroupType = tXPP_EmptyProperty;
}

CPropStonePropertiesInd::CPropStonePropertiesInd(TXPubProperty nGroupType)
                        :CModelBaseInd(tPropStoneProperties),
                         CSyncNotifier_WP(tWPNotifier_PropStoneProperties, NULL),
                         CSyncResponse_Proj(tRO_PropStone_Property),
                         CSyncResponse_ER(tRO_PropStone_Property),
                         CSyncResponse_IFS(tRO_PropStone_Property)
{
  m_bPropertiesChanged = false;

  m_pPropStoneParent = 0;
  SetPropertyGroupType(nGroupType, true);
}

CPropStonePropertiesInd::CPropStonePropertiesInd(CPropStonePropertiesInd& cProperties)
                        :CModelBaseInd(tPropStoneProperties),
                         CSyncNotifier_WP(tWPNotifier_PropStoneProperties, NULL),
                         CSyncResponse_Proj(tRO_PropStone_Property),
                         CSyncResponse_ER(tRO_PropStone_Property),
                         CSyncResponse_IFS(tRO_PropStone_Property)
{
  SetSyncGroup(cProperties.GetSyncGroup());
  SetPropStonePropertiesInd(&cProperties);
}

CPropStonePropertiesInd::CPropStonePropertiesInd(CPropStonePropertiesInd* pProperties)
                        :CModelBaseInd(tPropStoneProperties),
                         CSyncNotifier_WP(tWPNotifier_PropStoneProperties, NULL),
                         CSyncResponse_Proj(tRO_PropStone_Property),
                         CSyncResponse_ER(tRO_PropStone_Property),
                         CSyncResponse_IFS(tRO_PropStone_Property)
{
  assert(pProperties);
  SetSyncGroup(pProperties->GetSyncGroup());
  SetPropStonePropertiesInd(pProperties);
}

CPropStonePropertiesInd::~CPropStonePropertiesInd()
{
  RemoveAllProperties();
}

bool CPropStonePropertiesInd::operator == (const CPropStonePropertiesInd& cProperties)
{
  if (m_nGroupType != cProperties.m_nGroupType)
    return false;
  CPropStonePropertiesInd* pProperties = (CPropStonePropertiesInd*)&cProperties;
  for (size_t nI = 0; nI < cProperties.CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pOther = pProperties->GetProperty(nI);
    if (!pOther)
      return false;
    CPropStonePropertyInd* pThis = GetProperty(nI);
    if (!pThis)
      return false;
    if (!((*pOther) == (*pThis)))
      return false;
  }
  return true;
}
void CPropStonePropertiesInd::operator = (const CPropStonePropertiesInd& cProperties)
{
  SetPropStonePropertiesInd((CPropStonePropertiesInd*)&cProperties);
}

void CPropStonePropertiesInd::SetPropStonePropertiesInd(CPropStonePropertiesInd* pProperties)
{
  if (!pProperties)
    return;

  m_bPropertiesChanged = true;

  RemoveAllProperties();
  m_nGroupType = pProperties->m_nGroupType;
  for (size_t nI = 0; nI < pProperties->CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pCopy = pProperties->GetProperty(nI);
    if (!pCopy)
      continue;
    CPropStonePropertyInd cProperty(GetPropertyGroupType());
    cProperty.SetPropStonePropertyInd(pCopy);
    AddPropertyIntern(&cProperty);
  }
}

bool CPropStonePropertiesInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);


  pXMLDoc->SetAttrib(PROPSTONE_PROPERTIES_PROPGROUP, GetPropertyGroupType());

  for (size_t nI = 0; nI < m_properties.size(); nI++)
  {
    if (pXMLDoc->AddChildElem(PROPSTONE_PROPERTY, _XT("")))
    {
      pXMLDoc->IntoElem();

      m_properties[nI]->SaveXMLContent(pXMLDoc);

      pXMLDoc->OutOfElem();
    }
  }

  bRet = true;
  m_bPropertiesChanged = false;

  return bRet;
}

bool CPropStonePropertiesInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  TXPubProperty nGroup = (TXPubProperty)pXMLDoc->GetAttribLong(PROPSTONE_PROPERTIES_PROPGROUP);
  if (nGroup % GROUPSTEP != 0 || nGroup >= tXPP_Count)
    nGroup = tXPP_FreeProps;

  m_nGroupType = tXPP_EmptyProperty;
  SetPropertyGroupType(nGroup, false);

  // jetzt muessen wir kontrollieren, um welche Prop Gruppe sich handelt
  // Falls sich um 'tPGT_FreeProps' handelt, dann muessen wir
  // die voreingestellte Eigenschaften loeschen, weil der Benutzer diese
  // loeschen konnte und es waere nicht gut solche Eigenschaften ihm
  // wieder darzustellen.
  // Bei anderen Prop Gruppen, muessen wir eben die voreingestellte Eigenschaften
  // ubernehmen, weil wir konnten die voreingestellte Eigenschaften aendern
  // und evtl. die neue Eigenschaften wuerde der Benutzer nicht mehr bekommen.
  if (GetPropertyGroupType() == tXPP_FreeProps)
    // Aber die 'NotRemovable' lassen wir drin.
    RemoveAllProperties(true);

  while (pXMLDoc->FindChildElem(PROPSTONE_PROPERTY))
  {
    pXMLDoc->IntoElem();

    CPropStonePropertyInd cProp(m_nGroupType);
    cProp.SetParent(this);
    cProp.ReadXMLContent(pXMLDoc);
    AssignPropertyIntern(&cProp);

    pXMLDoc->OutOfElem();
  }

  bRet = true;
  m_bPropertiesChanged = false;

  return bRet;
}

bool CPropStonePropertiesInd::CheckTypeAndSetProperties(const xtstring& sXMLDocText)
{
  bool bRet = true;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLDocText.c_str());
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_PROPSTONE))
  {
    if (cXMLDoc.FindChildElem(PROPSTONE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <PropStone>
      // load fields
      if (cXMLDoc.FindChildElem(PROPSTONE_PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>

        CPropStonePropertiesInd cProps((CSyncWPNotifiersGroup*)0);
        cProps.ReadXMLContent(&cXMLDoc);
        if (cProps.GetPropertyGroupType() != GetPropertyGroupType())
          bRet = false;
        else
          SetPropStonePropertiesInd(&cProps);

        cXMLDoc.OutOfElem();
        // in <Node>
      }

      cXMLDoc.OutOfElem();
      m_bPropertiesChanged = false;
    }
  }
  return bRet;
}

TPropertyGroupMainType CPropStonePropertiesInd::GetPropertyGroupMainType()
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (m_sPropGroups[nI].nPropertyGroupType == m_nGroupType)
      return m_sPropGroups[nI].nPropertyGroupMainType;
  }
  return tPGMT_GroupNotValid;
}

xtstring CPropStonePropertiesInd::GetPropertyGroupTypeName()
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (m_sPropGroups[nI].nPropertyGroupType == m_nGroupType)
      return m_sPropGroups[nI].pPropGroupName;
  }
  return _XT("");
}

CPropStonePropertyInd* CPropStonePropertiesInd::Call_SetOverFunction_GetFirstPreviousNormalProperty(size_t nForThisProperty)
{
  // Von der Eigenschaft (ueber nForThisProperty definiert) suchen wir
  // erste vorherige Eigenschaft, die vom typ 'tPT_Normal' ist.
  CPropStonePropertyInd* propData = 0;
  {
    size_t nSearchedPropertyIndex = nForThisProperty;
    do
    {
      nSearchedPropertyIndex--;
      propData = GetProperty(nSearchedPropertyIndex);
    }
    while (propData && propData->GetPropertyType() != tPT_Normal);
  }
  return propData;
}

bool CPropStonePropertiesInd::Call_SetOverFunction(size_t nIndexOfProperty)
{
  CPropStonePropertyInd* propToSet = GetProperty(nIndexOfProperty);
  if (!propToSet)
    return false;
  if (propToSet->GetPropertyType() != tPT_SetOverFunction_DontShow)
    return false;

  // Wir muessen Eigenschaft suchen, wo der Dateiname steht.
  // Es ist erste vorherige Eigenschaft, die vom Typ 'tPT_Normal' ist.
  CPropStonePropertyInd* propData = Call_SetOverFunction_GetFirstPreviousNormalProperty(nIndexOfProperty);
  if (!propData)
    return false;

  if (propToSet->GetPropertyFunction() == tPF_SetFileContent)
  {
    xtstring sFullPathFileName = propData->GetValue();

    unsigned char* data = 0;
    unsigned long nLen = 0;
    // zuerst laenge bestimmen
    if (!CUtils::ReadFileContent(sFullPathFileName, nLen, 0))
      return false;

    if (!nLen)
    {
      propToSet->SetValue(xtstring(_XT("")));
    }
    else
    {
      data = new unsigned char[nLen];
      if (!data)
        return false;
      if (!CUtils::ReadFileContent(sFullPathFileName, nLen, data))
      {
        delete data;
        return false;
      }
      xtstring sContent = CXPubMarkUp::EncodeBase64(data, nLen);
      propToSet->SetValue(sContent);
      delete data;
    }
  }
  else if (propToSet->GetPropertyFunction() == tPF_SetFileDatum)
  {
    xtstring sFullPathFileName = propData->GetValue();

    time_t timeChanged;
    if (!CUtils::GetFileChangedTime(sFullPathFileName, timeChanged))
      return false;

    QLHTM nQLHTM;
    nQLHTM.Reset();
    CXPubDateTime::_LOCALTIME_1970(&nQLHTM, timeChanged);

    CXPubDateTime time;
    time.m_nTime = CXPubDateTime::_MKTIME(&nQLHTM);
    propToSet->SetValue(time);
  }
  else if (propToSet->GetPropertyFunction() == tPF_SetFileLength)
  {
    xtstring sFullPathFileName = propData->GetValue();

    long nFileLength;
    if (!CUtils::GetFileLength(sFullPathFileName, nFileLength))
      return false;

    propToSet->SetValue(nFileLength);
  }
  else if (propToSet->GetPropertyFunction() == tPF_SetFileMD5)
  {
    xtstring sFullPathFileName = propData->GetValue();

    xtstring sMD5;
    if (!CUtils::GetFileMD5(sFullPathFileName, sMD5))
      return false;

    propToSet->SetValue(sMD5);
  }
  else
    return false;


  return true;
}

bool CPropStonePropertiesInd::SetPropertyGroupType(TXPubProperty nGroupType,
                                                   bool bCopyDefaultValues)
{
  if (nGroupType % GROUPSTEP != 0 || nGroupType >= tXPP_Count)
    return false;

  if (m_nGroupType == nGroupType)
    // keine Aenderung
    return true;

  // zuerst alles loeschen
  RemoveAllProperties();

  XPPropertyDef* pPropDef = 0;
  // zuerst werden wir die Gruppe suchen
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      // nichts mehr da - es fehlt was in m_sPropGroups
      return false;
    if (m_sPropGroups[nI].nPropertyGroupType == nGroupType)
    {
      pPropDef = m_sPropGroups[nI].pProperties;
      break;
    }
  }

  m_nGroupType = nGroupType;
  if (!pPropDef)
    // normallerweise in freie Gruppe ist keine Eigenschaft definiert
    return true;

  for (int nI = 0; pPropDef[nI].pPropName != 0; nI++)
  {
    CPropStonePropertyInd cProp(GetPropertyGroupType());
    if (bCopyDefaultValues)
      cProp.GetExpression().SetExprText((pPropDef[nI].pPropDefValue ? pPropDef[nI].pPropDefValue : _XT("")));
    else
      cProp.GetExpression().SetExprText(_XT(""));
    cProp.GetExpression().SetExprValues(_XT(""));
    cProp.GetExpression().SetExprSymbols(_XT(""));
    cProp.m_sInternalName = pPropDef[nI].pPropName;
    cProp.m_nProperty = pPropDef[nI].nProperty;
    cProp.m_nPropertyType = pPropDef[nI].nPropertyType;
    cProp.m_nPropertyFunction = pPropDef[nI].nPropertyFunction;
    cProp.m_xValue.Clear();
    AddPropertyIntern(&cProp);
  }

  return true;
}







bool CPropStonePropertiesInd::GetPropertyValue(CXPubVariant& cValue,
                                               const xtstring& sPropertyName)
{
  CPropStonePropertyInd* pProp = GetProperty(sPropertyName);
  if (!pProp)
    return false;
  cValue = *pProp->GetValuePtr();
  return true;
}
bool CPropStonePropertiesInd::GetPropertyValue(CXPubVariant& cValue,
                                               TXPubProperty nProperty)
{
  CPropStonePropertyInd* pProp = GetProperty(nProperty);
  if (!pProp)
    return false;
  cValue = *pProp->GetValuePtr();
  return true;
}
bool CPropStonePropertiesInd::GetPropertyValue(CXPubVariant& cValue,
                                               size_t nIndex)
{
  CPropStonePropertyInd* pProp = GetProperty(nIndex);
  if (!pProp)
    return false;
  cValue = *pProp->GetValuePtr();
  return true;
  return true;
}
bool CPropStonePropertiesInd::SetPropertyValue(CXPubVariant& cValue,
                                               const xtstring& sPropertyName)
{
  CPropStonePropertyInd* pProp = GetProperty(sPropertyName);
  if (!pProp)
    return false;
  pProp->SetValue(cValue);
  return true;
}
bool CPropStonePropertiesInd::SetPropertyValue(CXPubVariant& cValue,
                                               TXPubProperty nProperty)
{
  CPropStonePropertyInd* pProp = GetProperty(nProperty);
  if (!pProp)
    return false;
  pProp->SetValue(cValue);
  return true;
}
bool CPropStonePropertiesInd::SetPropertyValue(CXPubVariant& cValue,
                                               size_t nIndex)
{
  CPropStonePropertyInd* pProp = GetProperty(nIndex);
  if (!pProp)
    return false;
  pProp->SetValue(cValue);
  return true;
}












CPropStonePropertyInd* CPropStonePropertiesInd::GetProperty(size_t nIndex)
{
  assert(nIndex < m_properties.size());
  if (nIndex < m_properties.size())
    return m_properties[nIndex];
  return 0;
}
CPropStonePropertyInd* CPropStonePropertiesInd::GetProperty(const xtstring& sName)
{
  for (CPropertiesIterator it = m_properties.begin(); it != m_properties.end(); it++)
    if ((*it)->GetInternalName() == sName)
      return (*it);
  return 0;
}
CPropStonePropertyInd* CPropStonePropertiesInd::GetProperty(TXPubProperty nProperty)
{
  for (CPropertiesIterator it = m_properties.begin(); it != m_properties.end(); it++)
    if ((*it)->GetProperty() == nProperty)
      return (*it);
  return 0;
}
bool CPropStonePropertiesInd::AddProperty(CPropStonePropertyInd *pProperty)
{
  assert(pProperty);
  if (!pProperty)
    return false;

  CPropStonePropertyInd *pProp;
  pProp = new CPropStonePropertyInd(GetPropertyGroupType());
  if (!pProp)
    return false;
  pProp->SetParent(this);
  pProp->SetPropStonePropertyInd(pProperty);
  m_properties.push_back(pProp);
  m_bPropertiesChanged = true;
  return true;
}
bool CPropStonePropertiesInd::AddPropertyIntern(CPropStonePropertyInd *pProperty)
{
  assert(pProperty);
  if (!pProperty)
    return false;

  CPropStonePropertyInd *pProp;
  pProp = new CPropStonePropertyInd(GetPropertyGroupType());
  if (!pProp)
    return false;
  pProp->SetParent(this);
  pProp->SetPropStonePropertyInd(pProperty);
  m_properties.push_back(pProp);
  return true;
}
bool CPropStonePropertiesInd::AssignPropertyIntern(CPropStonePropertyInd *pProperty)
{
  assert(pProperty);
  if (!pProperty)
    return false;

  CPropStonePropertyInd* pProp = GetProperty(pProperty->m_sInternalName);
  if (pProp)
  {
    pProp->SetPropStonePropertyInd(pProperty);
    return true;
  }
  if (GetPropertyGroupType() == tXPP_FreeProps)
    return AddPropertyIntern(pProperty);
  return true;
}
bool CPropStonePropertiesInd::RemoveProperty(size_t nIndex)
{
  assert(GetPropertyGroupType() == tXPP_FreeProps);
  if (GetPropertyGroupType() != tXPP_FreeProps)
    return false;

  assert(nIndex < m_properties.size());
  if (nIndex < m_properties.size())
  {
    delete (m_properties[nIndex]);
    m_properties.erase(m_properties.begin() + nIndex);
    m_bPropertiesChanged = true;
    return true;
  }
  return false;
}
bool CPropStonePropertiesInd::RemoveProperty(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    if (sName == (GetProperty(nI))->GetInternalName())
      return RemoveProperty(nI);
  }
  return false;
}
bool CPropStonePropertiesInd::RemoveAllProperties(bool bOnlyRemovableProps/* = false*/)
{
  if (bOnlyRemovableProps)
  {
    bool bRemoved = true;
    while (bRemoved)
    {
      bRemoved = false;
      for (CPropertiesIterator it = m_properties.begin(); it != m_properties.end(); it++)
      {
        CPropStonePropertyInd* prop = *it;
        if (prop->GetPropertyType() == tPT_NotRemovable)
          continue;
        m_properties.erase(it);
        delete prop;
        bRemoved = true;
        break;
      }
    }
  }
  else
  {
    for (CPropertiesIterator it = m_properties.begin(); it != m_properties.end(); it++)
      delete (*it);
    m_properties.erase(m_properties.begin(), m_properties.end());
  }
  return true;
}































//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CPropStoneInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
xtstring CPropStoneInd::GetNameOfPropertyNameInFreeProps()
{
  assert(sizeof(m_sFreeProp) > 1);
  if (sizeof(m_sFreeProp) <= 1)
    return _XT("");
  return m_sFreeProp[0].pPropName;
}

CPropStoneInd::CPropStoneInd(CSyncWPNotifiersGroup* pGroup)
              :CModelBaseInd(tPropStone),
               CSyncNotifier_WP(tWPNotifier_PropStone, pGroup),
               m_properties(pGroup)
{
  m_pMPStone = 0;
  m_bPropStoneChanged = false;

  m_properties.SetPropStoneParent(this);
}

CPropStoneInd::CPropStoneInd(CPropStoneInd& cStone)
              :CModelBaseInd(tPropStone),
               CSyncNotifier_WP(tWPNotifier_PropStone, NULL),
               m_properties((CSyncWPNotifiersGroup*)0)
{
  m_pMPStone = 0;
  m_properties.SetSyncGroup(cStone.GetSyncGroup());
  SetSyncGroup(cStone.GetSyncGroup());
  SetPropStoneInd(&cStone);
}

CPropStoneInd::CPropStoneInd(CPropStoneInd* pStone)
              :CModelBaseInd(tPropStone),
               CSyncNotifier_WP(tWPNotifier_PropStone, NULL),
               m_properties((CSyncWPNotifiersGroup*)0)
{
  m_pMPStone = 0;
  assert(pStone);
  m_properties.SetSyncGroup(pStone->GetSyncGroup());
  SetSyncGroup(pStone->GetSyncGroup());
  SetPropStoneInd(pStone);
}

CPropStoneInd::~CPropStoneInd()
{
}

void CPropStoneInd::SetPropStoneInd(CPropStoneInd* pStone)
{
  if (!pStone)
    return;

  m_bPropStoneChanged = true;
  m_properties.SetPropStonePropertiesInd(&(pStone->m_properties));
}

bool CPropStoneInd::GetChanged()
{
  if (m_bPropStoneChanged)
    return true;
  return m_properties.GetChanged();
}

bool CPropStoneInd::SaveXMLContent(xtstring& sXMLDocText)
{
  bool bRet = false;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(EMPTY_PROPSTONE_XML);
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_PROPSTONE))
  {
    if (cXMLDoc.AddChildElem(PROPSTONE_ELEM_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // in <PropStone>
      if (cXMLDoc.AddChildElem(PROPSTONE_PROPERTIES_ELEM_NAME, _XT("")))
      {
        cXMLDoc.IntoElem();
        // in <Properties>

        m_properties.SaveXMLContent(&cXMLDoc);

        cXMLDoc.OutOfElem();
        // in <Node>
      }

      cXMLDoc.OutOfElem();
      bRet = true;
      m_bPropStoneChanged = false;
    }
  }
  sXMLDocText = cXMLDoc.GetDoc();
  return bRet;
}

bool CPropStoneInd::ReadXMLContent(const xtstring& sXMLDocText)
{
  bool bRet = true;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLDocText.c_str());
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_PROPSTONE))
  {
    if (cXMLDoc.FindChildElem(PROPSTONE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <PropStone>
      // load fields
      if (cXMLDoc.FindChildElem(PROPSTONE_PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>

        m_properties.ReadXMLContent(&cXMLDoc);

        cXMLDoc.OutOfElem();
        // in <Node>
      }

      cXMLDoc.OutOfElem();
      bRet = true;
      m_bPropStoneChanged = false;
    }
  }
  return bRet;
}

bool CPropStoneInd::SetPropertyGroupType(TXPubProperty nGroupType)
{
  return m_properties.SetPropertyGroupType(nGroupType, true);
}

size_t CPropStoneInd::CountOfDefaultGroups() const
{
  size_t nCount = 0;
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    nCount++;
  }
  return nCount;
}
TPropertyGroupMainType CPropStoneInd::GetPropertyGroupMainType(TXPubProperty nGroupType)
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (m_sPropGroups[nI].nPropertyGroupType == nGroupType)
      return m_sPropGroups[nI].nPropertyGroupMainType;
  }
  return tPGMT_GroupNotValid;
}
TXPubProperty CPropStoneInd::GetPropertyGroupType(size_t nIndex)
{
  for (size_t nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (nIndex == nI)
      return m_sPropGroups[nI].nPropertyGroupType;
  }
  return tXPP_FreeProps;
}
xtstring CPropStoneInd::GetDefaultTextForPropertyGroup(TXPubProperty nGroupType)
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (m_sPropGroups[nI].nPropertyGroupType == nGroupType)
      return m_sPropGroups[nI].pPropGroupName;
  }
  return _XT("");
}

TXPubProperty CPropStoneInd::GetFirstGroupFromGroupMainType(TPropertyGroupMainType nGroupMainType)
{
  for (size_t nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (m_sPropGroups[nI].nPropertyGroupMainType == nGroupMainType)
      return m_sPropGroups[nI].nPropertyGroupType;
  }
  return tXPP_FreeProps;
}





























//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CPropStoneContainer
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CPropStoneContainer::CPropStoneContainer()
{
}
CPropStoneContainer::~CPropStoneContainer()
{
  RemoveAllPropertyGroups();
  RemoveAllGarbageGroups();
}
bool CPropStoneContainer::SaveXMLContent(xtstring& sXMLDocText)
{
  bool bRet = true;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(EMPTY_PROPSTONES_XML);
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_PROPSTONES))
  {
    for (CPropertyGroupsIterator it = m_cPropertyGroups.begin(); it != m_cPropertyGroups.end(); it++)
    {
      if (cXMLDoc.AddChildElem(PROPSTONE_ELEM_NAME, _XT("")))
      {
        cXMLDoc.IntoElem();
        // in <PropStone>
        if (cXMLDoc.AddChildElem(PROPSTONE_PROPERTIES_ELEM_NAME, _XT("")))
        {
          cXMLDoc.IntoElem();
          // in <Properties>

          (*it)->SaveXMLContent(&cXMLDoc);

          cXMLDoc.OutOfElem();
          // in <PropStone>
        }

        cXMLDoc.OutOfElem();
      }
    }
  }
  sXMLDocText = cXMLDoc.GetDoc();
  return bRet;
}
bool CPropStoneContainer::ReadXMLContent(const xtstring& sXMLDocText)
{
  bool bRet = true;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLDocText.c_str());
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_PROPSTONES))
  {
    while (cXMLDoc.FindChildElem(PROPSTONE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <PropStone>
      // load fields
      if (cXMLDoc.FindChildElem(PROPSTONE_PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>

        CPropStonePropertiesInd properties((CSyncWPNotifiersGroup*)0);
        properties.ReadXMLContent(&cXMLDoc);
        AddPropertyGroup(&properties);

        cXMLDoc.OutOfElem();
        // in <Node>
      }

      cXMLDoc.OutOfElem();
    }
  }
  return bRet;
}
CPropStonePropertiesInd* CPropStoneContainer::GetPropertyGroup(size_t nIndex)
{
  assert(nIndex < m_cPropertyGroups.size());
  if (nIndex < m_cPropertyGroups.size())
    return m_cPropertyGroups[nIndex];
  return 0;
}
bool CPropStoneContainer::AddPropertyGroup(CPropStonePropertiesInd* pProperties)
{
  assert(pProperties);
  if (!pProperties)
    return false;

  CPropStonePropertiesInd *pProps;
  pProps = new CPropStonePropertiesInd(pProperties);
  if (!pProps)
    return false;
  m_cPropertyGroups.push_back(pProps);
  return true;
}
bool CPropStoneContainer::MovePropertyGroupUp(size_t nIndex)
{
  if (nIndex == 0)
    return true;
  if (nIndex >= m_cPropertyGroups.size())
    return false;

  CPropertyGroupsIterator it1 = m_cPropertyGroups.begin() + nIndex;
  CPropertyGroupsIterator it2 = m_cPropertyGroups.begin() + (nIndex - 1);
  CPropStonePropertiesInd* props = *it1;
  *it1 = *it2;
  *it2 = props;
  return true;
}
bool CPropStoneContainer::MovePropertyGroupDown(size_t nIndex)
{
  if (nIndex >= m_cPropertyGroups.size())
    return false;
  if (nIndex + 1 == m_cPropertyGroups.size())
    return true;

  CPropertyGroupsIterator it1 = m_cPropertyGroups.begin() + nIndex;
  CPropertyGroupsIterator it2 = m_cPropertyGroups.begin() + (nIndex + 1);
  CPropStonePropertiesInd* props = *it1;
  *it1 = *it2;
  *it2 = props;
  return true;
}
bool CPropStoneContainer::RemovePropertyGroup(size_t nIndex)
{
  assert(nIndex < m_cPropertyGroups.size());
  if (nIndex < m_cPropertyGroups.size())
  {
    delete (m_cPropertyGroups[nIndex]);
    m_cPropertyGroups.erase(m_cPropertyGroups.begin() + nIndex);
    return true;
  }
  return false;
}
bool CPropStoneContainer::RemoveAllPropertyGroups()
{
  for (CPropertyGroupsIterator it = m_cPropertyGroups.begin(); it != m_cPropertyGroups.end(); it++)
    delete (*it);
  m_cPropertyGroups.erase(m_cPropertyGroups.begin(), m_cPropertyGroups.end());
  return true;
}

bool CPropStoneContainer::RemoveAllGarbageGroups()
{
  for (CPropertyGroupsIterator it = m_cGarbageGroups.begin(); it != m_cGarbageGroups.end(); it++)
    delete (*it);
  m_cGarbageGroups.erase(m_cGarbageGroups.begin(), m_cGarbageGroups.end());
  return true;
}

bool CPropStoneContainer::GetAllPropGroupNames(CxtstringVector& arrPropGroupNames)
{
  arrPropGroupNames.erase(arrPropGroupNames.begin(), arrPropGroupNames.end());
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    arrPropGroupNames.push_back(m_sPropGroups[nI].pPropGroupName);
  }
  return true;
}

xtstring CPropStoneContainer::PropGroupNameFromIndex(int nPropGroupIndex)
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (nPropGroupIndex == nI)
      return m_sPropGroups[nI].pPropGroupName;
  }
  return _XT("");
}

int CPropStoneContainer::IndexFromPropGroupName(const xtstring& sPropGroupName)
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (sPropGroupName == m_sPropGroups[nI].pPropGroupName)
      return nI;
  }
  return -1;
}

bool CPropStoneContainer::PropGroupExist(int nPropGroupIndex)
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (nPropGroupIndex == nI)
      return true;
  }
  return false;
}

xtstring CPropStoneContainer::ConvertPropNameToNameForEvaluate(const xtstring& sPropName)
{
#define DEFAULT_RESTRICT_CHARS    _XT(" /\\\a\n\r\t\"'`~!?@#$%^&*-+=()|{}[]<>:;,.")

  xtstring sRet;
  sRet = sPropName;
  size_t nPos;
  nPos = sRet.find(_XT("<"));
  if (nPos != xtstring::npos && nPos != 0)
    sRet.erase(sRet.begin() + nPos, sRet.end());
  while ((nPos = sRet.find_first_of(DEFAULT_RESTRICT_CHARS)) != xtstring::npos)
    sRet.erase(sRet.begin() + nPos, sRet.begin() + nPos + 1);
  return sRet;
}

bool CPropStoneContainer::GetAllPropsFromPropGroupName(const xtstring& sPropGroupName,
                                                       CxtstringVector& arrPropNames)
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (sPropGroupName == m_sPropGroups[nI].pPropGroupName)
    {
      for (int nJ = 0; ; nJ++)
      {
        if (m_sPropGroups[nI].pProperties[nJ].pPropName == 0)
          break;
        arrPropNames.push_back(ConvertPropNameToNameForEvaluate(m_sPropGroups[nI].pProperties[nJ].pPropName));
      }
      return true;
    }
  }
  return false;
}

TXPubProperty CPropStoneContainer::GetPropertyGroupTypeFromPropertyGroupName(const xtstring& sPropGroupName)
{
  for (int nI = 0; ; nI++)
  {
    if (m_sPropGroups[nI].pPropGroupName == 0)
      break;
    if (sPropGroupName == m_sPropGroups[nI].pPropGroupName)
      return m_sPropGroups[nI].nPropertyGroupType;
  }
  return tXPP_FreeProps;
}

void* CPropStoneContainer::GetNextGroup(TXPubProperty nPropGroupType,
                                        void* pStartForNext)
{
  if (pStartForNext)
  {
    CPropStonePropertiesInd* pProp = 0;
    switch (nPropGroupType)
    {
    case tXPP_FreeProps:
      pProp = (CPropStonePropertiesInd*)pStartForNext;
      break;
    case tXPP_ExcelProps:
      pProp = ((CPropStoneExcelProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_RTFProps:
      pProp = ((CPropStoneRTFProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_PDFProps:
      pProp = ((CPropStonePDFProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_UNZIPProps:
      pProp = ((CPropStoneUNZIPProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_ZIPProps:
      pProp = ((CPropStoneZIPProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_FTPTransferProps:
      pProp = ((CPropStoneFTPTransferProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_CMDProps:
      pProp = ((CPropStoneCMDProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_SMTPProps:
      pProp = ((CPropStoneSMTPProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_OutlookProps:
      pProp = ((CPropStoneOutlookProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_EncodingProps:
      pProp = ((CPropStoneEncodingProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_EncryptionProps:
      pProp = ((CPropStoneEncryptionProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_FileMoveProps:
      pProp = ((CPropStoneFileMoveProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_FileCopyProps:
      pProp = ((CPropStoneFileCopyProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_FileRemoveProps:
      pProp = ((CPropStoneFileRemoveProps*)pStartForNext)->m_pProperties;
      break;
    case tXPP_IndesignNewDocProps:
      pProp = ((CPropStoneIndesignNewDocProps*)pStartForNext)->m_pProperties;
      break;
// XPubWizard --> CaseBlockForFunctionGetNextGroup
    }
    bool bFirstPhase = true;
    for (CPropertyGroupsIterator it = m_cPropertyGroups.begin(); it != m_cPropertyGroups.end(); it++)
    {
      if ((*it)->GetPropertyGroupType() == nPropGroupType)
      {
        if (bFirstPhase)
        {
          if (pProp == (*it))
            bFirstPhase = false;
        }
        else
        {
          return ReturnGarbageGroup(nPropGroupType, (*it));
        }
      }
    }
  }
  else
  {
    for (CPropertyGroupsIterator it = m_cPropertyGroups.begin(); it != m_cPropertyGroups.end(); it++)
    {
      if ((*it)->GetPropertyGroupType() == nPropGroupType)
      {
        return ReturnGarbageGroup(nPropGroupType, (*it));
      }
    }
  }
  return 0;
}

void* CPropStoneContainer::ReturnGarbageGroup(TXPubProperty nPropGroupType,
                                              CPropStonePropertiesInd* pPropGroup)
{
  void* pRet = 0;
  switch (nPropGroupType)
  {
  case tXPP_FreeProps:
    {
      CPropStonePropertiesInd* p = new CPropStonePropertiesInd((CSyncWPNotifiersGroup*)0);
      *p = *pPropGroup;
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_ExcelProps:
    {
      CPropStoneExcelProps* p = new CPropStoneExcelProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_RTFProps:
    {
      CPropStoneRTFProps* p = new CPropStoneRTFProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_PDFProps:
    {
      CPropStonePDFProps* p = new CPropStonePDFProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_UNZIPProps:
    {
      CPropStoneUNZIPProps* p = new CPropStoneUNZIPProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_ZIPProps:
    {
      CPropStoneZIPProps* p = new CPropStoneZIPProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_FTPTransferProps:
    {
      CPropStoneFTPTransferProps* p = new CPropStoneFTPTransferProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_CMDProps:
    {
      CPropStoneCMDProps* p = new CPropStoneCMDProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_SMTPProps:
    {
      CPropStoneSMTPProps* p = new CPropStoneSMTPProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_OutlookProps:
    {
      CPropStoneOutlookProps* p = new CPropStoneOutlookProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_EncodingProps:
    {
      CPropStoneEncodingProps* p = new CPropStoneEncodingProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_EncryptionProps:
    {
      CPropStoneEncryptionProps* p = new CPropStoneEncryptionProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_FileMoveProps:
    {
      CPropStoneFileMoveProps* p = new CPropStoneFileMoveProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_FileCopyProps:
    {
      CPropStoneFileCopyProps* p = new CPropStoneFileCopyProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_FileRemoveProps:
    {
      CPropStoneFileRemoveProps* p = new CPropStoneFileRemoveProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
  case tXPP_IndesignNewDocProps:
    {
      CPropStoneIndesignNewDocProps* p = new CPropStoneIndesignNewDocProps(pPropGroup);
      pRet = p;
      m_cGarbageGroups.push_back(p);
    }
    break;
// XPubWizard --> CaseBlockForFunctionReturnGarbageGroup
  }
  return pRet;
}
