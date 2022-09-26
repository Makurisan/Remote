
#ifndef __PDF_PDF_H__
#define __PDF_PDF_H__

// MIK #ifdef __cplusplus
// MIK extern "C"
// MIK {
// MIK #endif

#include <math.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

//#include <stdio.h>
#include <time.h>
#include "pdfdef.h"

#if defined(MAC_OS_MWE)
//#include <unix.h>
#define __dead2
#define __pure2
#include <extras.h>
#endif

#include "PDFBaseFont.h"
#include "PDFDocument.h"
#include "PDFLayer.h"


CPDFDocument *PDFGetDocClass(PDFDocPTR i_pPDFDoc);

void PDFBgnText(PDFDocPTR pdf, long clipmode);

void PDFEndText(PDFDocPTR pdf);

void PDFFlushDoc(PDFDocPTR i_pPDFDoc);

bool_a PDFEncrypt(PDFDocPTR i_pPDFDoc, unsigned char *i_pBuffer, long i_lSize, int i_iObjNum, int i_iObjGen);

bool_a PDFEncryptWithPassword(unsigned char *i_pBuffer, long i_lSize, int i_iObjNum, int i_iObjGen,
                              std::string i_strID, std::string i_strO, int i_iPerm,
                              std::string i_strOwnerPassw, int i_iEncRevision, int i_iLen);

long PDFWriteEncryption(PDFDocPTR i_pPDFDoc, long objNumber);

long PDFWriteInfo(PDFDocPTR i_pPDFDoc, long objNumber);

bool_a PDFInit(PDFDocPTR i_pPDFDoc);

void *PDFMalloc(size_t i_Size);

void PDFRealloc(void **io_ppBuffer, size_t i_Size, size_t i_Add);

long PDFRound(float i_fNum);

long PDFStrLenUC(const unsigned char *i_pBuf);

long PDFStrLenC(const char *i_pBuf);

char *PDFStrDup(const char *i_pStr);

bool_a IsWhiteSpace(char i_cChar);

bool_a IsDelimiter(char i_cChar);


size_t PDFStrStrKey(std::string& i_str, const char *i_pczFind, size_t i_index);
size_t PDFStrStrBoolKey(std::string& i_str, const char *i_pczFind);
size_t PDFStrStrNumKey(std::string& i_str, const char *i_pczFind);
size_t PDFStrStrStringKey(std::string& i_str, const char *i_pczFind);
size_t PDFStrStrHEXStringKey(std::string& i_str, const char *i_pczFind);
size_t PDFStrStrNameKey(std::string& i_str, const char *i_pczFind);
size_t PDFStrStrArrayKey(std::string& i_str, const char *i_pczFind);
size_t PDFStrStrDicKey(std::string& i_str, const char *i_pczFind);

char *PDFStrJoin(const char *i_pStr1, const char *i_pStr2);

float PDFConvertDimensionFromDefault(float i_fValue);
long PDFConvertDimensionFromDefaultL(long i_lValue);

float PDFConvertDimensionToDefault(float i_fValue);
long PDFConvertDimensionToDefaultL(long i_lValue);

long PDFConvertMMToPoints(float i_lMM);

long PDFConvertPointsToTwips(long i_lPoints);

long PDFConvertTwipsToPoints(long i_lTwips);

long getFileSize(const char *file);


void PDFFlushString(PDFDocPTR i_pPDFDoc, CPDFString i_String, CPDFBaseFont *i_pFont);

void PDFDrawUnderline(PDFDocPTR i_pPDFDoc, CPDFBaseFont *i_pFont, float i_fFontSize, long i_lWidth,
                      long i_lX, long i_lY, CPDFColor& i_rColor);

void PDFDrawDoubleUnderline(PDFDocPTR i_pPDFDoc, CPDFBaseFont *i_pFont, float i_fFontSize, long i_lWidth,
                            long i_lX, long i_lY, CPDFColor& i_rColor);

void PDFDrawStrikeout(PDFDocPTR i_pPDFDoc, CPDFBaseFont *i_pFont, float i_fFontSize, long i_lWidth,
                      long i_lX, long i_lY, CPDFColor& i_rColor);


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//pages
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool_a PDFSetCurrentPage(PDFDocPTR i_pPDFDoc, long i_lPage);

bool_a PDFEndPage(PDFDocPTR in_docHandle,long page);

bool_a PDFDeletePage(PDFDocPTR i_pPDFDoc, long i_iPage);


