#if !defined(_FORMATWORKERWRAPPERS_H_)
#define _FORMATWORKERWRAPPERS_H_



#include "AddOnManagerExpImp.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32



//#define FIELD_TOKEN_START           _XT("<<--!")
//#define FIELD_TOKEN_END             _XT("!-->>")
#define FIELD_TOKEN_START           _XT("\\<field name=\\\"")
#define FIELD_TOKEN_END             _XT("\\\" type=\\\"field\\\" \\>")

#define STONE_TOKEN_FRONT_START     _XT("\\<field name=\\\"")
#define STONE_TOKEN_FRONT_END       _XT("\\\" type=\\\"stone\\\" start \\>")
#define STONE_TOKEN_TAIL_START      _XT("\\<field name=\\\"")
#define STONE_TOKEN_TAIL_END        _XT("\\\" type=\\\"stone\\\" end \\>")

#define TEMPLATE_TOKEN_FRONT_START  _XT("\\<field name=\\\"")
#define TEMPLATE_TOKEN_FRONT_END    _XT("\\\" type=\\\"template\\\" start \\>")
#define TEMPLATE_TOKEN_TAIL_START   _XT("\\<field name=\\\"")
#define TEMPLATE_TOKEN_TAIL_END     _XT("\\\" type=\\\"template\\\" end \\>")



class CXPubAddOnManager;
class CFormatWorker;
class CFormatWorkerWrapper_Root;


class CFormatWorkerWrapper_Field;
class CFormatWorkerWrapper_Stone;
class CFormatWorkerWrapper_Template;

#define TSOFWW_NEUTRAL_EMPTY                              0x0001
#define TSOFWW_SYNCHRON_WHITE                             0x0002
#define TSOFWW_CHANGEDVALUENOTINPANEL_YELLOW              0x0004
#define TSOFWW_CHANGEDVALUEINPANEL_GREEN                  0x0008
#define TSOFWW_ERROR_RED                                  0x0010
#define TSOFWW_SYNCHRON_WHITE_BROKENLINK                  0x0020
#define TSOFWW_CHANGEDVALUENOTINPANEL_YELLOW_BROKENLINK   0x0040
#define TSOFWW_CHANGEDVALUEINPANEL_GREEN_BROKENLINK       0x0080
#define TSOFWW_ERROR_RED_BROKENLINK                       0x0100
#define TSOFWW_PARENTKEY                                  0x0200
#define TSOFWW_PARENTATTENTION                            0x0400
#define TSOFWW_PARENTERROR                                0x0800


  typedef enum TStatusOfFWWrapper
  {
    tSOFWW_FirstDummy = -1,
    tSOFWW_Neutral_Empty,
    tSOFWW_Synchron_White,
    tSOFWW_ChangedValueNotInPanel_Yellow,
    tSOFWW_ChangedValueInPanel_Green,
    tSOFWW_Error_Red,
    tSOFWW_Synchron_White_BrokenLink,
    tSOFWW_ChangedValueNotInPanel_Yellow_BrokenLink,
    tSOFWW_ChangedValueInPanel_Green_BrokenLink,
    tSOFWW_Error_Red_BrokenLink,
    tSOFWW_ParentKey,
    tSOFWW_ParentAttention,
    tSOFWW_ParentError,
    tSOFWW_LastDummy,
    tSOFWW_Default = tSOFWW_Neutral_Empty
  };


typedef std::vector<CFormatWorkerWrapper_Field*>    CFieldWrappers;
typedef CFieldWrappers::iterator                    CFieldWrappersIterator;
typedef CFieldWrappers::const_iterator              CFieldWrappersConstIterator;
typedef CFieldWrappers::reverse_iterator            CFieldWrappersReverseIterator;

typedef std::vector<CFormatWorkerWrapper_Stone*>    CStoneWrappers;
typedef CStoneWrappers::iterator                    CStoneWrappersIterator;
typedef CStoneWrappers::const_iterator              CStoneWrappersConstIterator;
typedef CStoneWrappers::reverse_iterator            CStoneWrappersReverseIterator;

typedef std::vector<CFormatWorkerWrapper_Template*> CTemplateWrappers;
typedef CTemplateWrappers::iterator                 CTemplateWrappersIterator;
typedef CTemplateWrappers::const_iterator           CTemplateWrappersConstIterator;
typedef CTemplateWrappers::reverse_iterator         CTemplateWrappersReverseIterator;


typedef std::map<xtstring, xtstring>    CNewValues;
typedef CNewValues::iterator            CNewValuesIterator;
typedef CNewValues::const_iterator      CNewValuesConstIterator;
typedef CNewValues::reverse_iterator    CNewValuesReverseIterator;
typedef pair<xtstring, xtstring>        CNewValuesPair;
typedef pair<CNewValuesIterator, bool>  CNewValuesInsert;

typedef std::map<xtstring, unsigned long> CCRC32s;
typedef CCRC32s::iterator                 CCRC32sIterator;
typedef CCRC32s::const_iterator           CCRC32sConstIterator;
typedef CCRC32s::reverse_iterator         CCRC32sReverseIterator;

