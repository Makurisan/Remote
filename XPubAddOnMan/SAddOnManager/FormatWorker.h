#if !defined(_FORMATWORKER_H_)
#define _FORMATWORKER_H_



#include "AddOnManagerExpImp.h"
#include "SModelInd/PropStoneInd.h"
#include "SModelInd/MPModelFieldsInd.h"
#include "SExpression/Expression.h"
//#include "SModelInd/XPubVariant.h"
#include "SErrors/ErrorNumbers.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


#define EMPTY_WORKPAPERCONTENT_XML      _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<WorkPaperContent></WorkPaperContent>\r\n")
#define MAIN_TAG_WORKPAPERCONTENT       _XT("WorkPaperContent")
#define WORKPAPER_1STAGE                _XT("WorkPaper_1Stage")
#define WORKPAPER_2STAGE                _XT("WorkPaper_2Stage")
#define WORKPAPER_DATA                  _XT("Data")

#define WORKPAPER_FORMAT                _XT("Format")
#define WORKPAPER_TYPE                  _XT("Type")
#define WORKPAPER_TEMPLATETARGET        _XT("TemplateTarget")
#define WORKPAPER_SAVETEMPLATEINFORMAT  _XT("SaveTemplateInFormat")
#define WORKPAPER_COPYTYPE              _XT("CopyType")
#define WORKPAPER_ADDONVALUE            _XT("AddOnValue")
#define WORKPAPER_TOKENTOREPLACE        _XT("TokenToReplace")
#define WORKPAPER_TAGPROPERTY           _XT("TagProperty")
#define WORKPAPER_NAME                  _XT("Name")
#define WORKPAPER_COMMENT               _XT("Comment")
#define WORKPAPER_FILESHOWNAME          _XT("FileShowName")
#define WORKPAPER_FOLDER                _XT("Folder")
#define WORKPAPER_FILE                  _XT("File")
#define WORKPAPER_ERMODELNAME           _XT("ERModelName")
#define WORKPAPER_SYMBOLICDBNAME        _XT("SymbolicDBName")
#define WORKPAPER_DBNAME                _XT("DBName")
#define WORKPAPER_PROVIDER              _XT("Provider")


////////////////////////////////////////////////////////////////////////////////
// internal variables
////////////////////////////////////////////////////////////////////////////////
#define FWVAR_TREESTARTACTIONTYP    _XT("TreeStartActiontyp")
#define FWVAR_TREESTARTELEMENTTYP   _XT("TreeStartElementtyp")
#define FWVAR_TEXTFOCUSACTIVE       _XT("TextFocusActive")
#define FWVAR_DOCUMENTBEGINPAGENBR	_XT("DocumentBeginPageNumber")

// TreeStartActiontyp
#define TSAT_NOTSTART               0
#define TSAT_STARTDBLCLK            1
#define TSAT_STARTDRAGDROP          2
// TreeStartElementtyp
#define TSET_UNDEFINED              _XT("Undefined")
#define TSET_TEMPLATE               _XT("Template")
#define TSET_STONE                  _XT("Stone")
#define TSET_FIELD                  _XT("Field")



class CXPubAddOnManager;
class CFormatWorker;
class CChannelToExecutedEntity;
class CChanelToExecutionModule;
class CConvertWorkerProgress;
class CFormatWorkerWrapper;

typedef std::vector<CFormatWorker*>       CFormatWorkers;
typedef CFormatWorkers::iterator          CFormatWorkersIterator;
typedef CFormatWorkers::const_iterator    CFormatWorkersConstIterator;
typedef CFormatWorkers::reverse_iterator  CFormatWorkersReverseIterator;

