#include "../ApplicationSFML.h"
#include "Graphics2D/GraphicsMgr.h"
#include "GUI/GUIMgr.h"
#include "Resource/ResourceMgr.h"
#include "GamePlay/GameMgr.h"
#include "Base/MsgLogger.h"
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Base/NumFuncs.h"
#include "GamePlay/GameGUILayout.h"

#ifdef _DEBUG
#define SHOW_FPS
const int32 FPS_BUF_SIZE = 128;
wchar_t sFPS[FPS_BUF_SIZE] = {0};
sf::Font* g_pFpsFont = NULL;
sf::Text* g_pFpsText = NULL;
float g_fpsFrameTime;
#endif


GraphicsMgrBase* g_pGraphicsMgr = 0;
static GUIMgr& g_GUIMgr = GUIMgr::Get();
const std::string PROG_NAME("Prime Time");

const float SLEEP_TIME = 0.0078125f;

extern void DisplayFatalErrorMsg( const wchar_t* szMsg );


/// Initialize the game
bool ApplicationSFML::InitApp( const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg )
{
	ApplicationSFML* pAppSFML = new ApplicationSFML();
	_pAppInstance = pAppSFML;

	return pAppSFML->Init( cmdLineParams, szCustomText, pCustomPicData, szCustomPicMsg );
}

static int32 s_FrameCount = 0;
static float32 s_LastFrameTime = 0.0f;

