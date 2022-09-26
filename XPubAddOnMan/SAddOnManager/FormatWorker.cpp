#if defined(WIN32)
#include "Shlwapi.h"
#endif

#include <assert.h>
#include "SSTLdef/STLdef.h"
#include "STools/Utils.h"
#include "SModelInd/ModelDef.h"
#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"

#include "XPubAddOnDeclarations.h"
#include "XPubAddOnManager.h"
#include "TagBuilder.h"

#include "FormatWorker.h"

#include "SExpression/Expression.h"






////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CFormatAddOn
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CFormatAddOn::CFormatAddOn(CXPubAddOnManager* pAddOnManager)
{
  m_pAddOnManager = pAddOnManager;
}
CFormatAddOn::~CFormatAddOn()
{
  DestroyAndRemoveAllFormatWorkers();
}
bool CFormatAddOn::AddFormatWorker(CFormatWorker* pWorker)
{
  if (!pWorker)
  {
    throw(ERR_ADDON_ADDWORKER_WITH_NULL_POINTER);
    return false;
  }

  size_t nCount = m_cWorkers.size();
  m_cWorkers.push_back(pWorker);
  if (nCount == m_cWorkers.size())
    throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);

  return (nCount != m_cWorkers.size());
}

bool CFormatAddOn::RemoveFormatWorker(CFormatWorker* pWorker)
{
  CFormatWorkersIterator it = m_cWorkers.begin();
  while (it != m_cWorkers.end())
  {
    if ((*it) == pWorker)
    {
      (*it)->DetachFromList(m_cWorkers.size());
      // Wir muessen es zuerst aus Liste entfernen
      // Es kann sein, dass mehrere Worker da eingefuegt sind
      // und nach delete waere 'it' ungueltig.
      m_cWorkers.erase(it);
      delete pWorker;
      return true;
    }
    it++;
  }
  throw(ERR_REMOVE_ELEMENT_BUT_NOT_PRESENT_IN_ARRAY);
  return false;
}

bool CFormatAddOn::DestroyAndRemoveAllFormatWorkers()
{
  while (m_cWorkers.size())
  {
    CFormatWorkersIterator it = m_cWorkers.begin();
    if (it != m_cWorkers.end())
    {
      (*it)->DetachFromList(m_cWorkers.size());
      m_pAddOnManager->ReleaseFormatWorker(*it);
      //delete (*it);
      //m_cWorkers.erase(m_cWorkers.begin());
    }
  }
  return true;
}





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CFormatWorker
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CFormatWorker::CFormatWorker(TWorkPaperType nWorkPaperType)
{
  m_nWorkPaperFormat = tDataFormatST_Default;
  m_nWorkPaperType = nWorkPaperType;
  m_nTemplateTarget = tTemplateTarget_Default;
  m_nSaveTemplateInFormat = tSaveTemplateInFormat_Default;
  m_nWorkPaperCopyType = tCopyType_Default;
  m_sAddOnValue.clear();
  m_sTokenToReplace.clear();
  m_sTagProperty.clear();
  m_sWorkPaperName.clear();
  m_sWorkPaperComment.clear();

  m_sFileShowName.clear();
  m_sFolder.clear();
  m_sFile.clear();
  m_sERModelName.clear();
  m_sSymbolicDBName.clear();
  m_sDBName.clear();
  m_sProvider.clear();

  m_pChannelCallBack = NULL;
  m_pExecModuleCallBack = NULL;
  m_pConvertWorkerProgress = NULL;

  m_pVisibilityPreprocess = 0;

  ResetInternalVariables();
}

CFormatWorker::~CFormatWorker()
{
  RemoveAllFormatWorkers();
  RemoveAllConstants();
  RemoveAllKeys();
  RemoveAllTexts();
  RemoveAllAreas();
}

bool CFormatWorker::CreatePath(const xtstring& sPath,
                               bool bLastElementToo)
{
  return CUtils::CreatePath(sPath, bLastElementToo);
}

void CFormatWorker::SetChannelToExecutionModule(CChannelToExecutedEntity* pChannelCallBack,
                                                CChanelToExecutionModule* pExecModuleCallBack)
{
  m_pChannelCallBack = pChannelCallBack;
  m_pExecModuleCallBack = pExecModuleCallBack;
}

void CFormatWorker::SetConvertWorkerProgress(CConvertWorkerProgress* pConvertWorkerProgress)
{
  m_pConvertWorkerProgress = pConvertWorkerProgress;
}

