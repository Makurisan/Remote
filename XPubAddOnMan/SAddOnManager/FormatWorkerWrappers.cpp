#include <assert.h>
#include "SSTLdef/STLdef.h"
#include "STools/Utils.h"
#include "SModelInd/ModelDef.h"
#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"

#include "XPubAddOnDeclarations.h"
#include "XPubAddOnManager.h"
#include "WorkPaperProcessor.h"
#include "TagBuilder.h"

#include "FormatWorker.h"
#include "FormatWorkerWrappers.h"

#include "SExpression/Expression.h"




#define ADDRESS_DELIMITER       _XT("/")

























////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CFormatWorkerWrapper
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CFormatWorkerWrapper::CFormatWorkerWrapper(TTypeOfFWWrapper nTypeOfObject,
                                           CFormatWorkerWrapper_Root* pRoot,
                                           CFormatWorkerWrapper* pParent)
{
  m_nTypeOfObject = nTypeOfObject;
  m_pRoot = pRoot;
  m_pParent = pParent;

  m_pFormatWorker = 0;

  m_nCRC32 = 0;

  RemoveNewValues();
  ClearElementID();
  ResetReferenceCounter();
  m_nStatusOfObject = tSOFWW_Default;
  m_nGUITreeStatus = tGTS_Default;
}

CFormatWorkerWrapper::CFormatWorkerWrapper(const CFormatWorkerWrapper& cWrapper)
{
  m_nTypeOfObject = cWrapper.m_nTypeOfObject;
  m_pRoot = cWrapper.m_pRoot;
  m_pParent = cWrapper.m_pParent;

  m_pFormatWorker = 0;

  m_nCRC32 = 0;

  try
  {
    if (m_pRoot && m_pRoot->GetAddOnManager() && cWrapper.m_pFormatWorker)
      m_pFormatWorker = m_pRoot->GetAddOnManager()->CreateFormatWorkerClone(cWrapper.m_pFormatWorker);
  }
  catch (...)
  {
    m_pFormatWorker = 0;
  }

  for (CCRC32sConstIterator it = cWrapper.m_cCRC32.begin(); it != cWrapper.m_cCRC32.end(); it++)
    m_cCRC32[it->first] = it->second;
  for (CNewValuesConstIterator it = cWrapper.m_cNewValues.begin(); it != cWrapper.m_cNewValues.end(); it++)
    m_cNewValues[it->first] = it->second;
  for (CNewStatusConstIterator it = cWrapper.m_cNewStatus.begin(); it != cWrapper.m_cNewStatus.end(); it++)
    m_cNewStatus[it->first] = it->second;
  m_sElementID = cWrapper.m_sElementID;
  m_sClientAddress = cWrapper.m_sClientAddress;
  ResetReferenceCounter();
  m_nStatusOfObject = cWrapper.m_nStatusOfObject;
  m_nGUITreeStatus = cWrapper.m_nGUITreeStatus;

  for (CFieldWrappersConstIterator it = cWrapper.m_cFieldWrappers.begin(); it != cWrapper.m_cFieldWrappers.end(); it++)
  {
    CFormatWorkerWrapper_Field* pNew = new CFormatWorkerWrapper_Field(*(*it));
    if (pNew)
      AddFieldWrapper(pNew);
  }
  for (CStoneWrappersConstIterator it = cWrapper.m_cStoneWrappers.begin(); it != cWrapper.m_cStoneWrappers.end(); it++)
  {
    CFormatWorkerWrapper_Stone* pNew = new CFormatWorkerWrapper_Stone(*(*it));
    if (pNew)
      AddStoneWrapper(pNew);
  }
  for (CTemplateWrappersConstIterator it = cWrapper.m_cTemplateWrappers.begin(); it != cWrapper.m_cTemplateWrappers.end(); it++)
  {
    CFormatWorkerWrapper_Template* pNew = new CFormatWorkerWrapper_Template(*(*it));
    if (pNew)
      AddTemplateWrapper(pNew);
  }

}

CFormatWorkerWrapper::~CFormatWorkerWrapper()
{
  RemoveFieldWrappers();
  RemoveStoneWrappers();
  RemoveTemplateWrappers();
  try
  {
    if (m_pRoot && m_pRoot->GetAddOnManager() && m_pFormatWorker)
      m_pRoot->GetAddOnManager()->ReleaseFormatWorker(m_pFormatWorker);
    m_pFormatWorker = 0;
  }
  catch (...)
  {
  }
}



bool CFormatWorkerWrapper::AddWrapper(CFormatWorkerWrapper* pWrapper)
{
  if (!pWrapper)
    return false;
  if (pWrapper->GetTypeOfObject() == tTOFWW_Template)
    return AddTemplateWrapper(dynamic_cast<CFormatWorkerWrapper_Template*>(pWrapper));
  else if (pWrapper->GetTypeOfObject() == tTOFWW_Stone)
    return AddStoneWrapper(dynamic_cast<CFormatWorkerWrapper_Stone*>(pWrapper));
  else if (pWrapper->GetTypeOfObject() == tTOFWW_Field)
    return AddFieldWrapper(dynamic_cast<CFormatWorkerWrapper_Field*>(pWrapper));
  return false;
}

CFormatWorkerWrapper_Field* CFormatWorkerWrapper::GetFieldWrapper(size_t nIndex)
{
  if (nIndex < m_cFieldWrappers.size())
    return m_cFieldWrappers[nIndex];
  else
    return 0;
}
bool CFormatWorkerWrapper::AddFieldWrapper(CFormatWorkerWrapper_Field* pWrapper)
{
  if (!pWrapper)
    return false;
  size_t nCount = CountOfFieldWrappers();
  m_cFieldWrappers.push_back(pWrapper);
  if (nCount == CountOfFieldWrappers())
    return false;
  return true;
}
bool CFormatWorkerWrapper::RemoveFieldWrappers()
{
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    delete (*it);
  m_cFieldWrappers.erase(m_cFieldWrappers.begin(), m_cFieldWrappers.end());
  return true;
}




CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetStoneWrapper(size_t nIndex)
{
  if (nIndex < m_cStoneWrappers.size())
    return m_cStoneWrappers[nIndex];
  else
    return 0;
}
CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetFirstTAGStoneWrapper()
{
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->GetFormatWorkerType() == tDataFormatST_Tag)
      return (*it);
  }
  return 0;
}
CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetFirstTAGStoneWrapper(const xtstring& sStoneName)
{
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->GetFormatWorkerType() == tDataFormatST_Tag)
    {
      if ((*it)->GetName() == sStoneName)
        return (*it);
    }
  }
  return 0;
}
bool CFormatWorkerWrapper::AddStoneWrapper(CFormatWorkerWrapper_Stone* pWrapper)
{
  if (!pWrapper)
    return false;
  size_t nCount = CountOfStoneWrappers();
  m_cStoneWrappers.push_back(pWrapper);
  if (nCount == CountOfStoneWrappers())
    return false;
  return true;
}
bool CFormatWorkerWrapper::RemoveStoneWrappers()
{
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    delete (*it);
  m_cStoneWrappers.erase(m_cStoneWrappers.begin(), m_cStoneWrappers.end());
  return true;
}
bool CFormatWorkerWrapper::RemoveAllNonPropertyStones()
{
  bool bRemoved = true;
  while (bRemoved)
  {
    bRemoved = false;
    for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    {
      if ((*it)->GetFormatWorkerType() == tDataFormatST_Property)
        continue;
      delete (*it);
      m_cStoneWrappers.erase(it);
      bRemoved = true;
      break;
    }
  }
  return true;
}




CFormatWorkerWrapper_Template* CFormatWorkerWrapper::GetTemplateWrapper(size_t nIndex)
{
  if (nIndex < m_cTemplateWrappers.size())
    return m_cTemplateWrappers[nIndex];
  else
    return 0;
}
bool CFormatWorkerWrapper::AddTemplateWrapper(CFormatWorkerWrapper_Template* pWrapper)
{
  if (!pWrapper)
    return false;
  size_t nCount = CountOfTemplateWrappers();
  m_cTemplateWrappers.push_back(pWrapper);
  if (nCount == CountOfTemplateWrappers())
    return false;
  return true;
}
bool CFormatWorkerWrapper::RemoveTemplateWrappers()
{
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    delete (*it);
  m_cTemplateWrappers.erase(m_cTemplateWrappers.begin(), m_cTemplateWrappers.end());
  return true;
}

bool CFormatWorkerWrapper::ReadXMLContent(CXPubMarkUp* pXMLDoc)
{
  if (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_BASE))
  {
    pXMLDoc->IntoElem();

    long n;
    m_sElementID = pXMLDoc->GetAttrib(WORKPAPERWRAPPER_ELEMENTID);
    m_sClientAddress = pXMLDoc->GetAttrib(WORKPAPERWRAPPER_CLIENTADDRESS);
    n = pXMLDoc->GetAttribLong(WORKPAPERWRAPPER_GUITREESTATUS);
    if (n <= tGTS_FirstDummy || n >= tGTS_LastDummy)
      n = tGTS_Default;
    m_nGUITreeStatus = (TGUITreeStatus)n;

    if (m_pRoot && m_pRoot->GetAddOnManager())
    {
      CFormatWorker* pFrm = m_pRoot->GetAddOnManager()->ReadWorkPaperFromMemory(pXMLDoc);
      if (pFrm)
        m_pFormatWorker = pFrm;
    }

    pXMLDoc->OutOfElem();
  }
  else
    return false;
  return true;
}
bool CFormatWorkerWrapper::SaveXMLContent(CXPubMarkUp* pXMLDoc)
{
  bool bRet = true;
  if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_BASE, _XT("")))
  {
    pXMLDoc->IntoElem();

    pXMLDoc->SetAttrib(WORKPAPERWRAPPER_ELEMENTID, m_sElementID.c_str());
    pXMLDoc->SetAttrib(WORKPAPERWRAPPER_CLIENTADDRESS, m_sClientAddress.c_str());
    pXMLDoc->SetAttrib(WORKPAPERWRAPPER_GUITREESTATUS, m_nGUITreeStatus);

    if (m_pFormatWorker)
      bRet = m_pFormatWorker->SaveWorkPaperToMemory(pXMLDoc);

    pXMLDoc->OutOfElem();
  }
  else
    return false;
  return bRet;
}
void CFormatWorkerWrapper::ResetInternalVariables()
{
  if (GetFormatWorker())
    GetFormatWorker()->ResetInternalVariables();
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    (*it)->ResetInternalVariables();
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    (*it)->ResetInternalVariables();
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    (*it)->ResetInternalVariables();
}

void CFormatWorkerWrapper::SetVariable_DocBeginPageNumber(const int nDocBeginPageNumber)
{
  if (GetFormatWorker())
    GetFormatWorker()->SetDocumentBeginPageNumber(nDocBeginPageNumber);
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    (*it)->SetVariable_DocBeginPageNumber(nDocBeginPageNumber);
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    (*it)->SetVariable_DocBeginPageNumber(nDocBeginPageNumber);
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    (*it)->SetVariable_DocBeginPageNumber(nDocBeginPageNumber);
}

void CFormatWorkerWrapper::SetVariable_TreeStartActiontyp(int nTreeStartActiontyp)
{
  if (GetFormatWorker())
    GetFormatWorker()->SetTreeStartActionTyp(nTreeStartActiontyp);
}
void CFormatWorkerWrapper::SetVariable_TreeStartElementtyp(const xtstring& sTreeStartElementtyp)
{
  if (GetFormatWorker())
    GetFormatWorker()->SetTreeStartElementtyp(sTreeStartElementtyp);
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    (*it)->SetVariable_TreeStartElementtyp(sTreeStartElementtyp);
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    (*it)->SetVariable_TreeStartElementtyp(sTreeStartElementtyp);
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    (*it)->SetVariable_TreeStartElementtyp(sTreeStartElementtyp);
}
void CFormatWorkerWrapper::SetVariable_TextFocusActive(bool bTextFocusActive)
{
  if (GetFormatWorker())
    GetFormatWorker()->SetTextFocusActive(bTextFocusActive);
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    (*it)->SetVariable_TextFocusActive(bTextFocusActive);
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    (*it)->SetVariable_TextFocusActive(bTextFocusActive);
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    (*it)->SetVariable_TextFocusActive(bTextFocusActive);
}

xtstring CFormatWorkerWrapper::GetElementIDForReplace_Front()
{
  xtstring sRet;
  sRet.clear();
  if (m_sElementID.size() == 0)
    return sRet;

  switch (GetTypeOfObject())
  {
  case tTOFWW_Template:
    sRet = TEMPLATE_TOKEN_FRONT_START;
    sRet += m_sElementID;
    sRet += TEMPLATE_TOKEN_FRONT_END;
    break;
  case tTOFWW_Stone:
    sRet = STONE_TOKEN_FRONT_START;
    sRet += m_sElementID;
    sRet += STONE_TOKEN_FRONT_END;
    break;
  case tTOFWW_Field:
    sRet = FIELD_TOKEN_START;
    sRet += m_sElementID;
    sRet += FIELD_TOKEN_END;
    break;
  }
  return sRet;
}
xtstring CFormatWorkerWrapper::GetElementIDForReplace_Tail()
{
  xtstring sRet;
  sRet.clear();
  if (m_sElementID.size() == 0)
    return sRet;

  switch (GetTypeOfObject())
  {
  case tTOFWW_Template:
    sRet = TEMPLATE_TOKEN_TAIL_START;
    sRet += m_sElementID;
    sRet += TEMPLATE_TOKEN_TAIL_END;
    break;
  case tTOFWW_Stone:
    sRet = STONE_TOKEN_TAIL_START;
    sRet += m_sElementID;
    sRet += STONE_TOKEN_TAIL_END;
    break;
  case tTOFWW_Field:
    sRet = FIELD_TOKEN_START;
    sRet += m_sElementID;
    sRet += FIELD_TOKEN_END;
    break;
  }
  return sRet;
}
bool CFormatWorkerWrapper::ElementIDDefined()
{
  if (m_sElementID.size())
    return true;
  return false;
}
bool CFormatWorkerWrapper::IsThisWrapperChild(CFormatWorkerWrapper* pWrapper)
{
  if (!pWrapper)
    return false;
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if (*it == pWrapper)
      return true;
    if ((*it)->IsThisWrapperChild(pWrapper))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if (*it == pWrapper)
      return true;
    if ((*it)->IsThisWrapperChild(pWrapper))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if (*it == pWrapper)
      return true;
    if ((*it)->IsThisWrapperChild(pWrapper))
      return true;
  }
  return false;
}

