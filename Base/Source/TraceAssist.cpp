/*=================================================================================================

	TraceAssist.cpp
	Base Library
	Trace Assisting Source
	\author Taylor Clark
	\Date May 7, 2006

	This source file contains the implementation for the trace helper functions.

=================================================================================================*/

#include "../Types.h"
#include "../TraceAssist.h"
#include <windows.h>
#include <process.h>
#include "../StringFuncs.h"

static const wchar_t* CLASS_NAME = L"TraceAssistWnd";
static const wchar_t* PROG_NAME = L"Trace Output";

/// The trace window handle
static HWND g_TraceHWnd = NULL;

/// The trace window handle
static HWND g_EditHWnd = NULL;

/// The flag for if the trace window is done
static bool g_FlagThreadDone = false;

/// The flag for if the thread is running
static bool g_IsThreadRunning = false;

// The window proc
static LRESULT CALLBACK TraceWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetTraceHWnd  Public
///
///	Get the trace window handle.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void* TraceAssist::GetTraceHWnd()
{
	return g_TraceHWnd;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetTraceHWnd  Public
///
///	Get the trace window handle.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void TraceAssist::FreeOutputWindow()
{
	// End the thread
	g_FlagThreadDone = true;
	while( g_IsThreadRunning )
		Sleep( 5 );
	
	if( !g_TraceHWnd )
		return;

	DestroyWindow( g_TraceHWnd );
	UnregisterClassW( CLASS_NAME, NULL );
	g_TraceHWnd = NULL;
	g_EditHWnd = NULL;
}


static void TraceThread( void* pInstance )
{
	g_IsThreadRunning = true;

	HINSTANCE hInstance = (HINSTANCE)pInstance;

	// Initialize the members of the window class
	WNDCLASSW wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW + 1;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = TraceWindowProc;
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.lpszMenuName = 0;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	
	// Register the window class
	if( RegisterClassW(&wndClass) == NULL )
	{
		// We could not register the window class
		MessageBoxW(NULL, L"Could not register window class.", PROG_NAME, MB_OK);

		// Exit the program
		g_IsThreadRunning = false;
		return;
	}

	// Get the right side of the screen
	int32 winLeft = 0;
	const int WIN_WIDTH = 400;
	HWND hDesktopWnd = GetDesktopWindow();
	if( hDesktopWnd )
	{
		RECT desktopRect;
		GetClientRect( hDesktopWnd, &desktopRect );
		winLeft = desktopRect.right - WIN_WIDTH;
	}

	// Create the window
	g_TraceHWnd = CreateWindowW( CLASS_NAME, PROG_NAME,
						WS_OVERLAPPEDWINDOW | WS_SYSMENU,
						winLeft, 0, WIN_WIDTH, 600,
						0, 0, hInstance, NULL);

	// Ensure the window was created
	if(g_TraceHWnd == NULL)
	{
		// We could not create the window
		// Display error message
		MessageBoxW(NULL, L"Could not create window.", PROG_NAME, MB_OK);

		// Exit the program
		g_IsThreadRunning = false;
		return;
	}

	// Get the trace window client rectangle
	RECT clientRect;
	GetClientRect( g_TraceHWnd, &clientRect );

	// Create the text box
	g_EditHWnd = CreateWindowW( L"EDIT", PROG_NAME,
						WS_CHILD | ES_MULTILINE | ES_READONLY | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_BORDER,
						0, 0, clientRect.right, clientRect.bottom,
						g_TraceHWnd, 0, NULL, NULL);

	// Display the window
	ShowWindow( g_TraceHWnd, SW_SHOW );

	// Loop while the trace window is up or GetMessage breaks
	MSG msg;
	while( !g_FlagThreadDone && g_TraceHWnd )
	{
		while( PeekMessage( &msg, g_TraceHWnd, 0, 0, PM_NOREMOVE ) )
		{
			// Get the message
			if( GetMessage(&msg, g_TraceHWnd, 0, 0) < 1 )
				break;

			// Dispatch the message
			DispatchMessage(&msg);
		}

		// Pass our timeslice off
		Sleep( 50 );
	}

	g_IsThreadRunning = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CreateOutputWindow  Public
///
///	Create an output window specifically for trace output.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void* TraceAssist::CreateOutputWindow( void* pInstance )
{
	// If the window exists then free it
	if( g_TraceHWnd && IsWindow(g_TraceHWnd) )
		FreeOutputWindow();

	// Start the update thread
	g_FlagThreadDone = false;
	uintptr_t retVal = _beginthread( TraceThread, 0, pInstance );
	if( retVal == -1 || retVal == 0 || retVal == 1 )
		return 0;
	else
		g_IsThreadRunning = true;

	// Wait for the window to be created
	while( !g_EditHWnd && g_IsThreadRunning )
		Sleep( 5 );

	// Return a handle to the edit control
	return g_EditHWnd;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	WindowProc()  Global
///
///	This function handles all windows messages.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK TraceWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// If this is a resize message for the main window
	if( uMsg == WM_SIZE && hWnd == g_TraceHWnd )
	{
		// Get the new client dimensions
		int width = (int)LOWORD( lParam );
		int height = (int)HIWORD( lParam );

		// Get the edit window
		POINT pos = { 2, 2 };
		HWND hEditWnd = ChildWindowFromPoint( g_TraceHWnd, pos );
		if( hEditWnd != NULL )
		{
			// Resize the text box
			SetWindowPos( hEditWnd, g_TraceHWnd, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER );
		}
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}