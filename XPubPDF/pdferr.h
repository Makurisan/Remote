

#define LAST_ERROR_COUNT                  20

static long g_lErrorBuffer[LAST_ERROR_COUNT];
static short g_sInit;



#define ERR_DOC_NOT_EXIST                 -1
#define ERR_OK                            0
#define ERR_WRITE_TO_FILE                 1
#define ERR_NULL_POINTER                  2
#define ERR_MALLOC_FAIL                   3


//  PDFSetLastError(pRetPdf, ERR_NULL_POINTER);


#define ERROR_COUNT                       4
static const char czDocNotExist[]         = "Document not exist.";
static const char *pczErrorTable[] =
{
  "OK"
  "Can't write to file."
  "Null pointer."
  "Allocation function failed."
};
