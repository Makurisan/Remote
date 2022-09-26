#if !defined(_MODELDEF_H_)
#define _MODELDEF_H_



#include "ModelExpImp.h"
#include "SSTLdef/STLdef.h"
#include "Messages.h"

class CXPubMarkUp;


#define XPUBLISHER_VERSION_STRING   _XT("101")
#define XPUBLISHER_VERSION_LONG     101


#define SUBFOLDER_ER          _XT("ERMOD")
#define SUBFOLDER_IFS         _XT("INFO")
#define SUBFOLDER_STONE       _XT("STONES")
#define SUBFOLDER_TEMPLATE    _XT("TEMPLATES")
#define SUBFOLDER_PYTHON      _XT("MODULE")

#define FILE_EXT_ER           _XT(".er")
#define FILE_EXT_IFS          _XT(".ifs")
#define FILE_EXT_STONE        _XT(".stone")
#define FILE_EXT_TEMPLATE     _XT(".template")
#define FILE_EXT_PYTHON       _XT(".py")

// default project constants
#define CONST_QUERYDATABASEPATH       _XT("QUERYDATABASEPATH")
#define CONST_QUERYEXCELPATH          _XT("QUERYEXCELPATH")
#define CONST_QUERYEXCELFILENAME      _XT("QUERYEXCELFILENAME")


typedef enum TLongDateFormat
{
  tLDFFirst_Dummy = 0,
  tLDF_DMY,                 // 1.2.2004     / 1.2.04        1.feb.2004
  tLDF_DDMMY,               // 01.02.2004   / 01.02.04      1.feb.2004
  tLDF_MDY,                 // 2.1.2004     / 2.1.04        1.feb.2004
  tLDF_MMDDY,               // 02.01.2004   / 02.01.04      1.feb.2004
  tLDF_YMD,                 // 2004.2.1     / 04.2.1        1.feb.2004
  tLDF_YMMDD,               // 2004.02.01   / 04.02.01      1.feb.2004
  tLDFLast_Dummy,
  tLDF_Default = tLDF_DDMMY
};

#define DEFAULT_YEARWITHCENTURY                 true
#define DEFAULT_DATESEPARATOR                   _XT(".")
#define DEFAULT_TIMESEPARATOR                   _XT(":")
#define DEFAULT_USESYMBOLSFROMSYSTEM            true
#define DEFAULT_DECIMALSYMBOL                   _XT(",")
#define DEFAULT_DIGITGROUPINGSYMBOL             _XT(".")
#define DEFAULT_USECOUNTOFDIGITSAFTERDECIMAL    false
#define DEFAULT_COUNTOFDIGITSAFTERDECIMAL       2
#define DEFAULT_NEGATIVESIGN                    _XT("-")



#define CONDDELIMITER_FIRST_LEVEL     _XT(';')
#define CONDDELIMITER_SECOND_LEVEL    _XT(' ')
#define CONDDELIMITER_THIRD_LEVEL     _XT(',')
typedef enum TGroupOfFieldTypes
{
  tFieldTypeGroupStatus = 1,
  tFieldTypeGroupErrorType,
  tFieldTypeGroupBinary,
  tFieldTypeGroupNumber,
  tFieldTypeGroupCurrency,
  tFieldTypeGroupDateTime,
  tFieldTypeGroupText,
};
typedef enum TFieldType
{
  tFieldTypeEmpty = 0,
  tFieldTypeTinyInt = 16,
  tFieldTypeSmallInt = 2,
  tFieldTypeInteger = 3,
  tFieldTypeBigInt = 20,
  tFieldTypeUnsignedTinyInt = 17,
  tFieldTypeUnsignedSmallInt = 18,
  tFieldTypeUnsignedInt = 19,
  tFieldTypeUnsignedBigInt = 21,
  tFieldTypeSingle = 4,
  tFieldTypeDouble = 5,
  tFieldTypeCurrency = 6,
  tFieldTypeDecimal = 14,
  tFieldTypeNumeric = 131,
  tFieldTypeBoolean = 11,
  tFieldTypeError = 10,
  tFieldTypeUserDefined = 132,
  tFieldTypeVariant = 12,
  tFieldTypeIDispatch = 9,
  tFieldTypeIUnknown = 13,
  tFieldTypeGUID = 72,
  tFieldTypeDate = 7,
  tFieldTypeDBDate = 133,
  tFieldTypeDBTime = 134,
  tFieldTypeDBTimeStamp = 135,
  tFieldTypeBSTR = 8,
  tFieldTypeChar = 129,
  tFieldTypeVarChar = 200,
  tFieldTypeLongVarChar = 201,
  tFieldTypeWChar = 130,
  tFieldTypeVarWChar = 202,
  tFieldTypeLongVarWChar = 203,
  tFieldTypeBinary = 128,
  tFieldTypeVarBinary = 204,
  tFieldTypeLongVarBinary = 205,
  tFieldTypeChapter = 136,
  tFieldTypeFileTime = 64,
  tFieldTypePropVariant = 138,
  tFieldTypeVarNumeric = 139
};

