#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"
#include "OpenedModules.h"






COpenedModules::COpenedModules()
{
  ResetActiveModule();
}

COpenedModules::~COpenedModules()
{
  RemoveAll();
}

bool COpenedModules::CopyFrom(COpenedModules* pCopy)
{
  RemoveAll();

  if (!pCopy)
    return true;

  TProjectFileType nType;
  xtstring sModuleName;

  // active module
  pCopy->GetActiveModule(nType, sModuleName);
  SetActiveModule(nType, sModuleName);

  // opened modules
  for (size_t nI = 0; nI < pCopy->CountOfOpenedModules(); nI++)
  {
    pCopy->GetOpenedModule(nI, nType, sModuleName);
    AddOpenedModule(nType, sModuleName);
  }
  return true;
}

void COpenedModules::RemoveAll()
{
  CAllOpenedModulesIterator it;
  for (it = m_cOpenedModules.begin(); it != m_cOpenedModules.end(); it++)
    delete (*it);
  m_cOpenedModules.erase(m_cOpenedModules.begin(), m_cOpenedModules.end());
}

size_t COpenedModules::CountOfOpenedModules() const
{
  return m_cOpenedModules.size();
}

bool COpenedModules::GetOpenedModule(size_t nIndex,
                                     TProjectFileType& nType,
                                     xtstring& sModuleName) const 
{
  if (nIndex >= m_cOpenedModules.size())
    return false;

  CAllOpenedModulesConstIterator it;
  it = m_cOpenedModules.begin() + nIndex;
  nType = (*it)->nType;
  sModuleName = (*it)->sModuleName;
  return true;
}

bool COpenedModules::AddOpenedModule(TProjectFileType nType,
                                     const xtstring& sModuleName)
{
  // Wir muessen zuerst kontrollieren, ob das schon da ist.
  // (Ein mal ist passiert, dass zwei Module 8.000 mal in Liste waren.)
  for (CAllOpenedModulesConstIterator it = m_cOpenedModules.begin(); it != m_cOpenedModules.end(); it++)
  {
    if (nType == (*it)->nType && sModuleName == (*it)->sModuleName)
      return true;
  }
  COneOpenedModule* pNew;
  pNew = new COneOpenedModule;
  if (!pNew)
    return false;

  pNew->nType = nType;
  pNew->sModuleName = sModuleName;

  m_cOpenedModules.push_back(pNew);
  return true;
}

bool COpenedModules::RemoveOpenedModule(size_t nIndex)
{
  if (nIndex >= m_cOpenedModules.size())
    return false;

  CAllOpenedModulesIterator it = m_cOpenedModules.begin();
  for (size_t nI = 0; nI < nIndex; nI++)
    it++;
  delete (*it);
  m_cOpenedModules.erase(it);
  return true;
}

void COpenedModules::ResetActiveModule()
{
  // da muessen wir etwas einstellen
  m_cActiveModule.nType = tProjectFile_Nothing;
  m_cActiveModule.sModuleName.clear();
}

void COpenedModules::SetActiveModule(TProjectFileType nType,
                                     const xtstring& sModuleName)
{
  m_cActiveModule.nType = nType;
  m_cActiveModule.sModuleName = sModuleName;
}

void COpenedModules::GetActiveModule(TProjectFileType& nType,
                                     xtstring& sModuleName) const
{
  nType = m_cActiveModule.nType;
  sModuleName = m_cActiveModule.sModuleName;
}

bool COpenedModules::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  TProjectFileType nType;
  xtstring sModuleName;

  // XMLDoc in <OpenedModules>

  // active module
  nType = (TProjectFileType)pXMLDoc->GetAttribLong(ACTIVE_MODULE_TYPE);
  sModuleName = pXMLDoc->GetAttrib(ACTIVE_MODULE_NAME);
  SetActiveModule(nType, sModuleName);

  while (pXMLDoc->FindChildElem(OPENED_MODULE_ELEMENT))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <OpenedModule>
    nType = (TProjectFileType)pXMLDoc->GetAttribLong(OPENED_MODULE_TYPE);
    sModuleName = pXMLDoc->GetAttrib(OPENED_MODULE_NAME);
    AddOpenedModule(nType, sModuleName);

    pXMLDoc->OutOfElem();
    // XMLDoc in <OpenedModules>
  }
  return true;
}

bool COpenedModules::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  TProjectFileType nType;
  xtstring sModuleName;

  // XMLDoc in <OpenedModules>

  // active module
  GetActiveModule(nType, sModuleName);
  pXMLDoc->SetAttrib(ACTIVE_MODULE_TYPE, nType);
  pXMLDoc->SetAttrib(ACTIVE_MODULE_NAME, sModuleName.c_str());

  for (CAllOpenedModulesIterator it = m_cOpenedModules.begin();
      it != m_cOpenedModules.end();
      it++)
  {
    if (pXMLDoc->AddChildElem(OPENED_MODULE_ELEMENT, _XT("")))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <OpenedModule>
      nType = (*it)->nType;
      sModuleName = (*it)->sModuleName;
      pXMLDoc->SetAttrib(OPENED_MODULE_TYPE, nType);
      pXMLDoc->SetAttrib(OPENED_MODULE_NAME, sModuleName.c_str());

      pXMLDoc->OutOfElem();
      // XMLDoc in <OpenedModules>
    }
  }
  return true;
}
