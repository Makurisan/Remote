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

#include "SSyncEng/SyncGroups.h"






void CMPModelERModelInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CMPModelERModelInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CMPModelERModelInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CMPModelERModelInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
}
void CMPModelERModelInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                CUserOfObject* pObject)
{
}
void CMPModelERModelInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // production database
  if (sOldName == GetDBNameProd())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_ERModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       /*CSyncResponse_Proj::*/GetTypeOfResponseObject(),
                                                       /*CSyncResponse_Proj::*/GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_ER_DBNameProd,
                                        1,
                                        GetDBNameProd());
    cUsers.push_back(pResponseObject);
  }
  // test database
  if (sOldName == GetDBNameTest())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_ERModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       /*CSyncResponse_Proj::*/GetTypeOfResponseObject(),
                                                       /*CSyncResponse_Proj::*/GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_ER_DBNameTest,
                                        2,
                                        GetDBNameTest());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelERModelInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == /*CSyncResponse_Proj::*/GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != /*CSyncResponse_Proj::*/GetSyncResponseObject())
    return;

  // production database
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDBNameProd(sNewName);
    ROC_PropertyChanged_Proj();
  }
  // test database
  if (pObject->m_nIdentify == 2 && pObject->GetChange())
  {
    SetDBNameTest(sNewName);
    ROC_PropertyChanged_Proj();
  }
}
void CMPModelERModelInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CMPModelERModelInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // production database
  if (sName == GetDBNameProd())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_ERModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       /*CSyncResponse_Proj::*/GetTypeOfResponseObject(),
                                                       /*CSyncResponse_Proj::*/GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_ER_DBNameProd,
                                        1,
                                        GetDBNameProd());
    cUsers.push_back(pResponseObject);
  }
  // test database
  if (sName == GetDBNameTest())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_ERModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       /*CSyncResponse_Proj::*/GetTypeOfResponseObject(),
                                                       /*CSyncResponse_Proj::*/GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_ER_DBNameTest,
                                        2,
                                        GetDBNameTest());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelERModelInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == /*CSyncResponse_Proj::*/GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != /*CSyncResponse_Proj::*/GetSyncResponseObject())
    return;

  // production database
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDBNameProd(_XT(""));
    ROC_PropertyChanged_Proj();
  }
  // test database
  if (pObject->m_nIdentify == 2 && pObject->GetChange())
  {
    SetDBNameTest(_XT(""));
    ROC_PropertyChanged_Proj();
  }
}
