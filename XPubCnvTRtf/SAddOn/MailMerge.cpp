#include "StdAfx.h"

#include "SSTLdef/STLdef.h"

#include "mailmerge.h"
#include "Shlwapi.h"
#include "Shlobj.h"
#include "Registry.h"

inline void TestHR(HRESULT x) {if FAILED(x) _com_issue_error(x);};

#define INSERTION_TIME "_InsDateTime"

sCnnList CMailMerge::m_sCnnOpen;

CMailMerge::CMailMerge(void)
{
	m_pCatalog = NULL;
}

CMailMerge::~CMailMerge(void)
{
	m_pCatalog = NULL;

	if(m_TabFields.size())
		m_TabFields.erase(m_TabFields.begin(), m_TabFields.end());

}

void CMailMerge::DetachStatic()
{
	sCnnListIterator::const_iterator iterCnnOpen = m_sCnnOpen.begin();
	while( iterCnnOpen != m_sCnnOpen.end() )
	{
		try
		{
			ADODB::_ConnectionPtr pCnn = (*iterCnnOpen).second;
			pCnn->Close();
			pCnn = NULL;
		}
		catch(_com_error &e)
		{
			// Notify the user of errors if any.
			_bstr_t bstrSource(e.Source());
			_bstr_t bstrDescription(e.Description());

			TRACE("\n\tSource :  %s \n\tdescription : %s \n ",(LPCSTR)bstrSource,(LPCSTR)bstrDescription);
		}
		catch(...)
		{
			TRACE("Error occured in include files....");
		}
		++iterCnnOpen;
	}
	if(m_sCnnOpen.size())
		m_sCnnOpen.erase(m_sCnnOpen.begin(), m_sCnnOpen.end());

}

BOOL CMailMerge::AddField(const xtstringex& sName, const xtstringex& sValue, ADOX::DataTypeEnum eDataType, int nLength)
{
	CField cField;
	cField.AddField(sName, sValue, eDataType, nLength);
	m_TabFields.insert(m_TabFields.end(), cField);

	return TRUE;
}

BOOL CMailMerge::AddTable(xtstringex& sTableName)
{
	BOOL bReturn = TRUE;
	if(CreateTable(sTableName))
	{
		ADODB::_RecordsetPtr m_pRecSet;
		try
		{
			HRESULT hr;
			TestHR(hr = m_pRecSet.CreateInstance(__uuidof(ADODB::Recordset)));
			CComVariant cTableName = CString("[") +sTableName.c_str()+ CString("]");
			m_pRecSet->Open(cTableName, m_pCatalog->GetActiveConnection(), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdTable);

			m_pRecSet->AddNew();

			COleDateTime cDate(COleDateTime::GetCurrentTime());
			CComVariant cComV(cDate);
			m_pRecSet->Fields->GetItem(INSERTION_TIME)->Value = cComV;

			list<CField>::iterator entry = m_TabFields.begin();
			while( entry != m_TabFields.end() )
			{
				m_pRecSet->Fields->GetItem(entry->GetName().c_str())->Value = (_bstr_t) entry->GetValue().c_str();
				entry++;		
			}
			m_pRecSet->Update();
			m_pRecSet->Close();

		}
		catch(_com_error &e)
		{
			// Notify the user of errors if any.
			_bstr_t bstrSource(e.Source());
			_bstr_t bstrDescription(e.Description());

			TRACE("\n\tSource :  %s \n\tdescription : %s \n ",(LPCSTR)bstrSource,(LPCSTR)bstrDescription);
			bReturn = FALSE;
		}
		catch(...)
		{
			TRACE("Error occured in include files....");
			bReturn = FALSE;
		}
	}

	return bReturn;
}

BOOL CMailMerge::DeleteTable(xtstringex& sTable)
{  
	BOOL bReturn = TRUE;
	
	try
	{	
		m_pCatalog->Tables->Delete(sTable.c_str());
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		TRACE("\n\tSource :  %s \n\tdescription : %s \n ",(LPCSTR)bstrSource,(LPCSTR)bstrDescription);
	}
	catch(...)
	{
		TRACE("Error occured in include files....");
	}
	
	return bReturn;
}