////////////////////////////////////////////////////////////////////////////////
// Klasse CFormatAddOn ist eine Basis klasse fuer alle Format AddOns
// In jedem Format AddOn muss von dieser Klasse eine Klasse abgeleitet
// Diese Klasse dient nur fuer die Verwaltung den erzeugten CFormatWorker
// Instanzen
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CFormatAddOn
{
public:
  CFormatAddOn(CXPubAddOnManager* pAddOnManager);
  virtual ~CFormatAddOn();

  virtual CFormatWorker* CreateFormatWorker(TWorkPaperType nWorkPaperType) = 0;
  virtual CFormatWorker* CreateFormatWorkerClone(CFormatWorker* pFormatWorker) = 0;
  virtual bool ReleaseFormatWorker(CFormatWorker* pFormatWorker) = 0;

  CXPubAddOnManager* GetAddOnManager(){return m_pAddOnManager;};

  bool DestroyAndRemoveAllFormatWorkers();
protected:
  bool AddFormatWorker(CFormatWorker* pWorker);
  bool RemoveFormatWorker(CFormatWorker* pWorker);

  CXPubAddOnManager*  m_pAddOnManager;
  CFormatWorkers      m_cWorkers;
};



class XPUBADDONMANAGER_EXPORTIMPORT CPropertyBase
{
public:
  CPropertyBase(){};
  virtual ~CPropertyBase(){};

  virtual TXPubProperty GetPropertyGroup() = 0;
  virtual xtstring GetNameOfFreeProperties() = 0;

  virtual bool GetPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty) = 0;
  virtual bool GetPropertyValue(CXPubVariant& cValue, const xtstring& sPropertyName) = 0;

  virtual bool FreePropPresent(const xtstring& sFreePropName) = 0;
};






////////////////////////////////////////////////////////////////////////////////
// Klasse CFormatWorker ist eine Basis klasse fuer alle Format Workers
// In jedem Format AddOn muss von dieser Klasse eine Klasse abgeleitet
// Diese Klasse implementiert die Arbeit mit jeweiligem Datenformat
////////////////////////////////////////////////////////////////////////////////

class XPUBADDONMANAGER_EXPORTIMPORT CFormatWorker : public CVariableValueCallBack
{
  friend class CFormatAddOn;
  friend class CXPubAddOnManager;
public:
  CFormatWorker(TWorkPaperType nWorkPaperType);
  virtual ~CFormatWorker();

  bool CreatePath(const xtstring& sPath, bool bLastElementToo);

  // get / set functions
  virtual xtstring GetProviderName(){return m_sProvider;};
  virtual xtstring GetDBName(){return m_sDBName;};
  virtual xtstring GetFileShowName(){return m_sFileShowName;};
  virtual xtstring GetFileName(){return m_sFile;};
  virtual xtstring GetFolderName(){return m_sFolder;};
  virtual TDataFormatForStoneAndTemplate GetWorkPaperDataFormat(){return m_nWorkPaperFormat;};
  virtual TTemplateTarget GetTemplateTarget(){return m_nTemplateTarget;};
  virtual void SetTemplateTarget(TTemplateTarget nTemplateTarget){m_nTemplateTarget = nTemplateTarget;};
  virtual TSaveTemplateInFormat GetSaveTemplateInFormat(){return m_nSaveTemplateInFormat;};
  virtual void SetSaveTemplateInFormat(TSaveTemplateInFormat nSaveTemplateInFormat){m_nSaveTemplateInFormat = nSaveTemplateInFormat;};
  virtual TWorkPaperType GetWorkPaperType(){return m_nWorkPaperType;};
  virtual xtstring GetAddOnValue(){return m_sAddOnValue;};
  virtual void SetAddOnValue(const xtstring& sAddOnValue){m_sAddOnValue = sAddOnValue;};
  virtual TWorkPaperCopyType GetWorkPaperCopyType(){return m_nWorkPaperCopyType;};
  virtual void SetWorkPaperCopyType(TWorkPaperCopyType nWorkPaperCopyType){m_nWorkPaperCopyType = nWorkPaperCopyType;};
  virtual TWorkPaperProcessingMode GetWorkPaperProcessingMode(){return m_nWorkPaperProcessingMode;};
  virtual void SetWorkPaperProcessingMode(TWorkPaperProcessingMode nWorkPaperProcessingMode){m_nWorkPaperProcessingMode = nWorkPaperProcessingMode;};
  virtual xtstring GetTokenToReplace(){return m_sTokenToReplace;};
  virtual void SetTokenToReplace(const xtstring& sTokenToReplace){m_sTokenToReplace = sTokenToReplace;};
  virtual xtstring GetTagProperty(){return m_sTagProperty;};
  virtual void SetTagProperty(const xtstring& sTagProperty){m_sTagProperty = sTagProperty;}
  virtual xtstring GetWorkPaperName(){return m_sWorkPaperName;};
  virtual void SetWorkPaperName(const xtstring& sWorkPaperName){m_sWorkPaperName = sWorkPaperName;};
  virtual xtstring GetWorkPaperComment(){return m_sWorkPaperComment;};
  virtual void SetWorkPaperComment(const xtstring& sWorkPaperName){m_sWorkPaperComment = sWorkPaperName;};

