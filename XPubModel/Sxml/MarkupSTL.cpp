// MarkupSTL.cpp: implementation of the CMarkupSTL class.
//
// Markup Release 8.1
// Copyright (C) 1999-2005 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "MarkupSTL.h"

using namespace std;

// Customization
#define x_EOL "\r\n" // can be \r\n or \n or empty
#define x_EOLLEN (sizeof(x_EOL)-1) // string length of x_EOL
#define x_ATTRIBQUOTE "\"" // can be double or single quote

//[CMARKUPDEV
// To add a version tag to new documents, define MARKUP_VERSIONTAG
#ifdef MARKUP_VERSIONTAG
#define x_VERSIONTAG "<?xml version=\"1.0\"?>"
#endif
//]CMARKUPDEV

void CMarkupSTL::operator=( const CMarkupSTL& markup )
{
	m_iPosParent = markup.m_iPosParent;
	m_iPos = markup.m_iPos;
	m_iPosChild = markup.m_iPosChild;
	m_iPosFree = markup.m_iPosFree;
	m_iPosDeleted = markup.m_iPosDeleted;
	m_nNodeType = markup.m_nNodeType;
	m_nNodeOffset = markup.m_nNodeOffset;
	m_nNodeLength = markup.m_nNodeLength;
	m_strDoc = markup.m_strDoc;
	m_strError = markup.m_strError;
	m_nFlags = markup.m_nFlags;

	// Copy used part of the index array
	m_aPos.RemoveAll();
	m_aPos.nSize = m_iPosFree;
	if ( m_aPos.nSize < 8 )
		m_aPos.nSize = 8;
	m_aPos.nSegs = m_aPos.SegsUsed();
	if ( m_aPos.nSegs )
	{
		m_aPos.pSegs = (ElemPos**)(new char[m_aPos.nSegs*sizeof(char*)]);
		int nSegSize = 1 << m_aPos.PA_SEGBITS;
		for ( int nSeg=0; nSeg < m_aPos.nSegs; ++nSeg )
		{
			if ( nSeg + 1 == m_aPos.nSegs )
				nSegSize = m_aPos.GetSize() - (nSeg << m_aPos.PA_SEGBITS);
			m_aPos.pSegs[nSeg] = (ElemPos*)(new char[nSegSize*sizeof(ElemPos)]);
			memcpy( m_aPos.pSegs[nSeg], markup.m_aPos.pSegs[nSeg], nSegSize*sizeof(ElemPos) );
		}
	}

	// Copy SavedPos map
	m_mapSavedPos.RemoveAll();
	if ( markup.m_mapSavedPos.pTable )
	{
		m_mapSavedPos.AllocMapTable();
		for ( int nSlot=0; nSlot < SavedPosMap::SPM_SIZE; ++nSlot )
		{
			SavedPos* pCopySavedPos = markup.m_mapSavedPos.pTable[nSlot];
			if ( pCopySavedPos )
			{
				int nCount = 0;
				while ( pCopySavedPos[nCount].nSavedPosFlags & SavedPosMap::SPM_USED )
				{
					++nCount;
					if ( pCopySavedPos[nCount-1].nSavedPosFlags & SavedPosMap::SPM_LAST )
						break;
				}
				SavedPos* pNewSavedPos = new SavedPos[nCount];
				for ( int nCopy=0; nCopy<nCount; ++nCopy )
					pNewSavedPos[nCopy] = pCopySavedPos[nCopy];
				pNewSavedPos[nCount-1].nSavedPosFlags |= SavedPosMap::SPM_LAST;
				m_mapSavedPos.pTable[nSlot] = pNewSavedPos;
			}
		}
	}

	MARKUP_SETDEBUGSTATE;
}

bool CMarkupSTL::SetDoc( const char* szDoc )
{
	// Set document text
	if ( szDoc )
		m_strDoc = szDoc;
	else
		m_strDoc.erase();

	m_strError.erase();
	return x_ParseDoc();
};

bool CMarkupSTL::IsWellFormed()
{
	if ( m_aPos.GetSize()
			&& ! (m_aPos[0].nFlags & MNF_ILLFORMED)
			&& m_aPos[0].iElemChild
			&& ! m_aPos[m_aPos[0].iElemChild].iElemNext )
		return true;
	return false;
}

bool CMarkupSTL::Load( const char* szFileName )
{
	if ( ! ReadTextFile(szFileName, m_strDoc, &m_strError, &m_nFlags) )
		return false;
	return x_ParseDoc();
}

bool CMarkupSTL::ReadTextFile( const char* szFileName, string& strDoc, string* pstrError, int* pnFlags )
{
	// Static utility method to load text file into strDoc
	//
	// Open file to read binary
	FILE* fp = fopen( szFileName, "rb" );
	if ( ! fp )
	{
		if ( pstrError )
			*pstrError = strerror(errno);
		return false;
	}

	// Set flags to 0 unless flags argument provided
	int nFlags = pnFlags?*pnFlags:0;
	char szDescBOM[20] = {0};
	char szResult[100];
	strDoc.erase();

	// Get file length
	fseek( fp, 0, SEEK_END );
	int nFileByteLen = ftell(fp);
	fseek( fp, 0, SEEK_SET );

	//[CMARKUPDEV
	// Check for Byte Order Mark (preamble)
	// FFFE is the usual Windows BOM, the little endian "LE" in UCS-2LE and UTF-16LE
	// FEFF is also possible but not handled here
	// EFBBBF if the UTF-8 BOM
	unsigned char szBOM[3] = {0,0,0};
	int nReadPreamble = 3;
	if ( nFileByteLen < nReadPreamble )
		nReadPreamble = nFileByteLen;
	if ( nReadPreamble )
		fread( szBOM, nReadPreamble, 1, fp );
	if ( szBOM[0] == 0xff && szBOM[1] == 0xfe )
	{
		nFlags |= MDF_UTF16LEFILE;
		strcpy( szDescBOM, "UTF-16LE BOM + " );
		nFileByteLen -= 2;
		if ( nReadPreamble > 2 )
			fseek( fp, 2, SEEK_SET );
		int nWideLen = nFileByteLen / 2;
		int nMBLen = 0;
		if ( nFileByteLen )
		{
			wchar_t* pUTF16Buffer = new wchar_t[nWideLen+1];
			fread( pUTF16Buffer, nFileByteLen, 1, fp );
			pUTF16Buffer[nWideLen] = (wchar_t)0;
			// For ANSI files, replace both UTF16To8 calls with wcstombs (arguments are the same)
			nMBLen = UTF16To8(NULL,pUTF16Buffer,0);
			char* pszBuffer = new char[nMBLen+1];
			UTF16To8(pszBuffer,pUTF16Buffer,nMBLen);
			delete [] pUTF16Buffer;
			strDoc.assign( pszBuffer, nMBLen );
			delete [] pszBuffer;
		}
		sprintf( szResult, "%s%d wide chars to %d bytes", szDescBOM, nWideLen, nMBLen );
		if ( pstrError )
			*pstrError = szResult;
		fclose( fp );
		if ( pnFlags )
			*pnFlags = nFlags;
		return true;
	}
	else if ( szBOM[0] == 0xef && szBOM[1] == 0xbb && szBOM[2] == 0xbf )
	{
		nFlags |= MDF_UTF8PREAMBLE;
		strcpy( szDescBOM, "UTF-8 BOM + " );
		nFileByteLen -= 3;
	}
	else
	{
		nFlags &= ~MDF_UTF16LEFILE;
		nFlags &= ~MDF_UTF8PREAMBLE;
		fseek( fp, 0, SEEK_SET );
	}
	//]CMARKUPDEV

	// Read file directly
	if ( nFileByteLen )
	{
		char* pszBuffer = new char[nFileByteLen];
		fread( pszBuffer, nFileByteLen, 1, fp );
		strDoc.assign( pszBuffer, nFileByteLen );
		delete [] pszBuffer;
	}
	sprintf( szResult, "%s%d bytes", szDescBOM, nFileByteLen );
	if ( pstrError )
		*pstrError = szResult;

	fclose( fp );
	if ( pnFlags )
		*pnFlags = nFlags;
	return true;
}

bool CMarkupSTL::Save( const char* szFileName )
{
	return WriteTextFile( szFileName, m_strDoc, &m_strError, &m_nFlags );
}

bool CMarkupSTL::WriteTextFile( const char* szFileName, string& strDoc, string* pstrError, int* pnFlags )
{
	// Static utility method to save strDoc to text file
	//
	// Open file to write binary
	bool bSuccess = true;
	FILE* fp = fopen( szFileName, "wb" );
	if ( ! fp )
	{
		if ( pstrError )
			*pstrError = strerror(errno);
		return false;
	}

	// Set flags to 0 unless flags argument provided
	int nFlags = pnFlags?*pnFlags:0;
	char szDescBOM[20] = {0};
	char szResult[100];

	// Get document length
	int nDocLength = (int)strDoc.size();

	//[CMARKUPDEV
	if ( nFlags & MDF_UTF16LEFILE )
	{
		const char* pszDoc = strDoc.c_str();
		unsigned char szBOM[2] = {0xff,0xfe};
		if ( fwrite( szBOM, 2, 1, fp ) == 1 )
		{
			strcpy( szDescBOM, "UTF-16LE BOM + " );
			int nWideLen = 0;
			if ( nDocLength )
			{
				nWideLen = UTF8To16(NULL,pszDoc,nDocLength);
				wchar_t* pBuffer = new wchar_t[nWideLen+1];
				UTF8To16(pBuffer,pszDoc,nDocLength);
				bSuccess = ( fwrite( pBuffer, nWideLen*2, 1, fp ) == 1 );
				delete [] pBuffer;
			}
			sprintf( szResult, "%d bytes to %s%d wide chars", nDocLength, szDescBOM, nWideLen );
			if ( pstrError )
				*pstrError = szResult;
		}
		if ( ! bSuccess && pstrError )
			*pstrError = strerror(errno);
		fclose(fp);
		if ( pnFlags )
			*pnFlags = nFlags;
		return bSuccess;
	}
	else if ( nFlags & MDF_UTF8PREAMBLE )
	{
		unsigned char szBOM[3] = {0xef,0xbb,0xbf};
		if ( fwrite( szBOM, 3, 1, fp ) == 1 )
			strcpy( szDescBOM, "UTF-8 BOM + " );
	}
	//]CMARKUPDEV

	if ( nDocLength )
		bSuccess = ( fwrite( strDoc.c_str(), nDocLength, 1, fp ) == 1 );
	sprintf( szResult, "%s%d bytes", szDescBOM, nDocLength );
	if ( pstrError )
		*pstrError = szResult;
	
	if ( ! bSuccess && pstrError )
		*pstrError = strerror(errno);
	fclose(fp);
	if ( pnFlags )
		*pnFlags = nFlags;
	return bSuccess;
}

bool CMarkupSTL::FindElem( const char* szName )
{
	// Change current position only if found
	//
	if ( m_aPos.GetSize() )
	{
		int iPos = x_FindElem( m_iPosParent, m_iPos, szName );
		if ( iPos )
		{
			// Assign new position
			x_SetPos( m_aPos[iPos].iElemParent, iPos, 0 );
			return true;
		}
	}
	return false;
}

bool CMarkupSTL::FindChildElem( const char* szName )
{
	// Change current child position only if found
	//
	// Shorthand: call this with no current main position
	// means find child under root element
	if ( ! m_iPos )
		FindElem();

	int iPosChild = x_FindElem( m_iPos, m_iPosChild, szName );
	if ( iPosChild )
	{
		// Assign new position
		int iPos = m_aPos[iPosChild].iElemParent;
		x_SetPos( m_aPos[iPos].iElemParent, iPos, iPosChild );
		return true;
	}

	return false;
}

string CMarkupSTL::EscapeText( const char* szText, int nFlags )
{
	// Convert text as seen outside XML document to XML friendly
	// replacing special characters with ampersand escape codes
	// E.g. convert "6>7" to "6&gt;7"
	//
	// &lt;   less than
	// &amp;  ampersand
	// &gt;   greater than
	//
	// and for attributes:
	//
	// &apos; apostrophe or single quote
	// &quot; double quote
	//
	static const char* szaReplace[] = { "&lt;","&amp;","&gt;","&apos;","&quot;" };
	const char* pFind = (nFlags&MNF_ESCAPEQUOTES)?"<&>\'\"":"<&>";
	string strText;
	const char* pSource = szText;
	int nDestSize = (int)strlen(pSource);
	nDestSize += nDestSize / 10 + 7;
	strText.reserve( nDestSize );
	char cSource = *pSource;
	const char* pFound;
	while ( cSource )
	{
		if ( (pFound=strchr(pFind,cSource)) != NULL )
		{
			bool bIgnoreAmpersand = false;
			if ( (nFlags&MNF_WITHREFS) && *pFound == '&' )
			{
				// Do not replace ampersand if it is start of any entity reference
				// &[#_:A-Za-zU][_:-.A-Za-z0-9U]*; where U is > 0x7f
				const char* pCheckEntity = pSource;
				++pCheckEntity;
				char c = *pCheckEntity;
				if ( (c>='A'&&c<='Z') || (c>='a'&&c<='z')
						|| c=='#' || c=='_' || c==':' || c>0x7f )
				{
					while ( 1 )
					{
						++pCheckEntity;
						c = *pCheckEntity;
						if ( c == ';' )
						{
							int nEntityLen = (int)(pCheckEntity - pSource) + 1;
							strText.append( pSource, nEntityLen );
							pSource = pCheckEntity;
							bIgnoreAmpersand = true;
						}
						else if ( (c>='A'&&c<='Z') || (c>='a'&&c<='z') || (c>='0'&&c<='9')
								|| c=='_' || c==':' || c=='-' || c=='.' || c>0x7f )
							continue;
						break;
					}
				}
			}
			if ( ! bIgnoreAmpersand )
			{
				pFound = szaReplace[pFound-pFind];
				strText.append( pFound );
			}
		}
		else
		{
			strText += cSource;
		}
		++pSource;
		cSource = *pSource;
	}
	return strText;
}

