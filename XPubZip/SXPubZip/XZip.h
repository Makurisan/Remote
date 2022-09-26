#if !defined(_XPUBZIP_H_)
#define _XPUBZIP_H_


#include "XPubZipExpImp.h"

class CZipArchive;
class CZipString;

class XPUBZIP_EXPORTIMPORT CXPubZipProgressCallBack
{
public:
  CXPubZipProgressCallBack(){};
  ~CXPubZipProgressCallBack(){};

  virtual void XPubZip_AddFolderContent_ProgressStart(int nMaxPos) = 0;
  virtual void XPubZip_AddFolderContent_Progress(int nPos, int nMaxPos) = 0;
  virtual void XPubZip_AddFolderContent_ProgressEnd(int nMaxPos) = 0;
};

class XPUBZIP_EXPORTIMPORT CXPubZip
{
public:
  CXPubZip(CXPubZipProgressCallBack* pCallBack);
  ~CXPubZip();

  typedef enum XPubZip_PathBehaviour
  {
    tXPZ_FullPath,
    tXPZ_RelativePath,
    tXPZ_NoPath
  };

  void SetZIPFile(LPCTSTR pZIPFile);
  void SetPassword(LPCTSTR pPassword);

  bool IsFileZIPFile();
  bool FileExistInZIP(LPCTSTR pFileName);
  long GetFileLength(LPCTSTR pFileName);
  bool GetFileContent(LPCTSTR pFileName, char* pData, long nLen);

  void CreateZIP_FolderContent(LPCTSTR pFolder, bool bRecursive, XPubZip_PathBehaviour nPathBehaviour);
  void CreateZIP_WildCard(LPCTSTR pFolder, bool bRecursive, XPubZip_PathBehaviour nPathBehaviour);

protected:
  void CreateZIP(const CZipString& sFolder, const CZipString& sWildcardToAdd, bool bRecursive, XPubZip_PathBehaviour nPathBehaviour);
  void AddFolderToZip(const CZipString& sFolder, const CZipString& sWildcardToAdd, CZipArchive* pZip, int nTotalCount, int& nActualCount);
  int AddFolderToZip_EstimateCountOfFiles(const CZipString& sFolder, const CZipString& sWildcardToAdd);

  CXPubZipProgressCallBack* m_pCallBack;
  CZipString*               m_pZIPFile;
  CZipString*               m_pPassword;
  bool                      m_bRecursive;
  XPubZip_PathBehaviour     m_nPathBehaviour;
};



#endif // _XPUBZIP_H_
