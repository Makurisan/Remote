#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"
#include "ModelDef.h"
#include "Constants.h"

CProjectConstant::CProjectConstant()
                 :m_caAssignment(CConditionOrAssignment::tCOAT_ProjConstant_Assignment, false)
{
  m_sName.clear();
  m_caAssignment.Reset();
}
CProjectConstant::~CProjectConstant()
{
}
void CProjectConstant::operator =(const CProjectConstant* pProjConstant)
{
  m_sName = pProjConstant->m_sName;
  m_caAssignment = pProjConstant->m_caAssignment;
}
bool CProjectConstant::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_sName = pXMLDoc->GetAttrib(PROJECTCONSTANT_ATTRIB_NAME);
  m_caAssignment.ReadXMLContent(pXMLDoc);
  return true;
}
bool CProjectConstant::SaveXMLDoc(CXPubMarkUp* pXMLDoc) const
{
  pXMLDoc->SetAttrib(PROJECTCONSTANT_ATTRIB_NAME, m_sName.c_str());
  m_caAssignment.SaveXMLContent(pXMLDoc);
  return true;
}











CProjectConstantArray::CProjectConstantArray()
{
  m_sArrayName = PROJ_CONSTANTS_PRODUCTION;
}

CProjectConstantArray::CProjectConstantArray(LPCXTCHAR pArrayName)
{
  m_sArrayName = pArrayName;
}

CProjectConstantArray::~CProjectConstantArray()
{
  RemoveAll();
}

void CProjectConstantArray::RemoveAll()
{
  for (CConstantMapIterarot it = m_cConstants.begin(); it != m_cConstants.end(); it++)
    delete (it->second);
  m_cConstants.erase(m_cConstants.begin(), m_cConstants.end());
}

bool CProjectConstantArray::CopyFrom(const CProjectConstantArray* pArray)
{
  RemoveAll();
  for (CConstantMapConstIterarot it = pArray->m_cConstants.begin();
      it != pArray->m_cConstants.end();
      it++)
  {
    CProjectConstant* pNew = new CProjectConstant;
    if (pNew)
    {
      *pNew = it->second;
      m_cConstants.insert(make_pair(pNew->GetName(), pNew));
    }
  }
  return true;
}

size_t CProjectConstantArray::CountOfConstants()
{
  return m_cConstants.size();
}

bool CProjectConstantArray::GetConstant(const xtstring& sName,
                                        CConditionOrAssignment& caAssignment) const 
{
  CConstantMapConstIterarot it = m_cConstants.find(sName);
  if (it == m_cConstants.end())
    return false;

  caAssignment = it->second->GetAssignment();
  return true;
}

bool CProjectConstantArray::GetConstant(size_t nIndex,
                                        xtstring& sName,
                                        CConditionOrAssignment& caAssignment) const
{
  if (nIndex >= m_cConstants.size())
    return false;
  CConstantMapConstIterarot it = m_cConstants.begin();
  for (size_t nI = 0; nI < nIndex; nI++)
    it++;
  sName = it->first;
  caAssignment = it->second->GetAssignment();
  return true;
}

bool CProjectConstantArray::SetConstant(const xtstring& sName,
                                        const CConditionOrAssignment& caAssignment)
{
  CConstantMapIterarot it = m_cConstants.find(sName);
  if (it == m_cConstants.end())
    return false;
  it->second->GetAssignment() = caAssignment;
  return true;
}

bool CProjectConstantArray::SetConstant(size_t nIndex,
                                        const CConditionOrAssignment& caAssignment)
{
  if (nIndex >= m_cConstants.size())
    return false;
  CConstantMapIterarot it = m_cConstants.begin();
  for (size_t nI = 0; nI < nIndex; nI++)
    it++;
  it->second->GetAssignment() = caAssignment;
  return true;
}

bool CProjectConstantArray::AddConstant(const xtstring& sName,
                                        const CConditionOrAssignment& caAssignment,
                                        size_t& nPos)
{
  pair<CConstantMapIterarot, bool> res;

  CProjectConstant* pNew = new CProjectConstant;
  if (!pNew)
    return false;

  pNew->SetName(sName);
  pNew->GetAssignment() = caAssignment;

  res = m_cConstants.insert(make_pair(sName, pNew));
  if (!res.second)
  {
    delete pNew;
    return false;
  }

  CConstantMapConstIterarot cit = m_cConstants.find(sName);
  CConstantMapIterarot it = m_cConstants.begin();
  for (size_t nI = 0; nI < m_cConstants.size(); nI++)
  {
    if (cit == it)
    {
      nPos = nI;
      return true;
    }
    it++;
  }
  return false;
}

