#if defined(_LIB)



class CXPubAddOnManager;
class CConvertFromAddOn;
class CConvertWorkerFrom;


CConvertFromAddOn* RegisterAddOnManager_LIB_FTAG(CXPubAddOnManager* pManager);
bool UnregisterAddOnManager_LIB_FTAG(CConvertFromAddOn* pCnvAddOn);
int GetCnvFromAddOnVersion_LIB_FTAG();
CConvertWorkerFrom* CreateConvertWorkerFrom__LIB_FTAG(CConvertFromAddOn* pCnvAddOn);
CConvertWorkerFrom* CreateConvertWorkerFromClone__LIB_FTAG(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker);
bool ReleaseConvertWorkerFrom__LIB_FTAG(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker);



#endif // defined(_LIB)