string CMarkupSTL::UnescapeText( const char* szText, int nTextLength /*=-1*/ )
{
	// Convert XML friendly text to text as seen outside XML document
	// ampersand escape codes replaced with special characters e.g. convert "6&gt;7" to "6>7"
	// ampersand numeric codes replaced with character e.g. convert &#60; to <
	// Conveniently the result is always the same or shorter in byte length
	//
	static const char* szaCode[] = { "lt;","amp;","gt;","apos;","quot;" };
	static int anCodeLen[] = { 3,4,3,5,5 };
	static const char* szSymbol = "<&>\'\"";
	string strText;
	const char* pSource = szText;
	if ( nTextLength == -1 )
		nTextLength = (int)strlen(szText);
	strText.reserve( nTextLength );
	int nChar = 0;
	while ( nChar < nTextLength )
	{
		if ( pSource[nChar] == '&' )
		{
			bool bCodeConverted = false;

			// Is it a numeric character reference?
			if ( pSource[nChar+1] == '#' )
			{
				// Is it a hex number?
				int nBase = 10;
				int nNumericChar = nChar + 2;
				char cChar = pSource[nNumericChar];
				if ( cChar == 'x' )
				{
					++nNumericChar;
					cChar = pSource[nNumericChar];
					nBase = 16;
				}

				// Look for terminating semi-colon within 7 characters
				int nCodeLen = 0;
				while ( nCodeLen < 7 && cChar && cChar != ';' )
				{
					// only ASCII digits 0-9, A-F, a-f expected
					++nCodeLen;
					cChar = pSource[nNumericChar + nCodeLen];
				}

				// Process unicode
				if ( cChar == ';' )
				{
					int nUnicode = strtol( &pSource[nNumericChar], NULL, nBase );
					/* MBCS
					int nMBLen = wctomb( &pDest[nLen], (wchar_t)nUnicode );
					if ( nMBLen > 0 )
						nLen += nMBLen;
					else
						nUnicode = 0;
					*/
					if ( nUnicode < 0x80 )
						strText += (char)nUnicode;
					else if ( nUnicode < 0x800 )
					{
						// Convert to 2-byte UTF-8
						strText += (char)(((nUnicode&0x7c0)>>6) | 0xc0);
						strText += (char)((nUnicode&0x3f) | 0x80);
					}
					else
					{
						// Convert to 3-byte UTF-8
						strText += (char)(((nUnicode&0xf000)>>12) | 0xe0);
						strText += (char)(((nUnicode&0xfc0)>>6) | 0x80);
						strText += (char)((nUnicode&0x3f) | 0x80);
					}
					if ( nUnicode )
					{
						// Increment index past ampersand semi-colon
						nChar = nNumericChar + nCodeLen + 1;
						bCodeConverted = true;
					}
				}
			}
			else // does not start with #
			{
				// Look for matching &code;
				for ( int nMatch = 0; nMatch < 5; ++nMatch )
				{
					if ( nChar < nTextLength - anCodeLen[nMatch]
						&& strncmp(szaCode[nMatch],&pSource[nChar+1],anCodeLen[nMatch]) == 0 )
					{
						// Insert symbol and increment index past ampersand semi-colon
						strText += szSymbol[nMatch];
						nChar += anCodeLen[nMatch] + 1;
						bCodeConverted = true;
						break;
					}
				}
			}

			// If the code is not converted, leave it as is
			if ( ! bCodeConverted )
			{
				strText += '&';
				++nChar;
			}
		}
		else // not &
		{
			strText += pSource[nChar];
			++nChar;
		}
	}
	return strText;
}

//[CMARKUPDEV
bool CMarkupSTL::SetDoc( const string& strDoc )
{
	m_strDoc = strDoc;
	m_strError.erase();
	return x_ParseDoc();
}

string CMarkupSTL::GetDeclaredEncoding( const char* szDoc )
{
	// Extract encoding attribute from XML Declaration
	string strEncoding;
	const char* pStart = strchr( szDoc, '<' );
	if ( pStart && pStart[1] == '?' )
	{
		const char* pEnd = strstr( szDoc, "?>" );
		if ( pEnd )
		{
			string strXMLDecl( pStart, (int)(pEnd-pStart)+2 );
			CMarkupSTL xmlDecl( strXMLDecl.c_str() );
			if ( xmlDecl.FindNode() )
				strEncoding = xmlDecl.GetAttrib( "encoding" );
		}
	}
	return strEncoding;
}

bool CMarkupSTL::FindPrevElem( const char* szName )
{
	// Change current position only if found
	//
	if ( m_aPos.GetSize() )
	{
		int iPos = x_FindPrevElem( m_iPosParent, m_iPos, szName );
		if ( iPos )
		{
			// Assign new position
			x_SetPos( m_aPos[iPos].iElemParent, iPos, 0 );
			return true;
		}
	}
	return false;
}

bool CMarkupSTL::FindPrevChildElem( const char* szName )
{
	// Change current child position only if found
	//
	int iPosChild = x_FindPrevElem( m_iPos, m_iPosChild, szName );
	if ( iPosChild )
	{
		// Assign new position
		int iPos = m_aPos[iPosChild].iElemParent;
		x_SetPos( m_aPos[iPos].iElemParent, iPos, iPosChild );
		return true;
	}

	return false;
}
//]CMARKUPDEV

int CMarkupSTL::FindNode( int nType )
{
	// Change current node position only if a node is found
	// If nType is 0 find any node, otherwise find node of type nType
	// Return type of node or 0 if not found
	// If found node is an element, change m_iPos

	// Determine where in document to start scanning for node
	int nTypeFound = 0;
	int nNodeOffset = m_nNodeOffset;
	if ( m_nNodeType > 1 )
	{
		// By-pass current node
		nNodeOffset += m_nNodeLength;
	}
	else
	{
		// Set position to begin looking for node
		nNodeOffset = 0; // default to start of document
		if ( m_iPos )
		{
			// After element
			nNodeOffset = m_aPos[m_iPos].StartAfter();
		}
		else if ( m_iPosParent )
		{
			// Immediately after start tag of parent
			if ( m_aPos[m_iPosParent].IsEmptyElement() )
				return 0;
			else
				nNodeOffset = m_aPos[m_iPosParent].StartContent();
		}
	}

	// Get nodes until we find what we're looking for
	int iPosNew = m_iPos;
	TokenPos token( m_strDoc, m_nFlags );
	NodePos node;
	token.nNext = nNodeOffset;
	do
	{
		nNodeOffset = token.nNext;
		nTypeFound = x_ParseNode( token, node );
		if ( nTypeFound == 0 )
		{
			// Check if we have reached the end of the parent element
			// Otherwise it is a lone end tag
			if ( m_iPosParent && nNodeOffset == m_aPos[m_iPosParent].StartContent()
					+ m_aPos[m_iPosParent].ContentLen() )
				return 0;
			nTypeFound = MNT_LONE_END_TAG;
		}
		else if ( nTypeFound < 0 )
		{
			if ( nTypeFound == -2 )
				return 0;
			// -1 is node error
			nTypeFound = MNT_NODE_ERROR;
		}
		else if ( nTypeFound == MNT_ELEMENT )
		{
			if ( iPosNew )
				iPosNew = m_aPos[iPosNew].iElemNext;
			else
				iPosNew = m_aPos[m_iPosParent].iElemChild;
			if ( ! iPosNew )
				return 0;
			if ( ! nType || (nType & nTypeFound) )
			{
				// Found element node, move position to this element
				x_SetPos( m_iPosParent, iPosNew, 0 );
				return m_nNodeType;
			}
			token.nNext = m_aPos[iPosNew].StartAfter();
		}
	}
	while ( nType && ! (nType & nTypeFound) );

	m_iPos = iPosNew;
	m_iPosChild = 0;
	m_nNodeOffset = nNodeOffset;
	m_nNodeLength = token.nNext - nNodeOffset;
	m_nNodeType = nTypeFound;
	MARKUP_SETDEBUGSTATE;
	return m_nNodeType;
}

bool CMarkupSTL::RemoveNode()
{
	if ( m_iPos || m_nNodeLength )
	{
		x_RemoveNode( m_iPosParent, m_iPos, m_nNodeType, m_nNodeOffset, m_nNodeLength );
		m_iPosChild = 0;
		MARKUP_SETDEBUGSTATE;
		return true;
	}
	return false;
}

string CMarkupSTL::GetTagName() const
{
	// Return the tag name at the current main position
	string strTagName;

	// This method is primarily for elements, however
	// it does return something for certain other nodes
	if ( m_nNodeLength )
	{
		switch ( m_nNodeType )
		{
		case MNT_PROCESSING_INSTRUCTION:
		case MNT_LONE_END_TAG:
			{
				// <?target or </tagname
				TokenPos token( m_strDoc, m_nFlags );
				token.nNext = m_nNodeOffset + 2;
				if ( x_FindName(token) )
					strTagName = x_GetToken( token );
			}
			break;
		case MNT_COMMENT:
			strTagName = "#comment";
			break;
		case MNT_CDATA_SECTION:
			strTagName = "#cdata-section";
			break;
		case MNT_DOCUMENT_TYPE:
			{
				// <!DOCTYPE name
				TokenPos token( m_strDoc, m_nFlags );
				token.nNext = m_nNodeOffset + 2;
				if ( x_FindName(token) && x_FindName(token) )
					strTagName = x_GetToken( token );
			}
			break;
		case MNT_TEXT:
		case MNT_WHITESPACE:
			strTagName = "#text";
			break;
		}
		return strTagName;
	}

	if ( m_iPos )
		strTagName = x_GetTagName( m_iPos );
	return strTagName;
}

bool CMarkupSTL::IntoElem()
{
	// If there is no child position and IntoElem is called it will succeed in release 6.3
	// (A subsequent call to FindElem will find the first element)
	// The following short-hand behavior was never part of EDOM and was misleading
	// It would find a child element if there was no current child element position and go into it
	// It is removed in release 6.3, this change is NOT backwards compatible!
	// if ( ! m_iPosChild )
	//	FindChildElem();

	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		x_SetPos( m_iPos, m_iPosChild, 0 );
		return true;
	}
	return false;
}

bool CMarkupSTL::OutOfElem()
{
	// Go to parent element
	if ( m_iPosParent )
	{
		x_SetPos( m_aPos[m_iPosParent].iElemParent, m_iPosParent, m_iPos );
		return true;
	}
	return false;
}

string CMarkupSTL::GetAttribName( int n ) const
{
	// Return nth attribute name of main position
	TokenPos token( m_strDoc, m_nFlags );
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[m_iPos].nStart + 1;
	else if ( m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return "";
	if ( x_FindAttrib(token,NULL,n) )
		return x_GetToken( token );
	return "";
}

bool CMarkupSTL::SavePos( const char* szPosName )
{
	// Save current element position in saved position map
	if ( szPosName )
	{
		SavedPos savedpos;
		if ( szPosName )
			savedpos.strName = szPosName;
		if ( m_iPosChild )
		{
			savedpos.iPos = m_iPosChild;
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_CHILD;
		}
		else if ( m_iPos )
		{
			savedpos.iPos = m_iPos;
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_MAIN;
		}
		else
		{
			savedpos.iPos = m_iPosParent;
		}
		savedpos.nSavedPosFlags |= SavedPosMap::SPM_USED;

		if ( ! m_mapSavedPos.pTable )
			m_mapSavedPos.AllocMapTable();
		int nSlot = m_mapSavedPos.Hash( szPosName );
		SavedPos* pSavedPos = m_mapSavedPos.pTable[nSlot];
		int nOffset = 0;
		if ( ! pSavedPos )
		{
			pSavedPos = new SavedPos[2];
			pSavedPos[1].nSavedPosFlags = SavedPosMap::SPM_LAST;
			m_mapSavedPos.pTable[nSlot] = pSavedPos;
		}
		else
		{
			while ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				if ( pSavedPos[nOffset].strName == szPosName )
					break;
				if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
				{
					int nNewSize = (nOffset + 6) * 2;
					SavedPos* pNewSavedPos = new SavedPos[nNewSize];
					for ( int nCopy=0; nCopy<=nOffset; ++nCopy )
						pNewSavedPos[nCopy] = pSavedPos[nCopy];
					pNewSavedPos[nOffset].nSavedPosFlags ^= SavedPosMap::SPM_LAST;
					pNewSavedPos[nNewSize-1].nSavedPosFlags = SavedPosMap::SPM_LAST;
					delete [] pSavedPos;
					pSavedPos = pNewSavedPos;
					m_mapSavedPos.pTable[nSlot] = pSavedPos;
					++nOffset;
					break;
				}
				++nOffset;
			}
		}
		if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_LAST;
		pSavedPos[nOffset] = savedpos;

		/*
		// To review hash table balance, uncomment and watch strBalance
		string strBalance;
		char szSlot[20];
		for ( nSlot=0; nSlot < SavedPosMap::SPM_SIZE; ++nSlot )
		{
			pSavedPos = m_mapSavedPos.pTable[nSlot];
			int nCount = 0;
			while ( pSavedPos && pSavedPos->nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				++nCount;
				if ( pSavedPos->nSavedPosFlags & SavedPosMap::SPM_LAST )
					break;
				++pSavedPos;
			}
			sprintf( szSlot, "%d ", nCount );
			strBalance += szSlot;
		}
		*/

		return true;
	}
	return false;
}

bool CMarkupSTL::RestorePos( const char* szPosName )
{
	// Restore element position if found in saved position map
	if ( szPosName && m_mapSavedPos.pTable )
	{
		int nSlot = m_mapSavedPos.Hash( szPosName );
		SavedPos* pSavedPos = m_mapSavedPos.pTable[nSlot];
		if ( pSavedPos )
		{
			int nOffset = 0;
			while ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				if ( pSavedPos[nOffset].strName == szPosName )
				{
					int i = pSavedPos[nOffset].iPos;
					if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_CHILD )
						x_SetPos( m_aPos[m_aPos[i].iElemParent].iElemParent, m_aPos[i].iElemParent, i );
					else if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_MAIN )
						x_SetPos( m_aPos[i].iElemParent, i, 0 );
					else
						x_SetPos( i, 0, 0 );
					return true;
				}
				if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
					break;
				++nOffset;
			}
		}
	}
	return false;
}

bool CMarkupSTL::RemoveElem()
{
	// Remove current main position element
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		int iPos = x_RemoveElem( m_iPos );
		x_SetPos( m_iPosParent, iPos, 0 );
		return true;
	}
	return false;
}

bool CMarkupSTL::RemoveChildElem()
{
	// Remove current child position element
	if ( m_iPosChild )
	{
		int iPosChild = x_RemoveElem( m_iPosChild );
		x_SetPos( m_iPosParent, m_iPos, iPosChild );
		return true;
	}
	return false;
}

