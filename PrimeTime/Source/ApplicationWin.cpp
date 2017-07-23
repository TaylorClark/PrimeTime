/*=================================================================================================

	\file Application.cpp
	Application Project
	Application Class Source
	\author Taylor Clark
	\Date August 19, 2007

	This source file contains the implementation of the application class.

=================================================================================================*/

#ifdef _DEBUG
#define SHOW_FPS
#endif

#include "../ApplicationWin.h"
#include <stdio.h>
#include "Graphics2D/GraphicsMgr.h"
#include "Graphics2D/TCFont.h"
#include "Base/TCAssert.h"
#include "GUI/GUIMgr.h"
#include "Resource/ResourceMgr.h"
#include "Base/MsgLogger.h"
#include "Base/TraceAssist.h"
#include "GamePlay/GameMgr.h"
#include "Audio/AudioMgr.h"
//#include "Network/NetworkMgr.h"
#include "Base/FourCC.h"
#include "Base/NumFuncs.h"
#include "Base/DataBlock.h"
#include "GamePlay/GUILayout_Game.h"
#include <dbghelp.h>
#include "Base/PTDefines.h"
#include "../Resource.h"


#ifdef _DEBUG
#include "../GUIEditDlg.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////
static LPTSTR CLASS_NAME = L"PrimeTime";
static LPTSTR PROG_NAME = L"Prime Time";
static LPTSTR MSGBOX_CAPTION = L"Prime Time: Math Adventure";
const DWORD SLEEP_TIME = 10;
const DWORD WINSTYLE_FULLSCREEN = WS_MAXIMIZE | WS_POPUP | WS_VISIBLE;
const DWORD WINSTYLE_WINDOWED = WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_BORDER;


///////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsMgrBase* g_pGraphicsMgr = 0;
static GUIMgr& g_GUIMgr = GUIMgr::Get();


extern void DisplayFatalErrorMsg( const wchar_t* szMsg );