bool CFormatWorkerWrapper::HasAnyChildElementID(int nLevel)
{
  if (nLevel && ElementIDDefined())
    return true;
  else if (GetTypeOfObject() == tTOFWW_Stone)
  {
    if (GetTypeOfObject() == tTOFWW_Stone)
    {
      CPropertyBase* pBase = GetPS_Me();
      if (pBase)
      {
        int nCount = GetPS_Me_ColDefinition_CountOfCols();
        for (int nI = 1; nI <= nCount; nI++)
        {
          xtstring sMyElementID = GetPS_Me_ColDefinition_ColElementID(nI);
          if (sMyElementID.size())
            return true;
        }
      }
    }
  }

  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildElementID(nLevel + 1))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildElementID(nLevel + 1))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildElementID(nLevel + 1))
      return true;
  }
  return false;
}

void CFormatWorkerWrapper::GetStatusCount(int nLevel,
                                          int& nStatus)
{
  if (nLevel)
  {
    switch (m_nStatusOfObject)
    {
    case tSOFWW_Neutral_Empty:
      nStatus |= TSOFWW_NEUTRAL_EMPTY;                                break;
    case tSOFWW_Synchron_White:
      nStatus |= TSOFWW_SYNCHRON_WHITE;                               break;
    case tSOFWW_ChangedValueNotInPanel_Yellow:
      nStatus |= TSOFWW_CHANGEDVALUENOTINPANEL_YELLOW;                break;
    case tSOFWW_ChangedValueInPanel_Green:
      nStatus |= TSOFWW_CHANGEDVALUEINPANEL_GREEN;                    break;
    case tSOFWW_Error_Red:
      nStatus |= TSOFWW_ERROR_RED;                                    break;
    case tSOFWW_Synchron_White_BrokenLink:
      nStatus |= TSOFWW_SYNCHRON_WHITE_BROKENLINK;                    break;
    case tSOFWW_ChangedValueNotInPanel_Yellow_BrokenLink:
      nStatus |= TSOFWW_CHANGEDVALUENOTINPANEL_YELLOW_BROKENLINK;     break;
    case tSOFWW_ChangedValueInPanel_Green_BrokenLink:
      nStatus |= TSOFWW_CHANGEDVALUEINPANEL_GREEN_BROKENLINK;         break;
    case tSOFWW_Error_Red_BrokenLink:
      nStatus |= TSOFWW_ERROR_RED_BROKENLINK;                         break;
    case tSOFWW_ParentKey:
      nStatus |= TSOFWW_PARENTKEY;                                    break;
    case tSOFWW_ParentAttention:
      nStatus |= TSOFWW_PARENTATTENTION;                              break;
    case tSOFWW_ParentError:
      nStatus |= TSOFWW_PARENTERROR;                                  break;
    default:
      assert(false);
    }
  }
  for (CNewStatusIterator it = m_cNewStatus.begin(); it != m_cNewStatus.end(); it++)
  {
    switch ((*it).second)
    {
    case tSOFWW_Neutral_Empty:
      nStatus |= TSOFWW_NEUTRAL_EMPTY;                                break;
    case tSOFWW_Synchron_White:
      nStatus |= TSOFWW_SYNCHRON_WHITE;                               break;
    case tSOFWW_ChangedValueNotInPanel_Yellow:
      nStatus |= TSOFWW_CHANGEDVALUENOTINPANEL_YELLOW;                break;
    case tSOFWW_ChangedValueInPanel_Green:
      nStatus |= TSOFWW_CHANGEDVALUEINPANEL_GREEN;                    break;
    case tSOFWW_Error_Red:
      nStatus |= TSOFWW_ERROR_RED;                                    break;
    case tSOFWW_Synchron_White_BrokenLink:
      nStatus |= TSOFWW_SYNCHRON_WHITE_BROKENLINK;                    break;
    case tSOFWW_ChangedValueNotInPanel_Yellow_BrokenLink:
      nStatus |= TSOFWW_CHANGEDVALUENOTINPANEL_YELLOW_BROKENLINK;     break;
    case tSOFWW_ChangedValueInPanel_Green_BrokenLink:
      nStatus |= TSOFWW_CHANGEDVALUEINPANEL_GREEN_BROKENLINK;         break;
    case tSOFWW_Error_Red_BrokenLink:
      nStatus |= TSOFWW_ERROR_RED_BROKENLINK;                         break;
    case tSOFWW_ParentKey:
      nStatus |= TSOFWW_PARENTKEY;                                    break;
    case tSOFWW_ParentAttention:
      nStatus |= TSOFWW_PARENTATTENTION;                              break;
    case tSOFWW_ParentError:
      nStatus |= TSOFWW_PARENTERROR;                                  break;
    default:
      assert(false);
    }
  }
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    (*it)->GetStatusCount(nLevel + 1, nStatus);
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    (*it)->GetStatusCount(nLevel + 1, nStatus);
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    (*it)->GetStatusCount(nLevel + 1, nStatus);
  }
}

bool CFormatWorkerWrapper::HasAnyChildThisStatus(TStatusOfFWWrapper nStatusOfObject,
                                                 int nLevel)
{
    if (nLevel && m_nStatusOfObject == nStatusOfObject)
      return true;
  for (CNewStatusIterator it = m_cNewStatus.begin(); it != m_cNewStatus.end(); it++)
  {
    if ((*it).second == nStatusOfObject)
      return true;
  }

  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildThisStatus(nStatusOfObject, nLevel + 1))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildThisStatus(nStatusOfObject, nLevel + 1))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildThisStatus(nStatusOfObject, nLevel + 1))
      return true;
  }
  return false;
}

bool CFormatWorkerWrapper::HasAnyChildOtherStatus(TStatusOfFWWrapper nStatusOfObject)
{
  if (m_nStatusOfObject != nStatusOfObject)
    return true;
  for (CNewStatusIterator it = m_cNewStatus.begin(); it != m_cNewStatus.end(); it++)
  {
    if ((*it).second != nStatusOfObject)
      return true;
  }

  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildOtherStatus(nStatusOfObject))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildOtherStatus(nStatusOfObject))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->HasAnyChildOtherStatus(nStatusOfObject))
      return true;
  }
  return false;
}

void CFormatWorkerWrapper::CheckStatusOfAllObjects()
{
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    (*it)->CheckStatusOfAllObjects();
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    (*it)->CheckStatusOfAllObjects();
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    (*it)->CheckStatusOfAllObjects();
	

  if (!HasAnyChildElementID(0))
    return;

  int nStatus = 0;
  GetStatusCount(0, nStatus);
  if (nStatus & TSOFWW_PARENTERROR)
    m_nStatusOfObject = tSOFWW_ParentError;
  else if (nStatus & TSOFWW_PARENTATTENTION)
    m_nStatusOfObject = tSOFWW_ParentAttention;
  else if (nStatus & TSOFWW_PARENTKEY)
    m_nStatusOfObject = tSOFWW_ParentKey;
  else if (nStatus & TSOFWW_ERROR_RED_BROKENLINK)
    m_nStatusOfObject = tSOFWW_Error_Red_BrokenLink;
  else if (nStatus & TSOFWW_CHANGEDVALUEINPANEL_GREEN_BROKENLINK)
    m_nStatusOfObject = tSOFWW_ChangedValueInPanel_Green_BrokenLink;
  else if (nStatus & TSOFWW_CHANGEDVALUENOTINPANEL_YELLOW_BROKENLINK)
    m_nStatusOfObject = tSOFWW_ChangedValueNotInPanel_Yellow_BrokenLink;
  else if (nStatus & TSOFWW_SYNCHRON_WHITE_BROKENLINK)
    m_nStatusOfObject = tSOFWW_Synchron_White_BrokenLink;
  else if (nStatus & TSOFWW_ERROR_RED)
    m_nStatusOfObject = tSOFWW_Error_Red;
  else if (nStatus & TSOFWW_CHANGEDVALUEINPANEL_GREEN)
    m_nStatusOfObject = tSOFWW_ChangedValueInPanel_Green;
  else if (nStatus & TSOFWW_CHANGEDVALUENOTINPANEL_YELLOW)
    m_nStatusOfObject = tSOFWW_ChangedValueNotInPanel_Yellow;
  else if (nStatus & TSOFWW_SYNCHRON_WHITE)
    m_nStatusOfObject = tSOFWW_Synchron_White;
  else
    m_nStatusOfObject = tSOFWW_Neutral_Empty;
/*
  if (HasAnyChildThisStatus(tSOFWW_ParentError, 0))
    m_nStatusOfObject = tSOFWW_ParentError;
  else if (HasAnyChildThisStatus(tSOFWW_ParentAttention, 0))
    m_nStatusOfObject = tSOFWW_ParentAttention;
	else if (HasAnyChildThisStatus(tSOFWW_ParentKey, 0))
    m_nStatusOfObject = tSOFWW_ParentKey;
	else if (HasAnyChildThisStatus(tSOFWW_Error_Red_BrokenLink, 0))
    m_nStatusOfObject = tSOFWW_Error_Red_BrokenLink;
	else if (HasAnyChildThisStatus(tSOFWW_ChangedValueInPanel_Green_BrokenLink, 0))
    m_nStatusOfObject = tSOFWW_ChangedValueInPanel_Green_BrokenLink;
	else if (HasAnyChildThisStatus(tSOFWW_ChangedValueNotInPanel_Yellow_BrokenLink, 0))
    m_nStatusOfObject = tSOFWW_ChangedValueNotInPanel_Yellow_BrokenLink;
	else if (HasAnyChildThisStatus(tSOFWW_Synchron_White_BrokenLink, 0))
    m_nStatusOfObject = tSOFWW_Synchron_White_BrokenLink;
	else if (HasAnyChildThisStatus(tSOFWW_Error_Red, 0))
    m_nStatusOfObject = tSOFWW_Error_Red;
	else if (HasAnyChildThisStatus(tSOFWW_ChangedValueInPanel_Green, 0))
    m_nStatusOfObject = tSOFWW_ChangedValueInPanel_Green;
	else if (HasAnyChildThisStatus(tSOFWW_ChangedValueNotInPanel_Yellow, 0))
    m_nStatusOfObject = tSOFWW_ChangedValueNotInPanel_Yellow;
	else if (HasAnyChildThisStatus(tSOFWW_Synchron_White, 0))
    m_nStatusOfObject = tSOFWW_Synchron_White;
  else
    m_nStatusOfObject = tSOFWW_Neutral_Empty;
*/
}

CFormatWorkerWrapper* CFormatWorkerWrapper::GetFormatWorkerWrapper(const xtstring& sElementID,
                                                                   CFormatWorkerWrapper** pParentOfFlatField/*= 0*/)
{
  if (ElementIDDefined() && GetElementID() == sElementID)
    return this;

  CFormatWorkerWrapper* pRet = 0;
  if (pParentOfFlatField && GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        xtstring sMyElementID = GetPS_Me_ColDefinition_ColElementID(nI);
        if (sMyElementID == sElementID)
        {
          *pParentOfFlatField = this;
          return pRet;
        }
      }
    }
  }

  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    pRet = (*it)->GetFormatWorkerWrapper(sElementID, pParentOfFlatField);
    if (pRet)
      return pRet;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    pRet = (*it)->GetFormatWorkerWrapper(sElementID, pParentOfFlatField);
    if (pRet)
      return pRet;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    pRet = (*it)->GetFormatWorkerWrapper(sElementID, pParentOfFlatField);
    if (pRet)
      return pRet;
  }
  return pRet;
}