long CMarkupSTL::GetAttribLong( const char* szAttrib ) const
{
  std::string sAttr = GetAttrib(szAttrib);
  return atol(sAttr.c_str());
}

//[CMARKUPDEV
string CMarkupSTL::FindGetData( const char* szName )
{
	if ( FindElem(szName) )
		return x_GetData( m_iPos );
	return "";
}

bool CMarkupSTL::FindSetData( const char* szName, const char* szData, int nFlags )
{
	if ( FindElem(szName) )
		return x_SetData( m_iPos, szData, nFlags );
	return false;
}

string CMarkupSTL::EncodeBase64( const unsigned char* pBuffer, int nBufferLen )
{
	// Return Base64 string
	// 1 byte takes 2, 2:3, 3:4, 4:6, 5:7, 6:8, 7:10....
	int nLenBase64 =  nBufferLen + nBufferLen/3 + 3;
	nLenBase64 += (nLenBase64 / 64) * 2 + 4; // CRLFs
	string strBase64;
	strBase64.resize( nLenBase64 );

	// Start with CRLF
	int nOffsetBase64 = 0;
	strBase64[nOffsetBase64++] = '\r';
	strBase64[nOffsetBase64++] = '\n';

	// Loop through pBuffer 3 bytes at a time
	const char* pCodes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int nOffsetBuffer = 0;
	unsigned int n3BufferBytes;
	int nTop;
	int nPad = 0;
	while ( nOffsetBuffer < nBufferLen )
	{
		// Set n3BufferBytes
		n3BufferBytes = pBuffer[nOffsetBuffer++];
		n3BufferBytes = n3BufferBytes << 8;
		if ( nOffsetBuffer < nBufferLen )
			n3BufferBytes |= pBuffer[nOffsetBuffer++];
		else
			++nPad;
		n3BufferBytes = n3BufferBytes << 8;
		if ( nOffsetBuffer < nBufferLen )
			n3BufferBytes |= pBuffer[nOffsetBuffer++];
		else
			++nPad;

		// Fill strBase64
		nTop = nOffsetBase64 + 4;
		while ( nTop != nOffsetBase64 )
		{
			--nTop;
			if ( nPad == 0 || nTop < nOffsetBase64 + 4 - nPad )
				strBase64[nTop] = pCodes[n3BufferBytes & 0x3f];
			else
				strBase64[nTop] = '=';
			n3BufferBytes = n3BufferBytes >> 6;
		}
		nOffsetBase64 += 4;

		if ( ! (nOffsetBase64 % 66) || nPad )
		{
			strBase64[nOffsetBase64++] = '\r';
			strBase64[nOffsetBase64++] = '\n';
		}
	}

	strBase64.resize( nOffsetBase64 );
	return strBase64;
}

int CMarkupSTL::DecodeBase64( const string& strBase64, unsigned char* pBuffer, int nBufferLen )
{
	// Return length of binary buffer after decoding
	// If nBufferLen is 0, returns long enough length without decoding
	// so that you can allocate a large enough buffer
	// 1 byte takes 2, 2:3, 3:4, 4:6, 5:7, 6:8, 7:10....
	// Otherwise, returns actual decoded length as long as it fits inside nBufferLen
	int nLenBase64 = (int)strBase64.size();
	if ( ! nBufferLen )
		return (nLenBase64/4)*3 + 3;

	// Loop through Base64, 4 bytes at a time
	const char* pBase64 = strBase64.c_str();
	int nOffsetBase64 = 0;
	int nOffsetBuffer = 0;
	unsigned int n3BufferBytes;
	int nTop;
	int nCount;
	int nCode;
	int nPad = 0;
	while ( nOffsetBase64 < nLenBase64 )
	{
		// Set n3BufferBytes
		nCount = 0;
		n3BufferBytes = 0;
		while ( nCount != 4 && nOffsetBase64 < nLenBase64 )
		{
			// Check encoded character and bypass if whitespace
			nCode = pBase64[nOffsetBase64++];
			if ( nCode =='\r' || nCode == '\n' )
				continue;

			// Shift previous bits up before OR-ing 6 lower sig bits
			n3BufferBytes = n3BufferBytes << 6;

			// Use ASCII codes for fast conversion
			if ( nCode > 96 ) //(a-z)
				n3BufferBytes |= (nCode-71);
			else if ( nCode > 64 ) //(A-Z)
				n3BufferBytes |= (nCode-65);
			else if ( nCode == 61 ) //(=)
				++nPad;
			else if ( nCode > 47 ) //(0-9)
				n3BufferBytes |= (nCode+4);
			else if ( nCode == 47 ) //(/)
				n3BufferBytes |= 63;
			else if ( nCode == 43 ) //(+)
				n3BufferBytes |= 62;
			++nCount;
		}

		if ( nCount == 4 )
		{
			// Fill pBuffer
			nTop = nOffsetBuffer + 3;
			if ( nTop - nPad > nBufferLen )
				return 0;
			while ( nTop != nOffsetBuffer )
			{
				--nTop;
				if ( nPad == 0 || nTop < nOffsetBuffer+3-nPad )
					pBuffer[nTop] = (unsigned char)(n3BufferBytes & 0xff);
				n3BufferBytes = n3BufferBytes >> 8;
			}
			nOffsetBuffer += 3-nPad;
		}
	}

	return nOffsetBuffer;
}

int CMarkupSTL::UTF16To8( char* pszUTF8, const wchar_t* pwszUTF16, int nUTF8Count )
{
	// Supports the same arguments as wcstombs
	// the pwszUTF16 source must be a NULL-terminated UTF-16 string
	// if pszUTF8 is NULL, the number of bytes required is returned and nUTF8Count is ignored
	// otherwise pszUTF8 is filled with the result string and NULL-terminated if nUTF8Count allows
	// nUTF8Count is the byte size of pszUTF8 and must be large enough for the NULL if NULL desired
	// and the number of bytes (excluding NULL) is returned
	//
	int nUChar = *pwszUTF16, nUTF8Len = 0;
	while ( nUChar )
	{
		// Decode UTF-16
		if ( (nUChar & ~0x000007ff) == 0xd800 ) // W1
		{
			++pwszUTF16;
			if ( ! *pwszUTF16 ) // W2
				break; // incorrect UTF-16
			nUChar = (((nUChar & 0x3ff) << 10) | (*pwszUTF16 & 0x3ff)) + 0x10000;
		}
		++pwszUTF16;

		// Encode UTF-8
		if ( ! (nUChar & ~0x0000007f) ) // < 0x80
		{
			if ( pszUTF8 )
			{
				if ( nUTF8Len < nUTF8Count )
					pszUTF8[nUTF8Len++] = (char)nUChar;
				else
					break;
			}
			else
				++nUTF8Len;
		}
		else if ( ! (nUChar & ~0x000007ff) ) // < 0x800
		{
			if ( pszUTF8 )
			{
				if ( nUTF8Len + 1 < nUTF8Count )
				{
					pszUTF8[nUTF8Len++] = (char)(((nUChar&0x7c0)>>6)|0xc0);
					pszUTF8[nUTF8Len++] = (char)((nUChar&0x3f)|0x80);
				}
				else
					break;
			}
			else
				nUTF8Len += 2;
		}
		else if ( ! (nUChar & ~0x0000ffff) ) // < 0x10000
		{
			if ( pszUTF8 )
			{
				if ( nUTF8Len + 2 < nUTF8Count )
				{
					pszUTF8[nUTF8Len++] = (char)(((nUChar&0xf000)>>12)|0xe0);
					pszUTF8[nUTF8Len++] = (char)(((nUChar&0xfc0)>>6)|0x80);
					pszUTF8[nUTF8Len++] = (char)((nUChar&0x3f)|0x80);
				}
				else
					break;
			}
			else
				nUTF8Len += 3;
		}
		else // < 0x110000
		{
			if ( pszUTF8 )
			{
				if ( nUTF8Len + 3 < nUTF8Count )
				{
					pszUTF8[nUTF8Len++] = (char)(((nUChar&0x1c0000)>>18)|0xf0);
					pszUTF8[nUTF8Len++] = (char)(((nUChar&0x3f000)>>12)|0x80);
					pszUTF8[nUTF8Len++] = (char)(((nUChar&0xfc0)>>6)|0x80);
					pszUTF8[nUTF8Len++] = (char)((nUChar&0x3f)|0x80);
				}
				else
					break;
			}
			else
				nUTF8Len += 4;
		}
		nUChar = *pwszUTF16;
	}
	if ( pszUTF8 && nUTF8Len < nUTF8Count )
		pszUTF8[nUTF8Len] = 0;
	return nUTF8Len;
}

int CMarkupSTL::UTF8To16( wchar_t* pwszUTF16, const char* pszUTF8, int nUTF8Count )
{
	// Supports the same arguments as mbstowcs
	// the pszUTF8 source must be a UTF-8 string which will be processed up to NULL-terminator or nUTF8Count
	// if pwszUTF16 is NULL, the number of wide chars required is returned
	// nUTF8Count is maximum UTF-8 bytes to convert and should include NULL if NULL desired in result
	// if pwszUTF16 is not NULL it is filled with the result string and it must be large enough
	// result will be NULL-terminated if NULL encountered in pszUTF8 before nUTF8Count
	// and the number of UTF-8 bytes converted is returned
	//
	int nUChar, nUTF8Len = 0, nWideLen = 0, nExtraChars;
	while ( nUTF8Len < nUTF8Count )
	{
		// Decode UTF-8
		nUChar = (unsigned char)*pszUTF8;
		if ( ! nUChar )
		{
			if ( pwszUTF16 )
				pwszUTF16[nWideLen] = 0;
			break;
		}
		if ( nUChar & 0x80 )
		{
			if ( ! (nUChar & 0x20) )
			{
				nExtraChars = 1;
				nUChar &= 0x1f;
			}
			else if ( ! (nUChar & 0x10) )
			{
				nExtraChars = 2;
				nUChar &= 0x0f;
			}
			else
			{
				nExtraChars = 3;
				nUChar &= 0x07;
			}
			while ( nExtraChars-- )
			{
				++pszUTF8;
				++nUTF8Len;
				if ( nUTF8Len < nUTF8Count && (*pszUTF8 & 0x80) )
				{
					nUChar = nUChar<<6;
					nUChar |= *pszUTF8 & 0x3f;
				}
				else
				{
					--pszUTF8;
					--nUTF8Len;
					nUChar = (wchar_t)'?';
					break;
				}
			}
		}
		++pszUTF8;
		++nUTF8Len;

		// Encode UTF-16
		if ( nUChar & ~0xffff )
		{
			if ( pwszUTF16 )
			{
				// Surrogate pair
				nUChar -= 0x10000;
				pwszUTF16[nWideLen++] = (wchar_t)(((nUChar>>10) & 0x3ff) | 0xd800); // W1
				pwszUTF16[nWideLen++] = (wchar_t)((nUChar & 0x3ff) | 0xdc00); // W2
			}
			else
				nWideLen += 2;
		}
		else
		{
			if ( pwszUTF16 )
				pwszUTF16[nWideLen++] = (wchar_t)nUChar;
			else
				++nWideLen;
		}
	}
	if ( ! pwszUTF16 )
		return nWideLen;
	return nUTF8Len;
}

string CMarkupSTL::UTF8ToA( const char* pszUTF8 )
{
	// Converts from UTF-8 to locale ANSI charset (via wide string)
	// this uses wcstombs which requires setlocale other than minimal "C" locale
	// e.g. setlocale(LC_ALL, "") enables the OS system locale charset
	// When larger than 255 wide chars an intermediate wide string buffer is allocated
	// And when result string is larger than 255 another intermediate buffer is allocated
	wchar_t wszBufferOnStack[256];
	wchar_t* pwszBuffer;
	int nUTF8Len = (int)strlen( pszUTF8 );
	int nWideLen = UTF8To16(NULL,pszUTF8,nUTF8Len);
	if ( nWideLen > 255 )
		pwszBuffer = new wchar_t[nWideLen+1];
	else
		pwszBuffer = wszBufferOnStack;
	UTF8To16(pwszBuffer,pszUTF8,nUTF8Len+1);
	int nALen = (int)wcstombs(NULL,pwszBuffer,0);
	string strANSI;
	if ( nALen > 255 )
	{
		char* pszBuffer = new char[nALen+1];
		wcstombs(pszBuffer,pwszBuffer,nALen);
		strANSI.assign( pszBuffer, nALen );
		delete [] pszBuffer;
	}
	else
	{
		char szBufferOnStack[256];
		wcstombs(szBufferOnStack,pwszBuffer,nALen);
		strANSI.assign( szBufferOnStack, nALen );
	}
	if ( nWideLen > 255 )
		delete [] pwszBuffer;
	return strANSI;
}

string CMarkupSTL::AToUTF8( const char* pszANSI )
{
	// Converts locale ANSI charset to UTF-8 (via wide string)
	// this uses mbstowcs which requires setlocale other than minimal "C" locale
	// e.g. setlocale(LC_ALL, "") enables the OS system locale charset
	// When larger than 255 wide chars an intermediate wide string buffer is allocated
	// And when result string is larger than 255 another intermediate buffer is allocated
	wchar_t wszBufferOnStack[256];
	wchar_t* pwszBuffer;
	int nALen = (int)strlen( pszANSI );
	int nWideLen = (int)mbstowcs(NULL,pszANSI,nALen);
	if ( nWideLen > 255 )
		pwszBuffer = new wchar_t[nWideLen+1];
	else
		pwszBuffer = wszBufferOnStack;
	mbstowcs(pwszBuffer,pszANSI,nALen+1);
	int nUTF8Len = UTF16To8(NULL,pwszBuffer,0);
	string strUTF8;
	if ( nUTF8Len > 255 )
	{
		char* pszBuffer = new char[nUTF8Len+1];
		UTF16To8(pszBuffer,pwszBuffer,nUTF8Len);
		strUTF8.assign( pszBuffer, nUTF8Len );
		delete [] pszBuffer;
	}
	else
	{
		char szBufferOnStack[256];
		UTF16To8(szBufferOnStack,pwszBuffer,nUTF8Len);
		strUTF8.assign( szBufferOnStack, nUTF8Len );
	}
	if ( nWideLen > 255 )
		delete [] pwszBuffer;
	return strUTF8;
}