char *PDFGetDocStream(PDFDocPTR i_pPDFDoc);

PDFStream *PDFCreateStream();

long PDFWriteFormatedBufToDocStream(PDFDocPTR i_pPDFDoc, const char *i_pczFormat, ...);

long PDFWriteBufToDocStream(PDFDocPTR i_pPDFDoc, const char *i_pczBuf, size_t i_Size);

bool_a PDFWriteToPageStream(PDFDocPTR i_pPDFDoc, char *i_pBuffer, size_t i_BuffSize, long i_lPage);

void PDFDeleteStream(PDFStream **i_pStream);



long PDFWritePagesObject(PDFDocPTR i_pPDFDoc, long objNumber);

long PDFWritePageObject(PDFDocPTR pdf, PDFPage *pInf);

long PDFWriteContentsFromMemory(PDFDocPTR pdf, PDFPage *pInf);

long PDFcloseContentMemStreamForPage(PDFDocPTR pdf, long page);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//viewer preferences, page layout
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool_a PDFEndDoc(PDFDocPTR *i_pPDFDoc);

char *PDFGetStream(PDFDocPTR i_pPDFDoc);

void PDFSetOwnerPassword(PDFDocPTR i_pPDFDoc, char *i_pOwnerPassword);

void PDFSetUserPassword(PDFDocPTR i_pPDFDoc, char *i_pUserPassword);

void PDFSetPermissionPassword(PDFDocPTR i_pPDFDoc, long i_lPermission);

void PDFEnableCompress(PDFDocPTR pdf, bool_a bON);

void PDFSetVersion(PDFDocPTR i_pPDFDoc, long i_lMajorLevel, long i_lMinorLeverl);

void PDFSetPageLayout(PDFDocPTR i_pPDFDoc, ePAGELAYOUTATTR i_ePageLayout);

void PDFSetPageMode(PDFDocPTR i_pPDFDoc, ePAGEMODEATTR i_ePageMode);

void PDFSetViewerPreferences(PDFDocPTR i_pPDFDoc, PPDFViewerPreferences i_pViewerPrfs);


bool_a PDFHasLineOnlySpace(PDFLine *i_pLine);

void PDFIncHEX(std::string &io_strHex);
long PDFConvertHEXToLong(std::string i_strHex);
long PDFConvertOCTToLong(std::string i_strOct);
bool_a PDFGetEndChar(char &io_cEnd);
bool_a PDFIsNumber(std::string i_str);
bool_a PDFIsPDFOperator(std::string i_str);

void PDFMakeUpper(std::string& sText);
void PDFRemoveEscapeChar(std::string& i_str);
std::string PDFGetStringUpToWhiteSpace(std::string& i_str, size_t &i_lIndex);
long PDFGetNumValue(std::string& i_str, const char *i_pczFind);
frec_a PDFGetRectValue(std::string& i_str, const char *i_pczFind);
PdfRgb PDFGetRGBValue(std::string& i_str, const char *i_pczFind);
long PDFGetObjValue(std::string& i_str, const char *i_pczFind, long& o_lGenNum);
bool_a PDFGetBoolValue(std::string& i_str, const char *i_pczFind);
bool_a PDFGetTextValue(std::string& i_str, const char *i_pczKey, std::string& o_strValue);
bool_a PDFGetTextValue(std::string& i_str, const char *i_pczKey, CPDFString& o_strValue);
char *PDFGetTextHEXValue(std::string& i_str, const char *i_pczFind);
char *PDFGetNameValue(std::string& i_str, const char *i_pczFind);
std::string PDFReadXrefObj(FILE *i_pFile, size_t i_Pos, size_t i_Size);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//images
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool_a PDFAddImage(PDFDocPTR i_pPDFDoc, const char *i_pczImageName,
                                  long i_lX, long i_lY, long i_lAngle,
                                  float i_fXScale, float i_fYScale);

bool_a PDFAddImageWH(PDFDocPTR i_pPDFDoc, const char *i_pczImageName,
                                    long i_lX, long i_lY, long i_lAngle,
                                    long i_lWidth, long i_lHeight);