typedef enum TDBStoneProcessingType
{
  tDBStonePTFirst_Dummy = 0,
  tDBStonePT_Add,
  tDBStonePT_Update,
  tDBStonePT_Remove,
  tDBStonePTLast_Dummy,
  tDBStonePT_Default = tDBStonePT_Add,
};

typedef enum TDBDefinitionType
{
  tDBTypeFirst_Dummy = 0,
  tDBTypeMySQL,
  tDBTypeMSADO,
  tDBTypeIMDB,
  tDBTypeDataProvider,
  tDBTypeFirebird,
  tDBTypeXMLEngine,
  tDBTypeSQLite,
  tDBTypeOracle,
  tDBTypeLast_Dummy,
  tDBType_Default = tDBTypeMSADO,
};

typedef enum TSaveTemplateInFormat
{
  tSaveTemplateInFormat_FirstDummy = -1,
  tSaveTemplateInFormat_Native = 0,
  tSaveTemplateInFormat_Plain = 1,
  tSaveTemplateInFormat_Html = 2,
  tSaveTemplateInFormat_Rtf = 3,
  tSaveTemplateInFormat_Tag = 4,
  tSaveTemplateInFormat_Db = 5,
  tSaveTemplateInFormat_Pdf = 6,
  tSaveTemplateInFormat_Ter = 7,
  tSaveTemplateInFormat_Word = 8,
  tSaveTemplateInFormat_WordXML = 9,
  tSaveTemplateInFormat_Excel = 11,
  tSaveTemplateInFormat_ExcelXML = 12,
  tSaveTemplateInFormat_Outlook = 13,
  tSaveTemplateInFormat_Mapi = 14,
  tSaveTemplateInFormat_ExecLogFile = 15,
  tSaveTemplateInFormat_Indesign = 16,
  tSaveTemplateInFormat_Fdf = 17,
  tSaveTemplateInFormat_IndesignBook = 18,
  tSaveTemplateInFormat_IndesignLib = 19,
  tSaveTemplateInFormat_IndesignTemplate = 20,
  tSaveTemplateInFormat_IndesignContent = 21,
  tSaveTemplateInFormat_LastDummy = 21,
  tSaveTemplateInFormat_Default = tSaveTemplateInFormat_Native,
};

typedef vector<TSaveTemplateInFormat>                   TSaveTemplateInFormatVector;
typedef TSaveTemplateInFormatVector::iterator           TSaveTemplateInFormatVectorIterator;
typedef TSaveTemplateInFormatVector::reverse_iterator   TSaveTemplateInFormatVectorReverseIterator;
typedef TSaveTemplateInFormatVector::const_iterator     TSaveTemplateInFormatVectorConstIterator;

typedef enum TTemplateTarget
{
  tTemplateTarget_FirstDummy = -1,
  tTemplateTarget_SaveToFile = 0,
  tTemplateTarget_InsertToTemplate,
  tTemplateTarget_SaveWorkPaperToMemory,
  tTemplateTarget_LastDummy,
  tTemplateTarget_Default = tTemplateTarget_SaveToFile,
};

#define PLAIN_FILE_EXTENSION      _XT(".txt")
#define HTML_FILE_EXTENSION       _XT(".html")
#define RTF_FILE_EXTENSION        _XT(".rtf")
#define TAG_FILE_EXTENSION        _XT(".xpubtag")
#define DB_FILE_EXTENSION         _XT(".db")
#define PROPERTY_FILE_EXTENSION   _XT(".prop")
#define XML_FILE_EXTENSION        _XT(".xml")
#define UNKNOWN_FILE_EXTENSION    _XT(".unknown")