bool CMarkupSTL::GetOffsets( int* pnStart, int* pnLen, int* pnInStart, int* pnInLen ) const
{
	// Return document offsets of current main position element
	// Offset methods are not part of EDOM but are for editor support
	if ( m_nNodeLength )
	{
		*pnStart = m_nNodeOffset;
		*pnLen = m_nNodeLength;
		if ( pnInStart )
		{
			if ( m_nNodeType == MNT_COMMENT )
				*pnInStart = m_nNodeOffset + 4;
			else if ( m_nNodeType == MNT_PROCESSING_INSTRUCTION )
				*pnInStart = m_nNodeOffset + 2;
			else if ( m_nNodeType == MNT_CDATA_SECTION )
				*pnInStart = m_nNodeOffset + 9;
			else if ( m_nNodeType == MNT_LONE_END_TAG )
				*pnInStart = m_nNodeOffset + 2;
			else
				*pnInStart = m_nNodeOffset;
		}
		if ( pnInLen )
		{
			if ( m_nNodeType == MNT_COMMENT )
				*pnInLen = m_nNodeLength - 7;
			else if ( m_nNodeType == MNT_PROCESSING_INSTRUCTION )
				*pnInLen = m_nNodeLength - 4;
			else if ( m_nNodeType == MNT_CDATA_SECTION )
				*pnInLen = m_nNodeLength - 12;
			else if ( m_nNodeType == MNT_LONE_END_TAG )
				*pnInLen = m_nNodeLength - 3;
			else
				*pnInLen = m_nNodeLength;
		}
		return true;
	}
	else if ( m_iPos )
	{
		*pnStart = m_aPos[m_iPos].nStart;
		*pnLen = m_aPos[m_iPos].nLength;
		if ( pnInStart )
		{
			if ( m_aPos[m_iPos].IsEmptyElement() )
				*pnInStart = *pnStart;
			else
				*pnInStart = m_aPos[m_iPos].StartContent();
		}
		if ( pnInLen )
			*pnInLen = m_aPos[m_iPos].ContentLen();
		return true;
	}
	return false;
}

bool CMarkupSTL::GetAttribOffsets( const char* szAttrib, int* pnStart, int* pnLen, int* pnInStart, int* pnInLen ) const
{
	// Return document offsets of current main position element's named attribute
	// Offset methods are not part of EDOM but are for editor support
	if ( m_iPos && szAttrib )
	{
		TokenPos token( m_strDoc, m_nFlags );
		token.nNext = m_aPos[m_iPos].nStart + 1;
		if ( x_FindAttrib(token,szAttrib) )
		{
			*pnStart = token.nPreSpaceStart + token.nPreSpaceLength;
			*pnLen = token.nR + 1 - *pnStart + ((token.nTokenFlags&MNF_QUOTED)?1:0);
			if ( pnInStart )
				*pnInStart = token.nL;
			if ( pnInLen )
				*pnInLen = token.Length();
			return true;
		}
	}
	return false;
}
//]CMARKUPDEV

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

bool CMarkupSTL::x_AllocPosArray( int nNewSize /*=0*/ )
{
	// Resize m_aPos when the document is created or the array is filled
	// The PosArray class is implemented using segments to reduce contiguous memory requirements
	// It reduces reallocations (copying of memory) since this only occurs within one segment
	// The "Grow By" algorithm ensures there are no reallocations after 2 segments
	//
	if ( ! nNewSize )
		nNewSize = m_iPosFree + (m_iPosFree>>1); // Grow By: multiply size by 1.5
	if ( m_aPos.GetSize() < nNewSize )
	{
		// Grow By: new size can be at most one more complete segment
		int nSeg = (m_aPos.GetSize()?m_aPos.GetSize()-1:0) >> m_aPos.PA_SEGBITS;
		int nNewSeg = (nNewSize-1) >> m_aPos.PA_SEGBITS;
		if ( nNewSeg > nSeg + 1 )
		{
			nNewSeg = nSeg + 1;
			nNewSize = (nNewSeg+1) << m_aPos.PA_SEGBITS;
		}

		// Allocate array of segments
		if ( m_aPos.nSegs <= nNewSeg )
		{
			int nNewSegments = 4 + nNewSeg * 2;
			char* pNewSegments = new char[nNewSegments*sizeof(char*)];
			if ( m_aPos.SegsUsed() )
				memcpy( pNewSegments, m_aPos.pSegs, m_aPos.SegsUsed()*sizeof(char*) );
			if ( m_aPos.pSegs )
				delete[] (char*)m_aPos.pSegs;
			m_aPos.pSegs = (ElemPos**)pNewSegments;
			m_aPos.nSegs = nNewSegments;
		}

		// Calculate segment sizes
		int nSegSize = m_aPos.GetSize() - (nSeg << m_aPos.PA_SEGBITS);
		int nNewSegSize = nNewSize - (nNewSeg << m_aPos.PA_SEGBITS);

		// Complete first segment
		int nFullSegSize = 1 << m_aPos.PA_SEGBITS;
		if ( nSeg < nNewSeg && nSegSize < nFullSegSize )
		{
			char* pNewFirstSeg = new char[ nFullSegSize * sizeof(ElemPos) ];
			if ( nSegSize )
			{
				// Reallocate
				memcpy( pNewFirstSeg, m_aPos.pSegs[nSeg], nSegSize * sizeof(ElemPos) );
				delete[] (char*)m_aPos.pSegs[nSeg];
			}
			m_aPos.pSegs[nSeg] = (ElemPos*)pNewFirstSeg;
		}

		// New segment
		char* pNewSeg = new char[ nNewSegSize * sizeof(ElemPos) ];
		if ( nNewSeg == nSeg && nSegSize )
		{
			// Reallocate
			memcpy( pNewSeg, m_aPos.pSegs[nSeg], nSegSize * sizeof(ElemPos) );
			delete[] (char*)m_aPos.pSegs[nSeg];
		}
		m_aPos.pSegs[nNewSeg] = (ElemPos*)pNewSeg;
		m_aPos.nSize = nNewSize;
	}
	return true;
}

bool CMarkupSTL::x_ParseDoc()
{
	// Preserve pre-parse result
	string strResult = m_strError;

	// Reset indexes
	ResetPos();
	m_mapSavedPos.RemoveAll();

	// Starting size of position array: 1 element per 64 bytes of document
	// Tight fit when parsing small doc, only 0 to 2 reallocs when parsing large doc
	// Start at 8 when creating new document
	m_iPosFree = 1;
	x_AllocPosArray( (int)m_strDoc.size() / 64 + 8 );
	m_iPosDeleted = 0;

	// Parse document
	m_aPos[0].ClearVirtualParent();
	if ( m_strDoc.size() )
	{
		TokenPos token( m_strDoc, m_nFlags );
		int iPos = x_ParseElem( 0, token );
		m_aPos[0].nLength = (int)m_strDoc.size();
		if ( iPos > 0 )
		{
			m_aPos[0].iElemChild = iPos;
			if ( m_aPos[iPos].iElemNext )
				m_strError = "Root element has sibling";
		}
		else
			m_strError = "No root element";
	}
	else
		m_strError = "Empty document";

	ResetPos();

	// Combine preserved result with parse error
	if ( ! strResult.empty() )
	{
		if ( m_strError.empty() )
			m_strError = strResult;
		else
			m_strError = strResult + ", " + m_strError;
	}

	return IsWellFormed();
};

int CMarkupSTL::x_ParseElem( int iPosParent, TokenPos& token )
{
	// This is either called by x_ParseDoc or x_AddSubDoc or x_SetElemContent
	// Returns index of the first element encountered or zero if no elements
	//
	int iElemRoot = 0;
	int iPos = iPosParent;
	int iVirtualParent = iPosParent;
	int nRootDepth = m_aPos[iPos].Level();
	token.nNext = 0;
	m_strError.erase();

	// Loop through the nodes of the document
	NodeStack aNodes;
	aNodes.Add();
	int nDepth = 0;
	int nMatchDepth;
	int iPosChild;
	int iPosMatch;
	int nTypeFound = 0;
	ElemPos* pElem;
	int iElemFirst, iElemLast;
	while ( 1 )
	{
		nTypeFound = x_ParseNode( token, aNodes.Top() );
		nMatchDepth = 0;
		if ( nTypeFound == MNT_ELEMENT ) // start tag
		{
			iPos = x_GetFreePos();
			if ( ! iElemRoot )
				iElemRoot = iPos;
			pElem = &m_aPos[iPos];
			pElem->iElemParent = iPosParent;
			pElem->iElemNext = 0;
			if ( m_aPos[iPosParent].iElemChild )
			{
				iElemFirst = m_aPos[iPosParent].iElemChild;
				iElemLast = m_aPos[iElemFirst].iElemPrev;
				m_aPos[iElemLast].iElemNext = iPos;
				pElem->iElemPrev = iElemLast;
				m_aPos[iElemFirst].iElemPrev = iPos;
				pElem->nFlags = 0;
			}
			else
			{
				m_aPos[iPosParent].iElemChild = iPos;
				pElem->iElemPrev = iPos;
				pElem->nFlags = MNF_FIRST;
			}
			pElem->SetLevel( nRootDepth + nDepth );
			pElem->iElemChild = 0;
			pElem->nStart = aNodes.Top().nStart;
			pElem->SetStartTagLen( aNodes.Top().nLength );
			if ( aNodes.Top().nFlags & MNF_EMPTY )
			{
				iPos = iPosParent;
				pElem->SetEndTagLen( 0 );
				pElem->nLength = aNodes.Top().nLength;
			}
			else
			{
				iPosParent = iPos;
				++nDepth;
				aNodes.Add();
			}
		}
		else if ( nTypeFound == 0 ) // end tag
		{
			nMatchDepth = nDepth;
			iPosMatch = iPos;
			while ( nMatchDepth && ! token.Match(aNodes.At(nMatchDepth-1).strMeta) )
			{
				/*
				// Auto-switch case sensitivity
				if ( ! (token.nTokenFlags & MDF_IGNORECASE ) )
				{
					token.nTokenFlags |= MDF_IGNORECASE;
					if ( token.Match(aNodes.At(nMatchDepth-1).strMeta) )
						break;
					token.nTokenFlags |= MDF_IGNORECASE;
				}
				*/
				--nMatchDepth;
				iPosMatch = m_aPos[iPosMatch].iElemParent;
			}
			if ( nMatchDepth == 0 )
			{
				// Not matched at all, it is a lone end tag, a non-element node
				m_aPos[iVirtualParent].nFlags |= MNF_ILLFORMED;
				m_aPos[iPos].nFlags |= MNF_ILLDATA;
				if ( m_strError.empty() )
				{
					char* szError = new char[token.Length()+100];
					sprintf( szError, "No start tag for end tag '%s' at offset %d",
						x_GetToken(token).c_str(), aNodes.Top().nStart );
					m_strError = szError;
					delete [] szError;
				}
			}
			else
			{
				pElem = &m_aPos[iPosMatch];
				pElem->nLength = aNodes.Top().nStart - pElem->nStart + aNodes.Top().nLength;
				pElem->SetEndTagLen( aNodes.Top().nLength );
			}
		}
		else if ( nTypeFound == -1 )
		{
			m_aPos[iVirtualParent].nFlags |= MNF_ILLFORMED;
			m_aPos[iPos].nFlags |= MNF_ILLDATA;
			if ( m_strError.empty() )
				m_strError = aNodes.Top().strMeta;
		}

		// Matched end tag, or end of document
		if ( nMatchDepth || nTypeFound == -2 )
		{
			if ( nDepth > nMatchDepth )
				m_aPos[iVirtualParent].nFlags |= MNF_ILLFORMED;

			// Process any non-ended elements
			while ( nDepth > nMatchDepth )
			{
				// Element with no end tag
				pElem = &m_aPos[iPos];
				iPosChild = pElem->iElemChild;
				iPosParent = pElem->iElemParent;
				pElem->SetEndTagLen( 0 );
				pElem->nFlags |= MNF_NONENDED;
				pElem->iElemChild = 0;
				pElem->nLength = pElem->StartTagLen();
				if ( pElem->nFlags & MNF_ILLDATA )
				{
					pElem->nFlags ^= MNF_ILLDATA;
					m_aPos[iPosParent].nFlags |= MNF_ILLDATA;
				}
				while ( iPosChild )
				{
					m_aPos[iPosChild].iElemParent = iPosParent;
					m_aPos[iPosChild].iElemPrev = iPos;
					m_aPos[iPos].iElemNext = iPosChild;
					iPos = iPosChild;
					iPosChild = m_aPos[iPosChild].iElemNext;
				}
				iPos = iPosParent;
				aNodes.Remove();
				--nDepth;

				// Error string
				// if end tag did not match, top node is end tag that did not match pElem
				// if end of document, any nodes below top have no end tag
				if ( m_strError.empty() )
				{
					if ( nTypeFound == 0 )
					{
						char* szError = new char[aNodes.Top().strMeta.size()+token.Length()+100];
						sprintf( szError, "End tag '%s' at offset %d does not match start tag '%s' at offset %d",
							x_GetToken(token).c_str(), token.nL-1, aNodes.Top().strMeta.c_str(), pElem->nStart );
						m_strError = szError;
						delete [] szError;
					}
					else
					{
						char* szError = new char[aNodes.Top().strMeta.size()+100];
						sprintf( szError, "Element '%s' at offset %d not ended",
							aNodes.Top().strMeta.c_str(), aNodes.Top().nStart );
						m_strError = szError;
						delete [] szError;
					}
				}
			}
			if ( nTypeFound == -2 )
				break;
			iPosParent = m_aPos[iPos].iElemParent;
			iPos = iPosParent;
			aNodes.Remove();
			--nDepth;
		}
	}
	return iElemRoot;
}

bool CMarkupSTL::x_FindAny( const char* szDoc, int& nChar )
{
	// Starting at nChar, find a non-whitespace char
	// return false if no non-whitespace before end of document, nChar points to end
	// otherwise return true and nChar points to non-whitespace char
	while ( szDoc[nChar] && strchr(" \t\n\r",szDoc[nChar]) )
		++nChar;
	return szDoc[nChar] != '\0';
}

