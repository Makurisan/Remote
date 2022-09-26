#if !defined(_PROJECTFILES_TEMPLATE_H_)
#define _PROJECTFILES_TEMPLATE_H_

#include "ModelExpImp.h"
#include "ProjectFiles_Base.h"
#include "ModelDef.h"
#include "ModelInd.h"
#include "SSyncEng/SyncEngine.h"
#include "MPModelFieldsInd.h"
#include "MPModelTemplateInd.h"
#include "DBTemplateInd.h"
#include "Sxml/XMLTagNames.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32




class CProjectFilesInd;

/////////////////////////////////////////////////////////////////////////////
// CProjectFileTemplate
class XPUBMODEL_EXPORTIMPORT CProjectFileTemplate : public CProjectFileBase,
                                                    public CSyncWPNotifiersGroup
{
  friend class CProjectFilesInd;
public:
  CProjectFileTemplate(const xtstring& sBaseName, bool bCreateObjectsImmediately);
  ~CProjectFileTemplate();
  void operator = (CProjectFileTemplate* pFile);

  virtual xtstring GetNameForShow();
  const xtstring& GetDataFileContent(){return m_sDataFileContent;};
  virtual void SetDataFileContent(const xtstring& sContent);

  virtual void SetModuleName(const xtstring& sModuleName);
  xtstring GetFileContentForCopy();
  bool SetFileContentFromPaste(const xtstring& sContent);
  const xtstring& GetXMLFileContent();
  void SetXMLFileContent(const xtstring& sContent);
  xtstring GetEmptyXMLContent(){return EMPTY_TEMPLATE_XPUB_XML_DOCUMENT;};

  bool IsXMLContentEmpty(){return (m_sXMLFileContent.length() == 0);};

  virtual LPCXTCHAR GetFileExtension(){return FILE_EXT_TEMPLATE;};
  virtual LPCXTCHAR GetSubfolder(){return SUBFOLDER_TEMPLATE;};

  bool Rename(const xtstring& sNewBaseFileName);
  bool Reload(const xtstring& sBaseFolder);
  bool ReloadAfterAlterByAnotherApp(bool bXMLChanged);
  static bool GetModuleNameFromXMLContent(const xtstring& sXMLContent, xtstring& sModuleName);
  static bool FileRead_Compressed(const xtstring& sCompressedFileName, const xtstring& sFileName, xtstring& sFileContent);
  static bool FileRead_Uncompressed(const xtstring& sFileName, xtstring& sFileContent);
  //
  CMPModelTemplateInd* GetMPModelTemplateInd();
  TDataFormatForStoneAndTemplate GetDataFormat(){return m_nDataFormat;};

  virtual void GetCreatedAndChangedTime(xtstring& sCreated, xtstring& sChanged, bool bWithSeconds);
  xtstring GetFullXMLFileName();
  xtstring GetDataFileName(LPCXTCHAR pXMLDoc = 0);
  xtstring GetFullDataFileName();

  CDBTemplateInd* GetDBTemplateInd(){return m_pSyncDBTemplate;};

  bool SaveXMLDocAndFile(const xtstring& sBaseFolder, CXPubMarkUp* pXMLDoc);

  bool ContentChanged();
protected:
  bool ReadXMLDocAndFile(const xtstring& sBaseFolder, CXPubMarkUp* pXMLDoc, const xtstring& sCompressedFileName);

  static bool FileWrite(const xtstring& sFileName, const xtstring& sFileContent);

  TDataFormatForStoneAndTemplate m_nDataFormat;
  bool m_bMixedFormatsInProject;
  // diese Variablen beinhaltet die Datei
  xtstring m_sXMLFileContent;
  xtstring m_sDataFileContent;
  // for sync use
  CMPModelTemplateInd* m_pSyncMPTemplate;
  CDBTemplateInd*      m_pSyncDBTemplate;
  bool m_bInCreateSyncObjects;
  bool SyncObjectsCreated(){return (m_pSyncMPTemplate != NULL);};
  bool SaveSyncObjectsIfChanged();
  bool CreateSyncObjects();
  bool DeleteSyncObjects(bool bSaveSyncObjectsIfChanged = true);
  //
  bool m_bCreateObjectsImmediately;
};

typedef std::vector<CProjectFileTemplate*>        CFileArrayTemplate;
typedef CFileArrayTemplate::iterator              CFileArrayTemplateIterator;
typedef CFileArrayTemplate::const_iterator        CFileArrayTemplateConstIterator;
typedef CFileArrayTemplate::reverse_iterator      CFileArrayTemplateReverseIterator;

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_PROJECTFILES_TEMPLATE_H_)
