


#define XPubTagStd_OK               0
#define XPubTagStd_LicenseNotFound  1
#define XPubTagStd_Not_Found        1 //TagStd not found
#define XPubTagStd_Alloc_Err        2 //memory allocation failed
#define XPubTagStd_Open_Err         3 //convert worker open failed
#define XPubTagStd_Get_Err          4 //convert worker get stream failed
#define XPubTagStd_Null_Err         5 //input parameter is null
#define XPubTagStd_File_Open_Err    6 //can not open file


/*
int XPubTagStdInit();
int XPubTagStdLicense(int iID, char* pLicense);
int XPubTagStdToPDF(int iID, char *pInput, char **ppOutput, long &lSize);
int XPubTagStdToPDFFile(int iID, char *pInput, char *pFile);
int XPubTagStdSetPassword(int iID, char *pOwner, char *pUser, long lPerm);
int XPubTagStdToRTF(int iID, char *pInput, char **ppOutput, long &lSize);
int XPubTagStdToRTFFile(int iID, char *pInput, char *pFile);
int XPubTagStdClose(int iID);
*/
