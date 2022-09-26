// MPModelTemplateInd.cpp: implementation of the CMPModelTemplateInd classes.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <time.h>
#include <string.h>

#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"

#include "ModelDef.h"
#include "ModelInd.h"
#include "MPModelFieldsInd.h"
#include "MPModelTemplateInd.h"






//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CMPModelTemplateInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CMPModelTemplateInd::CMPModelTemplateInd(CSyncWPNotifiersGroup* pGroup)
                    :CModelBaseInd(tMPModel),
                     CSyncNotifier_WP(tWPNotifier_TemplateMainProperty, pGroup),
                     CSyncResponse_Proj(tRO_MainProperty_Template),
                     CSyncResponse_ER(tRO_MainProperty_Template),
                     CSyncResponse_IFS(tRO_MainProperty_Template),
                     CSyncResponse_WP(tRO_MainProperty_Template),
                     m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                     m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true),
                     m_caTemplateFileShowName(CConditionOrAssignment::tCOAT_TemplateFileShowName_Assignment, true),
                     m_caTemplateFile(CConditionOrAssignment::tCOAT_TemplateFile_Assignment, true),
                     m_caTemplateFolder(CConditionOrAssignment::tCOAT_TemplateFolder_Assignment, true),
                     m_caTemplateDBName(CConditionOrAssignment::tCOAT_TemplateDBName_Assignment, true)
{
  m_sName.erase();
  m_sComment.erase();
  m_sAutor.erase();

  m_caVisibility.Reset();
  m_sInformationSight.erase();
  m_bUseInformationSight = true;
  m_sIFSTargetEntity.erase();
  m_bUseIFSTargetEntity = true;
  m_sStoneOrTemplate.erase();
  m_bUseStoneOrTemplate = true;
  m_nWorkPaperCopyType = tCopyType_Default;
  m_nWorkPaperProcessingMode = tProcessingMode_Default;
  m_sAddOnValue.clear();
  m_caTag.Reset();
  m_nTargetForTemplate = tTemplateTarget_Default;
  m_nSaveTemplateInFormat = tSaveTemplateInFormat_Default;
  m_nTemplateFileIcon = 0;
  m_caTemplateFileShowName.Reset();
  m_caTemplateFile.Reset();
  m_caTemplateFolder.Reset();
  m_sTemplateSymbolicDBName.erase();
  m_caTemplateDBName.Reset();
  m_nSourceViewActive = 1;
  m_nDataFormat = tDataFormatST_Default;
  m_nView1 = tViewTypeST_Scintilla;
  m_nView2 = tViewTypeST_DontUse;
}

CMPModelTemplateInd::CMPModelTemplateInd(CMPModelTemplateInd& cModel)
                    :CModelBaseInd(tMPModel),
                     CSyncNotifier_WP(tWPNotifier_TemplateMainProperty, NULL),
                     CSyncResponse_Proj(tRO_MainProperty_Template),
                     CSyncResponse_ER(tRO_MainProperty_Template),
                     CSyncResponse_IFS(tRO_MainProperty_Template),
                     CSyncResponse_WP(tRO_MainProperty_Template),
                     m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                     m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true),
                     m_caTemplateFileShowName(CConditionOrAssignment::tCOAT_TemplateFileShowName_Assignment, true),
                     m_caTemplateFile(CConditionOrAssignment::tCOAT_TemplateFile_Assignment, true),
                     m_caTemplateFolder(CConditionOrAssignment::tCOAT_TemplateFolder_Assignment, true),
                     m_caTemplateDBName(CConditionOrAssignment::tCOAT_TemplateDBName_Assignment, true)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelTemplateInd(&cModel);
}

CMPModelTemplateInd::CMPModelTemplateInd(CMPModelTemplateInd* pModel)
                    :CModelBaseInd(tMPModel),
                     CSyncNotifier_WP(tWPNotifier_TemplateMainProperty, NULL),
                     CSyncResponse_Proj(tRO_MainProperty_Template),
                     CSyncResponse_ER(tRO_MainProperty_Template),
                     CSyncResponse_IFS(tRO_MainProperty_Template),
                     CSyncResponse_WP(tRO_MainProperty_Template),
                     m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                     m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true),
                     m_caTemplateFileShowName(CConditionOrAssignment::tCOAT_TemplateFileShowName_Assignment, true),
                     m_caTemplateFile(CConditionOrAssignment::tCOAT_TemplateFile_Assignment, true),
                     m_caTemplateFolder(CConditionOrAssignment::tCOAT_TemplateFolder_Assignment, true),
                     m_caTemplateDBName(CConditionOrAssignment::tCOAT_TemplateDBName_Assignment, true)
{
  assert(pModel);
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelTemplateInd(pModel);
}

