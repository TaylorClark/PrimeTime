//=================================================================================================
/*!
	\file Main.cpp
	Prime Time Application Source
	\author Taylor Clark
	\date May 21, 2007

	This source file contains the entry point for the application.
*/
//=================================================================================================

#include "Base/Types.h"
#include <windows.h>
#include "Base/StringFuncs.h"
#include "Base/FileFuncs.h"
#include "Base/NumFuncs.h"
#include <tchar.h>
#include <io.h>
#include "Base/PTDefines.h"
#include <time.h>
#include <fstream>

#ifdef _DEBUG
#define VLD_AGGREGATE_DUPLICATES
//#include <vld.h>
#endif


// Auto-generated, don't mess with this
// This message says "Playable Demo"
//const wchar_t CUST_MSG[76] = { 0xFFFF, 0x0F0F, 0x88BF, 0xD431, 25806, 26676, 4537, 13, 240, 324, 291, 363, 291, 294, 324, 303, 96, 204, 303, 327, 333, 33325, 27509, 26478, 20373, 48595, 3427, 20888, 279, 14610, 43871, 51145, 609, 27121, 48569, 15838, 36382, 20384, 33871, 18277, 44250, 44541, 44409, 20635, 41101, 21733, 23687, 43119, 40033, 14952, 33851, 17692, 19426, 4767, 34793, 16222, 5937, 21892, 5173, 19048, 46400, 43501, 49332, 50361, 20951, 36880, 53052, 4173, 51876, 38617, 51790, 23326, 6084, 19088, 48306, 25876 };

// This message says "Gameplay Demo"
//const wchar_t CUST_MSG[76] = { 0xFFFF, 0x0F0F, 0x88BF, 0xD431, 31607, 4046, 11956, 13, 213, 291, 327, 303, 336, 324, 291, 363, 96, 204, 303, 327,333, 51946, 12738, 54142, 50028, 49158, 1908, 53030, 14717, 29750,39405, 17944, 51261, 40453, 14819, 24194, 19033, 10575, 41524, 12951,21604, 47213, 9559, 18830, 7135, 14587, 33416, 40343, 46039, 53415,26027, 17236, 32091, 13028, 53591, 22571, 3166, 37824, 9812, 44084,10870, 35051, 50692, 52440, 35357, 19696, 12809, 38475, 44425, 50355,23771, 12249, 22274, 18661, 36768, 30623 };

// ' ' (An empty message)
// Don't modify  0xFFFF, 0x000F, 0x9AB5, 0xD431, it's used to find the custom message data
const wchar_t CUST_MSG[76] = { 0xFFFF, 0x000F, 0x9AB5, 0xD431, 18884, 22020, 50873, 1, 96, 29883, 52221, 17697, 2504, 46268, 23630, 49804, 36415, 40871, 26827, 18954, 52856, 13531, 19154, 12360, 26345, 17575, 2520, 16094, 2921, 24324, 36394, 48084, 6813, 2399, 48777, 47993, 39409, 34390, 49391, 17467, 51274, 13666, 40167, 10592, 23377, 14305, 17088, 4005, 12065, 22734, 50984, 47689, 3143, 36938, 16966, 2532, 34939, 43777, 40339, 25017, 42894, 2795, 19104, 26554, 5048, 4142, 10889, 110, 9050, 41517, 10400, 13584, 45806, 20919, 22006, 39195 };

extern uint8 PictureData[];

// Don't modify  0xFFFF, 0x0F0F, 0x88BF, 0xD431, it's used to find the custom message data
const wchar_t CUST_PIC_MSG[76] = { 0xFFFF, 0x0F0F, 0x88BF, 0xD431, 18884, 22020, 50873, 1, 96, 29883, 52221, 17697, 2504, 46268, 23630, 49804, 36415, 40871, 26827, 18954, 52856, 13531, 19154, 12360, 26345, 17575, 2520, 16094, 2921, 24324, 36394, 48084, 6813, 2399, 48777, 47993, 39409, 34390, 49391, 17467, 51274, 13666, 40167, 10592, 23377, 14305, 17088, 4005, 12065, 22734, 50984, 47689, 3143, 36938, 16966, 2532, 34939, 43777, 40339, 25017, 42894, 2795, 19104, 26554, 5048, 4142, 10889, 110, 9050, 41517, 10400, 13584, 45806, 20919, 22006, 39195 };