typedef std::map<xtstring, TStatusOfFWWrapper>  CNewStatus;
typedef CNewStatus::iterator                    CNewStatusIterator;
typedef CNewStatus::const_iterator              CNewStatusConstIterator;
typedef CNewStatus::reverse_iterator            CNewStatusReverseIterator;
typedef pair<xtstring, int>                     CNewStatusPair;
typedef pair<CNewStatusIterator, bool>          CNewStatusInsert;



#define ALL_WORKPAPERS_XML                    _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<AllWorkPapers></AllWorkPapers>\r\n")
#define MAIN_TAG_WORKPAPERS                   _XT("AllWorkPapers")
#define PROP_XMLVERSION                       _XT("XMLVersion")
#define TOPTEMPLATE                           _XT("TopTemplate")
#define TOPTEMPLATE_INDEX                     _XT("Index")
#define TOPTEMPLATE_NAME                      _XT("Name")
#define TOPTEMPLATE_COMMENT                   _XT("Comment")
#define TOPTEMPLATE_FILESHOWNAME              _XT("FileShowName")

#define EMPTY_WORKPAPER_WRAPPER_CONTENT_XML   _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<WorkPaperWrapperContent></WorkPaperWrapperContent>\r\n")
#define MAIN_TAG_WORKPAPER_WRAPPER_CONTENT    _XT("WorkPaperWrapperContent")
#define WORKPAPERWRAPPER_TEMPLATE             _XT("WorkPaperWrapper_Template")
#define WORKPAPERWRAPPER_STONE                _XT("WorkPaperWrapper_Stone")
#define WORKPAPERWRAPPER_FIELD                _XT("WorkPaperWrapper_Field")
#define WORKPAPERWRAPPER_FIELD_KEY            _XT("WorkPaperWrapper_Field_Key")
#define WORKPAPERWRAPPER_FIELD_TEXT           _XT("WorkPaperWrapper_Field_Text")
#define WORKPAPERWRAPPER_FIELD_AREA           _XT("WorkPaperWrapper_Field_Area")
#define WORKPAPERWRAPPER_BASE                 _XT("WorkPaperWrapper_Base")
#define WORKPAPERWRAPPER_ELEMENTID            _XT("ElementID")
#define WORKPAPERWRAPPER_CLIENTADDRESS        _XT("ClientAddress")
#define WORKPAPERWRAPPER_GUITREESTATUS        _XT("GUITreeStatus")

// elementid defines
#define ELEMENTID_DELIMITER                 _XT('/')
// key words
#define KEYWORD_READONLY                    _XT("readonly")

// Free Props defines
#define PROP_NAME_NAME                      _XT("Name")
#define PROP_NAME_SHOW                      _XT("Show")
#define PROP_NAME_SHOWCHILDS                _XT("ShowChilds")
#define PROP_NAME_COLCOUNT                  _XT("ColumnCount")
#define PROP_NAME_COLTEXT                   _XT("Column%ld")
#define PROP_NAME_COLMASK                   _XT("Column%ldMask")
#define PROP_NAME_COLELEMENTID              _XT("Column%ldElementID")
#define PROP_NAME_COLPICTURE                _XT("Column%ldPicture")
#define PROP_NAME_COLTAG                    _XT("Column%ldTag")
#define PROP_NAME_COLWIDTH                  _XT("Column%ldWidth")
#define PROP_NAME_COLICON                   _XT("Column%ldIcon")
#define PROP_NAME_COLALIGN                  _XT("Column%ldAlignment")
#define PROP_NAME_COLBUTTON                 _XT("Column%ldButton")
#define PROP_NAME_COLENABLE                 _XT("Column%ldEnable")

#define PROP_NAME_EXEC_ID                   _XT("ActionID")
#define PROP_NAME_EXEC_SERVER               _XT("Server")
#define PROP_NAME_EXEC_PORT                 _XT("Port")
#define PROP_NAME_EXEC_USER                 _XT("User")
#define PROP_NAME_EXEC_PASSWORD             _XT("Password")
#define PROP_NAME_EXEC_PROJECT              _XT("Project")
#define PROP_NAME_EXEC_IFS                  _XT("IFS")
#define PROP_NAME_EXEC_PARAMETER            _XT("Parameter")
#define PROP_NAME_EXEC_CREATELOGFILE        _XT("CreateLogFile")
#define PROP_NAME_EXEC_UPREMOTEFILE         _XT("UploadRemoteFile")
#define PROP_NAME_EXEC_UPLOCALFILE          _XT("UploadLocalFile")
#define PROP_NAME_EXEC_DOWNREMOTEFILE       _XT("DownloadRemoteFile")
#define PROP_NAME_EXEC_DOWNLOCALFILE        _XT("DownloadLocalFile")
#define PROP_NAME_EXEC_DYNAMICEXECUTION     _XT("DynamicExecution")
#define PROP_NAME_VALUEANDSTATUS_ELEMENTID  _XT("ElementID")
#define PROP_NAME_VALUEANDSTATUS_STATUS     _XT("Status")
#define PROP_NAME_VALUEANDSTATUS_VALUE      _XT("Value")
#define PROP_NAME_VALUEANDSTATUS_OLDVALUE   _XT("OldValue")
#define PROP_NAME_VALUEANDSTATUS_LANGUAGE   _XT("Language")
#define PROP_NAME_VALUEANDSTATUS_TYPE       _XT("Type")
#define PROP_NAME_VALUEANDSTATUS_TYPE_TEMPLATE  _XT("Template")
#define PROP_NAME_VALUEANDSTATUS_TYPE_STONE     _XT("Stone")
#define PROP_NAME_VALUEANDSTATUS_TYPE_FIELD     _XT("Field")
#define PROP_NAME_REPORTFILTER_DESCRIPTION  _XT("Description%ld%s")
#define PROP_NAME_REPORTFILTER_PARAMETER    _XT("Parameter%ld")
#define PROP_NAME_PICTURE_SYMBOLICNAME      _XT("SymbolicName")

