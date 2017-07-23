//=================================================================================================
/*!
	\file Main.cpp
	Application Project
	Prime Time Entry Point Source
	\author Taylor Clark
	\date December 6, 2005

	This source file contains the entry point for the game.
*/
//=================================================================================================

#ifdef _DEBUG
//#define VLD_AGGREGATE_DUPLICATES
//#include <vld.h>
#endif

#ifdef WIN32
#define WINVER 0x0500
#define _WIN32_WINDOWS 0x500
#define _WIN32_WINNT 0x500
#define _WIN32_IE 0x500
#include <windows.h>
#include <dbghelp.h>
#endif

#ifdef WINDOWS
#include "../ApplicationWin.h"
#elif SFML
#include "../ApplicationSFML.h"
#endif

#include "Base/MsgLogger.h"
#include "Resource/ResourceMgr.h"
#include "Network/NetworkMgr.h"
#include "Network/HttpRequestMgr.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Base/PTDefines.h"
#include "Base/NumFuncs.h"
#include "Base/FileFuncs.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* PARAM_TEST_FIREWALL = L"TestFirewall";
uint32 PT_VERSION = 19;


///////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////////////////////////////////////////////
static void TestFirewall();


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetPTVersion()  Global
///
///	Get the current Prime Time DLL version times ten. For example version 1.3 will return 13. This
/// function is exported from the DLL.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
uint32 GetPTVersion()
{
	return PT_VERSION;
}


#ifdef WIN32
void DisplayFatalErrorMsg( const wchar_t* szMsg )
#else
void DisplayFatalErrorMsg( const wchar_t* )
#endif
{
#ifdef WIN32
	MessageBox(NULL,
				szMsg,
				L"Prime Time: Math Adventure",
				MB_OK | MB_ICONEXCLAMATION);
#endif
}


#ifdef WIN32

//Supress the warning about unreachable code
#pragma warning( push )
#pragma warning( disable : 4702 )

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  UnhandledExcCB()  Global
///
///	The unhandled exception handler callback function.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LONG WINAPI UnhandledExcCB( struct _EXCEPTION_POINTERS* pExceptionInfo )
{
#ifdef _PT_DEMO
	UNREFERENCED_PARAMETER(pExceptionInfo);
#endif

	// Terminate the graphics (Resources must be freed first)
	try
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"*In UnhandledExcCB(), terminating resource and graphics managers then hiding window." );
		ResourceMgr::Get().Term();
		g_pGraphicsMgr->Term();

#ifdef WINDOWS
		ApplicationWin* pAppWin = static_cast<ApplicationWin*>(ApplicationBase::Get());
		ShowWindow( pAppWin->GetHWnd(), SW_HIDE );
#endif
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"*In UnhandledExcCB(), successfully terminated resource and graphics managers." );	
	}
	catch(...){}

	// Close the logger
	try
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"*In UnhandledExcCB(), closing logger then writing minidump." );
		MsgLogger::Get().Term();
	}
	catch(...){}

#ifndef _PT_DEMO
	try
	{
		// Initialize a mini-dump exception information structure
		MINIDUMP_EXCEPTION_INFORMATION aMiniDumpExceptionInfo;
		aMiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
		aMiniDumpExceptionInfo.ExceptionPointers = pExceptionInfo;
		aMiniDumpExceptionInfo.ClientPointers = FALSE;

		// Create the full file path to the output file
		std::wstring sPath = TCBase::GetUserFilesPath();
		sPath += PTDefines::DUMP_PRE_FILE_NAME;
		sPath += L"1.";
		sPath += PTDefines::DUMP_FILE_EXT;

		// Ensure a unique file name
		uint32 fileIndex = 2;
		while( _waccess_s( sPath.c_str(), 0 ) == 0 )
		{
			// Create the new file name
			sPath = TCBase::GetUserFilesPath();
			sPath += PTDefines::DUMP_PRE_FILE_NAME;
			sPath += TCBase::EasyUIToA( ++fileIndex );
			sPath += L".";
			sPath += PTDefines::DUMP_FILE_EXT;
		}

		// Create the output file
		HANDLE hFile = CreateFileW( sPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		
		// If the file was successfully opened
		if( hFile != INVALID_HANDLE_VALUE )
		{
			// Write the dump file
			MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &aMiniDumpExceptionInfo, NULL, NULL );
			CloseHandle( hFile );
		}

		DisplayFatalErrorMsg(L"Prime Time made a mistake and now it can't continue playing. We, the creators of Prime Time, are sorry for the inconvenience. If you haven't updated lately it is recommended that you do so because this bug may already be fixed in a newer version. Try playing again and you shouldn't have any issues.");
	}
	catch(...){}