typedef enum TDataFormatForStoneAndTemplate
{
  tDataFormatST_FirstDummy = -1,
  tDataFormatST_Plain = 0,
  tDataFormatST_Html,
  tDataFormatST_Rtf,
  tDataFormatST_Tag,
  tDataFormatST_Db,
  tDataFormatST_Property,
  tDataFormatST_Pdf,
  tDataFormatST_Word,
  tDataFormatST_Outlook,
  tDataFormatST_InDesign,
  tDataFormatST_Fdf,
  tDataFormatST_Xml,
  tDataFormatST_LastDummy,
  tDataFormatST_Default = tDataFormatST_Plain,
};

typedef enum TViewTypeForStoneAndTemplate
{
  tViewTypeST_FirstDummy = -1,
  tViewTypeST_DontUse = 0,
  tViewTypeST_Scintilla,
  tViewTypeST_Html,
  tViewTypeST_Te,
  tViewTypeST_Db,
  tViewTypeST_Property,
  tViewTypeST_LastDummy,
};

typedef enum TProjectFileType
{
  tProjectFile_Nothing = 0, // wegen XML eingefuehrt
  tProjectFileER = 1,
  tProjectFileIFS,
  tProjectFileStone,
  tProjectFileTemplate,
  tProjectFilePythonModule,
};

typedef enum TAddFileConflictResolution
{
  tAFCR_OverwriteOldModule,
  tAFCR_DetachOldModule,
  tAFCR_DetachAndDeleteOldModule,
  tAFCR_ChangeNameOfNewModule,
  tAFCR_DoNothing,
  tAFCR_Default = tAFCR_ChangeNameOfNewModule,
};

typedef enum THitType
{
  tHitType_FirstDummy = 0,
  tHitType_Template = 1,
  tHitType_Stone = 2,
  tHitType_UserFunction = 10,
  tHitType_LastDummy,
  tHitType_Default = tHitType_Stone,
};

typedef enum TChildExecution
{ 
  tChildExecution_FirstDummy = -1,
  tChildExecution_BeforeHit = 0,
  tChildExecution_AfterHit,
  tChildExecution_LastDummy,
  tChildExecution_Default = tChildExecution_BeforeHit,
};

typedef enum TReadMethod
{
  tReadMethod_FirstDummy = -1,
  tReadMethod_ReadPath = 0,
  tReadMethod_ConditionPath,
  tReadMethod_LastDummy,
  tReadMethod_Default = tReadMethod_ReadPath,
};

typedef enum TIFSTypeForThirdParty
{
  tIFSType_FirstDummy = -1,
  tIFSType_Closed = 0,
  tIFSType_Opened,
  tIFSType_Template,
  tIFSType_LastDummy,
  tIFSType_Default = tIFSType_Closed,
};

typedef enum XPubGetProjectIFSs_Which
{
  XPubGPIFS_Closed = 1,
  XPubGPIFS_Opened = 2,
  XPubGPIFS_Template = 4,
  XPubGPIFS_ClosedAndOpened = XPubGPIFS_Closed | XPubGPIFS_Opened,
  XPubGPIFS_ClosedAndTemplate = XPubGPIFS_Closed | XPubGPIFS_Template,
  XPubGPIFS_OpenedAndTemplate = XPubGPIFS_Opened | XPubGPIFS_Template,
  XPubGPIFS_All  = XPubGPIFS_Closed | XPubGPIFS_Opened | XPubGPIFS_Template,
};

typedef enum TShowBehaviour
{ // Checkbox: Checkbox(Ja), Checkbox(Nein), Nur Checkbox(Nein), Nur Checkbox(Ja), Keine
  tShowBehaviour_FirstDummy = -1,
  tShowBehaviour_NotLockable = 0,                 // keine
  tShowBehaviour_Lockable_NotLocked,              // checkbox (ja)
  tShowBehaviour_Lockable_Locked,                 // checkbox (nein)
  tShowBehaviour_Lockable_NotLocked_OnlyCheck,    // nur checkbox (ja)
  tShowBehaviour_Lockable_Locked_OnlyCheck,       // nur checkbox (nein)
  tShowBehaviour_LastDummy,
  tShowBehaviour_Default = tShowBehaviour_NotLockable,
};

