//=================================================================================================
/*!
	\file Main.cpp
	Prime Time Entry Point Source
	\author Taylor Clark
	\date December 6, 2005

	This source file contains the entry point for the application
*/
//=================================================================================================

#ifdef _DEBUG
#define VLD_AGGREGATE_DUPLICATES
#include <vld.h>
#endif

#include "Network/NetConnLAN.h"
#include <windows.h>
#include <stdio.h>
#include "Graphics2D/Graphics2D.h"
#include "Base/TCAssert.h"
#include "Base/MsgLogger.h"
#include <time.h>
#include "Base/FileFuncs.h"
#include "../GameMgr.h"
#include "Base/TraceAssist.h"
#include "Network/NetworkMgr.h"
#include "Network/NetMsg.h"



///////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////
static const wchar_t* CLASS_NAME = L"NetworkTest";
static const wchar_t* PROG_NAME = L"Prime Time Network Test";
#define ERRORMSG(str) MessageBox( NULL, str, L"Prime Time", MB_OK | MB_ICONERROR )


///////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void MainLoop();


///////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////////////////////////
bool g_bProgDone = false;
GraphicsMgr& g_GraphicsMgr = GraphicsMgr::Get();
HWND g_hWnd = 0;
Point2i g_MousePos;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  WinMain()  Global
///
///	Program execution begins in this function.  The window class is registered and the window is
///	created.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int )
{	
	MsgLogger::Get().AddListener( (TCBase::GetAppPath() + L"Output.txt").c_str() );
	MsgLogger::Get().AddListener( TraceAssist::CreateOutputWindow(hInstance) );

	MsgLogger::Get().Output( L"Preparing graphics" );
	
	// Test graphics for use
	if( !g_GraphicsMgr.PrepareGraphics() )
	{
		ERRORMSG( L"Failed to intialize graphics manager internal data." );
		return 0;
	}

	// Initialize the members of the window class
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW + 1;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.lpszMenuName = 0;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	
	// Register the window class
	if( RegisterClass(&wndClass) == NULL )
	{
		// We could not register the window class
		MessageBox(NULL, L"Could not register window class.", PROG_NAME, MB_OK);

		// Exit the program
		return 0;
	}

	// Create the window
	g_hWnd = CreateWindow( CLASS_NAME, PROG_NAME,
						WS_OVERLAPPEDWINDOW | WS_EX_TOPMOST,
						CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
						0, 0, hInstance, NULL);

	// Ensure the window was created
	if(g_hWnd == NULL)
	{
		// We could not create the window so display error message
		MessageBox(NULL, L"Could not create window.", PROG_NAME, MB_OK);

		// Exit the program
		return 0;
	}

	// Initialize the random number generator
	srand( (unsigned int)time( NULL ) );

	// Initialize the graphics manager
	if( !g_GraphicsMgr.Init( g_hWnd ) )
	{
		ERRORMSG( L"Failed to intialize graphics manager." );
		g_GraphicsMgr.Term();
		return 0;
	}

	if( !NetworkMgr::Get().Init() )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to initialize the network manager." );
		g_GraphicsMgr.Term();
		return false;
	}

	// Display the window
	ShowWindow( g_hWnd, SW_SHOW );

	// Initialize the boolean done value before entering the main message loop
	g_bProgDone = false;

	MainLoop();
	
	NetworkMgr::Get().Term();
	
	g_GraphicsMgr.Term();

	MsgLogger::Get().Term();

	TraceAssist::FreeOutputWindow();

	// Free the windows class
	UnregisterClass( CLASS_NAME, hInstance );
	
	// Exit the program and return no problem
	return 0;
}


class TestNetMsg : public NetMsg
{
public:
	int val;

	TestNetMsg( int x = 0 ) : val(x)
	{}

	/// Get the size of the message data
	virtual uint32 GetMsgDataSize() const { return sizeof(val); }

	static NetMsg* CreateMsg( DataBlock dataBlock )
	{
		TestNetMsg* pRetMsg = new TestNetMsg();
		pRetMsg->val = dataBlock.ReadInt32();
		return pRetMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( DataBlockOut& dataOut ) const { dataOut.WriteVal( val ); return sizeof(val); }

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return 10; }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MainLoop()  Global
///
///	The function containing the main game loop.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MainLoop()
{
	NetworkMgr::Get().RegisterNetMsg( 10, TestNetMsg::CreateMsg );

	GameMgr gameMgr;
	if( !gameMgr.Init() )
		return;

	// Initialize the performance counter
	LARGE_INTEGER timerFreq, curFrameTime, newFrameTime;
	QueryPerformanceFrequency( &timerFreq );	
	float timerFreqScalar = 1.0f / (float)timerFreq.QuadPart;

	// The main message loop
	MSG msg;
	float lastSendTimer = 0.0f;
	QueryPerformanceCounter( &curFrameTime );
	while( g_bProgDone == false ) 
	{
		// Get a windows message when available
		if( PeekMessage( &msg, g_hWnd, 0, 0, PM_NOREMOVE ) )
		{
			// Get the message
			GetMessage(&msg, g_hWnd, 0, 0);

			// Get WM_CHAR messages				
			TranslateMessage( &msg );

			// Dispatch the message
			DispatchMessage(&msg);
		}

		// Get the elapsed time
		QueryPerformanceCounter( &newFrameTime );
		float frameTime = (float)(newFrameTime.QuadPart - curFrameTime.QuadPart) * timerFreqScalar;
		if( frameTime > 1.0f )
			frameTime = 1.0f;
		curFrameTime = newFrameTime;

		lastSendTimer += frameTime;

		gameMgr.Update( frameTime, g_MousePos );

		gameMgr.Draw();

		// Display the scene
		g_GraphicsMgr.DisplayScene( true );

		// Don't hog the cpu
		Sleep( 20 );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	WindowProc()  Global
///
///	This function handles all windows messages.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	// If the window is being closed
	case WM_CLOSE:
		g_bProgDone = true;
		return 0;
	
	case WM_KEYDOWN:
		{
			if( wParam == VK_ADD  || wParam == VK_UP || wParam == VK_RIGHT )
			{
			}
		}
		return 0;

	case WM_CHAR:
		{
			if( (char)wParam == 'c' )
			{
				NetworkMgr::Get().ConnectToTCPConn( IPAddress("192.168.0.119"),12344);
			}
			else if( (char)wParam == 'l' )
			{
				NetworkMgr::Get().WaitForTCPConn( 12344 );
			}
		}
		return 0;

	case WM_MOUSEMOVE:
		{
			g_MousePos.x = LOWORD( lParam );
			g_MousePos.y = HIWORD( lParam );
		}
		return 0;
	default:
		break;
	}

	// Defaultly handle the messages
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}