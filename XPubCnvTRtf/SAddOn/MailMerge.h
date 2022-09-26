#pragma once
 
class CField
{
 public:

	CField(){};
	virtual ~CField(){};
	void AddField(const xtstringex& sName, const xtstringex& sValue, ADOX::DataTypeEnum eDataType, int nLength)
	{ m_sName = sName; m_eDataType = eDataType; m_nLength = nLength; m_sValue = sValue; };

	xtstringex& GetName(){ return m_sName;  };
	xtstringex& GetValue(){ return m_sValue; };
	ADOX::DataTypeEnum GetDataType(){ return m_eDataType; };
	int GetLength(){ return m_nLength; };
	
protected:

	xtstringex m_sName;
	xtstringex m_sValue;
	
	ADOX::DataTypeEnum m_eDataType;
	int m_nLength;

};

typedef std::list<CField> TableFields;
typedef std::map<xtstringex, ADODB::_ConnectionPtr> sCnnList;
typedef sCnnList::iterator sCnnListIterator;

class CMailMerge 
{
public:

	CMailMerge(void);
	virtual ~CMailMerge(void);

public:

	BOOL OpenDatabase(const xtstringex& sProvider, const xtstringex& sDataSource);
	BOOL CheckDatabase(xtstringex& sDataSource);
	
	BOOL AddField(const xtstringex& sName, const xtstringex& sValue, ADOX::DataTypeEnum eDataType, int nLength);
	BOOL AddTable(xtstringex& sTableName);
	BOOL CreateTable(xtstringex& sTable);
	BOOL DeleteTable(xtstringex& sTable);
	BOOL CheckIfAlreadyOpen(const xtstringex& sProvider, const xtstringex& sDataSource);
	BOOL CreateDatabase(const xtstringex& sProvider, const xtstringex& sDataSource);

	BOOL AddRecord();

	static void DetachStatic();

  xtstring GetPureMDBPathName(const xtstring& sDataSource);
protected:

	BOOL GetDefaultMdbPath(xtstringex& sMdbPath);

protected:

	ADOX::_CatalogPtr m_pCatalog;		// um Datenbankstruktur zu modifizieren	
	TableFields m_TabFields;
	static sCnnList m_sCnnOpen;			// statische Liste aller bereits geöffneter MDB's

};