  // set functions for "SaveToFile"
  virtual void PreSetTemplateFileNameAndFolder(const xtstring& sFolder, const xtstring& sFile, const xtstring& sFileShowName){m_sFolder = sFolder; m_sFile = sFile; m_sFileShowName = sFileShowName;};
  virtual void PreSetSymbolicDBNameAndDBType(const xtstring& sERModelName, const xtstring& sSymbolicDBName, const xtstring& sDBName, const xtstring& sFileShowName){m_sERModelName = sERModelName; m_sSymbolicDBName = sSymbolicDBName; m_sDBName = sDBName; m_sFileShowName = sFileShowName;};

  // get / set WorkPaper
  virtual bool SetWorkPaperContent(const xtstring& sContent) = 0;
  virtual xtstring GetWorkPaperContent() = 0;
  // work functions
  virtual bool SaveInFormatAvailable(TSaveTemplateInFormat nFormat) = 0;
  virtual TSaveTemplateInFormat GetDefaultSaveInFormat() = 0;
  virtual bool InsertWorkPaper(CFormatWorker* pInsertObject,
                               int& nCountOfNotRealised,
                               int& nCountOfRealised,
                               int& nIncrementStateInsertCount) = 0;
  virtual bool ReplaceTextField(const xtstring& sTokenToReplace,
                                const xtstring& sReplaceText) = 0;
  virtual bool ReplaceAreaField(const xtstring& sTokenToReplace,
                                bool bWholeArea) = 0;
  virtual bool ReplaceLinkField(const xtstring& sTokenToReplace,
                                const xtstring& sQuickInfo,
                                const xtstring& sHRef,
                                const xtstring& sTargetFrame) = 0;
  virtual bool ReplaceGraphicField(const xtstring& sTokenToReplace,
                                   const xtstring& sQuickInfo,
                                   const xtstring& sSource) = 0;
  virtual bool FieldDefinedInArea(const xtstring& sAreaToken,
                                  const xtstring& sFieldToken) = 0;
  virtual bool InsertText_Front(const xtstring& sText);
  virtual bool InsertText_Tail(const xtstring& sText);

  // init
  // diese Funktion ist aufgerufen, wenn alle Variablen sind in der Klasse gesetzt
  // (ausser TagProperty)  und vor Arbeit mit dem FormatWorker
  virtual bool Init(){return true;};
  // close
  // diese Funktion ist aufgerufen, wenn mit dem Arbeitsblatt nichts geschehen sollte
  virtual void Close(){};
  // Detach
  // diese Funktion ist kurz vor dem Moment von CFormatAddOn aufgerufen,
  // wenn diese Instanz von Liste entfernt wird
  // nItemCount bedeutet, wie viele Instanzen sind in der Liste mit dieser Instanz
  virtual void DetachFromList(size_t nItemsCount) = 0;

  virtual CXPubAddOnManager* GetAddOnManager() = 0;

  virtual bool IsPropertyFormatWorker() = 0;
  virtual CPropertyBase* GetPropertyBase(){return 0;};

  // end work functions
  virtual bool LastWorkOnWorkPaper() = 0;
  virtual bool SaveWorkPaperToFile(CGeneratedFiles& cGeneratedFiles) = 0;
  virtual bool SaveWorkPaperToDatabase(CGeneratedFiles& cGeneratedFiles) = 0;
  virtual bool SaveWorkPaperToMemory(xtstring& sXMLMemoryContent);
  virtual bool SaveWorkPaperToMemory(CXPubMarkUp* pXMLDoc);
  virtual bool CleanUpWorkPaper(CxtstringVector& arrStringsToRemove) = 0;

protected:
  // related files support
  void AddRelatedFilesToGeneratedFilesList(CGeneratedFiles& cGeneratedFiles);

