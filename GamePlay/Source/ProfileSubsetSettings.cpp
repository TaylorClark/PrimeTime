/*=================================================================================================

	\file ProfileSubsetSettings.cpp
	Game Play Library
	Settings Profile Subset Source
	\author Taylor Clark
	\Date July 14, 2007

	This source file contains the implementation of the game settings player profile subset.

=================================================================================================*/

#include "../ProfileSubsetSettings.h"
#include "Base/TCAssert.h"

const FourCC ProfileSubsetSettings::SUBSET_ID("STNS");


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ProfileSubsetSettings::Serialize()  Public
///
///	\param serializer The serializer that reads and writes data
///
///	Transfer the subset data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfileSubsetSettings::Serialize( Serializer& serializer )
{
	// Write the number of settings
	uint32 numSettings = (uint32)GameDefines::GT_Num_Types;
	serializer.AddData( numSettings );

	// Write the game settings
	for( uint32 settingIndex = 0; settingIndex < numSettings && settingIndex < (uint32)GameDefines::GT_Num_Types; ++settingIndex )
	{
		int32 diffLevel = (int32)m_Settings[settingIndex].diffLevel;
		serializer.AddData( diffLevel );
		m_Settings[settingIndex].diffLevel = (GameDefines::EGameplayDiffLevel)diffLevel;
		serializer.AddData( m_Settings[settingIndex].timeLimit );
	}

	// Write the practice settings
	m_PracticeSettings.Serialize( serializer );

	return true;
}

/// Set the settings for a gameplay mode
void ProfileSubsetSettings::SetSettings( GameDefines::EGameType gameType, uint32 timeLimit, GameDefines::EGameplayDiffLevel diffLevel )
{
	uint32 settingIndex = (uint32)(gameType & GameDefines::GT_Mask_Type) - 1;
	if( settingIndex >= (uint32)GameDefines::GT_Num_Types )
	{
		TCBREAKX( L"Invalid game type" );
		return;
	}
	m_Settings[settingIndex].timeLimit = timeLimit;
	m_Settings[settingIndex].diffLevel = diffLevel;
}

/// Get the settings for a gameplay mode
ProfileSubsetSettings::GameModeSettings ProfileSubsetSettings::GetSettings( GameDefines::EGameType gameType )
{
	uint32 settingIndex = (uint32)(gameType & GameDefines::GT_Mask_Type) - 1;
	if( settingIndex >= GameDefines::GT_Num_Types )
	{
		TCBREAKX( L"Invalid game type" );
		return GameModeSettings();
	}
	return m_Settings[settingIndex];
}