bool_a PDFAddImageWHIndex(PDFDocPTR i_pPDFDoc, long i_lIndex,
                                    long i_lX, long i_lY, long i_lAngle,
                                    long i_lWidth, long i_lHeight);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//outline entry
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PDFOutlineEntry *PDFAddOutlineEntry(PDFDocPTR i_pPDFDoc, PDFOutlineEntry *i_pFollowThis, bool_a i_bSubEntry,
                                    bool_a i_bOpen, long i_lPage, char *i_pTitle, eDESTSPEC i_eDest, long i_lLeft,
                                    long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                                    float i_fR, float i_fG, float i_fB, short i_sFkey, long i_lFont,
                                    PDFOutlineEntry *i_pOutlineEntry);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


long PDFCreateLinkAnnotationFile(PDFDocPTR i_pPDFDoc, const char *i_pczName,
                                  long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                                  long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                                  const char *i_pLocation);

long PDFCreateLinkAnnotationURI(PDFDocPTR i_pPDFDoc, const char *i_pczName,
                                const char *i_pLocation);

long PDFCreateLinkAnnotationDest(PDFDocPTR i_pPDFDoc, const char *i_pczName,
                  long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                  long i_lTop, long i_lRight, long i_lBottom, float i_fZoom);

long PDFCreateLinkAnnotationEmptyDest(PDFDocPTR i_pPDFDoc, const char *i_pczName);

long PDFCreateLinkAnnotation(PDFDocPTR i_pPDFDoc, const char *i_pczName, long i_lPage,
                             char *i_pczDest, char *i_pczAction, float i_fZoom);

bool_a PDFChangeAnnotationDest(PDFDocPTR i_pPDFDoc, const char *i_pczName,
                  long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                  long i_lTop, long i_lRight, long i_lBottom, float i_fZoom);

bool_a PDFGetAnnotationIndex(PDFDocPTR i_pPDFDoc, const char *i_pczName);


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//error handling
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
long PDFGetLastError();

void PDFSetLastError(long i_lError);

const char *PDFGetErrorDescription(long i_lError);
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//graphic
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool_a PDFLineTo(PDFDocPTR i_pPDFDoc, float i_fX, float i_fY);
bool_a PDFLineToL(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY);

bool_a PDFSetDashedLine(PDFDocPTR i_pPDFDoc);

bool_a PDFSetSolidLine(PDFDocPTR i_pPDFDoc);

bool_a PDFMoveTo(PDFDocPTR i_pPDFDoc, float i_fX, float i_fY);
bool_a PDFMoveToL(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY);

bool_a PDFRectangle(PDFDocPTR i_pPDFDoc, frec_a i_Rec, bool_a i_bFill);
bool_a PDFRectangleL(PDFDocPTR i_pPDFDoc, rec_a i_Rec, bool_a i_bFill);

bool_a PDFSetLineWidth(PDFDocPTR i_pPDFDoc, float i_fWidth);
bool_a PDFSetLineWidthL(PDFDocPTR i_pPDFDoc, long i_lWidth);

void PDFSaveGraphicsState(PDFDocPTR i_pPDFDoc);

void PDFRestoreGraphicsState(PDFDocPTR i_pPDFDoc);

void PDFSetClippingRect(PDFDocPTR i_pPDFDoc, rec_a i_recPosSize);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//simple text
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool_a PDFAddText(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY, long i_lOrient, char *i_pString);

bool_a PDFContinueText(PDFDocPTR i_pPDFDoc, char *i_pczString);

bool_a PDFSetLeading(PDFDocPTR i_pPDFDoc, float i_fLeading);

void PDFWordSpacing(PDFDocPTR i_pPDFDoc, float i_fSpacing);

bool_a PDFNextLine(PDFDocPTR i_pPDFDoc);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//TextBox
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool_a PDFMoveTextBoxTo(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY, CPDFTextBox *i_pTextBox);

bool_a PDFJoinTextBox(CPDFTextBox *i_pParentTB, CPDFTextBox *i_pChildTB);

void PDFCopyTextBox(CPDFTextBox *i_pTextBoxTo, CPDFTextBox *i_pTextBoxFrom);

bool_a PDFCopyTextBoxAttributes(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBoxTo, CPDFTextBox *i_pTextBoxFrom);

CPDFTextBox *PDFUnJoinTextBox(CPDFTextBox *i_pTextBox);

long PDFAddTextToTextBox(PDFDocPTR i_pPDFDoc, CPDFString i_Text, CPDFTextBox *i_pTextBox);

bool_a PDFSetTextBoxBorder(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, float i_fLeft, float i_fTop,
                         float i_fRight, float i_fBottom);

bool_a PDFGetTextBoxBorder(CPDFTextBox *i_pTextBox, float *o_fLeft, float *o_fTop,
                                          float *o_fRight, float *o_fBottom);