typedef enum TSQLCommandShowMethod
{
  tSQLCommandShowMethod_FirstDummy = 0,
  tSQLCommandShowMethod_SQLCommandForm = 1,
  tSQLCommandShowMethod_OnlyDescription,
  tSQLCommandShowMethod_LastDummy,
  tSQLCommandShowMethod_Default = tSQLCommandShowMethod_SQLCommandForm,
};

typedef enum TConditionShowMethod
{
  tConditionShowMethod_FirstDummy = 0,
  tConditionShowMethod_TextForm = 1,
  tConditionShowMethod_ExpressionForm,
  tConditionShowMethod_OnlyDescription,
  tConditionShowMethod_LastDummy,
  tConditionShowMethod_Default = tConditionShowMethod_ExpressionForm,
};

typedef enum TTypeOfEntity
{
  tTypeOfEntity_FirstDummy = -1,
  tTypeOfEntity_True = 0,
  tTypeOfEntity_TrueFalse,
  tTypeOfEntity_SelectCase,
  tTypeOfEntity_LastDummy,
  tTypeOfEntity_Default = tTypeOfEntity_True,
};


typedef enum TWorkPaperType
{
  tWorkPaperType_FirstDummy = 0,
  tWorkPaperType_Stone = 1,
  tWorkPaperType_Template,
  tWorkPaperType_LastDummy,
  tWorkPaperType_Default = tWorkPaperType_Stone,
};

typedef enum TWorkPaperCopyType
{
  tCopyType_FirstDummy = -1,
  // arbetsblatt wird in zielarbeitsblatt erstem PlaceHolder ersetzen
  tCopyType_ReplaceSingle = 0,
  // arbetsblatt wird in zielarbeitsblatt alle PlaceHolder ersetzen
  tCopyType_ReplaceMultiple,
  // arbetsblatt wird in zielarbeitsblatt vor erstem PlaceHolder eingefuegt
  tCopyType_InsertSingle,
  // arbetsblatt wird in zielarbeitsblatt vor alle PlaceHolder eingefuegt
  tCopyType_InsertMultiple,
  // arbetsblatt wird in zielarbeitsblatt am ende angehaengt
  tCopyType_InsertBack,
  // arbetsblatt wird in zielarbeitsblatt vorne eingefuegt
  tCopyType_InsertFront,
//  // in Liste anhaengen
//  tCopyType_AddToList,
  // arbeitsblatt nach aktuelle Position einfuegen
  tCopyType_ActualPosition,
  tCopyType_LastDummy,
  tCopyType_Default = tCopyType_ReplaceSingle,
};

typedef enum TWorkPaperProcessingMode
{
  tProcessingMode_FirstDummy = -1,
  // arbeitsblatt wird sofort eingefuegt
  tProcessingMode_Immediately,
  // arbeitsblatt wird in liste angehaengt und spaeter eingefuegt
  tProcessingMode_Later,
  tProcessingMode_LastDummy,
  tProcessingMode_Default = tProcessingMode_Immediately,
};


typedef enum TPreviewType
{
  tPreviewType_FirstDummy = -1,
  tPreviewType_RTF = 0,
  tPreviewType_PDF,
  tPreviewType_LastDummy,
  tPreviewType_Default = tPreviewType_RTF,
};

class XPUBMODEL_EXPORTIMPORT CDirectoryCheckerCallBack
{
public:
  virtual bool CheckOrCreateDirectory(const xtstring& sFolder, LPCXTCHAR pSubFolder) = 0;
  virtual bool RemoveFile(const xtstring& sFileName) = 0;
  virtual bool ExistThisFile(const xtstring& sFileName) = 0;
};

class CExecutionMessage;
class CHitEntryInfo;
typedef enum TSourceOfMessage;



typedef unsigned long       OUT_MESSAGE_ITEM;
typedef unsigned long       LOG_MESSAGE_ITEM;
#define OUT_MESSAGE_NOT_CREATED   0xABCDEF
#define LOG_MESSAGE_NOT_CREATED   0xABCDEF


