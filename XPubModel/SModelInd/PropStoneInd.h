// PropStoneInd.h: interface for the CPropStoneInd classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PROPSTONEIND_H_)
#define _PROPSTONEIND_H_


#include "ModelExpImp.h"
#include "XPubVariant.h"

#include "SSyncEng/SyncEngineBase.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32




class CXPubMarkUp;
class CPropStoneInd;
class CPropStonePropertiesInd;
class CMPModelStoneInd;


// XPubWizard --> PropGroupStep
#define GROUPSTEP   110

// erste property im Block ist property group type
// letzte property im Block ist spezielle Property - end
// die freie Property Gruppe muss immer an erste Stelle bleiben
// in Funktionen ist damit gerechnet
typedef enum TXPubProperty
{
  tXPP_EmptyProperty = -1,

  // XPubWizard --> PropGroupBlock
  tXPP_FreeProps = 0 * GROUPSTEP,               // freie Eigenschaften
  tXPP_FreeProps_Name,
  tXPP_FreeProps_Value,
  tXPP_FreeProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_ExcelProps = 1 * GROUPSTEP,              // vordefiniert - Excel Eigenschaften
  tXPP_EXCP_GroupProperties,
  tXPP_EXCP_Title,
  tXPP_EXCP_Theme,
  tXPP_EXCP_Author,
  tXPP_EXCP_Manager,
  tXPP_EXCP_Firma,
  tXPP_EXCP_Category,
  tXPP_EXCP_Keywords,
  tXPP_EXCP_Comment,
  tXPP_EXCP_GroupSecurity,
  tXPP_EXCP_PasswordForOpen,
  tXPP_EXCP_PasswordForEdit,
  tXPP_EXCP_Count,
  tXPP_ExcelProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_RTFProps = 2 * GROUPSTEP,                // vordefiniert - RTF Eigenschaften
  tXPP_RTFP_GroupProperties,
  tXPP_RTFP_Title,
  tXPP_RTFP_Theme,
  tXPP_RTFP_Author,
  tXPP_RTFP_Manager,
  tXPP_RTFP_Firma,
  tXPP_RTFP_Category,
  tXPP_RTFP_Keywords,
  tXPP_RTFP_Comment,
  tXPP_RTFP_GroupSecurity,
  tXPP_RTFP_PasswordForOpen,
  tXPP_RTFP_PasswordForEdit,
  tXPP_RTFP_Count,
  tXPP_RTFProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_PDFProps = 3 * GROUPSTEP,                // vordefiniert - PDF Eigenschaften
  tXPP_PDFP_DocSummary,
  tXPP_PDFP_Title,
  tXPP_PDFP_Subject,
  tXPP_PDFP_Author,
  tXPP_PDFP_Keywords,
  tXPP_PDFP_Creator,
  tXPP_PDFP_Producer,
  tXPP_PDFP_GroupSecurity,
  tXPP_PDFP_UserPassword,
  tXPP_PDFP_MasterPassword,
  tXPP_PDFP_Print,
  tXPP_PDFP_Edit,
  tXPP_PDFP_Copy,
  tXPP_PDFP_Commenting,
  tXPP_PDFP_LowPrinting,
  tXPP_PDFP_Assemble,
  tXPP_PDFP_FormInput,
  tXPP_PDFP_ContentExtraction,
  tXPP_PDFP_Count,
  tXPP_PDFProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_UNZIPProps = 4 * GROUPSTEP,              // Unzip                                  pre / post
  tXPP_UNZIP_Archivename,   // der Archivename incl. Pfad
  tXPP_UNZIP_Files,         // die zu entpackenden Dateien aus Archiv
  tXPP_UNZIP_Destination,   // das Zielverzeichnis der zu entpackenden Dateien
  tXPP_UNZIP_Password,      // Passwort
  tXPP_UNZIP_Count,
  tXPP_UNZIPProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_ZIPProps = 5 * GROUPSTEP,                // Zip                                    pre / post
  tXPP_ZIP_Archivename,     // der Archivename incl. Pfad
  tXPP_ZIP_AddFiles,        // die hinzuzufügenden Dateien z.B. "e:\Dateien\*.doc"
  tXPP_ZIP_SubDirs,         // Unterverzeichnisse mit einschließen
  tXPP_ZIP_StorePath,       // Pfade in Archiv speichern
  tXPP_ZIP_Password,        // Passwort
  tXPP_ZIP_Count,
  tXPP_ZIPProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_FTPTransferProps = 6 * GROUPSTEP,        // FTP-Transfer                           pre / post
  tXPP_FTP_Server,          // Server. ftp.x-publisher.com
  tXPP_FTP_Port,            // Port
  tXPP_FTP_RemoteStartDir,
  tXPP_FTP_RemoteFile,      // Dateiname auf Server
  tXPP_FTP_LocalFile,       // lokaler Dateiname mit Pfad
  tXPP_FTP_Download,        // TRUE und FALSE für Upload
  tXPP_FTP_Username,        // Benutzername
  tXPP_FTP_Password,        // Passwort
  tXPP_FTP_TimeOut,
  tXPP_FTP_Transfermode,
  tXPP_FTP_Count,
  tXPP_FTPTransferProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_CMDProps = 7 * GROUPSTEP,                // Befehlsdatei oder Programm             pre / post
  tXPP_CMD_CmdFile,         // Name der Kommandozeilendatei
  tXPP_CMD_CurDir,          //
  tXPP_CMD_CmdLine,         //
  tXPP_CMD_Count,
  tXPP_CMDProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_SMTPProps = 8 * GROUPSTEP,               // Mail an SMTP                           pre / post
  tXPP_SMTP_SMTPServer,     // z.B. "smtp.1und1.com"
  tXPP_SMTP_SMTPPort,       // z.B. "25"
  tXPP_SMTP_From,           // Absender
  tXPP_SMTP_To,             // Empfänger
  tXPP_SMTP_Subject,        // Betreff
  tXPP_SMTP_Filename,       // Messagetextfile
  tXPP_SMTP_User,           // Benutzer
  tXPP_SMTP_Password,       // Passwort
  tXPP_SMTP_Count,
  tXPP_SMTPProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_OutlookProps = 9 * GROUPSTEP,            // outlook mail
  tXPP_OUTL_From,
  tXPP_OUTL_To,
  tXPP_OUTL_Subject,
  tXPP_OUTL_Count,
  tXPP_OutlookProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_EncodingProps = 10 * GROUPSTEP,          // encoding                               pre / post
  tXPP_ENCO_FileFrom,
  tXPP_ENCO_FileTo,
  tXPP_ENCO_Mode,
  tXPP_ENCO_Count,
  tXPP_EncodingProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_EncryptionProps = 11 * GROUPSTEP,        // encryption                             pre / post
  tXPP_ENCR_FileFrom,
  tXPP_ENCR_FileTo,
  tXPP_ENCR_Key,
  tXPP_ENCR_Count,
  tXPP_EncryptionProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_FileMoveProps = 12 * GROUPSTEP,          // datei verschieben                      pre / post
  tXPP_FMOV_FileFrom,
  tXPP_FMOV_FileTo,
  tXPP_FMOV_Count,
  tXPP_FileMoveProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_FileCopyProps = 13 * GROUPSTEP,          // datei kopieren                         pre / post
  tXPP_FCOP_FileFrom,
  tXPP_FCOP_FileTo,
  tXPP_FCOP_Count,
  tXPP_FileCopyProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_FileRemoveProps = 14 * GROUPSTEP,        // datei verschieben                      pre / post
  tXPP_FREM_File,
  tXPP_FREM_Count,
  tXPP_FileRemoveProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_IndesignNewDocProps = 15 * GROUPSTEP,    // Indesign Neues Dokument 
  tXPP_INDD_DocumentPreset,
  tXPP_INDD_NumberOfPages,
  tXPP_INDD_FacingPages,
  tXPP_INDD_MasterTextFrame,
  // Page 
  tXPP_INDD_Page,
  tXPP_INDD_PageSize,
  tXPP_INDD_Width,
  tXPP_INDD_Height,
  tXPP_INDD_Orientation,
  // Columns
  tXPP_INDD_Columns,
  tXPP_INDD_ColumnsNumber,
  tXPP_INDD_ColumnsGutter,
  // Margins
  tXPP_INDD_Margins,
  tXPP_INDD_MarginsTop,
  tXPP_INDD_MarginsBottom,
  tXPP_INDD_MarginsInside,
  tXPP_INDD_MarginsOutside,
  // Bleed & Slug
  tXPP_INDD_BleedSlugs,
  tXPP_INDD_BleedTop,
  tXPP_INDD_SlugTop,
  tXPP_INDD_BleedBottom,
  tXPP_INDD_SlugBottom,
  tXPP_INDD_BleedInside,
  tXPP_INDD_SlugInside,
  tXPP_INDD_BleedOutside,
  tXPP_INDD_SlugOutside,

  tXPP_INDD_Count,
  tXPP_IndesignNewDocProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_Base64TransferProps = 16 * GROUPSTEP,    // transfer binary file
  tXPP_B64T_VisibleName,
  tXPP_B64T_DestinateFileName,
  tXPP_B64T_ReferenceFileName,
  tXPP_B64T_FileContent,
  tXPP_B64T_FileDatum,
  tXPP_B64T_FileLength,
  tXPP_B64T_ShowInClient,
  tXPP_B64T_Count,
  tXPP_Base64TransferProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_Base64TransferInfoProps = 17 * GROUPSTEP,    // transfer binary file
  tXPP_B64TI_VisibleName,
  tXPP_B64TI_DestinateFileName,
  tXPP_B64TI_ReferenceFileName,
  tXPP_B64TI_FileMD5,
  tXPP_B64TI_FileDatum,
  tXPP_B64TI_FileLength,
  tXPP_B64TI_Count,
  tXPP_Base64TransferInfoProps_EndOfGroup,

  // XPubWizard --> PropGroupBlock
  tXPP_Count = 18 * GROUPSTEP,
};