#define PROP_NAMETEXT_SHOW                  _XT("Show")
#define PROP_NAMETEXT_SHOWCHILDS            _XT("ShowChilds")
#define PROP_NAMETEXT_FIELDS                _XT("Fields")
#define PROP_NAMETEXT_FLATHEADER            _XT("FlatHeader")
#define PROP_NAMETEXT_EXECACTIONREMOTE      _XT("ExecuteActionRemote")
#define PROP_NAMETEXT_EXECACTIONLOCAL       _XT("ExecuteActionLocal")
#define PROP_NAMETEXT_REPORTFILTER          _XT("ReportFilter")
#define PROP_NAMETEXT_VALUEANDSTATUS        _XT("ValueAndStatus")
#define PROP_NAMETEXT_PICTURE               _XT("Picture")

#define PROP_NAMETEXT_GLOBALS               _XT("Globals")
// Spezial Names at Indesign
#define PROP_INDESIGN_BOOKNAME							_XT("BookName")
#define PROP_INDESIGN_BOOKDIR							  _XT("BookDirectory")
#define PROP_INDESIGN_PROJEKTNAME					  _XT("ProjectName")

#define PROP_ACTIONIDTEXT_WRITEREPORT       _XT("WriteReport")
#define PROP_ACTIONIDTEXT_UPDATEDATA        _XT("UpdateData")
#define PROP_ACTIONIDTEXT_GETALLUSED        _XT("GetAllUsed")
#define PROP_ACTIONIDTEXT_GETPICTURE        _XT("GetPicture")
#define PROP_ACTIONIDTEXT_GETPICTUREINFO    _XT("GetPictureInfo")

#define PROP_NAME_EXEC_OPENDOCUMENT			_XT("PreOpenDocument")
#define PROP_NAME_EXEC_OPENBOOK					_XT("PreOpenBook")
#define PROP_NAME_EXEC_DOCUMENTTEMPLATE _XT("PreTemplateDocument")

#define TEXT_TEMPLATE_FOR_FIELD             _XT("$$Field")
#define COLUMN_TEMPLATE_FOR_FLAT            _XT("$$COLUMN%02ld")





class CWrapperObjectStatus
{
public:
  CWrapperObjectStatus(){};
  CWrapperObjectStatus(const CWrapperObjectStatus& cObject){Copy(cObject);};
  ~CWrapperObjectStatus(){};

  void operator =(const CWrapperObjectStatus& cObject){Copy(cObject);};
  void Copy(const CWrapperObjectStatus& cObject)
  {
    sElementID = cObject.sElementID;
    sOldValue = cObject.sOldValue;
    sNewValue = cObject.sNewValue;
    nStatus = cObject.nStatus;
  };

  xtstring            sElementID;
  xtstring            sOldValue;
  xtstring            sNewValue;
  TStatusOfFWWrapper  nStatus;
};

typedef std::vector<CWrapperObjectStatus>     CWrapperObjectStatusVector;
typedef CWrapperObjectStatusVector::iterator  CWrapperObjectStatusVectorIterator;

typedef std::map<xtstring, CWrapperObjectStatus, less<xtstring> > CWrapperObjectStatusMap;
typedef CWrapperObjectStatusMap::iterator                         CWrapperObjectStatusMapIterator;

class XPUBADDONMANAGER_EXPORTIMPORT CFormatWorkerWrapper
{
  friend class CXPubAddOnManager;
public:
  typedef enum TTypeOfFWWrapper
  {
    tTOFWW_FirstDummy = 0,
    tTOFWW_Template,
    tTOFWW_Stone,
    tTOFWW_Field,
    tTOFWW_LastDummy
  };
  typedef enum TGUITreeStatus
  {
    tGTS_FirstDummy = 0,
    tGTS_Collapsed,
    tGTS_Expanded,
    tGTS_LastDummy,
    tGTS_Default = tGTS_Collapsed
  };

  CFormatWorkerWrapper(TTypeOfFWWrapper nTypeOfObject, CFormatWorkerWrapper_Root* pRoot, CFormatWorkerWrapper* pParent);
  CFormatWorkerWrapper(const CFormatWorkerWrapper& cWrapper);
  virtual ~CFormatWorkerWrapper();

