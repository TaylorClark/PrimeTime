//=================================================================================================
/*!
	\file PlayerProfile.h
	Game Play Library
	Player Profile Header
	\author Taylor Clark
	\date July 1, 2006

	This header contains the definition for the player profile class.
*/
//=================================================================================================

#pragma once
#ifndef __PlayerProfile_h
#define __PlayerProfile_h

#include "Base/Types.h"
#include <string>
#include <list>
#include <map>
#include "Base/FourCC.h"
#include "GameDefines.h"


class ProfileSubset;
class Serializer;


// Ignore the warning for ungeneratable assignment operator
#pragma warning( push )
#pragma warning( disable : 4512 )

//-------------------------------------------------------------------------------------------------
/*!
	\class PlayerProfile
	\brief Represents a player profile

	This class defines a player profile object that store miscellaneous stats.
*/
//-------------------------------------------------------------------------------------------------
class PlayerProfile
{
public:

	/// The player profile file extension with the leading .
	static const wchar_t* PROFILE_EXT;

	/// The player profile file path from the game path
	static const wchar_t* PROFILE_PATH;

	/// The profile file fourCC code
 	static const FourCC FOURCC_PROFILE;

private:

	/// The player name
	const std::wstring m_sName;

	/// The stat subsets
	typedef std::map<FourCC,ProfileSubset*> SubsetMap;
	SubsetMap m_Subsets;

	/// The packed subsets
	typedef std::map<FourCC,DataBlock> PackedSubsetMap;
	PackedSubsetMap m_PackedSubsets;

	/// Load/save the profile data
	//bool TransferData_Old( Serializer& serializer );

	/// Load/save the profile data
	bool TransferData( Serializer& serializer );

public:

	/// The default constructor, private so the caller needs to use the OpenProfile() function
	PlayerProfile( const wchar_t* szName ) : m_sName( szName )
	{
	}

	/// The default destructor
	virtual ~PlayerProfile();

	/// Get a profile data subset
	ProfileSubset* GetSubset( FourCC subsetID );

	/// Store a profile's data to file
	void SaveToFile();

	/// Get the player's name
	const std::wstring& GetName() const { return m_sName; }

	/// Get all known profiles
	static std::list<std::wstring> GetAllProfileNames();

	/// Open or create a player profile
	static PlayerProfile* OpenProfile( const wchar_t* szProfileName );

	/// Get a profile file path based on a profile name
	static std::wstring GetFilePathFromName( const wchar_t* szProfileName );

	/// Test if a name is a valid profile name
	static bool IsValidProfileName( const wchar_t* szName );

	/// Determine if a profile exists
	static bool DoesProfileExist( const wchar_t* szName );

	/// If the profile directory does not exist then create it
	static void EnsureProfileDirExists();
};

#pragma warning( pop )

#endif // 