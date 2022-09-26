// PathUtil.h: interface for the CPathUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PathUtil_H_)
#define _PathUtil_H_

#include "SModelInd/ModelExpImp.h"

using namespace std;


class XPUBMODEL_EXPORTIMPORT CPathUtil
{
public:
  static xtstring GetPathWithSlashFromFullFileName(const xtstring& sFile);
  static xtstring GetPathWithoutSlashFromFullFileName(const xtstring& sFile);
  static xtstring MakeFullPath(const xtstring& sFirstPart, const xtstring& sSecondPart);
  static xtstring MakeFullPathSpecial(const xtstring& sPartialPath, const xtstring& sFileMayBeWithPath);
  static bool PathExist(const xtstring& sPath);
  static bool FileExist(const xtstring& sFile);
};


#endif // !defined(_PathUtil_H_)
