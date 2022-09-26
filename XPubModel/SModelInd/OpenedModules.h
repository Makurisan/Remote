#if !defined(_OPENEDMODULES_H_)
#define _OPENEDMODULES_H_


#include "ModelExpImp.h"

#include "ModelDef.h"



class CXPubMarkUp;


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32



class XPUBMODEL_EXPORTIMPORT COneOpenedModule
{
public:
  TProjectFileType nType;
  xtstring sModuleName;
};

typedef std::vector<COneOpenedModule*>      CAllOpenedModules;
typedef CAllOpenedModules::iterator         CAllOpenedModulesIterator;
typedef CAllOpenedModules::const_iterator   CAllOpenedModulesConstIterator;
typedef CAllOpenedModules::reverse_iterator CAllOpenedModulesReverseIterator;



class XPUBMODEL_EXPORTIMPORT COpenedModules
{
public:
  COpenedModules();
  ~COpenedModules();

  bool CopyFrom(COpenedModules* pCopy);

  void RemoveAll();

  size_t  CountOfOpenedModules() const;
  bool    GetOpenedModule(size_t nIndex, TProjectFileType& nType, xtstring& sModuleName) const;
  bool    AddOpenedModule(TProjectFileType nType, const xtstring& sModuleName);
  bool    RemoveOpenedModule(size_t nIndex);

  void ResetActiveModule();
  void SetActiveModule(TProjectFileType nType, const xtstring& sModuleName);
  void GetActiveModule(TProjectFileType& nType, xtstring& sModuleName) const;

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

protected:
  CAllOpenedModules m_cOpenedModules;
  COneOpenedModule  m_cActiveModule;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_OPENEDMODULES_H_)
