#if defined(_LIB)

#include <windows.h>


class CXPubAddOnManager;
class CConvertToAddOn;
class CConvertWorkerTo;


CConvertToAddOn* RegisterAddOnManager_LIB_TPDF(CXPubAddOnManager* pManager);
bool UnregisterAddOnManager_LIB_TPDF(CConvertToAddOn* pCnvAddOn);
int GetCnvToAddOnVersion_LIB_TPDF();
CConvertWorkerTo* CreateConvertWorkerTo__LIB_TPDF(CConvertToAddOn* pCnvAddOn, HWND hWndParent);
CConvertWorkerTo* CreateConvertWorkerToClone__LIB_TPDF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);
bool ReleaseConvertWorkerTo__LIB_TPDF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);



#endif // defined(_LIB)
