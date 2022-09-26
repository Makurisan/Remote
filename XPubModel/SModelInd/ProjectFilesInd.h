#if !defined(_PROJECTFILESIND_H_)
#define _PROJECTFILESIND_H_



#include "ModelExpImp.h"


#include "ModelDef.h"
#include "ModelInd.h"
#include "ERModelInd.h"
#include "IFSEntityInd.h"
#include "MPModelFieldsInd.h"
#include "MPModelERModelInd.h"
#include "MPModelIFSInd.h"
#include "MPModelStoneInd.h"
#include "MPModelTemplateInd.h"
#include "MPModelPythonInd.h"
#include "PropStoneInd.h"
#include "DBStoneInd.h"
#include "DBTemplateInd.h"
#include "Messages.h"
#include "Constants.h"
#include "OpenedModules.h"
#include "ZoomFactor.h"
#include "DBDefinitions.h"
#include "ExternalEditors.h"
#include "XMLModelClasses.h"
#include "DTNSupport.h"
#include "Sxml/XMLTagNames.h"
#include "SSyncEng/SyncEngine.h"
#include "SMD5/MD5.h"

#include "ProjectFiles_ER.h"
#include "ProjectFiles_IFS.h"
#include "ProjectFiles_Stone.h"
#include "ProjectFiles_Template.h"
#include "ProjectFiles_Python.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32



class CProjectFilesInd;
class CDTNSupport;
class CGS_DateTimeNumber;


/////////////////////////////////////////////////////////////////////////////
// CProjectFilesInd 

class XPUBMODEL_EXPORTIMPORT CProjectFilesInd
{
public:
  xtstring GetMD5Sum_XML(){return m_sMD5Sum_XML;};
  void GetTime_XML(time_t& timeChanged){timeChanged = timeChanged_XML;};
protected:
  void SetMD5Sum_XML(const xtstring& sMD5Sum){m_sMD5Sum_XML = sMD5Sum;};
  void SetTime_XML(time_t timeChanged){timeChanged_XML = timeChanged;};
  xtstring m_sMD5Sum_XML;
  time_t timeChanged_XML;

  // Construction
public:
  CProjectFilesInd();
  virtual ~CProjectFilesInd();
  void SetCallBacks(CDirectoryCheckerCallBack* pDirChecker,
                    COutMessageContainerCallBack* pOutMsgContainer);
  CDirectoryCheckerCallBack*    GetDirectoryChecker(){return m_pDirChecker;};
  COutMessageContainerCallBack* GetOutMessageContainter(){return m_pOutMsgContainer;};

  const xtstring& GetXMLFileContent(){return m_sXMLDocText;};

// Attributes
public:

// Properties function
public:
  // base properties
  xtstring GetProjectName(){return m_sProjectName;};
  void SetProjectName(const xtstring& sProjectName){m_sProjectName = sProjectName;};
  xtstring GetProjectSubject(){return m_sProjectSubject;};
  void SetProjectSubject(const xtstring& sProjectSubject){m_sProjectSubject = sProjectSubject;};
  xtstring GetProjectVersion(){return m_sProjectVersion;};
  void SetProjectVersion(const xtstring& sProjectVersion){m_sProjectVersion = sProjectVersion;};
  xtstring GetTestFolder(){return m_sTestFolder;};
  void SetTestFolder(const xtstring& sTestFolder){m_sTestFolder = sTestFolder;};

  xtstring GetProjectFolder(){return m_sProjectFolder;};
  void SetProjectFolder(const xtstring& sProjectFolder){m_sProjectFolder = sProjectFolder;};
  xtstring GetProjectFileName(){return m_sProjectFileName;};
  void SetProjectFileName(const xtstring& sProjectFileName){m_sProjectFileName = sProjectFileName;};
  xtstring GetProjectBaseFileName(){return m_sProjectBaseFileName;};
  void SetProjectBaseFileName(const xtstring& sProjectBaseFileName){m_sProjectBaseFileName = sProjectBaseFileName;};
  xtstring GetProjectFullFileName(){return m_sProjectFullFileName;};
  void SetProjectFullFileName(const xtstring& sProjectFullFileName){m_sProjectFullFileName = sProjectFullFileName;};

