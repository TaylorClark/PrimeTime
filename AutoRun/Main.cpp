/*=================================================================================================

	\file Main.cpp
	CD Auto-run Application
	Main Window Source
	\author Taylor Clark
	\Date September 3, 2007

	This source file contains the entry point for the CD auto-run application

=================================================================================================*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "Resource.h"
#include "CleanHBitmap.h"
#include "..\\Base\\FileFuncs.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////////////////////////
static LPTSTR CLASS_NAME = _T("PrimeTime");
static LPTSTR PROG_NAME = _T("Prime Time: Math Adventure CD");
const DWORD WINSTYLE_WINDOWED = WS_CAPTION | WS_BORDER | WS_SYSMENU;

enum EButtonID
{
	BID_Play = 0,
	BID_Install,
	BID_Exit,
	BID_COUNT
};

const POINT WIN_SIZE = {640, 480};

const POINT BTN_POS_PLAY = {85,225};
const POINT BTN_SIZE_PLAY = {472,32};

const POINT BTN_POS_INSTALL = {100,296};
const POINT BTN_SIZE_INSTALL = {441,35};

const POINT BTN_POS_EXIT = {274,424};
const POINT BTN_SIZE_EXIT = {98,31};

const RECT BTN_RECTS[BID_COUNT] = {{BTN_POS_PLAY.x,BTN_POS_PLAY.y,BTN_POS_PLAY.x+BTN_SIZE_PLAY.x,BTN_POS_PLAY.y+BTN_SIZE_PLAY.y},
{BTN_POS_INSTALL.x,BTN_POS_INSTALL.y,BTN_POS_INSTALL.x+BTN_SIZE_INSTALL.x,BTN_POS_INSTALL.y+BTN_SIZE_INSTALL.y},
{BTN_POS_EXIT.x,BTN_POS_EXIT.y,BTN_POS_EXIT.x+BTN_SIZE_EXIT.x,BTN_POS_EXIT.y+BTN_SIZE_EXIT.y} };


///////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////////////////////////
bool m_IsAppDone = false;
HINSTANCE g_hInstance = 0;
CleanHBitmap* g_pBGBitmap = 0;
CleanHBitmap* g_pBtnBitmap[BID_COUNT] = {0};
CleanHBitmap* g_pBackBuffer = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool RegisterWindowClass();
RECT GetAppWindowRect();


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	WinMain  Global
///
///	The entry point for the application
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	g_hInstance = hInstance;

	// Register the window class
	if( !RegisterWindowClass() )
		return 0;

	// Create a normal window
	RECT clientRect = GetAppWindowRect();
	HWND hWnd = CreateWindow( CLASS_NAME, PROG_NAME,
					WINSTYLE_WINDOWED,
					clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
					0, 0, g_hInstance, NULL);
	
	// Ensure the window was created
	if(hWnd == NULL)
		return 0;

	g_pBGBitmap = new CleanHBitmap( hInstance, IDB_BGBITMAP );

	HDC hWindowDC = GetDC( hWnd );
	g_pBackBuffer = new CleanHBitmap( hWindowDC, WIN_SIZE.x, WIN_SIZE.y );
	ReleaseDC( hWnd, hWindowDC );

	g_pBtnBitmap[BID_Play] = new CleanHBitmap( hInstance, IDB_BTN_PLAY );
	g_pBtnBitmap[BID_Install] = new CleanHBitmap( hInstance, IDB_BTN_INSTALL );
	g_pBtnBitmap[BID_Exit] = new CleanHBitmap( hInstance, IDB_BTN_EXIT );

	// Display the window
	ShowWindow( hWnd, SW_SHOW );

	// Get a windows message when available
	MSG msg;
	while( !m_IsAppDone )
	{
		// Get the message
		GetMessage(&msg, NULL, 0, 0);

		// Dispatch the message
		DispatchMessage(&msg);
	}

	// Free the windows class
	UnregisterClass( CLASS_NAME, g_hInstance );

	delete g_pBGBitmap;
	g_pBGBitmap = 0;

	delete g_pBackBuffer;
	g_pBackBuffer = 0;

	for( int btnIndex = 0; btnIndex < BID_COUNT; ++btnIndex )
	{
		delete g_pBtnBitmap[btnIndex];
		g_pBtnBitmap[btnIndex] = 0;
	}

	// Return success
	return 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GetAppWindowRect  Global
///
///	\returns The window rectangle in screen pixels for the desired size and position
///
///	Get the proper location and size for the application window.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
RECT GetAppWindowRect()
{
	// Get the desired size of the window
	RECT clientRect = {0,0,WIN_SIZE.x,WIN_SIZE.y};
	AdjustWindowRect( &clientRect, WINSTYLE_WINDOWED, FALSE );

	// Convert the top-left to represent the window top-left rather than the client
	clientRect.right = clientRect.right - clientRect.left;
	clientRect.bottom = clientRect.bottom - clientRect.top;
	clientRect.left = 0;
	clientRect.top = 0;

	// Get the screen dimensions
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Center the window
	clientRect.left = (screenWidth - clientRect.right) / 2;
	clientRect.right += clientRect.left;
	clientRect.top = (screenHeight - clientRect.bottom) / 2;
	clientRect.bottom += clientRect.top;

	return clientRect;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GetButtonAtPoint  Global
///
///	\param testPos The position to search for a button
///	\returns The index of the button at the test point or -1 if none exists
///
///	Find the button from a client location.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static int GetButtonAtPoint( POINT testPos )
{
	for( int btnIndex = 0; btnIndex < BID_COUNT; ++btnIndex )
	{
		const RECT& curRect = BTN_RECTS[btnIndex];

		if( testPos.x > curRect.left && testPos.x < curRect.right
			&& testPos.y > curRect.top && testPos.y < curRect.bottom )
			return btnIndex;
	}

	return -1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	StartProcess  Global
///
///	\param szFileName The executable to launch
///	\param szArgs The command line arguments, can be NULL, to pass to the executable
///
///	Start a new process
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool StartProcess( const wchar_t* szFileName, const wchar_t* szArgs )
{
	// Create an empty startup info object since none of its values our relevant in this case
	STARTUPINFO info;
	memset( &info, 0, sizeof(info) );
	info.cb = sizeof(info);
	PROCESS_INFORMATION procInfo;
	memset( &procInfo, 0, sizeof(procInfo) );
	
	// Copy the command line arguments to a non-const buffer as CreateProcess() requires
	wchar_t* szArgsBuffer = NULL;
	if( szArgs && wcslen(szArgs) > 0 )
	{
		int argsLen = (int)wcslen(szArgs);
		szArgsBuffer = new wchar_t[ argsLen + 1];
		wcscpy_s( szArgsBuffer, argsLen + 1, szArgs );
		szArgsBuffer[ argsLen ] = 0;
	}

	// Run Prime Time in another process
	BOOL procRet = CreateProcess( szFileName, szArgsBuffer, NULL, NULL, FALSE, 0, NULL, NULL, &info, &procInfo );
	
	// Free the args buffer
	if( szArgsBuffer )
		delete [] szArgsBuffer;

	if( procRet == 0 )
		return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	OnButtonPress  Global
///
///	\param buttonID The button that was pressed
///
///	A message handler invoked when the user clicks on a button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static void OnButtonPress( EButtonID buttonID )
{
	switch( buttonID )
	{
	case BID_Play:
		StartProcess( (TCBase::GetAppPath() + L"PlayNow\\PrimeTime.exe").c_str(), L" /runoncd" );
		break;

	case BID_Install:
		StartProcess( (TCBase::GetAppPath() + L"Setup\\PTSetup.exe").c_str(), NULL );
		break;

	case BID_Exit:
		break;
	}

	m_IsAppDone = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	WindowProc  Global
///
///	This function handles all of the windows messages for the application.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		
	// If the window is being closed
	case WM_CLOSE:
		{
			m_IsAppDone = true;
		}
		return 0;
	
	// When the left mouse button is pressed
	case WM_LBUTTONDOWN:
		{
			// Get the mouse position
			POINT mousePos;
			GetCursorPos( &mousePos );
			ScreenToClient( hWnd, &mousePos );

			// Get the button under the mouse
			int overBtnID = GetButtonAtPoint( mousePos );
			if( overBtnID != -1 )
				OnButtonPress( (EButtonID)overBtnID );
		}
		return 0;

	// When the left mouse button is released
	case WM_LBUTTONUP:
		return 0;

	case WM_MOUSEMOVE:
		InvalidateRect( hWnd, NULL, FALSE );
		break;

	case WM_PAINT:
		{
			// Get the mouse position
			POINT mousePos;
			GetCursorPos( &mousePos );
			ScreenToClient( hWnd, &mousePos );

			// Get the button under the mouse
			int overBtnID = GetButtonAtPoint( mousePos );

			BitBlt( g_pBackBuffer->GetDC(), 0, 0, 640, 480, g_pBGBitmap->GetDC(), 0, 0, SRCCOPY);

			// If the mouse is over a button
			if( overBtnID != -1 )
			{
				BitBlt( g_pBackBuffer->GetDC(), BTN_RECTS[overBtnID].left, BTN_RECTS[overBtnID].top,
									BTN_RECTS[overBtnID].right - BTN_RECTS[overBtnID].left,
									BTN_RECTS[overBtnID].bottom - BTN_RECTS[overBtnID].top,
									g_pBtnBitmap[overBtnID]->GetDC(), 0, 0, SRCCOPY);
			}

			PAINTSTRUCT ps;
			HDC hWindowDC = BeginPaint( hWnd, &ps );

			BitBlt( hWindowDC, 0, 0, WIN_SIZE.x, WIN_SIZE.y, g_pBackBuffer->GetDC(), 0, 0, SRCCOPY);

			EndPaint( hWnd, &ps );
		}
		break;

	/*case WM_MOUSEMOVE:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam );
		}
		break;*/

	default:
		break;
	}

	// Defaultly handle the messages
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegisterWindowClass  Global
///
///	Register the window class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegisterWindowClass()
{	
	// Initialize the members of the window class
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	wndClass.hInstance = g_hInstance;
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