
#include "pdferr.h"
#include "pdffnc.h"


void PDFresetTextCTM(PDFDocPTR pdf)
{
    pdf->strTextCTM.m_fA = 1.0; pdf->strTextCTM.m_fB = 0.0;
    pdf->strTextCTM.m_fC = 0.0; pdf->strTextCTM.m_fD = 1.0;
    pdf->strTextCTM.m_fE = 0.0; pdf->strTextCTM.m_fF = 0.0;
}

void PDFConcat(PDFDocPTR i_pPDFDoc, float i_fA, float i_fB, float i_fC, float i_fD, float i_fE, float i_fF)
{
  char czTemp[201] = {0};
  i_fE = PDFConvertDimensionToDefault(i_fE);
  i_fF = PDFConvertDimensionToDefault(i_fF);
  sprintf(czTemp, "%.4f %.4f %.4f %.4f %.4f %.4f %s\n",
          i_fA, i_fB, i_fC, i_fD, i_fE, i_fF, czMODIFYCTM);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
}

void PDFConcatL(PDFDocPTR i_pPDFDoc, long i_lA, long i_lB, long i_lC, long i_lD, long i_lE, long i_lF)
{
  PDFConcat(i_pPDFDoc, (float)i_lA, (float)i_lB, (float)i_lC, (float)i_lD, (float)i_lE, (float)i_lF);
}

void PDFrotate(PDFDocPTR pdf, long angle)
{
  float vsin, vcos, fAngle;

  fAngle = (float)(angle * (PI / 180.0));
  vsin = (float)sin(fAngle);
  vcos = (float)cos(fAngle);
  PDFConcat(pdf, vcos, vsin, -vsin, vcos, 0.0, 0.0);
}

void PDFSaveGraphicsState(PDFDocPTR i_pPDFDoc)
{
  PDFWriteToPageStream(i_pPDFDoc, "\nq\n", strlen("\nq\n"), i_pPDFDoc->m_lActualPage);
}

void PDFTranslate(PDFDocPTR i_pPDFDoc, float i_fX, float i_fY)
{
  PDFConcat(i_pPDFDoc, 1.0, 0.0, 0.0, 1.0, i_fX, i_fY);
}

void PDFTranslateL(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY)
{
   PDFTranslate(i_pPDFDoc, (float)i_lX, (float)i_lY);
}

void PDFscale(PDFDocPTR pdf, float sx, float sy)
{
  PDFConcat(pdf, sx, 0.0, 0.0, sy, 0.0, 0.0);
}

void PDFSetClippingRect(PDFDocPTR i_pPDFDoc, rec_a i_recPosSize)
{
  char  czTemp[201]   = {0};
  float fTop, fBottom, fLeft, fRight;

  fTop = PDFConvertDimensionToDefault((float)i_recPosSize.lTop);
  fBottom = PDFConvertDimensionToDefault((float)i_recPosSize.lBottom);
  fLeft = PDFConvertDimensionToDefault((float)i_recPosSize.lLeft);
  fRight = PDFConvertDimensionToDefault((float)i_recPosSize.lRight);
  sprintf(czTemp, "%.4f %.4f m\n%.4f %.4f l\n%.4f %.4f l\n%.4f %.4f l\nW\nn\n",
      fLeft, fBottom, fLeft, fTop, fRight, fTop, fRight, fBottom);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
}

void PDFRestoreGraphicsState(PDFDocPTR i_pPDFDoc)
{
  PDFWriteToPageStream(i_pPDFDoc, "Q\n", strlen("Q\n"), i_pPDFDoc->m_lActualPage);
}

bool_a PDFRectangle(PDFDocPTR i_pPDFDoc, frec_a i_Rec, bool_a i_bFill)
{
  char czTemp[201] = {0};
  float fTop, fBottom, fLeft, fRight;

  fTop = PDFConvertDimensionToDefault(i_Rec.fTop);
  fBottom = PDFConvertDimensionToDefault(i_Rec.fBottom);
  fLeft = PDFConvertDimensionToDefault(i_Rec.fLeft);
  fRight = PDFConvertDimensionToDefault(i_Rec.fRight);

  if (i_bFill)
  {
    sprintf(czTemp, "%.4f %.4f %.4f %.4f re B*\n", fLeft, fBottom, (fRight - fLeft), (fTop - fBottom));
    PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
  }
  else
  {
    sprintf(czTemp, "%.4f %.4f %.4f %.4f re S\n\n", fLeft, fBottom, (fRight - fLeft), (fTop - fBottom));
    PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
  }

  return true_a;
}