  // serialize / deserialize - pure WorkPaper Version - 1. stage
  virtual bool ReadFromXMLMemoryContent(CFormatWorkerWrapper* pWrapper, CXPubMarkUp* pXMLDoc);
  virtual bool StartReadFromXMLMemoryContent(CFormatWorkerWrapper* pWrapper, CXPubMarkUp* pXMLDoc);
  virtual bool EndReadFromXMLMemoryContent(CFormatWorkerWrapper* pWrapper, CXPubMarkUp* pXMLDoc);
  virtual bool WriteToXMLMemoryContent(CXPubMarkUp* pXMLDoc);
  virtual bool StartWriteToXMLMemoryContent(CXPubMarkUp* pXMLDoc);
  virtual bool EndWriteToXMLMemoryContent(CXPubMarkUp* pXMLDoc);

protected:
  // copy
  virtual CFormatWorker* Clone() = 0;
  bool BasicClone(CFormatWorker* pNewFormatWorker);

  virtual void SetWorkPaperDataFormat(TDataFormatForStoneAndTemplate nWorkPaperFormat){m_nWorkPaperFormat = nWorkPaperFormat;};

  TDataFormatForStoneAndTemplate  m_nWorkPaperFormat;
  TWorkPaperType                  m_nWorkPaperType;
  TTemplateTarget                 m_nTemplateTarget;
  TSaveTemplateInFormat           m_nSaveTemplateInFormat;
  TWorkPaperCopyType              m_nWorkPaperCopyType;
  TWorkPaperProcessingMode        m_nWorkPaperProcessingMode;
  xtstring                        m_sAddOnValue; // diese Variable ist gueltig, nur wenn m_nWorkPaperCopyType == tCopyType_SpecialStone
  xtstring                        m_sTokenToReplace;
  xtstring                        m_sTagProperty;
  xtstring                        m_sWorkPaperName;
  xtstring                        m_sWorkPaperComment;

  // PreSet variables 
  xtstring m_sFileShowName;
  xtstring m_sFolder;
  xtstring m_sFile;
  xtstring m_sERModelName;
  xtstring m_sSymbolicDBName;
 
//MAK: weiß nicht ob das bleiben soll
  // brauche es bei Serienbrief in FormatWorkerRTF
  xtstring m_sDBName;
  xtstring m_sProvider;


public:
  virtual void SetChannelToExecutionModule(CChannelToExecutedEntity* pChannelCallBack,
                                           CChanelToExecutionModule* pExecModuleCallBack);
  CChannelToExecutedEntity* GetChannelCallBack(){return m_pChannelCallBack;};
  CChanelToExecutionModule* GetExecModuleCallBack(){return m_pExecModuleCallBack;};
  virtual void SetConvertWorkerProgress(CConvertWorkerProgress* pConvertWorkerProgress);
  CConvertWorkerProgress* GetConvertWorkerProgress(){return m_pConvertWorkerProgress;};
protected:
  CChannelToExecutedEntity* m_pChannelCallBack;
  CChanelToExecutionModule* m_pExecModuleCallBack;
  CConvertWorkerProgress*   m_pConvertWorkerProgress;
  xtstring m_sErrorText;
  CErrorObject  m_cError;





public: // nur wegen test - spaeter protected

  // support for property stone (FormatWorker)
  // -->
  bool PropertyGroupPresent(TXPubProperty nPropertyGroupType);

  bool GetPDFPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty);
  bool GetPDFPermissionProperty(CXPubVariant& sPwdOption, long& lPermission);
  
  bool GetOutlookPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty);
  bool GetRTFPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty);
  bool GetExcelPropertyValue(CXPubVariant& cValue,TXPubProperty nProperty);
  bool GetFreePropertyValue(CXPubVariant& cValue, const xtstring& sPropName);
  
  CPropertyBase* GetProperties(TXPubProperty nPropertyGroupType);
  CPropertyBase* GetFreeProperties(const xtstring& sPropertiesName);
  // <--

  bool FormatWorkerPresentButNotPropertyFormatWorker();
  bool TemplateFormatWorkerPresent();
  CFormatWorker* GetFirstTemplateFormatWorker();

  // Format Workers 'Zwischenspeichern'
  size_t CountOfFormatWorkers(){return m_cFormatWorkers.size();};
  bool AddFormatWorker(CFormatWorker* pFormatWorker);
  CFormatWorker* GetFormatWorker(size_t nIndex);