  // Lebenswichtige Funktion.
  // In dieser Funktion sind alle Identifikationen bestimmt.
  // (eindeutige ID, update token, ...
  virtual void SetFormatWorker(CFormatWorker* pFormatWorker) = 0;

  TTypeOfFWWrapper GetTypeOfObject(){return m_nTypeOfObject;};


  TGUITreeStatus GetGUITreeStatus(){return m_nGUITreeStatus;};
  void SetGUITreeStatus(TGUITreeStatus nGUITreeStatus){m_nGUITreeStatus = nGUITreeStatus;};

  CFormatWorkerWrapper* GetFormatWorkerWrapper(const xtstring& sElementID, CFormatWorkerWrapper** pParentOfFlatField = 0);

  bool GetCRC32(const xtstring& sElementID, unsigned long& nCRC32);
  bool SetCRC32(const xtstring& sElementID, unsigned long nCRC32);
  bool GetRawContent(const xtstring& sElementID, const xtstring& sStartElementTyp, bool bTextFocusActive, bool bFlatStyle, xtstring& sRawContent);

  void GetAllElementID(CWrapperObjectStatusMap& cTemplates, CWrapperObjectStatusMap& cStones, CWrapperObjectStatusMap& cFields);
  TStatusOfFWWrapper GetStatus(){return m_nStatusOfObject;};
  bool GetStatusOfObject(const xtstring& sElementID, TStatusOfFWWrapper& nStatusOfObject);
  bool SetStatusOfObject(const xtstring& sElementID, TStatusOfFWWrapper nStatusOfObject);
  void ResetStatusOfAllObjects();
  bool GetValue(const xtstring& sElementID, xtstring& sContent, xtstring& sContentMask, xtstring& sNewContent);
  bool SetValue(const xtstring& sElementID, const xtstring& sNewContent);
  void ResetNewValues();
  xtstring GetClientAddressOfParent(bool& bElementIDFound, const xtstring& sElementID);

  CFormatWorkerWrapper* GetParent(){return m_pParent;};
  CFormatWorkerWrapper_Root* GetRoot(){return m_pRoot;};
  TDataFormatForStoneAndTemplate GetFormatWorkerType(){if (!m_pFormatWorker) return tDataFormatST_Default; return m_pFormatWorker->GetWorkPaperDataFormat();};
  CFormatWorker* GetFormatWorker(){return m_pFormatWorker;};

  bool AddWrapper(CFormatWorkerWrapper* pWrapper);

  size_t CountOfFieldWrappers(){return m_cFieldWrappers.size();};
  CFormatWorkerWrapper_Field* GetFieldWrapper(size_t nIndex);
  bool AddFieldWrapper(CFormatWorkerWrapper_Field* pWrapper);
  bool RemoveFieldWrappers();

  size_t CountOfStoneWrappers(){return m_cStoneWrappers.size();};
  CFormatWorkerWrapper_Stone* GetStoneWrapper(size_t nIndex);
  CFormatWorkerWrapper_Stone* GetFirstTAGStoneWrapper();
  CFormatWorkerWrapper_Stone* GetFirstTAGStoneWrapper(const xtstring& sStoneName);
  bool AddStoneWrapper(CFormatWorkerWrapper_Stone* pWrapper);
  bool RemoveStoneWrappers();
  bool RemoveAllNonPropertyStones();

  size_t CountOfTemplateWrappers(){return m_cTemplateWrappers.size();};
  CFormatWorkerWrapper_Template* GetTemplateWrapper(size_t nIndex);
  bool AddTemplateWrapper(CFormatWorkerWrapper_Template* pWrapper);
  bool RemoveTemplateWrappers();

  // serialize / deserialize
  virtual bool ReadXMLContent(CXPubMarkUp* pXMLDoc);
  virtual bool SaveXMLContent(CXPubMarkUp* pXMLDoc);

  void ResetInternalVariables();
  // nTreeStartActiontyp = TSAT_NOTSTART, TSAT_STARTDBLCLK, TSAT_STARTDRAGDROP
  // nTreeStartActiontyp ist nur in actual Wrapper gesetzt.
  void SetVariable_TreeStartActiontyp(int nTreeStartActiontyp);
  // sTreeStartElementtyp = TSET_UNDEFINED, TSET_TEMPLATE, TSET_STONE, TSET_FIELD
  // sTreeStartElementtyp ist in allen Childs gesetzt.
  void SetVariable_TreeStartElementtyp(const xtstring& sTreeStartElementtyp);
  // Focus in Dokumenttextrahmen aktiv?
  void SetVariable_TextFocusActive(bool bTextFocusActive);
	// setzt die SeitenNummer der ersten Seite aus dem Dokument
  void SetVariable_DocBeginPageNumber(const int nDocBeginPageNumber);

public:
  xtstring GetElementIDForReplace_Front();
  xtstring GetElementIDForReplace_Tail();
  bool ElementIDDefined();
  xtstring GetElementID(){return m_sElementID;};
  void SetClientAddress(const xtstring& sClientAddress){m_sClientAddress = sClientAddress;};
  xtstring GetClientAddress(){return m_sClientAddress;};
  void ClearClientAddress(){m_sClientAddress.clear();};