CMPModelTemplateInd::~CMPModelTemplateInd()
{
  RemoveAllMPConstantInds();
  RemoveAllMPKeyInds();
  RemoveAllMPTextInds();
  RemoveAllMPAreaInds();
  RemoveAllMPLinkInds();
  RemoveAllMPGraphicInds();
}

void CMPModelTemplateInd::SetMPModelTemplateInd(CMPModelTemplateInd* pModel)
{
  if (!pModel)
    return;

  m_sName = pModel->m_sName;
  m_sComment = pModel->m_sComment;
  m_sAutor = pModel->m_sAutor;
  m_caVisibility = pModel->m_caVisibility;
  m_sInformationSight = pModel->m_sInformationSight;
  m_bUseInformationSight = pModel->m_bUseInformationSight;
  m_sIFSTargetEntity = pModel->m_sIFSTargetEntity;
  m_bUseIFSTargetEntity = pModel->m_bUseIFSTargetEntity;
  m_sStoneOrTemplate = pModel->m_sStoneOrTemplate;
  m_bUseStoneOrTemplate = pModel->m_bUseStoneOrTemplate;
  m_nWorkPaperCopyType = pModel->m_nWorkPaperCopyType;
  m_nWorkPaperProcessingMode = pModel->m_nWorkPaperProcessingMode;
  m_sAddOnValue = pModel->m_sAddOnValue;
  m_caTag = pModel->m_caTag;
  m_nTargetForTemplate = pModel->m_nTargetForTemplate;
  m_nSaveTemplateInFormat = pModel->m_nSaveTemplateInFormat;
  m_nTemplateFileIcon = pModel->m_nTemplateFileIcon;
  m_caTemplateFileShowName = pModel->m_caTemplateFileShowName;
  m_caTemplateFile = pModel->m_caTemplateFile;
  m_caTemplateFolder = pModel->m_caTemplateFolder;
  m_sTemplateSymbolicDBName = pModel->m_sTemplateSymbolicDBName;
  m_caTemplateDBName = pModel->m_caTemplateDBName;
  m_nSourceViewActive = pModel->m_nSourceViewActive;
  m_nDataFormat = pModel->m_nDataFormat;
  m_nView1 = pModel->m_nView1;
  m_nView2 = pModel->m_nView2;

  size_t nI;

  RemoveAllMPConstantInds();
  for (nI = 0; nI < pModel->CountOfMPConstantInds(); nI++)
    AddMPConstantInd(pModel->GetMPConstantInd(nI));
  RemoveAllMPKeyInds();
  for (nI = 0; nI < pModel->CountOfMPKeyInds(); nI++)
    AddMPKeyInd(pModel->GetMPKeyInd(nI));
  RemoveAllMPTextInds();
  for (nI = 0; nI < pModel->CountOfMPTextInds(); nI++)
    AddMPTextInd(pModel->GetMPTextInd(nI));
  RemoveAllMPAreaInds();
  for (nI = 0; nI < pModel->CountOfMPAreaInds(); nI++)
    AddMPAreaInd(pModel->GetMPAreaInd(nI));
  RemoveAllMPLinkInds();
  for (nI = 0; nI < pModel->CountOfMPLinkInds(); nI++)
    AddMPLinkInd(pModel->GetMPLinkInd(nI));
  RemoveAllMPGraphicInds();
  for (nI = 0; nI < pModel->CountOfMPGraphicInds(); nI++)
    AddMPGraphicInd(pModel->GetMPGraphicInd(nI));
}

