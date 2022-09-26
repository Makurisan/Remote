#if !defined(_XPUBTAGSTD_H_)
#define _XPUBTAGSTD_H_


#include "XPubTagStdExpImp.h"

#include "SSTLDef/STLdef.h"
#include "SModelInd/ModelDef.h"
#include "SAddOnManager/XPubAddOnManager.h"
#include "SAddOnManager/TagBuilder.h"
#include <map>




class  CXPubTagStd;

typedef std::map <int, CXPubTagStd *>            XPubTagStdMap;
//typedef std::map <int, CXPubTagStd *>::iterator    XPUBTAGSTD_EXPORTS XPubTagStdMapIter;

class  CXPubTagStd
{
public:
  CXPubTagStd(CXPubAddOnManager *pAddOnMgr);
  ~CXPubTagStd();

  void License(char* pLicense);
  int ToPDF(char *pInput, char **ppOutput, long &lSize);
  int ToRTF(char *pInput, char **ppOutput, long &lSize);
  void SetPassword(char *pOwner, char *pUser, long lPerm);

protected:
  void CheckOpenFile();

  CTagBuilder m_cBuilder;
  long m_lPerm;
  char *m_pOwner;
  char *m_pUser;
  bool m_bLicense;
  bool m_bDemoLicense;
  xtstring m_sLicense;
};

class CXPubTagStdMgr
{
public:
  CXPubTagStdMgr();
  ~CXPubTagStdMgr();

  void RemoveAll();

  int CreateTagStd();
  int Close(int iID);
  int GetTagBuiderCount();
  int License(int iID, char* pLicense);
  int ConvertTagToPDF(int iID, char *pInput, char **ppOutput, long &lSize);
  int ConvertTagToPDF(int iID, char *pInput, char *pFile);
  int SetPassword(int iID, char *pOwner, char *pUser, long lPerm);
  int ConvertTagToRTF(int iID, char *pInput, char **ppOutput, long &lSize);
  int ConvertTagToRTF(int iID, char *pInput, char *pFile);

protected:

  XPubTagStdMap       m_TagStdMap;
  int                 m_iTagStd;
  CXPubAddOnManager   m_cAddOnManager;
  
};

#endif