  void IncreaseReferenceCounter(){m_nReferenceCounter++;};
  void DecreaseReferenceCounter(){m_nReferenceCounter--; if (m_nReferenceCounter < 0)m_nReferenceCounter = 0;};
  int GetReferenceCounter(){return m_nReferenceCounter;};
  void ResetReferenceCounter(){m_nReferenceCounter = 0;};

  bool IsThisWrapperChild(CFormatWorkerWrapper* pWrapper);
  bool HasAnyChildElementID(int nLevel);
  void GetStatusCount(int nLevel, int& nStatus);
  bool HasAnyChildThisStatus(TStatusOfFWWrapper nStatusOfObject, int nLevel);
  bool HasAnyChildOtherStatus(TStatusOfFWWrapper nStatusOfObject);
  void CheckStatusOfAllObjects();

protected:
  void SetElementID(const xtstring& sElementID){m_sElementID = sElementID;};
  void ClearElementID(){m_sElementID.clear();};

protected:
  CFormatWorker*  m_pFormatWorker;

  CFieldWrappers    m_cFieldWrappers;
  CStoneWrappers    m_cStoneWrappers;
  CTemplateWrappers m_cTemplateWrappers;

  unsigned long                   m_nCRC32;
  CCRC32s     m_cCRC32;

  CNewValues  m_cNewValues;
  void RemoveNewValues(){m_cNewValues.clear();};
  xtstring GetNewValue(const xtstring sElementID){CNewValuesIterator it = m_cNewValues.find(sElementID); if (it != m_cNewValues.end()) return it->second; return _XT("");};
  CNewStatus  m_cNewStatus;
  void RemoveNewStatus(){m_cNewStatus.clear();};
  TStatusOfFWWrapper GetNewStatus(const xtstring sElementID){CNewStatusIterator it = m_cNewStatus.find(sElementID); if (it != m_cNewStatus.end()) return it->second; return tSOFWW_Default;};

private:
  TTypeOfFWWrapper                m_nTypeOfObject;
  xtstring                        m_sElementID;
  xtstring                        m_sClientAddress;
  int                             m_nReferenceCounter;
  TStatusOfFWWrapper              m_nStatusOfObject;
  TGUITreeStatus                  m_nGUITreeStatus;

  CFormatWorkerWrapper_Root*      m_pRoot;
  CFormatWorkerWrapper*           m_pParent;

public:
  // Prop Stone Functions
  bool WrapperIsPropStone();

  xtstring  GetPS_Me_TextInNameProperty();
  CXPubVariant  GetPS_Me_ElementID();
  int       GetPS_Me_ColDefinition_CountOfCols();
  xtstring  GetPS_Me_ColDefinition_ColText(int nCol);
  xtstring  GetPS_Me_ColDefinition_ColMask(int nCol);
  xtstring  GetPS_Me_ColDefinition_ColElementID(int nCol);
  xtstring  GetPS_Me_ColDefinition_ColPicture(int nCol);
  xtstring  GetPS_Me_ColDefinition_ColTag(int nCol);
  xtstring  GetPS_Me_ColDefinition_ColElementIDForReplace(int nCol);
  bool      GetPS_Me_ColDefinition_ColWidthDefined(int nCol);
  int       GetPS_Me_ColDefinition_ColWidth(int nCol);
  bool      GetPS_Me_ColDefinition_ColIconDefined(int nCol);
  int       GetPS_Me_ColDefinition_ColIconIndex(int nCol);
  bool      GetPS_Me_ColDefinition_ColAlignDefined(int nCol);
  int       GetPS_Me_ColDefinition_ColAlign(int nCol);
  bool      GetPS_Me_ColDefinition_ColButtonDefined(int nCol);
  int       GetPS_Me_ColDefinition_ColButton(int nCol);
  CPropertyBase* GetPS_Me_ExecuteActionRemote();
  CPropertyBase* GetPS_Me_ExecuteActionLocal();
  CPropertyBase* GetPS_Me_GlobalSettings();

