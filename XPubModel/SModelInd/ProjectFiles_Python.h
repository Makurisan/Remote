#if !defined(_PROJECTFILES_PYTHON_H_)
#define _PROJECTFILES_PYTHON_H_

#include "ModelExpImp.h"
#include "ProjectFiles_Base.h"
#include "ModelDef.h"
#include "ModelInd.h"
#include "SSyncEng/SyncEngine.h"
#include "MPModelPythonInd.h"
#include "Sxml/XMLTagNames.h"



#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32




class CProjectFilesInd;

/////////////////////////////////////////////////////////////////////////////
// CProjectFilePythonModule
class XPUBMODEL_EXPORTIMPORT CProjectFilePythonModule : public CProjectFileBase
{
  friend class CProjectFilesInd;
public:
  CProjectFilePythonModule(const xtstring& sBaseName, bool bCreateObjectsImmediately);
  ~CProjectFilePythonModule();
  void operator = (CProjectFilePythonModule* pFile);

  const xtstring& GetDataFileContent(){return m_sDataFileContent;};
  void SetDataFileContent(const xtstring& sContent);

  virtual void SetModuleName(const xtstring& sModuleName);
  xtstring GetFileContentForCopy();
  bool SetFileContentFromPaste(const xtstring& sContent);
  const xtstring& GetXMLFileContent();
  void SetXMLFileContent(const xtstring& sContent);
  xtstring GetEmptyXMLContent(){return _XT("");};

  bool IsXMLContentEmpty(){return (m_sXMLFileContent.length() == 0);};

  virtual LPCXTCHAR GetFileExtension(){return FILE_EXT_PYTHON;};
  virtual LPCXTCHAR GetSubfolder(){return SUBFOLDER_PYTHON;};

  bool Rename(const xtstring& sNewBaseFileName);
  bool Reload(const xtstring& sBaseFolder);
  bool ReloadAfterAlterByAnotherApp(bool bXMLChanged);
  static bool GetModuleNameFromXMLContent(const xtstring& sXMLContent, xtstring& sModuleName);
  static bool FileRead_Compressed(const xtstring& sCompressedFileName, const xtstring& sFileName, xtstring& sXMLFileContent, xtstring& sPythonFileContent);
  static bool FileRead_Uncompressed(const xtstring& sFileName, xtstring& sXMLFileContent, xtstring& sPythonFileContent);
  //
  CMPModelPythonInd* GetMPModelPythonInd();

  virtual void GetCreatedAndChangedTime(xtstring& sCreated, xtstring& sChanged, bool bWithSeconds);
  xtstring GetFullXMLFileName();
  xtstring GetDataFileName(LPCXTCHAR pXMLDoc = 0);
  xtstring GetFullDataFileName();

  bool SaveXMLDocAndFile(const xtstring& sBaseFolder, CXPubMarkUp* pXMLDoc);

  bool ContentChanged();
protected:
  bool ReadXMLDocAndFile(const xtstring& sBaseFolder, CXPubMarkUp* pXMLDoc, const xtstring& sCompressedFileName);

  static bool FileWrite(const xtstring& sFileName, const xtstring& sXMLFileContent, const xtstring& sPythonFileContent);

  // diese Variablen beinhaltet die Datei
  xtstring m_sXMLFileContent;
  xtstring m_sDataFileContent;
  // for sync use
  bool m_bInCreateSyncObjects;
  bool SyncObjectsCreated(){return false;};
  bool SaveSyncObjectsIfChanged();
  bool CreateSyncObjects();
  bool DeleteSyncObjects(bool bSaveSyncObjectsIfChanged = true);
  //
  bool m_bCreateObjectsImmediately;
};

typedef std::vector<CProjectFilePythonModule*>    CFileArrayPythonModule;
typedef CFileArrayPythonModule::iterator          CFileArrayPythonModuleIterator;
typedef CFileArrayPythonModule::const_iterator    CFileArrayPythonModuleConstIterator;
typedef CFileArrayPythonModule::reverse_iterator  CFileArrayPythonModuleReverseIterator;


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_PROJECTFILES_PYTHON_H_)
