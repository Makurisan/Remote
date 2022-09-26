#ifndef __PDFMANAGER_H__
#define __PDFMANAGER_H__

#include "XPubPDFExpImp.h"
#include <map>

#if defined(WIN32)
#pragma warning(disable : 4251)
#endif // WIN32

#include "pdfdef.h"

class CPDFDocument;

struct str_comp_less
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

struct str_comp_greater
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) > 0;
  }
};


typedef std::map <const char *, CPDFDocument *, str_comp_less>             XPUBPDF_EXPORTIMPORT  PDFDocMap;
typedef std::map <const char *, CPDFDocument *, str_comp_less>::iterator   XPUBPDF_EXPORTIMPORT  PDFDocIterator;

class XPUBPDF_EXPORTIMPORT CPDFManager
{
public:
  CPDFManager();
  ~CPDFManager();

  //creates a document called i_pDocName (file's name)
  //if succeeds return value is pointer to created document or NULL if an error occurs
  CPDFDocument *CreateDocument(const char *i_pDocName);
  //deletes document called i_pDocName
  //if succeeds return value is true otherwise false
  bool_a DeleteDocument(const char *i_pDocName);
  //deletes all documents
  void DeleteAllDocument();
  //returns count of documents created by this class
  long GetDocumentCount();
  //returns pointer to document called i_pDocName
  //if succeeds return value is pointer to document otherwise NULL
  CPDFDocument *GetDocument(const char *i_pDocName);
  //write document called i_pDocName to file called "i_pDocName.pdf"
  bool_a FlushDocument(const char *i_pDocName);
  //write all documents created by this class to file
  bool_a FlushAll();

private:

  //map of documents created by this class
  PDFDocMap   m_PDFDocMap;
};

#if defined(WIN32)
#pragma warning(default : 4251)
#endif // WIN32

#endif  /*  __PDFMANAGER_H__  */