class XPUBMODEL_EXPORTIMPORT CLogFileSupport
{
public:
  //
  virtual LOG_MESSAGE_ITEM ShowLogMessage(LOG_MESSAGE_ITEM nParentItem, CExecutionMessage* pMessage) = 0;
  virtual bool ChangeSourceOfMessage(LOG_MESSAGE_ITEM nItem, TSourceOfMessage nSourceOfMessage) = 0;
  virtual bool ChangeTextInLogMessage(LOG_MESSAGE_ITEM nItem, const xtstring& sText, const xtstring& sMsgDescription) = 0;
  virtual bool AddTextInLogMessage(LOG_MESSAGE_ITEM nItem, const xtstring& sText) = 0;
  //
  virtual bool ShowHitEntryInfo(CHitEntryInfo* pHitEntryInfo, bool bRootItemRealised) = 0;

  //
  virtual bool SetLogFile(const xtstring& sFileName) = 0;
  virtual xtstring GetLogFile() = 0;
};

typedef enum TExecStartedFrom
{
  // Ausfuehrung
  tESF_XPublisher_Exec = 1,
  tESF_WEB_Exec,
  tESF_ATL_Exec,
  tESF_ProcessFiles_Exec,
  tESF_XPubClient_Exec,

  // Query Interface
  tESF_XPublisher_QueryInterface = 20,

  // SQL Abfrage
  tESF_XPublisher_BuildFreeSelection = 40,
  tESF_ATL_BuildFreeSelection,
  tESF_XPubClient_BuildFreeSelection,

  // anderes
  tESF_Other = 60,
};

typedef enum TExecFileGeneratedType
{
  // gewöhnliche Datei, die aus Vorlage entstanden ist
  tExecGeneratedFile_OrdinaryFile = 0,

  // Steinzeiten, wenn man von XPubFrmRtf DB generiert hat 
  // tExecGeneratedFile_DatabaseFile = 1,

  // LOG Datei, die in XPubWEBExec erzeugt wurde
  tExecGeneratedFile_LOGFile = 2,

  // Datei, die eventuell bei Ausführung gar nicht erzeugt wurde,
  // aber sollte als Besteindteil den generierten Dateien betrachtet werden
  tExecGeneratedFile_RelatedFile = 3,
};

typedef vector<TExecFileGeneratedType>                  TExecFileGeneratedTypeVector;
typedef TExecFileGeneratedTypeVector::iterator          TExecFileGeneratedTypeVectorIterator;
typedef TExecFileGeneratedTypeVector::reverse_iterator  TExecFileGeneratedTypeVectorReverseIterator;
typedef TExecFileGeneratedTypeVector::const_iterator    TExecFileGeneratedTypeVectorConstIterator;

class XPUBMODEL_EXPORTIMPORT CAddOnManagerCallBack
{
public:
  CAddOnManagerCallBack* GetAddOnManagerCallBack(){return this;};
};

class XPUBMODEL_EXPORTIMPORT CTransferTemplateOverMemoryCallBack
{
public:
  virtual bool TransferTemplateOverMemory(const xtstring& sWorkPaperName, const xtstring& sXMLMemoryContent) = 0;
};

class XPUBMODEL_EXPORTIMPORT CConditionOrAssignment
{
public:
  typedef enum TConditionOrAssignemntType
  {
    tCOAT_Temp_Object = -1,
    tCOAT_Main_Condition = 0,
    tCOAT_Port_Condition,
    tCOAT_ERTableRealName_Assignment,
    tCOAT_HitName_Assignment,
    tCOAT_Hit_Condition,
    tCOAT_Tag_Assignment,
    tCOAT_TemplateFileShowName_Assignment,
    tCOAT_TemplateFile_Assignment,
    tCOAT_TemplateFolder_Assignment,
    tCOAT_TemplateDBName_Assignment,
    tCOAT_Visibility_Condition,
    tCOAT_Content_Assignment,
    tCOAT_Address_Assignment,
    tCOAT_QuickInfo_Assignment,
    tCOAT_PicFileNameLocal_Assignment,
    tCOAT_PicFileName_Assignment,
    tCOAT_Assignment_Assignment,
    tCOAT_AssignField_Assignment,
    tCOAT_AssignValue_Assignment,
    tCOAT_ProjConstant_Assignment,
    tCOAT_PropStoneProperty_Assignment,
    tCOAT_DBStoneValue_Assignment,
    tCOAT_DBTemplateValue_Assignment,
    tCOAT_OneConditionVariation_Condition,
    tCOAT_ParameterForTextForm_Assignment,
    tCOAT_KeyFieldValue_Assignment,
    tCOAT_HierarchyItem_Condition,
  };