  // date / time / number settings
  TLongDateFormat GetLongDateFormat(){return m_nLongDateFormat;};
  void SetLongDateFormat(TLongDateFormat nLongDateFormat){m_nLongDateFormat = nLongDateFormat;m_cDTNSupport.SetLongDateFormat(m_nLongDateFormat);};
  bool GetYearWithCentury(){return m_bYearWithCentury;};
  void SetYearWithCentury(bool bYearWithCentury){m_bYearWithCentury = bYearWithCentury;m_cDTNSupport.SetYearWithCentury(m_bYearWithCentury);};
  xtstring GetDateSeparator(){return m_sDateSeparator;};
  void SetDateSeparator(const xtstring& sDateSeparator){m_sDateSeparator = sDateSeparator;m_cDTNSupport.SetDateSeparator(m_sDateSeparator);};
  xtstring GetTimeSeparator(){return m_sTimeSeparator;};
  void SetTimeSeparator(const xtstring& sTimeSeparator){m_sTimeSeparator = sTimeSeparator;m_cDTNSupport.SetTimeSeparator(m_sTimeSeparator);};
  bool GetUseSymbolsFromSystem(){return m_bUseSymbolsFromSystem;};
  void SetUseSymbolsFromSystem(bool bUseSymbolsFromSystem){m_bUseSymbolsFromSystem = bUseSymbolsFromSystem;m_cDTNSupport.SetUseSymbolsFromSystem(m_bUseSymbolsFromSystem);};
  xtstring GetDecimalSymbol(){return m_sDecimalSymbol;};
  void SetDecimalSymbol(const xtstring& sDecimalSymbol){m_sDecimalSymbol = sDecimalSymbol;m_cDTNSupport.SetDecimalSymbol(m_sDecimalSymbol);};
  xtstring GetDigitGroupingSymbol(){return m_sDigitGroupingSymbol;};
  void SetDigitGroupingSymbol(const xtstring& sDigitGroupingSymbol){m_sDigitGroupingSymbol = sDigitGroupingSymbol;m_cDTNSupport.SetDigitGroupingSymbol(m_sDigitGroupingSymbol);};
  bool GetUseCountOfDigitsAfterDecimal(){return m_bUseCountOfDigitsAfterDecimal;};
  void SetUseCountOfDigitsAfterDecimal(bool bUseCountOfDigitsAfterDecimal){m_bUseCountOfDigitsAfterDecimal = bUseCountOfDigitsAfterDecimal;m_cDTNSupport.SetUseCountOfDigitsAfterDecimal(m_bUseCountOfDigitsAfterDecimal);};
  int GetCountOfDigitsAfterDecimal(){return m_nCountOfDigitsAfterDecimal;};
  void SetCountOfDigitsAfterDecimal(int nCountOfDigitsAfterDecimal){m_nCountOfDigitsAfterDecimal = nCountOfDigitsAfterDecimal;m_cDTNSupport.SetCountOfDigitsAfterDecimal(m_nCountOfDigitsAfterDecimal);};
    // gui - opened modules
    const COpenedModules& GetOpenedModules(){return m_cOpenedModules;};
    bool SetOpenedModules(COpenedModules* pOpenedModules){return m_cOpenedModules.CopyFrom(pOpenedModules);};
    // gui - zoom factors
    long GetZoomFactorER(const xtstring& sModuleName){return m_cZoomFactorsER.GetZoomFactor(sModuleName);};
    bool SetZoomFactorER(const xtstring& sModuleName, long nZoomFactor){return m_cZoomFactorsER.SetZoomFactor(sModuleName, nZoomFactor);};
    long GetZoomFactorIFS(const xtstring& sModuleName){return m_cZoomFactorsIFS.GetZoomFactor(sModuleName);};
    bool SetZoomFactorIFS(const xtstring& sModuleName, long nZoomFactor){return m_cZoomFactorsIFS.SetZoomFactor(sModuleName, nZoomFactor);};
    // gui - last filter
    xtstring GetLastFilterIFS(){return m_sLastFilterIFS;};
    void SetLastFilterIFS(const xtstring& sLastFilterIFS){m_sLastFilterIFS = sLastFilterIFS;};
    // gui - last ifs for exec
    xtstring GetLastIFSForExec(){return m_sLastIFSForExec;};
    void SetLastIFSForExec(const xtstring& sLastIFSForExec){m_sLastIFSForExec = sLastIFSForExec;};
  // constants properties
  CProjectConstantArray* GetTestConstantsPtr(){return &m_cTestProjectConstants;};
  const CProjectConstantArray& GetTestConstants(){return m_cTestProjectConstants;};
  bool SetTestConstants(const CProjectConstantArray& cConstants){return m_cTestProjectConstants.CopyFrom(&cConstants);};
  CProjectConstantArray* GetProductionConstantsPtr(){return &m_cProductionProjectConstants;};
  const CProjectConstantArray& GetProductionConstants(){return m_cProductionProjectConstants;};
  bool SetProductionConstants(const CProjectConstantArray& cConstants){return m_cProductionProjectConstants.CopyFrom(&cConstants);};
  // DB Definitions
  CDBDefinitions* GetDBDefinitions(){return &m_cDBDefinitions;};
  void CopyDBDefinitions(CDBDefinitions* pDBDefinitions, bool& bDefsChanged){m_cDBDefinitions.CopyFrom(pDBDefinitions, bDefsChanged);};
  void AssignDBDefinitions(CDBDefinitions* pDBDefinitions){m_cDBDefinitions.AssignFrom(pDBDefinitions);};
  bool GetAllSymbolicDBNames(CxtstringVector& cNames){return m_cDBDefinitions.GetAllSymbolicDBNames(cNames);};
  CDBDefinition* GetDBDefinition(const xtstring& sDBDefinitionName){return m_cDBDefinitions.Get(sDBDefinitionName);};
  // external editors
  CExternalEditors* GetExternalEditors(){return &m_cExternalEditors;};
  void CopyExternalEditors(CExternalEditors* pExternalEditors, bool& bEditorsChanged){m_cExternalEditors.CopyFrom(pExternalEditors, bEditorsChanged);};
  void AssignExternalEditors(CExternalEditors* pExternalEditors){m_cExternalEditors.AssignFrom(pExternalEditors);};