bool CFormatWorker::BasicClone(CFormatWorker* pNewFormatWorker)
{
  pNewFormatWorker->m_nWorkPaperFormat = m_nWorkPaperFormat;
  pNewFormatWorker->m_nSaveTemplateInFormat = m_nSaveTemplateInFormat;
  pNewFormatWorker->m_nWorkPaperType = m_nWorkPaperType;
  pNewFormatWorker->m_nWorkPaperCopyType = m_nWorkPaperCopyType;
  pNewFormatWorker->m_sAddOnValue = m_sAddOnValue;
  pNewFormatWorker->m_sTokenToReplace = m_sTokenToReplace;
  pNewFormatWorker->m_sTagProperty = m_sTagProperty;
  pNewFormatWorker->m_sWorkPaperName = m_sWorkPaperName;
  pNewFormatWorker->m_sWorkPaperComment = m_sWorkPaperComment;
  // PreSet variables
  pNewFormatWorker->m_sFolder = m_sFolder;
  pNewFormatWorker->m_sFile = m_sFile;
  pNewFormatWorker->m_sERModelName = m_sERModelName;
  pNewFormatWorker->m_sSymbolicDBName = m_sSymbolicDBName;

//MAK: weiß nicht ob das bleiben soll
  // brauche es bei Serienbrief in FormatWorkerRTF
  pNewFormatWorker->m_sDBName = m_sDBName;
  pNewFormatWorker->m_sProvider = m_sProvider;

  // format worker list
  for (size_t nI = 0; nI < CountOfFormatWorkers(); nI++)
  {
    CFormatWorker* pFW = GetFormatWorker(nI);
    if (!pFW)
      continue;
    // Wir muessen nicht CreateFormatWorkerClone benutzen.
    // Das ist in AddFormatWorker gemacht.
    //CFormatWorker* pNewFW = pFW->GetAddOnManager()->CreateFormatWorkerClone(pFW);
    //if (pNewFW)
    //  pNewFormatWorker->AddFormatWorker(pNewFW);
    pNewFormatWorker->AddFormatWorker(pFW);
  }

  for (size_t nI = 0; nI < CountOfConstants(); nI++)
  {
    CMPModelConstantInd_ForFrm* p = GetConstant(nI);
    if (!p)
      continue;
    pNewFormatWorker->AddConstant(p);
  }
  for (size_t nI = 0; nI < CountOfKeys(); nI++)
  {
    CMPModelKeyInd_ForFrm* p = GetKey(nI);
    if (!p)
      continue;
    pNewFormatWorker->AddKey(p);
  }
  for (size_t nI = 0; nI < CountOfTexts(); nI++)
  {
    CMPModelTextInd_ForFrm* p = GetText(nI);
    if (!p)
      continue;
    pNewFormatWorker->AddText(p);
  }
  for (size_t nI = 0; nI < CountOfAreas(); nI++)
  {
    CMPModelAreaInd_ForFrm* p = GetArea(nI);
    if (!p)
      continue;
    pNewFormatWorker->AddArea(p);
  }

  return true;
}







