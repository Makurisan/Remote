// MPModelPythonInd.cpp: implementation of the CMPModelPythonInd classes.
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
#include "MPModelPythonInd.h"






//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CMPModelPythonInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CMPModelPythonInd::CMPModelPythonInd()
                  :CModelBaseInd(tMPModel)
{
  m_sName = _XT("");
}

CMPModelPythonInd::CMPModelPythonInd(CMPModelPythonInd& cModel)
                  :CModelBaseInd(tMPModel)
{
  SetMPModelPythonInd(&cModel);
}

CMPModelPythonInd::CMPModelPythonInd(CMPModelPythonInd* pModel)
                  :CModelBaseInd(tMPModel)
{
  SetMPModelPythonInd(pModel);
}

CMPModelPythonInd::~CMPModelPythonInd()
{
}

void CMPModelPythonInd::SetMPModelPythonInd(CMPModelPythonInd* pModel)
{
  if (!pModel)
    return;

  m_sName = pModel->m_sName;
}

bool CMPModelPythonInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(PROPERTY_MP_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <MainProperty>
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_NAME, GetName().c_str());

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CMPModelPythonInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <MainProperty>
  SetName(xtstring(pXMLDoc->GetAttrib(NODE_ATTRIB_NAME)));

  return bRet;
}

bool CMPModelPythonInd::ReadXMLModuleName(CXPubMarkUp *pXMLDoc,
                                          xtstring& sModuleName)
{
  bool bRet = true;
  assert(pXMLDoc);

  // in <Node>
  sModuleName = pXMLDoc->GetAttrib(NODE_ATTRIB_NAME);
  return bRet;
}