///////////////////////////////////////////////////////////////////////////////////////////////////
//  ApplicationWin::InitApp  Static Public
///
///	Initialize the application.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicationWin::InitApp( HINSTANCE hInstance, const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg )
{
	ApplicationWin* pAppWin = new ApplicationWin();
	_pAppInstance = pAppWin;

	return pAppWin->Init( hInstance, cmdLineParams, szCustomText, pCustomPicData, szCustomPicMsg );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  ApplicationWin::Init  Public
///
///	Initialize the application.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicationWin::Init( HINSTANCE hInstance, const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg )
{
	// Register the window class
	if( !RegisterWindowClass() )
	{
		// The window class could not be registered
		MsgLogger::Get().Output( L"Failed to register window class." );
		DisplayFatalErrorMsg( L"Could not register window class." );

		// Exit the program
		return false;
	}

	m_hInstance = hInstance;

	// Get the instance of the graphics manager
	g_pGraphicsMgr = &GraphicsMgrBase::Get();


#ifdef _DEBUG
	bool startFullScreen = false;	
#else
	bool startFullScreen = true;
#endif

	// If the player specified the windowed command line parameter then start windowed
	if( cmdLineParams.HasOption( L"windowed" ) )
		startFullScreen = false;

	// If the game is starting in full screen mode
	if( startFullScreen )
	{
		// Create a top most full screen window
		m_hWnd = CreateWindowEx( WS_EX_TOPMOST,
						CLASS_NAME, PROG_NAME,
						WINSTYLE_FULLSCREEN,
						CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
						0, 0, m_hInstance, NULL);
	}
	// Else the game is starting windowed
	else
	{
		// Get the desired size of the window
		RECT clientRect = {0,0,1024,768};
		AdjustWindowRect( &clientRect, WINSTYLE_WINDOWED, FALSE );

		// Create a normal window
		m_hWnd = CreateWindow( CLASS_NAME, PROG_NAME,
						WINSTYLE_WINDOWED,
						CW_USEDEFAULT, CW_USEDEFAULT, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
						0, 0, m_hInstance, NULL);
	}

	// Ensure the window was created
	if(m_hWnd == NULL)
	{
		// We could not create the window
		// Display error message
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to create window" );
		DisplayFatalErrorMsg( L"Prime Time could not create a window for the game. Try restarting your computer and playing again." );

		// Exit the program
		return false;
	}

	// Display the window
	ShowWindow( m_hWnd, SW_SHOW );

	// If this is debug mode then show the GUI edit window
#ifdef ENABLE_GUI_EDITOR
	GUIEditDlg::Get().Init( m_hWnd );
	GUIEditDlg::Get().Show();
#endif

	// Initialize the graphics manager
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Window class registered and window created, now initializing graphics..." );
	if( !g_pGraphicsMgr->Init( m_hWnd, startFullScreen ) )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to intialize graphics manager." );
		DisplayFatalErrorMsg( L"The graphics could not be initialized properly. Prime Time requires at least DirectX 5 and a monitor that can display at least 1024x768 resolution. If you meet these requirements then make sure no other games are running and/or restart your computer and then try running Prime Time again." );

		g_pGraphicsMgr->Term();
		return false;
	}
	
	return ApplicationBase::InitGameMgrs( cmdLineParams, szCustomText, pCustomPicData, szCustomPicMsg );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ApplicationWin::Draw  Private
///
///	Draw the scene.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationWin::Draw()
{
	// If the window is not visible then don't draw
	if( m_IsWinMinimized )
		return;

	// Begin a new scene
	g_pGraphicsMgr->BeginScene( false );

	// Since all visible graphics are represented by UI elements then drawing the UI will render
	// the entire game scene
	g_GUIMgr.Draw();

	// Display the scene
	if( !g_pGraphicsMgr->DisplayScene() )
	{
		// If the image resources need reloading due to surface loss
		if( g_pGraphicsMgr->ImagesNeedReload() )
		{
			ResourceMgr::Get().ReloadImageResourceData();
			g_pGraphicsMgr->ClearImageReloadFlag();
			
			// Allow the GUI to handle the refresh
			if( GUIMgr::Get().GetCurrentLayout() )
			{
				GameGUILayout* pCurLayout = (GameGUILayout*)GUIMgr::Get().GetCurrentLayout();
				pCurLayout->OnGraphicsRefresh();
			}

			MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Image data reloaded." );
		}

		// Try to display again
		if( !g_pGraphicsMgr->DisplayScene() )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Failed to redisplay, restarting graphics." );

			bool isFullScreen = g_pGraphicsMgr->IsFullScreen();
			g_pGraphicsMgr->Term();
			g_pGraphicsMgr->Init( m_hWnd, isFullScreen );
			ResourceMgr::Get().RecreateImageResources();
			
			// Allow the GUI to handle the refresh
			if( GUIMgr::Get().GetCurrentLayout() )
			{
				GameGUILayout* pCurLayout = (GameGUILayout*)GUIMgr::Get().GetCurrentLayout();
				pCurLayout->OnGraphicsRefresh();
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ApplicationWin::Run  Public
///
///	The function containing the main game loop.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationWin::Run()
{
	// Initialize the performance counter
	LARGE_INTEGER timerFreq, curFrameTime, newFrameTime;
	QueryPerformanceFrequency( &timerFreq );	
	float timerFreqScalar = 1.0f / (float)timerFreq.QuadPart;

	// The main message loop
	MSG msg;
	QueryPerformanceCounter( &curFrameTime );
#ifdef SHOW_FPS
	const int32 FPS_BUF_SIZE = 128;
	wchar_t sFPS[FPS_BUF_SIZE] = {0};
#endif

	while( !m_IsAppDone )
	{
		// Get a windows message when available
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			// Get the message
			GetMessage(&msg, NULL, 0, 0);

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

		// If the user wants to toggle full-screen
		if( m_ToggleFullscreen )
		{
			ToggleFullScreen();
			
			// Allow the GUI to handle the refresh
			if( GUIMgr::Get().GetCurrentLayout() )
			{
				GameGUILayout* pCurLayout = (GameGUILayout*)GUIMgr::Get().GetCurrentLayout();
				pCurLayout->OnGraphicsRefresh();
			}

			// Clear the toggle flag
			m_ToggleFullscreen = false;
		}

		// Update the user interface
		g_GUIMgr.Update( frameTime );
		
		// Update the audio manager
		AudioMgr::Get().Update( frameTime );

		// If the game is running then update it then draw it
		if( GameMgr::Get().IsPlaying() )
			GameMgr::Get().Update( frameTime );

		Draw();

#ifdef SHOW_FPS
		// Display the frames per second
		int32 numChars = swprintf_s( sFPS, FPS_BUF_SIZE, L"FPS: %f", (1.0f / frameTime) );
		HDC hDC = GetDC( m_hWnd );
		TextOut( hDC, 0,0, sFPS, numChars );
		ReleaseDC( m_hWnd, hDC );
#endif

		// Don't hog the cpu
		Sleep( SLEEP_TIME );
	}
}


#ifdef _DEBUG
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ApplicationWin::Run_Dbg  Public
///
///	The debug version of the function containing the main game loop.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationWin::Run_Dbg()
{
	// Initialize the performance counter
	LARGE_INTEGER timerFreq;
	LARGE_INTEGER curTime, newTime;
	LARGE_INTEGER startMetricTime, endMetricTime;
	LARGE_INTEGER startLoopTime, endLoopTime;
	QueryPerformanceFrequency( &timerFreq );
	float32 timerFreqScalar = 1.0f / (float32)timerFreq.QuadPart;
	float64 timerFreqScalarDbl = 1.0 / (float64)timerFreq.QuadPart;
	uint32 totalFrameCount = 0;

	
#ifdef SHOW_FPS
	float32 secTimer = 0.0f;
	int32 frameCount = 0;
	int32 fpsTextLen = 0;
	const int32 FPS_BUF_SIZE = 128;
	wchar_t sFPS[FPS_BUF_SIZE] = {0};
#endif

	float64 updateTime = 0.0;
	float64 drawTime = 0.0;

	// The main message loop
	MSG msg;
	QueryPerformanceCounter( &curTime );
	QueryPerformanceCounter( &startLoopTime );
	while( m_IsAppDone == false )
	{
		// Get a windows message when available
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			// Get the message
			GetMessage(&msg, NULL, 0, 0);

#ifdef ENABLE_GUI_EDITOR
			// If the message is not a dialog message then handle it normal, otherwise
			// IsDialogMessage() handles it
			bool isDlgMsg = IsDialogMessage( GUIEditDlg::Get().m_HWnd, &msg ) != 0;
			if( !isDlgMsg )
			{
                // Get WM_CHAR messages
				TranslateMessage( &msg );

				// Dispatch the message
				DispatchMessage(&msg);
			}
#else
			// Get WM_CHAR messages				
			TranslateMessage( &msg );

			// Dispatch the message
			DispatchMessage(&msg);
#endif
		}

		// Get the elapsed time
		QueryPerformanceCounter( &newTime );
		float frameTime = (float)(newTime.QuadPart - curTime.QuadPart) * timerFreqScalar;
		if( frameTime > 0.75f )
			frameTime = 0.75f;
		curTime = newTime;

		// If the user wants to toggle full-screen
		if( m_ToggleFullscreen )
		{
			ToggleFullScreen();

			// Allow the GUI to handle the refresh
			if( GUIMgr::Get().GetCurrentLayout() )
			{
				GameGUILayout* pCurLayout = (GameGUILayout*)GUIMgr::Get().GetCurrentLayout();
				pCurLayout->OnGraphicsRefresh();
			}

			// Clear the toggle flag
			m_ToggleFullscreen = false;
		}
		QueryPerformanceCounter( &startMetricTime );

		// Update the user interface
		g_GUIMgr.Update( frameTime );

		if( GameMgr::Get().IsPlaying() )
			GameMgr::Get().Update( frameTime );

		AudioMgr::Get().Update( frameTime );

		QueryPerformanceCounter( &endMetricTime );
		updateTime += (double)(endMetricTime.QuadPart - startMetricTime.QuadPart) * timerFreqScalarDbl;


		// If the window is visible
		if( !m_IsWinMinimized )
		{
			QueryPerformanceCounter( &startMetricTime );

			// Begin a new scene
			g_pGraphicsMgr->BeginScene( true );

			// Since all visible graphics are represented by UI elements then drawing the UI will render
			// the entire game scene
			g_GUIMgr.Draw();

			// Display the scene
			if( !g_pGraphicsMgr->DisplayScene() )
			{
				// If the images need to be reloaded then do so
				if( g_pGraphicsMgr->ImagesNeedReload() )
				{
					ResourceMgr::Get().RecreateImageResources();
					g_pGraphicsMgr->ClearImageReloadFlag();
					
					// Allow the GUI to handle the refresh
					if( GUIMgr::Get().GetCurrentLayout() )
					{
						GameGUILayout* pCurLayout = (GameGUILayout*)GUIMgr::Get().GetCurrentLayout();
						pCurLayout->OnGraphicsRefresh();
					}

					MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Image data reloaded." );
				}

				// Try to display again
				if( !g_pGraphicsMgr->DisplayScene() )
				{
					MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Failed to redisplay, restarting graphics." );

					bool isFullScreen = g_pGraphicsMgr->IsFullScreen();
					g_pGraphicsMgr->Term();
					g_pGraphicsMgr->Init( m_hWnd, isFullScreen );
					ResourceMgr::Get().RecreateImageResources();
					
					// Allow the GUI to handle the refresh
					if( GUIMgr::Get().GetCurrentLayout() )
					{
						GameGUILayout* pCurLayout = (GameGUILayout*)GUIMgr::Get().GetCurrentLayout();
						pCurLayout->OnGraphicsRefresh();
					}
				}
			}

			QueryPerformanceCounter( &endMetricTime );
			drawTime += (double)(endMetricTime.QuadPart - startMetricTime.QuadPart) * timerFreqScalarDbl;
		}

#ifdef SHOW_FPS
		// The frames per second text
		secTimer += frameTime;
		++frameCount;
		if( secTimer > 1.0f )
		{
			fpsTextLen = swprintf_s( sFPS, FPS_BUF_SIZE, L"FPS: %d", frameCount );
			frameCount = 0;
			secTimer = 0.0f;
		}

		// Display the frames per second
		HDC hDC = GetDC( m_hWnd );
		TextOut( hDC, 0,0, sFPS, fpsTextLen );
		ReleaseDC( m_hWnd, hDC );
#endif

		// Don't hog the cpu
		++totalFrameCount;
		Sleep( SLEEP_TIME );
	}
	QueryPerformanceCounter( &endLoopTime );

	GameMgr::Get().Clear();

	// Output the frame data
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Time spent updating : %f", updateTime );
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Time spent drawing : %f", drawTime );
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Average update time (%u frames): %f", totalFrameCount, updateTime / (double)totalFrameCount );
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Average draw time : %f", drawTime / (double)totalFrameCount );

	double loopTime = (double)(endLoopTime.QuadPart - startLoopTime.QuadPart) * timerFreqScalarDbl;
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Total loop time : %f", loopTime );

	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Unaccounted for time (LoopTime - (update + draw)) : %f", loopTime - (drawTime + updateTime));
	
	double totalTime = updateTime + drawTime;
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Percent update of total time: %f%%", (updateTime / totalTime) * 100.0 );
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ApplicationWin::WindowProc  Static Private
///
///	This function handles all windows messages.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ApplicationWin::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ApplicationWin* pAppWin = static_cast<ApplicationWin*>(Get());

	switch(uMsg)
	{
		// If the user wants to toggle full screen or windows
		case WM_SYSKEYDOWN:
        {
            // Set the flag to signify that the user wants to toggle full-screen
			if( wParam == VK_RETURN )
				pAppWin->m_ToggleFullscreen = true;
        }
		break;

	// If the window is being closed
	case WM_CLOSE:
		{
#ifdef _PT_DEMO
			if( GUIMgr::Get().GetCurrentLayout() && GUIMgr::Get().GetCurrentLayout()->GetLayoutID() != GameDefines::MS_DemoExit )
				GUIMgr::Get().SetActiveLayout( GameDefines::MS_DemoExit );
#else
			pAppWin->SetAppToExit();
#endif
		}
		return 0;

	case WM_SIZE:
		{
			if( wParam == SIZE_MINIMIZED )
				pAppWin->m_IsWinMinimized = true;
			else
				pAppWin->m_IsWinMinimized = false;
		}
		break;
	
	// When the left mouse button is pressed
	case WM_LBUTTONDOWN:
		g_GUIMgr.OnMouseDown( GUIDefines::MB_Left );
		return 0;

	// When the left mouse button is released
	case WM_LBUTTONUP:
		g_GUIMgr.OnMouseUp( GUIDefines::MB_Left );
		return 0;

	// When the middle mouse button is pressed
	case WM_MBUTTONDOWN:
		g_GUIMgr.OnMouseDown( GUIDefines::MB_Middle );
		return 0;

	// When the middle mouse button is released
	case WM_MBUTTONUP:
		g_GUIMgr.OnMouseUp( GUIDefines::MB_Middle );
		return 0;

	// When the right mouse button is pressed
	case WM_RBUTTONDOWN:
		g_GUIMgr.OnMouseDown( GUIDefines::MB_Right );
		return 0;

	// When the right mouse button is released
	case WM_RBUTTONUP:
		g_GUIMgr.OnMouseUp( GUIDefines::MB_Right );
		return 0;

	case WM_MOUSEMOVE:
		{
			g_GUIMgr.SetCursorPos( Point2i( LOWORD(lParam), HIWORD(lParam ) ) );
		}
		break;

	case WM_KEYDOWN:
		{
			// Handle a key being pressed
			int32 keyRepeat = (int)lParam & 0x00FF;
			for( int32 repeatCount = 0; repeatCount < keyRepeat; ++repeatCount )
				g_GUIMgr.OnKeyDown( (unsigned int)wParam );
		}
		return 0;

	case WM_KEYUP:
		{
			// Handle a key being release
			int32 keyRepeat = (int)lParam & 0x00FF;
			for( int32 repeatCount = 0; repeatCount < keyRepeat; ++repeatCount )
				g_GUIMgr.OnKeyUp( (unsigned int)wParam );
		}
		return 0;

	case WM_CHAR:
		{
			// Handle a character being pressed
			wchar_t charPressed = (wchar_t)wParam;

#ifdef ENABLE_GUI_EDITOR
			// If ther character is G display the GUI dialog
			if( charPressed == L'g' )
				GUIEditDlg::Get().Show();
#endif

			int32 keyRepeat = (int)lParam & 0x00FF;
			for( int32 repeatCount = 0; repeatCount < keyRepeat; ++repeatCount )
				g_GUIMgr.OnKeyChar( charPressed );
		}
		return 0;

	default:
		break;
	}

	// Defaultly handle the messages
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ApplicationWin::ToggleFullScreen  Private
///
///	Toggle between windowed and full-screen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationWin::ToggleFullScreen()
{
	// Close the current graphics manager
	bool isCurrentlyFullScreen = g_pGraphicsMgr->IsFullScreen();
	std::list< TCImage* > loadedImages = ResourceMgr::Get().GetAllTCImages();
	g_pGraphicsMgr->FreeTCImageSurfaces( loadedImages );
	g_pGraphicsMgr->Term();

	// If the program is going to windowed mode
	if( isCurrentlyFullScreen )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Toggling from full screen to windowed mode" );

		// Update the window style
		SetWindowLong( m_hWnd, GWL_STYLE, WINSTYLE_WINDOWED );
		SetWindowLong( m_hWnd, GWL_EXSTYLE, 0 );

		// Get the desired size of the window
		RECT clientRect = {0,0,1024,768};
		AdjustWindowRect( &clientRect, WINSTYLE_WINDOWED, FALSE );

		// Position the window and resize it
		if ( SetWindowPos(m_hWnd, NULL, 0, 0, 
				clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
				SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED) == 0 )
		{
		}
	}
	// Else go to full screen
	else
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Toggling from windowed to full screen" );

		// Update the window style
		SetWindowLong( m_hWnd, GWL_STYLE, WINSTYLE_FULLSCREEN );
		SetWindowLong( m_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST );

		// Call SetWindowPos to update the window
		if ( SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED) == 0 )
		{
		}
	}

	// Ensure the window is visible
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	// Reinitialize the graphics manager
	if( !g_pGraphicsMgr->Init( m_hWnd, !isCurrentlyFullScreen ) )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to reinitialize the graphics manager after toggling between windowed and full screen mode" );
	}

	// Update the resource manager
	ResourceMgr::Get().RecreateImageResources();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ApplicationWin::Term  Protected
///
///	Clean up the application
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationWin::Term()
{
	ApplicationBase::Term();

	TraceAssist::FreeOutputWindow();

	// Free the windows class
	UnregisterClass( CLASS_NAME, m_hInstance );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ApplicationWin::RegisterWindowClass  Private
///
///	Register the window class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicationWin::RegisterWindowClass()
{
#ifdef _DEBUG
	HMODULE hCurInst = GetModuleHandle( L"PrimeTime_dbg.dll" );
#else
	HMODULE hCurInst = GetModuleHandle( L"PrimeTime.dll" );
#endif
	
	// Initialize the members of the window class
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(hCurInst, MAKEINTRESOURCE(IDI_MAINICON));
	wndClass.hInstance = m_hInstance;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.lpszMenuName = 0;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	
	// Register the window class
	if( RegisterClass(&wndClass) == NULL )
		return false;

	// Return success
	return true;
}