bool CFormatWorkerWrapper::GetCRC32(const xtstring& sElementID,
                                    unsigned long& nCRC32)
{
  if (ElementIDDefined() && GetElementID() == sElementID)
  {
    nCRC32 = m_nCRC32;
    return true;
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        if (GetPS_Me_ColDefinition_ColElementID(nI) == sElementID)
        {
          if (m_cCRC32.find(sElementID) != m_cCRC32.end())
            nCRC32 = m_cCRC32[sElementID];
          return true;
        }
      }
    }
  }
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->GetCRC32(sElementID, nCRC32))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->GetCRC32(sElementID, nCRC32))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->GetCRC32(sElementID, nCRC32))
      return true;
  }
  return false;
}
bool CFormatWorkerWrapper::SetCRC32(const xtstring& sElementID,
                                    unsigned long nCRC32)
{
  if (ElementIDDefined() && GetElementID() == sElementID)
  {
    m_nCRC32 = nCRC32;
    return true;
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        if (GetPS_Me_ColDefinition_ColElementID(nI) == sElementID)
        {
          m_cCRC32[sElementID] = nCRC32;
          return true;
        }
      }
    }
  }
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->SetCRC32(sElementID, nCRC32))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->SetCRC32(sElementID, nCRC32))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->SetCRC32(sElementID, nCRC32))
      return true;
  }
  return false;
}
bool CFormatWorkerWrapper::GetRawContent(const xtstring& sElementID,
                                         const xtstring& sStartElementTyp,
                                         bool bTextFocusActive,
                                         bool bFlatStyle,
                                         xtstring& sRawContent)
{
  if (GetTypeOfObject() == tTOFWW_Field
      && ElementIDDefined()
      && GetElementID() == sElementID)
  {
    sRawContent = ((CFormatWorkerWrapper_Field*)this)->GetContentResult();
    return true;
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        xtstring sMyElementID = GetPS_Me_ColDefinition_ColElementID(nI);
        if (sMyElementID == sElementID)
        {
          sRawContent = GetPS_Me_ColDefinition_ColText(nI);
          return true;
        }
      }
    }
  }
  if (GetTypeOfObject() == tTOFWW_Stone
      && ElementIDDefined()
      && GetElementID() == sElementID)
  {
              CFormatWorkerWrapper* pFormatWorkerWrapper = this;
              bool bUseElementID_Field = false;
              bool bUseElementID_Stone = false;
              bool bUseElementID_Template = false;

              bool bDeleteStone = false;
              CFormatWorkerWrapper_Stone* pStone = 0;
              if (bFlatStyle)
              {
                CFormatWorkerWrapper* pTemplate = pFormatWorkerWrapper->GetParent();
                if (!pTemplate || pTemplate->GetTypeOfObject() != CFormatWorkerWrapper::tTOFWW_Template)
                  // Was soll das?
                  return false;
                CFormatWorkerWrapper_Stone* pTAGStone = pTemplate->GetFirstTAGStoneWrapper();
                if (!pTAGStone)
                  // no money, no funny
                  return false;
                CFormatWorkerWrapper_Stone* pNEWTAGStone = new CFormatWorkerWrapper_Stone(*pTAGStone);
                if (!pNEWTAGStone || !pNEWTAGStone->GetFormatWorker())
                {
                  if (pNEWTAGStone)
                    delete pNEWTAGStone;
                  return false;
                }
                CFormatWorker* pFW = pNEWTAGStone->GetFormatWorker();
                for (int nI = 1; nI < 11; nI++)
                {
                  xtstring sField;
                  sField.Format(COLUMN_TEMPLATE_FOR_FLAT, nI);
                  try
                  {
                    xtstring sElementID;
                    xtstring sPicture;
                    sElementID = pFormatWorkerWrapper->GetPS_Me_ColDefinition_ColElementID(nI);
                    sPicture = pFormatWorkerWrapper->GetPS_Me_ColDefinition_ColPicture(nI);
                    if (sPicture.size())
                      pFW->ReplaceTextField(sField.c_str(), sPicture);
                    else if (bUseElementID_Field && sElementID.size())
                      pFW->ReplaceTextField(sField.c_str(), pFormatWorkerWrapper->GetPS_Me_ColDefinition_ColElementIDForReplace(nI));
                    else
                      pFW->ReplaceTextField(sField.c_str(), pFormatWorkerWrapper->GetPS_Me_ColDefinition_ColText(nI));
                  }
                  catch (...) {}
                }
                if (bUseElementID_Stone)
                {
                  try
                  {
                    xtstring sElementID = pFormatWorkerWrapper->GetElementID();
                    if (sElementID.size())
                    {
                      xtstring sFront = pFormatWorkerWrapper->GetElementIDForReplace_Front();
                      xtstring sTail = pFormatWorkerWrapper->GetElementIDForReplace_Tail();
                      pFW->InsertText_Front(sFront);
                      pFW->InsertText_Tail(sTail);
                    }
                  }
                  catch (...) {}
                }
                bDeleteStone = true;
                pStone = pNEWTAGStone;
              }
              else
                pStone = (CFormatWorkerWrapper_Stone*)pFormatWorkerWrapper;

              // Als erste, Variablen setzen
              pStone->SetVariable_TreeStartActiontyp(TSAT_STARTDBLCLK);
              pStone->SetVariable_TreeStartElementtyp(sStartElementTyp);
              pStone->SetVariable_TextFocusActive(bTextFocusActive);
              pStone->SetVariable_DocBeginPageNumber(1);

              CWorkPaperProcessor cProcessor;
              cProcessor.SetGeneratedStone(pStone);
              cProcessor.SetUseElementID(bUseElementID_Field, bUseElementID_Stone, bUseElementID_Template);

              xtstring sResult;
              bool bRet = cProcessor.ProcessGeneratedWorkPaper(sResult);
              if (!bRet)
                sResult.clear();
              if (pStone && bDeleteStone)
                delete pStone;
              sRawContent = sResult;
              return bRet;
  }
  if (GetTypeOfObject() == tTOFWW_Template
      && ElementIDDefined()
      && GetElementID() == sElementID)
  {
              CFormatWorkerWrapper* pFormatWorkerWrapper = this;
              bool bUseElementID_Field = false;
              bool bUseElementID_Stone = false;
              bool bUseElementID_Template = false;

              bool bDeleteTemplate = false;
              CFormatWorkerWrapper_Template* pTemplate = 0;
              if (bFlatStyle)
              {
                CFormatWorkerWrapper_Stone* pTAGStone = pFormatWorkerWrapper->GetFirstTAGStoneWrapper();
                if (!pTAGStone)
                  // no money, no funny
                  return false;
                CFormatWorkerWrapper_Template* pCopyTemplate = new CFormatWorkerWrapper_Template(*(CFormatWorkerWrapper_Template*)pFormatWorkerWrapper);
                if (!pCopyTemplate || !pCopyTemplate->GetFormatWorker())
                {
                  if (pCopyTemplate)
                    delete pCopyTemplate;
                  return false;
                }
                pCopyTemplate->RemoveAllNonPropertyStones();
                for (size_t nI = 0; nI < pFormatWorkerWrapper->CountOfStoneWrappers(); nI++)
                {
                  if (!pFormatWorkerWrapper->GetStoneWrapper(nI)->GetFormatWorker()->IsPropertyFormatWorker())
                    continue;
                  CFormatWorkerWrapper_Stone* pDataStone = pFormatWorkerWrapper->GetStoneWrapper(nI);
                  if (!pDataStone)
                    continue;
                  xtstring sTextInNameProperty = pDataStone->GetPS_Me_TextInNameProperty();
                  if (sTextInNameProperty.CompareNoCase(PROP_NAMETEXT_FIELDS) != 0)
                    continue;
                  CFormatWorkerWrapper_Stone* pNEWTAGStone = new CFormatWorkerWrapper_Stone(*pTAGStone);
                  if (!pNEWTAGStone || !pNEWTAGStone->GetFormatWorker())
                  {
                    if (pCopyTemplate)
                      delete pCopyTemplate;
                    if (pNEWTAGStone)
                      delete pNEWTAGStone;
                    return false;
                  }
                  CFormatWorker* pFW = pNEWTAGStone->GetFormatWorker();
                  for (int nI = 1; nI < 11; nI++)
                  {
                    xtstring sField;
                    sField.Format(COLUMN_TEMPLATE_FOR_FLAT, nI);
                    try
                    {
                      xtstring sElementID;
                      xtstring sPicture;
                      sElementID = pDataStone->GetPS_Me_ColDefinition_ColElementID(nI);
                      sPicture = pDataStone->GetPS_Me_ColDefinition_ColPicture(nI);
                      if (sPicture.size())
                        pFW->ReplaceTextField(sField.c_str(), sPicture);
                      else if (bUseElementID_Field && sElementID.size())
                        pFW->ReplaceTextField(sField.c_str(), pDataStone->GetPS_Me_ColDefinition_ColElementIDForReplace(nI));
                      else
                        pFW->ReplaceTextField(sField.c_str(), pDataStone->GetPS_Me_ColDefinition_ColText(nI));
                    }
                    catch (...) {}
                  }
                  if (bUseElementID_Stone)
                  {
                    try
                    {
                      xtstring sElementID = pDataStone->GetElementID();
                      if (sElementID.size())
                      {
                        xtstring sFront = pDataStone->GetElementIDForReplace_Front();
                        xtstring sTail = pDataStone->GetElementIDForReplace_Tail();
                        pFW->InsertText_Front(sFront);
                        pFW->InsertText_Tail(sTail);
                      }
                    }
                    catch (...) {}
                  }
                  pCopyTemplate->AddStoneWrapper(pNEWTAGStone);
                }
                bDeleteTemplate = true;
                pTemplate = pCopyTemplate;
              }
              else
                pTemplate = (CFormatWorkerWrapper_Template*)pFormatWorkerWrapper;

              // Als erste, Variablen setzen
              pTemplate->SetVariable_TreeStartActiontyp(TSAT_STARTDBLCLK);
              pTemplate->SetVariable_TreeStartElementtyp(sStartElementTyp);
              pTemplate->SetVariable_TextFocusActive(bTextFocusActive);
              pTemplate->SetVariable_DocBeginPageNumber(1);

              CWorkPaperProcessor cProcessor;
              cProcessor.SetGeneratedTemplate(pTemplate);
              cProcessor.SetUseElementID(bUseElementID_Field, bUseElementID_Stone, bUseElementID_Template);

              xtstring sResult;
              bool bRet = cProcessor.ProcessGeneratedWorkPaper(sResult);
              if (!bRet)
                sResult.clear();
              if (pTemplate && bDeleteTemplate)
                delete pTemplate;
              sRawContent = sResult;
              return bRet;
  }

  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->GetRawContent(sElementID, sStartElementTyp, bTextFocusActive, bFlatStyle, sRawContent))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->GetRawContent(sElementID, sStartElementTyp, bTextFocusActive, bFlatStyle, sRawContent))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->GetRawContent(sElementID, sStartElementTyp, bTextFocusActive, bFlatStyle, sRawContent))
      return true;
  }
  return false;
}

void CFormatWorkerWrapper::GetAllElementID(CWrapperObjectStatusMap& cTemplates,
                                           CWrapperObjectStatusMap& cStones,
                                           CWrapperObjectStatusMap& cFields)
{
  xtstring sContent, sContentMask, sNewContent;
  if (ElementIDDefined())
  {
    if (GetTypeOfObject() == tTOFWW_Template)
    {
      if (cTemplates.find(GetElementID()) == cTemplates.end())
      {
        GetValue(GetElementID(), sContent, sContentMask, sNewContent);
        CWrapperObjectStatus cStatus;
        cStatus.sElementID = GetElementID();
        cStatus.sOldValue = sContent;
        cStatus.sNewValue = sNewContent;
        cStatus.nStatus = GetStatus();
        cTemplates[GetElementID()] = cStatus;
      }
    }
    else if (GetTypeOfObject() == tTOFWW_Stone)
    {
      if (cStones.find(GetElementID()) == cStones.end())
      {
        GetValue(GetElementID(), sContent, sContentMask, sNewContent);
        CWrapperObjectStatus cStatus;
        cStatus.sElementID = GetElementID();
        cStatus.sOldValue = sContent;
        cStatus.sNewValue = sNewContent;
        cStatus.nStatus = GetStatus();
        cStones[GetElementID()] = cStatus;
      }
    }
    else if (GetTypeOfObject() == tTOFWW_Field)
    {
      if (cFields.find(GetElementID()) == cFields.end())
      {
        GetValue(GetElementID(), sContent, sContentMask, sNewContent);
        CWrapperObjectStatus cStatus;
        cStatus.sElementID = GetElementID();
        cStatus.sOldValue = sContent;
        cStatus.sNewValue = sNewContent;
        cStatus.nStatus = GetStatus();
        cFields[GetElementID()] = cStatus;
      }
    }
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        xtstring sMyElementID = GetPS_Me_ColDefinition_ColElementID(nI);
        if (sMyElementID.size())
        {
          if (cFields.find(sMyElementID) == cFields.end())
          {
            GetValue(sMyElementID, sContent, sContentMask, sNewContent);
            CWrapperObjectStatus cStatus;
            cStatus.sElementID = sMyElementID;//GetElementID();
            cStatus.sOldValue = sContent;
            cStatus.sNewValue = sNewContent;
            GetStatusOfObject(sMyElementID, cStatus.nStatus);
            cFields[sMyElementID] = cStatus;
          }
        }
      }
    }
  }

  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    (*it)->GetAllElementID(cTemplates, cStones, cFields);
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    (*it)->GetAllElementID(cTemplates, cStones, cFields);
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    (*it)->GetAllElementID(cTemplates, cStones, cFields);
}

bool CFormatWorkerWrapper::GetStatusOfObject(const xtstring& sElementID,
                                             TStatusOfFWWrapper& nStatusOfObject)
{
  if (ElementIDDefined() && GetElementID() == sElementID)
  {
    nStatusOfObject = m_nStatusOfObject;
    return true;
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        xtstring sMyElementID = GetPS_Me_ColDefinition_ColElementID(nI);
        if (sMyElementID == sElementID)
        {
          nStatusOfObject = GetNewStatus(sElementID);
          return true;
        }
      }
    }
  }
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->GetStatusOfObject(sElementID, nStatusOfObject))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->GetStatusOfObject(sElementID, nStatusOfObject))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->GetStatusOfObject(sElementID, nStatusOfObject))
      return true;
  }
  return false;
}