void CMPModelTemplateInd::SetInformationSight(const xtstring& sInformationSight)
{
  m_sInformationSight = sInformationSight;
  size_t nI;
  for (nI = 0; nI < CountOfMPKeyInds(); nI++)
  {
    CMPModelKeyInd* p = GetMPKeyInd(nI);
    if (p)
      p->SetInformationSight(GetInformationSight());
  }
  for (nI = 0; nI < CountOfMPTextInds(); nI++)
  {
    CMPModelTextInd* p = GetMPTextInd(nI);
    if (p)
      p->SetInformationSight(GetInformationSight());
  }
  for (nI = 0; nI < CountOfMPAreaInds(); nI++)
  {
    CMPModelAreaInd* p = GetMPAreaInd(nI);
    if (p)
      p->SetInformationSight(GetInformationSight());
  }
  for (nI = 0; nI < CountOfMPLinkInds(); nI++)
  {
    CMPModelLinkInd* p = GetMPLinkInd(nI);
    if (p)
      p->SetInformationSight(GetInformationSight());
  }
  for (nI = 0; nI < CountOfMPGraphicInds(); nI++)
  {
    CMPModelGraphicInd* p = GetMPGraphicInd(nI);
    if (p)
      p->SetInformationSight(GetInformationSight());
  }
}

