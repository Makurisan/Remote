#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"
#include "DBDefinitions.h"



#define DEFAULT_DB_PORT_MYSQL       3306


CDBDefinition::CDBDefinition()
{
  ResetValues();
}

CDBDefinition::~CDBDefinition()
{
}

void CDBDefinition::ResetValues()
{
  // common
  m_sName.clear();
  m_nDBTypeToUse = tDBType_Default;
  // MySQL
  m_MySQL_sDatabaseName.clear();
  m_MySQL_sDBUserName.clear();
  m_MySQL_sDBPassword.clear();
  m_MySQL_sDBHostNameOrIP.clear();
  m_MySQL_nDBPort = DEFAULT_DB_PORT_MYSQL;
  m_MySQL_bDBCompress = false;
  // MSADO
  m_MSADO_sDatabaseName.clear();
  m_MSADO_sDBUserName.clear();
  m_MSADO_sDBPassword.clear();
  m_MSADO_sProvider.clear();
  // IMDB
  m_IMDB_sSharedMemoryName.clear();
  // DataProvider
  m_DataProvider_sProviderName.clear();
  m_DataProvider_sProviderSpecific.clear();
  // Firebird
  m_Firebird_sServerName.clear();
  m_Firebird_sDatabaseName.clear();
  m_Firebird_sUserName.clear();
  m_Firebird_sUserPassword.clear();
  // XMLEngine
  m_XMLEngine_sFolder.clear();
  m_XMLEngine_bUseInMemoryDatabase = true;
  m_XMLEngine_sTempDatabaseFileName.clear();
  // SQLite
  m_SQLite_sDatabaseFileName.clear();
}

bool CDBDefinition::CopyFrom(const CDBDefinition* pDBDefinition,
                             bool& bValuesChanged)
{
  bValuesChanged = false;
  if (!pDBDefinition)
    return false;

  // common
  if (m_sName != pDBDefinition->m_sName)
    bValuesChanged = true;
  if (m_nDBTypeToUse != pDBDefinition->m_nDBTypeToUse)
    bValuesChanged = true;
  // MySQL
  if (m_MySQL_sDatabaseName != pDBDefinition->m_MySQL_sDatabaseName)
    bValuesChanged = true;
  if (m_MySQL_sDBUserName != pDBDefinition->m_MySQL_sDBUserName)
    bValuesChanged = true;
  if (m_MySQL_sDBPassword != pDBDefinition->m_MySQL_sDBPassword)
    bValuesChanged = true;
  if (m_MySQL_sDBHostNameOrIP != pDBDefinition->m_MySQL_sDBHostNameOrIP)
    bValuesChanged = true;
  if (m_MySQL_nDBPort != pDBDefinition->m_MySQL_nDBPort)
    bValuesChanged = true;
  if (m_MySQL_bDBCompress != pDBDefinition->m_MySQL_bDBCompress)
    bValuesChanged = true;
  // MSADO
  if (m_MSADO_sDatabaseName != pDBDefinition->m_MSADO_sDatabaseName)
    bValuesChanged = true;
  if (m_MSADO_sDBUserName != pDBDefinition->m_MSADO_sDBUserName)
    bValuesChanged = true;
  if (m_MSADO_sDBPassword != pDBDefinition->m_MSADO_sDBPassword)
    bValuesChanged = true;
  if (m_MSADO_sProvider != pDBDefinition->m_MSADO_sProvider)
    bValuesChanged = true;
  // IMDB
  if (m_IMDB_sSharedMemoryName != pDBDefinition->m_IMDB_sSharedMemoryName)
    bValuesChanged = true;
  // DataProvider
  if (m_DataProvider_sProviderName != pDBDefinition->m_DataProvider_sProviderName)
    bValuesChanged = true;
  if (m_DataProvider_sProviderSpecific != pDBDefinition->m_DataProvider_sProviderSpecific)
    bValuesChanged = true;
  // Firebird
  if (m_Firebird_sServerName != pDBDefinition->m_Firebird_sServerName)
    bValuesChanged = true;
  if (m_Firebird_sDatabaseName != pDBDefinition->m_Firebird_sDatabaseName)
    bValuesChanged = true;
  if (m_Firebird_sUserName != pDBDefinition->m_Firebird_sUserName)
    bValuesChanged = true;
  if (m_Firebird_sUserPassword != pDBDefinition->m_Firebird_sUserPassword)
    bValuesChanged = true;
  // XMLEngine
  if (m_XMLEngine_sFolder != pDBDefinition->m_XMLEngine_sFolder)
    bValuesChanged = true;
  if (m_XMLEngine_bUseInMemoryDatabase != pDBDefinition->m_XMLEngine_bUseInMemoryDatabase)
    bValuesChanged = true;
  if (m_XMLEngine_sTempDatabaseFileName != pDBDefinition->m_XMLEngine_sTempDatabaseFileName)
    bValuesChanged = true;
  // SQLite
  if (m_SQLite_sDatabaseFileName != pDBDefinition->m_SQLite_sDatabaseFileName)
    bValuesChanged = true;

  // common
  m_sName                           = pDBDefinition->m_sName;
  m_nDBTypeToUse                    = pDBDefinition->m_nDBTypeToUse;

  // MySQL
  m_MySQL_sDatabaseName             = pDBDefinition->m_MySQL_sDatabaseName;
  m_MySQL_sDBUserName               = pDBDefinition->m_MySQL_sDBUserName;
  m_MySQL_sDBPassword               = pDBDefinition->m_MySQL_sDBPassword;
  m_MySQL_sDBHostNameOrIP           = pDBDefinition->m_MySQL_sDBHostNameOrIP;
  m_MySQL_nDBPort                   = pDBDefinition->m_MySQL_nDBPort;
  m_MySQL_bDBCompress               = pDBDefinition->m_MySQL_bDBCompress;
  // MSADO
  m_MSADO_sDatabaseName             = pDBDefinition->m_MSADO_sDatabaseName;
  m_MSADO_sDBUserName               = pDBDefinition->m_MSADO_sDBUserName;
  m_MSADO_sDBPassword               = pDBDefinition->m_MSADO_sDBPassword;
  m_MSADO_sProvider                 = pDBDefinition->m_MSADO_sProvider;
  // IMDB
  m_IMDB_sSharedMemoryName          = pDBDefinition->m_IMDB_sSharedMemoryName;
  // DataProvider
  m_DataProvider_sProviderName      = pDBDefinition->m_DataProvider_sProviderName;
  m_DataProvider_sProviderSpecific  = pDBDefinition->m_DataProvider_sProviderSpecific;
  // Firebird
  m_Firebird_sServerName            = pDBDefinition->m_Firebird_sServerName;
  m_Firebird_sDatabaseName          = pDBDefinition->m_Firebird_sDatabaseName;
  m_Firebird_sUserName              = pDBDefinition->m_Firebird_sUserName;
  m_Firebird_sUserPassword          = pDBDefinition->m_Firebird_sUserPassword;
  // XMLEngine
  m_XMLEngine_sFolder               = pDBDefinition->m_XMLEngine_sFolder;
  m_XMLEngine_bUseInMemoryDatabase  = pDBDefinition->m_XMLEngine_bUseInMemoryDatabase;
  m_XMLEngine_sTempDatabaseFileName = pDBDefinition->m_XMLEngine_sTempDatabaseFileName;
  // SQLite
  m_SQLite_sDatabaseFileName        = pDBDefinition->m_SQLite_sDatabaseFileName;

  return true;
}