bool CFormatWorkerWrapper::SetStatusOfObject(const xtstring& sElementID,
                                             TStatusOfFWWrapper nStatusOfObject)
{
  if (ElementIDDefined() && GetElementID() == sElementID)
  {
    m_nStatusOfObject = nStatusOfObject;
    return true;
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        if (GetPS_Me_ColDefinition_ColElementID(nI) == sElementID)
        {
          m_cNewStatus[sElementID] = nStatusOfObject;
          return true;
        }
      }
    }
  }
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    if ((*it)->SetStatusOfObject(sElementID, nStatusOfObject))
      return true;
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    if ((*it)->SetStatusOfObject(sElementID, nStatusOfObject))
      return true;
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    if ((*it)->SetStatusOfObject(sElementID, nStatusOfObject))
      return true;
  return false;
}
void CFormatWorkerWrapper::ResetStatusOfAllObjects()
{
  RemoveNewStatus();
  m_nStatusOfObject = tSOFWW_Default;
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    (*it)->ResetStatusOfAllObjects();
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    (*it)->ResetStatusOfAllObjects();
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    (*it)->ResetStatusOfAllObjects();
}
bool CFormatWorkerWrapper::GetValue(const xtstring& sElementID,
                                    xtstring& sContent,
                                    xtstring& sContentMask,
                                    xtstring& sNewContent)
{
  if (GetTypeOfObject() == tTOFWW_Field
      && ElementIDDefined()
      && GetElementID() == sElementID)
  {
    sContent = ((CFormatWorkerWrapper_Field*)this)->GetContentResult();
    sContentMask = ((CFormatWorkerWrapper_Field*)this)->GetContentMask();
    sNewContent = GetNewValue(sElementID);
    return true;
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        xtstring sMyElementID = GetPS_Me_ColDefinition_ColElementID(nI);
        if (sMyElementID == sElementID)
        {
          sContent = GetPS_Me_ColDefinition_ColText(nI);
          sContentMask = GetPS_Me_ColDefinition_ColMask(nI);
          sNewContent = GetNewValue(sElementID);
          return true;
        }
      }
    }
  }
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->GetValue(sElementID, sContent, sContentMask, sNewContent))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->GetValue(sElementID, sContent, sContentMask, sNewContent))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->GetValue(sElementID, sContent, sContentMask, sNewContent))
      return true;
  }
  return false;
}

bool CFormatWorkerWrapper::SetValue(const xtstring& sElementID,
                                    const xtstring& sNewContent)
{
  if (GetTypeOfObject() == tTOFWW_Field
      && ElementIDDefined()
      && GetElementID() == sElementID)
  {
    m_cNewValues[sElementID] = sNewContent;
    return true;
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        if (GetPS_Me_ColDefinition_ColElementID(nI) == sElementID)
        {
          m_cNewValues[sElementID] = sNewContent;
          return true;
        }
      }
    }
  }
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    if ((*it)->SetValue(sElementID, sNewContent))
      return true;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    if ((*it)->SetValue(sElementID, sNewContent))
      return true;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    if ((*it)->SetValue(sElementID, sNewContent))
      return true;
  }
  return false;
}

void CFormatWorkerWrapper::ResetNewValues()
{
  RemoveNewValues();
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
    (*it)->ResetNewValues();
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
    (*it)->ResetNewValues();
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
    (*it)->ResetNewValues();
}

xtstring CFormatWorkerWrapper::GetClientAddressOfParent(bool& bElementIDFound,
                                                        const xtstring& sElementID)
{
  // Wir muessen ganz unten zu dem Element fahren, wo sElementID definiert ist.
  // Dann baim hochfahren muessen wir erst m_sClientAddress merken, die definiert ist.

  if (ElementIDDefined() && GetElementID() == sElementID)
  {
    // da starten wir Rueckfahrt
    bElementIDFound = true;
    return m_sClientAddress;
  }
  if (GetTypeOfObject() == tTOFWW_Stone)
  {
    CPropertyBase* pBase = GetPS_Me();
    if (pBase)
    {
      int nCount = GetPS_Me_ColDefinition_CountOfCols();
      for (int nI = 1; nI <= nCount; nI++)
      {
        if (GetPS_Me_ColDefinition_ColElementID(nI) == sElementID)
        {
          bElementIDFound = true;
          // da starten wir Rueckfahrt
          // Nur Bemerkung - da sind wir in Prop Baustein fuer flache Darstellung
          return m_sClientAddress;
        }
      }
    }
  }
  xtstring sRetValue;
  for (CFieldWrappersIterator it = m_cFieldWrappers.begin(); it != m_cFieldWrappers.end(); it++)
  {
    sRetValue = (*it)->GetClientAddressOfParent(bElementIDFound, sElementID);
    if (sRetValue.size())
      return sRetValue;
    if (bElementIDFound)
      // Wir sind schon auf Rueckfahrt
      return m_sClientAddress;
  }
  for (CStoneWrappersIterator it = m_cStoneWrappers.begin(); it != m_cStoneWrappers.end(); it++)
  {
    sRetValue = (*it)->GetClientAddressOfParent(bElementIDFound, sElementID);
    if (sRetValue.size())
      return sRetValue;
    if (bElementIDFound)
      // Wir sind schon auf Rueckfahrt
      return m_sClientAddress;
  }
  for (CTemplateWrappersIterator it = m_cTemplateWrappers.begin(); it != m_cTemplateWrappers.end(); it++)
  {
    sRetValue = (*it)->GetClientAddressOfParent(bElementIDFound, sElementID);
    if (sRetValue.size())
      return sRetValue;
    if (bElementIDFound)
      // Wir sind schon auf Rueckfahrt
      return m_sClientAddress;
  }
  // Wenn wir da sind, niemand hat ClientAddress gemeldet.
  // Wir liefert von aktuellem Objekt
  if (bElementIDFound)
    return m_sClientAddress;
  return _XT("");
}

bool CFormatWorkerWrapper::WrapperIsPropStone()
{
  if (!m_pFormatWorker)
    return false;
  if (m_pFormatWorker->IsPropertyFormatWorker())
    return true;
  return false;
}

xtstring CFormatWorkerWrapper::GetPS_Me_TextInNameProperty()
{
  xtstring sRet;
  CFormatWorker* pFW = GetFormatWorker();
  if (GetFormatWorker()
      && GetFormatWorker()->IsPropertyFormatWorker()
      && GetFormatWorker()->GetPropertyBase()
      && GetFormatWorker()->GetPropertyBase()->GetPropertyGroup() == tXPP_FreeProps
      && GetFormatWorker()->GetPropertyBase()->FreePropPresent(PROP_NAME_NAME))
  {
    CXPubVariant cName;
    GetFormatWorker()->GetPropertyBase()->GetPropertyValue(cName, PROP_NAME_NAME);
    sRet = cName;
  }
  return sRet;
}
CXPubVariant CFormatWorkerWrapper::GetPS_Me_ElementID()
{
  CXPubVariant val;
  val.Clear();
  if (GetFormatWorker()
      && GetFormatWorker()->IsPropertyFormatWorker()
      && GetFormatWorker()->GetPropertyBase()
      && GetFormatWorker()->GetPropertyBase()->GetPropertyGroup() == tXPP_FreeProps)
  {
    GetFormatWorker()->GetPropertyBase()->GetPropertyValue(val, WORKPAPERWRAPPER_ELEMENTID);
  }
  return val;
}
int CFormatWorkerWrapper::GetPS_Me_ColDefinition_CountOfCols()
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return 0;
  if (!pProps->FreePropPresent(PROP_NAME_COLCOUNT))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, PROP_NAME_COLCOUNT);
  return (long)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColText(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLTEXT, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColMask(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLMASK, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColElementID(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLELEMENTID, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColPicture(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLPICTURE, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColTag(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLTAG, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColElementIDForReplace(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLELEMENTID, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  xtstring sText = FIELD_TOKEN_START;
  sText += (xtstring)cValue;
  sText += FIELD_TOKEN_END;
  return sText;
}
bool CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColWidthDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLWIDTH, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColWidth(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLWIDTH, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColIconDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLICON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColIconIndex(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLICON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColAlignDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLALIGN, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColAlign(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLALIGN, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColButtonDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLBUTTON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Me_ColDefinition_ColButton(int nCol)
{
  CPropertyBase* pProps = GetPS_Me();
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLBUTTON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
CPropertyBase* CFormatWorkerWrapper::GetPS_Me_ExecuteActionRemote()
{
  CPropertyBase* pRet = 0;

  CFormatWorker* pFW = GetFormatWorker();
  if (!pFW)
    return pRet;
  if (!pFW->IsPropertyFormatWorker())
    return pRet;
  // Die Funktion liefert !NULL nur falls um PROP geht.
  CPropertyBase* pProps = pFW->GetPropertyBase();
  if (!pProps)
    return pRet;
  // Wir haben prop Baustein
  if (pProps->GetPropertyGroup() != tXPP_FreeProps)
    return pRet;
  if (!pProps->FreePropPresent(PROP_NAME_NAME))
    return pRet;
  CXPubVariant cName;
  pProps->GetPropertyValue(cName, PROP_NAME_NAME);
  xtstring sName;
  sName = cName;
  if (sName.CompareNoCase(PROP_NAMETEXT_EXECACTIONREMOTE))
    return pRet;
  pRet = pProps;

  return pRet;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Me_GlobalSettings()
{
  CPropertyBase* pRet = 0;
  CFormatWorker* pFW = GetFormatWorker();
  if (!pFW)
    return pRet;
  if (!pFW->IsPropertyFormatWorker())
    return pRet;
  // Die Funktion liefert !NULL nur falls um PROP geht.
  CPropertyBase* pProps = pFW->GetPropertyBase();
  if (!pProps)
    return pRet;
  // Wir haben prop Baustein
  if (pProps->GetPropertyGroup() != tXPP_FreeProps)
    return pRet;
  if (!pProps->FreePropPresent(PROP_NAME_NAME))
    return pRet;
  CXPubVariant cName;
  pProps->GetPropertyValue(cName, PROP_NAME_NAME);
  xtstring sName;
  sName = cName;
  if (sName.CompareNoCase(PROP_NAMETEXT_GLOBALS))
    return pRet;
  pRet = pProps;

  return pRet;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Me_ExecuteActionLocal()
{
  CPropertyBase* pRet = 0;
  CFormatWorker* pFW = GetFormatWorker();
  if (!pFW)
    return pRet;
  if (!pFW->IsPropertyFormatWorker())
    return pRet;
  // Die Funktion liefert !NULL nur falls um PROP geht.
  CPropertyBase* pProps = pFW->GetPropertyBase();
  if (!pProps)
    return pRet;
  // Wir haben prop Baustein
  if (pProps->GetPropertyGroup() != tXPP_FreeProps)
    return pRet;
  if (!pProps->FreePropPresent(PROP_NAME_NAME))
    return pRet;
  CXPubVariant cName;
  pProps->GetPropertyValue(cName, PROP_NAME_NAME);
  xtstring sName;
  sName = cName;
  if (sName.CompareNoCase(PROP_NAMETEXT_EXECACTIONLOCAL))
    return pRet;
  pRet = pProps;

  return pRet;
}


CPropertyBase* CFormatWorkerWrapper::GetPS_Me()
{
  CFormatWorker* pFW = GetFormatWorker();
  if (!pFW)
    return 0;
  if (!pFW->IsPropertyFormatWorker())
    return 0;
  // Die Funktion liefert !NULL nur falls um PROP geht.
  CPropertyBase* pProps = pFW->GetPropertyBase();
  if (!pProps)
    return 0;
  // Wir haben prop Baustein
  if (pProps->GetPropertyGroup() != tXPP_FreeProps)
    return 0;
  if (!pProps->FreePropPresent(PROP_NAME_NAME))
    return 0;
  return pProps;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Child(const xtstring& sTextInNameProperty)
{
  CPropertyBase* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(sTextInNameProperty))
      continue;
    pRet = pProps;
    break;
  }
  return pRet;
}

bool CFormatWorkerWrapper::GetPS_Child_Show()
{
  bool bRet = true;
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_SHOW);
  if (pProps)
  {
    if (pProps->FreePropPresent(PROP_NAME_SHOW))
    {
      CXPubVariant cValue;
      pProps->GetPropertyValue(cValue, PROP_NAME_SHOW);
      bRet = cValue;
    }
  }
  return bRet;
}
bool CFormatWorkerWrapper::GetPS_Child_ShowChilds()
{
  bool bRet = true;
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_SHOWCHILDS);
  if (pProps)
  {
    if (pProps->FreePropPresent(PROP_NAME_SHOWCHILDS))
    {
      CXPubVariant cValue;
      pProps->GetPropertyValue(cValue, PROP_NAME_SHOWCHILDS);
      bRet = cValue;
    }
  }
  return bRet;
}
bool CFormatWorkerWrapper::GetPS_Child_ColumnDefinition_Defined()
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_ColDefinition_CountOfCols()
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return 0;
  if (!pProps->FreePropPresent(PROP_NAME_COLCOUNT))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, PROP_NAME_COLCOUNT);
  return (long)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColText(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLTEXT, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColElementID(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLELEMENTID, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColPicture(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLPICTURE, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColTag(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLTAG, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColWidthDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLWIDTH, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColWidth(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLWIDTH, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColIconDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLICON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColIconIndex(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLICON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColAlignDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLALIGN, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColAlign(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLALIGN, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColButtonDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLBUTTON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_ColDefinition_ColButton(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FIELDS);
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLBUTTON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColumnDefinition_Defined()
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_CountOfCols()
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return 0;
  if (!pProps->FreePropPresent(PROP_NAME_COLCOUNT))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, PROP_NAME_COLCOUNT);
  return (long)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColText(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLTEXT, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
xtstring CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ElementID(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return _XT("");
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLELEMENTID, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return _XT("");
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (xtstring)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColWidthDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLWIDTH, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColWidth(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLWIDTH, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColIconDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLICON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColIconIndex(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLICON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColAlignDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLALIGN, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColAlign(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLALIGN, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}
bool CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColButtonDefined(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return false;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLBUTTON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return false;
  return true;
}
int CFormatWorkerWrapper::GetPS_Child_FlatHeader_ColDefinition_ColButton(int nCol)
{
  CPropertyBase* pProps = GetPS_Child(PROP_NAMETEXT_FLATHEADER);
  if (!pProps)
    return 0;
  xtstring sPropertyName;
  sPropertyName.Format(PROP_NAME_COLBUTTON, nCol);
  if (!pProps->FreePropPresent(sPropertyName))
    return 0;
  CXPubVariant cValue;
  pProps->GetPropertyValue(cValue, sPropertyName);
  return (long)cValue;
}


CPropertyBase* CFormatWorkerWrapper::GetPS_Child_ExecuteActionRemote(int nID)
{
  CPropertyBase* pRet = 0;
  CFormatWorkerWrapper_Stone* pPropStone = GetPS_Child_ExecuteActionRemoteWrapper(nID);
  if (!pPropStone)
    return pRet;
  CFormatWorker* pFW = pPropStone->GetFormatWorker();
  if (!pFW)
    return pRet;
  pRet = pFW->GetPropertyBase();
  return pRet;
}

CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetPS_Child_ExecuteActionRemoteWrapper(int nID)
{
  CFormatWorkerWrapper_Stone* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(PROP_NAMETEXT_EXECACTIONREMOTE))
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_EXEC_ID))
      continue;
    CXPubVariant cID;
    pProps->GetPropertyValue(cID, PROP_NAME_EXEC_ID);
    if (nID != (long)cID)
      continue;
    pRet = pPropStone;
    break;
  }
  return pRet;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Child_ExecuteActionLocal(int nID)
{
  CPropertyBase* pRet = 0;
  CFormatWorkerWrapper_Stone* pPropStone = GetPS_Child_ExecuteActionLocalWrapper(nID);
  if (!pPropStone)
    return pRet;
  CFormatWorker* pFW = pPropStone->GetFormatWorker();
  if (!pFW)
    return pRet;
  pRet = pFW->GetPropertyBase();;
  return pRet;
}

CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetPS_Child_ExecuteActionLocalWrapper(int nID)
{
  CFormatWorkerWrapper_Stone* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(PROP_NAMETEXT_EXECACTIONLOCAL))
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_EXEC_ID))
      continue;
    CXPubVariant cID;
    pProps->GetPropertyValue(cID, PROP_NAME_EXEC_ID);
    if (nID != (long)cID)
      continue;
    pRet = pPropStone;
    break;
  }
  return pRet;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Child_ExecuteActionRemote(const xtstring& sID)
{
  CPropertyBase* pRet = 0;
  CFormatWorkerWrapper_Stone* pPropStone = GetPS_Child_ExecuteActionRemoteWrapper(sID);
  if (!pPropStone)
    return pRet;
  CFormatWorker* pFW = pPropStone->GetFormatWorker();
  if (!pFW)
    return pRet;
  pRet = pFW->GetPropertyBase();
  return pRet;
}

CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetPS_Child_ExecuteActionRemoteWrapper(const xtstring& sID)
{
  CFormatWorkerWrapper_Stone* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(PROP_NAMETEXT_EXECACTIONREMOTE))
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_EXEC_ID))
      continue;
    CXPubVariant cID;
    pProps->GetPropertyValue(cID, PROP_NAME_EXEC_ID);
    if (sID != (xtstring)cID)
      continue;
    pRet = pPropStone;
    break;
  }
  return pRet;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Child_ExecuteActionLocal(const xtstring& sID)
{
  CPropertyBase* pRet = 0;
  CFormatWorkerWrapper_Stone* pPropStone = GetPS_Child_ExecuteActionLocalWrapper(sID);
  if (!pPropStone)
    return pRet;
  CFormatWorker* pFW = pPropStone->GetFormatWorker();
  if (!pFW)
    return pRet;
  pRet = pFW->GetPropertyBase();;
  return pRet;
}

CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetPS_Child_ExecuteActionLocalWrapper(const xtstring& sID)
{
  CFormatWorkerWrapper_Stone* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(PROP_NAMETEXT_EXECACTIONLOCAL))
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_EXEC_ID))
      continue;
    CXPubVariant cID;
    pProps->GetPropertyValue(cID, PROP_NAME_EXEC_ID);
    if (sID != (xtstring)cID)
      continue;
    pRet = pPropStone;
    break;
  }
  return pRet;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Child_GlobalSettings()
{
  CPropertyBase* pRet = 0;
  CFormatWorkerWrapper_Stone* pPropStone = GetPS_Child_GlobalSettingsWrapper();
  if (!pPropStone)
    return pRet;
  CFormatWorker* pFW = pPropStone->GetFormatWorker();
  if (!pFW)
    return pRet;
  pRet = pFW->GetPropertyBase();
  return pRet;
}
CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetPS_Child_GlobalSettingsWrapper()
{
  CFormatWorkerWrapper_Stone* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(PROP_NAMETEXT_GLOBALS))
      continue;
    pRet = pPropStone;
    break;
  }
  return pRet;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Child_ValueAndStatus()
{
  CPropertyBase* pRet = 0;
  CFormatWorkerWrapper_Stone* pPropStone = GetPS_Child_ValueAndStatusWrapper();
  if (!pPropStone)
    return pRet;
  CFormatWorker* pFW = pPropStone->GetFormatWorker();
  if (!pFW)
    return pRet;
  pRet = pFW->GetPropertyBase();;
  return pRet;
}

CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetPS_Child_ValueAndStatusWrapper()
{
  CFormatWorkerWrapper_Stone* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(PROP_NAMETEXT_VALUEANDSTATUS))
      continue;
    pRet = pPropStone;
    break;
  }
  return pRet;
}
CPropertyBase* CFormatWorkerWrapper::GetPS_Child_ReportFilter()
{
  CPropertyBase* pRet = 0;
  CFormatWorkerWrapper_Stone* pPropStone = GetPS_Child_ReportFilterWrapper();
  if (!pPropStone)
    return pRet;
  CFormatWorker* pFW = pPropStone->GetFormatWorker();
  if (!pFW)
    return pRet;
  pRet = pFW->GetPropertyBase();;
  return pRet;
}

CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetPS_Child_ReportFilterWrapper()
{
  CFormatWorkerWrapper_Stone* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(PROP_NAMETEXT_REPORTFILTER))
      continue;
    pRet = pPropStone;
    break;
  }
  return pRet;
}

