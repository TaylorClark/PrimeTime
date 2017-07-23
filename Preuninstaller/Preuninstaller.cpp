// Preuninstaller.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Preuninstaller.h"
#include "ProgressDlg.h"
#include "Base/FileFuncs.h"
#include "Base/StringFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The name of the uninstaller executable
static const wchar_t* UNINSTALLER_FILE = L"post_uninstall.dll";
static const wchar_t* ORIGINAL_FILE = L"Uninstall.exe";

// The files required for a directory to be considered the game directory
static const wchar_t* REQ_FILES[] = { L"PrimeTime.exe", L"numbers.rdb", L"fmod.dll", L"PrimeTime.dll" };
static const uint32 NUM_REQ_FILES = sizeof(REQ_FILES) / sizeof(REQ_FILES[0]);

// The name of the player profiles directory
static const wchar_t* PROFILE_DIR = L"Profiles";


// CPreuninstallerApp

BEGIN_MESSAGE_MAP(CPreuninstallerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPreuninstallerApp construction

CPreuninstallerApp::CPreuninstallerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPreuninstallerApp object

CPreuninstallerApp theApp;

void RunUninstaller();
bool IsInPrimeTimeDirectory();
void RemoveProfiles();
void RemoveLogsAndDumps();
void RunPostUninstaller();
bool RunExecutable( const wchar_t* szFullEXEPath );

// CPreuninstallerApp initialization

BOOL CPreuninstallerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	std::wstring sFile = TCBase::GetAppFileName();
	std::wstring sFilePath = TCBase::GetAppPath();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	

#ifndef _DEBUG
	// Ensure this is the Prime Time directory
	if( !IsInPrimeTimeDirectory() )
	{
		MessageBox( NULL, L"The directory from which this uninstaller is being run is not the Prime Time game directory. If you think this message is wrong then you may need to reinstall Prime Time for the uninstaller to work (Weird, huh? This is a precautionary measure to ensure that no files become corrupt). We apologize for the inconvenience and feel free to visit the website to contact support if needed.", L"Prime Time: Math Adventure", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
#endif // _DEBUG

	// If this file is the original file name
	if( TCBase::GetAppFileName() == ORIGINAL_FILE )
	{
		// Confirm the user wants to uninstall Prime Time
		int retVal = MessageBox( NULL, L"Are you sure you want to uninstall Prime Time?", L"Uninstall Prime Time: Math Adventure?", MB_YESNO | MB_ICONQUESTION );
		if( retVal == IDNO )
			return FALSE;

		// Get a temporary file name and if the function fails use a default temporary name
		const uint32 TEMP_FILE_NAME_LEN = 512;
		wchar_t szTempFileName[TEMP_FILE_NAME_LEN] = {0};
		if( _wtmpnam_s( szTempFileName, TEMP_FILE_NAME_LEN ) != 0 )
			swprintf_s( szTempFileName, TEMP_FILE_NAME_LEN, L"temp51xuninstall.exe" );

		// Store the full path of the temporary file
		std::wstring sFullTempPath = TCBase::CombinePaths( TCBase::GetAppPath(), std::wstring(szTempFileName) );

		// Copy the file
		CopyFile( TCBase::GetAppFullPath().c_str(), sFullTempPath.c_str(), FALSE );

		// Run the copy
		RunExecutable( sFullTempPath.c_str() );
	}
	// Else this file is running as a copy
	else
	{
		// Execute the uninstaller
		RunUninstaller();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void RunUninstaller()
{
	CProgressDlg dlg;
	dlg.Create( CProgressDlg::IDD, NULL );
	dlg.SetProgress( 0.0f );
	dlg.ShowWindow( SW_SHOW );

	// Delete the profile directory
	dlg.SetProgress( 0.25f );
	dlg.SetLabelText( L"Removing player profiles..." );
	dlg.ShowWindow( SW_HIDE );
	int retVal = MessageBox( NULL, L"Are you sure you want to remove your player profiles? If you ever decide to install Prime Time again then you can pick up where you left off if you select 'No'. If you press 'Yes' your player profiles will be deleted.", L"Remove Player Profiles?", MB_YESNO | MB_ICONQUESTION );
	dlg.ShowWindow( SW_SHOW );
	if( retVal == IDYES )
	{
	}

	// Delete logs and dump files
	dlg.SetProgress( 0.5f );
	dlg.SetLabelText( L"Removing log files..." );
	RemoveLogsAndDumps();

	// Run the actual uninstaller
	dlg.SetProgress( 0.75f );
	dlg.SetLabelText( L"Removing game data files..." );
	RunPostUninstaller();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// RemoveProfiles  Global
///
///	Remove the player profile files. If the directory is empty once the profiles are deleted then
///	the directory will be deleted as well.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void RemoveProfiles()
{
	// Get the profile directory path with profile file wildcard
	std::wstring sProfileDir = TCBase::GetAppPath() + PROFILE_DIR + L"\\*.ptp";

	// Go through the files in the directory
	CFileFind finder;
	BOOL isFindingFiles = finder.FindFile(sProfileDir.c_str());
	while( isFindingFiles )
	{
		// Delete this file
		_wremove( (const wchar_t*)finder.GetFilePath() );

		// Find the next file
		isFindingFiles = finder.FindNextFile();
	}

	// Try to remove the directory, if there are still files within then this function will fail
	sProfileDir = TCBase::GetAppPath() + PROFILE_DIR;
	RemoveDirectory( sProfileDir.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// RemoveLogsAndDumps  Global
///
///	Remove the Prime Time logs and any crash dumps that are still in the game directory.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void RemoveLogsAndDumps()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// RunPostUninstaller  Global
///
///	Run the uninstaller that removes the rest of the game data. This function starts the
///	uninstaller, waits for it to prompt the user to confirm uninstallation, then simulates a key
///	press to auto-press the 'OK' button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void RunPostUninstaller()
{
	// Get the full path to the uninstaller
	std::wstring sFullPath = TCBase::GetAppPath();
	sFullPath += UNINSTALLER_FILE;

    // Run the uninstaller
	RunExecutable( sFullPath.c_str() );
}

bool IsInPrimeTimeDirectory()
{
	// Get the current path
	std::wstring sAppPath = TCBase::GetAppPath();

	// Ensure the basic files are in this directory
	for( uint32 curFileIndex = 0; curFileIndex < NUM_REQ_FILES; ++curFileIndex )
	{
		std::wstring sCurFile = sAppPath + REQ_FILES[curFileIndex];

		if( !TCBase::DoesFileExist( sCurFile.c_str() ) )
			return false;
	}

	// The files exist so it is valid
	return true;
}

bool RunExecutable( const wchar_t* szFullEXEPath )
{
	// Initialize the data need for CreateProcess()
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	// Start the child process
    if( !CreateProcess( szFullEXEPath,   // The module name
        NULL, // Command line. 
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &pi )             // Pointer to PROCESS_INFORMATION structure.
    ) 
    {
        return false;
    }

	return true;
}