typedef enum TPropertyGroupMainType
{
  tPGMT_GroupNotValid = 0,
  tPGMT_OrdinaryGroup = 1,
  tPGMT_PrePostGroup,
  tPGMT_Last,
};

typedef enum TPropertyType
{
  tPT_Normal = 0,                     // normale Eigenschaft
  tPT_Group = 1,                      // Gruppe der Eigenschaften, alle Eigenschaften, die in Definition folgen, gehoeren zu dieser Gruppe
  tPT_NotEditable = 2,                // normale Eigenschaft, aber nicht editierbar
  tPT_NotRemovable = 3,               // normale Eigenschaft, aber nicht loeschbar
  tPT_DontShow = 4,                   // diese Eigenschaft gar nicht darstellen
  tPT_SetOverFunction_DontShow = 5,   // Wert ueber Funktion in Ausfuehrungszeit bestimmen.
                                      // Was genau in diesem Eigenschaftstypen passieren muss,
                                      // definiert TPropertyFunction.
                                      // Wenn eine Eigenschaft von Typ 'SetOverFunction' ist, dann muss man
                                      // fuer die Funktion vorherige Eigenschaft suchen,
                                      // die vom Typ 'tPT_Normal' ist und dort findet man notwendige Informationen.
};

typedef enum TPropertyFunction
{
  tPF_NoFunction = 0,     // nichts machen
  tPF_SetFileContent = 1, // Inhalt der Datei in dieser Eigenschaft setzen. 
                          // Der Dateiname ist in vorherige Eigenschaft definiert, die vom Typ 'tPT_Normal' ist.
  tPF_SetFileDatum = 2,   // Datum der Datei in dieser Eigenschaft setzen.
                          // Der Dateiname ist in vorherige Eigenschaft definiert, die vom Typ 'tPT_Normal' ist.
  tPF_SetFileLength = 3,  // Laenge der Datei in dieser Eigenschaft setzen.
                          // Der Dateiname ist in vorherige Eigenschaft definiert, die vom Typ 'tPT_Normal' ist.
  tPF_SetFileMD5 = 4,     // MD5 der Datei bereichnen und in dieser Eigenschaft setzen.
                          // Der Dateiname ist in vorherige Eigenschaft definiert, die vom Typ 'tPT_Normal' ist.
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// old group numbers -->
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// die freie Property Gruppe muss immer an erste Stelle bleiben
// in Funktionen ist damit gerechnet
typedef enum TPropertyGroupType_Old
{
  t_PGT_FreeProps = 0,     // freie Eigenschaften
  t_PGT_ExcelProps,        // vordefiniert - Excel Eigenschaften
  t_PGT_RTFProps,          // vordefiniert - RTF Eigenschaften
  t_PGT_PDFProps,          // vordefiniert - PDF Eigenschaften
  t_PGT_UNZIPProps,        // Unzip                                            pre / post
  t_PGT_ZIPProps,          // Zip                                              pre / post
  t_PGT_FTPTransferProps,  // FTP-Transfer                                     pre / post
  t_PGT_CMDProps,          // Befehlsdatei oder Programm                       pre / post
  t_PGT_SMTPProps,         // Mail an SMTP                                     pre / post
  t_PGT_OutlookProps,      // outlook mail
  t_PGT_EncodingProps,     // encoding                                         pre / post
  t_PGT_EncryptionProps,   // encryption                                       pre / post
  t_PGT_FileMoveProps,     // datei verschieben                                pre / post
  t_PGT_FileCopyProps,     // datei kopieren                                   pre / post
  t_PGT_FileRemoveProps,   // datei verschieben                                pre / post
  t_PGT_IndesignNewDocProps,// Indesign Neues Dokument 
  t_PGT_Count,
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// old group numbers <--
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





















typedef struct _XPPropertyDef
{
  TXPubProperty     nProperty;          // ein von ENUMs
  TPropertyType     nPropertyType;      // Art der Eigenschaft
  TPropertyFunction nPropertyFunction;  // Function for Property
	XTCHAR            *pPropName;         // default Name, bei vordefinierten Gruppen kann in GUI ueberschrieben
	XTCHAR            *pPropDefValue;     // Bedingung - Vorsicht - man muss in GUI dazu auch Values und Symbols erzeugen
} XPPropertyDef;

typedef struct _XPPropertyGroupDef
{
  TPropertyGroupMainType  nPropertyGroupMainType;
  TXPubProperty           nPropertyGroupType;
  XTCHAR                  *pPropGroupName;
  XPPropertyDef           *pProperties;
} XPPropertyGroupDef;







class XPUBMODEL_EXPORTIMPORT CPropStonePropertyInd
{
  friend class CPropStonePropertiesInd;
public:
  CPropStonePropertyInd(TXPubProperty nPropGroup);
  ~CPropStonePropertyInd();

  bool operator == (CPropStonePropertyInd& cProperty);
  void SetPropStonePropertyInd(CPropStonePropertyInd* pProperty);

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  CConditionOrAssignment&         GetExpression(){return m_caExpression;};
  const CConditionOrAssignment&   GetExpression_Const() const {return m_caExpression;};
  CConditionOrAssignment*         GetExpression_Ptr(){return &m_caExpression;};

  void SetExpression(const CConditionOrAssignment& caExpression);

  xtstring GetInternalName(){return m_sInternalName;};
  TXPubProperty GetProperty(){return m_nProperty;};
  TPropertyType GetPropertyType(){return m_nPropertyType;};
  TPropertyFunction GetPropertyFunction(){return m_nPropertyFunction;};
  CXPubVariant GetValue(){return m_xValue;};
  const CXPubVariant* GetValuePtr(){return &m_xValue;};

  void SetInternalName(const xtstring& sInternalName);
  void SetProperty(TXPubProperty nProperty);
  void SetPropertyType(TPropertyType nPropertyType);
  void SetPropertyFunction(TPropertyFunction nPropertyFunction);
  void SetValue(const CXPubVariant& xValue);

protected:
  void SetParent(CPropStonePropertiesInd* pParent){m_pPropertiesParent = pParent;};
protected:
  CPropStonePropertiesInd*  m_pPropertiesParent;
  TXPubProperty             m_nPropGroup; // X * GROUPSTEP

  CConditionOrAssignment  m_caExpression;
  xtstring          m_sInternalName;
  TXPubProperty     m_nProperty;
  TPropertyType     m_nPropertyType;
  TPropertyFunction m_nPropertyFunction;
  CXPubVariant      m_xValue;
};

typedef vector<CPropStonePropertyInd*> CProperties;
typedef CProperties::iterator          CPropertiesIterator;
typedef CProperties::const_iterator    CPropertiesCIterator;
typedef CProperties::reverse_iterator  CPropertiesRIterator;







class XPUBMODEL_EXPORTIMPORT CPropStonePropertiesInd : public CModelBaseInd,
                                                       public CSyncNotifier_WP,
                                                       public CSyncResponse_Proj,
                                                       public CSyncResponse_ER,
                                                       public CSyncResponse_IFS
{
  friend class CPropStoneInd;
public:
  CPropStonePropertiesInd(TXPubProperty nGroupType);
  CPropStonePropertiesInd(CSyncWPNotifiersGroup* pGroup);
  CPropStonePropertiesInd(CPropStonePropertiesInd& cProperties);
  CPropStonePropertiesInd(CPropStonePropertiesInd* pField);
  virtual ~CPropStonePropertiesInd();

  bool operator ==(const CPropStonePropertiesInd& cProperties);
  void operator =(const CPropStonePropertiesInd& cProperties);

  void SetPropStonePropertiesInd(CPropStonePropertiesInd* pProperties);

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  // diese Funktion setzt die Eigenschaften direkt vom FormatWorker,
  // wo CPropStoneInd abgelegt (serialisiert) ist.
  // Wenn diese Funktion aufgerufen ist, in dieser Klasse muss schon PropertyGroupType
  // gesetzt werden und sXMLDocText ist nur dann eingelesen, wenn in XML
  // gleiches PropertyGroupType steht
  bool CheckTypeAndSetProperties(const xtstring& sXMLDocText);

  bool GetPropertyValue(CXPubVariant& cValue, const xtstring& sPropertyName);
  bool GetPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty);
  bool GetPropertyValue(CXPubVariant& cValue, size_t nIndex);
  bool SetPropertyValue(CXPubVariant& cValue, const xtstring& sPropertyName);
  bool SetPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty);
  bool SetPropertyValue(CXPubVariant& cValue, size_t nIndex);

  size_t CountOfProperties() const {return m_properties.size();};
  CPropStonePropertyInd* GetProperty(size_t nIndex);
  CPropStonePropertyInd* GetProperty(const xtstring& sName);
  CPropStonePropertyInd* GetProperty(TXPubProperty nProperty);
  bool AddProperty(CPropStonePropertyInd *pProperty);
  bool RemoveProperty(size_t nIndex);
  bool RemoveProperty(const xtstring& sName);


  bool GetChanged(){return m_bPropertiesChanged;};
  void SetChanged(){m_bPropertiesChanged = true;};

  TPropertyGroupMainType GetPropertyGroupMainType();
  TXPubProperty GetPropertyGroupType(){return m_nGroupType;};
  xtstring GetPropertyGroupTypeName();

  bool Call_SetOverFunction(size_t nIndexOfProperty);

protected:
  bool RemoveAllProperties(bool bOnlyRemovableProps = false);
  bool AddPropertyIntern(CPropStonePropertyInd *pProperty);
  bool AssignPropertyIntern(CPropStonePropertyInd *pProperty);

  void ClearChanged(){m_bPropertiesChanged = false;};
  void SetPropStoneParent(CPropStoneInd* pParent){m_pPropStoneParent = pParent;};

  bool SetPropertyGroupType(TXPubProperty nGroupType, bool bCopyDefaultValues);

  CPropStonePropertyInd* Call_SetOverFunction_GetFirstPreviousNormalProperty(size_t nForThisProperty);
protected:
  CPropStoneInd* m_pPropStoneParent;

  TXPubProperty         m_nGroupType;
  CProperties           m_properties;

  // diese Variable ist wegen SyncEngine definiert
  // wenn sich Stand durch SyncEngine aendert, View merkt es nicht,
  // dass sich da was geaendert hat und muss nachfragen
  bool m_bPropertiesChanged;

protected:
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){m_bPropertiesChanged = true;};
  virtual void ROC_PropertyChanged_ER(){m_bPropertiesChanged = true;};
  virtual void ROC_PropertyChanged_IFS(){m_bPropertiesChanged = true;};

  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObject* pObject);

  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);

  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual xtstring SE_GetModuleName(){return _XT("");};
};