// Index 4 is year of expiration + 2010, index 5 is month 1-12, index 6 is day 1-31
// These are 16 bit to make the customization code simpler
const uint16 ExpirationDate[10] = { 0x6543, 0x5432, 0x5321, 0x3210, 3, 6, 30, 0xF8F9, 0xFFFF, 0xFFFF };
	
// This value here is a place holder, the Prime Time store will replace the value within the
// EXE file with a generated key
const uint64 keys[] = {0xffff33a45bcc8009, 0x0000000100000002, 0xaadc0a081dffb05e, 0x0101020103010002};

#ifdef _DEBUG
/*
#ifdef WINDOWS
const LPCTSTR szDLLName = _T("PrimeTime_dbg.dll");
#elif SFML
const LPCTSTR szDLLName = _T("PrimeTime_sf_dbg.dll");
#endif
*/
const LPCTSTR szDLLName = _T("PrimeTime_dbg.dll");
#else

// What the heck!?! The ifdef WINDOWS right below never works
//const LPCTSTR szDLLName = _T("PrimeTime.dll");

/*
#ifdef WINDOWS
const LPCTSTR szDLLName = _T("PrimeTime.dll");
#elif SFML
const LPCTSTR szDLLName = _T("PrimeTime_sf.dll");
#endif
*/
const LPCTSTR szDLLName = _T("PrimeTime.dll");

#endif


const wchar_t* PARAM_NOUPDATE = L"noupdate";

/// The function type to run the game
typedef int (*RunGameFunc)( HINSTANCE, const TCBase::ParamList&, uint64 gameKey, const wchar_t* szCustomText, const uint8* pCustomPic, const wchar_t* szCustomPicText );

/// The function type to allow the game DLL to validate the key
typedef uint32 (*AAAAAaaaaa)( uint64 val );

/// Check if there are updated files to use
//bool IsUpdateDownloaded();

/// Run the auto-updater
void RunUpdater();

/// Get a handle to the game DLL
HMODULE GetPrimeTimeDLL();

bool EvaluationExpirationDateChecker();


///////////////////////////////////////////////////////////////////////////////////////////////////
//  WinMain()  Global
///
///	Program execution begins in this function.  The window class is registered and the window is
///	created.
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WINDOWS
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
#else
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int )
#endif
{
#ifdef WINDOWS
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nShowCmd);
#endif

	// Parse the command line parameters
	TCBase::ParamList cmdLineParams;
	cmdLineParams.ParseCmdLineOptions( lpCmdLine );
	
#ifndef _PT_DEMO
	// If the "don't update" option is not specified then run the updater
	//if( !cmdLineParams.HasOption( PARAM_NOUPDATE ) )
	//	RunUpdater();
#endif

#ifdef EVALUATION
	if( !EvaluationExpirationDateChecker() )
		return 0;