  bool GetUseTestConstantsForProduction(){return m_bUseTestConstantsForProduction;};
  void SetUseTestConstantsForProduction(bool bUse){m_bUseTestConstantsForProduction = bUse;};

  TPreviewType GetPreviewType(){return m_nPreviewType;};
  void SetPreviewType(TPreviewType nPreviewType){m_nPreviewType = nPreviewType;};
  xtstring GetTranslateCodeFile(){return m_sTranslateCodeFile;};
  void SetTranslateCodeFile(const xtstring& sTranslateCodeFile){m_sTranslateCodeFile = sTranslateCodeFile;};

  bool GetIFSStruct(TIFSTypeForThirdParty nType, xtstring& sXMLDoc);
  bool GetEntitiesStruct(TIFSTypeForThirdParty nType, xtstring& sXMLDoc);
  bool GetEntitiesStruct(TIFSTypeForThirdParty nType, const xtstring& sIFSName, xtstring& sXMLDoc);

protected:
  bool MakeOpenedEntitiesPath(CProjectFileIFS* pProjFile, const xtstring& sEntityName, COpenedItemsContainer* pStruct);
// Operations

public:
  const CDTNSupport& GetDateTimeNumberSupport(){return m_cDTNSupport;};

  virtual void SetForSettings(CProjectFilesInd& cPFiles);
  virtual void SetFromSettings(CProjectFilesInd& cPFiles);

  xtstring GetFileBaseName(TProjectFileType nProjectFileType);
  xtstring GetFileExtension(TProjectFileType nProjectFileType);

  bool ExistThisModuleName(TProjectFileType nProjectFileType, const xtstring& sModuleName);
  bool ExistThisFileBaseName(TProjectFileType nProjectFileType, const xtstring& sBaseFileName);
  bool ExistThisFileBaseNameAsFile(TProjectFileType nProjectFileType, const xtstring& sBaseFileName);
  bool GetModuleName(TProjectFileType nProjectFileType, const xtstring& sFileName, xtstring& sModuleName);

  size_t GetCountOfERFiles(){return m_cFilesER.size();};
  size_t GetCountOfIFSFiles(){return m_cFilesIFS.size();};
  size_t GetCountOfStoneFiles(){return m_cFilesStone.size();};
  size_t GetCountOfTemplateFiles(){return m_cFilesTemplate.size();};
  size_t GetCountOfPythonModuleFiles(){return m_cFilesPythonModule.size();};

