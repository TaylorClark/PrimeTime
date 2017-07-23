#include "../ApplicationBase.h"
#include "GUI/GUIMgr.h"
#include "Resource/ResourceMgr.h"
#include "Audio/AudioMgr.h"
#include "Base/MsgLogger.h"
#include "Graphics2D/GraphicsMgr.h"
#ifdef WIN32
#include "Network/NetworkMgr.h"
#endif
#include "Network/HttpRequestMgr.h"
#include "GamePlay/PlayerProfile.h"
#include "GamePlay/GameGUILayout.h"
#include "GamePlay/GUILayout_Game.h"
#include "GamePlay/GameMgr.h"
#include "GamePlay/GameDefines.h"
#include "Base/NumFuncs.h"
#include "Base/SimpleIni.h"


ApplicationBase* ApplicationBase::_pAppInstance = 0;
extern uint32 PT_VERSION;
const ResourceID g_MenuMusicResID = 127;
uint64 ApplicationBase::GameKey = 0;
std::wstring ApplicationBase::s_resourcePath;


extern void DisplayFatalErrorMsg( const wchar_t* szMsg );


Resource* CreateImage( ResourceID resID, DataBlock* pDataBlock )
{
	return (Resource*)g_pGraphicsMgr->LoadImageFromMemory( resID, pDataBlock );
}

Resource* CreateSound( ResourceID resID, DataBlock* pDataBlock )
{
	return (Resource*)AudioMgr::LoadSoundFromMemory( resID, pDataBlock );
}

Resource* CreateMusic( ResourceID resID, DataBlock* pDataBlock )
{
	return (Resource*)AudioMgr::LoadMusicFromMemory( resID, pDataBlock );
}

Resource* CreateFont( ResourceID resID, DataBlock* pDataBlock )
{
	TCFont* pFont = g_pGraphicsMgr->LoadFontFromMemory( resID, pDataBlock );
	pFont->SetImage( ResourceMgr::Get().GetTCImage( pFont->GetImgResID() ) );
	return (Resource*)pFont;
}

Resource* CreateSprite( ResourceID resID, DataBlock* pDataBlock )
{
	RefSprite* pSprite = g_pGraphicsMgr->LoadSpriteFromMemory( resID, pDataBlock );
	pSprite->SetImage( ResourceMgr::Get().GetTCImage( pSprite->GetImgResID() ) );
	return (Resource*)pSprite;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ApplicationBase::DecodeCustomText  Static Private
///
///	This function takes in a string of text and decodes into the message displayed on the title
///	screen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring ApplicationBase::DecodeCustomText( const wchar_t* szText )
{
	std::wstring sRetStr;

	// If the string is invalid then bail
	if( !szText || wcslen( szText ) == 0 )
		return sRetStr;

	// The first value stores the length of the message
	int32 msgLen = (int32)szText[0];
	if( msgLen < 0 || msgLen > 64 || msgLen > (int32)wcslen( szText ) )
		return sRetStr;

	// Go through the characters and turn each value into a valid character
	for( int32 charIndex = 0; charIndex < msgLen; ++charIndex )
		sRetStr += (wchar_t)( (int32)szText[charIndex + 1] / 3);

	// Return the compiled message
	return sRetStr;
}


bool ApplicationBase::InitGameMgrs( const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg )
{
	// Initialize the random number generator
	srand( (unsigned int)time( NULL ) );

	_pAppInstance->InitFromIni();

	// Hookup the functions to load resources
	ResourceMgr::Get().HookupCreateFunc( RT_Image, CreateImage );
	ResourceMgr::Get().HookupCreateFunc( RT_Sound, CreateSound );
	ResourceMgr::Get().HookupCreateFunc( RT_Music, CreateMusic, true );
	ResourceMgr::Get().HookupCreateFunc( RT_Font, CreateFont );
	ResourceMgr::Get().HookupCreateFunc( RT_Sprite, CreateSprite );

	// Initialize the resource manager
	MsgLogger::Get().Output( L"Initializing the resource manager" );
	ResourceMgr::Get().Init();

	// Initialize the audio manager
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Initializing audio..." );
	if( !AudioMgr::Get().Init() )
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to initialize audio..." );

	// Initialize the GUI manager
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Initializing GUI..." );
	if( !GUIMgr::Get().Init() )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to intialize GUI manager." );
		DisplayFatalErrorMsg( L"Prime Time failed to initialize the user interface system. Make sure no other copies of Prime Time are running and/or reboot your system then try again." );

		GUIMgr::Get().Term();
		AudioMgr::Get().Term();
		ResourceMgr::Get().Term();
		g_pGraphicsMgr->Term();
		return false;
	}

#ifndef _PT_DEMO
	// Store the flag if the game is being run off the CD
	if( cmdLineParams.HasOption( L"runoncd" ) )
		Get()->m_IsRunningOffCD = true;

	// Ensure there is a directory for player profiles
	if( !Get()->m_IsRunningOffCD )
		PlayerProfile::EnsureProfileDirExists();
#endif

	// Initialize the game related data
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Loading base resource objects..." );
	if( !GameDefines::InitializeBaseObjects() )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to load required resources" );
		DisplayFatalErrorMsg( L"Prime Time failed to load/find all of the resources required to play. Try restarting your computer to ensure no other files are using the required resources. If this doesn't help please download Prime Time again." );

		GameDefines::FreeBaseObjects();
		GUIMgr::Get().Term();
		AudioMgr::Get().Term();
		ResourceMgr::Get().Term();
		g_pGraphicsMgr->Term();
		return false;
	}

	// Initialize the network manager