typedef vector<CPropStonePropertiesInd*>  CPropertyGroups;
typedef CPropertyGroups::iterator         CPropertyGroupsIterator;
typedef CPropertyGroups::const_iterator   CPropertyGroupsCIterator;
typedef CPropertyGroups::reverse_iterator CPropertyGroupsRIterator;











class XPUBMODEL_EXPORTIMPORT CPropStoneInd : public CModelBaseInd,
                                             public CSyncNotifier_WP
{
  friend class CPropStonePropertiesInd;
public:
  static xtstring GetNameOfPropertyNameInFreeProps();
public:
  CPropStoneInd(CSyncWPNotifiersGroup* pGroup);
  CPropStoneInd(CPropStoneInd& cStone);
  CPropStoneInd(CPropStoneInd* pStone);
  virtual ~CPropStoneInd();

  void SetPropStoneInd(CPropStoneInd* pStone);

  bool SaveXMLContent(xtstring& sXMLDocText);
  bool ReadXMLContent(const xtstring& sXMLDocText);

  // wenn folgende Funktion aufgerufen ist, dann sind die einzelne Eigenschaften initialisiert
  // zwischen anderen ist auch Default Value nach Expression zugewiesen
  // Nach Aufruf dieser Funktion muss man die ExprValues und ExprSymbols in Ordnung bringen
  TPropertyGroupMainType GetPropertyGroupMainType(){return m_properties.GetPropertyGroupMainType();};
  bool SetPropertyGroupType(TXPubProperty nGroupType);
  TXPubProperty GetPropertyGroupType(){return m_properties.GetPropertyGroupType();};

  size_t CountOfDefaultGroups() const;
  TPropertyGroupMainType GetPropertyGroupMainType(TXPubProperty nGroupType);
  TXPubProperty GetPropertyGroupType(size_t nIndex);
  xtstring GetDefaultTextForPropertyGroup(TXPubProperty nGroupType);

  TXPubProperty GetFirstGroupFromGroupMainType(TPropertyGroupMainType nGroupMainType);

  CPropStonePropertiesInd& GetProperties(){return m_properties;};

  bool GetChanged();
  void ClearChanged(){m_bPropStoneChanged = false; m_properties.ClearChanged();};

  void SetMPStone(CMPModelStoneInd* pMPStone){m_pMPStone = pMPStone;};
private:

  CPropStonePropertiesInd m_properties;

  CMPModelStoneInd* m_pMPStone;

  // diese Variable ist wegen SyncEngine definiert
  // wenn sich Stand durch SyncEngine aendert, View merkt es nicht,
  // dass sich da was geaendert hat und muss nachfragen
  bool m_bPropStoneChanged;

protected:
  virtual xtstring SE_GetModuleName(){return _XT("");};
};






















