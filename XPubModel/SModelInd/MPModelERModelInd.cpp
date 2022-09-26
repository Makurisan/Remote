// MPModelERModelInd.cpp: implementation of the CMPModelERModelInd classes.
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
#include "MPModelERModelInd.h"






//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CMPModelERModelInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CMPModelERModelInd::CMPModelERModelInd(CSyncERNotifiersGroup* pGroup)
                   :CModelBaseInd(tMPModel),
                    CSyncNotifier_ER(tERNotifier_MainProperty, pGroup),
                    CSyncResponse_Proj(tRO_MainProperty_ER)
{
  m_bShowRelationNamesER = false;
  //
  m_sName = _XT("");

  m_sAutor.erase();
  m_sPassword.erase();
  //
  m_sDBNameProd.erase();
  m_sDBNameTest.erase();
  //
  m_nPosLeft = 0;
  m_nPosTop = 0;
  m_nSizeWidth = 0;
  m_nSizeHeight = 0;
  //
  m_nColor = 0;
  m_nColorBack = 0;
  m_bMoveable = false;
  m_bSelectable = true;
  m_bVisible = true;
  m_bSizeable = true;
}

CMPModelERModelInd::CMPModelERModelInd(CMPModelERModelInd& cModel)
                   :CModelBaseInd(tMPModel),
                    CSyncNotifier_ER(tERNotifier_MainProperty, NULL),
                    CSyncResponse_Proj(tRO_MainProperty_ER)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelERModelInd(&cModel);
}

CMPModelERModelInd::CMPModelERModelInd(CMPModelERModelInd* pModel)
                   :CModelBaseInd(tMPModel),
                    CSyncNotifier_ER(tERNotifier_MainProperty, NULL),
                    CSyncResponse_Proj(tRO_MainProperty_ER)
{
  assert(pModel);
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelERModelInd(pModel);
}

CMPModelERModelInd::~CMPModelERModelInd()
{
}

void CMPModelERModelInd::SE_OnTableNameChanged(const xtstring& sOldName,
                                               const xtstring& sNewName)
{
}

void CMPModelERModelInd::SetMPModelERModelInd(CMPModelERModelInd* pModel)
{
  if (!pModel)
    return;

  m_sName = pModel->m_sName;
  m_sAutor = pModel->m_sAutor;
  m_sPassword = pModel->m_sPassword;

  m_sDBNameProd = pModel->m_sDBNameProd;
  m_sDBNameTest = pModel->m_sDBNameTest;
}

bool CMPModelERModelInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(PROPERTY_MP_ELEM_NAME, _XT("")))
  {
//    time_t t = time(NULL);
//    SetTimeChanged(t);
    pXMLDoc->IntoElem();
    // in <MainProperty>
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_AUTOR, GetAutor().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_PASSWORD, GetPassword().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_DBNAMEPROD, GetDBNameProd().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_DBNAMETEST, GetDBNameTest().c_str());

    // folgendes kann nur fuer ER und DA abgespeichert werden
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SHOW_REL_NAMES, GetShowRelationNamesER());
    if (pXMLDoc->AddChildElem(NODEUI_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <NodeUI>
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_X, GetLeft());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_Y, GetTop());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_CX, GetWidth());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_CY, GetHeight());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR, GetColor());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR_BACK, GetColorBack());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_MOVEABLE, GetMoveable());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_SELECTABLE, GetSelectable());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_VISIBLE, GetVisible());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_SIZEABLE, GetSizeable());

      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CMPModelERModelInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <MainProperty>
  SetName(pXMLDoc->GetAttrib(NODE_ATTRIB_NAME));
  SetAutor(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_AUTOR));
  SetPassword(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_PASSWORD));
  SetDBNameProd(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_DBNAMEPROD));
  SetDBNameTest(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_DBNAMETEST));

  // weiteres kann nur fuer ER und DA gelesen werden
  SetShowRelationNamesER(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SHOW_REL_NAMES) != false);
  // load UI
  if (pXMLDoc->FindChildElem(NODEUI_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <NodeUI>

    SetLeft(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_X));
    SetTop(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_Y));
    SetWidth(abs(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_CX)));
    SetHeight(abs(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_CY)));
    SetColor(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR));
    SetColorBack(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_BACK));
    SetMoveable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_MOVEABLE) != false);
    SetSelectable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_SELECTABLE) != false);
    SetVisible(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_VISIBLE) != false);
    SetSizeable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_SIZEABLE) != false);

    pXMLDoc->OutOfElem();
    // in <MainProperty>
    bRet = true;
  }

  return bRet;
}

bool CMPModelERModelInd::ReadXMLModuleName(CXPubMarkUp *pXMLDoc,
                                           xtstring& sModuleName)
{
  bool bRet = true;
  assert(pXMLDoc);

  // in <Node>
  sModuleName = pXMLDoc->GetAttrib(NODE_ATTRIB_NAME);
  return bRet;
}

void CMPModelERModelInd::CopyDiverseProperties(CMPModelERModelInd* pModel)
{
  m_nColor = pModel->m_nColor;
  m_nColorBack = pModel->m_nColorBack;
  m_bMoveable = pModel->m_bMoveable;
  m_bSelectable = pModel->m_bSelectable;
  m_bVisible = pModel->m_bVisible;
  m_bSizeable = pModel->m_bSizeable;
}

void CMPModelERModelInd::CopyPositionProperties(CMPModelERModelInd* pModel)
{
  m_nPosLeft = pModel->m_nPosLeft;
  m_nPosTop = pModel->m_nPosTop;
  m_nSizeWidth = pModel->m_nSizeWidth;
  m_nSizeHeight = pModel->m_nSizeHeight;
}

///////////////////////////////////////////////////////////////////////////////
// interface fo common properties
///////////////////////////////////////////////////////////////////////////////