bool_a PDFRectangleL(PDFDocPTR i_pPDFDoc, rec_a i_Rec, bool_a i_bFill)
{
  frec_a frec;
  frec.fBottom = (float)i_Rec.lBottom;
  frec.fLeft = (float)i_Rec.lLeft;
  frec.fRight = (float)i_Rec.lRight;
  frec.fTop = (float)i_Rec.lTop;
  return PDFRectangle(i_pPDFDoc, frec, i_bFill);
}

bool_a PDFSetDashedLine(PDFDocPTR i_pPDFDoc)
{
  char czTemp[201] = {0};
  sprintf(czTemp, "[3] 0 d\n");
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
  return true_a;
}

bool_a PDFSetSolidLine(PDFDocPTR i_pPDFDoc)
{
  char czTemp[201] = {0};
  sprintf(czTemp, "[] 0 d\n");
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
  return true_a;
}

bool_a PDFMoveTo(PDFDocPTR i_pPDFDoc, float i_fX, float i_fY)
{
  char czTemp[201] = {0};

  i_fX = PDFConvertDimensionToDefault(i_fX);
  i_fY = PDFConvertDimensionToDefault(i_fY);
  sprintf(czTemp, "%.4f %.4f m\n", i_fX, i_fY);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);

  return true_a;
}

bool_a PDFMoveToL(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY)
{
  return PDFMoveTo(i_pPDFDoc, (float)i_lX, (float)i_lY);
}

bool_a PDFLineTo(PDFDocPTR i_pPDFDoc, float i_fX, float i_fY)
{
  char czTemp[201] = {0};

  i_fX = PDFConvertDimensionToDefault(i_fX);
  i_fY = PDFConvertDimensionToDefault(i_fY);
  sprintf(czTemp, "%.4f %.4f l S 0 J\n", i_fX, i_fY);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);

  return true_a;
}

bool_a PDFLineToL(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY)
{
  return PDFLineTo(i_pPDFDoc, (float)i_lX, (float)i_lY);
}

bool_a PDFSetLineWidth(PDFDocPTR i_pPDFDoc, float i_fWidth)
{
  char czTemp[201] = {0};

  i_fWidth = PDFConvertDimensionToDefault(i_fWidth);
  sprintf(czTemp, "%.4f w\n", i_fWidth);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);

  return true_a;
}

bool_a PDFSetLineWidthL(PDFDocPTR i_pPDFDoc, long i_lWidth)
{
  return PDFSetLineWidth(i_pPDFDoc, (float)i_lWidth);
}

PDFCTM PDFIdentMatrix()
{
  PDFCTM retM(1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0);

  return retM;
}

PDFCTM PDFMulMatrix(PDFCTM i_M1, PDFCTM i_M2)
{
  float A[3][3];
  float B[3][3];
  float C[3][3];
  int x, y, i;
  PDFCTM retM;

  A[0][0] = i_M1.m_fA; A[0][1] = i_M1.m_fB; A[0][2] = 0;
  A[1][0] = i_M1.m_fC; A[1][1] = i_M1.m_fD; A[1][2] = 0;
  A[2][0] = i_M1.m_fE; A[2][1] = i_M1.m_fF; A[2][2] = 1;

  B[0][0] = i_M2.m_fA; B[0][1] = i_M2.m_fB; B[0][2] = 0;
  B[1][0] = i_M2.m_fC; B[1][1] = i_M2.m_fD; B[1][2] = 0;
  B[2][0] = i_M2.m_fE; B[2][1] = i_M2.m_fF; B[2][2] = 1;

  //A*B:
  for(x = 0; x < 3; x++)
  {
    for(y = 0; y < 3; y++)
    {
      C[x][y]=0;
      for(i = 0; i < 3; i++)
      {
        C[x][y] += A[x][i] * B[i][y];
      }
    }
  }
  retM.m_fA = C[0][0]; retM.m_fB = C[0][1];
  retM.m_fC = C[1][0]; retM.m_fD = C[1][1];
  retM.m_fE = C[2][0]; retM.m_fF = C[2][1];

  return retM;
}

