// Utils.h: interface for the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_Utils_H_)
#define _Utils_H_

#include "SModelInd/ModelExpImp.h"

using namespace std;


class XPUBMODEL_EXPORTIMPORT CUtils
{
public:
  static xtstring GetTimeStamp(bool bMiliToo);
  static xtstring GetTimeStamp_OnlyNumbers(bool bMiliToo);
  static void SleepInMILISEC(int nSleep);
  static void SleepInSEC(int nSleep);

  static void ResetRandom();

  static bool CreatePath(const xtstring& sPath, bool bLastElementToo);

  static bool CreateFileWithContent(const xtstring& sFullPathFileName, unsigned long nDataSize, const unsigned char *data);
  static bool SetFileChangedTime(const xtstring& sFullPathFileName, time_t timeChanged);

  static bool ReadFileContent(const xtstring& sFullPathFileName, unsigned long& nDataSize, unsigned char* data);
  static bool GetFileChangedTime(const xtstring& sFullPathFileName, time_t& timeChanged);
  static bool GetFileLength(const xtstring& sFullPathFileName, long& nFileLength);
  static bool GetFileMD5(const xtstring& sFullPathFileName, xtstring& sMD5);

  static bool RemoveFile(const xtstring& sFileName);

  static xtstring GetLoggedUser();
};


#endif // !defined(_Utils_H_)
