// MPModelPythonInd.h: interface for the CMPModelPythonInd classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MPMODELPYTHONIND_H_)
#define _MPMODELPYTHONIND_H_


#include "ModelExpImp.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CXPubMarkUp;

class CMPModelPythonInd;


class XPUBMODEL_EXPORTIMPORT CMPModelPythonInd : public CModelBaseInd
{
public:
  CMPModelPythonInd();
  CMPModelPythonInd(CMPModelPythonInd& cModel);
  CMPModelPythonInd(CMPModelPythonInd* pModel);
  virtual ~CMPModelPythonInd();

  void SetMPModelPythonInd(CMPModelPythonInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);
  static bool ReadXMLModuleName(CXPubMarkUp *pXMLDoc, xtstring& sModuleName);


  /////////////////////////////////////////////////////////////
  // interface for common properties
  // Name
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};

private:
  // common properties
  xtstring m_sName;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_MPMODELPYTHONIND_H_)