bool CMPModelTemplateInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(PROPERTY_MP_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <MainProperty>
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_COMMENT, GetComment().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_AUTOR, GetAutor().c_str());

    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SOURCEVIEWACTIVE, GetSourceViewActive());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_TARGETFORTEMPLATE, GetTargetForTemplate());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SAVETEMPLATEINFORMAT, GetSaveTemplateInFormat());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_TEMPLATEFILE_ICON, GetTemplateFileIcon());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_TEMPLATESYMBOLICDBNAME, GetTemplateSymbolicDBName().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_DATAFORMAT, GetDataFormat());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_VIEW1, GetView1());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_VIEW2, GetView2());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_IFSVIEW, GetInformationSight().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_USEIFSVIEW, GetUseInformationSight());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_IFSTARGETENTITY, GetIFSTargetEntity().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_USEIFSTARGETENTITY, GetUseIFSTargetEntity());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_STONEORTEMPLATE, GetStoneOrTemplate().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_USESTONEORTEMPLATE, GetUseStoneOrTemplate());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_WPCOPYTYPE, GetWorkPaperCopyType());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_WPPROCESSINGMODE, GetWorkPaperProcessingMode());
    m_caVisibility.SaveXMLContent(pXMLDoc);
    m_caTag.SaveXMLContent(pXMLDoc);
    m_caTemplateFileShowName.SaveXMLContent(pXMLDoc);
    m_caTemplateFile.SaveXMLContent(pXMLDoc);
    m_caTemplateFolder.SaveXMLContent(pXMLDoc);
    m_caTemplateDBName.SaveXMLContent(pXMLDoc);
    if (pXMLDoc->AddChildElem(CONSTANTS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Constants>
      for (size_t nI = 0; nI < CountOfMPConstantInds(); nI++)
        GetMPConstantInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    if (pXMLDoc->AddChildElem(KEYS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Keys>
      for (size_t nI = 0; nI < CountOfMPKeyInds(); nI++)
        GetMPKeyInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    if (pXMLDoc->AddChildElem(TEXTS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Texts>
      for (size_t nI = 0; nI < CountOfMPTextInds(); nI++)
        GetMPTextInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    if (pXMLDoc->AddChildElem(AREAS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Areas>
      for (size_t nI = 0; nI < CountOfMPAreaInds(); nI++)
        GetMPAreaInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    if (pXMLDoc->AddChildElem(LINKS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Links>
      for (size_t nI = 0; nI < CountOfMPLinkInds(); nI++)
        GetMPLinkInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    if (pXMLDoc->AddChildElem(GRAPHICS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Graphics>
      for (size_t nI = 0; nI < CountOfMPGraphicInds(); nI++)
        GetMPGraphicInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CMPModelTemplateInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  long lVal;
  assert(pXMLDoc);

  // in <MainProperty>
  SetName(pXMLDoc->GetAttrib(NODE_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(NODE_ATTRIB_COMMENT));
  SetAutor(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_AUTOR));

  SetSourceViewActive(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SOURCEVIEWACTIVE));
  lVal = pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_TARGETFORTEMPLATE);
  if (lVal <= tTemplateTarget_FirstDummy || lVal >= tTemplateTarget_LastDummy)
    lVal = tTemplateTarget_Default;
  SetTargetForTemplate((TTemplateTarget)lVal);
  lVal = pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SAVETEMPLATEINFORMAT);
  if (lVal <= tSaveTemplateInFormat_FirstDummy || lVal >= tSaveTemplateInFormat_LastDummy)
    lVal = tSaveTemplateInFormat_Default;
  SetSaveTemplateInFormat((TSaveTemplateInFormat)lVal);
  SetTemplateFileIcon(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_TEMPLATEFILE_ICON));
  SetTemplateSymbolicDBName(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_TEMPLATESYMBOLICDBNAME));
  lVal = pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_DATAFORMAT);
  if (lVal <= tDataFormatST_FirstDummy || lVal >= tDataFormatST_LastDummy)
    lVal = tDataFormatST_Default;
  SetDataFormat((TDataFormatForStoneAndTemplate)lVal);
  lVal = pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_VIEW1);
  if (lVal <= tViewTypeST_FirstDummy || lVal >= tViewTypeST_LastDummy)
    lVal = tViewTypeST_DontUse;
  SetView1((TViewTypeForStoneAndTemplate)lVal);
  lVal = pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_VIEW2);
  if (lVal <= tViewTypeST_FirstDummy || lVal >= tViewTypeST_LastDummy)
    lVal = tViewTypeST_DontUse;
  SetView2((TViewTypeForStoneAndTemplate)lVal);
  SetInformationSight(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_IFSVIEW));
  SetUseInformationSight(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_USEIFSVIEW) != 0);
  SetIFSTargetEntity(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_IFSTARGETENTITY));
  SetUseIFSTargetEntity(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_USEIFSTARGETENTITY) != 0);
  SetStoneOrTemplate(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_STONEORTEMPLATE));
  SetUseStoneOrTemplate(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_USESTONEORTEMPLATE) != 0);
  TWorkPaperCopyType nType = (TWorkPaperCopyType)pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_WPCOPYTYPE);
  if (nType <= tCopyType_FirstDummy || nType >= tCopyType_LastDummy)
    nType = tCopyType_Default;
  SetWorkPaperCopyType(nType);
  TWorkPaperProcessingMode nMode = (TWorkPaperProcessingMode)pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_WPPROCESSINGMODE);
  if (nMode <= tProcessingMode_FirstDummy || nMode >= tProcessingMode_LastDummy)
    nMode = tProcessingMode_Default;
  SetWorkPaperProcessingMode(nMode);
  m_caVisibility.ReadXMLContent(pXMLDoc);
  m_caTag.ReadXMLContent(pXMLDoc);
  m_caTemplateFileShowName.ReadXMLContent(pXMLDoc);
  m_caTemplateFile.ReadXMLContent(pXMLDoc);
  m_caTemplateFolder.ReadXMLContent(pXMLDoc);
  m_caTemplateDBName.ReadXMLContent(pXMLDoc);

  RemoveAllMPConstantInds();
  RemoveAllMPKeyInds();
  RemoveAllMPTextInds();
  RemoveAllMPAreaInds();
  RemoveAllMPLinkInds();
  RemoveAllMPGraphicInds();
  // load Constants
  if (pXMLDoc->FindChildElem(CONSTANTS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Constants>
    while (pXMLDoc->FindChildElem(CONSTANT_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Constant>
      CMPModelConstantInd cModel(GetSyncGroup());
      cModel.ReadXMLContent(pXMLDoc);
      AddMPConstantInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <Constants>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load Keys
  if (pXMLDoc->FindChildElem(KEYS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Keys>
    while (pXMLDoc->FindChildElem(KEY_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Key>
      CMPModelKeyInd cModel(CUserOfObject::tMT_TemplateModule, GetInformationSight(), GetSyncGroup());
      cModel.ReadXMLContent(pXMLDoc);
      AddMPKeyInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <Keys>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load Texts
  if (pXMLDoc->FindChildElem(TEXTS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Texts>
    while (pXMLDoc->FindChildElem(TEXT_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Text>
      CMPModelTextInd cModel(CUserOfObject::tMT_TemplateModule, GetInformationSight(), GetSyncGroup());
      cModel.ReadXMLContent(pXMLDoc);
      AddMPTextInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <Texts>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load Areas
  if (pXMLDoc->FindChildElem(AREAS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Areas>
    while (pXMLDoc->FindChildElem(AREA_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Area>
      CMPModelAreaInd cModel(CUserOfObject::tMT_TemplateModule, GetInformationSight(), GetSyncGroup());
      cModel.ReadXMLContent(pXMLDoc);
      AddMPAreaInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <Areas>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load Links
  if (pXMLDoc->FindChildElem(LINKS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Links>
    while (pXMLDoc->FindChildElem(LINK_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Link>
      CMPModelLinkInd cModel(CUserOfObject::tMT_TemplateModule, GetInformationSight(), GetSyncGroup());
      cModel.ReadXMLContent(pXMLDoc);
      AddMPLinkInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <Links>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load Graphics
  if (pXMLDoc->FindChildElem(GRAPHICS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Graphics>
    while (pXMLDoc->FindChildElem(GRAPHIC_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Graphic>
      CMPModelGraphicInd cModel(CUserOfObject::tMT_TemplateModule, GetInformationSight(), GetSyncGroup());
      cModel.ReadXMLContent(pXMLDoc);
      AddMPGraphicInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <Graphicss>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }

  return bRet;
}

bool CMPModelTemplateInd::ReadXMLModuleName(CXPubMarkUp *pXMLDoc,
                                            xtstring& sModuleName)
{
  bool bRet = true;
  assert(pXMLDoc);

  // in <Node>
  sModuleName = pXMLDoc->GetAttrib(NODE_ATTRIB_NAME);
  return bRet;
}

void CMPModelTemplateInd::GetMPConstantInds(CxtstringVector& arrConstants)
{
  for (size_t nI = 0; nI < CountOfMPConstantInds(); nI++)
    arrConstants.push_back(m_arrConstants[nI]->GetName());
}

CMPModelConstantInd* CMPModelTemplateInd::GetMPConstantInd(const xtstring& sName)
{
  CMPModelConstantInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfMPConstantInds(); nI++)
  {
    if (sName == m_arrConstants[nI]->GetName())
    {
      pRetModel = m_arrConstants[nI];
      break;
    }
  }
  return pRetModel;
}
bool CMPModelTemplateInd::RemoveMPConstantInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrConstants.size());
  if (nIndex >= 0 && nIndex < m_arrConstants.size())
  {
    CMPModelConstantInd* pModel = m_arrConstants[nIndex];
    delete pModel;
    m_arrConstants.erase(m_arrConstants.begin() + nIndex);
    return true;
  }
  return false;
}
bool CMPModelTemplateInd::RemoveMPConstantInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfMPConstantInds(); nI++)
  {
    if (sName == GetMPConstantInd(nI)->GetName())
      return RemoveMPConstantInd(nI);
  }
  return false;
}
bool CMPModelTemplateInd::RemoveAllMPConstantInds()
{
  while (m_arrConstants.size())
    RemoveMPConstantInd(0);
  m_arrConstants.erase(m_arrConstants.begin(), m_arrConstants.end());
  return true;
}

CMPModelKeyInd* CMPModelTemplateInd::GetMPKeyInd(const xtstring& sName)
{
  CMPModelKeyInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfMPKeyInds(); nI++)
  {
    if (sName == m_arrKeys[nI]->GetName())
    {
      pRetModel = m_arrKeys[nI];
      break;
    }
  }
  return pRetModel;
}
bool CMPModelTemplateInd::RemoveMPKeyInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrKeys.size());
  if (nIndex >= 0 && nIndex < m_arrKeys.size())
  {
    CMPModelKeyInd* pModel = m_arrKeys[nIndex];
    delete pModel;
    m_arrKeys.erase(m_arrKeys.begin() + nIndex);
    return true;
  }
  return false;
}
bool CMPModelTemplateInd::RemoveMPKeyInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfMPKeyInds(); nI++)
  {
    if (sName == GetMPKeyInd(nI)->GetName())
      return RemoveMPKeyInd(nI);
  }
  return false;
}
bool CMPModelTemplateInd::RemoveAllMPKeyInds()
{
  while (m_arrKeys.size())
    RemoveMPKeyInd(0);
  m_arrKeys.erase(m_arrKeys.begin(), m_arrKeys.end());
  return true;
}

CMPModelTextInd* CMPModelTemplateInd::GetMPTextInd(const xtstring& sName)
{
  CMPModelTextInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfMPTextInds(); nI++)
  {
    if (sName == m_arrTexts[nI]->GetName())
    {
      pRetModel = m_arrTexts[nI];
      break;
    }
  }
  return pRetModel;
}
bool CMPModelTemplateInd::RemoveMPTextInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrTexts.size());
  if (nIndex >= 0 && nIndex < m_arrTexts.size())
  {
    CMPModelTextInd* pModel = m_arrTexts[nIndex];
    delete pModel;
    m_arrTexts.erase(m_arrTexts.begin() + nIndex);
    return true;
  }
  return false;
}
bool CMPModelTemplateInd::RemoveMPTextInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfMPTextInds(); nI++)
  {
    if (sName == GetMPTextInd(nI)->GetName())
      return RemoveMPTextInd(nI);
  }
  return false;
}
bool CMPModelTemplateInd::RemoveAllMPTextInds()
{
  while (m_arrTexts.size())
    RemoveMPTextInd(0);
  m_arrTexts.erase(m_arrTexts.begin(), m_arrTexts.end());
  return true;
}

CMPModelAreaInd* CMPModelTemplateInd::GetMPAreaInd(const xtstring& sName)
{
  CMPModelAreaInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfMPAreaInds(); nI++)
  {
    if (sName == m_arrAreas[nI]->GetName())
    {
      pRetModel = m_arrAreas[nI];
      break;
    }
  }
  return pRetModel;
}
bool CMPModelTemplateInd::RemoveMPAreaInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrAreas.size());
  if (nIndex >= 0 && nIndex < m_arrAreas.size())
  {
    CMPModelAreaInd* pModel = m_arrAreas[nIndex];
    delete pModel;
    m_arrAreas.erase(m_arrAreas.begin() + nIndex);
    return true;
  }
  return false;
}
bool CMPModelTemplateInd::RemoveMPAreaInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfMPAreaInds(); nI++)
  {
    if (sName == GetMPAreaInd(nI)->GetName())
      return RemoveMPAreaInd(nI);
  }
  return false;
}
bool CMPModelTemplateInd::RemoveAllMPAreaInds()
{
  while (m_arrAreas.size())
    RemoveMPAreaInd(0);
  m_arrAreas.erase(m_arrAreas.begin(), m_arrAreas.end());
  return true;
}

CMPModelLinkInd* CMPModelTemplateInd::GetMPLinkInd(const xtstring& sName)
{
  CMPModelLinkInd* pRetModel = NULL;
  for (size_t nI = 0; nI < m_arrLinks.size(); nI++)
  {
    if (sName == m_arrLinks[nI]->GetName())
    {
      pRetModel = m_arrLinks[nI];
      break;
    }
  }
  return pRetModel;
}
bool CMPModelTemplateInd::RemoveMPLinkInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrLinks.size());
  if (nIndex >= 0 && nIndex < m_arrLinks.size())
  {
    CMPModelLinkInd* pModel = m_arrLinks[nIndex];
    delete pModel;
    m_arrLinks.erase(m_arrLinks.begin() + nIndex);
    return true;
  }
  return false;
}
bool CMPModelTemplateInd::RemoveMPLinkInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfMPLinkInds(); nI++)
  {
    if (sName == GetMPLinkInd(nI)->GetName())
      return RemoveMPLinkInd(nI);
  }
  return false;
}
bool CMPModelTemplateInd::RemoveAllMPLinkInds()
{
  while (m_arrLinks.size())
    RemoveMPLinkInd(0);
  m_arrLinks.erase(m_arrLinks.begin(), m_arrLinks.end());
  return true;
}

