#if !defined(_XPUBTAGBUILDER_H_)
#define _XPUBTAGBUILDER_H_


#include "XPubTagBuilderExpImp.h"

#include "SSTLDef/STLdef.h"
#include "SModelInd/ModelDef.h"
#include "SAddOnManager/XPubAddOnManager.h"
#include "SAddOnManager/TagBuilder.h"
#include <map>

#if defined(MAC_OS_MWE) || defined(LINUX)
extern "C"
{
#endif

int  XPubTagBuilderInit();
int  XPubTagBuilderToPDF(int iID, char *pInput, char **ppOutput, long &lSize);
int  XPubTagBuilderToPDFFile(int iID, char *pInput, char *pFile);
int  XPubTagBuilderSetPassword(int iID, char *pOwner, char *pUser, long lPerm);
int  XPubTagBuilderToRTF(int iID, char *pInput, char **ppOutput, long &lSize);
int  XPubTagBuilderToRTFFile(int iID, char *pInput, char *pFile);
int  XPubTagBuilderClose(int iID);

#if defined(MAC_OS_MWE) || defined(LINUX)
}
#endif

class  CXPubTagBuilder;

typedef std::map <int, CXPubTagBuilder *>            XPubTagBuilderMap;
//typedef std::map <int, CXPubTagBuilder *>::iterator    XPUBTAGBUILDER_EXPORTS XPubTagBuilderMapIter;

class  CXPubTagBuilder
{
public:
  CXPubTagBuilder(CXPubAddOnManager *pAddOnMgr);
  ~CXPubTagBuilder();

  int ToPDF(char *pInput, char **ppOutput, long &lSize);
  int ToRTF(char *pInput, char **ppOutput, long &lSize);
  void SetPassword(char *pOwner, char *pUser, long lPerm);

protected:
  CTagBuilder m_cBuilder;
  long m_lPerm;
  char *m_pOwner;
  char *m_pUser;
};

class CXPubTagBuilderMgr
{
public:
  CXPubTagBuilderMgr();
  ~CXPubTagBuilderMgr();

  int CreateTagBuilder();
  int Close(int iID);
  int GetTagBuiderCount();
  int ConvertTagToPDF(int iID, char *pInput, char **ppOutput, long &lSize);
  int ConvertTagToPDF(int iID, char *pInput, char *pFile);
  int SetPassword(int iID, char *pOwner, char *pUser, long lPerm);
  int ConvertTagToRTF(int iID, char *pInput, char **ppOutput, long &lSize);
  int ConvertTagToRTF(int iID, char *pInput, char *pFile);

protected:

  XPubTagBuilderMap   m_TagBuilderMap;
  int                 m_iTagBuilder;
  CXPubAddOnManager m_cAddOnManager;
  
};

#endif