  bool      GetPS_Child_Show();
  bool      GetPS_Child_ShowChilds();
  bool      GetPS_Child_ColumnDefinition_Defined();
  int       GetPS_Child_ColDefinition_CountOfCols();
  xtstring  GetPS_Child_ColDefinition_ColText(int nCol);
  xtstring  GetPS_Child_ColDefinition_ColElementID(int nCol);
  xtstring  GetPS_Child_ColDefinition_ColPicture(int nCol);
  xtstring  GetPS_Child_ColDefinition_ColTag(int nCol);
  bool      GetPS_Child_ColDefinition_ColWidthDefined(int nCol);
  int       GetPS_Child_ColDefinition_ColWidth(int nCol);
  bool      GetPS_Child_ColDefinition_ColIconDefined(int nCol);
  int       GetPS_Child_ColDefinition_ColIconIndex(int nCol);
  bool      GetPS_Child_ColDefinition_ColAlignDefined(int nCol);
  int       GetPS_Child_ColDefinition_ColAlign(int nCol);
  bool      GetPS_Child_ColDefinition_ColButtonDefined(int nCol);
  int       GetPS_Child_ColDefinition_ColButton(int nCol);
  bool      GetPS_Child_FlatHeader_ColumnDefinition_Defined();
  int       GetPS_Child_FlatHeader_ColDefinition_CountOfCols();
  xtstring  GetPS_Child_FlatHeader_ColDefinition_ColText(int nCol);
  xtstring  GetPS_Child_FlatHeader_ColDefinition_ElementID(int nCol);
  bool      GetPS_Child_FlatHeader_ColDefinition_ColWidthDefined(int nCol);
  int       GetPS_Child_FlatHeader_ColDefinition_ColWidth(int nCol);
  bool      GetPS_Child_FlatHeader_ColDefinition_ColIconDefined(int nCol);
  int       GetPS_Child_FlatHeader_ColDefinition_ColIconIndex(int nCol);
  bool      GetPS_Child_FlatHeader_ColDefinition_ColAlignDefined(int nCol);
  int       GetPS_Child_FlatHeader_ColDefinition_ColAlign(int nCol);
  bool      GetPS_Child_FlatHeader_ColDefinition_ColButtonDefined(int nCol);
  int       GetPS_Child_FlatHeader_ColDefinition_ColButton(int nCol);
  CPropertyBase*              GetPS_Child_ExecuteActionRemote(int nID);
  CFormatWorkerWrapper_Stone* GetPS_Child_ExecuteActionRemoteWrapper(int nID);
  CPropertyBase*              GetPS_Child_ExecuteActionLocal(int nID);
  CFormatWorkerWrapper_Stone* GetPS_Child_ExecuteActionLocalWrapper(int nID);
  CPropertyBase*              GetPS_Child_ExecuteActionRemote(const xtstring& sID);
  CFormatWorkerWrapper_Stone* GetPS_Child_ExecuteActionRemoteWrapper(const xtstring& sID);
  CPropertyBase*              GetPS_Child_ExecuteActionLocal(const xtstring& sID);
  CFormatWorkerWrapper_Stone* GetPS_Child_ExecuteActionLocalWrapper(const xtstring& sID);
  CPropertyBase*              GetPS_Child_ValueAndStatus();
  CFormatWorkerWrapper_Stone* GetPS_Child_ValueAndStatusWrapper();
  CPropertyBase*              GetPS_Child_ReportFilter();
  CFormatWorkerWrapper_Stone* GetPS_Child_ReportFilterWrapper();
  CPropertyBase*              GetPS_Child_Picture();
  CFormatWorkerWrapper_Stone* GetPS_Child_PictureWrapper();
  CPropertyBase*              GetPS_Child_GlobalSettings();
  CFormatWorkerWrapper_Stone* GetPS_Child_GlobalSettingsWrapper();
private:
  CPropertyBase* GetPS_Me();
  CPropertyBase* GetPS_Child(const xtstring& sTextInNameProperty);
};




class XPUBADDONMANAGER_EXPORTIMPORT CFormatWorkerWrapper_Field : public CFormatWorkerWrapper
{
public:
  CFormatWorkerWrapper_Field(CFormatWorkerWrapper_Root* pRoot, CFormatWorkerWrapper* pParent);
  CFormatWorkerWrapper_Field(const CFormatWorkerWrapper_Field& cField);
  ~CFormatWorkerWrapper_Field();

  xtstring GetName();
  xtstring GetComment();
  xtstring GetContentResult();
  xtstring GetContentMask();

  // Lebenswichtige Funktion.
  // In dieser Funktion sind alle Identifikationen bestimmt.
  // (eindeutige ID, update token, ...
  virtual void SetFormatWorker(CFormatWorker* pFormatWorker){assert(false);};

  bool IsKey(){return (m_pKey != 0);};
  bool IsText(){return (m_pText != 0);};
  bool IsArea(){return (m_pArea != 0);};

  // Lebenswichtige Funktion.
  // In dieser Funktion sind alle Identifikationen bestimmt.
  // (eindeutige ID, update token, ...
  void SetKey(CMPModelKeyInd_ForFrm* pKey);
  void SetText(CMPModelTextInd_ForFrm* pText);
  void SetArea(CMPModelAreaInd_ForFrm* pArea);

  CMPModelKeyInd_ForFrm*      GetKey(){return m_pKey;};
  CMPModelTextInd_ForFrm*     GetText(){return m_pText;};
  CMPModelAreaInd_ForFrm*     GetArea(){return m_pArea;};

  // serialize / deserialize
  virtual bool ReadXMLContent(CXPubMarkUp* pXMLDoc);
  virtual bool SaveXMLContent(CXPubMarkUp* pXMLDoc);

private:
  CMPModelKeyInd_ForFrm*      m_pKey;
  CMPModelTextInd_ForFrm*     m_pText;
  CMPModelAreaInd_ForFrm*     m_pArea;
};




