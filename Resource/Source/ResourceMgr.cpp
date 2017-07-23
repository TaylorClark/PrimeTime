//=================================================================================================
/*!
	\file ResourceMgr.cpp
	Resources Library
	Resource Manager Source
	\author Taylor Clark
	\date March 3, 2006

	This source file contains the implementation for ResourceManager class.
*/
//=================================================================================================
#include "../ResourceMgr.h"
#include <fstream>
#include "Base/MsgLogger.h"
#include "Base/NetSafeDataBlock.h"
#include "Base/StringFuncs.h"
#include "Base/TCAssert.h"
#include "Base/FileFuncs.h"
#include "Graphics2D/TCFont.h"
#include "Graphics2D/RefSprite.h"
#ifndef TOOLS
#include "Graphics2D/GraphicsMgr.h"
#endif
#include "GUI/GUIMgr.h"
#include "Audio/AudioMgr.h"
#include "Base/NetSafeSerializer.h"
#include "PrimeTime/ApplicationBase.h"


// Initialize the static variables
//const int ResourceMgr::MAX_RES_PER_DB = 128;
const FourCC ResourceMgr::FOURCCKEY_RESDB( "RSDB" );


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::Init()  Public
///
///	Initialize the resource manager and find the resource files.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::Init()
{
	// Get the resources
	FindResources();
}