class XPUBMODEL_EXPORTIMPORT CPropStoneExcelProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneExcelProps():CPropStonePropertiesInd(tXPP_ExcelProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneExcelProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_ExcelProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneExcelProps()
    {};

  bool operator ==(const CPropStoneExcelProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneExcelProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetTitle(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_Title);return GetPropertyValue(cValue, tXPP_EXCP_Title);}
  bool SetTitle(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_Title);return SetPropertyValue(cValue, tXPP_EXCP_Title);}
  bool GetTheme(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_Theme);return GetPropertyValue(cValue, tXPP_EXCP_Title);}
  bool SetTheme(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_Theme);return SetPropertyValue(cValue, tXPP_EXCP_Title);}
  bool GetAuthor(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_Author);return GetPropertyValue(cValue, tXPP_EXCP_Title);}
  bool SetAuthor(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_Author);return SetPropertyValue(cValue, tXPP_EXCP_Title);}
  bool GetManager(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_Manager);return GetPropertyValue(cValue, tXPP_EXCP_Manager);}
  bool SetManager(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_Manager);return SetPropertyValue(cValue, tXPP_EXCP_Manager);}
  bool GetFirma(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_Firma);return GetPropertyValue(cValue, tXPP_EXCP_Firma);}
  bool SetFirma(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_Firma);return SetPropertyValue(cValue, tXPP_EXCP_Firma);}
  bool GetCategory(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_Category);return GetPropertyValue(cValue, tXPP_EXCP_Category);}
  bool SetCategory(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_Category);return SetPropertyValue(cValue, tXPP_EXCP_Category);}
  bool GetKeywords(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_Keywords);return GetPropertyValue(cValue, tXPP_EXCP_Keywords);}
  bool SetKeywords(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_Keywords);return SetPropertyValue(cValue, tXPP_EXCP_Keywords);}
  bool GetComment(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_Comment);return GetPropertyValue(cValue, tXPP_EXCP_Comment);}
  bool SetComment(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_Comment);return SetPropertyValue(cValue, tXPP_EXCP_Comment);}
  bool GetPasswordForOpen(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_PasswordForOpen);return GetPropertyValue(cValue, tXPP_EXCP_PasswordForOpen);}
  bool SetPasswordForOpen(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_PasswordForOpen);return SetPropertyValue(cValue, tXPP_EXCP_PasswordForOpen);}
  bool GetPasswordForEdit(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_EXCP_PasswordForEdit);return GetPropertyValue(cValue, tXPP_EXCP_PasswordForEdit);}
  bool SetPasswordForEdit(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_EXCP_PasswordForEdit);return SetPropertyValue(cValue, tXPP_EXCP_PasswordForEdit);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneRTFProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneRTFProps():CPropStonePropertiesInd(tXPP_RTFProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneRTFProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_RTFProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneRTFProps()
    {};

  bool operator ==(const CPropStoneRTFProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneRTFProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetTitle(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_Title);return GetPropertyValue(cValue, tXPP_RTFP_Title);}
  bool SetTitle(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_Title);return SetPropertyValue(cValue, tXPP_RTFP_Title);}
  bool GetTheme(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_Theme);return GetPropertyValue(cValue, tXPP_RTFP_Theme);}
  bool SetTheme(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_Theme);return SetPropertyValue(cValue, tXPP_RTFP_Theme);}
  bool GetAuthor(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_Author);return GetPropertyValue(cValue, tXPP_RTFP_Author);}
  bool SetAuthor(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_Author);return SetPropertyValue(cValue, tXPP_RTFP_Author);}
  bool GetManager(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_Manager);return GetPropertyValue(cValue, tXPP_RTFP_Manager);}
  bool SetManager(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_Manager);return SetPropertyValue(cValue, tXPP_RTFP_Manager);}
  bool GetFirma(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_Firma);return GetPropertyValue(cValue, tXPP_RTFP_Firma);}
  bool SetFirma(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_Firma);return SetPropertyValue(cValue, tXPP_RTFP_Firma);}
  bool GetCategory(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_Category);return GetPropertyValue(cValue, tXPP_RTFP_Category);}
  bool SetCategory(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_Category);return SetPropertyValue(cValue, tXPP_RTFP_Category);}
  bool GetKeywords(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_Keywords);return GetPropertyValue(cValue, tXPP_RTFP_Keywords);}
  bool SetKeywords(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_Keywords);return SetPropertyValue(cValue, tXPP_RTFP_Keywords);}
  bool GetComment(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_Comment);return GetPropertyValue(cValue, tXPP_RTFP_Comment);}
  bool SetComment(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_Comment);return SetPropertyValue(cValue, tXPP_RTFP_Comment);}
  bool GetPasswordForOpen(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_PasswordForOpen);return GetPropertyValue(cValue, tXPP_RTFP_PasswordForOpen);}
  bool SetPasswordForOpen(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_PasswordForOpen);return SetPropertyValue(cValue, tXPP_RTFP_PasswordForOpen);}
  bool GetPasswordForEdit(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_RTFP_PasswordForEdit);return GetPropertyValue(cValue, tXPP_RTFP_PasswordForEdit);}
  bool SetPasswordForEdit(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_RTFP_PasswordForEdit);return SetPropertyValue(cValue, tXPP_RTFP_PasswordForEdit);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStonePDFProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStonePDFProps():CPropStonePropertiesInd(tXPP_PDFProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStonePDFProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_PDFProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStonePDFProps()
    {};

  bool operator ==(const CPropStonePDFProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStonePDFProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetTitle(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Title);return GetPropertyValue(cValue, tXPP_PDFP_Title);}
  bool SetTitle(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Title);return SetPropertyValue(cValue, tXPP_PDFP_Title);}
  bool GetSubject(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Subject);return GetPropertyValue(cValue, tXPP_PDFP_Subject);}
  bool SetSubject(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Subject);return SetPropertyValue(cValue, tXPP_PDFP_Subject);}
  bool GetAuthor(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Author);return GetPropertyValue(cValue, tXPP_PDFP_Author);}
  bool SetAuthor(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Author);return SetPropertyValue(cValue, tXPP_PDFP_Author);}
  bool GetKeywords(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Keywords);return GetPropertyValue(cValue, tXPP_PDFP_Keywords);}
  bool SetKeywords(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Keywords);return SetPropertyValue(cValue, tXPP_PDFP_Keywords);}
  bool GetCreator(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Creator);return GetPropertyValue(cValue, tXPP_PDFP_Creator);}
  bool SetCreator(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Creator);return SetPropertyValue(cValue, tXPP_PDFP_Creator);}
  bool GetProducer(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Producer);return GetPropertyValue(cValue, tXPP_PDFP_Producer);}
  bool SetProducer(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Producer);return SetPropertyValue(cValue, tXPP_PDFP_Producer);}
  bool GetUserPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_UserPassword);return GetPropertyValue(cValue, tXPP_PDFP_UserPassword);}
  bool SetUserPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_UserPassword);return SetPropertyValue(cValue, tXPP_PDFP_UserPassword);}
  bool GetMasterPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_MasterPassword);return GetPropertyValue(cValue, tXPP_PDFP_MasterPassword);}
  bool SetMasterPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_MasterPassword);return SetPropertyValue(cValue, tXPP_PDFP_MasterPassword);}
  bool GetPrint(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Print);return GetPropertyValue(cValue, tXPP_PDFP_Print);}
  bool SetPrint(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Print);return SetPropertyValue(cValue, tXPP_PDFP_Print);}
  bool GetEdit(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Edit);return GetPropertyValue(cValue, tXPP_PDFP_Edit);}
  bool SetEdit(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Edit);return SetPropertyValue(cValue, tXPP_PDFP_Edit);}
  bool GetCopy(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Copy);return GetPropertyValue(cValue, tXPP_PDFP_Copy);}
  bool SetCopy(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Copy);return SetPropertyValue(cValue, tXPP_PDFP_Copy);}
  bool GetCommenting(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_Commenting);return GetPropertyValue(cValue, tXPP_PDFP_Commenting);}
  bool SetCommenting(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_Commenting);return SetPropertyValue(cValue, tXPP_PDFP_Commenting);}
  bool GetLowPrinting(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_PDFP_LowPrinting);return GetPropertyValue(cValue, tXPP_PDFP_LowPrinting);}
  bool SetLowPrinting(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_PDFP_LowPrinting);return SetPropertyValue(cValue, tXPP_PDFP_LowPrinting);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneUNZIPProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneUNZIPProps():CPropStonePropertiesInd(tXPP_UNZIPProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneUNZIPProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_UNZIPProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneUNZIPProps()
    {};

  bool operator ==(const CPropStoneUNZIPProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneUNZIPProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetArchivename(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_UNZIP_Archivename);return GetPropertyValue(cValue, tXPP_UNZIP_Archivename);}
  bool SetArchivename(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_UNZIP_Archivename);return SetPropertyValue(cValue, tXPP_UNZIP_Archivename);}
  bool GetFiles(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_UNZIP_Files);return GetPropertyValue(cValue, tXPP_UNZIP_Files);}
  bool SetFiles(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_UNZIP_Files);return SetPropertyValue(cValue, tXPP_UNZIP_Files);}
  bool GetDestination(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_UNZIP_Destination);return GetPropertyValue(cValue, tXPP_UNZIP_Destination);}
  bool SetDestination(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_UNZIP_Destination);return SetPropertyValue(cValue, tXPP_UNZIP_Destination);}
  bool GetPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_UNZIP_Password);return GetPropertyValue(cValue, tXPP_UNZIP_Password);}
  bool SetPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_UNZIP_Password);return SetPropertyValue(cValue, tXPP_UNZIP_Password);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneZIPProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneZIPProps():CPropStonePropertiesInd(tXPP_ZIPProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneZIPProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_ZIPProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneZIPProps()
    {};

  bool operator ==(const CPropStoneZIPProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneZIPProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetAddFiles(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ZIP_AddFiles);return GetPropertyValue(cValue, tXPP_ZIP_AddFiles);}
  bool SetAddFiles(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ZIP_AddFiles);return SetPropertyValue(cValue, tXPP_ZIP_AddFiles);}
  bool GetSubDirs(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ZIP_SubDirs);return GetPropertyValue(cValue, tXPP_ZIP_SubDirs);}
  bool SetSubDirs(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ZIP_SubDirs);return SetPropertyValue(cValue, tXPP_ZIP_SubDirs);}
  bool GetStorePath(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ZIP_StorePath);return GetPropertyValue(cValue, tXPP_ZIP_StorePath);}
  bool SetStorePath(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ZIP_StorePath);return SetPropertyValue(cValue, tXPP_ZIP_StorePath);}
  bool GetPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ZIP_Password);return GetPropertyValue(cValue, tXPP_ZIP_Password);}
  bool SetPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ZIP_Password);return SetPropertyValue(cValue, tXPP_ZIP_Password);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneFTPTransferProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneFTPTransferProps():CPropStonePropertiesInd(tXPP_FTPTransferProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneFTPTransferProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_FTPTransferProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneFTPTransferProps()
    {};

  bool operator ==(const CPropStoneFTPTransferProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneFTPTransferProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetServer(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Server);return GetPropertyValue(cValue, tXPP_FTP_Server);}
  bool SetServer(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Server);return SetPropertyValue(cValue, tXPP_FTP_Server);}
  bool GetPort(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Port);return GetPropertyValue(cValue, tXPP_FTP_Port);}
  bool SetPort(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Port);return SetPropertyValue(cValue, tXPP_FTP_Port);}
  bool GetRemoteStartDir(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_RemoteStartDir);return GetPropertyValue(cValue, tXPP_FTP_RemoteStartDir);}
  bool SetRemoteStartDir(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_RemoteStartDir);return SetPropertyValue(cValue, tXPP_FTP_RemoteStartDir);}
  bool GetRemoteFile(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_RemoteFile);return GetPropertyValue(cValue, tXPP_FTP_RemoteFile);}
  bool SetRemoteFile(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_RemoteFile);return SetPropertyValue(cValue, tXPP_FTP_RemoteFile);}
  bool GetLocalFile(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_LocalFile);return GetPropertyValue(cValue, tXPP_FTP_LocalFile);}
  bool SetLocalFile(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_LocalFile);return SetPropertyValue(cValue, tXPP_FTP_LocalFile);}
  bool GetDownload(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Download);return GetPropertyValue(cValue, tXPP_FTP_Download);}
  bool SetDownload(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Download);return SetPropertyValue(cValue, tXPP_FTP_Download);}
  bool GetUsername(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Username);return GetPropertyValue(cValue, tXPP_FTP_Username);}
  bool SetUsername(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Username);return SetPropertyValue(cValue, tXPP_FTP_Username);}
  bool GetPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Password);return GetPropertyValue(cValue, tXPP_FTP_Password);}
  bool SetPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Password);return SetPropertyValue(cValue, tXPP_FTP_Password);}
  bool GetTimeOut(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_TimeOut);return GetPropertyValue(cValue, tXPP_FTP_TimeOut);}
  bool SetTimeOut(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_TimeOut);return SetPropertyValue(cValue, tXPP_FTP_TimeOut);}
  bool GetTransfermode(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Transfermode);return GetPropertyValue(cValue, tXPP_FTP_Transfermode);}
  bool SetTransfermode(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FTP_Transfermode);return SetPropertyValue(cValue, tXPP_FTP_Transfermode);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneCMDProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneCMDProps():CPropStonePropertiesInd(tXPP_CMDProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneCMDProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_CMDProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneCMDProps()
    {};

  bool operator ==(const CPropStoneCMDProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneCMDProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetCmdFile(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_CMD_CmdFile);return GetPropertyValue(cValue, tXPP_CMD_CmdFile);}
  bool SetCmdFile(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_CMD_CmdFile);return SetPropertyValue(cValue, tXPP_CMD_CmdFile);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneSMTPProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneSMTPProps():CPropStonePropertiesInd(tXPP_SMTPProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneSMTPProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_SMTPProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneSMTPProps()
    {};

  bool operator ==(const CPropStoneSMTPProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneSMTPProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetSMTPServer(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_SMTP_SMTPServer);return GetPropertyValue(cValue, tXPP_SMTP_SMTPServer);}
  bool SetSMTPServer(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_SMTP_SMTPServer);return SetPropertyValue(cValue, tXPP_SMTP_SMTPServer);}
  bool GetSMTPPort(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_SMTP_SMTPPort);return GetPropertyValue(cValue, tXPP_SMTP_SMTPPort);}
  bool SetSMTPPort(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_SMTP_SMTPPort);return SetPropertyValue(cValue, tXPP_SMTP_SMTPPort);}
  bool GetFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_SMTP_From);return GetPropertyValue(cValue, tXPP_SMTP_From);}
  bool SetFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_SMTP_From);return SetPropertyValue(cValue, tXPP_SMTP_From);}
  bool GetTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_SMTP_To);return GetPropertyValue(cValue, tXPP_SMTP_To);}
  bool SetTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_SMTP_To);return SetPropertyValue(cValue, tXPP_SMTP_To);}
  bool GetSubject(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_SMTP_Subject);return GetPropertyValue(cValue, tXPP_SMTP_Subject);}
  bool SetSubject(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_SMTP_Subject);return SetPropertyValue(cValue, tXPP_SMTP_Subject);}
  bool GetFilename(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_SMTP_Filename);return GetPropertyValue(cValue, tXPP_SMTP_Filename);}
  bool SetFilename(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_SMTP_Filename);return SetPropertyValue(cValue, tXPP_SMTP_Filename);}
  bool GetUser(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_SMTP_User);return GetPropertyValue(cValue, tXPP_SMTP_User);}
  bool SetUser(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_SMTP_User);return SetPropertyValue(cValue, tXPP_SMTP_User);}
  bool GetPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_SMTP_Password);return GetPropertyValue(cValue, tXPP_SMTP_Password);}
  bool SetPassword(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_SMTP_Password);return SetPropertyValue(cValue, tXPP_SMTP_Password);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneOutlookProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneOutlookProps():CPropStonePropertiesInd(tXPP_OutlookProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneOutlookProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_OutlookProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneOutlookProps()
    {};

  bool operator ==(const CPropStoneOutlookProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneOutlookProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_OUTL_From);return GetPropertyValue(cValue, tXPP_OUTL_From);}
  bool SetFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_OUTL_From);return SetPropertyValue(cValue, tXPP_OUTL_From);}
  bool GetTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_OUTL_To);return GetPropertyValue(cValue, tXPP_OUTL_To);}
  bool SetTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_OUTL_To);return SetPropertyValue(cValue, tXPP_OUTL_To);}
  bool GetSubject(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_OUTL_Subject);return GetPropertyValue(cValue, tXPP_OUTL_Subject);}
  bool SetSubject(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_OUTL_Subject);return SetPropertyValue(cValue, tXPP_OUTL_Subject);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneEncodingProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneEncodingProps():CPropStonePropertiesInd(tXPP_EncodingProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneEncodingProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_EncodingProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneEncodingProps()
    {};

  bool operator ==(const CPropStoneEncodingProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneEncodingProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetFileFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ENCO_FileFrom);return GetPropertyValue(cValue, tXPP_ENCO_FileFrom);}
  bool SetFileFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ENCO_FileFrom);return SetPropertyValue(cValue, tXPP_ENCO_FileFrom);}
  bool GetFileTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ENCO_FileTo);return GetPropertyValue(cValue, tXPP_ENCO_FileTo);}
  bool SetFileTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ENCO_FileTo);return SetPropertyValue(cValue, tXPP_ENCO_FileTo);}
  bool GetMode(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ENCO_Mode);return GetPropertyValue(cValue, tXPP_ENCO_Mode);}
  bool SetMode(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ENCO_Mode);return SetPropertyValue(cValue, tXPP_ENCO_Mode);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneEncryptionProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneEncryptionProps():CPropStonePropertiesInd(tXPP_EncryptionProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneEncryptionProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_EncryptionProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneEncryptionProps()
    {};

  bool operator ==(const CPropStoneEncryptionProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneEncryptionProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetFileFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ENCR_FileFrom);return GetPropertyValue(cValue, tXPP_ENCR_FileFrom);}
  bool SetFileFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ENCR_FileFrom);return SetPropertyValue(cValue, tXPP_ENCR_FileFrom);}
  bool GetFileTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ENCR_FileTo);return GetPropertyValue(cValue, tXPP_ENCR_FileTo);}
  bool SetFileTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ENCR_FileTo);return SetPropertyValue(cValue, tXPP_ENCR_FileTo);}
  bool GetKey(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_ENCR_Key);return GetPropertyValue(cValue, tXPP_ENCR_Key);}
  bool SetKey(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_ENCR_Key);return SetPropertyValue(cValue, tXPP_ENCR_Key);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneFileMoveProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneFileMoveProps():CPropStonePropertiesInd(tXPP_FileMoveProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneFileMoveProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_FileMoveProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneFileMoveProps()
    {};

  bool operator ==(const CPropStoneFileMoveProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneFileMoveProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetFileFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_FMOV_FileFrom);return GetPropertyValue(cValue, tXPP_FMOV_FileFrom);}
  bool SetFileFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FMOV_FileFrom);return SetPropertyValue(cValue, tXPP_FMOV_FileFrom);}
  bool GetFileTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_FMOV_FileTo);return GetPropertyValue(cValue, tXPP_FMOV_FileTo);}
  bool SetFileTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FMOV_FileTo);return SetPropertyValue(cValue, tXPP_FMOV_FileTo);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneFileCopyProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneFileCopyProps():CPropStonePropertiesInd(tXPP_FileCopyProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneFileCopyProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_FileCopyProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneFileCopyProps()
    {};

  bool operator ==(const CPropStoneFileCopyProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneFileCopyProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetFileFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_FCOP_FileFrom);return GetPropertyValue(cValue, tXPP_FCOP_FileFrom);}
  bool SetFileFrom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FCOP_FileFrom);return SetPropertyValue(cValue, tXPP_FCOP_FileFrom);}
  bool GetFileTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_FCOP_FileTo);return GetPropertyValue(cValue, tXPP_FCOP_FileTo);}
  bool SetFileTo(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FCOP_FileTo);return SetPropertyValue(cValue, tXPP_FCOP_FileTo);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneFileRemoveProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneFileRemoveProps():CPropStonePropertiesInd(tXPP_FileRemoveProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneFileRemoveProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_FileRemoveProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneFileRemoveProps()
    {};

  bool operator ==(const CPropStoneFileRemoveProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneFileRemoveProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetFile(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_FREM_File);return GetPropertyValue(cValue, tXPP_FREM_File);}
  bool SetFile(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_FREM_File);return SetPropertyValue(cValue, tXPP_FREM_File);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
class XPUBMODEL_EXPORTIMPORT CPropStoneIndesignNewDocProps : public CPropStonePropertiesInd
{
  friend class CPropStoneContainer;
public:
  // real construct
  CPropStoneIndesignNewDocProps():CPropStonePropertiesInd(tXPP_IndesignNewDocProps)
    {m_pProperties = 0;};
  // bound construct
  CPropStoneIndesignNewDocProps(CPropStonePropertiesInd* pProps):CPropStonePropertiesInd(tXPP_IndesignNewDocProps)
    {m_pProperties = pProps;};
  // destruct
  ~CPropStoneIndesignNewDocProps()
    {};

  bool operator ==(const CPropStoneIndesignNewDocProps& cProperties){if (m_pProperties)return m_pProperties->operator ==(cProperties);return operator ==(cProperties);};
  void operator =(const CPropStoneIndesignNewDocProps& cProperties){if (m_pProperties)m_pProperties->operator =(cProperties);CPropStonePropertiesInd::operator =(cProperties);};

  bool GetDocumentPreset(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_DocumentPreset);return GetPropertyValue(cValue, tXPP_INDD_DocumentPreset);}
  bool SetDocumentPreset(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_DocumentPreset);return SetPropertyValue(cValue, tXPP_INDD_DocumentPreset);}
  bool GetNumberOfPages(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_NumberOfPages);return GetPropertyValue(cValue, tXPP_INDD_NumberOfPages);}
  bool SetNumberOfPages(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_NumberOfPages);return SetPropertyValue(cValue, tXPP_INDD_NumberOfPages);}
  bool GetFacingPages(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_FacingPages);return GetPropertyValue(cValue, tXPP_INDD_FacingPages);}
  bool SetFacingPages(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_FacingPages);return SetPropertyValue(cValue, tXPP_INDD_FacingPages);}
  bool GetMasterTextFrame(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_MasterTextFrame);return GetPropertyValue(cValue, tXPP_INDD_MasterTextFrame);}
  bool SetMasterTextFrame(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_MasterTextFrame);return SetPropertyValue(cValue, tXPP_INDD_MasterTextFrame);}
  bool GetPageSize(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_PageSize);return GetPropertyValue(cValue, tXPP_INDD_PageSize);}
  bool SetPageSize(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_PageSize);return SetPropertyValue(cValue, tXPP_INDD_PageSize);}
  bool GetWidth(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_Width);return GetPropertyValue(cValue, tXPP_INDD_Width);}
  bool SetWidth(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_Width);return SetPropertyValue(cValue, tXPP_INDD_Width);}
  bool GetHeight(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_Height);return GetPropertyValue(cValue, tXPP_INDD_Height);}
  bool SetHeight(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_Height);return SetPropertyValue(cValue, tXPP_INDD_Height);}
  bool GetOrientation(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_Orientation);return GetPropertyValue(cValue, tXPP_INDD_Orientation);}
  bool SetOrientation(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_Orientation);return SetPropertyValue(cValue, tXPP_INDD_Orientation);}
  bool GetColumnsNumber(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_ColumnsNumber);return GetPropertyValue(cValue, tXPP_INDD_ColumnsNumber);}
  bool SetColumnsNumber(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_ColumnsNumber);return SetPropertyValue(cValue, tXPP_INDD_ColumnsNumber);}
  bool GetColumnsGutter(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_ColumnsGutter);return GetPropertyValue(cValue, tXPP_INDD_ColumnsGutter);}
  bool SetColumnsGutter(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_ColumnsGutter);return SetPropertyValue(cValue, tXPP_INDD_ColumnsGutter);}
  bool GetMarginsTop(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_MarginsTop);return GetPropertyValue(cValue, tXPP_INDD_MarginsTop);}
  bool SetMarginsTop(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_MarginsTop);return SetPropertyValue(cValue, tXPP_INDD_MarginsTop);}
  bool GetMarginsBottom (CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue,tXPP_INDD_MarginsBottom);return GetPropertyValue(cValue, tXPP_INDD_MarginsBottom);}
  bool SetMarginsBottom(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_MarginsBottom);return SetPropertyValue(cValue, tXPP_INDD_MarginsBottom);}
  bool GetMarginsInside(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_MarginsInside);return GetPropertyValue(cValue, tXPP_INDD_MarginsInside);}
  bool SetMarginsInside(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_MarginsInside);return SetPropertyValue(cValue, tXPP_INDD_MarginsInside);}
  bool GetMarginsOutside(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->GetPropertyValue(cValue, tXPP_INDD_MarginsOutside);return GetPropertyValue(cValue, tXPP_INDD_MarginsOutside);}
  bool SetMarginsOutside(CXPubVariant& cValue){if (m_pProperties) return m_pProperties->SetPropertyValue(cValue, tXPP_INDD_MarginsOutside);return SetPropertyValue(cValue, tXPP_INDD_MarginsOutside);}
