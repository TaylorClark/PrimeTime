//=========4========================================================================================
/*!
	\file PTDefines.h
	Prime Time
	Defines Header
	\author Taylor Clark
	\date May 21, 2007

	This file contains the defines useful to Prime Time and the related tools.
*/
//=================================================================================================

#pragma once
#ifndef __PTDefines_h
#define __PTDefines_h

namespace PTDefines
{
	// If you modify either of these strings, make sure to update the Uninstall.au3 file immediately
	extern const wchar_t* DUMP_PRE_FILE_NAME;
	extern const wchar_t* DUMP_FILE_EXT;
	extern const wchar_t* LOG_PRE_FILE_NAME;
	extern const wchar_t* LOG_FILE_EXT;

	extern const char* UPDATER_OPTION_SILENT;
	extern const wchar_t* WUPDATER_OPTION_SILENT;

	// The folder to which files are downloaded
	extern const wchar_t* UPDATE_FOLDER;

	// The updater executable file name
	extern const char* UPDATER_EXE_FILE;
	extern const wchar_t* WUPDATER_EXE_FILE;

	// The game executable file name
	extern const char* GAME_EXE_FILE;
	extern const wchar_t* WGAME_EXE_FILE;
};

#endif // __PTDefines_h