//=================================================================================================
/*!
	\file GameGUILayout.cpp
	Game Play Library
	Game GUI Base Layout Source
	\author Taylor Clark
	\date July 10, 2006

	This source file contains the implementation for base game GUI layout class.
*/
//=================================================================================================

#include "../GameGUILayout.h"
#include "Base/NetSafeSerializer.h"
#include <fstream>
#include "Base/MsgLogger.h"
#include "../GameDefines.h"
#include "GUI/GUIMgr.h"
#include "../GUILayout_Game.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Resource/ResourceMgr.h"
#include "GUI/GUIControl.h"
#include "Base/NumFuncs.h"
#include "Graphics2D/GraphicsDefines.h"
#include "GUI/GUIControl.h"
#include "Audio/AudioMgr.h"
#include "GUI/GUIMessages.h"
#include "../AnimSprite.h"
#include "Base/StringFuncs.h"

const float32 STATIC_DISP_TIME = 0.25f;
const float32 CHANNEL_NUM_DISP_TIME = 1.75f;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameGUILayout::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameGUILayout::OnActivate()
{
#ifdef _DEBUG
	OnShowCtrlsActivate();
#endif // _DEBUG

	// Activate the base class
	GUILayout::OnActivate();

	// Clear the time since activation
	m_TimeSinceActivate = 0.0f;

	/// Get the channel number font
	m_ChannelNumFont = ResourceMgr::Get().GetTCFont( RESID_FONT_CHANNEL_FONT );

	AudioSampleHndl staticSound = ResourceMgr::Get().GetAudioSample( RESID_SOUND_TV_STATIC );
	if( staticSound.GetObj() )
	{
		SoundInstance playingStatic = staticSound.GetObj()->Play();
		//playingStatic.SetVolume( 0.5f );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameGUILayout::OnGraphicsRefresh  Public
///
/// Reset the needed data if the graphics system has been reset
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameGUILayout::OnGraphicsRefresh()
{
	GUIMgr::Get().GetCurrentLayout()->DrawStaticsToBackground();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameGUILayout::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameGUILayout::Update( float32 frameTime )
{
	// Update the base class
	GUILayout::Update( frameTime );

	// If static is finishing this frame
	if( m_TimeSinceActivate < STATIC_DISP_TIME && (m_TimeSinceActivate + frameTime) > STATIC_DISP_TIME )
	{
		// Complete the drawing of any static objects to the background
		DrawStaticsToBackground();
	}

	// Update the time since this layout was loaded
	m_TimeSinceActivate += frameTime;	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameGUILayout::HandleMessage  Public
///
///	\param pMsg The GUI message to handle
///	\returns Whether or not the message was handled
///
///	Handle a GUI message.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameGUILayout::HandleMessage( GUIMessage* pMsg )
{
	// Allow the normal message handling to process
	bool msgHandled = GUILayout::HandleMessage( pMsg );
	if( msgHandled )
		return true;

	// If the unhandled message is a keydown message
	if( pMsg->GetType() == GUIDefines::GMT_KeyDown )
	{
		GUIMessageKeyDown* pKeyMsg = reinterpret_cast<GUIMessageKeyDown*>( pMsg );
		
		// If 'S' was pushed then toggle the sound
		if( pKeyMsg->GetKeyCode() == (uint32)'S' )
		{
			float32 masterVolume = AudioMgr::Get().GetMasterVolume();
			if( masterVolume < 0.1f )
				AudioMgr::Get().SetMasterVolume( 1.0f );
			else if( masterVolume < 0.6f )
				AudioMgr::Get().SetMasterVolume( 0.0f );
			else
				AudioMgr::Get().SetMasterVolume( 0.5f );
		}

		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameGUILayout::Draw  Public
///
///	Draw the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameGUILayout::Draw() const
{
	// If time is still elapsing to draw static
	if( m_TimeSinceActivate < STATIC_DISP_TIME )
	{
		RefSpriteHndl staticSprite = GameDefines::GetStaticSprite();
		const Vector2i spriteSize( staticSprite->GetSrcRect().size );
		const uint32 numFrames = staticSprite->GetNumFrames();
		AnimSprite animStatic( staticSprite );
				
		Vector2i displayDims = g_pGraphicsMgr->GetDisplayDims();
		//const int32 HALF_SCREEN_WIDTH = (displayDims.x / 2) + 1;
		//const int32 HALF_SCREEN_HEIGHT = (displayDims.y / 2) + 1;
				
		// Draw the static
		const uint32 frameIndex = rand() % numFrames;
		g_pGraphicsMgr->DrawImageEx( Box2i(Point2i(),displayDims), staticSprite->GetImage(), staticSprite->GetSrcRect(frameIndex), 0 );
		
		// Draw the border and back control if they exist
		const GUIControl* pCtrl = GetCtrlByName( L"tvframe_sprite" );
		if( pCtrl )
			pCtrl->Draw();
		pCtrl = GetCtrlByName( L"back_button" );
		if( pCtrl )
			pCtrl->Draw();
	}
	else
	{
		// Draw normally
		GUILayout::Draw();

		// If the time is still elapsing to draw the channel number
		if( m_TimeSinceActivate < CHANNEL_NUM_DISP_TIME && m_ChannelNumFont.GetObj() && m_LayoutID != GameDefines::MS_InGame1Player )
			g_pGraphicsMgr->DrawFontText( m_ChannelNumFont.GetObj(), TCBase::EasyIToA(m_LayoutID + 2), Point2i( 800, 75 ) );
	}
}

	
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameGUILayout::LoadLayouts  static Public
///
///	\param dataBlock The data block of GUI data
/// \returns True if the GUI resource data was successfully loaded, otherwise false
///
///	Load the menu data from memory.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameGUILayout::LoadLayouts( const std::wstring& sFilePath )
{
	// Open the file
	std::ifstream inFile( TCBase::Narrow(sFilePath).c_str(), std::ios_base::in | std::ios_base::binary );
	if( !inFile )
		return false;

	// Load the file
   	NetSafeSerializer serializer( &inFile );
	bool loaded = TransferGUIResourceData( serializer );

	// Close the file
	inFile.close();
	
	// Return success
	return loaded;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameGUILayout::TransferGUIResourceData  Static Public
///
///	\param serializer The serializer object to load and save data
/// \returns True if the GUI resource data was successfully loaded/saved, otherwise false
///
///	Load or save the GUI data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameGUILayout::TransferGUIResourceData( Serializer& serializer )
{
	// If we are loading then clear the GUI data
	if( serializer.InReadMode() )
		GUIMgr::Get().ClearLayouts();

	// Read in the FourCC key
	int32 fourCCKeyVal = GUIMgr::FOURCCKEY_GUI.ToInt32();
	serializer.AddData( fourCCKeyVal );
	FourCC fileFourCC( fourCCKeyVal );
	
	// Ensure the key is correct
	if( fileFourCC != GUIMgr::FOURCCKEY_GUI )
	{
		// Bail
		MsgLogger::Get().Output( MsgLogger::MI_Note, L"The GUI rdb file is not the correct resource file due to an invalid FourCC code." );
		return false;
	}

	// The number of layouts
	uint32 numLayouts = GUIMgr::Get().GetNumLayouts();
	serializer.AddData( numLayouts );
	
	// If we are reading in
	if( serializer.InReadMode() )
	{
		// Go through each layout
		bool loadedLayoutIDs[GameDefines::MS_COUNT] = {false};
		for( uint32 layoutIndex = 0; layoutIndex < numLayouts; ++layoutIndex )
		{
			// Read in layout ID
			uint32 layoutID = 0;
			serializer.AddData( layoutID );

			// Ensure a valid ID
			if( layoutID >= GameDefines::MS_COUNT )
				break;

			// Read the layout size
			uint32 layoutDataSize = 0;
			serializer.AddData( layoutDataSize );

			// Create the layout
			GameGUILayout* pNewLayout = GameGUILayout::CreateGameLayout( (GameDefines::EMenuScreens)layoutID, true );

			// If the layouts are being read and created directly
#ifdef FORCE_CREATE_LAYOUT
			if( pNewLayout )
			{
				// Transfer the layout data
				pNewLayout->TransferLayout( serializer );

				// Hook up the controls by name to their code object
				pNewLayout->HookupCtrls();

				// Store the layout
				GUIMgr::Get().AddLayout( pNewLayout );
			}
			else
				serializer.Skip( layoutDataSize );

// Else the layout creation is being suppressed until needed
#else
			// Create a buffer for the data
			uint8* pBufferData = new uint8[ layoutDataSize ];

			// Store the file data
			serializer.AddRawData( pBufferData, layoutDataSize );

			// Create a compressed layout object
			GUIMgr::CompressedLayout compLayout;
			compLayout.dataSize = layoutDataSize;
			compLayout.pLayout = pNewLayout;
			compLayout.pLayoutData = pBufferData;

			// Store the layout
			GUIMgr::Get().AddCompressedLayout( compLayout );
#endif // FORCE_CREATE_LAYOUT

			// Store the layout ID as valid
			loadedLayoutIDs[ layoutID ] = true;
		}

		// Go through all of the expected GUI layouts and create objects for those that are missing
		// them
		for( uint32 layoutIndex = 0; layoutIndex < GameDefines::MS_COUNT; ++layoutIndex )
		{
			// If this layout was loaded then do nothing
			if( loadedLayoutIDs[layoutIndex] )
				continue;

			// Add the layout to fill the hole
			GUIMgr::Get().AddLayout( CreateGameLayout( (GameDefines::EMenuScreens)layoutIndex, true ) );
			//GUIMgr::Get().AddLayout( CreateGameLayout( (GameDefines::EMenuScreens)layoutIndex ) );
		}
	}
	// Else the layouts are being stored
	else
	{
#ifdef ENABLE_GUI_EDITOR
		GUIMgr::Get().SaveLayouts( serializer );
#endif
	}

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameGUILayout::CreateGameLayout  Static Public
///
///	\param serializer The serializer object to load and save data
/// \returns True if the GUI resource data was successfully loaded/saved, otherwise false
///
///	Load or save the GUI data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameGUILayout* GameGUILayout::CreateGameLayout( GameDefines::EMenuScreens layoutID, bool onlyLoadKnown )
{
	switch( layoutID )
	{
	case GameDefines::MS_TitleScreen:
		return new GUILayoutTitleScreen();

	case GameDefines::MS_ModeSelect:
		return new GUILayoutPlayMode();

	case GameDefines::MS_ModeOptions:
		return new GUILayoutModeOptions();

	case GameDefines::MS_InGame1Player:
		return new GUILayoutInGame();

	case GameDefines::MS_ProfileSelection:
		return new GUILayoutProfileSelection();

	case GameDefines::MS_ViewStats:
		return new GUILayoutViewStats();

	case GameDefines::MS_LearningAssistance:
		return new GUILayoutLearningAssistance();

	//case GameDefines::MS_MultiplayerType:
	//	return new GUILayoutMultiplayType();

	//case GameDefines::MS_LANLobby:
	//	return new GUILayoutLANLobby();
	
	//case GameDefines::MS_PreNetGame:
	//	return new GUILayoutPreNetGame();

	case GameDefines::MS_HelpMeLearn:
		return new GUILayoutHelpMeLearn();

	case GameDefines::MS_PracticeSetup:
		return new GUILayoutPracticeSetup();

	case GameDefines::MS_Tutorials:
		return new GUILayoutTutorials();

	case GameDefines::MS_GameResults:
		return new GUILayoutGameResults();

	case GameDefines::MS_DemoExit:
        return new GUILayoutDemoExit();
            
    default:
        // Unknown layout and stops warnings on Mac
        break;
	}

	// By default use a normal game layout
	if( onlyLoadKnown )
		return NULL;

	return new GameGUILayout();
}