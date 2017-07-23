//=================================================================================================
/*!
	\file ResourceMgr.h
	Resources Library
	Resource Manager Header
	\author Taylor Clark
	\date March 3, 2006

	This header contains the definition for the resource manager class.
*/
//=================================================================================================

#pragma once
#ifndef __ResourceMgr_h
#define __ResourceMgr_h

#include "Resource.h"
#include <vector>
#include <list>
#include <map>
#include "Base/FourCC.h"
#include "Base/ISerializer.h"
#include "Graphics2D/RefSprite.h"
#include "Graphics2D/TCImage.h"
#include "Graphics2D/TCFont.h"
#include "Audio/SoundMusic.h"

class DataBlock;


/// A resource creation function
typedef Resource* (CreateFunc)( ResourceID, DataBlock* );


//-------------------------------------------------------------------------------------------------
/*!
	\class ResourceMgr
	\brief Provides the functionality to load resources.

	This class defines the resource manager that is used to load resources and free resources,
	such as images and sounds rather than memory blocks.
*/
//-------------------------------------------------------------------------------------------------
class ResourceMgr
{
public:

	/// The number of characters in the name 
	static const uint32 RES_IDX_NAME_LEN = 24;

	/// The first resource ID
	static const uint32 STARTING_RES_ID = 100;

#pragma pack(push,1)
	/// An entry into the resource index
	struct ResourceIndexItem
	{
		// The default constructor to initialize the data
		ResourceIndexItem() : resourceID(0),
							resType( RT_Error ),
							dataOffset( 0 )
		{
			szName[0] = 0;
		}

		// The ID of this resource
		ResourceID resourceID;

		// The type of this resource
		EResourceType resType;

		// The offset within the file to the data of this resource
		uint32 dataOffset;

		// The name of the resource
		wchar_t szName[ RES_IDX_NAME_LEN ];

		void ToFromFile( TCBase::ISerializer& serializer );
	};
#pragma pack(pop)
	
	/// A known resource
	struct KnownResourceItem
	{
		// The default constructor to initialize the data
		KnownResourceItem() : indexData(),
								dataFileIndex( -1 )
		{
		}

		/// The index data
		ResourceIndexItem indexData;

		/// The data file to find the resource
		int32 dataFileIndex;
	};
	typedef std::vector< KnownResourceItem > KnownResVector;

	/// A structure defining how a resource is created
	struct ResCreateData
	{
		/// The creation function
		CreateFunc* pCreateFunc;

		/// If this resource type requires reference to the memory passed into the creation function
		bool retainMemory;
	};

	typedef std::map< uint32, ResCreateData > ResCreateMap;

private:

	/// The list of known resources
	KnownResVector m_KnownResources;

	/// The list of known data files
	std::vector< std::wstring > m_ResourceDataFiles;

	/// The map of resource type to create function
	ResCreateMap m_ResCreateFuncs;

	/// Find all resources in the local directory
	void FindResources();

	/// Load a resource database
	bool LoadResourceDB( const wchar_t* szResFile );

	/// Load a resource for use
	Resource* GetResource( ResourceID resID, bool forceReload = false );

	/// The constructor is private because this class uses the singleton pattern
	ResourceMgr(){}


	/// Create a resource based on the type
	Resource* CreateResource( EResourceType resType, ResourceID resID, DataBlock* pDataBlock, bool* pResRefsDataBlock ) const;	

	/// Convert a resource ID to known resource vector index
	KnownResVector::size_type ResIDToIndex( ResourceID resID ){ return resID - ResourceMgr::STARTING_RES_ID; }

	/// The map of already loaded resources
	typedef std::map< ResourceID, Resource* > LoadedResMap;
	LoadedResMap m_LoadedResources;

	/// The game path
	//mutable std::wstring m_sAppPath;

	// Reload image data
	void ReloadImageDataFunc( bool isRecreate );

public:

	/// The accessor for the one and only instance of the class
	static ResourceMgr& Get()
	{
		static ResourceMgr s_ResMgr;
		return s_ResMgr;
	}

	/// The maximum number of resources per resource data file
	static const int MAX_RES_PER_DB = 128;

	/// The resource data file FourCC key
	static const FourCC FOURCCKEY_RESDB;

	/// Add a resource type creation function
	void HookupCreateFunc( uint32 typeID, CreateFunc* pFunc, bool retainMem = false );

	/// Retrieve a sprite, loading if necessary
	RefSpriteHndl GetRefSprite( ResourceID resID );

	/// Retrieve an image, loading if necessary
	TCImageHndl GetTCImage( ResourceID resID );

	/// Reload an image
	void ReloadImage( TCImage* pImage, bool isRecreate );

	/// Retrieve a font, loading if necessary
	TCFontHndl GetTCFont( ResourceID resID );

	/// Retrieve a sound, loading if necessary
	AudioSampleHndl GetAudioSample( ResourceID resID );

	/// Retrieve a music stream, loading if necessary
	MusicStreamHndl GetMusicStream( ResourceID resID );

	/// Free the memory associated with a loaded resource
	bool ReleaseResource( ResourceID resID );

	/// Get all of the loaded images
	std::list< TCImage* > GetAllTCImages();

	/// Initialize the resource manager and find the resource files
	void Init();

	/// Release all resources and memory that was loaded
	void Term();

	/// Check if a resource type is a game resource or independent resource
	static bool IsGameResource( EResourceType resType )
	{
		// Handle the resource type
		switch( resType )
		{
		case RT_Font:
		case RT_Sprite:
            return true;
                
        default:
            // Stop warnings on mac
            break;
		}

		// If we didn't return true yet then it is not a game resource
		return false;
	}

	/// Get the resource path
	//const std::wstring& GetApplicationPath() const;

	/// Get the number of known resources
	uint32 GetNumResources() const { return (uint32)m_KnownResources.size(); }

	/// Reload image data for TCImage resources
	void ReloadImageResourceData();
	
	/// Reload image resources
	void RecreateImageResources();

#ifdef _DEBUG

	/// Get all of the known resources
	std::list<ResourceIndexItem> GetAllFonts();

	const KnownResVector& GetKnownRes() const { return m_KnownResources; }

#endif
};

#endif // __ResourceMgr_h