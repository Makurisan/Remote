


#define XPubTagBuilder_OK             0
#define XPubTagBuilder_Not_Found      1 //TagBuilder not found
#define XPubTagBuilder_Alloc_Err      2 //memory allocation failed
#define XPubTagBuilder_Open_Err       3 //convert worker open failed
#define XPubTagBuilder_Get_Err        4 //convert worker get stream failed
#define XPubTagBuilder_Null_Err       5 //input parameter is null
#define XPubTagBuilder_File_Open_Err  6 //can not open file


/*
int XPubTagBuilderInit();
int XPubTagBuilderToPDF(int iID, char *pInput, char **ppOutput, long &lSize);
int XPubTagBuilderToPDFFile(int iID, char *pInput, char *pFile);
int XPubTagBuilderSetPassword(int iID, char *pOwner, char *pUser, long lPerm);
int XPubTagBuilderToRTF(int iID, char *pInput, char **ppOutput, long &lSize);
int XPubTagBuilderToRTFFile(int iID, char *pInput, char *pFile);
int XPubTagBuilderClose(int iID);
*/