bool_a PDFSetTextBoxMargins(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lLeft, long i_lTop,
                         long i_lRight, long i_lBottom);

bool_a PDFGetTextBoxMargins(CPDFTextBox *i_pTextBox, long *o_lLeft, long *o_lTop,
                                          long *o_lRight, long *o_lBottom);

bool_a PDFAddImageToTextBox(PDFDocPTR i_pPDFDoc, char *i_pImageName, CPDFTextBox *i_pTextBox,
                              long i_lLeftPos, long i_lTopPos, long i_lWidth, long i_lHeight,
                              long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                              bool_a i_bAddToLine);

bool_a PDFAddScaledImageToTextBox(PDFDocPTR i_pPDFDoc, char *i_pImageName, CPDFTextBox *i_pTextBox,
                              long i_lLeftPos, long i_lTopPos, long i_lScaleX, long i_lScaleY,
                              long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                              bool_a i_bAddToLine);

bool_a PDFAddLink(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lIndex);

bool_a PDFAddFillBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lBreakCount);

bool_a PDFAddAnchor(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, std::string i_strAnchor);

//bool_a PDFReplaceBookmarks(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox);

bool_a PDFAddPageMark(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, ePAGEMARKTYPE i_eType);

bool_a PDFAppendEmptyLine(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lWidth, long i_lHeight);

bool_a PDFAppendLineElement(PDFDocPTR i_pPDFDoc, long i_lWidth, CPDFString i_String,
                            bool_a i_bBlockEnd, short sBlock, CPDFTextBox *i_pTextBox);

bool_a PDFInsertLine(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox);

PDFLine *PDFCreateLine(PDFDocPTR i_pPDFDoc, PDFParagraph *i_pParagraph, bool_a i_bFirstLine);

PDFParagraph *PDFCreateParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox);

bool_a PDFAddParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox);

bool_a PDFAddBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox);

bool_a PDFAddColumnBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox);

bool_a PDFAddCanPageBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox);

void PDFCanPageBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, bool_a i_bDelete);

bool_a PDFInsertBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBoxFrom, CPDFTextBox *i_pTextBoxTo, PDFLineElemList *i_pElem);

bool_a PDFCopyOverflowLines(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBoxFrom, CPDFTextBox *i_pTextBoxTo,
                            bool_a i_bCheckBreak, long i_lHeight);

bool_a PDFCopyLineElement(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox,
                          PDFLineElemList *i_pElement, CPDFTextBox *i_pTextBoxFrom, bool_a i_bReplacePageMark);

PDFLineElemList *PDFCreateCopyElemList(PDFLineElemList *i_pElemList);

bool_a PDFAddToLineElemList(PDFDocPTR i_pPDFDoc, PDFLine *i_pLine, long i_lWidth, long i_lHeight,
                       CPDFString *i_pStr, long i_lImage, rec_a *i_precImageDist, long i_lSpace, CPDFTextBox *i_pTextBox);

bool_a PDFAppendEmptyLineElement(PDFDocPTR i_pPDFDoc, bool_a i_bBlockEnd, short i_sBlock,
                                 bool_a i_bTabStop, short i_sTab, CPDFTextBox *i_pTextBox);

bool_a PDFFreeTBImageList(CPDFTextBox *i_pTextBox);

bool_a PDFFreeTBSpaceList(CPDFTextBox *i_pTextBox);

bool_a PDFAddToTBImageList(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lIndex,
                           rec_a i_recImage, rec_a i_recDist);

bool_a PDFIsParagraphEmpty(PDFParagraph *i_pPara);

bool_a PDFIsLastLineEmpty(CPDFTextBox *i_pTextBox);

bool_a PDFIsTextBoxEmpty(CPDFTextBox *i_pTextBox);

bool_a PDFNeededParagraph(CPDFTextBox *i_pTextBox);

PDFTBSpace *PDFGetTBSpace(CPDFTextBox *i_pTextBox, long i_lID);

PDFParagraph *PDFGetTBSpaceParagraph(CPDFTextBox *i_pTextBox, long i_lID);

void PDFDeleteTBSpace(CPDFTextBox *i_pTextBox, long i_lID);

bool_a PDFAddToTBSpaceList(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, rec_a i_recSpace,
                                          long i_lID, eSPACETYPE i_eSpaceType);

bool_a PDFFreeTBRanges(rec_a ***i_pppRec);

