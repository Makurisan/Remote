// MPModelIFSInd.cpp: implementation of the CMPModelIFSInd classes.
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
#include "MPModelIFSInd.h"






//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CMPModelIFSInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CMPModelIFSInd::CMPModelIFSInd(CSyncIFSNotifiersGroup* pGroup)
               :CModelBaseInd(tMPModel),
                CSyncNotifier_IFS(tIFSNotifier_MainProperty, pGroup),
                CSyncResponse_ER(tRO_MainProperty_IFS)
{
  m_sName = _XT("");

  m_sAutor.erase();
  m_sPassword.erase();
  m_nIFSTypeForThirdParty = tIFSType_Default;
  m_sIFSNameForThirdParty.erase();
  m_bShowOpenedEntitiesHierarchical = true;
  m_bShowOpenedNameAlways = true;
  m_bShowExpanded = true;
  m_nShowBehaviour = tShowBehaviour_Default;
  //
  m_sERModelName.erase();
  m_sParamTable.erase();
  m_sIFSPropertyStones.erase();
  m_sIFSDBStones.erase();
  m_bShowRelationNames = true;
  m_bShowHitLines = true;
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

CMPModelIFSInd::CMPModelIFSInd(CMPModelIFSInd& cModel)
               :CModelBaseInd(tMPModel),
                CSyncNotifier_IFS(tIFSNotifier_MainProperty, NULL),
                CSyncResponse_ER(tRO_MainProperty_IFS)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelIFSInd(&cModel);
}

CMPModelIFSInd::CMPModelIFSInd(CMPModelIFSInd* pModel)
               :CModelBaseInd(tMPModel),
                CSyncNotifier_IFS(tIFSNotifier_MainProperty, NULL),
                CSyncResponse_ER(tRO_MainProperty_IFS)
{
  assert(pModel);
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelIFSInd(pModel);
}

CMPModelIFSInd::~CMPModelIFSInd()
{
}

void CMPModelIFSInd::OnTableNameChanged(const xtstring& sOldName,
                                        const xtstring& sNewName)
{
//  if (GetParamTable() == sOldName)
//  {
//    SetParamTable(sNewName);
//    staticParamTableChanged();
//  }
}

void CMPModelIFSInd::SetMPModelIFSInd(CMPModelIFSInd* pModel)
{
  if (!pModel)
    return;

  m_sParamTable = pModel->m_sParamTable;
  m_sIFSPropertyStones = pModel->m_sIFSPropertyStones;
  m_sIFSDBStones = pModel->m_sIFSDBStones;

  m_sName = pModel->m_sName;
  m_sAutor = pModel->m_sAutor;
  m_sPassword = pModel->m_sPassword;
  m_nIFSTypeForThirdParty = pModel->m_nIFSTypeForThirdParty;
  m_sIFSNameForThirdParty = pModel->m_sIFSNameForThirdParty;
  m_bShowOpenedEntitiesHierarchical = pModel->m_bShowOpenedEntitiesHierarchical;
  m_bShowOpenedNameAlways = pModel->m_bShowOpenedNameAlways;
  m_bShowExpanded = pModel->m_bShowExpanded;
  m_nShowBehaviour = pModel->m_nShowBehaviour;

  m_sERModelName = pModel->m_sERModelName;
}

bool CMPModelIFSInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(PROPERTY_MP_ELEM_NAME, _XT("")))
  {
//    time_t t = time(NULL);
//    SetTimeChanged(t);
    pXMLDoc->IntoElem();
    // in <MainProperty>
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_PARAMTABLE, GetParamTableName().c_str());

    pXMLDoc->AddChildElem(PROPERTY_MP_IFSPROPERTYSTONES, GetIFSPropertyStones().c_str());
    pXMLDoc->AddChildElem(PROPERTY_MP_IFSDBSTONES, GetIFSDBStones().c_str());

    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_AUTOR, GetAutor().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_PASSWORD, GetPassword().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_IFSTYPEFORTHIRDPARTY, GetIFSTypeForThirdParty());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_IFSNAMEFORTHIRDPARTY, GetIFSNameForThirdParty().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SHOWOPENEDENTITIESHIERARCHICAL, GetShowOpenedEntitiesHierarchical());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SHOWOPENEDNAMEALWAYS, GetShowOpenedNameAlways());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SHOWEXPANDED, GetShowExpanded());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SHOWBEHAVIOUR, GetShowBehaviour());

    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_ERMODELNAME, GetERModelName().c_str());

    // folgendes kann nur fuer ER und DA abgespeichert werden
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SHOW_REL_NAMES, GetShowRelationNames());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SHOW_HIT_LINES, GetShowHitLines());
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

