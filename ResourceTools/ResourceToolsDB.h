//=================================================================================================
/*!
	\file ResourceDB.h
	Resource Manager Tool
	Resource Database Source
	\author Taylor Clark
	\date March 3, 2006

	This header contains the definition for the resource base class.
*/
//=================================================================================================

#pragma once
#ifndef __ResourceDB_h
#define __ResourceDB_h

#include <list>
#include <string>
#include "Resource/ResourceMgr.h"
#include "Base/FourCC.h"

namespace TCBase
{
	class Serializer;
}

typedef void (*ProgressCB)( float32 normProgress, const wchar_t* szFile );

//-------------------------------------------------------------------------------------------------
/*!
	\class ResourceDB
	\brief Provides the functionality resource database
*/
//-------------------------------------------------------------------------------------------------
class ResourceToolsDB
{
public:

	typedef std::map< unsigned int, std::wstring > ExportMap;

	/// The information about a resource that is to be compiled
	struct ToolsResourceInfo
	{
		ToolsResourceInfo() : resType( RT_Error ),
								resID(0),
								imgResType( IRT_Bitmap )
		{}

		// The file path relative to the resource root path
		std::wstring sFilePath;

		// The resource type
		EResourceType resType;

		// The resource name
		std::wstring sName;

		// The ID of this resource
		ResourceID resID;

		// The resource header #define identifier
		std::wstring sResHeaderDefine;
		
		// The image type
		EImageResourceType imgResType;
	};

	typedef std::list< ToolsResourceInfo > ResourceList;

	/// The information at the beginning of all game resource files
	struct GameResourceFileInfo
	{
		GameResourceFileInfo() : fourCCKey( 0 ),
									resType( RT_Error )
		{
			memset( sName, 0, sizeof(wchar_t) * ResourceMgr::RES_IDX_NAME_LEN);
		}

		/// The fourCC key
		FourCC fourCCKey;

		/// The resource type
		EResourceType resType;

		/// The name of the resource
		wchar_t sName[ResourceMgr::RES_IDX_NAME_LEN];

		void Transfer( TCBase::ISerializer& serializer );
	};

private:

	/// The current version of the tools resource index file
	static const uint32 TOOLSDB_VERSION = 2;

	/// The current version of the exported resource data files
	static const uint32 RES_DB_VER = 1;

	/// The list of files to add to this database
	ResourceList m_ResourceList;

	/// The next resource ID
	uint32 m_NextResID;

	/// The root path for resources
	std::wstring m_sResRootPath;

	/// Save/load the index data
	void TransferToolsIndex( TCBase::ISerializer& serializer );

	/// Version 1 index file
	void TransferToolsIndexV1( TCBase::ISerializer& serializer );

	/// Version 2 index file
	void TransferToolsIndexV2( TCBase::ISerializer& serializer );

	/// Get the maximum resource ID
	uint32 GetMaxResID() const;

public:

	/// The default constructor
	ResourceToolsDB()
	{
		m_NextResID = ResourceMgr::STARTING_RES_ID;
	}

	/// The resource file FourCC key
	static const FourCC FOURCCKEY_RESFILE;

	static const wchar_t* REGPATH_RootKeyPath;
	static const wchar_t* REGPATH_ResPath;
	static const wchar_t* REGPATH_GamePath;
	static const wchar_t* FILENAME_ToolsResIndex;

	/// Add a file name to be compiled into the database
	bool AddFile( const std::wstring& sFilePath, ToolsResourceInfo* pRetInfo );

	/// Remove a resource
	bool RemoveResource( int resID );

	/// Get a descriptive string for resource type
	static const wchar_t* ResTypeEnumToString( EResourceType type );

	/// Compile the files into a database
	void CompileFiles( const std::wstring& sOutPath, const ExportMap& outputMap, ProgressCB pProgressCB ) throw(...);

	/// Save a tools version of the database
	bool SaveToolsIndex( const std::wstring& sOutFile );
	
	/// Load a tools resource database
	bool LoadToolsIndex( const wchar_t* szInFileName );

	/// Pack a resource into a memory block ready for insertion into a resource database
	void PackData( const ToolsResourceInfo& resInfo, uint8** ppData, uint32* pDataLen ) const throw(...);

	/// Retrieve information about a resource file
	static bool GetResFileInfo( const std::wstring& sFileName, GameResourceFileInfo& retItem );

	/// Get the list of resources
	const ResourceList& GetResList() const { return m_ResourceList; }

	/// Get the number of resources in the database of a specified type
	int GetNumResOfType( EResourceType resType ) const;

	/// Set the resources base path
	void SetResPath( const wchar_t* szNewPath ){ m_sResRootPath = szNewPath; }

	/// Get the current resource base path
	const std::wstring& GetResPath() const { return m_sResRootPath; }

	/// Get the info about a resource ID
	bool GetResInfo( ResourceID resID, ToolsResourceInfo& resInfo );

	/// Update a resource entry
	void UpdateResourceInfo( ResourceID oldResID, const ToolsResourceInfo& newInfo );

	/// Get the #define for a resource ID
	static std::wstring ConvertResToHeaderDefine( const ResourceToolsDB::ToolsResourceInfo& res );

	/// Clear the tool resource database
	void Clear();

	/// Get the nex resource ID
	int GetNextResID(){ return m_NextResID; }

	/// Set the next resource ID if it is valid
	bool SetNextResID( uint32 nextResID );
};

#endif // __Resource_h