//=================================================================================================
/*!
	\file GameMap.cpp
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

#include <windows.h>
#include <stdio.h>
#include "Graphics2D/GraphicsMgr.h"
#include "Graphics2D/TCFont.h"
#include "GamePlay/AnimSprite.h"
#include "Base/TCAssert.h"
#include "Base/FileFuncs.h"
#include "Resource/ResourceMgr.h"
#include "Base/MsgLogger.h"
#include "Base/TraceAssist.h"
#include "Audio/AudioMgr.h"
#include <time.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////
static const wchar_t* CLASS_NAME = L"PrimeTime";
static const wchar_t* PROG_NAME = L"Prime Time";
#define ERRORMSG(str) MessageBox( NULL, str, L"Prime Time", MB_OK | MB_ICONERROR )


///////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void MainLoop();
extern void TestDDrawPerformance( TCImage* pImage );


///////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////////////////////////
bool g_bProgDone = false;
HWND g_hWnd = 0;
GraphicsMgrBase* g_pGraphicsMgr = &GraphicsMgrBase::Get();
//GraphicsMgrBase& g_GraphicsMgr = *g_pGraphicsMgr;


Resource* CreateImage( ResourceID resID, DataBlock dataBlock )
{
	return (Resource*)g_pGraphicsMgr->LoadImageFromMemory( resID, dataBlock );
}

Resource* CreateSound( ResourceID resID, DataBlock dataBlock )
{
	return (Resource*)AudioMgr::LoadSoundFromMemory( resID, dataBlock );
}

Resource* CreateMusic( ResourceID resID, DataBlock dataBlock )
{
	return (Resource*)AudioMgr::LoadMusicFromMemory( resID, dataBlock );
}

Resource* CreateFont( ResourceID resID, DataBlock dataBlock )
{
	TCFont* pFont = g_pGraphicsMgr->LoadFontFromMemory( resID, dataBlock );
	pFont->SetImage( ResourceMgr::Get().GetTCImage( pFont->GetImgResID() ) );
	return (Resource*)pFont;
}

Resource* CreateSprite( ResourceID resID, DataBlock dataBlock )
{
	RefSprite* pSprite = g_pGraphicsMgr->LoadSpriteFromMemory( resID, dataBlock );
	pSprite->SetImage( ResourceMgr::Get().GetTCImage( pSprite->GetImgResID() ) );
	return (Resource*)pSprite;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  WinMain()  Global
///
///	Program execution begins in this function.  The window class is registered and the window is
///	created.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{	
	MsgLogger::Get().AddListener( (TCBase::GetAppPath() + L"Output.txt").c_str() );
	
	ResourceMgr::Get().HookupCreateFunc( RT_Image, CreateImage );
	ResourceMgr::Get().HookupCreateFunc( RT_Sound, CreateSound );
	ResourceMgr::Get().HookupCreateFunc( RT_Music, CreateMusic, true );
	ResourceMgr::Get().HookupCreateFunc( RT_Font, CreateFont );
	ResourceMgr::Get().HookupCreateFunc( RT_Sprite, CreateSprite );

	MsgLogger::Get().Output( L"Initializing the resource manager" );
	ResourceMgr::Get().Init();
	
	MsgLogger::Get().Output( L"Preparing graphics" );
	
	// Set a global exception handler
	//SetUnhandledExceptionFilter( CrashExceptionHandler );

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

	
	/*WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX*/

#ifdef _DEBUG
	// Create the window
	g_hWnd = CreateWindow( CLASS_NAME, PROG_NAME,
						WS_OVERLAPPEDWINDOW | WS_EX_TOPMOST | WS_CLIPCHILDREN,
						CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
						0, 0, hInstance, NULL);
#else
	// Create the window for release mode
	g_hWnd = CreateWindow( CLASS_NAME, PROG_NAME,
						WS_MAXIMIZE | WS_POPUP | WS_EX_TOPMOST,
						CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
						0, 0, hInstance, NULL);
	

	// Create the window for release mode
	/*HWND hWnd = CreateWindow( CLASS_NAME, PROG_NAME,
						WS_MAXIMIZE | WS_OVERLAPPEDWINDOW | WS_EX_TOPMOST,
						CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
						0, 0, hInstance, NULL);*/
#endif

	// Ensure the window was created
	if(g_hWnd == NULL)
	{
		// We could not create the window
		// Display error message
		MessageBox(NULL, L"Could not create window.", PROG_NAME, MB_OK);

		// Exit the program
		return 0;
	}

	// Initialize the random number generator
	srand( (unsigned int)time( NULL ) );

	// Initialize the graphics manager
	if( !g_pGraphicsMgr->Init( g_hWnd, false ) )
	{
		ERRORMSG( L"Failed to intialize graphics manager." );
		g_pGraphicsMgr->Term();
		return 0;
	}

	// Initialize the audio manager
	AudioMgr::Get().Init();

	// Display the window
	ShowWindow( g_hWnd, SW_SHOW );

	// Initialize the boolean done value before entering the main message loop
	g_bProgDone = false;

	MainLoop();
	
	ResourceMgr::Get().Term();

	AudioMgr::Get().Term();

	g_pGraphicsMgr->Term();

	MsgLogger::Get().Term();

	TraceAssist::FreeOutputWindow();

	// Free the windows class
	UnregisterClass( CLASS_NAME, hInstance );
	
	// Exit the program and return no problem
	return 0;
}