CPropertyBase* CFormatWorkerWrapper::GetPS_Child_Picture()
{
  CPropertyBase* pRet = 0;
  CFormatWorkerWrapper_Stone* pPropStone = GetPS_Child_PictureWrapper();
  if (!pPropStone)
    return pRet;
  CFormatWorker* pFW = pPropStone->GetFormatWorker();
  if (!pFW)
    return pRet;
  pRet = pFW->GetPropertyBase();;
  return pRet;
}

CFormatWorkerWrapper_Stone* CFormatWorkerWrapper::GetPS_Child_PictureWrapper()
{
  CFormatWorkerWrapper_Stone* pRet = 0;
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pPropStone = GetStoneWrapper(nI);
    if (!pPropStone)
      continue;
    CFormatWorker* pFW = pPropStone->GetFormatWorker();
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    // Die Funktion liefert !NULL nur falls um PROP geht.
    CPropertyBase* pProps = pFW->GetPropertyBase();
    if (!pProps)
      continue;
    // Wir haben prop Baustein
    if (pProps->GetPropertyGroup() != tXPP_FreeProps)
      continue;
    if (!pProps->FreePropPresent(PROP_NAME_NAME))
      continue;
    CXPubVariant cName;
    pProps->GetPropertyValue(cName, PROP_NAME_NAME);
    xtstring sName;
    sName = cName;
    if (sName.CompareNoCase(PROP_NAMETEXT_PICTURE))
      continue;
    pRet = pPropStone;
    break;
  }
  return pRet;
}

























////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CFormatWorkerWrapper_Field
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CFormatWorkerWrapper_Field::CFormatWorkerWrapper_Field(CFormatWorkerWrapper_Root* pRoot,
                                                       CFormatWorkerWrapper* pParent)
                           :CFormatWorkerWrapper(CFormatWorkerWrapper::tTOFWW_Field,
                                                 pRoot,
                                                 pParent)
{
  m_pKey = 0;
  m_pText = 0;
  m_pArea = 0;
}
CFormatWorkerWrapper_Field::CFormatWorkerWrapper_Field(const CFormatWorkerWrapper_Field& cField)
                           :CFormatWorkerWrapper(cField)
{
  m_pKey = 0;
  m_pText = 0;
  m_pArea = 0;
  if (cField.m_pKey)
    m_pKey = new CMPModelKeyInd_ForFrm(*(cField.m_pKey));
  if (cField.m_pText)
    m_pText = new CMPModelTextInd_ForFrm(*(cField.m_pText));
  if (cField.m_pArea)
    m_pArea = new CMPModelAreaInd_ForFrm(*(cField.m_pArea));
}
CFormatWorkerWrapper_Field::~CFormatWorkerWrapper_Field()
{
  if (m_pKey)
    delete m_pKey;
  if (m_pText)
    delete m_pText;
  if (m_pArea)
    delete m_pArea;
}
xtstring CFormatWorkerWrapper_Field::GetName()
{
  xtstring sRet;
  if (m_pKey)
    sRet = m_pKey->GetName();
  else if (m_pText)
    sRet = m_pText->GetName();
  else if (m_pArea)
    sRet = m_pArea->GetName();
  return sRet;
}
xtstring CFormatWorkerWrapper_Field::GetComment()
{
  xtstring sRet;
  if (m_pKey)
    sRet = m_pKey->GetComment();
  else if (m_pText)
    sRet = m_pText->GetComment();
  else if (m_pArea)
    sRet = m_pArea->GetComment();
  return sRet;
}
xtstring CFormatWorkerWrapper_Field::GetContentResult()
{
  xtstring sRet;
  if (m_pText)
    sRet = m_pText->GetContentResult();
  return sRet;
}
xtstring CFormatWorkerWrapper_Field::GetContentMask()
{
  xtstring sRet;
  if (m_pText)
    sRet = m_pText->GetContentTextMask();
  return sRet;
}
void CFormatWorkerWrapper_Field::SetKey(CMPModelKeyInd_ForFrm* pKey)
{
  m_pKey = pKey;
  RemoveNewValues();
  ClearElementID();
  if (m_pKey)
  {
    if (m_pKey->GetTagPropertyResult().size())
    {
      SetElementID(m_pKey->GetTagPropertyResult());
/*
      xtstring sTAG;
      sTAG = ADDRESS_DELIMITER;
      size_t nDelimiterLength = sTAG.size();
      sTAG = m_pKey->GetTagPropertyResult();
      size_t nPos = sTAG.find(ADDRESS_DELIMITER);
      if (nPos != xtstring::npos)
      {
        // wir haben auch Adresse
        xtstring s1;
        s1.assign(sTAG.begin(), sTAG.begin() + nPos);
        SetElementID(s1);
        s1.assign(sTAG.begin() + nPos + nDelimiterLength, sTAG.end());
      }
      else
        // wir haben NICHT Adresse
        SetElementID(m_pKey->GetTagPropertyResult());
*/
    }
  }
}
void CFormatWorkerWrapper_Field::SetText(CMPModelTextInd_ForFrm* pText)
{
  m_pText = pText;
  RemoveNewValues();
  ClearElementID();
  if (m_pText)
  {
    if (m_pText->GetTagPropertyResult().size())
    {
      SetElementID(m_pText->GetTagPropertyResult());
/*
      xtstring sTAG;
      sTAG = ADDRESS_DELIMITER;
      size_t nDelimiterLength = sTAG.size();
      sTAG = m_pText->GetTagPropertyResult();
      size_t nPos = sTAG.find(ADDRESS_DELIMITER);
      if (nPos != xtstring::npos)
      {
        // wir haben auch Adresse
        xtstring s1;
        s1.assign(sTAG.begin(), sTAG.begin() + nPos);
        SetElementID(s1);
        s1.assign(sTAG.begin() + nPos + nDelimiterLength, sTAG.end());
      }
      else
        // wir haben NICHT Adresse
        SetElementID(m_pText->GetTagPropertyResult());
*/
    }
  }
}
void CFormatWorkerWrapper_Field::SetArea(CMPModelAreaInd_ForFrm* pArea)
{
  m_pArea = pArea;
  RemoveNewValues();
  ClearElementID();
//  if (m_pArea)
//  {
//    if (m_pArea->GetTagPropertyResult().size())
//      SetElementID(m_pArea->GetTagPropertyResult());
//  }
}

bool CFormatWorkerWrapper_Field::ReadXMLContent(CXPubMarkUp* pXMLDoc)
{
  if (!CFormatWorkerWrapper::ReadXMLContent(pXMLDoc))
    return false;

  bool bRet = false;

  if (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_FIELD_KEY))
  {
    pXMLDoc->IntoElem();
    CMPModelKeyInd_ForFrm* pKey = new CMPModelKeyInd_ForFrm;
    if (pKey)
    {
      if (pXMLDoc->FindChildElem(KEY_ELEM_NAME))
      {
        pXMLDoc->IntoElem();
        if (pKey->ReadXMLContent(pXMLDoc))
        {
          m_pKey = pKey;
          bRet = true;
        }
        pXMLDoc->OutOfElem();
      }
      else
        delete pKey;
    }
    pXMLDoc->OutOfElem();
  }
  if (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_FIELD_TEXT))
  {
    pXMLDoc->IntoElem();
    CMPModelTextInd_ForFrm* pText = new CMPModelTextInd_ForFrm;
    if (pText)
    {
      if (pXMLDoc->FindChildElem(TEXT_ELEM_NAME))
      {
        pXMLDoc->IntoElem();
        if (pText->ReadXMLContent(pXMLDoc))
        {
          m_pText = pText;
          bRet = true;
        }
        pXMLDoc->OutOfElem();
      }
      else
        delete pText;
    }
    pXMLDoc->OutOfElem();
  }
  if (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_FIELD_AREA))
  {
    pXMLDoc->IntoElem();
    CMPModelAreaInd_ForFrm* pArea = new CMPModelAreaInd_ForFrm;
    if (pArea)
    {
      if (pXMLDoc->FindChildElem(AREA_ELEM_NAME))
      {
        pXMLDoc->IntoElem();
        if (pArea->ReadXMLContent(pXMLDoc))
        {
          m_pArea = pArea;
          bRet = true;
        }
        pXMLDoc->OutOfElem();
      }
      else
        delete pArea;
    }
    pXMLDoc->OutOfElem();
  }

  return bRet;
}
bool CFormatWorkerWrapper_Field::SaveXMLContent(CXPubMarkUp* pXMLDoc)
{
  if (!CFormatWorkerWrapper::SaveXMLContent(pXMLDoc))
    return false;

  bool bRet = true;
  if (m_pKey)
  {
    if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_FIELD_KEY, _XT("")))
    {
      pXMLDoc->IntoElem();
      bRet = m_pKey->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
    }
  }
  if (m_pText)
  {
    if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_FIELD_TEXT, _XT("")))
    {
      pXMLDoc->IntoElem();
      bRet = m_pText->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
    }
  }
  if (m_pArea)
  {
    if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_FIELD_AREA, _XT("")))
    {
      pXMLDoc->IntoElem();
      bRet = m_pArea->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
    }
  }

  return bRet;
}














