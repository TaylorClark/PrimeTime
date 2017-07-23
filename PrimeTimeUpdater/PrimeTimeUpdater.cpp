// PrimeTimeUpdater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PrimeTimeUpdater.h"
#include "Base/FileFuncs.h"
#include "FileDownloader.h"
#include "FileProgressDlg.h"
#include <direct.h>
#include "Base/MsgLogger.h"
#include "Base/StringFuncs.h"
#include "Base/NumFuncs.h"
#include "MD5.h"
#include <set>
#include "SysTrayIconDlg.h"
#include "ResultsDlg.h"
#include "Base/PTDefines.h"
#include <Tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The command line options
const wchar_t* CMDLINE_PASSIVE = L"silent";
const wchar_t* CMDLINE_RUNGAME = L"rungame";
const wchar_t* CMDLINE_TESTFIREWALL = L"testFirewall";

// The dialog to display progress when not is passive mode
static CFileProgressDlg sg_ProgressDlg;

// If the application is being run in passive mode
bool g_IsInPassiveMode = false;

// The message to display in the results dialog at the end
std::wstring g_sResultsDlgMsg;

extern GameUpdateInfo DownloadNeededFiles();
extern void UploadLogFiles();
extern void ApplyUpdate( GameUpdateInfo updateInfo );
extern void PumpMessages();

int IsEXERunning( const wchar_t* szEXEPath );