bool CFormatWorker::PropertyGroupPresent(TXPubProperty nPropertyGroupType)
{
  // wir suchen immer von hinten
  for (CFormatWorkersReverseIterator it = m_cFormatWorkers.rbegin(); it != m_cFormatWorkers.rend(); it++)
  {
    if (!(*it)->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = (*it)->GetPropertyBase();
    if (pProp && pProp->GetPropertyGroup() == nPropertyGroupType)
      return true;
  }
  return false;
}

bool CFormatWorker::GetPDFPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty)
{
  // wir suchen immer von hinten
  for (CFormatWorkersReverseIterator it = m_cFormatWorkers.rbegin(); it != m_cFormatWorkers.rend(); it++)
  {
    if (!(*it)->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = (*it)->GetPropertyBase();
    if (pProp && pProp->GetPropertyGroup() == tXPP_PDFProps)
      return pProp->GetPropertyValue(cValue, nProperty);
  }
  return false;
}

// die PDF-Properties aus dem Baustein für TAG-Builder bereitstellen
bool CFormatWorker::GetPDFPermissionProperty(CXPubVariant& sPwdOption, long& lPermission)
{
	GetPDFPropertyValue(sPwdOption, tXPP_PDFP_Print);
	if ((bool)sPwdOption)
		lPermission |= PDF_PERMISSION_PRINTING;

	GetPDFPropertyValue(sPwdOption, tXPP_PDFP_LowPrinting);
	if (!(bool)sPwdOption)
		lPermission |= PDF_PERMISSION_LOWPRINTING;

	GetPDFPropertyValue(sPwdOption, tXPP_PDFP_Commenting);
	if ((bool)sPwdOption)
		lPermission |= PDF_PERMISSION_COMMENTING;

	GetPDFPropertyValue(sPwdOption, tXPP_PDFP_Edit);
	if ((bool)sPwdOption)
		lPermission |= PDF_PERMISSION_EDITING;

	GetPDFPropertyValue(sPwdOption, tXPP_PDFP_Copy);
	if ((bool)sPwdOption)
		lPermission |= PDF_PERMISSION_COPYING;

	return true;
}

bool CFormatWorker::GetOutlookPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty)
{
  // wir suchen immer von hinten
  for (CFormatWorkersReverseIterator it = m_cFormatWorkers.rbegin(); it != m_cFormatWorkers.rend(); it++)
  {
    if (!(*it)->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = (*it)->GetPropertyBase();
    if (pProp && pProp->GetPropertyGroup() == tXPP_OutlookProps)
      return pProp->GetPropertyValue(cValue, nProperty); 
  }
  return false;
}

bool CFormatWorker::GetRTFPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty)
{
  // wir suchen immer von hinten
  for (CFormatWorkersReverseIterator it = m_cFormatWorkers.rbegin(); it != m_cFormatWorkers.rend(); it++)
  {
    if (!(*it)->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = (*it)->GetPropertyBase();
    if (pProp && pProp->GetPropertyGroup() == tXPP_RTFProps)
      return pProp->GetPropertyValue(cValue, nProperty); 
  }
  return false;
}

bool CFormatWorker::GetExcelPropertyValue(CXPubVariant& cValue, TXPubProperty nProperty)
{
  // wir suchen immer von hinten
  for (CFormatWorkersReverseIterator it = m_cFormatWorkers.rbegin(); it != m_cFormatWorkers.rend(); it++)
  {
    if (!(*it)->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = (*it)->GetPropertyBase();
    if (pProp && pProp->GetPropertyGroup() == tXPP_ExcelProps)
      return pProp->GetPropertyValue(cValue, nProperty);
  }
  return false;
}

bool CFormatWorker::GetFreePropertyValue(CXPubVariant& cValue, const xtstring& sPropName)
{
  // wir suchen immer von hinten
  for (CFormatWorkersReverseIterator it = m_cFormatWorkers.rbegin(); it != m_cFormatWorkers.rend(); it++)
  {
    if (!(*it)->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = (*it)->GetPropertyBase();
    if (pProp && pProp->GetPropertyGroup() == tXPP_FreeProps)
    {
      if (pProp->FreePropPresent(sPropName))
        return pProp->GetPropertyValue(cValue, sPropName);
    }
  }
  return false;
}

CPropertyBase* CFormatWorker::GetProperties(TXPubProperty nPropertyGroupType)
{
  // wir suchen immer von hinten
  for (CFormatWorkersReverseIterator it = m_cFormatWorkers.rbegin(); it != m_cFormatWorkers.rend(); it++)
  {
    if (!(*it)->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = (*it)->GetPropertyBase();
    if (pProp && pProp->GetPropertyGroup() == nPropertyGroupType)
      return pProp;
  }
  return 0;
}
CPropertyBase* CFormatWorker::GetFreeProperties(const xtstring& sPropertiesName)
{
  // wir suchen immer von hinten
  for (CFormatWorkersReverseIterator it = m_cFormatWorkers.rbegin(); it != m_cFormatWorkers.rend(); it++)
  {
    if (!(*it)->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = (*it)->GetPropertyBase();
    if (pProp && pProp->GetPropertyGroup() == tXPP_FreeProps)
      if (sPropertiesName == pProp->GetNameOfFreeProperties())
        return pProp;
  }
  return 0;
}
bool CFormatWorker::AddFormatWorker(CFormatWorker* pFormatWorker)
{
  if (!pFormatWorker)
    return false;

  CFormatWorker* pCloneFormatWorker;
  pCloneFormatWorker = pFormatWorker->GetAddOnManager()->CreateFormatWorkerClone(pFormatWorker);
  if (!pCloneFormatWorker)
    return false;

  size_t nCount = m_cFormatWorkers.size();
  m_cFormatWorkers.push_back(pCloneFormatWorker);
  if (nCount == m_cFormatWorkers.size())
  {
    pFormatWorker->GetAddOnManager()->ReleaseFormatWorker(pCloneFormatWorker);
    return false;
  }
  return true;
}
CFormatWorker* CFormatWorker::GetFormatWorker(size_t nIndex)
{
  if (nIndex >= m_cFormatWorkers.size())
    return 0;
  return m_cFormatWorkers[nIndex];
}
void CFormatWorker::RemoveAllFormatWorkers()
{
  for (size_t nI = 0; nI < m_cFormatWorkers.size(); nI++)
  {
    CFormatWorker* pFW = GetFormatWorker(nI);
    if (!pFW)
      continue;
    pFW->GetAddOnManager()->ReleaseFormatWorker(pFW);
  }
  m_cFormatWorkers.erase(m_cFormatWorkers.begin(), m_cFormatWorkers.end());
}
bool CFormatWorker::FormatWorkerPresentButNotPropertyFormatWorker()
{
  for (size_t nI = 0; nI < m_cFormatWorkers.size(); nI++)
  {
    CFormatWorker* pFW = GetFormatWorker(nI);
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      return true;
  }
  return false;
}
bool CFormatWorker::TemplateFormatWorkerPresent()
{
  for (size_t nI = 0; nI < m_cFormatWorkers.size(); nI++)
  {
    CFormatWorker* pFW = GetFormatWorker(nI);
    if (!pFW)
      continue;
    if (pFW->GetWorkPaperType() == tWorkPaperType_Template)
      return true;
  }
  return false;
}
CFormatWorker* CFormatWorker::GetFirstTemplateFormatWorker()
{
  for (size_t nI = 0; nI < m_cFormatWorkers.size(); nI++)
  {
    CFormatWorker* pFW = GetFormatWorker(nI);
    if (!pFW)
      continue;
    if (pFW->GetWorkPaperType() == tWorkPaperType_Template)
      return pFW;
  }
  return 0;
}












bool CFormatWorker::SaveWorkPaperToMemory(xtstring& sXMLMemoryContent)
{
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(EMPTY_WORKPAPERCONTENT_XML))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_TAG_WORKPAPERCONTENT)
  {
    if (cXMLDoc.AddChildElem(WORKPAPER_1STAGE, _XT("")))
    {
      // into 1. stage
      cXMLDoc.IntoElem();

      // pre set values
      cXMLDoc.SetAttrib(WORKPAPER_FORMAT, this->GetWorkPaperDataFormat());
      cXMLDoc.SetAttrib(WORKPAPER_TYPE, this->GetWorkPaperType());

      if (!this->WriteToXMLMemoryContent(&cXMLDoc))
        return false;

      // out of 1. stage
      cXMLDoc.OutOfElem();
    }
    else
      return false;
  }
  else
    return false;
  sXMLMemoryContent = cXMLDoc.GetDoc();
  return true;
}

bool CFormatWorker::SaveWorkPaperToMemory(CXPubMarkUp* pXMLDoc)
{
  if (pXMLDoc->AddChildElem(WORKPAPER_1STAGE, _XT("")))
  {
    // into 1. stage
    pXMLDoc->IntoElem();

    // pre set values
    pXMLDoc->SetAttrib(WORKPAPER_FORMAT, this->GetWorkPaperDataFormat());
    pXMLDoc->SetAttrib(WORKPAPER_TYPE, this->GetWorkPaperType());

    if (!this->WriteToXMLMemoryContent(pXMLDoc))
      return false;

    // out of 1. stage
    pXMLDoc->OutOfElem();
  }
  else
    return false;
  return true;
}

void CFormatWorker::AddRelatedFilesToGeneratedFilesList(CGeneratedFiles& cGeneratedFiles)
{
  for (size_t nI = 0; nI < CountOfFormatWorkers(); nI++)
  {
    CFormatWorker* pFW = GetFormatWorker(nI);
    if (!pFW)
      continue;
    if (!pFW->IsPropertyFormatWorker())
      continue;
    CPropertyBase* pProp = pFW->GetPropertyBase();
    if (!pProp)
      continue;
    if (pProp->GetPropertyGroup() != tXPP_FileCopyProps)
      continue;

    xtstring sFileFrom;
    xtstring sFileTo;
    CXPubVariant cValue;
    if (!pProp->GetPropertyValue(cValue, tXPP_FCOP_FileFrom))
      continue;
    sFileFrom = (xtstring)cValue;
    if (!pProp->GetPropertyValue(cValue, tXPP_FCOP_FileTo))
      continue;
    sFileTo = (xtstring)cValue;

    CGeneratedFile cGeneratedFile;
    cGeneratedFile.m_sFullFileName = sFileFrom;
    cGeneratedFile.m_sFileShowName = _XT("");
    cGeneratedFile.m_nFileFormat = tSaveTemplateInFormat_Native;
    cGeneratedFile.m_nFileType = tExecGeneratedFile_RelatedFile;
    cGeneratedFile.m_sTag = sFileTo;
	cGeneratedFile.m_bFileGeneratedOverMemory = false;
    cGeneratedFiles.push_back(cGeneratedFile);
  }
}




bool CFormatWorker::ReadFromXMLMemoryContent(CFormatWorkerWrapper* pWrapper,
                                             CXPubMarkUp* pXMLDoc)
{
  if (!StartReadFromXMLMemoryContent(pWrapper, pXMLDoc))
    return false;

  // da erledigen wir FormatWorker spezifische Dinge
  if (!pXMLDoc->FindChildElem(WORKPAPER_DATA))
    return false;
  pXMLDoc->IntoElem();
  SetWorkPaperContent(pXMLDoc->GetData());
  pXMLDoc->OutOfElem();

  if (!EndReadFromXMLMemoryContent(pWrapper, pXMLDoc))
    return false;
  return true;
}
bool CFormatWorker::StartReadFromXMLMemoryContent(CFormatWorkerWrapper* pWrapper,
                                                  CXPubMarkUp* pXMLDoc)
{
  if (!pXMLDoc->FindChildElem(WORKPAPER_2STAGE))
    return false;

  // into 2. stage
  pXMLDoc->IntoElem();

  // data
  m_nWorkPaperFormat = (TDataFormatForStoneAndTemplate)pXMLDoc->GetAttribLong(WORKPAPER_FORMAT);
  m_nWorkPaperType = (TWorkPaperType)pXMLDoc->GetAttribLong(WORKPAPER_TYPE);
  m_nTemplateTarget = (TTemplateTarget)pXMLDoc->GetAttribLong(WORKPAPER_TEMPLATETARGET);
  m_nSaveTemplateInFormat = (TSaveTemplateInFormat)pXMLDoc->GetAttribLong(WORKPAPER_SAVETEMPLATEINFORMAT);
  m_nWorkPaperCopyType = (TWorkPaperCopyType)pXMLDoc->GetAttribLong(WORKPAPER_COPYTYPE);
  m_sAddOnValue = pXMLDoc->GetAttrib(WORKPAPER_ADDONVALUE);
  m_sTokenToReplace = pXMLDoc->GetAttrib(WORKPAPER_TOKENTOREPLACE);
  m_sTagProperty = pXMLDoc->GetAttrib(WORKPAPER_TAGPROPERTY);
  m_sWorkPaperName = pXMLDoc->GetAttrib(WORKPAPER_NAME);
  m_sWorkPaperComment = pXMLDoc->GetAttrib(WORKPAPER_COMMENT);
  m_sFileShowName = pXMLDoc->GetAttrib(WORKPAPER_FILESHOWNAME);
  m_sFolder = pXMLDoc->GetAttrib(WORKPAPER_FOLDER);
  m_sFile = pXMLDoc->GetAttrib(WORKPAPER_FILE);
  m_sERModelName = pXMLDoc->GetAttrib(WORKPAPER_ERMODELNAME);
  m_sSymbolicDBName = pXMLDoc->GetAttrib(WORKPAPER_SYMBOLICDBNAME);
  m_sDBName = pXMLDoc->GetAttrib(WORKPAPER_DBNAME);
  m_sProvider = pXMLDoc->GetAttrib(WORKPAPER_PROVIDER);

  return true;
}
bool CFormatWorker::EndReadFromXMLMemoryContent(CFormatWorkerWrapper* pWrapper,
                                                CXPubMarkUp* pXMLDoc)
{
  if (pXMLDoc->FindChildElem(CONSTANTS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Constants>
    while (pXMLDoc->FindChildElem(CONSTANT_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Constant>
      CMPModelConstantInd_ForFrm cModel;
      cModel.ReadXMLContent(pXMLDoc);
// Bei Constant sollten wir es nicht tun
/*
      if (pWrapper)
      {
        CMPModelConstantInd_ForFrm* pNew = new CMPModelConstantInd_ForFrm(cModel);
        if (pNew)
        {
          CFormatWorkerWrapper_Field* pNewWrapper = new CFormatWorkerWrapper_Field(pWrapper->GetRoot());
          if (!pNewWrapper)
            delete pNew;
          else
          {
            pNewWrapper->SetConstant(pNew);
            if (!pWrapper->AddWrapper(pNewWrapper))
            {
              pNewWrapper->SetConstant(0);
              delete pNewWrapper;
              delete pNew;
            }
          }
        }
      }
      else
*/
      AddConstant(&cModel);
      pXMLDoc->OutOfElem();
      // in <Constants>
    }
    pXMLDoc->OutOfElem();
  }
  if (pXMLDoc->FindChildElem(KEYS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Keys>
    while (pXMLDoc->FindChildElem(KEY_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Key>
      CMPModelKeyInd_ForFrm cModel;
      cModel.ReadXMLContent(pXMLDoc);
      if (pWrapper)
      {
        CMPModelKeyInd_ForFrm* pNew = new CMPModelKeyInd_ForFrm(cModel);
        if (pNew)
        {
          CFormatWorkerWrapper_Field* pNewWrapper = new CFormatWorkerWrapper_Field(pWrapper->GetRoot(), pWrapper);
          if (!pNewWrapper)
            delete pNew;
          else
          {
            pNewWrapper->SetKey(pNew);
            if (!pWrapper->AddWrapper(pNewWrapper))
            {
              pNewWrapper->SetKey(0);
              delete pNewWrapper;
              delete pNew;
            }
          }
        }
      }
      else
        AddKey(&cModel);
      pXMLDoc->OutOfElem();
      // in <Keys>
    }
    pXMLDoc->OutOfElem();
  }
  if (pXMLDoc->FindChildElem(TEXTS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Texts>
    while (pXMLDoc->FindChildElem(TEXT_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Text>
      CMPModelTextInd_ForFrm cModel;
      cModel.ReadXMLContent(pXMLDoc);
      if (pWrapper)
      {
        CMPModelTextInd_ForFrm* pNew = new CMPModelTextInd_ForFrm(cModel);
        if (pNew)
        {
          CFormatWorkerWrapper_Field* pNewWrapper = new CFormatWorkerWrapper_Field(pWrapper->GetRoot(), pWrapper);
          if (!pNewWrapper)
            delete pNew;
          else
          {
            pNewWrapper->SetText(pNew);
            if (!pWrapper->AddWrapper(pNewWrapper))
            {
              pNewWrapper->SetText(0);
              delete pNewWrapper;
              delete pNew;
            }
          }
        }
      }
      else
        AddText(&cModel);
      pXMLDoc->OutOfElem();
      // in <Texts>
    }
    pXMLDoc->OutOfElem();
  }
  if (pXMLDoc->FindChildElem(AREAS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Areas>
    while (pXMLDoc->FindChildElem(AREA_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Area>
      CMPModelAreaInd_ForFrm cModel;
      cModel.ReadXMLContent(pXMLDoc);
      if (pWrapper)
      {
        CMPModelAreaInd_ForFrm* pNew = new CMPModelAreaInd_ForFrm(cModel);
        if (pNew)
        {
          CFormatWorkerWrapper_Field* pNewWrapper = new CFormatWorkerWrapper_Field(pWrapper->GetRoot(), pWrapper);
          if (!pNewWrapper)
            delete pNew;
          else
          {
            pNewWrapper->SetArea(pNew);
            if (!pWrapper->AddWrapper(pNewWrapper))
            {
              pNewWrapper->SetArea(0);
              delete pNewWrapper;
              delete pNew;
            }
          }
        }
      }
      else
        AddArea(&cModel);
      pXMLDoc->OutOfElem();
      // in <Areas>
    }
    pXMLDoc->OutOfElem();
  }
  while (pXMLDoc->FindChildElem(WORKPAPER_1STAGE))
  {
    // into 1. stage
    pXMLDoc->IntoElem();

    // pre set values
    TDataFormatForStoneAndTemplate nWorkPaperFormat = (TDataFormatForStoneAndTemplate)pXMLDoc->GetAttribLong(WORKPAPER_FORMAT);
    TWorkPaperType nWorkPaperType = (TWorkPaperType)pXMLDoc->GetAttribLong(WORKPAPER_TYPE);


    CFormatWorker* pFmt = 0;
    pFmt = GetAddOnManager()->CreateFormatWorker(nWorkPaperFormat, nWorkPaperType);
    if (!pFmt)
      return false;

    CFormatWorkerWrapper* pNewWrapper = 0;
    if (pWrapper)
    {
      if (nWorkPaperType == tWorkPaperType_Stone)
        pNewWrapper = new CFormatWorkerWrapper_Stone(pWrapper->GetRoot(), pWrapper);
      else
        pNewWrapper = new CFormatWorkerWrapper_Template(pWrapper->GetRoot(), pWrapper);
      if (!pNewWrapper)
      {
        GetAddOnManager()->ReleaseFormatWorker(pFmt);
        return false;
      }
    }

    if (!pFmt->ReadFromXMLMemoryContent(pNewWrapper, pXMLDoc))
    {
      GetAddOnManager()->ReleaseFormatWorker(pFmt);
      if (pNewWrapper)
      {
        pNewWrapper->SetFormatWorker(0);
        delete pNewWrapper;
      }
      return false;
    }

    if (pNewWrapper)
    {
      pNewWrapper->SetFormatWorker(pFmt);
      if (!pWrapper->AddWrapper(pNewWrapper))
      {
        GetAddOnManager()->ReleaseFormatWorker(pFmt);
        pNewWrapper->SetFormatWorker(0);
        delete pNewWrapper;
        return false;
      }
    }
    else
    {
      if (!AddFormatWorker(pFmt))
      {
        GetAddOnManager()->ReleaseFormatWorker(pFmt);
        if (pNewWrapper)
        {
          pNewWrapper->SetFormatWorker(0);
          delete pNewWrapper;
        }
        return false;
      }
      GetAddOnManager()->ReleaseFormatWorker(pFmt);
    }

    // out of 1. stage
    pXMLDoc->OutOfElem();
  }

  // out of 2. stage
  pXMLDoc->OutOfElem();
  return true;
}




bool CFormatWorker::WriteToXMLMemoryContent(CXPubMarkUp* pXMLDoc)
{
  if (!StartWriteToXMLMemoryContent(pXMLDoc))
    return false;

  // da erledigen wir FormatWorker spezifische Dinge
  if (!pXMLDoc->AddChildElem(WORKPAPER_DATA, GetWorkPaperContent().c_str()))
    return false;

  if (!EndWriteToXMLMemoryContent(pXMLDoc))
    return false;
  return true;
}
bool CFormatWorker::StartWriteToXMLMemoryContent(CXPubMarkUp* pXMLDoc)
{
  if (!pXMLDoc->AddChildElem(WORKPAPER_2STAGE, _XT("")))
    return false;

  // into 2. stage
  pXMLDoc->IntoElem();

  // data
  pXMLDoc->SetAttrib(WORKPAPER_FORMAT, m_nWorkPaperFormat);
  pXMLDoc->SetAttrib(WORKPAPER_TYPE, m_nWorkPaperType);
  pXMLDoc->SetAttrib(WORKPAPER_TEMPLATETARGET, m_nTemplateTarget);
  pXMLDoc->SetAttrib(WORKPAPER_SAVETEMPLATEINFORMAT, m_nSaveTemplateInFormat);
  pXMLDoc->SetAttrib(WORKPAPER_COPYTYPE, m_nWorkPaperCopyType);
  pXMLDoc->SetAttrib(WORKPAPER_ADDONVALUE, m_sAddOnValue.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_TOKENTOREPLACE, m_sTokenToReplace.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_TAGPROPERTY, m_sTagProperty.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_NAME, m_sWorkPaperName.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_COMMENT, m_sWorkPaperComment.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_FILESHOWNAME, m_sFileShowName.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_FOLDER, m_sFolder.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_FILE, m_sFile.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_ERMODELNAME, m_sERModelName.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_SYMBOLICDBNAME, m_sSymbolicDBName.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_DBNAME, m_sDBName.c_str());
  pXMLDoc->SetAttrib(WORKPAPER_PROVIDER, m_sProvider.c_str());

  return true;
}
bool CFormatWorker::EndWriteToXMLMemoryContent(CXPubMarkUp* pXMLDoc)
{
  if (pXMLDoc->AddChildElem(CONSTANTS_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    for (size_t nI = 0; nI < CountOfConstants(); nI++)
      GetConstant(nI)->SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  if (pXMLDoc->AddChildElem(KEYS_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    for (size_t nI = 0; nI < CountOfKeys(); nI++)
      GetKey(nI)->SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  if (pXMLDoc->AddChildElem(TEXTS_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    for (size_t nI = 0; nI < CountOfTexts(); nI++)
      GetText(nI)->SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  if (pXMLDoc->AddChildElem(AREAS_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    for (size_t nI = 0; nI < CountOfAreas(); nI++)
      GetArea(nI)->SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  for (size_t nI = 0; nI < CountOfFormatWorkers(); nI++)
  {
    CFormatWorker* pFW = GetFormatWorker(nI);
    if (!pFW)
      continue;
    if (pXMLDoc->AddChildElem(WORKPAPER_1STAGE, _XT("")))
    {
      // into 1. stage
      pXMLDoc->IntoElem();

      // wir muessen zuerst Format und Type ablegen
      pXMLDoc->SetAttrib(WORKPAPER_FORMAT, pFW->GetWorkPaperDataFormat());
      pXMLDoc->SetAttrib(WORKPAPER_TYPE, pFW->GetWorkPaperType());

      if (!pFW->WriteToXMLMemoryContent(pXMLDoc))
        return false;

      // out of 1. stage
      pXMLDoc->OutOfElem();
    }
    else
      return false;
  }

  // out of 2. stage
  pXMLDoc->OutOfElem();
  return true;
}














size_t CFormatWorker::CountOfConstants()
{
  return m_cConstants.size();
}
void CFormatWorker::GetAllConstants(CxtstringVector& arrConstants)
{
  for (size_t nI = 0; nI < CountOfConstants(); nI++)
    arrConstants.push_back(m_cConstants[nI]->GetName());
}
CMPModelConstantInd_ForFrm* CFormatWorker::GetConstant(size_t nIndex)
{
  assert(nIndex < m_cConstants.size());
  if (nIndex < m_cConstants.size())
    return m_cConstants[nIndex];
  return NULL;
}
void CFormatWorker::AddConstant(CMPModelConstantInd_ForFrm* pModel)
{
  if (!pModel)
    return;
  CMPModelConstantInd_ForFrm* ppModel;
  ppModel = new CMPModelConstantInd_ForFrm(pModel);
  if (!ppModel)
    return;
  m_cConstants.push_back(ppModel);
}
bool CFormatWorker::RemoveAllConstants()
{
  for (CMPModelConstantInds_ForFrmIterator it = m_cConstants.begin(); it != m_cConstants.end(); it++)
    delete *it;
  m_cConstants.erase(m_cConstants.begin(), m_cConstants.end());
  return true;
}
size_t CFormatWorker::CountOfKeys()
{
  return m_cKeys.size();
}
CMPModelKeyInd_ForFrm* CFormatWorker::GetKey(size_t nIndex)
{
  assert(nIndex < m_cKeys.size());
  if (nIndex < m_cKeys.size())
    return m_cKeys[nIndex];
  return NULL;
}
void CFormatWorker::AddKey(CMPModelKeyInd_ForFrm* pModel)
{
  if (!pModel)
    return;
  CMPModelKeyInd_ForFrm* ppModel;
  ppModel = new CMPModelKeyInd_ForFrm(pModel);
  if (!ppModel)
    return;
  m_cKeys.push_back(ppModel);
}
bool CFormatWorker::RemoveAllKeys()
{
  for (CMPModelKeyInds_ForFrmIterator it = m_cKeys.begin(); it != m_cKeys.end(); it++)
    delete *it;
  m_cKeys.erase(m_cKeys.begin(), m_cKeys.end());
  return true;
}
size_t CFormatWorker::CountOfTexts()
{
  return m_cTexts.size();
}
CMPModelTextInd_ForFrm* CFormatWorker::GetText(size_t nIndex)
{
  assert(nIndex < m_cTexts.size());
  if (nIndex < m_cTexts.size())
    return m_cTexts[nIndex];
  return NULL;
}
void CFormatWorker::AddText(CMPModelTextInd_ForFrm* pModel)
{
  if (!pModel)
    return;
  CMPModelTextInd_ForFrm* ppModel;
  ppModel = new CMPModelTextInd_ForFrm(pModel);
  if (!ppModel)
    return;
  m_cTexts.push_back(ppModel);
}
bool CFormatWorker::RemoveAllTexts()
{
  for (CMPModelTextInds_ForFrmIterator it = m_cTexts.begin(); it != m_cTexts.end(); it++)
    delete *it;
  m_cTexts.erase(m_cTexts.begin(), m_cTexts.end());
  return true;
}
size_t CFormatWorker::CountOfAreas()
{
  return m_cAreas.size();
}
CMPModelAreaInd_ForFrm* CFormatWorker::GetArea(size_t nIndex)
{
  assert(nIndex < m_cAreas.size());
  if (nIndex < m_cAreas.size())
    return m_cAreas[nIndex];
  return NULL;
}
void CFormatWorker::AddArea(CMPModelAreaInd_ForFrm* pModel)
{
  if (!pModel)
    return;
  CMPModelAreaInd_ForFrm* ppModel;
  ppModel = new CMPModelAreaInd_ForFrm(pModel);
  if (!ppModel)
    return;
  m_cAreas.push_back(ppModel);
}
bool CFormatWorker::RemoveAllAreas()
{
  for (CMPModelAreaInds_ForFrmIterator it = m_cAreas.begin(); it != m_cAreas.end(); it++)
    delete *it;
  m_cAreas.erase(m_cAreas.begin(), m_cAreas.end());
  return true;
}

















bool CFormatWorker::GetValue(const xtstring& sName,
                             CXPubVariant& cValue)
{

	if (sName == FWVAR_DOCUMENTBEGINPAGENBR)
  {
    cValue = (long)GetDocumentBeginPageNumber();
    return true;
  }

  if (sName == FWVAR_TREESTARTACTIONTYP)
  {
    cValue = (long)GetTreeStartActionTyp();
    return true;
  }
  if (sName == FWVAR_TREESTARTELEMENTTYP)
  {
    cValue = GetTreeStartElementtyp();
    return true;
  }
  if (sName == FWVAR_TEXTFOCUSACTIVE)
  {
    cValue = GetTextFocusActive();
    return true;
  }
  if (!m_pVisibilityPreprocess)
    return false;
  if (!m_pVisibilityPreprocess->GetValueValue(sName))
    return false;
  cValue = *m_pVisibilityPreprocess->GetValueValue(sName);
  return true;
}
bool CFormatWorker::GetValue_CB(const xtstring& sEntityName,
                                const xtstring& sFieldName,
                                CXPubVariant& cValue)
{
  return GetValue(sFieldName, cValue);
}
bool CFormatWorker::ExecuteVisibility(CMPModelKeyInd_ForFrm* pField,
                                      CXPubVariant& cResult)
{
  if (!pField)
    return false;

  bool bRet = false;
  m_pVisibilityPreprocess = pField->GetVisibilityPreprocess();
  bRet = Execute(true, cResult, pField->GetVisibility());
  m_pVisibilityPreprocess = 0;
  return bRet;
}
bool CFormatWorker::ExecuteVisibility(CMPModelTextInd_ForFrm* pField,
                                      CXPubVariant& cResult)
{
  if (!pField)
    return false;

  bool bRet = false;
  m_pVisibilityPreprocess = pField->GetVisibilityPreprocess();
  bRet = Execute(true, cResult, pField->GetVisibility());
  m_pVisibilityPreprocess = 0;
  return bRet;
}
bool CFormatWorker::ExecuteVisibility(CMPModelAreaInd_ForFrm* pField,
                                      CXPubVariant& cResult)
{
  if (!pField)
    return false;

  bool bRet = false;
  m_pVisibilityPreprocess = pField->GetVisibilityPreprocess();
  bRet = Execute(true, cResult, pField->GetVisibility());
  m_pVisibilityPreprocess = 0;
  return bRet;
}
bool CFormatWorker::Execute(bool bCondition,
                            CXPubVariant& cResult,
                            const CConditionOrAssignment& cCondition)
{
  if (cCondition.GetExprText().empty())
  {
    cResult = true;
    return true;
  }

//  SetMSGItemsInVariableValueCallBack(&nOutItem, &nLogItem);
  CExpression cExp(/*m_pDataPool->m_pPythonState*/0, this);
  if (cExp.Set(cCondition.GetExprText(), cCondition.GetExprSymbols(), cCondition.GetExprValues()))
  {
/*
    CXPubVariant cValue;
    // Werte, die über Datenbank gesetzt werden müssen
    map<xtstring, CProductionVariable>::iterator First = cExp.m_lstVariable.begin(); 
    for (; First != cExp.m_lstVariable.end(); ++First)
    {
      //xtstring x = (*First).first;
      CProductionVariable& cProVar = (*First).second;
      if (GetValue(cProVar.m_cVarName, cValue))
      {
        // Wert setzen
        cProVar.SetFieldValue(cValue);
      }
      else
      {
        // error
        return false;
      }
    }
*/
    try
    {
      if (cExp.Evaluate(bCondition, cResult) == 0)
        return true;
      else
        return false;
    }
    catch (int nError)
    {
      // error
      xtstring sMsgKeyword;
      xtstring sMsgText;
      xtstring sMsgDescription;
      xtstring sDescInfo;
      switch (nError)
      {
      case ERR_EXPR_NOERROR_SETRESULT_FALSE:
        cResult = false;
        return true;
      case ERR_EXPR_NOERROR_SETRESULT_TRUE:
        cResult = true;
        return true;
      case ERR_EXPR_NOERROR_SETRESULT_EMPTYSTRING:
        cResult = xtstring(_XT(""));
        return true;
      case ERR_EXPR_NOERROR_SETRESULT_LONG0:
        cResult = (long)0;
        return true;
      case ERR_EXPR_ERROR_WITHOUT_MESSAGE:
        // Es ist zu Fehler gekommen, aber wir sollten keine weiteres Text darstellen
        return false;
      case ERR_EXPR_DIVIDEBYZERO:
        sMsgKeyword = _XT("Auswertung");
				sMsgText = _XT("Unerlaubte Operation: Teilung durch 0 aufgetreten.");
        break;
			case ERR_EXPR_MALFORMEDXML:
				sMsgKeyword = _XT("Auswertung");
				sMsgText = _XT("Fataler Fehler: Fehlerhafte XML Datei.");
				break;
			case ERR_EXPR_VARIABLE_CONTENT_NOTFIELD:
				sMsgKeyword = _XT("Auswertung");
				sMsgText = _XT("Fataler Fehler: Der Inhalt der Variablen ist kein Feld in der Ausführung.");
				break;		
			case ERR_EXPR_VARIABLEUNASSIGNED:
				sMsgKeyword = _XT("Auswertung");
				sMsgText = _XT("Fataler Fehler: Variable nicht definiert, zuordenbar.");
				break;
      case ERR_EXPR_SCRIPTING_ARGUMENT_FAILED:
        sMsgKeyword = _XT("Auswertung");
        sMsgText = _XT("Fataler Fehler: Argumente im Skriptmodul falsch übergeben.");
        break;
      case ERR_EXPR_SCRIPTING_FUNCTIONCALL_FAILED:
        sMsgKeyword = _XT("Auswertung");
        sMsgText = _XT("Fataler Fehler: Fehler bei Ausführung einer Skriptfunktion aufgetreten.");
        cExp.GetErrorInfo(sDescInfo);
        sMsgDescription = sDescInfo;
        break;
      case ERR_EXPR_SCRIPTING_CANNOTFIND_FUNCTION:
        sMsgKeyword = _XT("Auswertung");
        sMsgText = _XT("ERR_EXPR_SCRIPTING_CANNOTFIND_FUNCTION");
        cExp.GetErrorInfo(sDescInfo);
        sMsgDescription = sDescInfo;
        break;
      case ERR_EXPR_SCRIPTING_CANNOTFIND_MODULE:
        sMsgKeyword = _XT("Auswertung");
        sMsgText = _XT("ERR_EXPR_SCRIPTING_CANNOTFIND_MODULE");
        cExp.GetErrorInfo(sDescInfo);
        sMsgDescription = sDescInfo;
        break;
      case ERR_EXPR_SCRIPTING_MODULE_NOTFOUND:
        sMsgKeyword = _XT("Auswertung");
        sMsgText = _XT("Fataler Fehler: Fehler bei Ausführung einer Skriptfunktion aufgetreten.");
        cExp.GetErrorInfo(sDescInfo);
        sMsgDescription = sDescInfo;
        break;
      default:
        sMsgKeyword = _XT("Auswertung");
        sMsgText = _XT("Unerwarteter Fehler.");
        sMsgDescription.Format(_XT("Expression: %s\nSymbols: %s\nValues: %s"), cCondition.GetExprText().c_str(), cCondition.GetExprSymbols().c_str(), cCondition.GetExprValues().c_str());
        break;
      }
      return false;
    }
    catch (...)
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  return true;
}
bool CFormatWorker::InsertText_Front(const xtstring& sText)
{
  throw("Funktion InsertText_Front nicht implementiert!");
  return false;
}
bool CFormatWorker::InsertText_Tail(const xtstring& sText)
{
  throw("Funktion InsertText_Tail nicht implementiert!");
  return false;
}