#endif
	

	// Load the game DLL
	HMODULE hGameDLL = GetPrimeTimeDLL();
	if( !hGameDLL )
		return 0;

	// Get the game function 
	RunGameFunc pRunGameFunc = (RunGameFunc)GetProcAddress( hGameDLL, "RunGame" );
	if( !pRunGameFunc )
	{
		MessageBox( NULL, _T("Invalid PrimeTime.dll file, please reinstall Prime Time: Math Adventure."), _T("Prime Time: Math Adventure"), MB_OK | MB_ICONSTOP );
		return 0;
	}

	// Get the key validation function
	AAAAAaaaaa pCheckFunc = (AAAAAaaaaa)GetProcAddress( hGameDLL, "AAAAAaaaaa" );
	if( !pCheckFunc )
	{
		MessageBox( NULL, _T("Invalid PrimeTime.dll file, please reinstall Prime Time: Math Adventure."), _T("Prime Time: Math Adventure"), MB_OK | MB_ICONSTOP );
		return 0;
	}
	
	// Call the check key function in the game DLL to validate if the key is valid
	uint32 IS_VALID_FLAG = 1 << 17;
	if( (pCheckFunc( keys[2] ) & IS_VALID_FLAG) == 0 )
	{
		std::string sMsg = _T("You have an invalid copy of Prime Time, please e-mail support to correct the problem. ");
		sMsg += _T("Error value ");
		sMsg += TCBase::Narrow( TCBase::EasyUI64ToA( keys[2] ) );

		MessageBox( NULL, sMsg.c_str(), _T("Prime Time: Math Adventure"), MB_OK | MB_ICONSTOP );
		return 0;
	}

	// Run the game
	pRunGameFunc( hInstance, cmdLineParams, keys[2], (const wchar_t*)CUST_MSG + 7, PictureData + 14, (const wchar_t*)CUST_PIC_MSG + 7 );

	// Remove our reference to the game DLL
	try
	{
		FreeLibrary( hGameDLL );
	}
	catch( ... )
	{
	}

	return 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// EvaluationExpirationDateChecker Global
