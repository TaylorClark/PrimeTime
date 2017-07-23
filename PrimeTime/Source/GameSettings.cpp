#include "../GameSettings.h"
#include "Base/SimpleIni.h"
#include "Base/FileFuncs.h"


const wchar_t* Ini_FileName = L"PrimeTime.ini";
const wchar_t* Ini_Section_Main = L"Main";
const wchar_t* Ini_Value_NumTimesPlayed = L"NumTimesPlayed";
const wchar_t* Ini_Value_AutoLogin = L"AutoLogin";
const wchar_t* Ini_Value_PromptFlags = L"PromptFlags";
const wchar_t* Ini_Value_IdealDisplayMode = L"IdealDisplayMode";

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
//	GameSettings::LoadFromFile()  Public
///	\returns True if the INI file was read successfully
///
/// Load the setting values from the INI file
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameSettings::LoadFromFile()
{
#ifdef WIN32
	std::wstring sIniFilePath = TCBase::GetUserFilesPath() + Ini_FileName;
#else
	std::string sIniFilePath = TCBase::Narrow( TCBase::GetUserFilesPath() + Ini_FileName );
#endif

	if( !TCBase::DoesFileExist(sIniFilePath.c_str()) )
	{
		// If the file doesn't exist yet, then try to create it
		CSimpleIniCaseW tempFile;
		if( tempFile.SaveFile( sIniFilePath.c_str() ) < 0 )
		{
			// For some reason we can't create the INI file so fail, but don't bother the player
			SetToNoBotherDefaults();
			return false;
		}

		SetToNewDefaults();
		return true;
	}

	CSimpleIniCaseW iniFile;
	if( iniFile.LoadFile( sIniFilePath.c_str() ) < 0 )
		return false;

	// Update the number of times played
#ifdef WIN32
	NumTimesPlayed = _wtoi( iniFile.GetValue(Ini_Section_Main, Ini_Value_NumTimesPlayed, L"0") );
#else
	NumTimesPlayed = atoi( TCBase::Narrow(iniFile.GetValue(Ini_Section_Main, Ini_Value_NumTimesPlayed, L"0")).c_str() );
#endif

	// Get if the user wants to auto-login a user
#ifdef WIN32
	const wchar_t* szProfileValue = iniFile.GetValue( Ini_Section_Main, Ini_Value_AutoLogin );
	if( szProfileValue )
		AutoLoginProfile = szProfileValue;
#endif

	// Get the prompt flags
	const wchar_t* szPromptFlags = iniFile.GetValue( Ini_Section_Main, Ini_Value_PromptFlags );
	if( szPromptFlags )
	{
		std::vector<std::wstring> flags = TCBase::Split( szPromptFlags, L',' );

		int numFlags = min( (int32)flags.size(), (int32)GameDefines::GT_Num_Types );
		for( int curGameTypeIndex = 0; curGameTypeIndex < numFlags; ++curGameTypeIndex )
			PromptedForTutorial[ curGameTypeIndex ] = flags[curGameTypeIndex] == L"1";
	}
	else if( NumTimesPlayed > 1 )
	{
		// If the prompt flags setting isn't set, but the game has been played before, then assume
		// the player has been prompted so we're not popping up irratiting message boxes
		for( int32 gameTypeIndex = 0; gameTypeIndex < (int32)GameDefines::GT_Num_Types; ++gameTypeIndex )
			PromptedForTutorial[gameTypeIndex] = true;
	}

#ifdef WIN32
	IdealDisplayMode = (GraphicsMgrBase::EDisplayMode)_wtoi( iniFile.GetValue(Ini_Section_Main, Ini_Value_IdealDisplayMode, L"0") );
#else
	IdealDisplayMode = (GraphicsMgrBase::EDisplayMode)atoi( TCBase::Narrow(iniFile.GetValue(Ini_Section_Main, Ini_Value_IdealDisplayMode, L"0")).c_str() );
#endif

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	GameSettings::SetToNoBotherDefaults()  Private
///
/// If something is wrong with the INI file then use defaults that skip extra prompts so we
/// don't bother the player
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSettings::SetToNoBotherDefaults()
{
	for( int32 gameTypeIndex = 0; gameTypeIndex < (int32)GameDefines::GT_Num_Types; ++gameTypeIndex )
		PromptedForTutorial[gameTypeIndex] = true;
	NumTimesPlayed = 2;
	AutoLoginProfile = L"";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	GameSettings::SetToNewDefaults()  Private
///
/// Set all the settings to values for a new application install
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSettings::SetToNewDefaults()
{
	for( int32 gameTypeIndex = 0; gameTypeIndex < (int32)GameDefines::GT_Num_Types; ++gameTypeIndex )
		PromptedForTutorial[gameTypeIndex] = false;
	NumTimesPlayed = 0;
	AutoLoginProfile = L"";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	GameSettings::SaveToFile()  Public
///	\returns True if the INI file was saved successfully
///
/// Save the settings to file
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameSettings::SaveToFile()
{
	#ifdef WIN32
	std::wstring sIniFilePath = TCBase::GetUserFilesPath() + Ini_FileName;
#else
	std::string sIniFilePath = TCBase::Narrow( TCBase::GetUserFilesPath() + Ini_FileName );
#endif
	
	if( !TCBase::DoesFileExist(sIniFilePath.c_str()) )
		return false;

	// First load the file so we don't clear any unused values
	CSimpleIniCaseW iniFile;
	if( iniFile.LoadFile( sIniFilePath.c_str() ) < 0 )
		return false;

	// Build the string that stores which tutorials have been prompted for
	std::wstring flagString;
	for( int32 curGameTypeIndex = 0; curGameTypeIndex < (int32)GameDefines::GT_Num_Types; ++curGameTypeIndex )
	{
		if( curGameTypeIndex > 0 )
			flagString += L",";

		flagString += PromptedForTutorial[curGameTypeIndex] ? L"1" : L"0";
	}

	// Store the values
	iniFile.SetLongValue( Ini_Section_Main, Ini_Value_NumTimesPlayed, NumTimesPlayed );
	iniFile.SetValue( Ini_Section_Main, Ini_Value_AutoLogin, AutoLoginProfile.c_str() );
	iniFile.SetValue( Ini_Section_Main, Ini_Value_PromptFlags, flagString.c_str() );
    iniFile.SetLongValue( Ini_Section_Main, Ini_Value_IdealDisplayMode, (int)IdealDisplayMode );

	// Save the file
	return iniFile.SaveFile( sIniFilePath.c_str() ) >= 0;
}