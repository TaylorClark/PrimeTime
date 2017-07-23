#define XML_UNICODE_WCHAR_T
#define XML_STATIC
#include "expat.h"
#include "Base/DataBlock.h"
#include "Base/FileFuncs.h"
#include "PrimeTimeUpdater.h"

int numUpdatesFound = 0;
enum ParseStates
{
	PS_None,
	PS_Root,
	PS_Update,
	PS_Update_File,
	PS_Update_File_URL,
	PS_Update_File_MD5,
	PS_Update_File_Size,
	PS_Update_Version,
	PS_COUNT
};

const wchar_t* XMLNODE_Root = L"primetimeupdates";
const wchar_t* XMLNODE_Update = L"update";
const wchar_t* XMLNODE_File = L"file";
const wchar_t* XMLNODE_File_Url = L"url";
const wchar_t* XMLNODE_File_MD5 = L"md5";
const wchar_t* XMLNODE_File_Version = L"version";
const wchar_t* XMLNODE_File_Size = L"size";



struct ParseData
{
	ParseData()
	{
		Clear();
	}

	ParseStates m_CurState;

	VersionUpdate m_CurUpdate;

	VersionUpdate::File m_CurrentFile;

	ApplicationUpdates m_FoundUpdates;

	int32 m_LastStoredVersion;

	// Clear the current update
	void ClearCurUpdate()
	{
		m_CurUpdate.version = -1;
		m_CurUpdate.files.clear();
	}

	// Clear all of the parse data
	void Clear()
	{
		m_CurState = PS_None;
		ClearCurUpdate();
		m_FoundUpdates.clear();
		m_LastStoredVersion = 100;
	}
};

static void XMLCALL startElement(void *pUserData, const wchar_t *szName, const wchar_t **pAtts )
{
	ParseData* pParseData = (ParseData*)pUserData;
	if( !pParseData )
		return;

	if( wcscmp( szName, XMLNODE_Root ) == 0 )
	{
		if( pParseData->m_CurState == PS_None )
			pParseData->m_CurState = PS_Root;
	}
	else if( wcscmp( szName, XMLNODE_Update ) == 0 )
	{
		if( pParseData->m_CurState == PS_Root )
		{
			pParseData->m_CurState = PS_Update;

			// Clear the update data for this entry
			pParseData->ClearCurUpdate();
		}
	}
	else if( wcscmp( szName, XMLNODE_File ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update )
			pParseData->m_CurState = PS_Update_File;
	}
	else if( wcscmp( szName, XMLNODE_File_Url ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update_File )
			pParseData->m_CurState = PS_Update_File_URL;
	}
	else if( wcscmp( szName, XMLNODE_File_MD5 ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update_File )
			pParseData->m_CurState = PS_Update_File_MD5;
	}
	else if( wcscmp( szName, XMLNODE_File_Version ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update )
			pParseData->m_CurState = PS_Update_Version;
	}
	else if( wcscmp( szName, XMLNODE_File_Size ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update_File )
			pParseData->m_CurState = PS_Update_File_Size;
	}
}

/// Handle an element close tag
static void XMLCALL endElement(void* pUserData, const wchar_t *szName)
{
	// Get the parse data object
	ParseData* pParseData = (ParseData*)pUserData;
	if( !pParseData )
		return;

	// Handle the element
	if( wcscmp( szName, XMLNODE_Root ) == 0 )
	{
		if( pParseData->m_CurState == PS_Root )
			pParseData->m_CurState = PS_None;
	}
	else if( wcscmp( szName, XMLNODE_Update ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update )
		{
			pParseData->m_CurState = PS_Root;

			// If the version is invalid
			int32 entryVersion = pParseData->m_CurUpdate.version;
			if( entryVersion == 0 )
				entryVersion = pParseData->m_LastStoredVersion + 1;
			
			// Ensure no duplicate versions
			ApplicationUpdates::iterator iterUpdate;
			--entryVersion;
			do
			{
				// Increment the version
				entryVersion++;

				// Find the entry in the map for this version if one exists
				iterUpdate = pParseData->m_FoundUpdates.find( entryVersion );
			}
			while( iterUpdate != pParseData->m_FoundUpdates.end() );
			
			// Add the update
			pParseData->m_LastStoredVersion = entryVersion;
			pParseData->m_CurUpdate.version = entryVersion;
			pParseData->m_FoundUpdates.insert( ApplicationUpdates::value_type( entryVersion, pParseData->m_CurUpdate ) );
			
			// Clear the update data for the next entry
			pParseData->ClearCurUpdate();
		}
	}
	else if( wcscmp( szName, XMLNODE_File ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update_File )
		{
			// Get the file name with extension
			pParseData->m_CurrentFile.sFileNameWExt = TCBase::GetFileNameWithExt( pParseData->m_CurrentFile.sURL.c_str() );

			// Store the file
			pParseData->m_CurUpdate.files.push_back( pParseData->m_CurrentFile );

			pParseData->m_CurrentFile.Clear();

			pParseData->m_CurState = PS_Update;
		}
	}
	else if( wcscmp( szName, XMLNODE_File_Url ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update_File_URL )
			pParseData->m_CurState = PS_Update_File;
	}
	else if( wcscmp( szName, XMLNODE_File_MD5 ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update_File_MD5 )
			pParseData->m_CurState = PS_Update_File;
	}
	else if( wcscmp( szName, XMLNODE_File_Version ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update_Version )
			pParseData->m_CurState = PS_Update;
	}
	else if( wcscmp( szName, XMLNODE_File_Size ) == 0 )
	{
		if( pParseData->m_CurState == PS_Update_File_Size )
			pParseData->m_CurState = PS_Update_File;
	}
}

/// Handle character data within the XML file
static void XMLCALL charData( void *pUserData, const wchar_t *szName, int len )
{
	// Get the parse data object
	ParseData* pParseData = (ParseData*)pUserData;
	if( !pParseData )
		return;

	if( pParseData->m_CurState == PS_Update_Version )
	{
		// Get the characters into a string
		std::wstring sVersion( szName, len );

		// Get the version into a floating point value
		double fVer = _wtof( sVersion.c_str() );

		// Scale the value by 100 and store it
		pParseData->m_CurUpdate.version = (int32)(fVer * 100.0);		
	}
	else if( pParseData->m_CurState == PS_Update_File_MD5 )
		pParseData->m_CurrentFile.sMD5 = std::wstring( szName, len );
	else if( pParseData->m_CurState == PS_Update_File_URL )
		pParseData->m_CurrentFile.sURL = std::wstring( szName, len );
	else if( pParseData->m_CurState == PS_Update_File_Size )
		pParseData->m_CurrentFile.expectedFileSize = (uint32)_wtoi( std::wstring( szName, len ).c_str() );
}

/// Parse the update XML file
ApplicationUpdates ParseUpdateXML( DataBlock xmlData )
{
	ParseData parseData;

	// Create and setup the XML parser
	XML_Parser parser = XML_ParserCreate( L"UTF-8" );
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler( parser, charData );
	XML_SetUserData( parser, &parseData );

	// Parse the data
	XML_Parse( parser, (const char*)xmlData.ReadData(xmlData.GetSize()), xmlData.GetSize(), true );

	// Free the data
	XML_ParserFree(parser);

	return parseData.m_FoundUpdates;
}