bool CMarkupSTL::x_FindName( CMarkupSTL::TokenPos& token )
{
	// Starting at token.nNext, bypass whitespace and find the next name
	// returns true on success, members of token point to token
	// returns false on end of document, members point to end of document
	const char* szDoc = token.szDoc;
	int nChar = token.nNext;

	// By-pass leading whitespace
	if ( ! x_FindAny(szDoc,nChar) )
	{
		// No token was found before end of document
		token.nL = nChar;
		token.nR = nChar - 1;
		token.nNext = nChar;
		return false;
	}

	// Go until special char or whitespace
	token.nL = nChar;
	while ( szDoc[nChar] && ! strchr(" \t\n\r<>=\\/?!",szDoc[nChar]) )
		++nChar;

	// Adjust end position if it is one special char
	if ( nChar == token.nL )
		++nChar; // it is a special char
	token.nR = nChar - 1;

	// nNext points to one past last char of token
	token.nNext = nChar;
	return true;
}

string CMarkupSTL::x_GetToken( const CMarkupSTL::TokenPos& token )
{
	// The token contains indexes into the document identifying a small substring
	// Build the substring from those indexes and return it
	if ( token.nL > token.nR )
		return "";
	string strToken( &token.szDoc[token.nL], token.Length() );
	return strToken;
}

int CMarkupSTL::x_FindElem( int iPosParent, int iPos, const char* szPath ) const
{
	// If szPath is NULL or empty, go to next sibling element
	// Otherwise go to next sibling element with matching path
	//
	/* //[!CMARKUPDEV
	if ( iPos )
		iPos = m_aPos[iPos].iElemNext;
	else
		iPos = m_aPos[iPosParent].iElemChild;

	// Finished here if szPath not specified
	if ( szPath == NULL || !szPath[0] )
		return iPos;

	// Search
	TokenPos token( m_strDoc, m_nFlags );
	while ( iPos )
	{
		// Compare tag name
		token.nNext = m_aPos[iPos].nStart + 1;
		x_FindName( token ); // Locate tag name
		if ( token.Match(szPath) )
			return iPos;
		iPos = m_aPos[iPos].iElemNext;
	}
	return 0;
	*/ //]!CMARKUPDEV

	//[CMARKUPDEV
	//
	// Example relative paths:
	//
	// "ITEM/QTY" first QTY child element of next ITEM element
	// "ITEM/*" first child element of next ITEM element
	//
	// Example absolute paths:
	//
	// "/"  root
	// "/ORDER"  root must be an ORDER element
	// "/*/ITEM" first ITEM child element of root
	//
	// Example anywhere path (only at beginning)
	//
	// "//A" first A element after current position (depth first)
	// "//A[@href='index.html']" A anywhere with specific href attribute
	//
	// Example paths with predicates
	//
	// "/ORDER/ITEM[2]" second ITEM element under root ORDER
	// "ITEM[NAME]" next ITEM element with a NAME child element
	// "*[@code] next element with code attribute
	//
	int iPathOffset = 0;
	bool bDepthFirst = false;
	if ( szPath && szPath[0] == '/' )
	{
		if ( szPath[1] == '/' ) // anywhere
		{
			bDepthFirst = true;
			if ( m_aPos[iPos].iElemChild )
				iPos = m_aPos[iPos].iElemChild;
			else
			{
				while ( iPos && ! m_aPos[iPos].iElemNext )
					iPos = m_aPos[iPos].iElemParent;
				iPos = m_aPos[iPos].iElemNext;
			}
			++iPathOffset;
		}
		else // absolute
		{
			// Go to root
			iPos = m_aPos[0].iElemChild;
			iPosParent = 0;
		}
		++iPathOffset;
	}
	else if ( iPos )
		iPos = m_aPos[iPos].iElemNext;
	else
		iPos = m_aPos[iPosParent].iElemChild;

	// Finished here if szPath not specified
	if ( szPath == NULL || !szPath[iPathOffset] )
		return iPos;

	// Loop through elements until criteria is met or end
	TokenPos token( m_strDoc, m_nFlags );
	while ( iPos )
	{
		// Compare tag name
		bool bMatch = false;
		token.nNext = m_aPos[iPos].nStart + 1;
		const char* pName = &szPath[iPathOffset];
		int iPathOffsetAtName = iPathOffset;
		if ( *pName == '*' || (x_FindName(token) && token.Match(pName)) )
		{
			bMatch = true;
			if ( *pName == '*' )
				++iPathOffset;
			else
				iPathOffset += token.Length();

			// Predicate (may cause non-match)
			if ( szPath[iPathOffset] == '[' )
			{
				++iPathOffset; // [
				if ( szPath[iPathOffset] >= '1' && szPath[iPathOffset] <= '9' )
				{
					// Loop through siblings to find nth element with matching name
					int nCount = 0;
					while ( szPath[iPathOffset] >= '0' && szPath[iPathOffset] <= '9' )
						nCount = nCount * 10 + (int)szPath[iPathOffset++] - (int)'0';
					--nCount;
					while ( nCount && m_aPos[iPos].iElemNext )
					{
						iPos = m_aPos[iPos].iElemNext;
						token.nNext = m_aPos[iPos].nStart + 1;
						if ( *pName == '*' || (x_FindName(token) && token.Match(pName)) )
							--nCount;
					}
					if ( nCount )
						return 0;
				}
				else // attrib or child tag name
				{
					if ( szPath[iPathOffset] == '@' )
					{
						++iPathOffset;
						token.nNext = m_aPos[iPos].nStart + 1;
						if ( x_FindAttrib(token,&szPath[iPathOffset]) )
						{
							do ++iPathOffset;
							while ( szPath[iPathOffset] && szPath[iPathOffset] != ']' );
						}
						else
							bMatch = false;
					}
					else // assume child tag name
					{
						int iPosChild = m_aPos[iPos].iElemChild;
						while ( iPosChild )
						{
							token.nNext = m_aPos[iPosChild].nStart + 1;
							if ( x_FindName(token) && token.Match(&szPath[iPathOffset]) )
								break;
							iPosChild = m_aPos[iPos].iElemChild;
						}
						if ( iPosChild )
							iPathOffset += token.Length();
						else
							bMatch = false;
					}
				}
				++iPathOffset; // ]
			}

			if ( bMatch )
			{
				// Another level of path
				if ( szPath[iPathOffset] == '/' )
				{
					if ( bDepthFirst )
						break; // multiple levels not supported for // path
					iPosParent = iPos;
					iPos = m_aPos[iPosParent].iElemChild;
					++iPathOffset;
				}
				else
					return iPos;
			}
			else
				iPathOffset = iPathOffsetAtName;
		}

		if ( ! bMatch )
		{
			if ( bDepthFirst )
			{
				if ( m_aPos[iPos].iElemChild )
					iPos = m_aPos[iPos].iElemChild;
				else
				{
					while ( iPos && ! m_aPos[iPos].iElemNext )
						iPos = m_aPos[iPos].iElemParent;
					iPos = m_aPos[iPos].iElemNext;
				}
			}
			else
				iPos = m_aPos[iPos].iElemNext;
		}
	}
	return 0;
	//]CMARKUPDEV
}

int CMarkupSTL::x_ParseNode( CMarkupSTL::TokenPos& token, CMarkupSTL::NodePos& node )
{
	// Call this with token.nNext set to the start of the node or tag
	// Upon return token.nNext points to the char after the node or tag
	// 
	// <!--...--> comment
	// <!DOCTYPE ...> dtd
	// <?target ...?> processing instruction
	// <![CDATA[...]]> cdata section
	// <NAME ...> element start tag
	// </NAME ...> element end tag
	//
	// returns the nodetype or
	// 0 for end tag
	// -1 for bad node
	// -2 for end of document
	//
	enum ParseBits
	{
		PD_OPENTAG = 1,
		PD_BANG = 2,
		PD_DASH = 4,
		PD_BRACKET = 8,
		PD_TEXTORWS = 16,
		PD_DOCTYPE = 32,
		PD_INQUOTE_S = 64,
		PD_INQUOTE_D = 128,
	};
	int nParseFlags = 0;

	const char* szFindEnd = NULL;
	int nNodeType = -1;
	int nEndLen = 0;
	int nName = 0;
	unsigned int cDminus1 = 0, cDminus2 = 0;
	#define FINDNODETYPE(e,t,n) { szFindEnd=e; nEndLen=(sizeof(e)-1); nNodeType=t; if(n) nName=(int)(pDoc-token.szDoc)+n-1; }
	#define FINDNODEBAD(e) { szFindEnd=">"; nEndLen=1; char szE[100]; sprintf(szE,"Incorrect %s at offset %d",e,nR); node.strMeta=szE; nNodeType=-1; }

	node.nStart = token.nNext;
	node.nFlags = 0;

	int nR = token.nNext;
	const char* pDoc = &token.szDoc[nR];
	register unsigned int cD = (unsigned int)*pDoc;
	if ( ! cD )
	{
		node.nLength = 0;
		node.nNodeType = 0;
		return -2; // end of document
	}

	while ( 1 )
	{
		cD = (unsigned int)*pDoc;
		if ( ! cD )
		{
			nR = (int)(pDoc - token.szDoc) - 1;
			if ( nNodeType != MNT_WHITESPACE && nNodeType != MNT_TEXT )
			{
				const char* szType = "tag";
				if ( (nParseFlags & PD_DOCTYPE) || nNodeType == MNT_DOCUMENT_TYPE )
					szType = "Doctype";
				else if ( nNodeType == MNT_ELEMENT )
					szType = "Element tag";
				else if ( nNodeType == 0 )
					szType = "Element end tag";
				else if ( nNodeType == MNT_CDATA_SECTION )
					szType = "CDATA Section";
				else if ( nNodeType == MNT_PROCESSING_INSTRUCTION )
					szType = "Processing instruction";
				else if ( nNodeType == MNT_COMMENT )
					szType = "Comment";
				nNodeType = -1;
				char szError[100];
				sprintf( szError, "%s at offset %d unterminated", szType, node.nStart );
				node.strMeta = szError;
			}
			break;
		}

		if ( nName )
		{
			if ( strchr(" \t\n\r/>",(char)cD) )
			{
				int nNameLen = (int)(pDoc - token.szDoc) - nName;
				if ( nNodeType == 0 )
				{
					token.nL = nName;
					token.nR = nName + nNameLen - 1;
				}
				else
				{
					node.strMeta.assign( &token.szDoc[nName], nNameLen );
				}
				nName = 0;
				cDminus2 = 0;
				cDminus1 = 0;
			}
			else
			{
				++pDoc;
				continue;
			}
		}

		if ( szFindEnd )
		{
			if ( cD == '>' && ! (nParseFlags & (PD_INQUOTE_S|PD_INQUOTE_D)) )
			{
				nR = (int)(pDoc - token.szDoc);
				if ( nEndLen == 1 )
				{
					szFindEnd = NULL;
					if ( nNodeType == MNT_ELEMENT && cDminus1 == '/' )
					{
						if ( (! cDminus2) || strchr(" \t\n\r\'\"",(char)cDminus2) )
							node.nFlags |= MNF_EMPTY;
					}
				}
				else if ( nR > nEndLen )
				{
					// Test for end of PI or comment
					const char* pEnd = pDoc - nEndLen + 1;
					const char* pFindEnd = szFindEnd;
					int nLen = nEndLen;
					while ( --nLen && *pEnd++ == *pFindEnd++ );
					if ( nLen == 0 )
						szFindEnd = NULL;
				}
				if ( ! szFindEnd && ! (nParseFlags & PD_DOCTYPE) )
					break;
			}
			else if ( cD == '<' && (nNodeType == MNT_TEXT || nNodeType == -1) )
			{
				nR = (int)(pDoc - token.szDoc) - 1;
				break;
			}
			else if ( nNodeType & (MNT_ELEMENT|MNT_DOCUMENT_TYPE) )
			{
				if ( cD == '\"' && ! (nParseFlags&PD_INQUOTE_S) )
					nParseFlags ^= PD_INQUOTE_D;
				else if ( cD == '\'' && ! (nParseFlags&PD_INQUOTE_D) )
					nParseFlags ^= PD_INQUOTE_S;
				if ( nNodeType == MNT_ELEMENT )
				{
					cDminus2 = cDminus1;
					cDminus1 = cD;
				}
			}
		}
		else if ( nParseFlags )
		{
			if ( nParseFlags & PD_TEXTORWS )
			{
				if ( cD == '<' )
				{
					nR = (int)(pDoc - token.szDoc) - 1;
					nNodeType = MNT_WHITESPACE;
					break;
				}
				else if ( ! strchr(" \t\n\r",(char)cD) )
				{
					nParseFlags ^= PD_TEXTORWS;
					FINDNODETYPE( "<", MNT_TEXT, 0 )
				}
			}
			else if ( nParseFlags & PD_OPENTAG )
			{
				nParseFlags ^= PD_OPENTAG;
				if ( cD > 0x60 || ( cD > 0x40 && cD < 0x5b ) || cD == 0x5f || cD == 0x3a )
					FINDNODETYPE( ">", MNT_ELEMENT, 1 )
				else if ( cD == '/' )
					FINDNODETYPE( ">", 0, 2 )
				else if ( cD == '!' )
					nParseFlags |= PD_BANG;
				else if ( cD == '?' )
					FINDNODETYPE( "?>", MNT_PROCESSING_INSTRUCTION, 2 )
				else
					FINDNODEBAD( "tag name character" )
			}
			else if ( nParseFlags & PD_BANG )
			{
				nParseFlags ^= PD_BANG;
				if ( cD == '-' )
					nParseFlags |= PD_DASH;
				else if ( cD == '[' && !(nParseFlags & PD_DOCTYPE) )
					nParseFlags |= PD_BRACKET;
				else if ( cD == 'D' && !(nParseFlags & PD_DOCTYPE) )
					nParseFlags |= PD_DOCTYPE;
				else if ( strchr("EAN",(char)cD) ) // <!ELEMENT ATTLIST ENTITY NOTATION
					FINDNODETYPE( ">", MNT_DOCUMENT_TYPE, 0 )
				else
					FINDNODEBAD( "! tag" )
			}
			else if ( nParseFlags & PD_DASH )
			{
				nParseFlags ^= PD_DASH;
				if ( cD == '-' )
					FINDNODETYPE( "-->", MNT_COMMENT, 0 )
				else
					FINDNODEBAD( "comment tag" )
			}
			else if ( nParseFlags & PD_BRACKET )
			{
				nParseFlags ^= PD_BRACKET;
				if ( cD == 'C' )
					FINDNODETYPE( "]]>", MNT_CDATA_SECTION, 0 )
				else
					FINDNODEBAD( "tag" )
			}
			else if ( nParseFlags & PD_DOCTYPE )
			{
				if ( cD == '<' )
					nParseFlags |= PD_OPENTAG;
				else if ( cD == '>' )
				{
					nR = (int)(pDoc - token.szDoc);
					nNodeType = MNT_DOCUMENT_TYPE;
					break;
				}
			}
		}
		else if ( cD == '<' )
		{
			nParseFlags |= PD_OPENTAG;
		}
		else
		{
			nNodeType = MNT_WHITESPACE;
			if ( strchr(" \t\n\r",(char)cD) )
				nParseFlags |= PD_TEXTORWS;
			else
				FINDNODETYPE( "<", MNT_TEXT, 0 )
		}
		++pDoc;
	}
	token.nNext = nR + 1;
	node.nLength = token.nNext - node.nStart;
	node.nNodeType = nNodeType;
	return nNodeType;
}

