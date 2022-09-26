#if !defined(_DBDEFINITIONS_H_)
#define _DBDEFINITIONS_H_


#include "ModelExpImp.h"
#include "ModelDef.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32

class CXPubMarkUp;

#define STANDALONE_DBDEFINITION       _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<StandAloneDBDefinition></StandAloneDBDefinition>\r\n")
#define STANDALONE_DBDEFINITION_TAG   _XT("StandAloneDBDefinition")

#define XMLTAG_DBDEFINITIONS                                _XT("DBDefinitions")
#define XMLTAG_DBDEFINITION                                 _XT("DBDefinition")
#define XMLTAG_DBDEFINITION_NAME                            _XT("Name")
#define XMLTAG_DBDEFINITION_DBTYPETOUSE                     _XT("DBTypeToUse")
#define XMLTAG_DBDEFINITION_MSADO_DATABASENAME              _XT("MSADO_DBName")
#define XMLTAG_DBDEFINITION_MSADO_DBUSERNAME                _XT("MSADO_DBUser")
#define XMLTAG_DBDEFINITION_MSADO_DBPASSWORD                _XT("MSADO_DBPassword")
#define XMLTAG_DBDEFINITION_MSADO_PROVIDER                  _XT("MSADO_DBProvider")
#define XMLTAG_DBDEFINITION_MYSQL_DATABASENAME              _XT("MySQL_DBName")
#define XMLTAG_DBDEFINITION_MYSQL_DBUSERNAME                _XT("MySQL_DBUser")
#define XMLTAG_DBDEFINITION_MYSQL_DBPASSWORD                _XT("MySQL_DBPassword")
#define XMLTAG_DBDEFINITION_MYSQL_DBHOSTNAMEORIP            _XT("MySQL_DBHostNameOrIP")
#define XMLTAG_DBDEFINITION_MYSQL_DBPORT                    _XT("MySQL_DBPort")
#define XMLTAG_DBDEFINITION_MYSQL_DBCOMPRESS                _XT("MySQL_DBCompress")
#define XMLTAG_DBDEFINITION_IMDB_SHAREDMEMORYNAME           _XT("IMDB_SharedMemoryName")
#define XMLTAG_DBDEFINITION_DATAPROVIDER_PROVIDERNAME       _XT("DataProvider_Provider")
#define XMLTAG_DBDEFINITION_DATAPROVIDER_PROVIDERSPECIFIC   _XT("DataProvider_ProviderSpecific")
#define XMLTAG_DBDEFINITION_FIREBIRD_SERVERNAME             _XT("Firebird_ServerName")
#define XMLTAG_DBDEFINITION_FIREBIRD_DATABASENAME           _XT("Firebird_DatabaseName")
#define XMLTAG_DBDEFINITION_FIREBIRD_USERNAME               _XT("Firebird_UserName")
#define XMLTAG_DBDEFINITION_FIREBIRD_USERPASSWORD           _XT("Firebird_UserPassword")
#define XMLTAG_DBDEFINITION_XMLENGINE_FOLDER                _XT("XMLEngine_Folder")
#define XMLTAG_DBDEFINITION_XMLENGINE_USEINMEMORYDATABASE   _XT("XMLEngine_UseInMemoryDatabase")
#define XMLTAG_DBDEFINITION_XMLENGINE_TEMPDATABASEFILENAME  _XT("XMLEngine_TempDatabaseFileName")
#define XMLTAG_DBDEFINITION_SQLITE_DATABASEFILENAME         _XT("SQLite_DatabaseFileName")


class XPUBMODEL_EXPORTIMPORT CDBDefinition
{
public:
  CDBDefinition();
  ~CDBDefinition();
  void ResetValues();

  bool CopyFrom(const CDBDefinition* pDBDefinition, bool& bValuesChanged);

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool  GetXMLDoc(xtstring& sXML);
  bool  SetXMLDoc(const xtstring& sXML);