  CConditionOrAssignment(TConditionOrAssignemntType nType, bool bUsedInWorkPaper);
  ~CConditionOrAssignment();

  void Reset();
  bool IsEmpty() const;

  void CopyWithUsedInWorkPaper(const CConditionOrAssignment& cClass);
  void CopyWithUsedInWorkPaper(const CConditionOrAssignment* pClass);
  void operator = (const CConditionOrAssignment& cClass);
  void operator = (const CConditionOrAssignment* pClass);
  bool operator == (const CConditionOrAssignment& cClass) const;
  bool operator != (const CConditionOrAssignment& cClass) const;

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc) const;
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetExprText() const {return m_sExprText;};
  void SetExprText(const xtstring& sText){m_sExprText = sText;};
  xtstring GetExprValues() const {return m_sExprValues;};
  void SetExprValues(const xtstring& sText){m_sExprValues = sText;};
  xtstring GetExprSymbols() const {return m_sExprSymbols;};
  void SetExprSymbols(const xtstring& sText){m_sExprSymbols = sText;};
  xtstring GetExprOffsets() const {return m_sExprOffsets;};
  void SetExprOffsets(const xtstring& sText){m_sExprOffsets = sText;};
  xtstring GetLastUsedIFS() const {return m_sLastUsedIFS;};
  void SetLastUsedIFS(const xtstring& sLastUsedIFS){m_sLastUsedIFS = sLastUsedIFS;};
  xtstring GetLastUsedEntity() const {return m_sLastUsedEntity;};
  void SetLastUsedEntity(const xtstring& sLastUsedEntity){m_sLastUsedEntity = sLastUsedEntity;};
  bool GetLastUsedUseEntityInExpression() const {return m_bLastUsedUseEntityInExpression;};
  void SetLastUsedUseEntityInExpression(bool bLastUsedUseEntityInExpression){m_bLastUsedUseEntityInExpression = bLastUsedUseEntityInExpression;};

  bool GetUsedInWorkPaper(){return m_bUsedInWorkPaper;};

protected:
  void GetXMLTags(xtstring& sXML_Text, xtstring& sXML_ExprValues, xtstring& sXML_ExprSymbols, xtstring& sXML_ExprOffsets);
  xtstring GetChildXMLTag() const;

  xtstring      m_sExprText;
  xtstring      m_sExprValues;
  xtstring      m_sExprSymbols;
  xtstring      m_sExprOffsets;
  xtstring      m_sLastUsedIFS;
  xtstring      m_sLastUsedEntity;
  bool          m_bLastUsedUseEntityInExpression;
  // non stream data -->
  TConditionOrAssignemntType  m_nType;
  bool                        m_bUsedInWorkPaper;
  // non stream data <--
};

typedef vector<CConditionOrAssignment*>   CCOAVector;
typedef CCOAVector::iterator              CCOAVectorIterator;
typedef CCOAVector::reverse_iterator      CCOAVectorReverseIterator;
typedef CCOAVector::const_iterator        CCOAVectorConstIterator;


class XPUBMODEL_EXPORTIMPORT CGeneratedFile
{
public:
  CGeneratedFile()
  {
    m_sFullFileName.clear();
    m_sFileShowName.clear();
    m_nFileFormat = tSaveTemplateInFormat_Native;
    m_nFileType = tExecGeneratedFile_OrdinaryFile;
    m_sTag.clear();
    m_bFileGeneratedOverMemory = false;
  };
  ~CGeneratedFile(){};

  void operator = (const CGeneratedFile& cOneFile)
  {
    m_sFullFileName = cOneFile.m_sFullFileName;
    m_sFileShowName = cOneFile.m_sFileShowName;
    m_nFileFormat = cOneFile.m_nFileFormat;
    m_nFileType = cOneFile.m_nFileType;
    m_sTag = cOneFile.m_sTag;
	m_bFileGeneratedOverMemory = cOneFile.m_bFileGeneratedOverMemory;
  }

