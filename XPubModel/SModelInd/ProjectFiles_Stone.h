#if !defined(_PROJECTFILES_STONE_H_)
#define _PROJECTFILES_STONE_H_

#include "ModelExpImp.h"
#include "ProjectFiles_Base.h"
#include "ModelDef.h"
#include "ModelInd.h"
#include "SSyncEng/SyncEngine.h"
#include "MPModelFieldsInd.h"
#include "MPModelStoneInd.h"
#include "DBStoneInd.h"
#include "PropStoneInd.h"
#include "Sxml/XMLTagNames.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32





class CProjectFilesInd;

/////////////////////////////////////////////////////////////////////////////
// CProjectFileStone
class XPUBMODEL_EXPORTIMPORT CProjectFileStone : public CProjectFileBase,
                                                 public CSyncWPNotifiersGroup
{
  friend class CProjectFilesInd;
public:
  CProjectFileStone(const xtstring& sBaseName, bool bCreateObjectsImmediately);
  ~CProjectFileStone();
  void operator = (CProjectFileStone* pFile);

  virtual xtstring GetNameForShow();
  const xtstring& GetDataFileContent(){return m_sDataFileContent;};
  virtual void SetDataFileContent(const xtstring& sContent);

  virtual void SetModuleName(const xtstring& sModuleName);
  xtstring GetFileContentForCopy();
  bool SetFileContentFromPaste(const xtstring& sContent);
  const xtstring& GetXMLFileContent();
  void SetXMLFileContent(const xtstring& sContent);
  xtstring GetEmptyXMLContent(){return EMPTY_STONE_XPUB_XML_DOCUMENT;};

  bool IsXMLContentEmpty(){return (m_sXMLFileContent.length() == 0);};

  virtual LPCXTCHAR GetFileExtension(){return FILE_EXT_STONE;};
  virtual LPCXTCHAR GetSubfolder(){return SUBFOLDER_STONE;};

  bool Rename(const xtstring& sNewBaseFileName);
  bool Reload(const xtstring& sBaseFolder);
  bool ReloadAfterAlterByAnotherApp(bool bXMLChanged);
  static bool GetModuleNameFromXMLContent(const xtstring& sXMLContent, xtstring& sModuleName);
  static bool FileRead_Compressed(const xtstring& sCompressedFileName, const xtstring& sFileName, xtstring& sFileContent);
  static bool FileRead_Uncompressed(const xtstring& sFileName, xtstring& sFileContent);
  //
  CMPModelStoneInd* GetMPModelStoneInd();
  TDataFormatForStoneAndTemplate GetDataFormat(){return m_nDataFormat;};

  virtual void GetCreatedAndChangedTime(xtstring& sCreated, xtstring& sChanged, bool bWithSeconds);
  xtstring GetFullXMLFileName();
  xtstring GetDataFileName(LPCXTCHAR pXMLDoc = 0);
  xtstring GetFullDataFileName();

  CDBStoneInd* GetDBStoneInd(){return m_pSyncDBStone;};
  CPropStoneInd* GetPropStoneInd(){return m_pSyncProp;};

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
  CMPModelStoneInd* m_pSyncMPStone;
  CDBStoneInd*      m_pSyncDBStone;
  CPropStoneInd*    m_pSyncProp;
  bool m_bInCreateSyncObjects;
  bool SyncObjectsCreated(){return (m_pSyncMPStone != NULL);};
  bool SaveSyncObjectsIfChanged();
  bool CreateSyncObjects();
  bool DeleteSyncObjects(bool bSaveSyncObjectsIfChanged = true);
  //
  bool m_bCreateObjectsImmediately;
};

typedef std::vector<CProjectFileStone*>       CFileArrayStone;
typedef CFileArrayStone::iterator             CFileArrayStoneIterator;
typedef CFileArrayStone::const_iterator       CFileArrayStoneConstIterator;
typedef CFileArrayStone::reverse_iterator     CFileArrayStoneReverseIterator;

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_PROJECTFILES_STONE_H_)