CMPModelGraphicInd* CMPModelTemplateInd::GetMPGraphicInd(const xtstring& sName)
{
  CMPModelGraphicInd* pRetModel = NULL;
  for (size_t nI = 0; nI < m_arrGraphics.size(); nI++)
  {
    if (sName == m_arrGraphics[nI]->GetName())
    {
      pRetModel = m_arrGraphics[nI];
      break;
    }
  }
  return pRetModel;
}
bool CMPModelTemplateInd::RemoveMPGraphicInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrGraphics.size());
  if (nIndex >= 0 && nIndex < m_arrGraphics.size())
  {
    CMPModelGraphicInd* pModel = m_arrGraphics[nIndex];
    delete pModel;
    m_arrGraphics.erase(m_arrGraphics.begin() + nIndex);
    return true;
  }
  return false;
}
bool CMPModelTemplateInd::RemoveMPGraphicInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfMPGraphicInds(); nI++)
  {
    if (sName == GetMPGraphicInd(nI)->GetName())
      return RemoveMPGraphicInd(nI);
  }
  return false;
}
bool CMPModelTemplateInd::RemoveAllMPGraphicInds()
{
  while (m_arrGraphics.size())
    RemoveMPGraphicInd(0);
  m_arrGraphics.erase(m_arrGraphics.begin(), m_arrGraphics.end());
  return true;
}