rec_a **PDFGetTBRanges(long i_lYBegin, long i_lYEnd, CPDFTextBox *i_pTextBox, short *o_pCount);

bool_a PDFSetParaAlignment(CPDFTextBox *i_pTextBox, short i_sAlignment);

bool_a PDFSetVerticalAlignment(CPDFTextBox *i_pTextBox, short i_sVAlignment);

bool_a PDFGetTextBoxActualSize(CPDFTextBox *i_pTextBox, long *o_plWidth, long *o_plHeight);

bool_a PDFSetParaLineSpace(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lLineSpace);

bool_a PDFSetTextDirection(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, eTEXTDIRECTION i_eDirection);

bool_a PDFSetParaIndents(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lLeft, long i_lTop,
                         long i_lRight, long i_lBottom, long i_lFirstIndent);

bool_a PDFGetParaIndents(CPDFTextBox *i_pTextBox, long *o_lLeft, long *o_lTop,
                                        long *o_lRight, long *o_lBottom, long *o_lFirstIndent);

bool_a PDFSetTextBoxTabs(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFTabStop *i_pTabs, short i_sTabsCount);

bool_a PDFSetTabDefault(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lTabSize, char i_cTabDec);

bool_a PDFInsertTextBoxTab(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFTabStop i_Tab);

short PDFGetNearestTabStop(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, short i_sTab);

short PDFGetTabStopIndex(PDFTabStop *i_pTabStops, short i_sTabCount, long i_lWidth, long i_lTabSize);

PDFTabStop PDFGetTabStop(PDFTabStop *i_pTabStops, short i_sTabCount, short i_sTab, long i_lTabSize);

bool_a PDFAddTabStop(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, short sTab);

bool_a PDFSetTextBoxTransparentBg(CPDFTextBox *i_pTextBox, bool_a i_bTransparent);

bool_a PDFHasTextBoxTransparentBg(CPDFTextBox *i_pTextBox);

bool_a PDFGetTextBoxBgImage(CPDFTextBox *i_pTextBox, long *o_plTB);

bool_a PDFSetTextBoxBgImage(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, const char *i_pczImageName);

bool_a PDFClearTextBoxBgImage(CPDFTextBox *i_pTextBox);

bool_a PDFDeleteLastParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox);

bool_a PDFCopyParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pDestTextBox, PDFParagraph *i_pSourceParagraph,
                        CPDFTextBox *i_pTextBoxFrom, bool_a i_bReplacePageMark, bool_a i_bNewParagraph);

bool_a PDFMoveTextBoxContents(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBoxFrom, CPDFTextBox *i_pTextBoxTo);

bool_a PDFAdjustParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lTop, bool_a i_bReplacePageMark);

bool_a PDFFreeParagraph(PDFParagraph **i_ppParagraph);

bool_a PDFDeleteLineElemList(PDFLineElemList **i_ppElemList);


PDFLineElemList *PDFFlushStringToFile(PDFDocPTR i_pPDFDoc, PDFLineElemList *i_pElemList, long i_lX,
                                      long i_lY, long i_lFirstIndent, CPDFTextBox *i_pTextBox,
                                      PDFParagraph *i_pParagraph, PDFLine *i_pLine, long *i_pAnnot);

long PDFGetStartPos(CPDFTextBox *i_pTextBox, PDFParagraph *i_pParagraph,
                      PDFLine *i_pLine, PDFLineElemList *i_pLineElem);

PDFLineElemList *PDFGetFixTableElem(CPDFTextBox *i_pTextBox, PDFLine *i_pLine);

long PDFGetBlockWidth(PDFLineElemList *i_pElemList, short sCurrBlock);

long PDFGetWidthAfterTab(PDFLineElemList *i_pElemList, short i_sTab);

long PDFGetLineWidth(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFLineElemList *i_pElemList,
                      long *o_plSpaceWidth, PDFParagraph *i_pParagraph);

long PDFGetLineWidthToTab(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFLineElemList *i_pElemList,
                            PDFParagraph *i_pParagraph, short i_sTab);

long PDFGetDecimalStopWidthAfterTab(PDFDocPTR i_pPDFDoc, PDFLineElemList *i_pElemList, short i_sTab, char i_cDecimalType);

long PDFGetBlockSpaces(PDFLineElemList *i_pElemList, short sCurrBlock);

long PDFGetCountSpaces(PDFLineElemList *i_pElemList);