////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CFormatWorkerWrapper_Stone
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CFormatWorkerWrapper_Stone::CFormatWorkerWrapper_Stone(CFormatWorkerWrapper_Root* pRoot,
                                                       CFormatWorkerWrapper* pParent)
                           :CFormatWorkerWrapper(CFormatWorkerWrapper::tTOFWW_Stone,
                                                 pRoot,
                                                 pParent)
{
}
CFormatWorkerWrapper_Stone::CFormatWorkerWrapper_Stone(const CFormatWorkerWrapper_Stone& cStone)
                           :CFormatWorkerWrapper(cStone)
{
}
CFormatWorkerWrapper_Stone::~CFormatWorkerWrapper_Stone()
{
}
xtstring CFormatWorkerWrapper_Stone::GetName()
{
  xtstring sRet;
  if (m_pFormatWorker)
    sRet = m_pFormatWorker->GetWorkPaperName();
  return sRet;
}
xtstring CFormatWorkerWrapper_Stone::GetComment()
{
  xtstring sRet;
  if (m_pFormatWorker)
    sRet = m_pFormatWorker->GetWorkPaperComment();
  return sRet;
}
void CFormatWorkerWrapper_Stone::SetFormatWorker(CFormatWorker* pFormatWorker)
{
  m_pFormatWorker = pFormatWorker;
  RemoveNewValues();
  ClearElementID();
  if (m_pFormatWorker)
  {
    if (m_pFormatWorker->GetTagProperty().size())
    {
      SetElementID(m_pFormatWorker->GetTagProperty());
/*
      xtstring sTAG;
      sTAG = ADDRESS_DELIMITER;
      size_t nDelimiterLength = sTAG.size();
      sTAG = m_pFormatWorker->GetTagProperty();
      size_t nPos = sTAG.find(ADDRESS_DELIMITER);
      if (nPos != xtstring::npos)
      {
        // wir haben auch Adresse
        xtstring s1;
        s1.assign(sTAG.begin(), sTAG.begin() + nPos);
        SetElementID(s1);
        s1.assign(sTAG.begin() + nPos + nDelimiterLength, sTAG.end());
      }
      else
        // wir haben NICHT Adresse
        SetElementID(m_pFormatWorker->GetTagProperty());
*/
    }
    else if (m_pFormatWorker->GetWorkPaperDataFormat() == tDataFormatST_Property)
    {
      // Falls definiert, es wird gesetzt
      SetElementID(GetPS_Me_ElementID());
    }
  }
}

bool CFormatWorkerWrapper_Stone::ReadXMLContent(CXPubMarkUp* pXMLDoc)
{
  if (!CFormatWorkerWrapper::ReadXMLContent(pXMLDoc))
    return false;

  while (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_STONE))
  {
    pXMLDoc->IntoElem();

    CFormatWorkerWrapper_Stone* pNewElement;
    pNewElement = new CFormatWorkerWrapper_Stone(GetRoot(), this);
    if (pNewElement)
    {
      bool bRet = false;
      bRet = pNewElement->ReadXMLContent(pXMLDoc);
      if (bRet)
        bRet = AddStoneWrapper(pNewElement);
      if (!bRet)
        delete pNewElement;
    }

    pXMLDoc->OutOfElem();
  }
  while (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_FIELD))
  {
    pXMLDoc->IntoElem();

    CFormatWorkerWrapper_Field* pNewElement;
    pNewElement = new CFormatWorkerWrapper_Field(GetRoot(), this);
    if (pNewElement)
    {
      bool bRet = false;
      bRet = pNewElement->ReadXMLContent(pXMLDoc);
      if (bRet)
        bRet = AddFieldWrapper(pNewElement);
      if (!bRet)
        delete pNewElement;
    }

    pXMLDoc->OutOfElem();
  }
  return true;
}
bool CFormatWorkerWrapper_Stone::SaveXMLContent(CXPubMarkUp* pXMLDoc)
{
  if (!CFormatWorkerWrapper::SaveXMLContent(pXMLDoc))
    return false;

  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pWrapper = GetStoneWrapper(nI);
    if (!pWrapper)
      continue;
    if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_STONE, _XT("")))
    {
      pXMLDoc->IntoElem();

      if (!pWrapper->SaveXMLContent(pXMLDoc))
        return false;

      pXMLDoc->OutOfElem();
    }
    else
      return false;
  }
  for (size_t nI = 0; nI < CountOfFieldWrappers(); nI++)
  {
    CFormatWorkerWrapper_Field* pWrapper = GetFieldWrapper(nI);
    if (!pWrapper)
      continue;
    if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_FIELD, _XT("")))
    {
      pXMLDoc->IntoElem();

      if (!pWrapper->SaveXMLContent(pXMLDoc))
        return false;

      pXMLDoc->OutOfElem();
    }
    else
      return false;
  }
  return true;
}
















////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CFormatWorkerWrapper_Template
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CFormatWorkerWrapper_Template::CFormatWorkerWrapper_Template(CFormatWorkerWrapper_Root* pRoot,
                                                             CFormatWorkerWrapper* pParent)
                              :CFormatWorkerWrapper(CFormatWorkerWrapper::tTOFWW_Template,
                                                    pRoot,
                                                    pParent)
{
  m_bFlatStyle = false;
}
CFormatWorkerWrapper_Template::CFormatWorkerWrapper_Template(const CFormatWorkerWrapper_Template& cTemplate)
                              :CFormatWorkerWrapper(cTemplate)
{
  m_bFlatStyle = cTemplate.m_bFlatStyle;
}
CFormatWorkerWrapper_Template::~CFormatWorkerWrapper_Template()
{
}
xtstring CFormatWorkerWrapper_Template::GetName()
{
  xtstring sRet;
  if (m_pFormatWorker)
    sRet = m_pFormatWorker->GetWorkPaperName();
  return sRet;
}
xtstring CFormatWorkerWrapper_Template::GetComment()
{
  xtstring sRet;
  if (m_pFormatWorker)
    sRet = m_pFormatWorker->GetWorkPaperComment();
  return sRet;
}
xtstring CFormatWorkerWrapper_Template::GetFileShowName()
{
  xtstring sRet;
  if (m_pFormatWorker)
    sRet = m_pFormatWorker->GetFileShowName();
  return sRet;
}
void CFormatWorkerWrapper_Template::SetFormatWorker(CFormatWorker* pFormatWorker)
{
  m_pFormatWorker = pFormatWorker;
  RemoveNewValues();
  ClearElementID();
  if (m_pFormatWorker)
  {
    if (m_pFormatWorker->GetTagProperty().size())
    {
      SetElementID(m_pFormatWorker->GetTagProperty());
/*
      xtstring sTAG;
      sTAG = ADDRESS_DELIMITER;
      size_t nDelimiterLength = sTAG.size();
      sTAG = m_pFormatWorker->GetTagProperty();
      size_t nPos = sTAG.find(ADDRESS_DELIMITER);
      if (nPos != xtstring::npos)
      {
        // wir haben auch Adresse
        xtstring s1;
        s1.assign(sTAG.begin(), sTAG.begin() + nPos);
        SetElementID(s1);
        s1.assign(sTAG.begin() + nPos + nDelimiterLength, sTAG.end());
      }
      else
        // wir haben NICHT Adresse
        SetElementID(m_pFormatWorker->GetTagProperty());
*/
    }
  }
}

bool CFormatWorkerWrapper_Template::ReadXMLContent(CXPubMarkUp* pXMLDoc)
{
  if (!CFormatWorkerWrapper::ReadXMLContent(pXMLDoc))
    return false;

  while (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_TEMPLATE))
  {
    pXMLDoc->IntoElem();

    CFormatWorkerWrapper_Template* pNewElement;
    pNewElement = new CFormatWorkerWrapper_Template(GetRoot(), this);
    if (pNewElement)
    {
      bool bRet = false;
      bRet = pNewElement->ReadXMLContent(pXMLDoc);
      if (bRet)
        bRet = AddTemplateWrapper(pNewElement);
      if (!bRet)
        delete pNewElement;
    }

    pXMLDoc->OutOfElem();
  }
  while (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_STONE))
  {
    pXMLDoc->IntoElem();

    CFormatWorkerWrapper_Stone* pNewElement;
    pNewElement = new CFormatWorkerWrapper_Stone(GetRoot(), this);
    if (pNewElement)
    {
      bool bRet = false;
      bRet = pNewElement->ReadXMLContent(pXMLDoc);
      if (bRet)
        bRet = AddStoneWrapper(pNewElement);
      if (bRet)
      {
        if (pNewElement->WrapperIsPropStone())
        {
          xtstring sTextInNameProp;
          sTextInNameProp = pNewElement->GetPS_Me_TextInNameProperty();
          if (!sTextInNameProp.CompareNoCase(PROP_NAMETEXT_FLATHEADER))
            m_bFlatStyle = true;
        }
      }
      if (!bRet)
        delete pNewElement;
    }

    pXMLDoc->OutOfElem();
  }
  while (pXMLDoc->FindChildElem(WORKPAPERWRAPPER_FIELD))
  {
    pXMLDoc->IntoElem();

    CFormatWorkerWrapper_Field* pNewElement;
    pNewElement = new CFormatWorkerWrapper_Field(GetRoot(), this);
    if (pNewElement)
    {
      bool bRet = false;
      bRet = pNewElement->ReadXMLContent(pXMLDoc);
      if (bRet)
        bRet = AddFieldWrapper(pNewElement);
      if (!bRet)
        delete pNewElement;
    }

    pXMLDoc->OutOfElem();
  }
  return true;
}
bool CFormatWorkerWrapper_Template::SaveXMLContent(CXPubMarkUp* pXMLDoc)
{
  if (!CFormatWorkerWrapper::SaveXMLContent(pXMLDoc))
    return false;

  for (size_t nI = 0; nI < CountOfTemplateWrappers(); nI++)
  {
    CFormatWorkerWrapper_Template* pWrapper = GetTemplateWrapper(nI);
    if (!pWrapper)
      continue;
    if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_TEMPLATE, _XT("")))
    {
      pXMLDoc->IntoElem();

      if (!pWrapper->SaveXMLContent(pXMLDoc))
        return false;

      pXMLDoc->OutOfElem();
    }
    else
      return false;
  }
  for (size_t nI = 0; nI < CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper_Stone* pWrapper = GetStoneWrapper(nI);
    if (!pWrapper)
      continue;
    if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_STONE, _XT("")))
    {
      pXMLDoc->IntoElem();

      if (!pWrapper->SaveXMLContent(pXMLDoc))
        return false;

      pXMLDoc->OutOfElem();
    }
    else
      return false;
  }
  for (size_t nI = 0; nI < CountOfFieldWrappers(); nI++)
  {
    CFormatWorkerWrapper_Field* pWrapper = GetFieldWrapper(nI);
    if (!pWrapper)
      continue;
    if (pXMLDoc->AddChildElem(WORKPAPERWRAPPER_FIELD, _XT("")))
    {
      pXMLDoc->IntoElem();

      if (!pWrapper->SaveXMLContent(pXMLDoc))
        return false;

      pXMLDoc->OutOfElem();
    }
    else
      return false;
  }
  return true;
}
bool CFormatWorkerWrapper_Template::GetXMLContent(xtstring& sXMLDoc)
{
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(EMPTY_WORKPAPER_WRAPPER_CONTENT_XML))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_TAG_WORKPAPER_WRAPPER_CONTENT)
  {
    if (cXMLDoc.AddChildElem(WORKPAPERWRAPPER_TEMPLATE, _XT("")))
    {
      cXMLDoc.IntoElem();

      if (!SaveXMLContent(&cXMLDoc))
        return false;

      cXMLDoc.OutOfElem();
    }
    else
      return false;
  }
  else
    return false;
  sXMLDoc = cXMLDoc.GetDoc();
  return true;
}
bool CFormatWorkerWrapper_Template::SetXMLContent(const xtstring& sXMLDoc)
{
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXMLDoc.c_str()))
    return false;

  cXMLDoc.ResetPos();

  bool bRet = false;
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_TAG_WORKPAPER_WRAPPER_CONTENT)
  {
    if (cXMLDoc.FindChildElem(WORKPAPERWRAPPER_TEMPLATE))
    {
      cXMLDoc.IntoElem();

      bRet = ReadXMLContent(&cXMLDoc);

      cXMLDoc.OutOfElem();
    }
    else
      return false;
  }

  return bRet;
}
