void ApplicationSFML::Run()
{
#ifdef SHOW_FPS
    // Initialize the objects needed to display the frames per second
    g_pFpsFont = new sf::Font();
	g_pFpsFont->loadFromFile("C:\\Windows\\Fonts\\ariblk.ttf");

    g_pFpsText = new sf::Text();
    g_pFpsText->setFont( *g_pFpsFont );
    g_pFpsText->setPosition( 0, 0 );
    g_pFpsText->setCharacterSize( 16 );
    g_pFpsText->setColor( sf::Color::Green );
#endif

	while( !m_IsAppDone )
	{
		// Handle events
		sf::Event curEvent;
		while (_pRenderWindow->pollEvent(curEvent))
			HandleEvent( curEvent );

		float frameTime = _clock.restart().asSeconds();
		s_LastFrameTime = frameTime;

        if( _timeToShowDisplayMode > 0.0f )
            _timeToShowDisplayMode -= frameTime;

		// Update the user interface
		g_GUIMgr.Update( frameTime );
		
		// Update the audio manager
		AudioMgr::Get().Update( frameTime );

		// If the game is running then update it then draw it
		if( GameMgr::Get().IsPlaying() )
			GameMgr::Get().Update( frameTime );

        if( _activeDisplayMode != _targetDisplayMode )
            GoToTargetDisplayMode();

#ifdef SHOW_FPS
        g_fpsFrameTime = frameTime;
#endif
		Draw();

		// Don't hog the cpu
		sf::sleep( sf::seconds( SLEEP_TIME ) );
	}

	_pRenderWindow->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  ApplicationSFML::Draw  Private
///
///	Draw the scene.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationSFML::Draw()
{
	// If the window is not visible then don't draw
	if( m_IsWinMinimized )
		return;

	_pRenderWindow->setActive(true);
		
	// Begin a new scene
	g_pGraphicsMgr->BeginScene( false );

	// Since all visible graphics are represented by UI elements then drawing the UI will render
	// the entire game scene
	g_GUIMgr.Draw();

	
	//sf::Unicode::Text countText( TCBase::EasyIToA( (int)(1.0f / s_LastFrameTime) ) );
	//sf::String s(countText);
	//s.SetPosition(0,0);
	//_pRenderWindow->Draw(s);
	 
	if( _timeToShowDisplayMode > 0.0f )
    {
        int stringYOffset = GameDefines::GetDefaultGameFont()->GetCharHeight() + 5;
        g_pGraphicsMgr->DrawFontText( GameDefines::GetDefaultGameFont().GetObj(), L"Full Screen", Point2i( 5, 5 ), _activeDisplayMode == GraphicsMgrBase::DM_NormalFill ? 0xFFFFFFFF : 0xFF808080 );
        g_pGraphicsMgr->DrawFontText( GameDefines::GetDefaultGameFont().GetObj(), L"Widescreen", Point2i( 5, 5 + stringYOffset ), _activeDisplayMode == GraphicsMgrBase::DM_WideScreen ? 0xFFFFFFFF : 0xFF808080 );
        g_pGraphicsMgr->DrawFontText( GameDefines::GetDefaultGameFont().GetObj(), L"Centered", Point2i( 5, 5 + stringYOffset * 2 ), _activeDisplayMode == GraphicsMgrBase::DM_CenteredDesktop ? 0xFFFFFFFF : 0xFF808080 );
        g_pGraphicsMgr->DrawFontText( GameDefines::GetDefaultGameFont().GetObj(), L"Windowed", Point2i( 5, 5 + stringYOffset * 3), _activeDisplayMode == GraphicsMgrBase::DM_Windowed ? 0xFFFFFFFF : 0xFF808080 );
    }

#ifdef SHOW_FPS
		// Display the frames per second
		swprintf_s( sFPS, FPS_BUF_SIZE, L"FPS: %d", (int)(1.0f / g_fpsFrameTime) );
        
        g_pFpsText->setString( sf::String( sFPS ) );
        
        _pRenderWindow->draw( *g_pFpsText );
#endif

	// Display the scene
	g_pGraphicsMgr->DisplayScene();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  ApplicationSFML::CycleToNextDisplayMode  Public
///
/// Cycle through the available display modes
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationSFML::CycleToNextDisplayMode()
{
    _targetDisplayMode = g_pGraphicsMgr->GetNextDisplayMode( _activeDisplayMode );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  ApplicationSFML::SetDisplayMode  Public
///
/// Change the display mode
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationSFML::SetDisplayMode( GraphicsMgrBase::EDisplayMode displayMode )
{
    _targetDisplayMode = displayMode;
}


void SetWindowDisplay(sf::RenderWindow* pWindow, GraphicsMgrBase::EDisplayMode displayMode )
{
    sf::ContextSettings winSettings;
	winSettings.antialiasingLevel = 0;

    switch( displayMode )
    {
    case GraphicsMgrBase::DM_NormalFill:
        pWindow->create( sf::VideoMode( 1024, 768, 32 ), PROG_NAME, sf::Style::Fullscreen, winSettings );
        break;

    case GraphicsMgrBase::DM_WideScreen:
    case GraphicsMgrBase::DM_CenteredDesktop:
        {
            sf::VideoMode desktopMode( g_pGraphicsMgr->DesktopDims.x, g_pGraphicsMgr->DesktopDims.y, 32 );
            pWindow->create( desktopMode, PROG_NAME, sf::Style::Fullscreen, winSettings );
        }
        break;

    case GraphicsMgrBase::DM_Windowed:
        pWindow->create( sf::VideoMode(1024, 768, 32), PROG_NAME, sf::Style::Titlebar | sf::Style::Close, winSettings );
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  ApplicationSFML::GoToTargetDisplayMode  Public
///
/// Update our display mode
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationSFML::GoToTargetDisplayMode()
{
    // If we just changed modes then make sure to show the available list of modes in the upper
    // left for a few seconds
    if( _targetDisplayMode != Settings.IdealDisplayMode )
        _timeToShowDisplayMode = 5.0f;

    SetWindowDisplay( _pRenderWindow, _targetDisplayMode );    
    
    // Tell the GraphicsMgr to update rendering
    g_pGraphicsMgr->SetDisplayMode( _targetDisplayMode );

    // Allow the GUI to handle the refresh
	if( GUIMgr::Get().GetCurrentLayout() )
	{
		GameGUILayout* pCurLayout = (GameGUILayout*)GUIMgr::Get().GetCurrentLayout();
		pCurLayout->OnGraphicsRefresh();
	}

    _activeDisplayMode = _targetDisplayMode;

    Settings.IdealDisplayMode = _activeDisplayMode;
    Settings.SaveToFile();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  ApplicationSFML::HandleEvent  Public
///
/// Handle an SFML app event
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationSFML::HandleEvent( const sf::Event& curEvent )
{
	ApplicationSFML* pAppSFML = static_cast<ApplicationSFML*>(Get());

	switch(curEvent.type)
	{
		case sf::Event::KeyPressed:
		{
			// ALT + Enter toggles full screen
			if( curEvent.key.alt && curEvent.key.code == sf::Keyboard::Return )
				pAppSFML->m_ToggleFullscreen = true;
			else if( curEvent.key.code >= sf::Keyboard::A && curEvent.key.code <= sf::Keyboard::Z )
			{   
				wchar_t pressedChar = (wchar_t)((int)curEvent.key.code - (int)sf::Keyboard::A + (int)'A');

				g_GUIMgr.OnKeyChar( pressedChar );
			}
			else
				g_GUIMgr.OnKeyDown( (unsigned int)curEvent.key.code );
		}
		break;

		case sf::Event::KeyReleased:
			g_GUIMgr.OnKeyUp( (unsigned int)curEvent.key.code );
			break;

		case sf::Event::Closed:
			{
#ifdef _PT_DEMO
			if( GUIMgr::Get().GetCurrentLayout() && GUIMgr::Get().GetCurrentLayout()->GetLayoutID() != GameDefines::MS_DemoExit )
				GUIMgr::Get().SetActiveLayout( GameDefines::MS_DemoExit );
#else
				ApplicationBase::Get()->SetAppToExit();
#endif
			}
			break;

		case sf::Event::Resized:
			pAppSFML->m_IsWinMinimized = curEvent.size.width == 0;
			break;
	
		case sf::Event::MouseButtonPressed:
			{
				if( curEvent.mouseButton.button == sf::Mouse::Left )
					g_GUIMgr.OnMouseDown( GUIDefines::MB_Left );
				else if( curEvent.mouseButton.button == sf::Mouse::Middle )
					g_GUIMgr.OnMouseDown( GUIDefines::MB_Middle );
				else if( curEvent.mouseButton.button == sf::Mouse::Right )
					g_GUIMgr.OnMouseDown( GUIDefines::MB_Right );
			}
			break;

		case sf::Event::MouseButtonReleased:
			{
				if( curEvent.mouseButton.button == sf::Mouse::Left )
					g_GUIMgr.OnMouseUp( GUIDefines::MB_Left );
				else if( curEvent.mouseButton.button == sf::Mouse::Middle )
					g_GUIMgr.OnMouseUp( GUIDefines::MB_Middle );
				else if( curEvent.mouseButton.button == sf::Mouse::Right )
					g_GUIMgr.OnMouseUp( GUIDefines::MB_Right );
			}
			break;

		case sf::Event::MouseMoved:
            {
                sf::Vector2f pos = _pRenderWindow->mapPixelToCoords( sf::Vector2i(curEvent.mouseMove.x, curEvent.mouseMove.y) );
			    g_GUIMgr.SetCursorPos( Point2i( (int32)pos.x, (int32)pos.y ) );
            }
			break;

	default:
		break;
	}

}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  ApplicationSFML::Init  Public
///
/// Initialize the game
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicationSFML::Init( const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg )
{
	// Get the instance of the graphics manager
	g_pGraphicsMgr = &GraphicsMgrBase::Get();

    // If the player specified the windowed command line parameter then start windowed
	if( cmdLineParams.HasOption( L"windowed" ) )
		_targetDisplayMode = GraphicsMgrBase::DM_Windowed;
    else
    {
        Settings.LoadFromFile();
        _targetDisplayMode = Settings.IdealDisplayMode;
    }

    // Before we resize the display, get the current desktop display resolution
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    GraphicsMgrBase::DesktopDims.x = desktopMode.width;
    GraphicsMgrBase::DesktopDims.y = desktopMode.height;

    // Create the window with the ideal display mode
    _pRenderWindow = new sf::RenderWindow();
    SetWindowDisplay( _pRenderWindow, _targetDisplayMode );
    _activeDisplayMode = _targetDisplayMode;
    
	// Ensure the window was created
	if(_pRenderWindow == NULL)
	{
		// We could not create the window
		// Display error message
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to create window" );
		DisplayFatalErrorMsg( L"Prime Time could not create a window for the game. Try restarting your computer and playing again." );

		// Exit the program
		return false;
	}

	// Initialize the graphics manager
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Window class registered and window created, now initializing graphics..." );
	if( !g_pGraphicsMgr->Init( _pRenderWindow, false ) )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to intialize graphics manager." );
		DisplayFatalErrorMsg( L"The graphics could not be initialized properly. Prime Time requires at least DirectX 5 and a monitor that can display at least 1024x768 resolution. If you meet these requirements then make sure no other games are running and/or restart your computer and then try running Prime Time again." );

		g_pGraphicsMgr->Term();
		return false;
	}

    bool retMgrs = ApplicationBase::InitGameMgrs( cmdLineParams, szCustomText, pCustomPicData, szCustomPicMsg );

    if( retMgrs )
        _targetDisplayMode = Settings.IdealDisplayMode;

	return retMgrs;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	ApplicationSFML::Term  Protected
///
///	Clean up the application
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationSFML::Term()
{
#ifdef SHOW_FPS
    delete g_pFpsText;
    g_pFpsText = NULL;

    delete g_pFpsFont;
    g_pFpsFont = NULL;
#endif

	ApplicationBase::Term();

	if( _pRenderWindow )
	{
		delete _pRenderWindow;
		_pRenderWindow = NULL;
	}
}