private:
  void RemoveAllFormatWorkers();
  CFormatWorkers      m_cFormatWorkers;





public: // nur wegen test - spaeter protected
  bool ExecuteVisibility(CMPModelKeyInd_ForFrm* pField, CXPubVariant& cResult);
  bool ExecuteVisibility(CMPModelTextInd_ForFrm* pField, CXPubVariant& cResult);
  bool ExecuteVisibility(CMPModelAreaInd_ForFrm* pField, CXPubVariant& cResult);

protected:
  virtual bool GetValue(const xtstring& sName, CXPubVariant& cValue);

private:
  bool Execute(bool bCondition, CXPubVariant& cResult, const CConditionOrAssignment& cCondition);

public:
  size_t CountOfConstants();
  void GetAllConstants(CxtstringVector& arrConstants);
  CMPModelConstantInd_ForFrm* GetConstant(size_t nIndex);
  void AddConstant(CMPModelConstantInd_ForFrm* pModel);
  bool RemoveAllConstants();


  size_t CountOfKeys();
  CMPModelKeyInd_ForFrm* GetKey(size_t nIndex);
  void AddKey(CMPModelKeyInd_ForFrm* pModel);
  bool RemoveAllKeys();

  size_t CountOfTexts();
  CMPModelTextInd_ForFrm* GetText(size_t nIndex);
  void AddText(CMPModelTextInd_ForFrm* pModel);
  bool RemoveAllTexts();

  size_t CountOfAreas();
  CMPModelAreaInd_ForFrm* GetArea(size_t nIndex);
  void AddArea(CMPModelAreaInd_ForFrm* pModel);
  bool RemoveAllAreas();

protected:
  CMPModelConstantInds_ForFrm m_cConstants;
  CMPModelKeyInds_ForFrm      m_cKeys;
  CMPModelTextInds_ForFrm     m_cTexts;
  CMPModelAreaInds_ForFrm     m_cAreas;

  // temp vars - non stream
  CVisibilityPreprocess* m_pVisibilityPreprocess;

////////////////////////////////////////////////////////////////////////////////
// internal variables
////////////////////////////////////////////////////////////////////////////////
public:
  void ResetInternalVariables()
	{
		SetTreeStartActionTyp(TSAT_NOTSTART);SetTreeStartElementtyp(TSET_UNDEFINED);
		SetTextFocusActive(false);SetDocumentBeginPageNumber(1);
	};
  
	int GetTreeStartActionTyp(){return m_nTreeStartActiontyp;};
  void SetTreeStartActionTyp(int n){m_nTreeStartActiontyp = n;};
  xtstring GetTreeStartElementtyp(){return m_sTreeStartElementtyp;};
  void SetTreeStartElementtyp(const xtstring& s){m_sTreeStartElementtyp = s;};
  bool GetTextFocusActive(){return m_bTextFocusActive;};
  void SetTextFocusActive(bool bTextFocusActive){m_bTextFocusActive = bTextFocusActive;};
  int GetDocumentBeginPageNumber(){return m_nDocBeginPageNumber;};
  void SetDocumentBeginPageNumber(int n){m_nDocBeginPageNumber = n;};

  // from
  virtual bool GetValue_CB(const xtstring& sEntityName, const xtstring& sFieldName, CXPubVariant& cValue);
protected:

	int m_nDocBeginPageNumber;				// aktuelle Nummer der ersten Seite des Dokuments, relevant bei geöffnetem Buch
  int m_nTreeStartActiontyp;        // Doppelklick(1), Drag&Drop(2), nicht Startelement(0) 
                                    // TSAT_NOTSTART, TSAT_STARTDBLCLK, TSAT_STARTDRAGDROP
  xtstring  m_sTreeStartElementtyp; // "Undefined", "Template", "Stone", "Field"
                                    // TSET_UNDEFINED, TSET_TEMPLATE, TSET_STONE, TSET_FIELD
  bool m_bTextFocusActive;          //
};



#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_FORMATWORKER_H_)