BOOL CMailMerge::CreateTable(xtstringex& sTable)
{  
	BOOL bReturn = TRUE;
	try
	{
  // Tabelle schon vorhanden?
		CComBSTR cCmBstr;
		int nTabCnt = m_pCatalog->Tables->Count;
		for(int i =0; i < nTabCnt; i++)
		{
			_variant_t vIndex = (short) i;
			m_pCatalog->Tables->GetItem(vIndex)->get_Name(&cCmBstr);
			if(CString(sTable.c_str()) == cCmBstr.m_str) return TRUE;
		}

	// Create and append new Table object to the atabase.
		HRESULT hr = S_OK;
		ADOX::_TablePtr pTblNew;
		TestHR(hr = pTblNew.CreateInstance(__uuidof(ADOX::Table)));

		pTblNew->Name = sTable.c_str();
		pTblNew->ParentCatalog = m_pCatalog;
	
/////////////////////////////////////////////////////////////////////////////
// wir fügen einen automatischen Index ein

#define INDEX_NAME "_Nummer"

		pTblNew->Columns->Append(INDEX_NAME, ADOX::adInteger,0);
		pTblNew->Columns->GetItem(INDEX_NAME)->Properties->
									GetItem("AutoIncrement")->Value = true;

		pTblNew->Columns->GetItem(INDEX_NAME)->Properties->
			GetItem("Description")->Value = _T("dieses Feld wurde vom XPublisher eingefügt");


		// Append the primary key.
		ADOX::_IndexPtr pIndex  = NULL;

		TestHR(hr = pIndex.CreateInstance(__uuidof(ADOX::Index)));
		pIndex->Name = INDEX_NAME;
		pIndex->Columns->Append(INDEX_NAME,ADOX::adInteger,0);
		pIndex->PutPrimaryKey(-1);
		pIndex->PutUnique(-1);


		pTblNew->Indexes->Append(_variant_t ((IDispatch*)pIndex));
		pIndex  = NULL;

/////////////////////////////////////////////////////////////////////////////
  // Feld über das Einfügezeitpunkt definiert wird
		pTblNew->Columns->Append(INSERTION_TIME, ADOX::adDate,0);
		pTblNew->Columns->GetItem(INSERTION_TIME)->Properties->
			GetItem("Description")->Value = _T("gibt an wann der Datensatz eingefügt wurde");

		/////////////////////////////////////////////////////////////////////////////

	// Spalten
		list<CField>::iterator entry = m_TabFields.begin();
		while( entry != m_TabFields.end() )
		{
			xtstring c = entry->GetName();
			pTblNew->Columns->Append(entry->GetName().c_str(), entry->GetDataType(), entry->GetLength());
			pTblNew->Columns->GetItem(entry->GetName().c_str())->Properties->
				GetItem("Nullable")->Value = true;
     // auf jedes Feld Index "(Duplikate möglich)"
			pTblNew->Indexes->Append(entry->GetName().c_str(), entry->GetName().c_str());
			entry++;		
		}

		m_pCatalog->Tables->Append(_variant_t((IDispatch*)pTblNew));
		m_pCatalog->Tables->Refresh();

		pTblNew = NULL;
		pIndex  = NULL;
	
	}   
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		TRACE("\n\tSource :  %s \n\tdescription : %s \n ",(LPCSTR)bstrSource,(LPCSTR)bstrDescription);
		 bReturn = FALSE;
	}
	catch(...)
	{
		TRACE("Error occured in include files....");
		bReturn = FALSE;
	}
	return bReturn;

}

