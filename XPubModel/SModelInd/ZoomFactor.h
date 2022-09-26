#if !defined(_ZOOMFACTOR_H_)
#define _ZOOMFACTOR_H_


#include "ModelExpImp.h"

#include "ModelDef.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CXPubMarkUp;



class XPUBMODEL_EXPORTIMPORT COneZoomFactor
{
public:
  xtstring  m_sModuleName;
  long      m_nZoomFactor;
};

typedef std::vector<COneZoomFactor*>        CAllZoomFactors;
typedef CAllZoomFactors::iterator           CAllZoomFactorsIterator;
typedef CAllZoomFactors::const_iterator     CAllZoomFactorsConstIterator;
typedef CAllZoomFactors::reverse_iterator   CAllZoomFactorsReverseIterator;



class XPUBMODEL_EXPORTIMPORT CZoomFactors
{
public:
  CZoomFactors();
  ~CZoomFactors();

  bool CopyFrom(CZoomFactors* pCopy);

  void RemoveAll();

  size_t  CountOfZoomFactors() const;
  bool    GetZoomFactor(size_t nIndex, xtstring& sModuleName, long& nZoomFactor) const;
  long    GetZoomFactor(const xtstring& sModuleName) const;
  bool    AddZoomFactor(const xtstring& sModuleName, long nZoomFactor);
  bool    SetZoomFactor(const xtstring& sModuleName, long nZoomFactor);
  bool    RemoveZoomFactor(size_t nIndex);
  bool    RemoveZoomFactor(const xtstring& sModuleName);

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

protected:
  CAllZoomFactors m_cZoomFactors;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_ZOOMFACTOR_H_)
