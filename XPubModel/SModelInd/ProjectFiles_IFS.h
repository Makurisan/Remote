#if !defined(_PROJECTFILES_IFS_H_)
#define _PROJECTFILES_IFS_H_

#include "ModelExpImp.h"
#include "ProjectFiles_Base.h"
#include "ModelDef.h"
#include "ModelInd.h"
#include "SSyncEng/SyncEngine.h"
#include "IFSEntityInd.h"
#include "MPModelIFSInd.h"
#include "Sxml/XMLTagNames.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32




class CProjectFilesInd;

/////////////////////////////////////////////////////////////////////////////
// CProjectFileIFS
class XPUBMODEL_EXPORTIMPORT CProjectFileIFS : public CProjectFileBase,
                                               public CSyncIFSNotifiersGroup
{
  friend class CProjectFilesInd;
public:
  CProjectFileIFS(const xtstring& sBaseName, bool bCreateObjectsImmediately);
  ~CProjectFileIFS();
  void operator = (CProjectFileIFS* pFile);

  virtual void SetModuleName(const xtstring& sModuleName);
  xtstring GetFileContentForCopy();
  bool SetFileContentFromPaste(const xtstring& sContent);
  const xtstring& GetXMLFileContent();
  void SetXMLFileContent(const xtstring& sContent);
  xtstring GetEmptyXMLContent(){return EMPTY_IFS_XPUB_XML_DOCUMENT;};

  bool IsXMLContentEmpty(){return (m_sXMLFileContent.length() == 0);};

  virtual LPCXTCHAR GetFileExtension(){return FILE_EXT_IFS;};
  virtual LPCXTCHAR GetSubfolder(){return SUBFOLDER_IFS;};

  bool Rename(const xtstring& sNewBaseFileName);
  bool Reload(const xtstring& sBaseFolder);
  bool ReloadAfterAlterByAnotherApp(bool bXMLChanged);
  static bool GetModuleNameFromXMLContent(const xtstring& sXMLContent, xtstring& sModuleName);
  static bool FileRead_Compressed(const xtstring& sCompressedFileName, const xtstring& sFileName, xtstring& sXMLFileContent);
  static bool FileRead_Uncompressed(const xtstring& sFileName, xtstring& sXMLFileContent);
  //
  CMPModelIFSInd* GetMPModelIFSInd();
  CIFSEntityInd*  GetMainIFSEntityInd();
  CIFSEntityInd*  GetIFSEntityInd(LPCXTCHAR pName);
  CIFSEntityInd*  GetIFSEntityInd(size_t nIndex);
  CIFSEntityInd*  CreateIFSEntityInd();
  bool            RemoveIFSEntityInd(LPCXTCHAR pName);
  void GetAllEntitiesFromProjectFile(CxtstringVector& arrNames, CxtstringVector& arrERNames, CintVector& arrPropIDs, CxtstringVector& arrDBStoneNames);
  void GetAllEntitiesFromProjectFile(bool bStartEntityToo, const xtstring& sStartEntity, CxtstringVector& arrNames, CxtstringVector& arrERNames, CintVector& arrPropIDs, CxtstringVector& arrDBStoneNames);
  void GetAllVariablesFromProjectFile(const xtstring& sStartEntity, CxtstringVector& xtarrEntityNames, CxtstringVector& xtarrVariableNames);

  virtual void GetCreatedAndChangedTime(xtstring& sCreated, xtstring& sChanged, bool bWithSeconds);
  xtstring GetFullXMLFileName();

  bool SaveXMLDocAndFile(const xtstring& sBaseFolder, CXPubMarkUp* pXMLDoc);

  bool ContentChanged();
protected:
  bool ReadXMLDocAndFile(const xtstring& sBaseFolder, CXPubMarkUp* pXMLDoc, const xtstring& sCompressedFileName);

  static bool FileWrite(const xtstring& sBaseFolder, const xtstring& sXMLFileContent);
  // 
  xtstring m_sXMLFileContent; // diese Variable beinhaltet die Datei
  // for sync use
typedef std::vector<CIFSEntityInd*> CIFSEntityVector;
typedef CIFSEntityVector::iterator  CIFSEntityVectorIterator;
  CMPModelIFSInd* m_pSyncMP;
  CIFSEntityVector m_cIFSEntities;
  bool m_bInCreateSyncObjects;
  bool SyncObjectsCreated(){return (m_pSyncMP != NULL);};
  bool SaveSyncObjectsIfChanged();
  bool CreateSyncObjects();
  bool DeleteSyncObjects(bool bSaveSyncObjectsIfChanged = true);
  //
  bool m_bCreateObjectsImmediately;
};

typedef std::vector<CProjectFileIFS*>             CFileArrayIFS;
typedef CFileArrayIFS::iterator                   CFileArrayIFSIterator;
typedef CFileArrayIFS::const_iterator             CFileArrayIFSConstIterator;
typedef CFileArrayIFS::reverse_iterator           CFileArrayIFSReverseIterator;


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_PROJECTFILES_IFS_H_)