BOOL CMailMerge::GetDefaultMdbPath(xtstringex& sMdbPath)
{
	CString cPersonal;
	CRegistry regLK;

	// wir setzen einen Defaultpfad für die mdb-Dateien 
	if (regLK.Connect(CRegistry::hKeyCurrentUser) && 
		regLK.Open(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders"), CRegistry::permissionRead))
	{
		if(regLK.GetStringValue(_T("Personal"), cPersonal))
		{
			if (cPersonal[cPersonal.GetLength() - 1] != _T('\\'))
				cPersonal += _T('\\');
		}
		regLK.Close();

		if (regLK.Connect(CRegistry::hKeyCurrentUser) && 
			regLK.Open(_T("Software\\Microsoft\\Office\\10.0\\Common\\DataServices"), CRegistry::permissionRead))
		{
			CString cMyData;
			if(regLK.GetStringValue(_T("MyData"), cMyData))
			{
				if (cMyData[cMyData.GetLength() - 1] != _T('\\'))
					cMyData += _T('\\');
			}
			cPersonal += cMyData;
		}
		else
		if (regLK.Connect(CRegistry::hKeyCurrentUser) && 
			regLK.Open(_T("Software\\Microsoft\\Office\\11.0\\Common\\DataServices"), CRegistry::permissionRead))
		{
			CString cMyData;
			if(regLK.GetStringValue(_T("MyData"), cMyData))
			{
				if (cMyData[cMyData.GetLength() - 1] != _T('\\'))
					cMyData += _T('\\');
			}
			cPersonal += cMyData;
		}
	
		CString cExpandedStr;

	#define MAX_PERSONAL_LENGTH 500

  // %USERPROFILE% expandieren
		::ExpandEnvironmentStrings(
			(LPCSTR)cPersonal, // string with environment variables
			cExpandedStr.GetBufferSetLength(MAX_PERSONAL_LENGTH),  // string with expanded strings 
			MAX_PERSONAL_LENGTH    // maximum characters in expanded string
			);

		cExpandedStr.ReleaseBuffer();
		sMdbPath = (LPCSTR)cExpandedStr;
		return TRUE;
	
	}

	return FALSE;
}

xtstring CMailMerge::GetPureMDBPathName(const xtstring& sDataSource)
{
	#define ADO_DATASOURCE "Data source="

	if (sDataSource.size() <= 0)
  {
    ASSERT(FALSE);
		return _XT("");
  }

	int nPos;
	xtstringex sPureMdbPathName = sDataSource;
	if((nPos=sPureMdbPathName.FindNoCase(ADO_DATASOURCE)) != -1)
	{				
		sPureMdbPathName = sPureMdbPathName.Mid(nPos+strlen(ADO_DATASOURCE));
		sPureMdbPathName.TrimRight(">");
		sPureMdbPathName.Trim("\"");
	}
	char drive[_MAX_DRIVE]={'\0'};char ext[_MAX_EXT]={'\0'};
	char dir[_MAX_DIR]={'\0'}; char fname[_MAX_FNAME]={'\0'};

// wenn kein Pfad in Datasource gesetzt ist, dann gehen wir auf "Personal"
	_splitpath( sPureMdbPathName.c_str(), drive, dir, fname, ext );
	if(*drive == '\0' && *dir == '\0')
	{
		xtstringex sPersonalPath;
		GetDefaultMdbPath(sPersonalPath);
		sPureMdbPathName = sPersonalPath + sPureMdbPathName;
	}
  return sPureMdbPathName;
}

BOOL CMailMerge::CheckDatabase(xtstringex& sDataSource)
{
  xtstring sPureMdbPathName;
  sPureMdbPathName = GetPureMDBPathName(sDataSource);
//  if (sPureMdbPathName.size())
//    return TRUE;
	sDataSource = ADO_DATASOURCE + sPureMdbPathName;
	return ::PathFileExists(sPureMdbPathName.c_str());
}

BOOL CMailMerge::CreateDatabase(const xtstringex& sProvider, const xtstringex& sDataSource)
{
	BOOL bReturn = TRUE;
	try
	{
		HRESULT hr = S_OK;
		TestHR(hr = m_pCatalog.CreateInstance(__uuidof (ADOX::Catalog)));
		_bstr_t strcnn( (sProvider+sDataSource+";Jet OLEDB:Engine Type=5").c_str() );
		m_pCatalog->Create(strcnn);
		m_pCatalog = NULL;
  // Datenbank regulär öffnen
		CheckIfAlreadyOpen(sProvider, sDataSource);
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		TRACE("\n\tSource :  %s \n\tdescription : %s \n ",(LPCSTR)bstrSource,(LPCSTR)bstrDescription);
		bReturn = FALSE;
	}
	catch(...)
	{
		TRACE("Error occured in include files....");
		bReturn = FALSE;
	}
	return bReturn;

}

BOOL CMailMerge::CheckIfAlreadyOpen(const xtstringex& sProvider, const xtstringex& sDataSource)
{

	sCnnListIterator::const_iterator iterCnnOpen = m_sCnnOpen.find( sDataSource );
	if ( iterCnnOpen != m_sCnnOpen.end())
	{
		return TRUE;
	}
	else
	{
		HRESULT hr = S_OK;
		ADODB::_ConnectionPtr cCnn;
		try
		{
			TestHR(hr = cCnn.CreateInstance(__uuidof(ADODB::Connection)));
			cCnn->Open(_bstr_t((sProvider+sDataSource).c_str()), "", "", NULL);
			m_sCnnOpen[sDataSource] = cCnn;	
		}
		catch(_com_error &e)
		{
			// Notify the user of errors if any.
			_bstr_t bstrSource(e.Source());
			_bstr_t bstrDescription(e.Description());

			TRACE("\n\tSource :  %s \n\tdescription : %s \n ",(LPCSTR)bstrSource,(LPCSTR)bstrDescription);
		}
		catch(...)
		{
			TRACE("Error occured in include files....");
		}
	}
	return FALSE;

}

BOOL CMailMerge::OpenDatabase(const xtstringex& sProvider, const xtstringex& sDataSource)
{   
	HRESULT hr = S_OK;
	try
	{
		if(m_pCatalog == NULL)
			TestHR(hr = m_pCatalog.CreateInstance(__uuidof (ADOX::Catalog)));
		
		xtstringex sMdbSource = sDataSource;
		if(!CheckDatabase(sMdbSource))
		{
			_bstr_t strcnn( (sProvider+sMdbSource).c_str() );
			m_pCatalog->Create(strcnn);
		}
		else
		{
			m_pCatalog->PutActiveConnection(_variant_t((IDispatch *) m_sCnnOpen[sDataSource]));
			//_bstr_t strcnn( (sProvider+sMdbSource).c_str() );
			//m_pCatalog->PutActiveConnection(strcnn);
		}
	}   
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		TRACE("\n\tSource :  %s \n\tdescription : %s \n ",(LPCSTR)bstrSource,(LPCSTR)bstrDescription);

	}
	catch(...)
	{
		TRACE("Error occured in include files....");
	}
	return TRUE;
}