class XPUBADDONMANAGER_EXPORTIMPORT CFormatWorkerWrapper_Stone : public CFormatWorkerWrapper
{
public:
  CFormatWorkerWrapper_Stone(CFormatWorkerWrapper_Root* pRoot, CFormatWorkerWrapper* pParent);
  CFormatWorkerWrapper_Stone(const CFormatWorkerWrapper_Stone& cStone);
  ~CFormatWorkerWrapper_Stone();

  xtstring GetName();
  xtstring GetComment();

  // Lebenswichtige Funktion.
  // In dieser Funktion sind alle Identifikationen bestimmt.
  // (eindeutige ID, update token, ...
  virtual void SetFormatWorker(CFormatWorker* pFormatWorker);

  // serialize / deserialize
  virtual bool ReadXMLContent(CXPubMarkUp* pXMLDoc);
  virtual bool SaveXMLContent(CXPubMarkUp* pXMLDoc);
};




class XPUBADDONMANAGER_EXPORTIMPORT CFormatWorkerWrapper_Template : public CFormatWorkerWrapper
{
public:
  CFormatWorkerWrapper_Template(CFormatWorkerWrapper_Root* pRoot, CFormatWorkerWrapper* pParent);
  CFormatWorkerWrapper_Template(const CFormatWorkerWrapper_Template& cTemplate);
  ~CFormatWorkerWrapper_Template();

  xtstring GetName();
  xtstring GetComment();
  xtstring GetFileShowName();

  bool IsFlatStyle(){return m_bFlatStyle;};

  // Lebenswichtige Funktion.
  // In dieser Funktion sind alle Identifikationen bestimmt.
  // (eindeutige ID, update token, ...
  virtual void SetFormatWorker(CFormatWorker* pFormatWorker);

  // serialize / deserialize
  virtual bool ReadXMLContent(CXPubMarkUp* pXMLDoc);
  virtual bool SaveXMLContent(CXPubMarkUp* pXMLDoc);
  bool GetXMLContent(xtstring& sXMLDoc);
  bool SetXMLContent(const xtstring& sXMLDoc);

protected:
  bool m_bFlatStyle;
};




class XPUBADDONMANAGER_EXPORTIMPORT CFormatWorkerWrapper_Document
{
  friend class CFormatWorkerWrapper_Root;
public:
  CFormatWorkerWrapper_Document(long nIndex, CFormatWorkerWrapper_Root* pRoot);
  virtual ~CFormatWorkerWrapper_Document();

  void operator = (const CFormatWorkerWrapper_Document* pDocument);

  long GetIndexOfTopTemplate(){return m_nIndexOfTopTemplate;};
  xtstring GetName(){return m_sName;};
  xtstring GetComment(){return m_sComment;};
  xtstring GetFileShowName(){return m_sFileShowName;};

  // serialize / deserialize
  bool ReadXMLContent(CXPubMarkUp* pXMLDoc);
  bool SaveXMLContent(CXPubMarkUp* pXMLDoc);

protected:
  CFormatWorkerWrapper_Template* GetTemplate(){return m_pTemplate;};

  void CloseTemplate(bool bSerializeTemplate);
  void OpenTemplate();
  bool TemplateOpened(){return m_bTemplateOpened;};

private:
  bool  m_bTemplateOpened;

  long      m_nIndexOfTopTemplate;
  xtstring  m_sName;
  xtstring  m_sComment;
  xtstring  m_sFileShowName;
  xtstring  m_sTemplateContent;

  CFormatWorkerWrapper_Template*  m_pTemplate;
  CFormatWorkerWrapper_Root*      m_pRoot;
};
typedef std::vector<CFormatWorkerWrapper_Document*>   CFWW_Documents;
typedef CFWW_Documents::iterator                      CFWW_DocumentsIterator;
typedef CFWW_Documents::const_iterator                CFWW_DocumentsConstIterator;
typedef CFWW_Documents::reverse_iterator              CFWW_DocumentsReverseIterator;







class XPUBADDONMANAGER_EXPORTIMPORT CFormatWorkerWrapper_Root
{
  friend class CFormatWorkerWrapper;
public:
  CFormatWorkerWrapper_Root(CXPubAddOnManager* pAddOnManager);
  CFormatWorkerWrapper_Root();
  ~CFormatWorkerWrapper_Root();

  // für jedes genierte Dokument aufrufen
  bool AddGeneratedWorkPaper(const xtstring& sXMLMemoryContent);
  bool SetGeneratedWorkPaper_MainDocument(const xtstring& sXMLMemoryContent);
  bool SetGeneratedWorkPaper_MainDocument(const CFormatWorkerWrapper_Document* pDocument);
  bool SetGeneratedWorkPaper_SatelliteDocument(const xtstring& sXMLMemoryContent);
  bool SetGeneratedWorkPaper_SatelliteDocument(const CFormatWorkerWrapper_Document* pDocument);


  size_t CountOfDocuments(){return m_cDocuments.size();};
  const CFormatWorkerWrapper_Document* GetDocumentConst(size_t nIndex);
  // Dokument aktivieren, 
  bool CloseActiveTemplateWrapper(bool bSerializeTemplate);
  bool ActivateTemplateWrapper(size_t nIndex, bool bSerializeOldTemplate);
  bool ActivateTemplateWrapper_MainDocument(bool bSerializeOldTemplate);
  bool ActivateTemplateWrapper_SatelliteDocument(bool bSerializeOldTemplate);
  xtstring GetDocumentName(size_t nIndex);
  xtstring GetDocumentComment(size_t nIndex);
  xtstring GetDocumentFileShowName(size_t nIndex);

