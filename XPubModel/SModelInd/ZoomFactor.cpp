#include <assert.h>
#include "SSTLdef/STLdef.h"
#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"
#include "ZoomFactor.h"






CZoomFactors::CZoomFactors()
{
}

CZoomFactors::~CZoomFactors()
{
  RemoveAll();
}

bool CZoomFactors::CopyFrom(CZoomFactors* pCopy)
{
  RemoveAll();

  if (!pCopy)
    return true;

  xtstring sModuleName;
  long nZoomFactor;

  // opened modules
  for (size_t nI = 0; nI < pCopy->CountOfZoomFactors(); nI++)
  {
    pCopy->GetZoomFactor(nI, sModuleName, nZoomFactor);
    AddZoomFactor(sModuleName, nZoomFactor);
  }
  return true;
}

void CZoomFactors::RemoveAll()
{
  CAllZoomFactorsIterator it;
  for (it = m_cZoomFactors.begin(); it != m_cZoomFactors.end(); it++)
    delete (*it);
  m_cZoomFactors.erase(m_cZoomFactors.begin(), m_cZoomFactors.end());
}

size_t CZoomFactors::CountOfZoomFactors() const
{
  return m_cZoomFactors.size();
}

bool CZoomFactors::GetZoomFactor(size_t nIndex,
                                 xtstring& sModuleName,
                                 long& nZoomFactor) const 
{
  if (nIndex >= m_cZoomFactors.size())
    return false;

  CAllZoomFactorsConstIterator it;
  it = m_cZoomFactors.begin() + nIndex;
  sModuleName = (*it)->m_sModuleName;
  nZoomFactor = (*it)->m_nZoomFactor;
  return true;
}

long CZoomFactors::GetZoomFactor(const xtstring& sModuleName) const
{
  CAllZoomFactorsConstIterator it = m_cZoomFactors.begin();
  while (it != m_cZoomFactors.end())
  {
    if ((*it)->m_sModuleName == sModuleName)
      return (*it)->m_nZoomFactor;
    it++;
  }

  return 100;
}

bool CZoomFactors::AddZoomFactor(const xtstring& sModuleName,
                                 long nZoomFactor)
{
  // Wir muessen zuerst kontrollieren, ob das schon da ist.
  // (Ein mal ist passiert, dass zwei Module 8.000 mal in Liste waren.)
  for (CAllZoomFactorsConstIterator it = m_cZoomFactors.begin(); it != m_cZoomFactors.end(); it++)
  {
    if (sModuleName == (*it)->m_sModuleName)
    {
      SetZoomFactor(sModuleName, nZoomFactor);
      return true;
    }
  }
  COneZoomFactor* pNew;
  pNew = new COneZoomFactor;
  if (!pNew)
    return false;

  pNew->m_sModuleName = sModuleName;
  pNew->m_nZoomFactor = nZoomFactor;

  m_cZoomFactors.push_back(pNew);
  return true;
}

bool CZoomFactors::SetZoomFactor(const xtstring& sModuleName,
                                 long nZoomFactor)
{
  CAllZoomFactorsConstIterator it = m_cZoomFactors.begin();
  while (it != m_cZoomFactors.end())
  {
    if ((*it)->m_sModuleName == sModuleName)
    {
      (*it)->m_nZoomFactor = nZoomFactor;
      return true;
    }
    it++;
  }
  return AddZoomFactor(sModuleName, nZoomFactor);
}

bool CZoomFactors::RemoveZoomFactor(size_t nIndex)
{
  if (nIndex >= m_cZoomFactors.size())
    return false;

  CAllZoomFactorsIterator it = m_cZoomFactors.begin();
  for (size_t nI = 0; nI < nIndex; nI++)
    it++;
  delete (*it);
  m_cZoomFactors.erase(it);
  return true;
}

bool CZoomFactors::RemoveZoomFactor(const xtstring& sModuleName)
{
  CAllZoomFactorsIterator it = m_cZoomFactors.begin();
  while (it != m_cZoomFactors.end())
  {
    if ((*it)->m_sModuleName == sModuleName)
    {
      delete (*it);
      m_cZoomFactors.erase(it);
      return true;
    }
  }
  return false;
}

bool CZoomFactors::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  xtstring sModuleName;
  long nZoomFactor;

  // XMLDoc in <ZoomFactors-XX>
  while (pXMLDoc->FindChildElem(ZOOMFACTOR_ELEMENT))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ZoomFactor>
    sModuleName = pXMLDoc->GetAttrib(ZOOMFACTOR_MODULE_NAME);
    nZoomFactor = pXMLDoc->GetAttribLong(ZOOMFACTOR_ZOOM_FACTOR);
    AddZoomFactor(sModuleName, nZoomFactor);

    pXMLDoc->OutOfElem();
    // XMLDoc in <ZoomFactors-XX>
  }
  return true;
}

bool CZoomFactors::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  xtstring sModuleName;
  long nZoomFactor;

  // XMLDoc in <ZoomFactors-XX>
  for (CAllZoomFactorsIterator it = m_cZoomFactors.begin();
      it != m_cZoomFactors.end();
      it++)
  {
    if (pXMLDoc->AddChildElem(ZOOMFACTOR_ELEMENT, _XT("")))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <ZoomFactor>
      sModuleName = (*it)->m_sModuleName;
      nZoomFactor = (*it)->m_nZoomFactor;
      pXMLDoc->SetAttrib(ZOOMFACTOR_MODULE_NAME, sModuleName.c_str());
      pXMLDoc->SetAttrib(ZOOMFACTOR_ZOOM_FACTOR, nZoomFactor);

      pXMLDoc->OutOfElem();
      // XMLDoc in <ZoomFactors-XX>
    }
  }
  return true;
}