bool CProjectConstantArray::RemoveConstant(size_t nIndex)
{
  if (nIndex >= m_cConstants.size())
    return false;
  CConstantMapIterarot it = m_cConstants.begin();
  for (size_t nI = 0; nI < nIndex; nI++)
    it++;
  m_cConstants.erase(it);
  return true;
}

bool CProjectConstantArray::RemoveConstant(const xtstring& sName)
{
  m_cConstants.erase(sName);
  return true;
}

bool CProjectConstantArray::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  RemoveAll();
  // XMLDoc in <Constants>
  if (pXMLDoc->FindChildElem(m_sArrayName.c_str()))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ConstantsTest> or <ConstantsProduction>

    ReadXMLDocRemainder(pXMLDoc);

    pXMLDoc->OutOfElem();
    // XMLDoc in <Constants>
  }
  return true;
}

bool CProjectConstantArray::SaveXMLDoc(CXPubMarkUp* pXMLDoc) const
{
  // XMLDoc in <Constants>
  if (pXMLDoc->AddChildElem(m_sArrayName.c_str(), _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ConstantsTest> or <ConstantsProduction>

    SaveXMLDocRemainder(pXMLDoc);

    pXMLDoc->OutOfElem();
    // XMLDoc in <Constants>
  }
  return true;
}

bool CProjectConstantArray::ReadXMLDocRemainder(CXPubMarkUp* pXMLDoc)
{
  // XMLDoc in <ConstantsLocal> or <ConstantsNet>
  while (pXMLDoc->FindChildElem(PROJECTCONSTANT_ELEMENT))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <Constant>
    CProjectConstant* pNew = new CProjectConstant;
    if (pNew)
    {
      pNew->ReadXMLDoc(pXMLDoc);
      m_cConstants.insert(make_pair(pNew->GetName(), pNew));
    }

    pXMLDoc->OutOfElem();
    // XMLDoc in <ConstantsLocal> or <ConstantsNet>
  }
  return true;
}

bool CProjectConstantArray::SaveXMLDocRemainder(CXPubMarkUp* pXMLDoc) const
{
  // XMLDoc in <ConstantsTest> or <ConstantsProduction>
  for (CConstantMapConstIterarot it = m_cConstants.begin();
      it != m_cConstants.end();
      it++)
  {
    if (pXMLDoc->AddChildElem(PROJECTCONSTANT_ELEMENT, _XT("")))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <Constant>
      it->second->SaveXMLDoc(pXMLDoc);

      pXMLDoc->OutOfElem();
      // XMLDoc in <ConstantsLocal> or <ConstantsNet>
    }
  }
  return true;
}

bool CProjectConstantArray::GetXMLDoc(xtstring& sXML) const
{
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(STANDALONE_CONSTANTS))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == STANDALONE_CONSTANTS_TAG)
  {
    if (SaveXMLDoc(&cXMLDoc))
    {
      sXML = cXMLDoc.GetDoc();
      return true;
    }
  }
  return false;
}

bool CProjectConstantArray::SetXMLDoc(const xtstring& sXML,
                                      bool bAnyConstants)
{
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXML.c_str()))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == STANDALONE_CONSTANTS_TAG)
  {
    if (bAnyConstants)
    {
      if (cXMLDoc.FindChildElem(PROJ_CONSTANTS_PRODUCTION))
      {
        cXMLDoc.IntoElem();
        // XMLDoc in <ConstantsProduction>

        ReadXMLDocRemainder(&cXMLDoc);

        cXMLDoc.OutOfElem();
        // XMLDoc in <Constants>
        return true;
      }
      else if (cXMLDoc.FindChildElem(PROJ_CONSTANTS_TEST))
      {
        cXMLDoc.IntoElem();
        // XMLDoc in <ConstantsTest>

        ReadXMLDocRemainder(&cXMLDoc);

        cXMLDoc.OutOfElem();
        // XMLDoc in <Constants>
        return true;
      }
    }
    else
    {
    if (ReadXMLDoc(&cXMLDoc))
      return true;
    }
  }
  return false;
}

