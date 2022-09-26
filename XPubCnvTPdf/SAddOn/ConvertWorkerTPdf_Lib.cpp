#if defined(_LIB)

#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"

#include "SAddOnManager/XPubAddOnManager.h"

#include "SAddOnManager/ConvertWorker.h"
#include "SAddOn/ConvertWorkerFTag.h"
#include "PDFDocument.h"

#include "ConvertWorkerTPdf.h"

CConvertToAddOn* RegisterAddOnManager_LIB_TPDF(CXPubAddOnManager* pManager)
{
  CConvertToAddOn* pNew = new CConvertToPdfAddOn(pManager);
  if (!pNew)
    throw(ERR_NEW_PROBLEM);
  return pNew;
}
bool UnregisterAddOnManager_LIB_TPDF(CConvertToAddOn* pCnvAddOn)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_UNREGISTER_ADDON_WITH_NULL_POINTER);
    return false;
  }

  delete pCnvAddOn;
  pCnvAddOn = NULL;
  return true;
}
int GetCnvToAddOnVersion_LIB_TPDF()
{
  return ConvertWorkerTPdf_Version;
}
CConvertWorkerTo* CreateConvertWorkerTo__LIB_TPDF(CConvertToAddOn* pCnvAddOn, HWND hWndParent)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerTo(hWndParent);
}
CConvertWorkerTo* CreateConvertWorkerToClone__LIB_TPDF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKERCLONE_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerToClone(pConvertWorker);
}
bool ReleaseConvertWorkerTo__LIB_TPDF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_RELEASEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->ReleaseConvertWorkerTo(pConvertWorker);
}



#endif // defined(_LIB)