/// Release all resources and memory that was loaded
void ResourceMgr::Term()
{
	// Free the inter-resource dependencies then free the object
	for( std::map< ResourceID, Resource* >::iterator iterRes = m_LoadedResources.begin();iterRes != m_LoadedResources.end();++iterRes)
		iterRes->second->ReleaseSubResources();

	// Free the resources
	for( std::map< ResourceID, Resource* >::iterator iterRes = m_LoadedResources.begin();iterRes != m_LoadedResources.end();++iterRes)
	{
		Resource* pRes = iterRes->second;
		//TCASSERT(pRes->GetRefCount() == 0);
		delete pRes;
	}
	m_LoadedResources.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::FindResources()  Private
///
///	Search the local directory for resource data files and enumerate the resources.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::FindResources()
{
	// Create the search string
	std::wstring sSearchPath = ApplicationBase::GetResourcePath();
	
	std::list<std::wstring> rdbFileList = TCBase::FindFiles( sSearchPath.c_str(), L"*.rdb" );
	
	// Go through the resource data files
	for( std::list<std::wstring>::iterator iterRDBFile = rdbFileList.begin(); iterRDBFile != rdbFileList.end(); ++iterRDBFile )
	{
		// Skip the GUI and numbers files since they are special cases
		if( *iterRDBFile == L"gui.rdb" || *iterRDBFile == L"numbers.rdb" )
			continue;

		// Load the resource file
		LoadResourceDB( (sSearchPath + *iterRDBFile).c_str() );
	}
}

void ResourceMgr::ResourceIndexItem::ToFromFile( TCBase::ISerializer& serializer )
{
	uint32 temp = resourceID;
	serializer.AddData( temp );
	resourceID = static_cast<ResourceID>( temp );

	temp = resType;
	serializer.AddData( temp );
	resType = static_cast<EResourceType>( temp );

	temp = dataOffset;
	serializer.AddData( temp );
	dataOffset = temp;

	if( serializer.InReadMode() )
		TCBase::ReadChars( serializer, szName, RES_IDX_NAME_LEN );
	else
		TCBase::WriteChars( serializer, szName, RES_IDX_NAME_LEN );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::LoadResourceDB()  Private
///
///	\param szResFile The full resource data file path
///	\returns True if the file was loaded successfuly, false otherwise
///
///	Load a resource data file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceMgr::LoadResourceDB( const wchar_t* szResFile )
{
	// Open the file
	std::ifstream inFileStream( TCBase::Narrow(szResFile).c_str(), std::ios_base::in | std::ios_base::binary );
	if( !inFileStream )
		return false;
	NetSafeSerializer serializer( &inFileStream );

	// TEST Get the file size
	/*inFile.seekg( 0, std::ios::end );
	int fileLen = inFile.tellg();
	inFile.seekg(0,std::ios::beg);*/
	
	// Read in the FourCC key
	int32 fourCCKeyVal = 0;
	serializer.AddData( fourCCKeyVal );
	//inFile.read( (char*)&fourCCKeyVal, sizeof(fourCCKeyVal) );
	FourCC fileFourCC( fourCCKeyVal );
	
	// Ensure the key is correct
	if( fileFourCC != ResourceMgr::FOURCCKEY_RESDB )
	{
		// Close the file and bail
		std::wstring sMsg = L"The file \"";
		sMsg += szResFile;
		sMsg += L"\" does not contain the resource data FourCC indentifier.";
		MsgLogger::Get().Output( sMsg );
		inFileStream.close();
		return false;
	}

	// Read in the version
	uint32 fileVer = 1;
	serializer.AddData( fileVer );
	//inFile.read( (char*)&fileVer, sizeof(fileVer) );

	// Read in the number of resources
	uint32 numResources = 0;
	serializer.AddData( numResources );
	//inFile.read( (char*)&numResources, sizeof(numResources) );

	// Ensure there are not too many resources
	if( numResources > ResourceMgr::MAX_RES_PER_DB )
	{
		// Close the file and bail
		inFileStream.close();
		MsgLogger::Get().Output( MsgLogger::MI_Error, L"This resource data file contains too many resources.  It supposedly contains %d.", numResources );
		return false;
	}

	// If the resource contains no resources then we are done
	if( numResources == 0 )
	{
		inFileStream.close();
		return false;
	}

	// Ensure there is enough data left in the file to read
	uint32 fileSize = serializer.GetInputLength();
	
	// Read in the resource index entries
	ResourceIndexItem* pResIndexEntries = new ResourceIndexItem[ numResources ];
	for( uint32 curResInfoIndex = 0; curResInfoIndex < numResources; ++curResInfoIndex )
		pResIndexEntries[curResInfoIndex].ToFromFile( serializer );

	// We are done reading the manifest data so we can close the file
	inFileStream.close();

	// Do a quick sanity check to ensure the last resource references a valid position in the file
	ResourceIndexItem& lastEntry = pResIndexEntries[ numResources - 1 ];
	if( lastEntry.dataOffset > fileSize )
	{
		// Close the file and bail
		std::wostringstream sMsg;
		sMsg << L"The file \"" << szResFile << L"\" is not a valid resource data file.";
		MsgLogger::Get().Output( sMsg.str() );
		return false;
	}

	// Store the data file name and index
	int32 newDataFileIndex = (int32)m_ResourceDataFiles.size();
	m_ResourceDataFiles.push_back( std::wstring(szResFile) );

	// Get the max resource ID
	uint32 curMaxResID = pResIndexEntries[0].resourceID;
	for( uint32 resIndex = 1; resIndex < numResources; ++resIndex )
	{
		if( pResIndexEntries[resIndex].resourceID > curMaxResID )
			curMaxResID = pResIndexEntries[resIndex].resourceID;
	}

	// Resize the array if needed
	int maxResIDArraySizeNeeded = (curMaxResID - ResourceMgr::STARTING_RES_ID) + 1;
	if( maxResIDArraySizeNeeded > (int)m_KnownResources.size() )
	{
		KnownResourceItem nullItem;
		nullItem.dataFileIndex = -1;
		nullItem.indexData.resourceID = 0;
		m_KnownResources.resize( maxResIDArraySizeNeeded, nullItem );
	}

	// Add the resource index entries to the known resources list
	for( uint32 resIndex = 0; resIndex < numResources; ++resIndex )
	{
		ResourceIndexItem& curEntry = pResIndexEntries[ resIndex ];

		// If there is a resource already with this resource ID
		int resArrayIndex = curEntry.resourceID - ResourceMgr::STARTING_RES_ID;
		if( m_KnownResources[ resArrayIndex ].dataFileIndex != -1 )
		{
			std::wostringstream outStr;
			outStr << L"The file \"" << szResFile << L"\" is trying to add a resource with the ID " << curEntry.resourceID << L" named " << curEntry.szName << L", but that resource ID is already in the known resource list with the name " << m_KnownResources[ resArrayIndex ].indexData.szName << L" so it will be skipped.";
			MsgLogger::Get().Output( outStr.str() );
			continue;
		}

		// Store the resource info
		KnownResourceItem newItem;
		newItem.indexData = curEntry;
		newItem.dataFileIndex = newDataFileIndex;
		m_KnownResources[ resArrayIndex ] = newItem;
	}

	// Free the index data
	delete [] pResIndexEntries;

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::HookupCreateFunc()  Public
///
///	\param resType The type of resource
///	\param pFunc The creation function for the resource type
/// \param retainMem Defaults to false. Indicates that the resource will manage its own memory
///         rather than be released after load.
///
///	Add a resource creation function.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::HookupCreateFunc( uint32 resType, CreateFunc* pFunc, bool retainMem )
{
	// Fill in the creation structure
	ResCreateData createData;
	createData.pCreateFunc = pFunc;
	createData.retainMemory = retainMem;

	// If the type already has a definition then overwrite the entry
	ResCreateMap::iterator iterEntry = m_ResCreateFuncs.find( resType );
	if( iterEntry != m_ResCreateFuncs.end() )
		iterEntry->second = createData;
	// Else just store the data
	else
		m_ResCreateFuncs.insert( ResCreateMap::value_type( resType, createData ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::CreateResource()  Private
///
///	\param resType The type of resource we are creating
///	\param resID The ID of the resource being loaded
///	\param dataBlock The block of memory representing the resource being loaded
///	\returns A pointer to the resource being loaded, false otherwise
///
///	Create a resource.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Resource* ResourceMgr::CreateResource( EResourceType resType, ResourceID resID, DataBlock* pDataBlock, bool* pResRefsDataBlock ) const
{
	if( pResRefsDataBlock )
		*pResRefsDataBlock = false;

	// Get the create function based on the type
	ResCreateMap::const_iterator iterResCreate = m_ResCreateFuncs.find( resType );
	if( iterResCreate == m_ResCreateFuncs.end() )
		return 0;

	// Create the resource
	ResCreateData createData = iterResCreate->second;
	Resource* pRetRes = createData.pCreateFunc( resID, pDataBlock );

	// Mark the flag notifying if the memory block can be freed
	if( createData.retainMemory && pResRefsDataBlock )
		*pResRefsDataBlock = true;

	if( !pRetRes )
	{
		// The type is unknown so return NULL
		std::wostringstream outStr;
		outStr << L"Failed to create resource with ID " << resID << L" due to unhandled type.";
		MsgLogger::Get().Output( outStr.str() );
		return NULL;
	}

	return pRetRes;
}


RefSpriteHndl ResourceMgr::GetRefSprite( ResourceID resID )
{
	Resource* pRes = GetResource( resID );
	if( pRes && pRes->GetResType() != RT_Sprite )
		return RefSpriteHndl( NULL );
	
	return RefSpriteHndl( (RefSprite*)pRes );
}

TCImageHndl ResourceMgr::GetTCImage( ResourceID resID )
{
	Resource* pRes = GetResource( resID );
	if( pRes && pRes->GetResType() != RT_Image )
		return TCImageHndl( NULL );
	
	return TCImageHndl( (TCImage*)pRes );
}

TCFontHndl ResourceMgr::GetTCFont( ResourceID resID )
{
	Resource* pRes = GetResource( resID );
	if( pRes && pRes->GetResType() != RT_Font )
		return TCFontHndl( NULL );
	
	return TCFontHndl( (TCFont*)pRes );
}

AudioSampleHndl ResourceMgr::GetAudioSample( ResourceID resID )
{
	Resource* pRes = GetResource( resID );
	if( pRes && pRes->GetResType() != RT_Sound )
		return AudioSampleHndl( NULL );
	
	return AudioSampleHndl( (AudioSample*)pRes );
}

/// Retrieve a music stream, loading if necessary
MusicStreamHndl ResourceMgr::GetMusicStream( ResourceID resID )
{
	Resource* pRes = GetResource( resID );
	if( pRes && pRes->GetResType() != RT_Music )
		return MusicStreamHndl( NULL );
	
	return MusicStreamHndl( (MusicStream*)pRes );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::ReleaseResource()  Public
///
///	\param resID The ID of the resource to release, if there are any references to the resource
///					then the resource will not be released
///
/// Free the memory associated with a loaded resource
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceMgr::ReleaseResource( ResourceID resID )
{
	if( m_LoadedResources.find( resID ) == m_LoadedResources.end() )
		return false;

	return false;
}

/// Get all of the loaded images
std::list< TCImage* > ResourceMgr::GetAllTCImages()
{
	std::list< TCImage* > retList;

	for( LoadedResMap::iterator iterRes = m_LoadedResources.begin(); iterRes != m_LoadedResources.end(); ++iterRes )
	{
		// Only use images
		Resource* pRes = iterRes->second;
		if( pRes->GetResType() != RT_Image )
			continue;
		retList.push_back( reinterpret_cast<TCImage*>( pRes ) );
	}

	return retList;
}

void ResourceMgr::ReloadImage( TCImage* pImage, bool isRecreate )
{
	// Determine if the resource ID is known
	KnownResVector::size_type resIndex = ResIDToIndex( pImage->GetResID() );

	// Get the index item
	const KnownResourceItem& resItem = m_KnownResources[ resIndex ];
	
	// Get the data file
	const std::wstring& sDataFile = m_ResourceDataFiles[ resItem.dataFileIndex ];
	
	// Open the data file
	std::ifstream inFile( TCBase::Narrow( sDataFile ).c_str(), std::ios_base::in | std::ios_base::binary );
	if( !inFile )
		return;
	NetSafeSerializer serializer( &inFile );

	// Get to the offset
	serializer.Seek( resItem.indexData.dataOffset );

	// Read in the data size
	uint32 dataSize = 0;
	serializer.AddData( dataSize );

	// Allocate memory for the data
	uint8* pData = new uint8[ dataSize ];
	if( !pData )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to allocate memory for reloading image with res ID %u", pImage->GetResID() );
		inFile.close();
		return;
	}

	// Read in the data
	serializer.AddRawData( pData, dataSize );
	NetSafeDataBlock resDataBlock( pData, dataSize );

	// We read the data we need so we can close the file
	inFile.close();

	// Parse the file data into a resource
#ifndef TOOLS
	if( isRecreate )
		g_pGraphicsMgr->RecreateImageData( pImage, &resDataBlock );
	else
		g_pGraphicsMgr->ReloadImageData( pImage, &resDataBlock );
#else
	// Prevent the warning for unreferenced parameter
	( isRecreate );
#endif

	// Free the data
	delete [] pData;
}

// Reload image data
void ResourceMgr::ReloadImageDataFunc( bool isRecreate )
{
	for( LoadedResMap::iterator iterRes = m_LoadedResources.begin(); iterRes != m_LoadedResources.end(); ++iterRes )
	{
		// Only use images
		Resource* pRes = iterRes->second;
		if( pRes->GetResType() != RT_Image )
			continue;
		ReloadImage( reinterpret_cast<TCImage*>( pRes ), isRecreate );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::ReloadImageResourceData()  Public
///
///	Reload all resources of a type.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::ReloadImageResourceData()
{
	ReloadImageDataFunc( false );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::RecreateImageResources()  Public
///
///	Recreate the TCImage resources.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::RecreateImageResources()
{
	ReloadImageDataFunc( true );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceMgr::GetResource()  Private
///
///	\param resID The ID of the resource to retrieve
///	\param forceReload True to ensure the resource is reloaded from file, otherwise the currently
///						loaded data is used
///	\returns A pointer to the resource being retrieved or NULL on failure
///
///	Retrieve an already active resource or load a new one.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Resource* ResourceMgr::GetResource( ResourceID resID, bool forceReload )
{
	// Determine if the resource ID is known
	std::vector<KnownResourceItem>::size_type resIndex = ResIDToIndex( resID );
	if( resIndex >= m_KnownResources.size() )
	{
		if( resID != 0 )
			MsgLogger::Get().Output( MsgLogger::MI_Error, L"Invalid resource ID: %u (It is out of the valid range).", resID );
		return NULL;
	}
	if( m_KnownResources[ resIndex ].dataFileIndex < 0 || (std::vector<KnownResourceItem>::size_type)m_KnownResources[ resIndex ].dataFileIndex >= m_ResourceDataFiles.size() )
	{
		MsgLogger::Get().Output( MsgLogger::MI_Error, L"Unknown resource ID: %u", resID );
		return NULL;
	}

	// See if the resource is already loaded
	if( !forceReload && m_LoadedResources.find( resID ) != m_LoadedResources.end() )
		return m_LoadedResources[ resID ];

	// Get the index item
	const KnownResourceItem& resItem = m_KnownResources[ resIndex ];
	
	// Get the data file
	const std::wstring& sDataFile = m_ResourceDataFiles[ resItem.dataFileIndex ];
	
	// Open the data file
	std::ifstream inFile( TCBase::Narrow( sDataFile ).c_str(), std::ios_base::in | std::ios_base::binary );
	if( !inFile )
		return NULL;

	NetSafeSerializer serializer( &inFile );

	// Get to the offset
	serializer.Seek( resItem.indexData.dataOffset );

	// Read in the data size
	uint32 dataSize = 0;
	serializer.AddData( dataSize );

	// Allocate memory for the data
	uint8* pData = new uint8[ dataSize ];
	if( !pData )
	{
		std::wostringstream outStr;
		outStr << L"Failed to load resource with ID " << resID << L" due to insufficient memory.";
		MsgLogger::Get().Output( outStr.str() );
		inFile.close();
		return NULL;
	}

	// Read in the data
	serializer.AddRawData( pData, dataSize );
	NetSafeDataBlock resDataBlock( pData, dataSize );

	// We read the data we need so we can close the file
	inFile.close();

	// Parse the file data into a resource
	bool resRefsDataBlock = false;
	Resource* pRes = CreateResource( resItem.indexData.resType, resID, &resDataBlock, &resRefsDataBlock );

	// Free the data if the resource does not need it
	if( !resRefsDataBlock )
		delete [] pData;

	// Only store valid resources
	if( pRes )
	{
		// Store the resource in our list of loaded resources
		LoadedResMap::iterator curIter = m_LoadedResources.find( resID );
		if( curIter == m_LoadedResources.end() )
			m_LoadedResources.insert( std::pair<ResourceID,Resource*>(resID,pRes) );
		else
			m_LoadedResources[ resID ] = pRes;
		
		// Set the name
		pRes->m_sName = resItem.indexData.szName;
	}

	// Return the resource pointer
	return pRes;
}