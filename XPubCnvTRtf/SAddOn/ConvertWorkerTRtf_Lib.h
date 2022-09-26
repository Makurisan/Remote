#if defined(_LIB)


class CXPubAddOnManager;
class CConvertToAddOn;
class CConvertWorkerTo;


CConvertToAddOn* RegisterAddOnManager_LIB_TRTF(CXPubAddOnManager* pManager);
bool UnregisterAddOnManager_LIB_TRTF(CConvertToAddOn* pCnvAddOn);
int GetCnvToAddOnVersion_LIB_TRTF();
CConvertWorkerTo* CreateConvertWorkerTo__LIB_TRTF(CConvertToAddOn* pCnvAddOn, HWND hWndParent);
CConvertWorkerTo* CreateConvertWorkerToClone__LIB_TRTF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);
bool ReleaseConvertWorkerTo__LIB_TRTF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);


#endif // defined(_LIB)
