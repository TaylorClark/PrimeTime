//=================================================================================================
/*!
	\file GameSettings.h
	Application Project
	Game Settings Header
	\author Taylor Clark
	\Date August 9, 2011

	This header contains the definition for the application object that is used to encapsulate
	the activity of the executable.
*/
//=================================================================================================

#pragma once
#ifndef __GameSettings_h
#define __GameSettings_h

#include "Base/Types.h"
#include <string>
#include "GamePlay/GameDefines.h"
#include "Graphics2D/GraphicsMgr.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameSettings
	\brief Represents the values that are persisted between each time the game application is run.
*/
//-------------------------------------------------------------------------------------------------
class GameSettings
{
private:

	/// If something is wrong with the INI file then use defaults that skip extra prompts so we
	/// don't bother the player
	void SetToNoBotherDefaults();

	/// Set all the settings to values for a new application install
	void SetToNewDefaults();

public:

	/// The default constructor
	GameSettings() : NumTimesPlayed( 0 ),
                     IdealDisplayMode( GraphicsMgrBase::DM_NormalFill )
	{
		for( int32 gameTypeIndex = 0; gameTypeIndex < (int32)GameDefines::GT_Num_Types; ++gameTypeIndex )
			PromptedForTutorial[gameTypeIndex] = false;
	}

	/// The number of times the game has been run
	int NumTimesPlayed;

	/// The player that should be automatically logged in when the game is started
	std::wstring AutoLoginProfile;

    /// The last used display mode
    GraphicsMgrBase::EDisplayMode IdealDisplayMode;

	/// Flags indicating if the player has been tried the tutorial for a specific game type. The
	/// flag can be set if either the player is prompted or the player actually plays through a
	/// tutorial. Note that the game types start at 1, not 0, so 1 is subtract from the game type
	/// to get the array index.
	bool PromptedForTutorial[ GameDefines::GT_Num_Types ];

	/// Load the settings from file
	bool LoadFromFile();

	/// Save the settings to file
	bool SaveToFile();
};

#endif // __GameSettings_h