#ifdef WIN32
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Initializing network manager..." );
	if( !NetworkMgr::Get().Init() )
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to initialize the network manager." );
#else
	HttpRequestMgr::Init();
#endif

	// Load the menus
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Loading GUI data..." );
	if( !GameGUILayout::LoadLayouts( GetResourcePath() + L"gui.rdb" ) )
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to load GUI data" );

	// Get the title layout
	GUILayoutTitleScreen* pLayout = (GUILayoutTitleScreen*)GUIMgr::Get().GetLayoutByID( (uint32)GameDefines::MS_TitleScreen );
	if( pLayout )
	{
		// If there is a custom message
		if( szCustomText )
		{
			// Display the text
			std::wstring sText = DecodeCustomText( szCustomText );
			pLayout->SetCustomText( sText.c_str() );
		}
		else
			pLayout->SetCustomText( NULL );

		// If there is a custom picture
		if( pCustomPicData && pCustomPicData[0] == 1 )
		{
			DataBlock imageData( pCustomPicData + 1, 4 + 4 + 4 + (128 * 128 * 3) );
			TCImage* pCustomPicImage = g_pGraphicsMgr->LoadImageFromMemory( 95, &imageData );

			pLayout->SetCustomPic( pCustomPicImage, (szCustomPicMsg != NULL) ? DecodeCustomText( szCustomPicMsg ).c_str() : NULL );
		}
		else
			pLayout->SetCustomPic( NULL, NULL );

		// Set the version
		pLayout->SetGameVersionText( PT_VERSION );
	}
	
	// Start at the title screen
	GUIMgr::Get().SetActiveLayout( (uint32)GameDefines::MS_TitleScreen );

	// Play music
	AudioMgr::Get().SetBGMusic( ResourceMgr::Get().GetMusicStream( g_MenuMusicResID ).GetObj() );

	// Handle the command line parameters
	for( const TCBase::ParamList::CmdLineParam* pCurParam = cmdLineParams.GetFirstOption(); pCurParam != 0; pCurParam = cmdLineParams.GetNextOption() )
	{
		// If the player wants to login
		if( pCurParam->sOption == L"login" && pCurParam->sParameters.size() > 0 )
		{
			std::wstring sUserName = pCurParam->sParameters.front();
			if( PlayerProfile::IsValidProfileName( sUserName.c_str() ) )
			{
				MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Auto-logging in %s", sUserName.c_str() );

				// Open the profile and use it if it is valid
				PlayerProfile* pProfile = PlayerProfile::OpenProfile( sUserName.c_str() );
				if( pProfile )
					GameMgr::Get().SetActiveProfile( pProfile );
			}
			else
				MSG_LOGGER_OUT( MsgLogger::MI_Warning, L"Auto-log in specified, but the name was invalid", sUserName.c_str() );
		}
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	ApplicationBase::InitFromIni  Private
///
///	Access settings from the log file.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationBase::InitFromIni()
{
	if( !Settings.LoadFromFile() )
		return;

	++Settings.NumTimesPlayed;

	// If the player wants to login
	if( Settings.AutoLoginProfile.length() > 0 )
	{
		if( PlayerProfile::DoesProfileExist( Settings.AutoLoginProfile.c_str() ) )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Based in the ini file, auto-logging in %s", Settings.AutoLoginProfile.c_str() );

			// Open the profile and use it if it is valid
			PlayerProfile* pProfile = PlayerProfile::OpenProfile( Settings.AutoLoginProfile.c_str() );
			if( pProfile )
				GameMgr::Get().SetActiveProfile( pProfile );
		}
		else
			MSG_LOGGER_OUT( MsgLogger::MI_Warning, L"Ini file auto-log in specified, but the name was invalid", Settings.AutoLoginProfile.c_str() );
	}
	
	// Save the file since we've updated it with the number of times played
	Settings.SaveToFile();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	ApplicationBase::SetAutoLoginUser  Public
///
///	Set the profile that will be logged-in the next time the application is started
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationBase::SetAutoLoginProfile( const wchar_t* szProfileName )
{
	Settings.AutoLoginProfile = szProfileName;
	Settings.SaveToFile();
}


std::wstring ApplicationBase::GetResourcePath()
{
	if( s_resourcePath.size() == 0 )
	{
#ifdef WIN32
		s_resourcePath = TCBase::GetAppPath();
#else
		s_resourcePath = TCBase::GetAppPath() + L"../Resources/";
#endif
	}

	return s_resourcePath;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	ApplicationBase::SendAppConnectMessage  Static Public
///
///	Notify the server of a message.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationBase::SendAppConnectMessage( const wchar_t* szMsg )
{
	std::wstring sInstallUri = L"http://www.PrimeTimeMath.com/AppConnect.aspx?m=";
	sInstallUri += szMsg;
	TCBase::AppendQueryStringParameter( sInstallUri, L"v", TCBase::EasyIToA(PT_VERSION) );
	TCBase::AppendQueryStringParameter( sInstallUri, L"k", TCBase::EasyUI64ToA(ApplicationBase::GameKey) );

#ifdef _PT_DEMO
	TCBase::AppendQueryStringParameter( sInstallUri, L"demo=1" );
#endif

#ifndef WIN32
	TCBase::AppendQueryStringParameter( sInstallUri, L"mac=1" );
#endif
	
	HttpRequestMgr::GetAndForgetAsync( sInstallUri.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	ApplicationBase::TermApp  Static Public
///
///	Clean up the application
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationBase::TermApp()
{
	if( _pAppInstance )
	{
		_pAppInstance->Term();
		delete _pAppInstance;
	}

	_pAppInstance = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	ApplicationBase::Term  Protected
///
///	Clean up the application
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationBase::Term()
{
	AudioMgr::Get().FreeResources();

	GameMgr::Get().SetActiveProfile( NULL );
	GameMgr::Get().Clear();

	GameDefines::FreeBaseObjects();

	GUIMgr::Get().Term();
	
	ResourceMgr::Get().Term();

	AudioMgr::Get().Term();

#ifdef WIN32
	NetworkMgr::Get().Term();
#else
	HttpRequestMgr::Term();
#endif

	g_pGraphicsMgr->Term();

	MsgLogger::Get().Term();
}