bool CDBDefinition::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  // XMLDoc in <DBDefinition>

  // common
  SetName                           (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_NAME));
  TDBDefinitionType nType;
  nType = (TDBDefinitionType)pXMLDoc->GetAttribLong(XMLTAG_DBDEFINITION_DBTYPETOUSE);
  if (nType <= tDBTypeFirst_Dummy || nType >= tDBTypeLast_Dummy)
    nType = tDBType_Default;
  SetDBTypeToUse(nType);
  // MySQL
  Set_MySQLDatabaseName             (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_MYSQL_DATABASENAME));
  Set_MySQLDBUserName               (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_MYSQL_DBUSERNAME));
  Set_MySQLDBPassword               (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_MYSQL_DBPASSWORD));
  Set_MySQLDBHostNameOrIP           (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_MYSQL_DBHOSTNAMEORIP));
  Set_MySQLDBPort                   (pXMLDoc->GetAttribLong(XMLTAG_DBDEFINITION_MYSQL_DBPORT));
  Set_MySQLDBCompress               (pXMLDoc->GetAttribLong(XMLTAG_DBDEFINITION_MYSQL_DBCOMPRESS) != 0);
  // MSADO
  Set_MSADODatabaseName             (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_MSADO_DATABASENAME));
  Set_MSADODBUserName               (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_MSADO_DBUSERNAME));
  Set_MSADODBPassword               (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_MSADO_DBPASSWORD));
  Set_MSADOProvider                 (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_MSADO_PROVIDER));
  // IMDB
  Set_IMDBSharedMemoryName          (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_IMDB_SHAREDMEMORYNAME));
  // DataProvider
  Set_DataProviderProviderName      (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_DATAPROVIDER_PROVIDERNAME));
  Set_DataProviderProviderSpecific  (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_DATAPROVIDER_PROVIDERSPECIFIC));
  // Firebird
  Set_FirebirdServerName            (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_FIREBIRD_SERVERNAME));
  Set_FirebirdDatabaseName          (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_FIREBIRD_DATABASENAME));
  Set_FirebirdUserName              (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_FIREBIRD_USERNAME));
  Set_FirebirdUserPassword          (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_FIREBIRD_USERPASSWORD));
  // XMLEngine
  Set_XMLEngineFolder               (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_XMLENGINE_FOLDER));
  Set_XMLEngineUseInMemoryDatabase  (pXMLDoc->GetAttribLong(XMLTAG_DBDEFINITION_XMLENGINE_USEINMEMORYDATABASE) != 0);
  Set_XMLEngineTempDatabaseFileName (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_XMLENGINE_TEMPDATABASEFILENAME));
  // SQLite
  Set_SQLiteDatabaseFileName        (pXMLDoc->GetAttrib(XMLTAG_DBDEFINITION_SQLITE_DATABASEFILENAME));

  return true;
}