long CalcParagraphsHeights(PDFParagraph *i_pPara);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

/*
PDFOutlineEntry *PDFaddOutlineAction(PDFDocPTR pdf, PDFOutlineEntry *afterThis, long sublevel,
                                        long open, const char *action_dict, const char *title);
*/

bool_a PDFDestSpec(char **o_pBuff, eDESTSPEC i_eDest, long i_lLeft, long i_lTop,
                   long i_lRight, long i_lBottom, float i_fZoom);


PDFCTM PDFIdentMatrix();
PDFCTM PDFMulMatrix(PDFCTM i_M1, PDFCTM i_M2);
void PDFresetTextCTM(PDFDocPTR pdf);
void PDFfile_close(PDFDocPTR pdf);
void PDFserializeOutlineEntries(long *objCount, PDFOutlineEntry *first, PDFOutlineEntry *last);
long PDFWriteCatalogObject(PDFDocPTR i_pPDFDoc, long objNumber);
long PDFWriteOutlinesObject(PDFDocPTR pdf, long objNumber);
long PDFWriteProcSetArray(PDFDocPTR pdf, long objNumber);
void PDFWriteOutlineEntries(PDFDocPTR pdf, PDFOutlineEntry *first, PDFOutlineEntry *last);
long PDFWriteOneOutlineEntry(PDFDocPTR pdf, PDFOutlineEntry *olent);
long PDFWriteXrefTrailer(PDFDocPTR pdf, long objNumber);
void PDFfreeAllOutlineEntries(PDFOutlineEntry *firstOL, PDFOutlineEntry *lastOL);
unsigned char *PDFconvertHexToBinary(const char *hexin, unsigned char *binout, long *length);
unsigned char PDFnibbleValue(unsigned char hexchar);
char *PDFescapeSpecialCharsBinary(const char *instr, long lengthIn, long *lengthOut);
char *PDFescapeSpecialChars(const char *instr);
void PDFrotate(PDFDocPTR pdf, long angle);
void PDFscale(PDFDocPTR pdf, float sx, float sy);
long PDFWriteImage(PDFDocPTR pdf, CPDFImage *i_pImg, long *i_pObjCount);

bool_a PDFAddFontToPage(PDFDocPTR i_pPDFDoc, long i_lFont, long i_lPage);
bool_a PDFHasPageFont(PDFDocPTR i_pPDFDoc, long i_lFont, long i_lPage);
bool_a PDFHasFontOnlyOnePage(PDFDocPTR i_pPDFDoc, long i_lFont);

bool_a PDFAddAnnotToPage(PDFDocPTR i_pPDFDoc, long i_lAnnot, long i_lPage);
bool_a PDFHasPageAnnot(PDFDocPTR i_pPDFDoc, long i_lAnnot, long i_lPage);

bool_a PDFAddImageToPage(PDFDocPTR i_pPDFDoc, long i_lImage, long i_lPage);
bool_a PDFHasPageImage(PDFDocPTR i_pPDFDoc, long i_lImage, long i_lPage);
bool_a PDFHasImageOnlyOnePage(PDFDocPTR i_pPDFDoc, long i_lAnnot);

void PDFSetTextMatrix(PDFDocPTR i_pPDFDoc, float i_fA, float i_fB, float i_fC, float i_fD, float i_fE, float i_fF);
void PDFSetTextMatrixL(PDFDocPTR i_pPDFDoc, long i_lA, long i_lB, long i_lC, long i_lD, long i_lE, long i_lF);
void PDFConcat(PDFDocPTR i_pPDFDoc, float i_fA, float i_fB, float i_fC, float i_fD, float i_fE, float i_fF);
void PDFConcatL(PDFDocPTR i_pPDFDoc, long i_lA, long i_lB, long i_lC, long i_lD, long i_lE, long i_lF);
void PDFTranslate(PDFDocPTR i_pPDFDoc, float i_fX, float i_fY);
void PDFTranslateL(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY);



const char *PDFGetGlyphName(unsigned short i_lCharCode, CPDFFontEncoding::eFontEncodingType i_eEncoding);
const char *PDFGetGlyphNameUnicode(const char *i_pczGlyphName);
const char *PDFGetGlyphName(long i_lHex);


unsigned char *PDFCompressStream(unsigned char *i_pucBuffer, long *io_pSize);

// MIK #ifdef __cplusplus
// MIK }
// MIK #endif

#endif  /*  __PDF_PDF_H__  */
