//=================================================================================================
/*!
	\file PlayerProfile.cpp
	Game Play Library
	Player Profile Source
	\author Taylor Clark
	\date July 29, 2006

	This source file contains the implementation for the GUI list control class.
*/
//=================================================================================================

#include "../PlayerProfile.h"
#include "Resource/ResourceMgr.h"
#include <fstream>
#include "Base/Serializer.h"
#include "../ProfileSubsetMultiplication.h"
#include "../ProfileSubsetAddition.h"
#include "../ProfileSubsetSettings.h"
#include "Base/MsgLogger.h"
#include "Base/TCAssert.h"
#include "Base/FileFuncs.h"
#include "Base/StringFuncs.h"

const wchar_t* PlayerProfile::PROFILE_EXT = L".ptp";
#ifdef WIN32
const wchar_t* PlayerProfile::PROFILE_PATH = L"Profiles\\";
#else
const wchar_t* PlayerProfile::PROFILE_PATH = L"Profiles/";
#endif
const FourCC PlayerProfile::FOURCC_PROFILE("PTPR");


static ProfileSubset* CreateSubset( FourCC subsetID )
{
	if( subsetID == ProfileSubsetAddition::SUBSET_ID )
		return new ProfileSubsetAddition();
	else if( subsetID == ProfileSubsetMultiplication::SUBSET_ID )
		return new ProfileSubsetMultiplication();
	else if( subsetID == ProfileSubsetSettings::SUBSET_ID )
		return new ProfileSubsetSettings();
	
	return 0;
}


bool PlayerProfile::IsValidProfileName( const wchar_t* szName )
{
	// Since text boxes only allow alpha numeric characters, no leading spaces, and no names
	// longer than 64 characters then ensure this file name has none of these conditions
	bool isValidName = true;
	if( wcslen(szName) < 1 || szName[0] == L' ' || wcslen(szName) >= 64 )
		isValidName = false;
	
	// Go through each character and ensure it is a valid character
	for( uint32 charIndex = 0; charIndex < wcslen(szName) && isValidName; ++charIndex )
		if( !iswalnum( (wint_t)szName[charIndex] ) && szName[charIndex] != L' ' )
			isValidName = false;

	return isValidName;
}