protected:
  // bound properties
  CPropStonePropertiesInd* m_pProperties;
};
// XPubWizard --> InsertNewPropClass









class XPUBMODEL_EXPORTIMPORT CPropStoneContainer
{
public:
  CPropStoneContainer();
  ~CPropStoneContainer();

  bool SaveXMLContent(xtstring& sXMLDocText);
  bool ReadXMLContent(const xtstring& sXMLDocText);

  size_t CountOfPropertyGroups() const {return m_cPropertyGroups.size();};
  CPropStonePropertiesInd* GetPropertyGroup(size_t nIndex);
  bool AddPropertyGroup(CPropStonePropertiesInd* pProperties);
  bool MovePropertyGroupUp(size_t nIndex);
  bool MovePropertyGroupDown(size_t nIndex);
  bool RemovePropertyGroup(size_t nIndex);
  bool RemoveAllPropertyGroups();

  bool RemoveAllGarbageGroups();

  static bool GetAllPropGroupNames(CxtstringVector& arrPropGroupNames);
  static xtstring PropGroupNameFromIndex(int nPropGroupIndex);
  static int IndexFromPropGroupName(const xtstring& sPropGroupName);
  static bool PropGroupExist(int nPropGroupIndex);
  static xtstring ConvertPropNameToNameForEvaluate(const xtstring& sPropName);
  static bool GetAllPropsFromPropGroupName(const xtstring& sPropGroupName, CxtstringVector& arrPropNames);
  static TXPubProperty GetPropertyGroupTypeFromPropertyGroupName(const xtstring& sPropGroupName);