  CProjectFileER*           GetERFile(size_t nIndex);
  CProjectFileER*           GetERFile(LPCXTCHAR pName);
  CProjectFileIFS*          GetIFSFile(size_t nIndex);
  CProjectFileIFS*          GetIFSFile(LPCXTCHAR pName);
  CProjectFileStone*        GetStoneFile(size_t nIndex);
  CProjectFileStone*        GetStoneFile(LPCXTCHAR pName);
  CProjectFileTemplate*     GetTemplateFile(size_t nIndex);
  CProjectFileTemplate*     GetTemplateFile(LPCXTCHAR pName);
  CProjectFilePythonModule* GetPythonModuleFile(size_t nIndex);
  CProjectFilePythonModule* GetPythonModuleFile(LPCXTCHAR pName);

  bool RemoveProjectFileER(size_t nIndex);
  bool RemoveProjectFileIFS(size_t nIndex);
  bool RemoveProjectFileStone(size_t nIndex);
  bool RemoveProjectFileTemplate(size_t nIndex);
  bool RemoveProjectFilePythonModule(size_t nIndex);

  virtual bool OnDocumentNew();
  // bCreateObjectsImmediately - TRUE - Objekte sind wegen SyncEng erzeugt / FALSE - Objekte sind nicht erzeugt
  virtual bool OnDocumentOpen(LPCXTCHAR lpszPathName, bool bCreateObjectsImmediately);
  virtual bool OnDocumentSave(LPCXTCHAR lpszPathName);
  virtual void OnDocumentClose();

protected:
  xtstring  m_sLastOpenedOrSavedProjectFile;
  bool ReadGUIProperties();
  bool WriteGUIProperties();

  virtual void ShowAllInProjectWorkBar(){};
  void RemoveAllFiles();

// Implementation
public:

  void SetBaseNameForER(const xtstring& sBaseName){m_sBaseNameER = sBaseName;};
  const xtstring& GetBaseNameForER(){return m_sBaseNameER;};
  void SetBaseNameForIFS(const xtstring& sBaseName){m_sBaseNameIFS = sBaseName;};
  const xtstring& GetBaseNameForIFS(){return m_sBaseNameIFS;};
  void SetBaseNameForStone(const xtstring& sBaseName){m_sBaseNameStone = sBaseName;};
  const xtstring& GetBaseNameForStone(){return m_sBaseNameStone;};
  void SetBaseNameForTemplate(const xtstring& sBaseName){m_sBaseNameTemplate = sBaseName;};
  const xtstring& GetBaseNameForTemplate(){return m_sBaseNameTemplate;};
  void SetBaseNameForPythonModule(const xtstring& sBaseName){m_sBaseNamePythonModule = sBaseName;};
  const xtstring& GetBaseNameForPythonModule(){return m_sBaseNamePythonModule;};

  void SetBaseFileNameForER(const xtstring& sBaseFileName){m_sBaseFileNameER = sBaseFileName;};
  const xtstring& GetBaseFileNameForER(){return m_sBaseFileNameER;};
  void SetBaseFileNameForIFS(const xtstring& sBaseFileName){m_sBaseFileNameIFS = sBaseFileName;};
  const xtstring& GetBaseFileNameForIFS(){return m_sBaseFileNameIFS;};
  void SetBaseFileNameForStone(const xtstring& sBaseFileName){m_sBaseFileNameStone = sBaseFileName;};
  const xtstring& GetBaseFileNameForStone(){return m_sBaseFileNameStone;};
  void SetBaseFileNameForTemplate(const xtstring& sBaseFileName){m_sBaseFileNameTemplate = sBaseFileName;};
  const xtstring& GetBaseFileNameForTemplate(){return m_sBaseFileNameTemplate;};
  void SetBaseFileNameForPythonModule(const xtstring& sBaseFileName){m_sBaseFileNamePythonModule = sBaseFileName;};
  const xtstring& GetBaseFileNameForPythonModule(){return m_sBaseFileNamePythonModule;};

  void ScanAfterAddOrDelWorkPaper();