/// Determine if a profile exists
bool PlayerProfile::DoesProfileExist( const wchar_t* szName )
{
	// Get the player's profile file path
	std::wstring sProfilePath = GetFilePathFromName( szName );

	return TCBase::DoesFileExist( sProfilePath.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PlayerProfile::~PlayerProfile  Public
///
///	The default destructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
PlayerProfile::~PlayerProfile()
{
	// Clear the subsets
	for( SubsetMap::iterator iterSubset = m_Subsets.begin(); iterSubset != m_Subsets.end(); ++iterSubset )
		delete iterSubset->second;
	m_Subsets.clear();

	// Go through the packed subsets
	for( PackedSubsetMap::iterator iterSubset = m_PackedSubsets.begin();iterSubset != m_PackedSubsets.end(); ++iterSubset )
		delete [] (uint8*)iterSubset->second.GetPtr();
	m_PackedSubsets.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PlayerProfile::GetAllProfileNames  Static Public
///
///	\returns A list of all of the profiles found.
///
///	Get a list of all of the known profiles.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::list<std::wstring> PlayerProfile::GetAllProfileNames()
{
	std::list<std::wstring> retList;

	// Create the search string
	std::wstring sSearchFilter = PROFILE_PATH;
	sSearchFilter += L"*";
	sSearchFilter += PROFILE_EXT;

	// Find the first resource data file
	std::list<std::wstring> profileFiles = TCBase::FindFiles( TCBase::GetUserFilesPath().c_str(), sSearchFilter.c_str() );

	// Go through and find store the profile names
	for( std::list<std::wstring>::iterator iterProfileFile = profileFiles.begin(); iterProfileFile != profileFiles.end(); ++iterProfileFile )
	{
		// Remove the extension
		std::wstring sCurFile = *iterProfileFile;
		sCurFile.erase( sCurFile.length() - wcslen( PROFILE_EXT ), wcslen( PROFILE_EXT ) );

		// Ensure it is a valid name
		if( !IsValidProfileName( sCurFile.c_str() ) )
			continue;

		// Store the name
		retList.push_back( sCurFile );
	}

	// Return the list of profile names
	return retList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PlayerProfile::TransferData_Old  Public
///
///	\param serializer The serializer used to read and write data
///	\returns Whether or not the data read in correctly
///
///	Load/save a player profile.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
//bool PlayerProfile::TransferData_Old( Serializer& serializer )
//{
//	// The FourCC key
//	int32 fourCCKeyVal = FOURCC_PROFILE.ToInt32();
//	serializer.AddData( fourCCKeyVal );
//	FourCC fileFourCC( fourCCKeyVal );
//
//	// If the FourCC is invalid
//	if( fileFourCC != FOURCC_PROFILE )
//		return false;
//
//	// The file version
//	const uint32 CUR_FILE_VERSION = 1;
//	uint32 fileVersion = CUR_FILE_VERSION;
//	serializer.AddData( fileVersion );
//
//	// If we are reading in and it is not our version
//	if( serializer.InReadMode() && fileVersion != CUR_FILE_VERSION )
//	{
//		return false;
//	}
//
//	// The number of settings
//	uint32 numSettings = (uint32)GameDefines::GT_Num_Types;
//	serializer.AddData( numSettings );
//
//	// If the number of settings is rediculous then bail
//	const uint32 REDICULOUS_NUM_SETTINGS = 20;
//	if( serializer.InReadMode() && numSettings > REDICULOUS_NUM_SETTINGS )
//		return false;
//
//	// The settings
//	for( uint32 settingIndex = 0; settingIndex < numSettings && settingIndex < (uint32)GameDefines::GT_Num_Types; ++settingIndex )
//	{
//		int32 diffLevel = (int32)m_Settings[settingIndex].diffLevel;
//		serializer.AddData( diffLevel );
//		m_Settings[settingIndex].diffLevel = (GameDefines::EGameplayDiffLevel)diffLevel;
//		serializer.AddData( m_Settings[settingIndex].timeLimit );
//	}
//
//	// The number of subsets
//	uint32 numSubsets = 0;
//	for( uint32 subsetIndex = 0; subsetIndex < S_COUNT; ++subsetIndex )
//	{
//		if( m_pSubStats[subsetIndex] != 0 )
//			++numSubsets;
//	}
//	serializer.AddData( numSubsets );
//
//	// If the number of settings is rediculous then bail
//	const uint32 REDICULOUS_NUM_SUBSETS = 10;
//	if( serializer.InReadMode() && numSubsets > REDICULOUS_NUM_SUBSETS )
//		return false;
//
//	// If we are saving then make sure we go through every subset
//	if( serializer.InWriteMode() )
//		numSubsets = S_COUNT;
//
//	// Go through the subsets
//	for( uint32 subsetIndex = 0; subsetIndex < numSubsets; ++subsetIndex )
//	{
//		// If we are saving then check if we have this subset
//		if( serializer.InWriteMode() && m_pSubStats[subsetIndex] == 0 )
//			continue;
//
//		// The subset Index
//		int32 curSubsetIndex = subsetIndex;
//		serializer.AddData( curSubsetIndex );
//
//		// If the subset index is greater than our current limit then bail since this file is a
//		// new version
//		if( curSubsetIndex >= S_COUNT )
//			return false;
//
//		// If we are loading then create the subset object
//		if( serializer.InReadMode() )
//			m_pSubStats[ curSubsetIndex ] = CreateSubset( (ESubsets)curSubsetIndex );
//		ProfileSubset* pCurSubset = m_pSubStats[ curSubsetIndex ];
//		if( !pCurSubset )
//			return false;
//
//		// Transfer the subset
//		pCurSubset->Serialize( serializer );
//	}
//
//	// Return success
//	return true;
//}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PlayerProfile::TransferData  Public
///
///	\param serializer The serializer used to read and write data
///	\returns Whether or not the data read in correctly
///
///	Load/save a player profile.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool PlayerProfile::TransferData( Serializer& serializer )
{
	// The FourCC key
	int32 fourCCKeyVal = FOURCC_PROFILE.ToInt32();
	serializer.AddData( fourCCKeyVal );
	FourCC fileFourCC( fourCCKeyVal );

	// If the FourCC is invalid
	if( fileFourCC != FOURCC_PROFILE )
		return false;

	// The file version
	const uint32 CUR_FILE_VERSION = 1;
	uint32 fileVersion = CUR_FILE_VERSION;
	serializer.AddData( fileVersion );

	// If we are reading in and it is not our version
	if( serializer.InReadMode() && fileVersion != CUR_FILE_VERSION )
	{
		return false;
	}

	// If the profile is being loaded
	if( serializer.InReadMode() )
	{
		// Read the profile subsets
		const uint32 dataLen = serializer.GetInputLength();
		while( serializer.GetOffset() < dataLen )
		{
			// The subset's FourCC code
			int32 subsetFourCCVal = 0;
			serializer.AddData( subsetFourCCVal );
			FourCC subsetFourCC( subsetFourCCVal );

			// Read the subset data length
			uint32 subsetDataLen = 0;
			serializer.AddData( subsetDataLen );

			// Allocate memory for the block
			uint8* pSubsetMem = new uint8[ subsetDataLen ];

			// Read in the block
			serializer.AddRawData( pSubsetMem, subsetDataLen );

			// Store the subset as packed
			m_PackedSubsets.insert( PackedSubsetMap::value_type( subsetFourCC, DataBlock( pSubsetMem, subsetDataLen ) ) );
		}
	}
	// Else if the profile is being stored to file
	else
	{
		// Go through all of the profile subset data
		for( SubsetMap::iterator iterSubset = m_Subsets.begin(); iterSubset != m_Subsets.end(); ++iterSubset )
		{
			// Write the subset identifier
			int32 fourCCVal = iterSubset->second->GetSubsetFourCC().ToInt32();
			serializer.AddData( fourCCVal );

			// Write a placeholder value for the subset data length and store the offset
			uint32 dataLenOffset = serializer.GetOffset();
			uint32 tempSize = 0;
			serializer.AddData( tempSize );

			// Write the subset data
			iterSubset->second->Serialize( serializer );

			// Get the subset's data length and store it
			uint32 curOffset = serializer.GetOffset();
			uint32 subsetDataLen = (curOffset - dataLenOffset) - sizeof(dataLenOffset);
			serializer.Seek( dataLenOffset );
			serializer.AddData( subsetDataLen );
			serializer.Seek( curOffset );
		}

		// Go through the packed subsets
		for( PackedSubsetMap::iterator iterSubset = m_PackedSubsets.begin();iterSubset != m_PackedSubsets.end(); ++iterSubset )
		{
			// Write the info
			int32 fourCCVal = iterSubset->first.ToInt32();
			serializer.AddData( fourCCVal );

			DataBlock& dataBlock = iterSubset->second;
			uint32 dataLen = dataBlock.GetSize();
			serializer.AddData( dataLen );

			uint8* pData = (uint8*)dataBlock.GetPtr() + 0;
			serializer.AddRawData( pData, dataLen );
		}
	}

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PlayerProfile::GetFilePathFromName  Static Public
///
///	\param szProfileName The name of the player
///	\returns A string containing the full file path of where the profile's file should be found
///
///	Get a profile file path based on a profile name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring PlayerProfile::GetFilePathFromName( const wchar_t* szProfileName )
{
	// Build the path
	std::wstring sProfilePath = TCBase::GetUserFilesPath();
	sProfilePath += PROFILE_PATH;
	sProfilePath += szProfileName;
	sProfilePath += PROFILE_EXT;

	return sProfilePath;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PlayerProfile::OpenProfile  Static Public
///
///	\param szProfileName The name of the player excluding and file system information.  It must
///							be validated before being passed in.
///	\returns A pointer to the opened profile, NULL on failure
///
///	Open or create a player profile.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
PlayerProfile* PlayerProfile::OpenProfile( const wchar_t* szProfileName )
{
	if( !IsValidProfileName(szProfileName) )
		return NULL;

	// Get the player's profile file path
	std::wstring sProfilePath = GetFilePathFromName( szProfileName );

	// See if the file does not exist
	if( !TCBase::DoesFileExist( sProfilePath.c_str() ) )
	{
		// Create a new profile
		PlayerProfile* pProfile = new PlayerProfile( szProfileName );

		// Write a preliminary file
		pProfile->SaveToFile();
		return pProfile;
	}

	// Open the file
	std::ifstream inFile( TCBase::Narrow(sProfilePath).c_str(), std::ios_base::in | std::ios_base::binary );

	// If the file failed to open
	if( !inFile )
		return NULL;

	// Create the profile
	PlayerProfile* pProfile = new PlayerProfile( szProfileName );
	if( !pProfile )
		return NULL;

	// Read in the data
	Serializer serializer( &inFile );
	pProfile->TransferData( serializer );

	// Return the profile
	return pProfile;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PlayerProfile::SaveToFile  Static Public
///
/// Store a profile's data to file
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerProfile::SaveToFile()
{
	// Get the player's profile file path
	std::wstring sProfilePath = GetFilePathFromName( m_sName.c_str() );

	// Open the file
	std::ofstream outFile( TCBase::Narrow(sProfilePath).c_str(), std::ios_base::out | std::ios_base::binary );

	// If the file failed to open
	if( !outFile )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to open file for saving profile." );
		return;
	}

	// Store the data
	Serializer serializer( &outFile );
	TransferData( serializer );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PlayerProfile::GetSubset  Public
///
///	\param subsetID The four character code identifier of the subset
///	\returns A pointer to the subset, or NULL on failure
///
/// Get a profile data subset
///
///////////////////////////////////////////////////////////////////////////////////////////////////
ProfileSubset* PlayerProfile::GetSubset( FourCC subsetID )
{
	// If the subset is unknown
	if( m_Subsets.find( subsetID ) == m_Subsets.end() )
	{
		// If the subset is not packed then create it anew
		if( m_PackedSubsets.find( subsetID ) == m_PackedSubsets.end() )
		{
			ProfileSubset* pNewSubset = ::CreateSubset( subsetID );
			if( pNewSubset )
				m_Subsets.insert( SubsetMap::value_type( subsetID, pNewSubset ) );
			return pNewSubset;
		}

		// Get the subset data
		DataBlock subsetData = m_PackedSubsets[ subsetID ];
		uint8* pOrigData = (uint8*)subsetData.GetPtr();

		// Create the subset
		ProfileSubset* pNewSubset = ::CreateSubset( subsetID );
		if( pNewSubset )
		{
			// Read the data
			Serializer reader( &subsetData );
			pNewSubset->Serialize( reader );

			// Add the entry to the normal map
			m_Subsets.insert( SubsetMap::value_type( subsetID, pNewSubset ) );
		}

		// Free the subset memory and remove the entry from the map
		delete [] pOrigData;
		m_PackedSubsets.erase( subsetID );

		// If the subset was not loaded then bail
		if( !pNewSubset )
			return 0;
	}

	// Return the subset
	return m_Subsets[ subsetID ];
}

/// Ensure the profile directory exists
void PlayerProfile::EnsureProfileDirExists()
{
	// Build the path
	std::wstring sProfilePath = TCBase::GetUserFilesPath();
	sProfilePath += PROFILE_PATH;

	// If the profile directory exists then bail
	if( TCBase::DoesFileExist(sProfilePath.c_str()) )
		return;

	// Create the directory
	TCBase::CreateDir(sProfilePath.c_str());
}