  CPropStoneExcelProps* GetNextGroup(CPropStoneExcelProps* pStartGroup){return ((CPropStoneExcelProps*)GetNextGroup(tXPP_ExcelProps, (void*)pStartGroup));};
  CPropStoneRTFProps* GetNextGroup(CPropStoneRTFProps* pStartGroup){return ((CPropStoneRTFProps*)GetNextGroup(tXPP_RTFProps, (void*)pStartGroup));};
  CPropStonePDFProps* GetNextGroup(CPropStonePDFProps* pStartGroup){return ((CPropStonePDFProps*)GetNextGroup(tXPP_PDFProps, (void*)pStartGroup));};
  CPropStoneUNZIPProps* GetNextGroup(CPropStoneUNZIPProps* pStartGroup){return ((CPropStoneUNZIPProps*)GetNextGroup(tXPP_UNZIPProps, (void*)pStartGroup));};
  CPropStoneZIPProps* GetNextGroup(CPropStoneZIPProps* pStartGroup){return ((CPropStoneZIPProps*)GetNextGroup(tXPP_ZIPProps, (void*)pStartGroup));};
  CPropStoneFTPTransferProps* GetNextGroup(CPropStoneFTPTransferProps* pStartGroup){return ((CPropStoneFTPTransferProps*)GetNextGroup(tXPP_FTPTransferProps, (void*)pStartGroup));};
  CPropStoneCMDProps* GetNextGroup(CPropStoneCMDProps* pStartGroup){return ((CPropStoneCMDProps*)GetNextGroup(tXPP_CMDProps, (void*)pStartGroup));};
  CPropStoneSMTPProps* GetNextGroup(CPropStoneSMTPProps* pStartGroup){return ((CPropStoneSMTPProps*)GetNextGroup(tXPP_SMTPProps, (void*)pStartGroup));};
  CPropStoneOutlookProps* GetNextGroup(CPropStoneOutlookProps* pStartGroup){return ((CPropStoneOutlookProps*)GetNextGroup(tXPP_OutlookProps, (void*)pStartGroup));};
  CPropStoneEncodingProps* GetNextGroup(CPropStoneEncodingProps* pStartGroup){return ((CPropStoneEncodingProps*)GetNextGroup(tXPP_EncodingProps, (void*)pStartGroup));};
  CPropStoneEncryptionProps* GetNextGroup(CPropStoneEncryptionProps* pStartGroup){return ((CPropStoneEncryptionProps*)GetNextGroup(tXPP_EncryptionProps, (void*)pStartGroup));};
  CPropStoneFileMoveProps* GetNextGroup(CPropStoneFileMoveProps* pStartGroup){return ((CPropStoneFileMoveProps*)GetNextGroup(tXPP_FileMoveProps, (void*)pStartGroup));};
  CPropStoneFileCopyProps* GetNextGroup(CPropStoneFileCopyProps* pStartGroup){return ((CPropStoneFileCopyProps*)GetNextGroup(tXPP_FileCopyProps, (void*)pStartGroup));};
  CPropStoneFileRemoveProps* GetNextGroup(CPropStoneFileRemoveProps* pStartGroup){return ((CPropStoneFileRemoveProps*)GetNextGroup(tXPP_FileRemoveProps, (void*)pStartGroup));};
  CPropStoneIndesignNewDocProps* GetNextGroup(CPropStoneIndesignNewDocProps* pStartGroup){return ((CPropStoneIndesignNewDocProps*)GetNextGroup(tXPP_IndesignNewDocProps, (void*)pStartGroup));};
// XPubWizard --> InsertNewPropFunction

protected:

  void* GetNextGroup(TXPubProperty nPropGroupType, void* pStartForNext);
  void* ReturnGarbageGroup(TXPubProperty nPropGroupType, CPropStonePropertiesInd* pPropGroup);

  CPropertyGroups   m_cPropertyGroups;
  CPropertyGroups   m_cGarbageGroups;
};

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_PROPSTONEIND_H_)
