#define PDF_MANAGER_BUILD

#if defined(WIN32)
#include <crtdbg.h>
#endif // WIN32
#include "pdffnc.h"
#include "PDFDocument.h"
#include "PDFManager.h"


CPDFManager::CPDFManager()
{
}

CPDFManager::~CPDFManager()
{
  DeleteAllDocument();
}

void CPDFManager::DeleteAllDocument()
{
  PDFDocIterator iterator;
  char *pczFirst = NULL;
  CPDFDocument *pDoc = NULL;
  for (iterator = m_PDFDocMap.begin(); iterator != m_PDFDocMap.end(); iterator++)
  {
    pczFirst = ((char *)(iterator->first));
    _ASSERT(pczFirst);
    if (pczFirst)
      free(pczFirst);
    pDoc = ((CPDFDocument *)(iterator->second));
    _ASSERT(pDoc);
    if (pDoc)
      delete pDoc;
  }
  m_PDFDocMap.clear();
}

CPDFDocument *CPDFManager::CreateDocument(const char *i_pDocName)
{
  char *pFirst = NULL;
  CPDFDocument *pRetDoc = NULL;
  if (!i_pDocName)
    return pRetDoc;
  if (GetDocument(i_pDocName) != NULL)
    return pRetDoc;
  pFirst = PDFStrDup(i_pDocName);
  if (!pFirst)
    return pRetDoc;
  pRetDoc = new CPDFDocument();
  if (!pRetDoc)
    return pRetDoc;
  if (!pRetDoc->Initialize(i_pDocName))
  {
    delete pRetDoc;
    pRetDoc = NULL;
    free(pFirst);
    return pRetDoc;
  }
  m_PDFDocMap[pFirst] = pRetDoc;
  return pRetDoc;
}

bool_a CPDFManager::DeleteDocument(const char *i_pDocName)
{
  if (!i_pDocName)
    return false_a;
  PDFDocIterator iterator;
  CPDFDocument *pDoc = NULL;
  char *pczFirst = NULL;
  iterator = m_PDFDocMap.find(i_pDocName);
  if (iterator != m_PDFDocMap.end())
  {
    pczFirst = ((char *)(iterator->first));
    _ASSERT(pczFirst);
    if (pczFirst)
      free(pczFirst);
    pDoc = (CPDFDocument *)(*iterator).second;
    _ASSERT(pDoc);
    if (pDoc)
      delete pDoc;
    m_PDFDocMap.erase(iterator);
    return true_a;
  }
  return false_a;
}

long CPDFManager::GetDocumentCount()
{
  return (long)m_PDFDocMap.size();
}

CPDFDocument *CPDFManager::GetDocument(const char *i_pDocName)
{
  CPDFDocument *pRetDoc = NULL;
  PDFDocIterator iterator;
  if (!i_pDocName)
    return pRetDoc;
  iterator = m_PDFDocMap.find(i_pDocName);
  if (iterator == m_PDFDocMap.end())
    return pRetDoc;
  pRetDoc = (CPDFDocument *)(*iterator).second;
  _ASSERT(pRetDoc);
  return pRetDoc;
}

bool_a CPDFManager::FlushDocument(const char *i_pDocName)
{
  if (!i_pDocName)
    return false_a;
  PDFDocIterator iterator;
  iterator = m_PDFDocMap.find(i_pDocName);
  if (iterator != m_PDFDocMap.end())
  {
    CPDFDocument *pDoc = ((CPDFDocument *)iterator->second);
    _ASSERT(pDoc);
    if (pDoc && !pDoc->IsFlush())
    {
      return pDoc->Flush();
    }
  }
  return false_a;
}

bool_a CPDFManager::FlushAll()
{
  bool_a bRet = true_a;
  PDFDocIterator iterator;
  CPDFDocument *pDoc = NULL;
  for (iterator = m_PDFDocMap.begin(); iterator != m_PDFDocMap.end(); iterator++)
  {
    pDoc = ((CPDFDocument *)(iterator->second));
    _ASSERT(pDoc);
    if (pDoc && !pDoc->IsFlush())
    {
      if (!pDoc->Flush())
        bRet = false_a;
    }
  }
  return bRet;
}