bool CMPModelTemplateInd::FieldInAnyGroupExist(const xtstring& sFieldName)
{
  if (GetMPConstantInd(sFieldName))
    return true;
  if (GetMPKeyInd(sFieldName))
    return true;
  if (GetMPTextInd(sFieldName))
    return true;
  if (GetMPAreaInd(sFieldName))
    return true;
  if (GetMPLinkInd(sFieldName))
    return true;
  if (GetMPGraphicInd(sFieldName))
    return true;
  return false;
}

bool CMPModelTemplateInd::IsNewFieldNameAsSubstringInAnotherFieldName(const xtstring& sOldFieldName,
                                                                      const xtstring& sNewFieldName,
                                                                      xtstring& sAnotherFieldName)
{
  for (size_t nI = 0; nI < CountOfMPConstantInds(); nI++)
  {
    CMPModelConstantInd* p = GetMPConstantInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = p->GetName().find(sNewFieldName);
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPKeyInds(); nI++)
  {
    CMPModelKeyInd* p = GetMPKeyInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = p->GetName().find(sNewFieldName);
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPTextInds(); nI++)
  {
    CMPModelTextInd* p = GetMPTextInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = p->GetName().find(sNewFieldName);
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPAreaInds(); nI++)
  {
    CMPModelAreaInd* p = GetMPAreaInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = p->GetName().find(sNewFieldName);
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPLinkInds(); nI++)
  {
    CMPModelLinkInd* p = GetMPLinkInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = p->GetName().find(sNewFieldName);
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPGraphicInds(); nI++)
  {
    CMPModelGraphicInd* p = GetMPGraphicInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = p->GetName().find(sNewFieldName);
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  return false;
}

bool CMPModelTemplateInd::IsAnotherFieldNameAsSubstringInThisNewFieldName(const xtstring& sOldFieldName,
                                                                          const xtstring& sNewFieldName,
                                                                          xtstring& sAnotherFieldName)
{
  for (size_t nI = 0; nI < CountOfMPConstantInds(); nI++)
  {
    CMPModelConstantInd* p = GetMPConstantInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = sNewFieldName.find(p->GetName());
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPKeyInds(); nI++)
  {
    CMPModelKeyInd* p = GetMPKeyInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = sNewFieldName.find(p->GetName());
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPTextInds(); nI++)
  {
    CMPModelTextInd* p = GetMPTextInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = sNewFieldName.find(p->GetName());
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPAreaInds(); nI++)
  {
    CMPModelAreaInd* p = GetMPAreaInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = sNewFieldName.find(p->GetName());
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPLinkInds(); nI++)
  {
    CMPModelLinkInd* p = GetMPLinkInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = sNewFieldName.find(p->GetName());
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  for (size_t nI = 0; nI < CountOfMPGraphicInds(); nI++)
  {
    CMPModelGraphicInd* p = GetMPGraphicInd(nI);
    if (!p)
      continue;
    if (p->GetName() == sOldFieldName)
      continue;
    size_t nPos = sNewFieldName.find(p->GetName());
    if (nPos != xtstring::npos)
    {
      sAnotherFieldName = p->GetName();
      return true;
    }
  }
  return false;
}