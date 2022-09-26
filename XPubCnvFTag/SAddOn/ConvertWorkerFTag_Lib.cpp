#if defined(_LIB)

#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"

#include "XPicture.h" 
 
#include "SAddOnManager/XPubAddOnManager.h"
#include "ConvertWorkerFTag.h"


CConvertFromAddOn* RegisterAddOnManager_LIB_FTAG(CXPubAddOnManager* pManager)
{
  CConvertFromAddOn* pNew = new CConvertFromTagAddOn(pManager);
  if (!pNew)
    throw(ERR_NEW_PROBLEM);
  return pNew;
}
bool UnregisterAddOnManager_LIB_FTAG(CConvertFromAddOn* pCnvAddOn)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_UNREGISTER_ADDON_WITH_NULL_POINTER);
    return false;
  }

  pCnvAddOn->DestroyAndRemoveAllConvertWorkers();
  delete pCnvAddOn;
  pCnvAddOn = NULL;
  return true;
}
int GetCnvFromAddOnVersion_LIB_FTAG()
{
  return ConvertWorkerFTag_Version;
}
CConvertWorkerFrom* CreateConvertWorkerFrom__LIB_FTAG(CConvertFromAddOn* pCnvAddOn)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerFrom();
}
CConvertWorkerFrom* CreateConvertWorkerFromClone__LIB_FTAG(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKERCLONE_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerFromClone(pConvertWorker);
}
bool ReleaseConvertWorkerFrom__LIB_FTAG(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_RELEASEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->ReleaseConvertWorkerFrom(pConvertWorker);
}



#endif // defined(_LIB)