bool CDBDefinition::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  // XMLDoc in <DBDefinition>

  // common
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_NAME,                            GetName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_DBTYPETOUSE,                     GetDBTypeToUse());
  // MySQL
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MYSQL_DATABASENAME,              Get_MySQLDatabaseName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MYSQL_DBUSERNAME,                Get_MySQLDBUserName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MYSQL_DBPASSWORD,                Get_MySQLDBPassword().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MYSQL_DBHOSTNAMEORIP,            Get_MySQLDBHostNameOrIP().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MYSQL_DBPORT,                    Get_MySQLDBPort());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MYSQL_DBCOMPRESS,                Get_MySQLDBCompress());
  // MSADO
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MSADO_DATABASENAME,              Get_MSADODatabaseName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MSADO_DBUSERNAME,                Get_MSADODBUserName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MSADO_DBPASSWORD,                Get_MSADODBPassword().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_MSADO_PROVIDER,                  Get_MSADOProvider().c_str());
  // IMDB
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_IMDB_SHAREDMEMORYNAME,           Get_IMDBSharedMemoryName().c_str());
  // DataProvider
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_DATAPROVIDER_PROVIDERNAME,       Get_DataProviderProviderName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_DATAPROVIDER_PROVIDERSPECIFIC,   Get_DataProviderProviderSpecific().c_str());
  // Firebird
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_FIREBIRD_SERVERNAME,             Get_FirebirdServerName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_FIREBIRD_DATABASENAME,           Get_FirebirdDatabaseName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_FIREBIRD_USERNAME,               Get_FirebirdUserName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_FIREBIRD_USERPASSWORD,           Get_FirebirdUserPassword().c_str());
  // XMLEngine
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_XMLENGINE_FOLDER,                Get_XMLEngineFolder().c_str());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_XMLENGINE_USEINMEMORYDATABASE,   Get_XMLEngineUseInMemoryDatabase());
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_XMLENGINE_TEMPDATABASEFILENAME,  Get_XMLEngineTempDatabaseFileName().c_str());
  // SQLite
  pXMLDoc->SetAttrib(XMLTAG_DBDEFINITION_SQLITE_DATABASEFILENAME,         Get_SQLiteDatabaseFileName().c_str());

  return true;
}

bool CDBDefinition::GetXMLDoc(xtstring& sXML)
{
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(STANDALONE_DBDEFINITION))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == STANDALONE_DBDEFINITION_TAG)
  {
    if (SaveXMLDoc(&cXMLDoc))
    {
      sXML = cXMLDoc.GetDoc();
      return true;
    }
  }
  return false;
}

bool CDBDefinition::SetXMLDoc(const xtstring& sXML)
{
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXML.c_str()))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == STANDALONE_DBDEFINITION_TAG)
    if (ReadXMLDoc(&cXMLDoc))
      return true;
  return false;
}
















CDBDefinitions::CDBDefinitions()
{
}
CDBDefinitions::~CDBDefinitions()
{
  RemoveAll();
}

bool CDBDefinitions::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  RemoveAll();
  // XMLDoc irgendwo
  if (pXMLDoc->FindChildElem(XMLTAG_DBDEFINITIONS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <DBDefinitions>
    while (pXMLDoc->FindChildElem(XMLTAG_DBDEFINITION))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <DBDefinition>
      CDBDefinition* pDBDef;
      pDBDef = new CDBDefinition;
      if (pDBDef)
      {
        if (pDBDef->ReadXMLDoc(pXMLDoc))
          m_arrDefinitions.push_back(pDBDef);
        else
          delete pDBDef;
      }
      pXMLDoc->OutOfElem();
      // XMLDoc in <DBDefinitions>
    }
    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }
  return true;
}

