//=================================================================================================
/*!
	\file ProfileSubsetSettings.h
	Game Play Library
	Base Profile Subset Header
	\author Taylor Clark
	\date July 14, 2007

	This header contains the definition for the game settings profile subset class.
*/
//=================================================================================================

#pragma once
#ifndef __ProfileSubsetSettings_h
#define __ProfileSubsetSettings_h

#include "ProfileSubset.h"
#include "GamePlay/GameDefines.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class ProfileSubsetSettings
	\brief Represents the profile subset base class
*/
//-------------------------------------------------------------------------------------------------
class ProfileSubsetSettings : public ProfileSubset
{
public:
	/// The FourCC identifier for this subset
	static const FourCC SUBSET_ID;

	struct GameModeSettings
	{
		/// The default constructor
		GameModeSettings() : timeLimit( 0 ),
			diffLevel( GameDefines::GPDL_5 )
		{}

		uint32 timeLimit;
		GameDefines::EGameplayDiffLevel diffLevel;
	};

private:

	/// The settings
	GameModeSettings m_Settings[ GameDefines::GT_Num_Types ];

public:

	/// The filter used to populate the practice settings
	GameDefines::ProductSubsetFilter m_PracticeSettings;

	ProfileSubsetSettings()
	{
		// Set the default time limit for Addition Attack
		uint32 addSetting = (uint32)GameDefines::GT_AdditionAttack - 1;
		m_Settings[ addSetting ].timeLimit = 5 * 60;
	}

	/// The destructor, virtual to ensure that derived class are destructed properly
	virtual ~ProfileSubsetSettings(){}

	/// Get the FourCC for this subset
	virtual FourCC GetSubsetFourCC() const { return SUBSET_ID; }

	/// Load/save the subset data
	virtual bool Serialize( Serializer& serializer );

	/// Set the settings for a gameplay mode
	void SetSettings( GameDefines::EGameType gameType, uint32 timeLimit, GameDefines::EGameplayDiffLevel diffLevel );

	/// Get the settings for a gameplay mode
	GameModeSettings GetSettings( GameDefines::EGameType gameType );
};

#endif // __ProfileSubsetSettings_h