#endif // _PT_DEMO

	// Time to bail
	exit( 0 );

	return EXCEPTION_EXECUTE_HANDLER;
}
#pragma warning( pop )
#else
typedef int HINSTANCE;
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RunGame()  Global
///
///	Program execution begins in this function.  The window class is registered and the window is
///	created. This function is exported from the DLL.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int RunGame( HINSTANCE hInstance, const TCBase::ParamList& cmdLineParams, uint64 gameKey, const wchar_t* szCustomText, const uint8* pCustomPic, const wchar_t* szCustomPicText )
{
	// Store the unique game key
	ApplicationBase::GameKey = gameKey;

#ifdef WIN32
	// Set the unhandled exception handler
	SetUnhandledExceptionFilter( UnhandledExcCB );
#endif

#ifndef WINDOWS
	// Reference the parameter to avoid compiler warnings
	UNREFERENCED_PARAMETER(hInstance);
#endif

#ifdef _DEBUG
	//MsgLogger::Get().AddListener( TraceAssist::CreateOutputWindow(hInstance) );
#endif

#ifndef _PT_DEMO
	if( !cmdLineParams.HasOption( L"runoncd" ) )
		MsgLogger::Get().AddListener( (TCBase::GetUserFilesPath() + PTDefines::LOG_PRE_FILE_NAME + L"1." + PTDefines::LOG_FILE_EXT).c_str() );
#endif

	// If this is being run to test the firewall
	if( cmdLineParams.HasOption( PARAM_TEST_FIREWALL ) )
	{
		// Test the firewall and bail
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Testing firewall" );
		TestFirewall();
		return 0;
	}

#ifdef WINDOWS
	if( !ApplicationWin::InitApp( hInstance, cmdLineParams, szCustomText, pCustomPic, szCustomPicText ) )
		return 0;

#elif SFML
	if( !ApplicationSFML::InitApp( cmdLineParams, szCustomText, pCustomPic, szCustomPicText ) )
		return 0;
#endif

	// Send the "application start" message
	ApplicationBase::SendAppConnectMessage(L"as");

	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Entering game..." );

#ifdef WINDOWS
	(static_cast<ApplicationWin*>(ApplicationBase::Get()))->Run();
#else
	ApplicationBase::Get()->Run();
#endif
	
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Terminating game..." );
	
	ApplicationBase::TermApp();
	
	// Exit the program and return no problem
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	TestFirewall  Global
///
///	Test the firewall by sending and attempting to receive data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static void TestFirewall()
{
	// Initialize the network manager
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Initializizing the network manager" );
	try
	{
		// Test making an HTTP request
		std::wstring sInstallUri = L"http://www.PrimeTimeMath.com/AppConnect.aspx?m=tf";
		TCBase::AppendQueryStringParameter( sInstallUri, L"v", TCBase::EasyIToA(PT_VERSION) );
		TCBase::AppendQueryStringParameter( sInstallUri, L"k", TCBase::EasyUI64ToA(ApplicationBase::GameKey) );

		HttpRequestMgr::Init();
		HttpRequestInfo requestInfo = HttpRequestMgr::Get( sInstallUri.c_str() );
		requestInfo.ReceivedData.Free();
		HttpRequestMgr::Term();

		NetworkMgr::Get().Init();
	
		// Test the firewall
		NetworkMgr::Get().TestFirewall();

		// Close the network manager
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Terminating the network manager" );
		NetworkMgr::Get().Term();
	}
	catch( ... )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"An exception occurred while testing the firewall" );
	}

	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Finished testing firewall" );
}