string CMarkupSTL::x_GetPath( int iPos ) const
{
	string strPath;
	while ( iPos )
	{
		string strTagName = x_GetTagName( iPos );
		int iPosParent = m_aPos[iPos].iElemParent;
		int iPosSib = 0;
		int nCount = 0;
		while ( iPosSib != iPos )
		{
			iPosSib = x_FindElem( iPosParent, iPosSib, strTagName.c_str() );
			++nCount;
		}
		if ( nCount > 1 )
		{
			char szPred[25];
			sprintf( szPred, "[%d]", nCount );
			strPath = "/" + strTagName + szPred + strPath;
		}
		else
			strPath = "/" + strTagName + strPath;
		iPos = iPosParent;
	}
	return strPath;
}

string CMarkupSTL::x_GetTagName( int iPos ) const
{
	// Return the tag name at specified element
	TokenPos token( m_strDoc, m_nFlags );
	token.nNext = m_aPos[iPos].nStart + 1;
	if ( ! iPos || ! x_FindName( token ) )
		return "";

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkupSTL::x_FindAttrib( CMarkupSTL::TokenPos& token, const char* szAttrib, int n/*=0*/ )
{
	// Return true if found, otherwise false and token.nNext is new insertion point
	// If szAttrib is NULL find attrib n and leave token at attrib name
	// If szAttrib is given, find matching attrib and leave token at value
	// support non-well-formed attributes e.g. href=/advanced_search?hl=en, nowrap
	// token also holds start and length of preceeding whitespace to support remove
	//
	int nPreSpaceStart;
	int nPreSpaceLength;
	int nChar;
	char cFirstChar;
	const char* szDoc = token.szDoc;
	int nAttrib = -1; // starts at tag name
	int nFoundAttribNameR = 0;
	bool bAfterEqual = false;
	while ( 1 )
	{
		// Starting at token.nNext, bypass whitespace and find the next token
		nChar = token.nNext;
		nPreSpaceStart = nChar;
		if ( ! x_FindAny(szDoc,nChar) )
			break;
		nPreSpaceLength = nChar - nPreSpaceStart;

		// Is it an opening quote?
		cFirstChar = szDoc[nChar];
		if ( cFirstChar == '\"' || cFirstChar == '\'' )
		{
			token.nTokenFlags |= MNF_QUOTED;

			// Move past opening quote
			++nChar;
			token.nL = nChar;

			// Look for closing quote
			while ( szDoc[nChar] && szDoc[nChar] != cFirstChar )
				++nChar;

			// Set right to before closing quote
			token.nR = nChar - 1;

			// Set nChar past closing quote unless at end of document
			if ( szDoc[nChar] )
				++nChar;
		}
		else
		{
			token.nTokenFlags &= ~MNF_QUOTED;

			// Go until special char or whitespace
			token.nL = nChar;
			if ( bAfterEqual )
			{
				while ( szDoc[nChar] && ! strchr(" \t\n\r>",szDoc[nChar]) )
					++nChar;
			}
			else
			{
				while ( szDoc[nChar] && ! strchr("= \t\n\r>/?",szDoc[nChar]) )
					++nChar;
			}

			// Adjust end position if it is one special char
			if ( nChar == token.nL )
				++nChar; // it is a special char
			token.nR = nChar - 1;
		}

		// nNext points to one past last char of token
		token.nNext = nChar;

		if ( ! bAfterEqual && ! (token.nTokenFlags&MNF_QUOTED) )
		{
			// Is it an equal sign?
			char cChar = szDoc[token.nL];
			if ( cChar == '=' )
			{
				bAfterEqual = true;
				continue;
			}

			// Is it the right angle bracket?
			if ( cChar == '>' || cChar == '/' || cChar == '?' )
			{
				token.nNext = nPreSpaceStart;
				break; // attrib not found
			}

			if ( nFoundAttribNameR )
				break;

			// Attribute name
			if ( nAttrib != -1 )
			{
				if ( ! szAttrib )
				{
					if ( nAttrib == n )
						return true; // found by number
				}
				else if ( token.Match(szAttrib) )
				{
					// Matched attrib name, go forward to value
					nFoundAttribNameR = token.nR;
					token.nPreSpaceStart = nPreSpaceStart;
					token.nPreSpaceLength = nPreSpaceLength;
				}
			}
			++nAttrib;
		}
		else if ( nFoundAttribNameR )
			break;
		bAfterEqual = false;
	}

	if ( nFoundAttribNameR )
	{
		if ( ! bAfterEqual )
		{
			// when attribute has no value the value is the attribute name
			token.nL = token.nPreSpaceStart + token.nPreSpaceLength;
			token.nR = nFoundAttribNameR;
			token.nNext = nFoundAttribNameR + 1;
		}
		return true; // found by name
	}
	return false; // not found
}

string CMarkupSTL::x_GetAttrib( int iPos, const char* szAttrib ) const
{
	// Return the value of the attrib
	TokenPos token( m_strDoc, m_nFlags );
	if ( iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[iPos].nStart + 1;
	else if ( iPos == m_iPos && m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return "";

	if ( szAttrib && x_FindAttrib( token, szAttrib ) )
		return UnescapeText( &token.szDoc[token.nL], token.Length() );
	return "";
}

bool CMarkupSTL::x_SetAttrib( int iPos, const char* szAttrib, int nValue )
{
	// Convert integer to string and call SetChildAttrib
	char szVal[25];
	sprintf( szVal, "%d", nValue );
	return x_SetAttrib( iPos, szAttrib, szVal );
}

bool CMarkupSTL::x_SetAttrib( int iPos, const char* szAttrib, const char* szValue )
{
	// Set attribute in iPos element
	TokenPos token( m_strDoc, m_nFlags );
	if ( iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[iPos].nStart + 1;
	else if ( iPos == m_iPos && m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return false;

	// Create insertion text depending on whether attribute already exists
	// Decision: for empty value leaving attrib="" instead of removing attrib
	int nReplace = 0;
	int nInsertAt;
	string strInsert;
	strInsert += x_ATTRIBQUOTE;
	strInsert += EscapeText( szValue, MNF_ESCAPEQUOTES );
	strInsert += x_ATTRIBQUOTE;
	if ( x_FindAttrib( token, szAttrib ) )
	{
		// Replace value
		nInsertAt = token.nL - ((token.nTokenFlags&MNF_QUOTED)?1:0);
		nReplace = token.Length() + ((token.nTokenFlags&MNF_QUOTED)?2:0);
	}
	else
	{
		// Insert string name value pair
		string strFormat;
		strFormat = " ";
		strFormat += szAttrib;
		strFormat += "=";
		strFormat += strInsert;
		strInsert = strFormat;
		nInsertAt = token.nNext;
	}

	x_DocChange( nInsertAt, nReplace, strInsert );
	int nAdjust = (int)strInsert.size() - nReplace;
	if ( m_nNodeType == MNT_PROCESSING_INSTRUCTION )
	{
		x_AdjustForNode( m_iPosParent, m_iPos, nAdjust );
		m_nNodeLength += nAdjust;
		MARKUP_SETDEBUGSTATE;
		return true;
	}
	m_aPos[iPos].AdjustStartTagLen( nAdjust );
	m_aPos[iPos].nLength += nAdjust;
	x_Adjust( iPos, nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}

//[CMARKUPDEV
bool CMarkupSTL::x_RemoveAttrib( int iPos, const char* szAttrib )
{
	// Find attribute in iPos element and remove it
	TokenPos token( m_strDoc, m_nFlags );
	if ( iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[iPos].nStart + 1;
	else if ( iPos == m_iPos && m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return false;

	if ( x_FindAttrib(token,szAttrib) )
	{
		int nReplace = token.nR + 1 - token.nPreSpaceStart + ((token.nTokenFlags&MNF_QUOTED)?1:0);
		x_DocChange( token.nPreSpaceStart, nReplace, "" );
		m_aPos[iPos].AdjustStartTagLen( - nReplace );
		m_aPos[iPos].nLength -= nReplace;
		x_Adjust( iPos, - nReplace );
		MARKUP_SETDEBUGSTATE;
		return true;
	}
	return false;
}

int CMarkupSTL::x_FindPrevElem( int iPosParent, int iPos, const char* szName )
{
	// If szName is NULL or empty, go to previous sibling element
	// Otherwise go to previous sibling element with matching name
	//
	if ( iPos )
	{
		if ( m_aPos[iPos].nFlags & MNF_FIRST )
			iPos = 0;
		else
			iPos = m_aPos[iPos].iElemPrev;
	}
	else
		iPos = m_aPos[m_aPos[iPosParent].iElemChild].iElemPrev;

	// Finished here if szName not specified
	if ( szName == NULL || !szName[0] )
		return iPos;

	// Search
	TokenPos token( m_strDoc, m_nFlags );
	while ( iPos )
	{
		// Compare tag name
		token.nNext = m_aPos[iPos].nStart + 1;
		x_FindName( token ); // Locate tag name
		if ( token.Match(szName) )
			return iPos;
		if ( m_aPos[iPos].nFlags & MNF_FIRST )
			iPos = 0;
		else
			iPos = m_aPos[iPos].iElemPrev;
	}
	return 0;
}
//]CMARKUPDEV

bool CMarkupSTL::x_CreateNode( string& strNode, int nNodeType, const char* szText )
{
	// Set strNode based on nNodeType and szData
	// Return false if szData would jeopardize well-formed document
	//
	switch ( nNodeType )
	{
	case MNT_PROCESSING_INSTRUCTION:
		strNode = "<?";
		strNode += szText;
		strNode += "?>";
		break;
	case MNT_COMMENT:
		strNode = "<!--";
		strNode += szText;
		strNode += "-->";
		break;
	case MNT_ELEMENT:
		strNode = "<";
		strNode += szText;
		strNode += "/>";
		break;
	case MNT_TEXT:
	case MNT_WHITESPACE:
		strNode = EscapeText( szText );
		break;
	case MNT_DOCUMENT_TYPE:
		strNode = szText;
		break;
	case MNT_LONE_END_TAG:
		return false;
	case MNT_CDATA_SECTION:
		if ( strstr(szText,"]]>") != NULL )
			return false;
		strNode = "<![CDATA[";
		strNode += szText;
		strNode += "]]>";
		break;
	}
	return true;
}

string CMarkupSTL::x_EncodeCDATASection( const char* szData )
{
	// Split CDATA Sections if there are any end delimiters
	string strData = "<![CDATA[";
	const char* pszNextStart = szData;
	const char* pszEnd = strstr( szData, "]]>" );
	while ( pszEnd )
	{
		strData += string( pszNextStart, (int)(pszEnd - pszNextStart) );
		strData += "]]]]><![CDATA[>";
		pszNextStart = pszEnd + 3;
		pszEnd = strstr( pszNextStart, "]]>" );
	}
	strData += pszNextStart;
	strData += "]]>";
	return strData;
}

bool CMarkupSTL::x_SetData( int iPos, int nValue )
{
	// Convert integer to string
	char szVal[25];
	sprintf( szVal, "%d", nValue );
	return x_SetData( iPos, szVal, 0 );
}

bool CMarkupSTL::x_SetData( int iPos, const char* szData, int nFlags )
{
	// Set data at specified position
	// if nFlags==1, set content of element to a CDATA Section
	string strInsert;

	if ( iPos == m_iPos && m_nNodeLength )
	{
		// Not an element
		if ( ! x_CreateNode(strInsert, m_nNodeType, szData) )
			return false;
		x_DocChange( m_nNodeOffset, m_nNodeLength, strInsert );
		x_AdjustForNode( m_iPosParent, iPos, (int)strInsert.size() - m_nNodeLength );
		m_nNodeLength = (int)strInsert.size();
		MARKUP_SETDEBUGSTATE;
		return true;
	}

	// Set data in iPos element
	if ( ! iPos || m_aPos[iPos].iElemChild )
		return false;

	// Build strInsert from szData based on nFlags
	if ( nFlags & MNF_WITHCDATA )
		strInsert = x_EncodeCDATASection( szData );
	else
		strInsert = EscapeText( szData, nFlags );

	// Insert
	NodePos node( MNF_WITHNOLINES|MNF_REPLACE );
	node.strMeta = strInsert;
	int iPosBefore = 0;
	int nReplace = x_InsertNew( iPos, iPosBefore, node );
	int nAdjust = (int)node.strMeta.size() - nReplace;
	x_Adjust( iPos, nAdjust );
	m_aPos[iPos].nLength += nAdjust;
	if ( m_aPos[iPos].nFlags & MNF_ILLDATA )
		m_aPos[iPos].nFlags &= ~MNF_ILLDATA;
	MARKUP_SETDEBUGSTATE;
	return true;
}

string CMarkupSTL::x_GetData( int iPos ) const
{
	if ( iPos == m_iPos && m_nNodeLength )
	{
		if ( m_nNodeType == MNT_COMMENT )
			return m_strDoc.substr( m_nNodeOffset+4, m_nNodeLength-7 );
		else if ( m_nNodeType == MNT_PROCESSING_INSTRUCTION )
			return m_strDoc.substr( m_nNodeOffset+2, m_nNodeLength-4 );
		else if ( m_nNodeType == MNT_CDATA_SECTION )
			return m_strDoc.substr( m_nNodeOffset+9, m_nNodeLength-12 );
		else if ( m_nNodeType == MNT_TEXT )
			return UnescapeText( &(m_strDoc.c_str())[m_nNodeOffset], m_nNodeLength );
		else if ( m_nNodeType == MNT_LONE_END_TAG )
			return m_strDoc.substr( m_nNodeOffset+2, m_nNodeLength-3 );
		else
			return m_strDoc.substr( m_nNodeOffset, m_nNodeLength );
	}

	// Return a string representing data between start and end tag
	// Return empty string if there are any children elements
	string strData;
	if ( ! m_aPos[iPos].iElemChild && ! m_aPos[iPos].IsEmptyElement() )
	{
		// Quick scan for any tags inside content
		int nContentLen = m_aPos[iPos].ContentLen();
		int nStartContent = m_aPos[iPos].StartContent();
		const char* pszContent = &(m_strDoc.c_str())[nStartContent];
		const char* pszTag = strchr( pszContent, '<' );
		if ( pszTag && ((int)(pszTag-pszContent) < nContentLen) )
		{
			// Concatenate all CDATA Sections and text nodes, ignore other nodes
			TokenPos token( m_strDoc, m_nFlags );
			token.nNext = nStartContent;
			NodePos node;
			while ( token.nNext < nStartContent + nContentLen )
			{
				x_ParseNode( token, node );
				if ( node.nNodeType == MNT_TEXT )
					strData += UnescapeText( &token.szDoc[node.nStart], node.nLength );
				else if ( node.nNodeType == MNT_CDATA_SECTION )
					strData += m_strDoc.substr( node.nStart+9, node.nLength-12 );
			}
		}
		else // no tags
			strData = UnescapeText( &(m_strDoc.c_str())[nStartContent], nContentLen );
	}
	return strData;
}

string CMarkupSTL::x_GetElemContent( int iPos ) const
{
	if ( iPos && m_aPos[iPos].ContentLen() )
		return m_strDoc.substr( m_aPos[iPos].StartContent(), m_aPos[iPos].ContentLen() );
	return "";
}

bool CMarkupSTL::x_SetElemContent( const char* szContent )
{
	// Set data in iPos element only
	if ( ! m_iPos )
		return false;

	if ( m_nNodeLength )
		return false; // not an element

	// Unlink all children
	int iPos = m_iPos;
	int iPosChild = m_aPos[iPos].iElemChild;
	bool bHadChild = (iPosChild != 0);
	while ( iPosChild )
		iPosChild = x_ReleaseSubDoc( iPosChild );
	if ( bHadChild )
		x_CheckSavedPos();

	// Parse content
	bool bWellFormed = true;
	TokenPos token( szContent, m_nFlags );
	int iPosVirtual = x_GetFreePos();
	m_aPos[iPosVirtual].ClearVirtualParent();
	m_aPos[iPosVirtual].SetLevel( m_aPos[iPos].Level() + 1 );
	iPosChild = x_ParseElem( iPosVirtual, token );
	if ( m_aPos[iPosVirtual].nFlags & MNF_ILLFORMED )
		bWellFormed = false;
	m_aPos[iPos].nFlags = (m_aPos[iPos].nFlags & ~MNF_ILLDATA) | (m_aPos[iPosVirtual].nFlags & MNF_ILLDATA);

	// Prepare insert and adjust offsets
	NodePos node( MNF_WITHNOLINES|MNF_REPLACE );
	node.strMeta = szContent;
	int iPosBefore = 0;
	int nReplace = x_InsertNew( iPos, iPosBefore, node );
	
	// Adjust and link in the inserted elements
	x_Adjust( iPosChild, node.nStart );
	m_aPos[iPosChild].nStart += node.nStart;
	m_aPos[iPos].iElemChild = iPosChild;
	while ( iPosChild )
	{
		m_aPos[iPosChild].iElemParent = iPos;
		iPosChild = m_aPos[iPosChild].iElemNext;
	}
	x_ReleasePos( iPosVirtual );

	int nAdjust = (int)node.strMeta.size() - nReplace;
	x_Adjust( iPos, nAdjust, true );
	m_aPos[iPos].nLength += nAdjust;

	x_SetPos( m_iPosParent, m_iPos, 0 );
	return bWellFormed;
}

void CMarkupSTL::x_DocChange( int nLeft, int nReplace, const string& strInsert )
{
	// Insert strInsert int m_strDoc at nLeft replacing nReplace chars
	// Do this with only one buffer reallocation if it grows
	//
	int nDocLength = (int)m_strDoc.size();
	int nInsLength = (int)strInsert.size();
	int nNewLength = nInsLength + nDocLength - nReplace;

	// When creating a document, reduce reallocs by reserving string space
	// Allow for 1.5 times the current allocation
	int nBufferLen = nNewLength;
	int nAllocLen = (int)m_strDoc.capacity();
	if ( nNewLength > nAllocLen )
	{
		nBufferLen += nBufferLen/2 + 128;
		if ( nBufferLen < nNewLength )
			nBufferLen = nNewLength;
		m_strDoc.reserve( nBufferLen );
	}

	m_strDoc.replace( nLeft, nReplace, strInsert );

	//[CMARKUPDEV
	m_nFlags |= MDF_MODIFIED;
	//]CMARKUPDEV
}

void CMarkupSTL::x_Adjust( int iPos, int nShift, bool bAfterPos /*=false*/ )
{
	// Loop through affected elements and adjust indexes
	// Algorithm:
	// 1. update children unless bAfterPos
	//    (if no children or bAfterPos is true, length of iPos not affected)
	// 2. update starts of next siblings and their children
	// 3. go up until there is a next sibling of a parent and update starts
	// 4. step 2
	int iPosTop = m_aPos[iPos].iElemParent;
	bool bPosFirst = bAfterPos; // mark as first to skip its children

	// Stop when we've reached the virtual parent (which has no tags)
	while ( m_aPos[iPos].StartTagLen() )
	{
		// Were we at containing parent of affected position?
		bool bPosTop = false;
		if ( iPos == iPosTop )
		{
			// Move iPosTop up one towards root
			iPosTop = m_aPos[iPos].iElemParent;
			bPosTop = true;
		}

		// Traverse to the next update position
		if ( ! bPosTop && ! bPosFirst && m_aPos[iPos].iElemChild )
		{
			// Depth first
			iPos = m_aPos[iPos].iElemChild;
		}
		else if ( m_aPos[iPos].iElemNext )
		{
			iPos = m_aPos[iPos].iElemNext;
		}
		else
		{
			// Look for next sibling of a parent of iPos
			// When going back up, parents have already been done except iPosTop
			while ( 1 )
			{
				iPos = m_aPos[iPos].iElemParent;
				if ( iPos == iPosTop )
					break;
				if ( m_aPos[iPos].iElemNext )
				{
					iPos = m_aPos[iPos].iElemNext;
					break;
				}
			}
		}
		bPosFirst = false;

		// Shift indexes at iPos
		if ( iPos != iPosTop )
			m_aPos[iPos].nStart += nShift;
		else
			m_aPos[iPos].nLength += nShift;
	}
}

int CMarkupSTL::x_InsertNew( int iPosParent, int& iPosRel, CMarkupSTL::NodePos& node )
{
	// Parent empty tag or tags with no content?
	bool bEmptyParentTag = iPosParent && m_aPos[iPosParent].IsEmptyElement();
	bool bNoContentParentTags = iPosParent && ! m_aPos[iPosParent].ContentLen();
	if ( node.nLength )
	{
		// Located at a non-element node
		if ( ! (node.nFlags & MNF_INSERT) )
			node.nStart += node.nLength;
	}
	else if ( iPosRel )
	{
		// Located at an element
		node.nStart = m_aPos[iPosRel].nStart;
		if ( ! (node.nFlags & MNF_INSERT) ) // follow iPosRel
			node.nStart += m_aPos[iPosRel].nLength;
	}
	else if ( bEmptyParentTag )
	{
		// Parent has no separate end tag, so split empty element
		if ( m_aPos[iPosParent].nFlags & MNF_NONENDED )
			node.nStart = m_aPos[iPosParent].StartContent();
		else
			node.nStart = m_aPos[iPosParent].StartContent() - 1;
	}
	else
	{
		if ( node.nFlags & (MNF_INSERT|MNF_REPLACE) )
			node.nStart = m_aPos[iPosParent].StartContent();
		else // before end tag
			node.nStart = m_aPos[iPosParent].StartAfter() - m_aPos[iPosParent].EndTagLen();
	}

	// Go up to start of next node, unless its splitting an empty element
	if ( ! (node.nFlags&(MNF_WITHNOLINES|MNF_REPLACE)) && ! bEmptyParentTag )
	{
		const char* szDoc = m_strDoc.c_str();
		int nChar = node.nStart;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == '<' )
			node.nStart = nChar;
	}

	// Is insert relative to element position? (i.e. not other kind of node)
	if ( ! node.nLength )
	{
		// Modify iPosRel to reflect position before
		if ( iPosRel )
		{
			if ( node.nFlags & MNF_INSERT )
			{
				if ( ! (m_aPos[iPosRel].nFlags & MNF_FIRST) )
					iPosRel = m_aPos[iPosRel].iElemPrev;
				else
					iPosRel = 0;
			}
		}
		else if ( ! (node.nFlags & MNF_INSERT) )
		{
			// If parent has a child, add after last child
			if ( m_aPos[iPosParent].iElemChild )
				iPosRel = m_aPos[m_aPos[iPosParent].iElemChild].iElemPrev;
		}
	}

	// Get node length (used only by x_AddNode)
	node.nLength = (int)node.strMeta.size();

	// Prepare end of lines
	if ( (! (node.nFlags & MNF_WITHNOLINES)) && (bEmptyParentTag || bNoContentParentTags) )
		node.nStart += x_EOLLEN;
	if ( ! (node.nFlags & MNF_WITHNOLINES) )
		node.strMeta += x_EOL;

	// Calculate insert offset and replace length
	int nReplace = 0;
	int nInsertAt = node.nStart;
	if ( bEmptyParentTag )
	{
		string strTagName = x_GetTagName( iPosParent );
		string strFormat;
		if ( node.nFlags & MNF_WITHNOLINES )
			strFormat = ">";
		else
			strFormat = ">" x_EOL;
		strFormat += node.strMeta;
		strFormat += "</";
		strFormat += strTagName;
		node.strMeta = strFormat;
		if ( m_aPos[iPosParent].nFlags & MNF_NONENDED )
		{
			nInsertAt = m_aPos[iPosParent].StartAfter() - 1;
			nReplace = 0;
			m_aPos[iPosParent].nFlags ^= MNF_NONENDED;
		}
		else
		{
			nInsertAt = m_aPos[iPosParent].StartAfter() - 2;
			nReplace = 1;
			m_aPos[iPosParent].AdjustStartTagLen( -1 );
		}
		m_aPos[iPosParent].SetEndTagLen( 3 + (int)strTagName.size() );
	}
	else
	{
		if ( node.nFlags & MNF_REPLACE )
		{
			nInsertAt = m_aPos[iPosParent].StartContent();
			nReplace = m_aPos[iPosParent].ContentLen();
		}
		else if ( bNoContentParentTags )
		{
			node.strMeta = x_EOL + node.strMeta;
			nInsertAt = m_aPos[iPosParent].StartContent();
		}
	}
	x_DocChange( nInsertAt, nReplace, node.strMeta );
	return nReplace;
}

bool CMarkupSTL::x_AddElem( const char* szName, int nValue, int nFlags )
{
	// Convert integer to string
	char szVal[25];
	sprintf( szVal, "%d", nValue );
	return x_AddElem( szName, szVal, nFlags );
}

bool CMarkupSTL::x_AddElem( const char* szName, const char* szValue, int nFlags )
{
	if ( nFlags & MNF_CHILD )
	{
		// Adding a child element under main position
		if ( ! m_iPos )
			return false;
	}
//[CMARKUPDEV
#if defined( x_VERSIONTAG )
	else if ( m_iPosParent == 0 && m_strDoc.empty() )
	{
		m_strDoc = x_VERSIONTAG x_EOL;
		m_aPos[0].nLength = m_strDoc.GetLength();
	}
#endif
//]CMARKUPDEV

	// Locate where to add element relative to current node
	NodePos node( nFlags );
	int iPosParent, iPosBefore;
	if ( nFlags & MNF_CHILD )
	{
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
		node.nStart = m_nNodeOffset;
		node.nLength = m_nNodeLength;
	}

	// Cannot have data in non-ended element
	if ( (nFlags&MNF_WITHNOEND) && szValue && szValue[0] )
		return false;

	// Allocate ElemPos structure for this element
	int iPos = x_GetFreePos();

	// Create string for insert
	// If no szValue is specified, an empty element is created
	// i.e. either <NAME>value</NAME> or <NAME/>
	//
	ElemPos* pElem = &m_aPos[iPos];
	int nLenName = (int)strlen(szName);
	if ( ! szValue || ! szValue[0] )
	{
		// <NAME/> empty element
		node.strMeta = "<";
		node.strMeta += szName;
		if ( nFlags & MNF_WITHNOEND )
		{
			node.strMeta += ">";
			pElem->SetStartTagLen( nLenName + 2 );
			pElem->nLength = nLenName + 2;
		}
		else
		{
			if ( nFlags & MNF_WITHXHTMLSPACE )
			{
				node.strMeta += " />";
				pElem->SetStartTagLen( nLenName + 4 );
				pElem->nLength = nLenName + 4;
			}
			else
			{
				node.strMeta += "/>";
				pElem->SetStartTagLen( nLenName + 3 );
				pElem->nLength = nLenName + 3;
			}
		}
		pElem->SetEndTagLen( 0 );
	}
	else
	{
		// <NAME>value</NAME>
		string strValue;
		if ( nFlags & MNF_WITHCDATA )
			strValue = x_EncodeCDATASection( szValue );
		else
			strValue = EscapeText( szValue, nFlags );
		int nLenValue = (int)strValue.size();
		node.strMeta = "<";
		node.strMeta += szName;
		node.strMeta += ">";
		node.strMeta += strValue;
		node.strMeta += "</";
		node.strMeta += szName;
		node.strMeta += ">";
		pElem->SetEndTagLen( nLenName + 3 );
		pElem->nLength = nLenName * 2 + nLenValue + 5;
		pElem->SetStartTagLen( nLenName + 2 );
	}

	// Insert
	int nReplace = x_InsertNew( iPosParent, iPosBefore, node );

	pElem->nStart = node.nStart;
	pElem->iElemChild = 0;
	if ( nFlags & MNF_WITHNOEND )
		pElem->nFlags = MNF_NONENDED;
	else
		pElem->nFlags = 0;
	x_LinkElem( iPosParent, iPosBefore, iPos );

	x_Adjust( iPos, (int)node.strMeta.size() - nReplace );

	if ( nFlags & MNF_CHILD )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else
		x_SetPos( iPosParent, iPos, 0 );
	return true;
}

string CMarkupSTL::x_GetSubDoc( int iPos ) const
{
	if ( iPos )
	{
		int nStart = m_aPos[iPos].nStart;
		int nNext = nStart + m_aPos[iPos].nLength;
		const char* szDoc = m_strDoc.c_str();
		int nChar = nNext;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == '<' )
			nNext = nChar;
		return m_strDoc.substr( nStart, nNext - nStart );
	}
	return "";
}

bool CMarkupSTL::x_AddSubDoc( const char* szSubDoc, int nFlags )
{
	// Add subdocument, parse, and modify positions of affected elements
	//
	NodePos node( nFlags );
	int iPosParent, iPosBefore;
	if ( nFlags & MNF_CHILD )
	{
		// Add a subdocument under main position, before or after child
		if ( ! m_iPos )
			return false;
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		// Add a subdocument under parent position, before or after main
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
		node.nStart = m_nNodeOffset;
		node.nLength = m_nNodeLength;
	}

	// Parse subdocument
	bool bWellFormed = true;
	TokenPos token( szSubDoc, m_nFlags );
	int iPosVirtual = x_GetFreePos();
	m_aPos[iPosVirtual].ClearVirtualParent();
	m_aPos[iPosVirtual].SetLevel( m_aPos[iPosParent].Level() + 1 );
	int iPos = x_ParseElem( iPosVirtual, token );
	if ( (!iPos) || m_aPos[iPosVirtual].nFlags & MNF_ILLFORMED )
		bWellFormed = false;
	if ( m_aPos[iPosVirtual].nFlags & MNF_ILLDATA )
		m_aPos[iPosParent].nFlags |= MNF_ILLDATA;

	// Extract subdocument without leading/trailing nodes
	int nExtractStart = 0;
	int iPosLast = m_aPos[iPos].iElemPrev;
	if ( bWellFormed )
	{
		nExtractStart = m_aPos[iPos].nStart;
		int nExtractLength = m_aPos[iPos].nLength;
		if ( iPos != iPosLast )
		{
			nExtractLength = m_aPos[iPosLast].nStart - nExtractStart + m_aPos[iPosLast].nLength;
			bWellFormed = false; // treat as subdoc here, but return not well-formed
		}
		node.strMeta.assign( &szSubDoc[nExtractStart], nExtractLength );
	}
	else
	{
		node.strMeta = szSubDoc;
		node.nFlags |= MNF_WITHNOLINES;
	}

	// Insert
	int nReplace = x_InsertNew( iPosParent, iPosBefore, node );

	// Adjust and link in the inserted elements
	// iPosVirtual will stop it from affecting rest of document
	int nAdjust = node.nStart - nExtractStart;
	if ( iPos && nAdjust )
	{
		x_Adjust( iPos, nAdjust );
		m_aPos[iPos].nStart += nAdjust;
	}
	int iPosChild = iPos;
	while ( iPosChild )
	{
		int iPosNext = m_aPos[iPosChild].iElemNext;
		x_LinkElem( iPosParent, iPosBefore, iPosChild );
		iPosBefore = iPosChild;
		iPosChild = iPosNext;
	}
	x_ReleasePos( iPosVirtual );

	// Now adjust remainder of document
	x_Adjust( iPosLast, (int)node.strMeta.size() - nReplace, true );

	// Set position to top element of subdocument
	if ( nFlags & MNF_CHILD )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else // Main
		x_SetPos( m_iPosParent, iPos, 0 );
	return bWellFormed;
}

int CMarkupSTL::x_RemoveElem( int iPos )
{
	// Remove element and all contained elements
	// Return new position
	//
	if ( ! iPos )
		return 0;

	// Determine whether any whitespace up to next tag
	int nAfterEnd = m_aPos[iPos].StartAfter();
	const char* szDoc = m_strDoc.c_str();
	int nChar = nAfterEnd;
	if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == '<' )
		nAfterEnd = nChar;

	// Remove from document, adjust affected indexes, and unlink
	int nLen = nAfterEnd - m_aPos[iPos].nStart;
	x_DocChange( m_aPos[iPos].nStart, nLen, string() );
	x_Adjust( iPos, - nLen, true );
	int iPosPrev = x_UnlinkElem( iPos );
	x_CheckSavedPos();
	return iPosPrev;
}

void CMarkupSTL::x_LinkElem( int iPosParent, int iPosBefore, int iPos )
{
	// Link in element, and initialize nFlags, and iElem indexes
	ElemPos* pElem = &m_aPos[iPos];
	pElem->iElemParent = iPosParent;
	if ( iPosBefore )
	{
		// Link in after iPosBefore
		pElem->nFlags &= ~MNF_FIRST;
		pElem->iElemNext = m_aPos[iPosBefore].iElemNext;
		if ( pElem->iElemNext )
			m_aPos[pElem->iElemNext].iElemPrev = iPos;
		else
			m_aPos[m_aPos[iPosParent].iElemChild].iElemPrev = iPos;
		m_aPos[iPosBefore].iElemNext = iPos;
		pElem->iElemPrev = iPosBefore;
	}
	else
	{
		// Link in as first child
		pElem->nFlags |= MNF_FIRST;
		if ( m_aPos[iPosParent].iElemChild )
		{
			pElem->iElemNext = m_aPos[iPosParent].iElemChild;
			pElem->iElemPrev = m_aPos[pElem->iElemNext].iElemPrev;
			m_aPos[pElem->iElemNext].iElemPrev = iPos;
			m_aPos[pElem->iElemNext].nFlags ^= MNF_FIRST;
		}
		else
		{
			pElem->iElemNext = 0;
			pElem->iElemPrev = iPos;
		}
		m_aPos[iPosParent].iElemChild = iPos;
	}
	if ( iPosParent )
		pElem->SetLevel( m_aPos[iPosParent].Level() + 1 );
}

int CMarkupSTL::x_UnlinkElem( int iPos )
{
	// Fix links to remove element and mark as deleted
	// return previous position or zero if none
	ElemPos* pElem = &m_aPos[iPos];

	// Find previous sibling and bypass removed element
	int iPosPrev = 0;
	if ( pElem->nFlags & MNF_FIRST )
	{
		if ( pElem->iElemNext ) // set next as first child
		{
			m_aPos[pElem->iElemParent].iElemChild = pElem->iElemNext;
			m_aPos[pElem->iElemNext].iElemPrev = pElem->iElemPrev;
			m_aPos[pElem->iElemNext].nFlags |= MNF_FIRST;
		}
		else // no children remaining
			m_aPos[pElem->iElemParent].iElemChild = 0;
	}
	else
	{
		iPosPrev = pElem->iElemPrev;
		m_aPos[iPosPrev].iElemNext = pElem->iElemNext;
		if ( pElem->iElemNext )
			m_aPos[pElem->iElemNext].iElemPrev = iPosPrev;
		else
			m_aPos[m_aPos[pElem->iElemParent].iElemChild].iElemPrev = iPosPrev;
	}
	x_ReleaseSubDoc( iPos );
	return iPosPrev;
}

int CMarkupSTL::x_ReleasePos( int iPos )
{
	int iPosNext = m_aPos[iPos].iElemNext;
	m_aPos[iPos].iElemNext = m_iPosDeleted;
	m_aPos[iPos].nFlags = MNF_DELETED;
	m_iPosDeleted = iPos;
	return iPosNext;
}

int CMarkupSTL::x_ReleaseSubDoc( int iPos )
{
	// Mark position structures as deleted by depth first traversal
	// Tricky because iElemNext used in traversal is overwritten for linked list of deleted
	// Return value is what iElemNext was before being overwritten
	//
	int iPosNext = 0, iPosTop = iPos;
	while ( 1 )
	{
		if ( m_aPos[iPos].iElemChild )
			iPos = m_aPos[iPos].iElemChild;
		else
		{
			while ( 1 )
			{
				iPosNext = x_ReleasePos( iPos );
				if ( iPos == iPosTop )
					return iPosNext;
				if ( iPosNext )
					break;
				iPos = m_aPos[iPos].iElemParent;
			}
			iPos = iPosNext;
		}
	}
	return iPosNext;
}

void CMarkupSTL::x_CheckSavedPos()
{
	// Remove any saved positions now pointing to deleted elements
	// Must be done as part of element removal before position reassigned
	if ( m_mapSavedPos.pTable )
	{
		for ( int nSlot = 0; nSlot < SavedPosMap::SPM_SIZE; ++nSlot )
		{
			SavedPos* pSavedPos = m_mapSavedPos.pTable[nSlot];
			if ( pSavedPos )
			{
				int nOffset = 0;
				int nSavedPosCount = 0;
				while ( 1 )
				{
					if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_USED )
					{
						int iPos = pSavedPos[nOffset].iPos;
						if ( ! (m_aPos[iPos].nFlags & MNF_DELETED) )
						{
							if ( nSavedPosCount < nOffset )
							{
								pSavedPos[nSavedPosCount] = pSavedPos[nOffset];
								pSavedPos[nSavedPosCount].nSavedPosFlags &= ~SavedPosMap::SPM_LAST;
							}
							++nSavedPosCount;
						}
					}
					if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
					{
						while ( nSavedPosCount <= nOffset )
							pSavedPos[nSavedPosCount++].nSavedPosFlags &= ~SavedPosMap::SPM_USED;
						break;
					}
					++nOffset;
				}
			}
		}
	}
}

void CMarkupSTL::x_AdjustForNode( int iPosParent, int iPos, int nShift )
{
	// Adjust affected indexes
	bool bAfterPos = true;
	if ( ! iPos )
	{
		// Change happened before or at first element under iPosParent
		// If there are any children of iPosParent, adjust from there
		// otherwise start at parent and adjust from there
		iPos = m_aPos[iPosParent].iElemChild;
		if ( iPos )
		{
			m_aPos[iPos].nStart += nShift;
			bAfterPos = false;
		}
		else
		{
			iPos = iPosParent;
			m_aPos[iPos].nLength += nShift;
		}
	}
	x_Adjust( iPos, nShift, bAfterPos );
}

bool CMarkupSTL::x_AddNode( int nNodeType, const char* szText, int nFlags )
{
	// Only comments, DTDs, and processing instructions are followed by CRLF
	// Other nodes are usually concerned with mixed content, so no CRLF
	if ( ! (nNodeType & (MNT_PROCESSING_INSTRUCTION|MNT_COMMENT|MNT_DOCUMENT_TYPE)) )
		nFlags |= MNF_WITHNOLINES;

	// Add node of nNodeType after current node position
	NodePos node( nFlags );
	if ( ! x_CreateNode(node.strMeta, nNodeType, szText) )
		return false;

	// Locate where to add node relative to current node
	int iPosBefore = m_iPos;
	int iPosParent = m_iPosParent;
	node.nStart = m_nNodeOffset;
	node.nLength = m_nNodeLength;
	node.nNodeType = nNodeType;

	int nReplace = x_InsertNew( iPosParent, iPosBefore, node );

	// If its a new element, create an ElemPos
	int iPos = iPosBefore;
	if ( nNodeType == MNT_ELEMENT )
	{
		// Set indexes
		iPos = x_GetFreePos();
		ElemPos* pElem = &m_aPos[iPos];
		pElem->nStart = node.nStart;
		pElem->SetStartTagLen( node.nLength );
		pElem->SetEndTagLen( 0 );
		pElem->nLength = node.nLength;
		node.nStart = 0;
		node.nLength = 0;
		pElem->iElemChild = 0;
		pElem->nFlags = 0;
		x_LinkElem( iPosParent, iPosBefore, iPos );
	}

	// Need to adjust element positions after iPos
	x_AdjustForNode( iPosParent, iPos, (int)node.strMeta.size() - nReplace );

	// Set current position
	m_iPos = iPos;
	m_iPosChild = 0;
	m_nNodeOffset = node.nStart;
	m_nNodeLength = node.nLength;
	m_nNodeType = nNodeType;
	MARKUP_SETDEBUGSTATE;
	return true;
}

void CMarkupSTL::x_RemoveNode( int iPosParent, int& iPos, int& nNodeType, int& nNodeOffset, int& nNodeLength )
{
	// Remove node and return new position
	//
	int iPosPrev = iPos;

	// Removing an element?
	if ( nNodeType == MNT_ELEMENT )
	{
		nNodeOffset = m_aPos[iPos].nStart;
		nNodeLength = m_aPos[iPos].nLength;
		iPosPrev = x_UnlinkElem( iPos );
		x_CheckSavedPos();
	}

	// Find previous node type, offset and length
	int nPrevOffset = 0;
	if ( iPosPrev )
		nPrevOffset = m_aPos[iPosPrev].StartAfter();
	else if ( iPosParent )
		nPrevOffset = m_aPos[iPosParent].StartContent();
	TokenPos token( m_strDoc, m_nFlags );
	NodePos node;
	token.nNext = nPrevOffset;
	int nPrevType = 0;
	while ( token.nNext < nNodeOffset )
	{
		nPrevOffset = token.nNext;
		nPrevType = x_ParseNode( token, node );
	}
	int nPrevLength = nNodeOffset - nPrevOffset;
	if ( ! nPrevLength )
	{
		// Previous node is iPosPrev element
		nPrevOffset = 0;
		if ( iPosPrev )
			nPrevType = MNT_ELEMENT;
	}

	// Remove node from document
 	x_DocChange( nNodeOffset, nNodeLength, string() );
	x_AdjustForNode( iPosParent, iPosPrev, - nNodeLength );

	// Was removed node a lone end tag?
	if ( nNodeType == MNT_LONE_END_TAG )
	{
		// See if we can unset parent MNF_ILLDATA flag
		token.nNext = m_aPos[iPosParent].StartContent();
		int nEndOfContent = token.nNext + m_aPos[iPosParent].ContentLen();
		int iPosChild = m_aPos[iPosParent].iElemChild;
		while ( token.nNext < nEndOfContent )
		{
			if ( x_ParseNode(token,node) <= 0 )
				break;
			if ( node.nNodeType == MNT_ELEMENT )
			{
				token.nNext = m_aPos[iPosChild].StartAfter();
				iPosChild = m_aPos[iPosChild].iElemNext;
			}
		}
		if ( token.nNext == nEndOfContent )
			m_aPos[iPosParent].nFlags &= ~MNF_ILLDATA;
	}

	nNodeType = nPrevType;
	nNodeOffset = nPrevOffset;
	nNodeLength = nPrevLength;
	iPos = iPosPrev;
}