  xtstring GetFileName() const;   // file name from m_sFullFileName
  xtstring GetFileFolder() const; // folder from m_sFullFileName

  xtstring                m_sFullFileName;
  xtstring                m_sFileShowName;
  TSaveTemplateInFormat   m_nFileFormat;
  TExecFileGeneratedType  m_nFileType;
  xtstring                m_sTag;
  bool                    m_bFileGeneratedOverMemory;
};

typedef vector<CGeneratedFile>            CGeneratedFiles;
typedef CGeneratedFiles::iterator         CGeneratedFilesIterator;
typedef CGeneratedFiles::reverse_iterator CGeneratedFilesReverseIterator;
typedef CGeneratedFiles::const_iterator   CGeneratedFilesConstIterator;

class XPUBMODEL_EXPORTIMPORT COutMessageContainerCallBack
{
public:
  COutMessageContainerCallBack(){};
  virtual ~COutMessageContainerCallBack(){};
  //
  virtual bool ClearAllMessages() = 0;
  virtual OUT_MESSAGE_ITEM ShowOutMessage(OUT_MESSAGE_ITEM nParentItem, CExecutionMessage* pMessage) = 0;
  virtual bool ChangeSourceOfMessage(OUT_MESSAGE_ITEM nItem, TSourceOfMessage nSourceOfMessage) = 0;
  virtual bool ChangeTextInOutMessage(OUT_MESSAGE_ITEM nItem, const xtstring& sText, const xtstring& sMsgDescription) = 0;
  virtual bool AddTextInOutMessage(OUT_MESSAGE_ITEM nItem, const xtstring& sText) = 0;
  //
  virtual bool ShowHitEntryInfo(CHitEntryInfo* pHitEntryInfo, bool bRootItemRealised) = 0;
  //
  virtual bool ExecFileGenerated(const xtstring& sIFSName, const CGeneratedFile& cGeneratedFile) = 0;
  //
  virtual void IFSModuleExecuteStart(const xtstring& sIFSName) = 0;
  virtual void IFSModuleExecuteEnd(const xtstring& sIFSName) = 0;
  //
  virtual void ProgressCreateNewProgressObject(size_t nCountOfIntervals) = 0;
  virtual void ProgressReleaseProgressObject() = 0;
  virtual void ProgressStepProgressObject() = 0;
  virtual void SetProgressStatus(unsigned int nProgressValue, unsigned int nProgressMax, const xtstring& sProgressText, const xtstring& sIFSName, const xtstring& sIFSOpenedName) = 0;
};

class XPUBMODEL_EXPORTIMPORT CHitEntryInfo
{
public:
  CHitEntryInfo(const xtstring& sIFSEntityName);
//  CHitEntryInfo(CHitEntryInfo& cEntry);
  CHitEntryInfo(CHitEntryInfo* pEntry);
  ~CHitEntryInfo();

  void DeleteAllHitEntries();

  bool AddChildHitEntryR(CHitEntryInfo* pEntry);
  bool AddChildHitEntryNR(CHitEntryInfo* pEntry);

  CHitEntryInfo* GetFirstChildHitEntryR(){return m_pFirstChildEntryR;};
  CHitEntryInfo* GetFirstChildHitEntryNR(){return m_pFirstChildEntryNR;};

  CHitEntryInfo* GetNextSiblingHitEntryR(){return m_pNextSiblingEntryR;};
  CHitEntryInfo* GetNextSiblingHitEntryNR(){return m_pNextSiblingEntryNR;};

  void SetOutLogMessageItem(OUT_MESSAGE_ITEM nOutMessageItem, LOG_MESSAGE_ITEM nLogMessageItem)
    {m_nOutMsgItem = nOutMessageItem; m_nLogMsgItem = nLogMessageItem;};
  OUT_MESSAGE_ITEM GetOutMsgItem() {return m_nOutMsgItem;};
  LOG_MESSAGE_ITEM GetLogMsgItem() {return m_nLogMsgItem;};