////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CFormatWorkerWrapper_Document
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CFormatWorkerWrapper_Document::CFormatWorkerWrapper_Document(long nIndex,
                                                             CFormatWorkerWrapper_Root* pRoot)
{
  m_bTemplateOpened = false;

  m_nIndexOfTopTemplate = nIndex;
  m_sName.clear();
  m_sComment.clear();
  m_sFileShowName.clear();
  m_sTemplateContent.clear();

  m_pTemplate = 0;
  m_pRoot = pRoot;
}
CFormatWorkerWrapper_Document::~CFormatWorkerWrapper_Document()
{
  if (m_pTemplate)
    delete m_pTemplate;
  m_pTemplate = 0;
}
void CFormatWorkerWrapper_Document::operator = (const CFormatWorkerWrapper_Document* pDocument)
{
  if (!pDocument)
    return;
  m_sName = pDocument->m_sName;
  m_sComment = pDocument->m_sComment;
  m_sFileShowName = pDocument->m_sFileShowName;
  m_sTemplateContent = pDocument->m_sTemplateContent;
}
bool CFormatWorkerWrapper_Document::ReadXMLContent(CXPubMarkUp* pXMLDoc)
{
  m_nIndexOfTopTemplate = pXMLDoc->GetAttribLong(TOPTEMPLATE_INDEX);
  m_sName = pXMLDoc->GetAttrib(TOPTEMPLATE_NAME);
  m_sComment = pXMLDoc->GetAttrib(TOPTEMPLATE_COMMENT);
  m_sFileShowName = pXMLDoc->GetAttrib(TOPTEMPLATE_FILESHOWNAME);
  m_sTemplateContent = pXMLDoc->GetData();
  return true;
}
bool CFormatWorkerWrapper_Document::SaveXMLContent(CXPubMarkUp* pXMLDoc)
{
  pXMLDoc->SetAttrib(TOPTEMPLATE_INDEX, m_nIndexOfTopTemplate);
  pXMLDoc->SetAttrib(TOPTEMPLATE_NAME, m_sName.c_str());
  pXMLDoc->SetAttrib(TOPTEMPLATE_COMMENT, m_sComment.c_str());
  pXMLDoc->SetAttrib(TOPTEMPLATE_FILESHOWNAME, m_sFileShowName.c_str());
  pXMLDoc->SetData(m_sTemplateContent.c_str());
  return true;
}
void CFormatWorkerWrapper_Document::CloseTemplate(bool bSerializeTemplate)
{
  m_bTemplateOpened = false;
  if (m_pTemplate && bSerializeTemplate)
    m_pTemplate->GetXMLContent(m_sTemplateContent);
  if (m_pTemplate)
    delete m_pTemplate;
  m_pTemplate = 0;
}
void CFormatWorkerWrapper_Document::OpenTemplate()
{
  m_bTemplateOpened = true;
  m_pTemplate = new CFormatWorkerWrapper_Template(m_pRoot, 0);
  if (m_pTemplate)
    m_pTemplate->SetXMLContent(m_sTemplateContent);
}
















////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CFormatWorkerWrapper_Root
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CFormatWorkerWrapper_Root::CFormatWorkerWrapper_Root()
{
  m_pAddOnManager = 0;
  m_pActualWorkPaperWrapper = 0;
  m_pGetAllUsedRemoteExecStone = 0;
  m_pGetAllUsedLocalExecStone = 0;
  m_pValueAndStatusStone = 0;
	m_pGlobalSetting = 0;
}
CFormatWorkerWrapper_Root::CFormatWorkerWrapper_Root(CXPubAddOnManager* pAddOnManager)
{
  m_pAddOnManager = pAddOnManager;
  m_pActualWorkPaperWrapper = 0;
}
CFormatWorkerWrapper_Root::~CFormatWorkerWrapper_Root()
{
  RemoveDocuments();
}

bool CFormatWorkerWrapper_Root::AddGeneratedWorkPaper(const xtstring& sXMLMemoryContent)
{
  CFormatWorkerWrapper_Document* pNewDocument;
  pNewDocument = CreateNewDocument((long)CountOfDocuments(), sXMLMemoryContent);
  if (!pNewDocument)
    return false;

  if (!AddOrInsertDocument(pNewDocument))
  {
    delete pNewDocument;
    return false;
  }
  return true;
}

bool CFormatWorkerWrapper_Root::SetGeneratedWorkPaper_MainDocument(const xtstring& sXMLMemoryContent)
{
  CFormatWorkerWrapper_Document* pNewDocument;
  pNewDocument = CreateNewDocument(tMainDocument_Index, sXMLMemoryContent);
  if (!pNewDocument)
    return false;

  if (!AddOrInsertDocument(pNewDocument, true))
  {
    delete pNewDocument;
    return false;
  }
  return true;
}
bool CFormatWorkerWrapper_Root::SetGeneratedWorkPaper_MainDocument(const CFormatWorkerWrapper_Document* pDocument)
{
  CFormatWorkerWrapper_Document* pNewDocument;
  pNewDocument = new CFormatWorkerWrapper_Document(tMainDocument_Index, this);
  if (!pNewDocument)
    return false;
  (*pNewDocument) = pDocument;
  if (!AddOrInsertDocument(pNewDocument, true))
  {
    delete pNewDocument;
    return false;
  }
  return true;
}
bool CFormatWorkerWrapper_Root::SetGeneratedWorkPaper_SatelliteDocument(const xtstring& sXMLMemoryContent)
{
  CFormatWorkerWrapper_Document* pNewDocument;
  pNewDocument = CreateNewDocument(tStalliteDocument_Index, sXMLMemoryContent);
  if (!pNewDocument)
    return false;

  RemoveDocuments_ExceptFirst();
  if (!AddOrInsertDocument(pNewDocument))
  {
    delete pNewDocument;
    return false;
  }
  return true;
}
bool CFormatWorkerWrapper_Root::SetGeneratedWorkPaper_SatelliteDocument(const CFormatWorkerWrapper_Document* pDocument)
{
  CFormatWorkerWrapper_Document* pNewDocument;
  pNewDocument = new CFormatWorkerWrapper_Document(tStalliteDocument_Index, this);
  if (!pNewDocument)
    return false;
  (*pNewDocument) = pDocument;
  RemoveDocuments_ExceptFirst();
  if (!AddOrInsertDocument(pNewDocument))
  {
    delete pNewDocument;
    return false;
  }
  return true;
}

bool CFormatWorkerWrapper_Root::CloseActiveTemplateWrapper(bool bSerializeTemplate)
{
  m_pActualWorkPaperWrapper = 0;

  for (CFWW_DocumentsIterator it = m_cDocuments.begin(); it != m_cDocuments.end(); it++)
    if ((*it)->m_bTemplateOpened)
      (*it)->CloseTemplate(bSerializeTemplate);
  return true;
}
bool CFormatWorkerWrapper_Root::ActivateTemplateWrapper(size_t nIndex, bool bSerializeOldTemplate)
{
  m_pActualWorkPaperWrapper = 0;

  for (CFWW_DocumentsIterator it = m_cDocuments.begin(); it != m_cDocuments.end(); it++)
    if ((*it)->m_bTemplateOpened)
      (*it)->CloseTemplate(bSerializeOldTemplate);

  bool bRet = false;
  CFormatWorkerWrapper_Document* p = GetDocument(nIndex);
  if (p)
  {
    p->OpenTemplate();
    m_pActualWorkPaperWrapper = p->m_pTemplate;
    bRet = true;
  }
  return bRet;
}
bool CFormatWorkerWrapper_Root::ActivateTemplateWrapper_MainDocument(bool bSerializeOldTemplate)
{
  m_pActualWorkPaperWrapper = 0;

  for (CFWW_DocumentsIterator it = m_cDocuments.begin(); it != m_cDocuments.end(); it++)
    if ((*it)->m_bTemplateOpened)
      (*it)->CloseTemplate(bSerializeOldTemplate);

  bool bRet = false;
  CFormatWorkerWrapper_Document* p = GetDocument(tMainDocument_Index);
  if (p)
  {
    p->OpenTemplate();
    m_pActualWorkPaperWrapper = p->m_pTemplate;
    bRet = true;
  }
  return bRet;
}
bool CFormatWorkerWrapper_Root::ActivateTemplateWrapper_SatelliteDocument(bool bSerializeOldTemplate)
{
  m_pActualWorkPaperWrapper = 0;

  for (CFWW_DocumentsIterator it = m_cDocuments.begin(); it != m_cDocuments.end(); it++)
    if ((*it)->m_bTemplateOpened)
      (*it)->CloseTemplate(bSerializeOldTemplate);

  bool bRet = false;
  CFormatWorkerWrapper_Document* p = GetDocument(tStalliteDocument_Index);
  if (p)
  {
    p->OpenTemplate();
    m_pActualWorkPaperWrapper = p->m_pTemplate;
    bRet = true;
  }
  return bRet;
}
xtstring CFormatWorkerWrapper_Root::GetDocumentName(size_t nIndex)
{
  xtstring sRet;
  CFormatWorkerWrapper_Document* p = GetDocument(nIndex);
  if (p)
    sRet = p->GetName();
  return sRet;
}
xtstring CFormatWorkerWrapper_Root::GetDocumentComment(size_t nIndex)
{
  xtstring sRet;
  CFormatWorkerWrapper_Document* p = GetDocument(nIndex);
  if (p)
    sRet = p->GetComment();
  return sRet;
}
xtstring CFormatWorkerWrapper_Root::GetDocumentFileShowName(size_t nIndex)
{
  xtstring sRet;
  CFormatWorkerWrapper_Document* p = GetDocument(nIndex);
  if (p)
    sRet = p->GetFileShowName();
  return sRet;
}

CFormatWorkerWrapper_Template* CFormatWorkerWrapper_Root::GetActualTemplateWrapper()
{
  return m_pActualWorkPaperWrapper;
}
CFormatWorkerWrapper_Document* CFormatWorkerWrapper_Root::CreateNewDocument(long nDocumentIndex,
                                                                            const xtstring& sXMLMemoryContent)
{
  if (!m_pAddOnManager)
    return 0;

  CFormatWorkerWrapper_Template* pNewElement;
  pNewElement = m_pAddOnManager->CreateTemplateWrapperClass(this, sXMLMemoryContent);
  if (!pNewElement)
    return 0;

  // Wir haben jetzt eine Vorlage.
  // Daten kopieren
  CFormatWorkerWrapper_Document* pNewDocument;
  pNewDocument = new CFormatWorkerWrapper_Document(nDocumentIndex, this);
  if (!pNewDocument)
  {
    delete pNewElement;
    return 0;
  }
  pNewDocument->m_sName = pNewElement->GetName();
  pNewDocument->m_sComment = pNewElement->GetComment();
  pNewDocument->m_sFileShowName = pNewElement->GetFileShowName();

  // Jetzt machen wir Voreinstellungen.
  // Es war verlangt, dass die TOP Vorlage einem "opened" Stand haben.
  pNewElement->SetGUITreeStatus(CFormatWorkerWrapper::tGTS_Expanded);

  pNewElement->GetXMLContent(pNewDocument->m_sTemplateContent);
  delete pNewElement;

  return pNewDocument;
}
bool CFormatWorkerWrapper_Root::AddOrInsertDocument(CFormatWorkerWrapper_Document* pDocument,
                                                    bool bInsertAtFront /*= false*/)
{
  if (!pDocument)
    return false;

  size_t nCount = CountOfDocuments();
  if (bInsertAtFront && nCount)
  {
    // insert
    CFWW_DocumentsIterator it = m_cDocuments.begin();
    delete (*it);
    m_cDocuments.erase(m_cDocuments.begin());
    m_cDocuments.insert(m_cDocuments.begin(), pDocument);
    size_t nCount1 = CountOfDocuments();
    nCount1 = CountOfDocuments();
  }
  else
  {
    // add
    m_cDocuments.push_back(pDocument);
    if (nCount == CountOfDocuments())
      return false;
  }

  if (nCount == 0 || bInsertAtFront)
  {
    // jetzt kommt da Spezialitaet
    if (m_pGetAllUsedRemoteExecStone)
      delete m_pGetAllUsedRemoteExecStone;
    if (m_pGetAllUsedLocalExecStone)
      delete m_pGetAllUsedLocalExecStone;
    if (m_pValueAndStatusStone)
      delete m_pValueAndStatusStone;
    if (m_pGlobalSetting)
      delete m_pGlobalSetting;
    
		m_pGetAllUsedRemoteExecStone = 0;
    m_pGetAllUsedLocalExecStone = 0;
    m_pValueAndStatusStone = 0;
		m_pGlobalSetting = 0;

    if (!pDocument->GetTemplate())
      pDocument->OpenTemplate();
    if (pDocument->GetTemplate())
    {
      if (pDocument->GetTemplate()->GetPS_Child_ExecuteActionRemote(PROP_ACTIONIDTEXT_GETALLUSED))
        m_pGetAllUsedRemoteExecStone = new CFormatWorkerWrapper_Stone(*(pDocument->GetTemplate()->GetPS_Child_ExecuteActionRemoteWrapper(PROP_ACTIONIDTEXT_GETALLUSED)));
      if (pDocument->GetTemplate()->GetPS_Child_ExecuteActionLocal(PROP_ACTIONIDTEXT_GETALLUSED))
        m_pGetAllUsedLocalExecStone = new CFormatWorkerWrapper_Stone(*(pDocument->GetTemplate()->GetPS_Child_ExecuteActionLocalWrapper(PROP_ACTIONIDTEXT_GETALLUSED)));
      if (pDocument->GetTemplate()->GetPS_Child_ValueAndStatusWrapper())
        m_pValueAndStatusStone = new CFormatWorkerWrapper_Stone(*(pDocument->GetTemplate()->GetPS_Child_ValueAndStatusWrapper()));
      if (pDocument->GetTemplate()->GetPS_Child_GlobalSettingsWrapper())
        m_pGlobalSetting = new CFormatWorkerWrapper_Stone(*(pDocument->GetTemplate()->GetPS_Child_GlobalSettingsWrapper()));
    
		}
    pDocument->CloseTemplate(false);
  }

  return true;
}
const CFormatWorkerWrapper_Document* CFormatWorkerWrapper_Root::GetDocumentConst(size_t nIndex)
{
  if (nIndex >= m_cDocuments.size())
    return 0;
  return m_cDocuments[nIndex];
}
CFormatWorkerWrapper_Document* CFormatWorkerWrapper_Root::GetDocument(size_t nIndex)
{
  if (nIndex >= m_cDocuments.size())
    return 0;
  return m_cDocuments[nIndex];
}
xtstring CFormatWorkerWrapper_Root::GetXMLVersion()
{
  return m_sXMLVersion;
}
void CFormatWorkerWrapper_Root::SetXMLVersion(const xtstring& sXMLVersion)
{
  m_sXMLVersion = sXMLVersion;
}
bool CFormatWorkerWrapper_Root::RemoveDocuments()
{
  for (CFWW_DocumentsIterator it = m_cDocuments.begin(); it != m_cDocuments.end(); it++)
    delete (*it);
  m_cDocuments.erase(m_cDocuments.begin(), m_cDocuments.end());

  // jetzt kommt da Spezialitaet
  if (m_pGetAllUsedRemoteExecStone)
    delete m_pGetAllUsedRemoteExecStone;
  if (m_pGetAllUsedLocalExecStone)
    delete m_pGetAllUsedLocalExecStone;
  if (m_pValueAndStatusStone)
    delete m_pValueAndStatusStone;
  if (m_pGlobalSetting)
    delete m_pGlobalSetting;

  m_pGetAllUsedRemoteExecStone = 0;
  m_pGetAllUsedLocalExecStone = 0;
  m_pValueAndStatusStone = 0;
  m_pGlobalSetting = 0;

  return true;
}
bool CFormatWorkerWrapper_Root::RemoveDocuments_ExceptFirst()
{
  for (CFWW_DocumentsIterator it = m_cDocuments.begin(); it != m_cDocuments.end(); it++)
  {
    if (it == m_cDocuments.begin())
      continue;
    delete (*it);
  }
  m_cDocuments.erase(m_cDocuments.begin() + 1, m_cDocuments.end());
  return true;
}
void CFormatWorkerWrapper_Root::ResetInternalVariablesInActualTemplate()
{
  if (m_pActualWorkPaperWrapper)
    m_pActualWorkPaperWrapper->ResetInternalVariables();
}



