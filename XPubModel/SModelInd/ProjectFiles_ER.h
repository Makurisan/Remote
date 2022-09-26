#if !defined(_PROJECTFILES_ER_H_)
#define _PROJECTFILES_ER_H_

#include "ModelExpImp.h"
#include "ProjectFiles_Base.h"
#include "ModelDef.h"
#include "ModelInd.h"
#include "SSyncEng/SyncEngine.h"
#include "ERModelInd.h"
#include "MPModelERModelInd.h"
#include "Sxml/XMLTagNames.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32





class CProjectFilesInd;

/////////////////////////////////////////////////////////////////////////////
// CProjectFileER
class XPUBMODEL_EXPORTIMPORT CProjectFileER : public CProjectFileBase,
                                              public CSyncERNotifiersGroup
{
  friend class CProjectFilesInd;
public:
  CProjectFileER(const xtstring& sBaseName, bool bCreateObjectsImmediately);
  ~CProjectFileER();
  void operator = (CProjectFileER* pFile);

  virtual void SetModuleName(const xtstring& sModuleName);
  xtstring GetFileContentForCopy();
  bool SetFileContentFromPaste(const xtstring& sContent);
  const xtstring& GetXMLFileContent();
  void SetXMLFileContent(const xtstring& sContent);
  xtstring GetEmptyXMLContent(){return EMPTY_ER_XPUB_XML_DOCUMENT;};

  bool IsXMLContentEmpty(){return (m_sXMLFileContent.length() == 0);};

  virtual LPCXTCHAR GetFileExtension(){return FILE_EXT_ER;};
  virtual LPCXTCHAR GetSubfolder(){return SUBFOLDER_ER;};

  bool Rename(const xtstring& sNewBaseFileName);
  bool Reload(const xtstring& sBaseFolder);
  bool ReloadAfterAlterByAnotherApp(bool bXMLChanged);
  static bool GetModuleNameFromXMLContent(const xtstring& sXMLContent, xtstring& sModuleName);
  static bool FileRead_Compressed(const xtstring& sCompressedFileName, const xtstring& sFileName, xtstring& sXMLFileContent);
  static bool FileRead_Uncompressed(const xtstring& sFileName, xtstring& sXMLFileContent);
  //
  CMPModelERModelInd* GetMPModelERModelInd();
  CERModelTableInd*   GetERModelTableInd(LPCXTCHAR pName);
  CERModelTableInd*   GetERModelTableInd(size_t nIndex);
  CERModelTableInd*   CreateERModelTableInd();
  bool                RemoveERModelTableInd(LPCXTCHAR pName);
  void GetAllERModels(CxtstringVector& sNames);

  virtual void GetCreatedAndChangedTime(xtstring& sCreated, xtstring& sChanged, bool bWithSeconds);
  xtstring GetFullXMLFileName();

  bool SaveXMLDocAndFile(const xtstring& sBaseFolder, CXPubMarkUp* pXMLDoc);

  bool ContentChanged();
protected:
  bool ReadXMLDocAndFile(const xtstring& sBaseFolder, CXPubMarkUp* pXMLDoc, const xtstring& sCompressedFileName);

  static bool FileWrite(const xtstring& sFileName, const xtstring& sXMLFileContent);

  // 
  xtstring m_sXMLFileContent; // diese Variable beinhaltet die Datei
  // for sync use
  CMPModelERModelInd* m_pSyncMP;
  CERTables           m_cERTables;
  bool m_bInCreateSyncObjects;
  bool SyncObjectsCreated(){return (m_pSyncMP != NULL);};
  bool SaveSyncObjectsIfChanged();
  bool CreateSyncObjects();
  bool DeleteSyncObjects(bool bSaveSyncObjectsIfChanged = true);
  //
  bool m_bCreateObjectsImmediately;
};

typedef std::vector<CProjectFileER*>              CFileArrayER;
typedef CFileArrayER::iterator                    CFileArrayERIterator;
typedef CFileArrayER::const_iterator              CFileArrayERConstIterator;
typedef CFileArrayER::reverse_iterator            CFileArrayERReverseIterator;


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_PROJECTFILES_ER_H_)