// CPrimeTimeUpdaterApp
BEGIN_MESSAGE_MAP(CPrimeTimeUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPrimeTimeUpdaterApp construction

CPrimeTimeUpdaterApp::CPrimeTimeUpdaterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPrimeTimeUpdaterApp object
CPrimeTimeUpdaterApp theApp;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  UpdatePrimaryProgress  Global
///
///	\param szMsg The message to display
///	\param normPct The normalized percentage
///
///	Update the primary progress of the application.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void UpdatePrimaryProgress( const wchar_t* szMsg, float32 normPct )
{
	MSG_LOGGER_OUT( MsgLogger::MI_Note, szMsg );
	if( !g_IsInPassiveMode )
	{
		sg_ProgressDlg.SetPrimaryText( szMsg );
		sg_ProgressDlg.SetPrimaryProgress( normPct );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  UpdateSecondaryProgress  Global
///
///	\param szMsg The message to display
///	\param normPct The normalized percentage
///
///	Update the secondary progress of the application.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void UpdateSecondaryProgress( const wchar_t* szMsg, float32 normPct )
{
	if( !g_IsInPassiveMode )
	{
		sg_ProgressDlg.SetSecondaryText( szMsg );
		sg_ProgressDlg.SetSecondaryProgress( normPct );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  UnhandledExcCB()  Global
///
///	The unhandled exception handler callback function.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LONG WINAPI UnhandledExcCB( struct _EXCEPTION_POINTERS* pExceptionInfo )
{
	// Close the logger
	try
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"In UnhandledExcCB(), closing logger and bailing out" );
		MsgLogger::Get().Term();
	}
	catch(...){}

	// Time to bail
	exit( 0 );

	return EXCEPTION_EXECUTE_HANDLER;
}


extern bool UploadFile( const wchar_t* szFile, const wchar_t* szFtpFile  );


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestFirewall()  Global
///
///	Test the firewall by downloading and uploading a file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static void TestFirewall()
{
	// Download a file
	const wchar_t* DOWNLOAD_FILE = L"http://www.primetimemath.net/test_dl.txt";
	FileDownloader file;
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Testing download" );
	file.DownloadToBuffer( DOWNLOAD_FILE, NULL );
		
	// Create a test text file
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Creating temporary text file" );
	const wchar_t* TEST_FILE = L"test_upload.txt";
	std::wofstream testFile( TEST_FILE );
	const wchar_t* TEST_TEXT = L"This file was uploaded.";
	testFile.write( TEST_TEXT, (std::streamsize)wcslen(TEST_TEXT) );
	testFile.close();

	// Upload the test file then delete it
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Testing upload" );
	UploadFile( TEST_FILE, TEST_FILE );
	_wremove( TEST_FILE );

	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Finished firewall test" );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CPrimeTimeUpdaterApp::InitInstance  Public
///
///	\returns A context sensitive return value
///
///	Called when the application is initialized.  This function contains the basic functionality
///	for the application.  When the application is run without command line parameters then the
///	normal operation is as follows:
///
///	1. Download the update XML file
///	2. Determine if there are any downloaded files
///	 |-	If there are downloaded files then determine the version. If it is unknown or old then
///		then the files are removed
///	3. Download the files needed to match up to the latest revision into a download folder
///	4. The MD5 of the downloaed files are confirmed against the update XML
///	5. The older versions of the downloaded files are renamed then the files in download folder
///		are copied to the parent folder
///	6. The old files and download folder are deleted
///
///	When the /silentdownload option is used the process does not display any windows and the
///	process is stopped after files are downloaded.  No update is made as this option is generally
///	used by the game to run the update in the background.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPrimeTimeUpdaterApp::InitInstance()
{
	// If the updater is already running then bail
	if( IsEXERunning( PTDefines::WUPDATER_EXE_FILE ) > 1 )
		return FALSE;

	// Initialize the message logger
	MsgLogger::Get().AddListener( (TCBase::GetAppPath() + PTDefines::LOG_PRE_FILE_NAME + L"Updater." + PTDefines::LOG_FILE_EXT).c_str(), true );
	
	// Set the unhandled exception handler so that the user never sees this tool crash
	SetUnhandledExceptionFilter( UnhandledExcCB );

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

	// Parse the command line
	const char* szCmdLine = GetCommandLineA();
	TCBase::ParamList paramList;
	paramList.ParseCmdLineOptions( szCmdLine );

	// If this is simply a test of the internet
	if( paramList.HasOption( CMDLINE_TESTFIREWALL ) )
	{
		// Test the firewall and bail
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Testing firewall" );
		try
		{
			TestFirewall();
		}
		catch( ... )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Testing firewall threw exception" );
		}

		return FALSE;
	}

	// Go through the command line parameters
	bool runGame = false;
	if( paramList.HasOption(CMDLINE_PASSIVE) )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Passive mode specified" );
		g_IsInPassiveMode = true;
	}
	if( paramList.HasOption(CMDLINE_RUNGAME) )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Game run option specified" );
		runGame = true;
	}

	// If the updater is in passive mode then create the system tray icon
	CSysTrayIconDlg trayIcon;
	if( g_IsInPassiveMode )
	{
		trayIcon.Create( MAKEINTRESOURCE(IDD_SYSTRAYICON_DIALOG) );
		trayIcon.ShowWindow( SW_HIDE );
		
	}
	// Else create the progress window
	else
	{
		sg_ProgressDlg.Create( CFileProgressDlg::IDD );
		sg_ProgressDlg.ShowWindow( SW_SHOW );
	}

	// Download the needed files
	GameUpdateInfo updateInfo = DownloadNeededFiles();

	// Upload crash reports and message logs
	UpdatePrimaryProgress( L"Uploading error logs...", 0.8f );
	UpdateSecondaryProgress( L"", 0.0f );
	UploadLogFiles();

	// Update the game files if the update is ready and not in passive mode
	if( updateInfo.isUpdateReady && !g_IsInPassiveMode )
	{
		UpdatePrimaryProgress( L"Updating game...", 0.9f );
		UpdateSecondaryProgress( L"Waiting for game to finish", 0.33f );

		// Wait until the game is finished
		while( IsEXERunning( PTDefines::WGAME_EXE_FILE ) > 0 )
		{
			PumpMessages();
			Sleep( 3000 );
		}

		// Modifiy the files
		UpdateSecondaryProgress( L"Applying update", 0.75f );
		ApplyUpdate( updateInfo );
	}
	else if( !updateInfo.isUpdateReady )
	{
		if( updateInfo.filesToCopyOver.size() > 0 )
			MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Update is not ready" );
	}

	// Display the results
	if( !g_IsInPassiveMode )
	{
		CResultsDlg dlg;
		dlg.SetMultilineResultText( g_sResultsDlgMsg.c_str() );
		dlg.DoModal();
	}

	// Close the message logger
	MsgLogger::Get().Term();

	// If the game is to be run
	if( runGame )
	{
		// Create an empty startup info object since none of its values our relevant in this case
		STARTUPINFO info;
		memset( &info, 0, sizeof(info) );
		
		// Run Prime Time in another process
		PROCESS_INFORMATION procInfo;
		if( CreateProcess( PTDefines::WGAME_EXE_FILE, L"/noupdate", NULL, NULL, FALSE,
			0, NULL, NULL, &info, &procInfo ) == 0 )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Updater failed to start game." );
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the application, rather
	// than start the application's message pump.
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IsEXERunning  Global
///
///	\param szEXEPath The file name, with extension, of the process to find
///	\returns The number of processes running with the matching executable name
///
///	Check if a specific process is active.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int IsEXERunning( const wchar_t* szEXEPath )
{
	// Validate the input
	if( !szEXEPath || wcslen(szEXEPath) == 0 )
		return 0;

	// Get a snapshot of the active processes
	HANDLE hToolsSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	// Determine if the process can be enumerated, if it fails then assume an update is available
	// since the update directory is populated
	PROCESSENTRY32 procEntry;
	memset( &procEntry, 0, sizeof(procEntry) );
	procEntry.dwSize = sizeof( procEntry );
	if( Process32First( hToolsSnapshot, &procEntry ) == FALSE )
		return 0;

	// Go through the running processes and see if the auto-updater is running
	bool isProcRunning = false;
	int numProcCount = 0;
	do
	{
		// Get the file name
		//wchar_t szFileName[ MAX_PATH ] = {0};
		//_wsplitpath_s( procEntry.szExeFile, NULL, 0, NULL, 0, szFileName, MAX_PATH, NULL, 0 );

		// If could be the Prime Time auto-updater then bail
		if( wcscmp( procEntry.szExeFile, szEXEPath ) == 0 )
			++numProcCount;
	}
	while( Process32Next( hToolsSnapshot, &procEntry ) );

	// Close the snapshot handle
	CloseHandle( hToolsSnapshot );

	// Return if the process is running
	return numProcCount;
}