struct ViewData
{
	TCImageHndl curImage;
	AnimSprite curSprite;
	TCFontHndl curFont;
	AudioSampleHndl curAudioSample;
	MusicStreamHndl curMusicStream;
	bool isPlayingSound;
	EResourceType resType;
	std::wstring sFontText;
	int32 soundChannelIndex;
	float soundLoopWaitTimer;

	ViewData() : isPlayingSound( false ),
					resType( RT_Error ),
					soundChannelIndex( -1 ),
					soundLoopWaitTimer( 0.0f )
	{}

	void Release()
	{
		curImage.ReleaseHandle();
		curSprite = AnimSprite();
		curFont.ReleaseHandle();
		if( isPlayingSound )
		{
			isPlayingSound = false;
			AudioMgr::Get().StopSample( soundChannelIndex );
			AudioMgr::Get().StopMusic( curMusicStream.GetObj() );
			soundChannelIndex = -1;
		}
		curAudioSample.ReleaseHandle();
		AudioMgr::Get();
	}
};


ResourceID g_CurResID = 0;
ResourceID g_NextResID = ResourceMgr::STARTING_RES_ID;
Box2i g_FontRect( 0, 0, 250, 500 );


void LoadItem( ViewData& viewData, ResourceID resIDToLoad )
{
	// Free the current data
	viewData.Release();

	// Ensure a valid resource ID
	if( resIDToLoad - ResourceMgr::STARTING_RES_ID >= ResourceMgr::Get().GetKnownRes().size() )
		return;
	
	ResourceMgr::KnownResourceItem curResItem = ResourceMgr::Get().GetKnownRes()[resIDToLoad - ResourceMgr::STARTING_RES_ID];
	viewData.resType = curResItem.indexData.resType;
	switch( viewData.resType )
	{
		case RT_Image:
		{
			viewData.curImage = ResourceMgr::Get().GetTCImage( resIDToLoad );
		}
		break;

		case RT_Sound:
		{
			viewData.curAudioSample = ResourceMgr::Get().GetAudioSample( resIDToLoad );
		}
		break;

		case RT_Music:
		{
			viewData.curMusicStream = ResourceMgr::Get().GetMusicStream( resIDToLoad );
		}
		break;

		case RT_Font:
		{
			const wchar_t* FONT_TEXT = L"AaBbCcDdEeFf GgHhIiJjKkLlMm NnOoPpQqRrSs TtUuVvWwXxYyZz 0123456789 -!:()";
			viewData.curFont = ResourceMgr::Get().GetTCFont( resIDToLoad );
			if( viewData.curFont.GetObj() )
				viewData.sFontText = viewData.curFont->GetWordWrapString( FONT_TEXT, g_FontRect );
		}
		break;

		case RT_Sprite:
		{
			viewData.curSprite = AnimSprite( ResourceMgr::Get().GetRefSprite(resIDToLoad) );
			viewData.curSprite.Play();
		}
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MainLoop()  Global
///
///	The function containing the main game loop.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MainLoop()
{
	// Initialize the performance counter
	LARGE_INTEGER timerFreq, curFrameTime, newFrameTime;
	QueryPerformanceFrequency( &timerFreq );	
	float timerFreqScalar = 1.0f / (float)timerFreq.QuadPart;

	ViewData viewData;
	LoadItem( viewData, g_NextResID );

	RECT clientRect;
	GetClientRect( g_hWnd, &clientRect );
	g_FontRect.pos.x = (clientRect.right - g_FontRect.size.x) / 2;
	g_FontRect.pos.y = (clientRect.bottom - g_FontRect.size.y) / 2;

	g_pGraphicsMgr->DisplayScene( true );

	// The main message loop
	MSG msg;
	QueryPerformanceCounter( &curFrameTime );
	uint32 resIndex = 0;
	const uint32 RES_ID_BUF_LEN = 128;
	wchar_t szResIDBuffer[RES_ID_BUF_LEN] = {0};
	int resIDStrLen = 0;
	const int FPS_BUF_SIZE = 128;
	wchar_t sFPS[FPS_BUF_SIZE] = {0};
	while( g_bProgDone == false )
	{
		// Get a windows message when available
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
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

		// If we need to load a new resource
		if( g_NextResID != 0 && g_CurResID != g_NextResID )
		{
			LoadItem( viewData, g_NextResID );
			g_CurResID = g_NextResID;
			g_NextResID = 0;

			uint32 resIndex = g_CurResID - ResourceMgr::STARTING_RES_ID;
			if( resIndex >= 0 && resIndex < ResourceMgr::Get().GetKnownRes().size() )
			{
				ResourceMgr::KnownResourceItem curResItem = ResourceMgr::Get().GetKnownRes()[ resIndex ];
				resIDStrLen = swprintf_s( szResIDBuffer, RES_ID_BUF_LEN, L"Res ID: %d   Type: %s   Name: %s", g_CurResID, Resource::GetResTypeStr(viewData.resType), curResItem.indexData.szName );
			}
		}

		switch( viewData.resType )
		{
			case RT_Image:
			{
				if( viewData.curImage.GetObj() )
					g_pGraphicsMgr->DrawImage( Point2i(100,100), viewData.curImage.GetObj(), Box2i(Point2i(),viewData.curImage.GetObj()->GetDims()) );
			}
			break;

			case RT_Sound:
			{
				// If we aren't playing then start
				if( !viewData.isPlayingSound )
				{
					viewData.soundChannelIndex = AudioMgr::Get().PlaySample( viewData.curAudioSample.GetObj() );
					viewData.isPlayingSound = true;
					viewData.soundLoopWaitTimer = 0.0f;
				}
				// Else we are playing
				else
				{
					// If the sample is stopped playing
					bool isPlaying = AudioMgr::Get().IsChannelPlaying( viewData.soundChannelIndex );
					if( !isPlaying )
					{
						viewData.soundLoopWaitTimer += frameTime;

						// If there has been enough delay then play the sound again
						if( viewData.soundLoopWaitTimer >= 1.0f )
						{
							viewData.soundLoopWaitTimer = 0.0f;
							viewData.soundChannelIndex = AudioMgr::Get().PlaySample( viewData.curAudioSample.GetObj() );
						}

					}
				}
			}
			break;

			case RT_Music:
			{
				// If we aren't playing then start
				if( !viewData.isPlayingSound )
				{
					//viewData.soundChannelIndex = AudioMgr::Get().PlayMusic( viewData.curMusicStream.GetObj() );
					AudioMgr::Get().PlayMusic( viewData.curMusicStream.GetObj() );
					viewData.isPlayingSound = true;
				}				
			}
			break;

			case RT_Font:
			{
				// Draw the bound rectangle
				g_pGraphicsMgr->DrawRect( g_FontRect, 0x0000FF00 );

				// Draw the letters
				g_pGraphicsMgr->DrawFontTextClipped( viewData.curFont.GetObj(), viewData.sFontText.c_str(), g_FontRect );
			}
			break;

			case RT_Sprite:
			{
				viewData.curSprite.Update( frameTime );
				g_pGraphicsMgr->DrawImage( Point2i(100,100),viewData.curSprite.GetImage(), viewData.curSprite.GetSrcCoords() );
			}
			break;
		}

		// Display the scene
		g_pGraphicsMgr->DisplayScene();

		// Display the frames per second
		int32 numChars = swprintf_s( sFPS, FPS_BUF_SIZE, L"FPS: %f", (1.0f / frameTime) );
		HDC hDC = GetDC( g_hWnd );
		TextOut( hDC, 0,0, sFPS, numChars );
		ReleaseDC( g_hWnd, hDC );

		// Don't hog the cpu
		Sleep( 10 );
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
				g_NextResID = g_CurResID - ResourceMgr::STARTING_RES_ID;
				do
				{
					g_NextResID++;
					if( g_NextResID >= ResourceMgr::Get().GetNumResources() )
						g_NextResID = 0;

					if( g_NextResID >= ResourceMgr::Get().GetNumResources() )
					{
						g_NextResID = 0;
						break;
					}

				}while( ResourceMgr::Get().GetKnownRes()[g_NextResID].indexData.resourceID == 0 && g_NextResID != g_CurResID );

				g_NextResID += ResourceMgr::STARTING_RES_ID;
			}
			else if( wParam == VK_SUBTRACT || wParam == VK_DOWN  || wParam == VK_LEFT )
			{
				g_NextResID = g_CurResID - ResourceMgr::STARTING_RES_ID;
				do
				{
					if( g_NextResID <= 0 )
						g_NextResID = ResourceMgr::Get().GetNumResources() - 1;
					else
						g_NextResID--;
					

					if( g_NextResID >= ResourceMgr::Get().GetNumResources() )
					{
						g_NextResID = 0;
						break;
					}

				}while( ResourceMgr::Get().GetKnownRes()[g_NextResID].indexData.resourceID == 0 && g_NextResID != g_CurResID );

				g_NextResID += ResourceMgr::STARTING_RES_ID;
			}
			else if( wParam == 'T' )
				TestDDrawPerformance(ResourceMgr::Get().GetTCImage(100).GetObj());
		}
		return 0;

	default:
		break;
	}

	// Defaultly handle the messages
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}