bool CFormatWorkerWrapper_Root::ReadAllDocumentsFromMemory(const xtstring& sXMLMemoryContent)
{
  CFormatWorker* pRet = 0;

  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXMLMemoryContent.c_str()))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_TAG_WORKPAPERS)
  {
    m_sXMLVersion = cXMLDoc.GetAttrib(PROP_XMLVERSION);
    while (cXMLDoc.FindChildElem(TOPTEMPLATE))
    {
      cXMLDoc.IntoElem();

      CFormatWorkerWrapper_Document* pNewElement;
      pNewElement = new CFormatWorkerWrapper_Document((long)CountOfDocuments(), this);
      if (pNewElement)
      {
        bool bRet = false;
        bRet = pNewElement->ReadXMLContent(&cXMLDoc);
        if (bRet)
          bRet = AddOrInsertDocument(pNewElement);
        if (!bRet)
          delete pNewElement;
      }

      cXMLDoc.OutOfElem();
    }
  }

  return true;
}
bool CFormatWorkerWrapper_Root::SaveAllDocumentsToMemory(xtstring& sXMLMemoryContent, bool bSerializeTemplate)
{
  for (CFWW_DocumentsIterator it = m_cDocuments.begin(); it != m_cDocuments.end(); it++)
    if ((*it)->m_bTemplateOpened)
      (*it)->CloseTemplate(bSerializeTemplate);

  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(ALL_WORKPAPERS_XML))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_TAG_WORKPAPERS)
  {
    cXMLDoc.SetAttrib(PROP_XMLVERSION, m_sXMLVersion.c_str());
    for (size_t nI = 0; nI < CountOfDocuments(); nI++)
    {
      CFormatWorkerWrapper_Document* pDocument = GetDocument(nI);
      if (!pDocument)
        continue;
      if (cXMLDoc.AddChildElem(TOPTEMPLATE, _XT("")))
      {
        cXMLDoc.IntoElem();

        if (!pDocument->SaveXMLContent(&cXMLDoc))
          return false;

        cXMLDoc.OutOfElem();
      }
      else
        return false;
    }
  }
  else
    return false;
  sXMLMemoryContent = cXMLDoc.GetDoc();
  return true;
}

CFormatWorkerWrapper_Template* CFormatWorkerWrapper_Root::GetTopTemplateOfWPWrapper(CFormatWorkerWrapper* pWrapper)
{
  if (!m_pActualWorkPaperWrapper)
    return 0;
  if (m_pActualWorkPaperWrapper->IsThisWrapperChild(pWrapper))
    return m_pActualWorkPaperWrapper;
  return 0;
}

void CFormatWorkerWrapper_Root::CheckStatusOfAllObjects()
{
  if (m_pActualWorkPaperWrapper)
    m_pActualWorkPaperWrapper->CheckStatusOfAllObjects();
}

bool CFormatWorkerWrapper_Root::GetCRC32(const xtstring& sElementID,
                                         unsigned long& nCRC32)
{
  if (m_pActualWorkPaperWrapper)
    return m_pActualWorkPaperWrapper->GetCRC32(sElementID, nCRC32);
  return false;
}
bool CFormatWorkerWrapper_Root::SetCRC32(const xtstring& sElementID,
                                         unsigned long nCRC32)
{
  if (m_pActualWorkPaperWrapper)
    return m_pActualWorkPaperWrapper->SetCRC32(sElementID, nCRC32);
  return false;
}
bool CFormatWorkerWrapper_Root::GetRawContent(const xtstring& sElementID,
                                              const xtstring& sStartElementTyp,
                                              bool bTextFocusActive,
                                              xtstring& sRawContent)
{
  if (m_pActualWorkPaperWrapper)
  {
    bool bFlatStyle = m_pActualWorkPaperWrapper->GetPS_Child_FlatHeader_ColumnDefinition_Defined();
    return m_pActualWorkPaperWrapper->GetRawContent(sElementID, sStartElementTyp, bTextFocusActive, bFlatStyle, sRawContent);
  }
  return false;
}

void CFormatWorkerWrapper_Root::GetAllElementID(CWrapperObjectStatusVector& cTemplates,
                                                CWrapperObjectStatusVector& cStones,
                                                CWrapperObjectStatusVector& cFields)
{
  cTemplates.clear();
  cStones.clear();
  cFields.clear();

  CWrapperObjectStatusMap cT, cS, cF;
  if (m_pActualWorkPaperWrapper)
    m_pActualWorkPaperWrapper->GetAllElementID(cT, cS, cF);

  for (CWrapperObjectStatusMapIterator it = cT.begin(); it != cT.end(); it++)
    cTemplates.push_back(it->second);
  for (CWrapperObjectStatusMapIterator it = cS.begin(); it != cS.end(); it++)
    cStones.push_back(it->second);
  for (CWrapperObjectStatusMapIterator it = cF.begin(); it != cF.end(); it++)
    cFields.push_back(it->second);
}

bool CFormatWorkerWrapper_Root::GetStatusOfObject(const xtstring& sElementID,
                                                  TStatusOfFWWrapper& nStatusOfObject)
{
  if (m_pActualWorkPaperWrapper)
    return m_pActualWorkPaperWrapper->GetStatusOfObject(sElementID, nStatusOfObject);
  return false;
}

bool CFormatWorkerWrapper_Root::SetStatusOfObject(const xtstring& sElementID,
                                                  TStatusOfFWWrapper nStatusOfObject)
{
  if (m_pActualWorkPaperWrapper)
  {
    // wir muessen zuerst kontrollieren, ob da lock ist
    size_t nPos = sElementID.FindNoCase(KEYWORD_READONLY);
    if (nPos != xtstring::npos)
    {
      // es ist was drin, wir kontrollieren es genau
      xtstring sNewElementID(sElementID);
      int nCount = sNewElementID.GetFieldCount(ELEMENTID_DELIMITER);
      for (int nI = 0; nI < nCount; nI++)
      {
        xtstring sField = sNewElementID.GetField(ELEMENTID_DELIMITER, nI);
        if (!sField.CompareNoCase(KEYWORD_READONLY))
        {
          nStatusOfObject = tSOFWW_Default;
          break;
        }
      }
    }
    return m_pActualWorkPaperWrapper->SetStatusOfObject(sElementID, nStatusOfObject);
  }
  return false;
}

void CFormatWorkerWrapper_Root::ResetStatusOfAllObjects()
{
  if (m_pActualWorkPaperWrapper)
    return m_pActualWorkPaperWrapper->ResetStatusOfAllObjects();
}

CFormatWorkerWrapper* CFormatWorkerWrapper_Root::GetFormatWorkerWrapper(const xtstring& sElementID,
                                                                        CFormatWorkerWrapper** pParentOfFlatField/*= 0*/)
{
  CFormatWorkerWrapper* pRet = 0;
  if (m_pActualWorkPaperWrapper)
    pRet = m_pActualWorkPaperWrapper->GetFormatWorkerWrapper(sElementID, pParentOfFlatField);
  return pRet;
}

bool CFormatWorkerWrapper_Root::GetValue(const xtstring& sElementID,
                                         xtstring& sContent,
                                         xtstring& sContentMask,
                                         xtstring& sNewContent)
{
  if (m_pActualWorkPaperWrapper)
    return m_pActualWorkPaperWrapper->GetValue(sElementID, sContent, sContentMask, sNewContent);
  return false;
}

bool CFormatWorkerWrapper_Root::SetValue(const xtstring& sElementID,
                                         const xtstring& sNewContent)
{
  if (m_pActualWorkPaperWrapper)
  {
    // wir muessen zuerst kontrollieren, ob da lock ist
    size_t nPos = sElementID.FindNoCase(KEYWORD_READONLY);
    if (nPos != xtstring::npos)
    {
      // es ist was drin, wir kontrollieren es genau
      xtstring sNewElementID(sElementID);
      int nCount = sNewElementID.GetFieldCount(ELEMENTID_DELIMITER);
      for (int nI = 0; nI < nCount; nI++)
      {
        xtstring sField = sNewElementID.GetField(ELEMENTID_DELIMITER, nI);
        if (!sField.CompareNoCase(KEYWORD_READONLY))
          return false;;
      }
    }
    return m_pActualWorkPaperWrapper->SetValue(sElementID, sNewContent);
  }
  return false;
}

void CFormatWorkerWrapper_Root::ResetNewValues()
{
  if (m_pActualWorkPaperWrapper)
    m_pActualWorkPaperWrapper->ResetNewValues();
}

xtstring CFormatWorkerWrapper_Root::GetClientAddressOfParent(const xtstring& sElementID)
{
  bool bElementIDFound = false;
  if (m_pActualWorkPaperWrapper)
    return m_pActualWorkPaperWrapper->GetClientAddressOfParent(bElementIDFound, sElementID);
  return _XT("");
}















CFormatWorkerWrapper_Stone* CPictureDownload_CB::GetExecuteActionLocal_GetPicture()
{
  if (!m_pRoot || !m_pRoot->GetActualTemplateWrapper())
    return 0;
  CFormatWorkerWrapper_Template* pTemplate = m_pRoot->GetActualTemplateWrapper();
  return pTemplate->GetPS_Child_ExecuteActionLocalWrapper(PROP_ACTIONIDTEXT_GETPICTURE);
}
CFormatWorkerWrapper_Stone* CPictureDownload_CB::GetExecuteActionRemote_GetPicture()
{
  if (!m_pRoot || !m_pRoot->GetActualTemplateWrapper())
    return 0;
  CFormatWorkerWrapper_Template* pTemplate = m_pRoot->GetActualTemplateWrapper();
  return pTemplate->GetPS_Child_ExecuteActionRemoteWrapper(PROP_ACTIONIDTEXT_GETPICTURE);
}
CFormatWorkerWrapper_Stone* CPictureDownload_CB::GetExecuteActionLocal_GetPictureInfo()
{
  if (!m_pRoot || !m_pRoot->GetActualTemplateWrapper())
    return 0;
  CFormatWorkerWrapper_Template* pTemplate = m_pRoot->GetActualTemplateWrapper();
  return pTemplate->GetPS_Child_ExecuteActionLocalWrapper(PROP_ACTIONIDTEXT_GETPICTUREINFO);
}
CFormatWorkerWrapper_Stone* CPictureDownload_CB::GetExecuteActionRemote_GetPictureInfo()
{
  if (!m_pRoot || !m_pRoot->GetActualTemplateWrapper())
    return 0;
  CFormatWorkerWrapper_Template* pTemplate = m_pRoot->GetActualTemplateWrapper();
  return pTemplate->GetPS_Child_ExecuteActionRemoteWrapper(PROP_ACTIONIDTEXT_GETPICTUREINFO);
}
CFormatWorkerWrapper_Stone* CPictureDownload_CB::GetPictereStone()
{
  if (!m_pRoot || !m_pRoot->GetActualTemplateWrapper())
    return 0;
  CFormatWorkerWrapper_Template* pTemplate = m_pRoot->GetActualTemplateWrapper();
  return pTemplate->GetPS_Child_PictureWrapper();
}