  // common
  xtstring GetName() const                                      {return m_sName;};
  void SetName(const xtstring& sName)                           {m_sName = sName;};
  TDBDefinitionType GetDBTypeToUse() const                      {return m_nDBTypeToUse;};
  void SetDBTypeToUse(TDBDefinitionType nDBTypeToUse)           {m_nDBTypeToUse = nDBTypeToUse;};

  // MySQL
  xtstring Get_MySQLDatabaseName() const                        {return m_MySQL_sDatabaseName;};
  void Set_MySQLDatabaseName(const xtstring& sText)             {m_MySQL_sDatabaseName = sText;};
  xtstring Get_MySQLDBUserName() const                          {return m_MySQL_sDBUserName;};
  void Set_MySQLDBUserName(const xtstring& sText)               {m_MySQL_sDBUserName = sText;};
  xtstring Get_MySQLDBPassword() const                          {return m_MySQL_sDBPassword;};
  void Set_MySQLDBPassword(const xtstring& sText)               {m_MySQL_sDBPassword = sText;};
  xtstring Get_MySQLDBHostNameOrIP() const                      {return m_MySQL_sDBHostNameOrIP;};
  void Set_MySQLDBHostNameOrIP(const xtstring& sText)           {m_MySQL_sDBHostNameOrIP = sText;};
  unsigned int Get_MySQLDBPort() const                          {return m_MySQL_nDBPort;};
  void Set_MySQLDBPort(unsigned int nDBPort)                    {m_MySQL_nDBPort = nDBPort;};
  bool Get_MySQLDBCompress() const                              {return m_MySQL_bDBCompress;};
  void Set_MySQLDBCompress(bool bDBCompress)                    {m_MySQL_bDBCompress = bDBCompress;};
  // MSADO
  xtstring Get_MSADODatabaseName() const                        {return m_MSADO_sDatabaseName;};
  void Set_MSADODatabaseName(const xtstring& sText)             {m_MSADO_sDatabaseName = sText;};
  xtstring Get_MSADODBUserName() const                          {return m_MSADO_sDBUserName;};
  void Set_MSADODBUserName(const xtstring& sText)               {m_MSADO_sDBUserName = sText;};
  xtstring Get_MSADODBPassword() const                          {return m_MSADO_sDBPassword;};
  void Set_MSADODBPassword(const xtstring& sText)               {m_MSADO_sDBPassword = sText;};
  xtstring Get_MSADOProvider() const                            {return m_MSADO_sProvider;};
  void Set_MSADOProvider(const xtstring& sText)                 {m_MSADO_sProvider = sText;};
  // IMDB
  xtstring Get_IMDBSharedMemoryName() const                     {return m_IMDB_sSharedMemoryName;};
  void Set_IMDBSharedMemoryName(const xtstring& sText)          {m_IMDB_sSharedMemoryName = sText;};
  // DataProvider
  xtstring Get_DataProviderProviderName() const                 {return m_DataProvider_sProviderName;};
  void Set_DataProviderProviderName(const xtstring& sText)      {m_DataProvider_sProviderName = sText;};
  xtstring Get_DataProviderProviderSpecific() const             {return m_DataProvider_sProviderSpecific;};
  void Set_DataProviderProviderSpecific(const xtstring& sText)  {m_DataProvider_sProviderSpecific = sText;};
  // Firebird
  xtstring Get_FirebirdServerName() const                       {return m_Firebird_sServerName;};
  void Set_FirebirdServerName(const xtstring& sText)            {m_Firebird_sServerName = sText;};
  xtstring Get_FirebirdDatabaseName() const                     {return m_Firebird_sDatabaseName;};
  void Set_FirebirdDatabaseName(const xtstring& sText)          {m_Firebird_sDatabaseName = sText;};
  xtstring Get_FirebirdUserName()                               {return m_Firebird_sUserName;};
  void Set_FirebirdUserName(const xtstring& sText)              {m_Firebird_sUserName = sText;};
  xtstring Get_FirebirdUserPassword()                           {return m_Firebird_sUserPassword;}
  void Set_FirebirdUserPassword(const xtstring& sText)          {m_Firebird_sUserPassword = sText;};
  // XMLEngine
  xtstring Get_XMLEngineFolder() const                          {return m_XMLEngine_sFolder;};
  void Set_XMLEngineFolder(const xtstring& sText)               {m_XMLEngine_sFolder = sText;};
  bool Get_XMLEngineUseInMemoryDatabase() const                   {return m_XMLEngine_bUseInMemoryDatabase;};
  void Set_XMLEngineUseInMemoryDatabase(bool bValue)              {m_XMLEngine_bUseInMemoryDatabase = bValue;};
  xtstring Get_XMLEngineTempDatabaseFileName() const             {return m_XMLEngine_sTempDatabaseFileName;};
  void Set_XMLEngineTempDatabaseFileName(const xtstring& sText)  {m_XMLEngine_sTempDatabaseFileName = sText;};
  // SQLite
  xtstring Get_SQLiteDatabaseFileName()                         {return m_SQLite_sDatabaseFileName;};
  void Set_SQLiteDatabaseFileName(const xtstring& sText)        {m_SQLite_sDatabaseFileName = sText;};

protected:

  // common
  xtstring            m_sName;
  TDBDefinitionType   m_nDBTypeToUse;

  // MySQL
  xtstring      m_MySQL_sDatabaseName;
  xtstring      m_MySQL_sDBUserName;
  xtstring      m_MySQL_sDBPassword;
  xtstring      m_MySQL_sDBHostNameOrIP;
  unsigned int  m_MySQL_nDBPort;
  bool          m_MySQL_bDBCompress;
  // MSADO
  xtstring      m_MSADO_sDatabaseName;
  xtstring      m_MSADO_sDBUserName;
  xtstring      m_MSADO_sDBPassword;
  xtstring      m_MSADO_sProvider;
  // IMDB
  xtstring      m_IMDB_sSharedMemoryName;
  // DataProvider
  xtstring      m_DataProvider_sProviderName;
  xtstring      m_DataProvider_sProviderSpecific;
  // Firebird
  xtstring      m_Firebird_sServerName;
  xtstring      m_Firebird_sDatabaseName;
  xtstring      m_Firebird_sUserName;
  xtstring      m_Firebird_sUserPassword;
  // XMLEngine
  xtstring      m_XMLEngine_sFolder;
  bool          m_XMLEngine_bUseInMemoryDatabase;
  xtstring      m_XMLEngine_sTempDatabaseFileName;
  // SQLite
  xtstring      m_SQLite_sDatabaseFileName;
};


typedef std::vector<CDBDefinition*>             CDBDefinitionArray;
typedef CDBDefinitionArray::iterator            CDBDefinitionArrayIterator;
typedef CDBDefinitionArray::const_iterator      CDBDefinitionArrayConstIterator;
typedef CDBDefinitionArray::reverse_iterator    CDBDefinitionArrayReverseIterator;

class XPUBMODEL_EXPORTIMPORT CDBDefinitions
{
public:
  CDBDefinitions();
  ~CDBDefinitions();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool CopyFrom(CDBDefinitions* pDefs, bool& bDefsChanged);
  bool AssignFrom(CDBDefinitions* pDefs);
  bool GetAllSymbolicDBNames(CxtstringVector& cNames);

  size_t CountOfDefinitions(){return m_arrDefinitions.size();};
  CDBDefinition* Get(const xtstring& sName);
  CDBDefinition* Get(size_t nIndex);
  bool Add(CDBDefinition* pDefinition);
  bool Add(CDBDefinition& cDefinition);
  bool Remove(const xtstring& sName);
  bool Remove(size_t nIndex);
  void RemoveAll();
protected:
  CDBDefinitionArray m_arrDefinitions;
};

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_DBDEFINITIONS_H_)