  void SetAddOnManager(CXPubAddOnManager* pAddOnManager){m_pAddOnManager = pAddOnManager;};
  CXPubAddOnManager* GetAddOnManager(){return m_pAddOnManager;};

  // an FormatWorker oder Tree übegeben
  CFormatWorkerWrapper_Template* GetActualTemplateWrapper();

  xtstring GetXMLVersion();
  void SetXMLVersion(const xtstring& sXMLVersion);
  bool RemoveDocuments();
  bool RemoveDocuments_ExceptFirst();

  void ResetInternalVariablesInActualTemplate();

public:
  enum
  {
    tMainDocument_Index = 0,
    tStalliteDocument_Index = 1,
  };
  bool SaveAllDocumentsToMemory(xtstring& sXMLMemoryContent, bool bSerializeTemplate);
  bool ReadAllDocumentsFromMemory(const xtstring& sXMLMemoryContent);

  CFormatWorkerWrapper_Template* GetTopTemplateOfWPWrapper(CFormatWorkerWrapper* pWrapper);
  void CheckStatusOfAllObjects();

  CFormatWorkerWrapper* GetFormatWorkerWrapper(const xtstring& sElementID, CFormatWorkerWrapper** pParentOfFlatField = 0);

  bool GetCRC32(const xtstring& sElementID, unsigned long& nCRC32);
  bool SetCRC32(const xtstring& sElementID, unsigned long nCRC32);
  bool GetRawContent(const xtstring& sElementID, const xtstring& sStartElementTyp, bool bTextFocusActive, xtstring& sRawContent);

  void GetAllElementID(CWrapperObjectStatusVector& cTemplates, CWrapperObjectStatusVector& cStones, CWrapperObjectStatusVector& cFields);
  bool GetStatusOfObject(const xtstring& sElementID, TStatusOfFWWrapper& nStatusOfObject);
  bool SetStatusOfObject(const xtstring& sElementID, TStatusOfFWWrapper nStatusOfObject);
  void ResetStatusOfAllObjects();
  bool GetValue(const xtstring& sElementID, xtstring& sContent, xtstring& sContentMask, xtstring& sNewContent);
  bool SetValue(const xtstring& sElementID, const xtstring& sNewContent);
  void ResetNewValues();
  xtstring GetClientAddressOfParent(const xtstring& sElementID);

  CFormatWorkerWrapper_Stone* GetGetAllUsedRemoteExecStone(){return m_pGetAllUsedRemoteExecStone;};
  CFormatWorkerWrapper_Stone* GetGetAllUsedLocalExecStone(){return m_pGetAllUsedLocalExecStone;};
  CFormatWorkerWrapper_Stone* GetValueAndStatusStone(){return m_pValueAndStatusStone;};
  CFormatWorkerWrapper_Stone* GetGlobalSettingsStone(){return m_pGlobalSetting;};
protected:
  CFormatWorkerWrapper_Document* CreateNewDocument(long nDocumentIndex, const xtstring& sXMLMemoryContent);
  bool AddOrInsertDocument(CFormatWorkerWrapper_Document* pDocument, bool bInsertAtFront = false);
  CFormatWorkerWrapper_Document* GetDocument(size_t nIndex);

  CFWW_Documents m_cDocuments;
  CFormatWorkerWrapper_Template*  m_pActualWorkPaperWrapper;
  CFormatWorkerWrapper_Stone*     m_pGetAllUsedRemoteExecStone;
  CFormatWorkerWrapper_Stone*     m_pGetAllUsedLocalExecStone;
  CFormatWorkerWrapper_Stone*     m_pValueAndStatusStone;
  CFormatWorkerWrapper_Stone*     m_pGlobalSetting;

private:
  xtstring            m_sXMLVersion;
  CXPubAddOnManager*  m_pAddOnManager;
};


class XPUBADDONMANAGER_EXPORTIMPORT CPictureDownload_CB
{
protected:
  CPictureDownload_CB(CFormatWorkerWrapper_Root* pRoot)
  {
    m_pRoot = pRoot;
  }
  ~CPictureDownload_CB()
  {
  }

  CFormatWorkerWrapper_Stone* GetExecuteActionLocal_GetPicture();
  CFormatWorkerWrapper_Stone* GetExecuteActionRemote_GetPicture();
  CFormatWorkerWrapper_Stone* GetExecuteActionLocal_GetPictureInfo();
  CFormatWorkerWrapper_Stone* GetExecuteActionRemote_GetPictureInfo();
  CFormatWorkerWrapper_Stone* GetPictereStone();
public:
  virtual xtstring GetFileFromSymbolicName(const xtstring& sSymbolicName) = 0;
protected:
  CFormatWorkerWrapper_Root* m_pRoot;
};



#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_FORMATWORKERWRAPPERS_H_)