  static bool CreateFromScratch_ProjectFile(const xtstring& sProjectFolder, const xtstring& sFileName, 	unsigned long nDataSize, unsigned char *data, time_t timeChanged);
  static bool CreateFromScratch_ERFile(const xtstring& sProjectFolder, const xtstring& sFileName, unsigned long nDataSize, unsigned char *data, time_t timeChanged);
  static bool CreateFromScratch_IFSFile(const xtstring& sProjectFolder, const xtstring& sFileName, unsigned long nDataSize, unsigned char *data, time_t timeChanged);
  static bool CreateFromScratch_StoneFile(const xtstring& sProjectFolder, const xtstring& sFileName, unsigned long nDataSize, unsigned char *data, time_t timeChanged);
  static bool CreateFromScratch_TemplateFile(const xtstring& sProjectFolder, const xtstring& sFileName, unsigned long nDataSize, unsigned char *data, time_t timeChanged);
  static bool CreateFromScratch_PythonFile(const xtstring& sProjectFolder, const xtstring& sFileName, unsigned long nDataSize, unsigned char *data, time_t timeChanged);

protected:
  CDTNSupport   m_cDTNSupport;
  // project (document) file
  xtstring       m_sXMLDocText;
  // project files
  CFileArrayER            m_cFilesER;
  CFileArrayIFS           m_cFilesIFS;
  CFileArrayStone         m_cFilesStone;
  CFileArrayTemplate      m_cFilesTemplate;
  CFileArrayPythonModule  m_cFilesPythonModule;

  xtstring  m_sProjectFolder;       // diese Variable ist niergendwo abgespeichert
  xtstring  m_sProjectFileName;     // diese Variable ist niergendwo abgespeichert
  xtstring  m_sProjectBaseFileName; // diese Variable ist niergendwo abgespeichert
  xtstring  m_sProjectFullFileName; // diese Variable ist niergendwo abgespeichert
  bool      m_bProjectFromZIPFile;  // diese Variable ist niergendwo abgespeichert

  // base properties
  xtstring m_sProjectName;
  xtstring m_sProjectSubject;
  xtstring m_sProjectVersion;
  xtstring m_sTestFolder;

	// error log
  xtstring m_sErrorLog;

  // date / time / number settings
  TLongDateFormat m_nLongDateFormat;
  bool            m_bYearWithCentury; // false-04 / true-2004
  xtstring        m_sDateSeparator;
  xtstring        m_sTimeSeparator;
  bool            m_bUseSymbolsFromSystem;
  xtstring        m_sDecimalSymbol;
  xtstring        m_sDigitGroupingSymbol;
  bool            m_bUseCountOfDigitsAfterDecimal;
  int             m_nCountOfDigitsAfterDecimal;

    // gui - opened modules
    COpenedModules  m_cOpenedModules;
    // gui - zoom factors
    CZoomFactors    m_cZoomFactorsER;
    CZoomFactors    m_cZoomFactorsIFS;
    // gui - last filter
    xtstring        m_sLastFilterIFS;
    // gui - last ifs for exec
    xtstring        m_sLastIFSForExec;

  // constant properties
  CProjectConstantArray m_cTestProjectConstants;
  CProjectConstantArray m_cProductionProjectConstants;
  bool            m_bUseTestConstantsForProduction;

  // DB Definitions
  CDBDefinitions  m_cDBDefinitions;

  // external editors
  CExternalEditors  m_cExternalEditors;

  // extras
  TPreviewType    m_nPreviewType;
  xtstring        m_sTranslateCodeFile;

  CDirectoryCheckerCallBack*    m_pDirChecker;
  COutMessageContainerCallBack* m_pOutMsgContainer;
  CGS_DateTimeNumber*           m_pGlobalSettings;

  // base names for modules
  xtstring m_sBaseNameER;
  xtstring m_sBaseNameIFS;
  xtstring m_sBaseNameStone;
  xtstring m_sBaseNameTemplate;
  xtstring m_sBaseNamePythonModule;
  xtstring m_sBaseFileNameER;
  xtstring m_sBaseFileNameIFS;
  xtstring m_sBaseFileNameStone;
  xtstring m_sBaseFileNameTemplate;
  xtstring m_sBaseFileNamePythonModule;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_PROJECTFILESIND_H_)