bool CMPModelIFSInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <MainProperty>
  SetParamTableName(xtstring(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_PARAMTABLE)));

  SetIFSPropertyStones(_XT(""));
  if (pXMLDoc->FindChildElem(PROPERTY_MP_IFSPROPERTYSTONES))
  {
    pXMLDoc->IntoElem();
    SetIFSPropertyStones(pXMLDoc->GetData());
    pXMLDoc->OutOfElem();
  }
  // folgendem ELSE IF irgendwann entfernen
  else if (pXMLDoc->FindChildElem(_XT("IFSStones")))
  {
    pXMLDoc->IntoElem();
    SetIFSPropertyStones(pXMLDoc->GetData());
    pXMLDoc->OutOfElem();
  }
  SetIFSDBStones(_XT(""));
  if (pXMLDoc->FindChildElem(PROPERTY_MP_IFSDBSTONES))
  {
    pXMLDoc->IntoElem();
    SetIFSDBStones(pXMLDoc->GetData());
    pXMLDoc->OutOfElem();
  }

  SetName(pXMLDoc->GetAttrib(NODE_ATTRIB_NAME));
  SetAutor(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_AUTOR));
  SetPassword(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_PASSWORD));
  TIFSTypeForThirdParty nType = (TIFSTypeForThirdParty)pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_IFSTYPEFORTHIRDPARTY);
  if (nType <= tIFSType_FirstDummy || nType >= tIFSType_LastDummy)
    nType = tIFSType_Default;
  SetIFSTypeForThirdParty(nType);
  SetIFSNameForThirdParty(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_IFSNAMEFORTHIRDPARTY));
  SetShowOpenedEntitiesHierarchical(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SHOWOPENEDENTITIESHIERARCHICAL) != 0);
  SetShowOpenedNameAlways(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SHOWOPENEDNAMEALWAYS) != 0);
  SetShowExpanded(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SHOWEXPANDED) != 0);
  SetShowBehaviour((TShowBehaviour)pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SHOWBEHAVIOUR));
  if (GetShowBehaviour() <= tShowBehaviour_FirstDummy || GetShowBehaviour() >= tShowBehaviour_LastDummy)
    SetShowBehaviour(tShowBehaviour_Default);

  SetERModelName(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_ERMODELNAME));

  // weiteres kann nur fuer ER und DA gelesen werden
  SetShowRelationNames(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SHOW_REL_NAMES) != 0);
  SetShowHitLines(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SHOW_HIT_LINES) != 0);
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

bool CMPModelIFSInd::ReadXMLModuleName(CXPubMarkUp *pXMLDoc,
                                       xtstring& sModuleName)
{
  bool bRet = true;
  assert(pXMLDoc);

  // in <Node>
  sModuleName = pXMLDoc->GetAttrib(NODE_ATTRIB_NAME);
  return bRet;
}

void CMPModelIFSInd::CopyDiverseProperties(CMPModelIFSInd* pModel)
{
  m_nColor = pModel->m_nColor;
  m_nColorBack = pModel->m_nColorBack;
  m_bMoveable = pModel->m_bMoveable;
  m_bSelectable = pModel->m_bSelectable;
  m_bVisible = pModel->m_bVisible;
  m_bSizeable = pModel->m_bSizeable;
}

void CMPModelIFSInd::CopyPositionProperties(CMPModelIFSInd* pModel)
{
  m_nPosLeft = pModel->m_nPosLeft;
  m_nPosTop = pModel->m_nPosTop;
  m_nSizeWidth = pModel->m_nSizeWidth;
  m_nSizeHeight = pModel->m_nSizeHeight;
}

///////////////////////////////////////////////////////////////////////////////
// interface fo common properties
///////////////////////////////////////////////////////////////////////////////