  const xtstring& GetHEI_EntityName()                       {return m_sHEI_EntityName;};
  const xtstring& GetHEI_WPName()                           {return m_sHEI_WPName;};
  const xtstring& GetHEI_WPComment()                        {return m_sHEI_WPComment;};
  const xtstring& GetHEI_TargetInformationSight()           {return m_sHEI_TargetInformationSight;};
  const xtstring& GetHEI_TargetEntityName()                 {return m_sHEI_TargetEntityName;};
  const xtstring& GetHEI_TargetWPName()                     {return m_sHEI_TargetWPName;};
  const xtstring& GetHEI_PlaceHolderToReplace()             {return m_sHEI_PlaceHolderToReplace;};
  TWorkPaperCopyType GetHEI_WorkPaperCopyType()             {return m_nHEI_WorkPaperCopyType;};
  TWorkPaperProcessingMode GetHEI_WorkPaperProcessingMode() {return m_nHEI_WorkPaperProcessingMode;};
  TWorkPaperType GetHEI_WorkPaperType()                     {return m_nHEI_WorkPaperType;};
  TTemplateTarget GetHEI_TartegForTemplate()                {return m_nHEI_TargetForTemplate;};
  TSaveTemplateInFormat GetHEI_SaveTemplateInFormat()       {return m_nHEI_SaveTemplateInFormat;};

  bool SaveToXMLDoc(CXPubMarkUp* pXMLDoc, bool bRootItem = false);
  static void ChangeTextForXMLLog(xtstring& sText);
protected:
  bool AppendHitEntryAsLastSiblingChildEntryR(CHitEntryInfo* pEntry);
  bool AppendHitEntryAsLastSiblingChildEntryNR(CHitEntryInfo* pEntry);

  CHitEntryInfo* m_pFirstChildEntryR;
  CHitEntryInfo* m_pFirstChildEntryNR;
  CHitEntryInfo* m_pNextSiblingEntryR;
  CHitEntryInfo* m_pNextSiblingEntryNR;

  OUT_MESSAGE_ITEM  m_nOutMsgItem;
  LOG_MESSAGE_ITEM  m_nLogMsgItem;

  xtstring m_sHEI_EntityName;

  xtstring                  m_sHEI_WPName;
  xtstring                  m_sHEI_WPComment;
  xtstring                  m_sHEI_TargetInformationSight;
  xtstring                  m_sHEI_TargetEntityName;
  xtstring                  m_sHEI_TargetWPName;
  xtstring                  m_sHEI_PlaceHolderToReplace;
  TWorkPaperCopyType        m_nHEI_WorkPaperCopyType;
  TWorkPaperProcessingMode  m_nHEI_WorkPaperProcessingMode;
  TWorkPaperType            m_nHEI_WorkPaperType;
  TTemplateTarget           m_nHEI_TargetForTemplate;
  TSaveTemplateInFormat     m_nHEI_SaveTemplateInFormat;
};


// folgende Klasse kann in Pre/Post Process Objekten
// und in FrmAddOns / Cnv AddOns benutzt werden,
// um throw(cErrorObject)
class XPUBMODEL_EXPORTIMPORT CErrorObject
{
public:
  CErrorObject()
  {
    m_nNumber = 0;
    m_sMsgKeyword.clear();
    m_sMsgText.clear();
    m_sMsgDescription.clear();
  };
  CErrorObject(int nNumber, const xtstring& sMsgKeyword, const xtstring& sMsgText, const xtstring& sMsgDescription)
  {
    m_nNumber = nNumber;
    m_sMsgKeyword = sMsgKeyword;
    m_sMsgText = sMsgText;
    m_sMsgDescription = sMsgDescription;
  };
  virtual~CErrorObject(){};

  void operator = (const CErrorObject& cErrorObject)
    {
      m_nNumber = cErrorObject.m_nNumber;
      m_sMsgKeyword = cErrorObject.m_sMsgKeyword;
      m_sMsgText = cErrorObject.m_sMsgText;
      m_sMsgDescription = cErrorObject.m_sMsgDescription;
    };

  int       m_nNumber;
  xtstring  m_sMsgKeyword;       // 1. Spalte in GUI
  xtstring  m_sMsgText;          // 2. Spalte in GUI
  xtstring  m_sMsgDescription;
};


#endif // !defined(_MODELDEF_H_)