/// \returns True if play is allowed, false if the game is expired
///
/// Prevent the game from being played if it's passed its expiration date.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool EvaluationExpirationDateChecker()
{
	std::wstring sReferenceToPreventRemoval( (wchar_t*)ExpirationDate );

	// Get the current date
	time_t curTime = time(NULL);
	tm curTm;
	localtime_s( &curTm, &curTime );
	int curYear = curTm.tm_year + 1900;
	int curMonth = curTm.tm_mon + 1;
	int curDay = curTm.tm_mday;

	// Get the expiration date
	int32 expirationYear = 2010 + ExpirationDate[4];
	int32 expirationMonth = ExpirationDate[5];
	int32 expirationDay = ExpirationDate[6];

	// If we're in the end year
	bool allowPlay = true;
	if( curYear == expirationYear )
	{
		// If we're in March
		if( curMonth == expirationMonth )
		{
			// If it's the last day
			if( curDay < expirationDay )
				MessageBox( NULL, _T("Don't forget, this evaluation copy of Prime Time expires at the end of this month. Please visit www.PrimeTimeMath.com for information on how to buy a full copy that never expires."), _T("Prime Time: Math Adventure"), MB_OK | MB_ICONSTOP );
			else
				allowPlay = false;
		}
		else if( curMonth > expirationMonth )
			allowPlay = false;
	}
	else if( curYear > expirationYear )
		allowPlay = false;

	if( !allowPlay )
		MessageBox( NULL, _T("We really hope your students have enjoyed Prime Time, but the time limit for this copy of Prime Time has expired. Please visit www.PrimeTimeMath.com for information on how to buy a full copy. Thanks for playing!"), _T("Prime Time: Math Adventure"), MB_OK | MB_ICONSTOP );

	return allowPlay;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  IsUpdateDownloaded()  Global
///	\return True is returned if there are files in the download folder
///
///	Check if there are update files in the update folder that have been downloaded by the auto-
///	updater.  It also checks if the auto-updater is currently running and, if so, will return
///	failure to avoid file conflicts.
///////////////////////////////////////////////////////////////////////////////////////////////////
//bool IsUpdateDownloaded()
//{
//	// Test if the update directory exists
//	std::string sPath = (TCBase::Narrow( TCBase::GetAppPath().c_str() ) + PTDefines::UPDATE_FOLDER + "\\").c_str();
//	if( _access_s( sPath.c_str(), 0 ) != 0 )
//		return false;
//
//	// Test if the directory is empty
//	WIN32_FIND_DATA findFileData;
//	HANDLE hCurFile = FindFirstFile( sPath.c_str(), &findFileData );
//
//	// If there are no files then bail
//	if( hCurFile == INVALID_HANDLE_VALUE )
//		return false;
//	FindClose( hCurFile );
//	hCurFile = 0;
//
//	// Get a snapshot of the active processes
//	HANDLE hToolsSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
//
//	// Determine if the process can be enumerated, if it fails then assume an update is available
//	// since the update directory is populated
//	PROCESSENTRY32 procEntry;
//	memset( &procEntry, 0, sizeof(procEntry) );
//	procEntry.dwSize = sizeof( procEntry );
//	if( Process32First( hToolsSnapshot, &procEntry ) == FALSE )
//		return true;
//
//	// Go through the running processes and see if the auto-updater is running
//	bool updaterProcRunning = false;
//	do
//	{
//		// Get the file name
//		char szFileName[ MAX_PATH ] = {0};
//		_splitpath_s( procEntry.szExeFile, NULL, 0, NULL, 0, szFileName, MAX_PATH, NULL, 0 );
//
//		// If could be the Prime Time auto-updater then bail
//		if( strcmp( szFileName, PTDefines::UPDATER_EXE_FILE ) == 0 )
//		{
//			updaterProcRunning = true;
//			break;
//		}
//	}
//	while( Process32Next( hToolsSnapshot, &procEntry ) );
//
//	// Close the snapshot handle
//	CloseHandle( hToolsSnapshot );
//
//	// If the process is running then return false since the game shouldn't try to update yet
//	if( updaterProcRunning )
//		return false;
//
//	// At this point there is a download updates folder and it has files in it so there is most
//	// likely an update possible
//	return true;
//}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RunUpdater()  Global
///
///	Run the auto-updater.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void RunUpdater()
{
	// If the auto-updater does not exist then bail
	std::string sModName = TCBase::Narrow( TCBase::GetAppPath().c_str() ) + PTDefines::UPDATER_EXE_FILE;
	if( !TCBase::DoesFileExist( TCBase::Widen( sModName.c_str() ).c_str() ) )
		return;

	// Determine if there is a possible update then attempt to run the auto-updater in update mode
	const uint32 PARAM_BUF_SIZE = 128;
	char szParam[PARAM_BUF_SIZE] = {0};
	
	// Run the auto-updater in silent mode
	strcpy_s( szParam, PARAM_BUF_SIZE, PTDefines::UPDATER_OPTION_SILENT );

	// Start the autp-updater
	STARTUPINFO startupInfo;
	memset( &startupInfo, 0, sizeof(startupInfo) );
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_HIDE;
	
	PROCESS_INFORMATION procInfo;
	memset( &procInfo, 0, sizeof(procInfo) );
	
	CreateProcess( sModName.c_str(), szParam, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &procInfo );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetPrimeTimeDLL()  Global
///
///	Get a handle to the game DLL. This handle must be closed by the caller.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
HMODULE GetPrimeTimeDLL()
{
	// Attempt the load the DLL normally
	HMODULE hGameDLL = LoadLibrary( szDLLName );

	// If the DLL failed to load
	if( !hGameDLL )
	{
		// Try to load the dll from the full path
		std::string sAppPath = TCBase::Narrow( TCBase::GetAppPath().c_str() );
		std::string sBasePath = sAppPath + szDLLName;
		hGameDLL = LoadLibrary( sBasePath.c_str() );

		// If the DLL failed to load the second time then display an error
		if( !hGameDLL )
		{
			DWORD errorCode = GetLastError();

			// Create the error message
			std::string sErrorMsg = _T("Failed to load ");
			sErrorMsg += sBasePath;
			sErrorMsg += _T(" please reinstall Prime Time.");

			sErrorMsg += _T("\nError code: ");
			sErrorMsg += TCBase::Narrow( TCBase::EasyUIToA( errorCode ) ).c_str();
			
			// Add in the system error message
			TCHAR szError[512];
			if( FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, LANG_SYSTEM_DEFAULT, (LPTSTR)szError, 512, NULL ) != 0 )
			{
				sErrorMsg += _T("\nError message: ");
				sErrorMsg += szError;
			}

			// Display the message
			MessageBox( NULL, sErrorMsg.c_str(), _T("Prime Time: Math Adventure"), MB_OK | MB_ICONSTOP );

			return 0;
		}
	}

	return hGameDLL;
}