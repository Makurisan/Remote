

class CXPubAddOnManager;
class CConvertFromAddOn;
class CConvertToAddOn;
class CConvertWorkerFrom;
class CConvertWorkerTo;
class CFormatAddOn;
class CFormatWorker;
typedef enum TWorkPaperType;


#if defined(USE_CNVFTAG_STUBS)

CConvertFromAddOn* RegisterAddOnManager_LIB_FTAG(CXPubAddOnManager* pManager){return 0;};
bool UnregisterAddOnManager_LIB_FTAG(CConvertFromAddOn* pCnvAddOn){return false;};
int GetCnvFromAddOnVersion_LIB_FTAG(){return 0;};
CConvertWorkerFrom* CreateConvertWorkerFrom__LIB_FTAG(CConvertFromAddOn* pCnvAddOn){return 0;};
CConvertWorkerFrom* CreateConvertWorkerFromClone__LIB_FTAG(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker){return 0;};
bool ReleaseConvertWorkerFrom__LIB_FTAG(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker){return false;};

#endif // defined(USE_CNVFTAG_STUBS)



#if defined(USE_CNVTRTF_STUBS)

CConvertToAddOn* RegisterAddOnManager_LIB_TRTF(CXPubAddOnManager* pManager){return 0;};
bool UnregisterAddOnManager_LIB_TRTF(CConvertToAddOn* pCnvAddOn){return false;};
int GetCnvToAddOnVersion_LIB_TRTF(){return 0;};
CConvertWorkerTo* CreateConvertWorkerTo__LIB_TRTF(CConvertToAddOn* pCnvAddOn, HWND hWndParent){return 0;};
CConvertWorkerTo* CreateConvertWorkerToClone__LIB_TRTF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker){return 0;};
bool ReleaseConvertWorkerTo__LIB_TRTF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker){return false;};

#endif // defined(USE_CNVTRTF_STUBS)



#if defined(USE_CNVTPDF_STUBS)

CConvertToAddOn* RegisterAddOnManager_LIB_TPDF(CXPubAddOnManager* pManager){return 0;};
bool UnregisterAddOnManager_LIB_TPDF(CConvertToAddOn* pCnvAddOn){return false;};
int GetCnvToAddOnVersion_LIB_TPDF(){return 0;};
CConvertWorkerTo* CreateConvertWorkerTo__LIB_TPDF(CConvertToAddOn* pCnvAddOn, HWND hWndParent){return 0;};
CConvertWorkerTo* CreateConvertWorkerToClone__LIB_TPDF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker){return 0;};
bool ReleaseConvertWorkerTo__LIB_TPDF(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker){return false;};

#endif // defined(USE_CNVTPDF_STUBS)



#if defined(USE_FRMDB_STUBS)

CFormatAddOn* RegisterAddOnManager_LIB_FRMDB(CXPubAddOnManager* pManager){return 0;};
bool UnregisterAddOnManager_LIB_FRMDB(CFormatAddOn* pFrmAddOn){return false;};
int GetFrmAddOnVersion_LIB_FRMDB(){return 0;};
CFormatWorker*  CreateFormatWorker__LIB_FRMDB(CFormatAddOn* pFrmAddOn, TWorkPaperType nWorkPaperType){return 0;};
CFormatWorker*  CreateFormatWorkerClone__LIB_FRMDB(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return 0;};
bool ReleaseFormatWorker__LIB_FRMDB(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return false;};

#endif // defined(USE_FRMPROP_STUBS)



#if defined(USE_FRMPROP_STUBS)

CFormatAddOn* RegisterAddOnManager_LIB_FRMPROP(CXPubAddOnManager* pManager){return 0;};
bool UnregisterAddOnManager_LIB_FRMPROP(CFormatAddOn* pFrmAddOn){return false;};
int GetFrmAddOnVersion_LIB_FRMPROP(){return 0;};
CFormatWorker*  CreateFormatWorker__LIB_FRMPROP(CFormatAddOn* pFrmAddOn, TWorkPaperType nWorkPaperType){return 0;};
CFormatWorker*  CreateFormatWorkerClone__LIB_FRMPROP(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return 0;};
bool ReleaseFormatWorker__LIB_FRMPROP(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return false;};

#endif // defined(USE_FRMPROP_STUBS)



#if defined(USE_FRMTAG_STUBS)

CFormatAddOn* RegisterAddOnManager_LIB_FRMTAG(CXPubAddOnManager* pManager){return 0;};
bool UnregisterAddOnManager_LIB_FRMTAG(CFormatAddOn* pFrmAddOn){return false;};
int GetFrmAddOnVersion_LIB_FRMTAG(){return 0;};
CFormatWorker*  CreateFormatWorker__LIB_FRMTAG(CFormatAddOn* pFrmAddOn, TWorkPaperType nWorkPaperType){return 0;};
CFormatWorker*  CreateFormatWorkerClone__LIB_FRMTAG(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return 0;};
bool ReleaseFormatWorker__LIB_FRMTAG(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return false;};

#endif // defined(USE_FRMTAG_STUBS)

#if defined(USE_FRMHTML_STUBS)

CFormatAddOn* RegisterAddOnManager_LIB_FRMHTML(CXPubAddOnManager* pManager){return 0;};
bool UnregisterAddOnManager_LIB_FRMHTML(CFormatAddOn* pFrmAddOn){return false;};
int GetFrmAddOnVersion_LIB_FRMHTML(){return 0;};
CFormatWorker*  CreateFormatWorker__LIB_FRMHTML(CFormatAddOn* pFrmAddOn, TWorkPaperType nWorkPaperType){return 0;};
CFormatWorker*  CreateFormatWorkerClone__LIB_FRMHTML(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return 0;};
bool ReleaseFormatWorker__LIB_FRMHTML(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return false;};

#endif // defined(USE_FRMHTML_STUBS)

#if defined(USE_FRMPLAIN_STUBS)

CFormatAddOn* RegisterAddOnManager_LIB_FRMPLAIN(CXPubAddOnManager* pManager){return 0;};
bool UnregisterAddOnManager_LIB_FRMPLAIN(CFormatAddOn* pFrmAddOn){return false;};
int GetFrmAddOnVersion_LIB_FRMPLAIN(){return 0;};
CFormatWorker*  CreateFormatWorker__LIB_FRMPLAIN(CFormatAddOn* pFrmAddOn, TWorkPaperType nWorkPaperType){return 0;};
CFormatWorker*  CreateFormatWorkerClone__LIB_FRMPLAIN(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return 0;};
bool ReleaseFormatWorker__LIB_FRMPLAIN(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker){return false;};

#endif // defined(USE_FRMPLAIN_STUBS)