bool CDBDefinitions::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  // XMLDoc irgendwo
  if (pXMLDoc->AddChildElem(XMLTAG_DBDEFINITIONS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <DBDefinitions>
    for (size_t nI = 0; nI < CountOfDefinitions(); nI++)
    {
      if (pXMLDoc->AddChildElem(XMLTAG_DBDEFINITION, _XT("")))
      {
        pXMLDoc->IntoElem();

        m_arrDefinitions[nI]->SaveXMLDoc(pXMLDoc);

        pXMLDoc->OutOfElem();
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }
  return true;
}

bool CDBDefinitions::CopyFrom(CDBDefinitions* pDefs,
                              bool& bDefsChanged)
{
  if (!pDefs)
    return false;

  bDefsChanged = false;
  // zuerst kontrollieren, ob was verschwunden ist
  for (size_t nI = 0; nI < CountOfDefinitions(); nI++)
  {
    CDBDefinition* pMy = Get(nI);
    if (!pMy)
      continue;
    CDBDefinition* p = pDefs->Get(pMy->GetName());
    if (!p)
    {
      // existiert nicht mehr
      Remove(nI);
      nI--;
      bDefsChanged = true;
    }
  }

  // jetzt kopieren und kontrollieren
  for (size_t nI = 0; nI < pDefs->CountOfDefinitions(); nI++)
  {
    CDBDefinition* p = pDefs->Get(nI);
    if (!p)
      continue;
    CDBDefinition* pMy = Get(p->GetName());
    if (!pMy)
    {
      bDefsChanged = true;
      Add(p);
    }
    else
      pMy->CopyFrom(p, bDefsChanged);
  }
  return true;
}

bool CDBDefinitions::AssignFrom(CDBDefinitions* pDefs)
{
  if (!pDefs)
    return false;

  bool bRet = true;

  RemoveAll();
  for (size_t nI = 0; nI < pDefs->CountOfDefinitions(); nI++)
    bRet &= Add(pDefs->Get(nI));
  return bRet;
}

bool CDBDefinitions::GetAllSymbolicDBNames(CxtstringVector& cNames)
{
  for (size_t nI = 0; nI < m_arrDefinitions.size(); nI++)
    cNames.push_back(m_arrDefinitions[nI]->GetName());
  return true;
}

CDBDefinition* CDBDefinitions::Get(const xtstring& sName)
{
  for (size_t nI = 0; nI < m_arrDefinitions.size(); nI++)
  {
    if (m_arrDefinitions[nI]->GetName() == sName)
      return m_arrDefinitions[nI];
  }
  return 0;
}

CDBDefinition* CDBDefinitions::Get(size_t nIndex)
{
  if (nIndex < 0 || nIndex >= m_arrDefinitions.size())
    return 0;

  return m_arrDefinitions[nIndex];
}

bool CDBDefinitions::Add(CDBDefinition* pDefinition)
{
  CDBDefinition* pNew;
  pNew = new CDBDefinition;
  if (!pNew)
    return false;

  bool bDummy;
  if (!pNew->CopyFrom(pDefinition, bDummy))
  {
    delete pNew;
    return false;
  }

  size_t nCount = m_arrDefinitions.size();
  m_arrDefinitions.push_back(pNew);
  return (nCount != m_arrDefinitions.size());
}

bool CDBDefinitions::Add(CDBDefinition& cDefinition)
{
  CDBDefinition* pNew;
  pNew = new CDBDefinition;
  if (!pNew)
    return false;

  bool bDummy;
  if (!pNew->CopyFrom(&cDefinition, bDummy))
  {
    delete pNew;
    return false;
  }

  size_t nCount = m_arrDefinitions.size();
  m_arrDefinitions.push_back(pNew);
  return (nCount != m_arrDefinitions.size());
}

bool CDBDefinitions::Remove(const xtstring& sName)
{
  for (size_t nI = 0; nI < m_arrDefinitions.size(); nI++)
  {
    if (m_arrDefinitions[nI]->GetName() == sName)
    {
      delete m_arrDefinitions[nI];
      m_arrDefinitions.erase(m_arrDefinitions.begin() + nI);
      return true;
    }
  }
  return false;
}

bool CDBDefinitions::Remove(size_t nIndex)
{
  if (nIndex < 0 || nIndex >= m_arrDefinitions.size())
    return false;

  delete m_arrDefinitions[nIndex];
  m_arrDefinitions.erase(m_arrDefinitions.begin() + nIndex);
  return true;
}

void CDBDefinitions::RemoveAll()
{
  for (size_t nI = 0; nI < m_arrDefinitions.size(); nI++)
    delete m_arrDefinitions[nI];
  m_arrDefinitions.erase(m_arrDefinitions.begin(), m_arrDefinitions.end());
  return;
}
