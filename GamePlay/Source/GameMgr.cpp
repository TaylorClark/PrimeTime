/*=================================================================================================

	\file GameMgr.cpp
	Game Play Library
	Game Manager Source
	\author Taylor Clark
	\Date May 12, 2006

	This source file contains the implementation of the game manager class.

=================================================================================================*/

#include "../GameMgr.h"
#include "../GameFieldMultBase.h"
#include "../GameFieldAdd.h"
#include "../GameFieldBlock.h"
#include "../GameLogicProduct.h"
#include "../GameLogicFractions.h"
#include "../GameLogicAdd.h"
#include "Resource/ResourceMgr.h"
#include <stdlib.h>
#include "GUI/GUIDefines.h"
#include "GUI/GUIMessages.h"
#include "Base/TCAssert.h"
#include "GUI/GUIMgr.h"
#include "../PlayerProfile.h"
#include "../ProfileSubsetMultiplication.h"
#include "Graphics2D/GraphicsMgr.h"
#include "../GameLogicTutorPT.h"
#include "../GameLogicTutorPTCombo.h"
#include "../GameLogicTutorCeiling.h"
#include "../GameLogicTutorAdd.h"
#include "../GameLogicTutorFractions.h"
#include "../ProfileSubsetAddition.h"
#include "Base/FileFuncs.h"
#include "../../PrimeTime/ResourceIDs.h"
#include "PrimeTime/ApplicationBase.h"

extern const ResourceID g_MenuMusicResID;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameMgr::StartGame  Public
///
///	Start a new game.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameMgr::StartGame( GameDefines::EGameType gameType, GameDefines::ProductSubsetFilter filter )
{
	m_IsInGame = true;
	m_IsPaused = false;
	m_TimePlayed = 0;
	
	// Clear data if we currently have any
	Clear();

	// Load the music
	_bgm = ResourceMgr::Get().GetMusicStream( RESID_MUSIC_DISCO_MUSIC );
	_bgmOverlay = ResourceMgr::Get().GetMusicStream( RESID_MUSIC_DISCO_OVERLAY_MUSIC );

	// Stop the menu music
	AudioMgr::Get().SetBGMusic( NULL );
	
	// Play the game music
	if( _bgm.GetObj() )
	{
		_bgm.GetObj()->Play();
		_bgm.GetObj()->SetVolume( 0.5f );
	}
	if( _bgmOverlay.GetObj() )
	{
		_bgmOverlay.GetObj()->Play();
		_bgmOverlay.GetObj()->SetVolume( 0.0f );
	}

	std::wstring sGameMsg = L"sng,";
	sGameMsg += TCBase::EasyUIToAHex( (int32)gameType );
	sGameMsg += L",";
	sGameMsg += TCBase::EasyIToA( (int32)m_GameDiff );;
	ApplicationBase::Get()->SendAppConnectMessage( sGameMsg.c_str() );

	switch( gameType & GameDefines::GT_Mask_Type )
	{
	case GameDefines::GT_PrimeTime:
	case GameDefines::GT_Ceiling:
		{
			// Create the logic object
			GameLogicProduct* pProdLogic = 0;
			if( (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial )
			{
				if( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime )
					pProdLogic = new GameLogicTutorPT();
				else if( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_Ceiling )
					pProdLogic = new GameLogicTutorCeiling();
			}
			else if( (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial2 )
				pProdLogic = new GameLogicTutorPTCombo();
			else
				pProdLogic = new GameLogicProduct();

			m_pGameLogic = pProdLogic;

			// If this game type is a practice mode
			if( (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Practice )
				pProdLogic->InitPractice( filter );
			
			// Set the logic to work with this field
			m_pGameLogic->Init( gameType, m_TimeLimit );
		}
		break;

	case GameDefines::GT_Fractions:
		{
			// Create the logic object
			if( (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial )
				m_pGameLogic = new GameLogicTutorFractions();
			else
				m_pGameLogic = new GameLogicFractions();			

			m_pGameLogic->Init( gameType, m_TimeLimit );
		}
		break;

	case GameDefines::GT_AdditionAttack:
		{
			// Create the logic object
			if( (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial )
				m_pGameLogic = new GameLogicTutorAdd();
			else
				m_pGameLogic = new GameLogicAdd();

			// Set the logic to work with this field
			m_pGameLogic->Init( gameType, m_TimeLimit );
		}
		break;

	default:
		m_IsInGame = false;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameMgr::Clear  Public
///
///	Clear the game data and free any memory used.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameMgr::Clear()
{
	// Clear the message box
	PopupMsgBox( NULL, NULL, NULL );

	// If we have logic then free it
	if( m_pGameLogic )
	{
		delete m_pGameLogic;
		m_pGameLogic = NULL;
	}

	// Clear the background music
	if( _bgm.GetObj() )
		_bgm.GetObj()->Stop();
	_bgm.ReleaseHandle();

	if( _bgmOverlay.GetObj() )
		_bgmOverlay.GetObj()->Stop();
	_bgmOverlay.ReleaseHandle();

	// Clear the times
	m_TimePlayed = 0.0f;
	m_GameEndFlagged = false;
	m_GameRetryFlagged = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameMgr::Update  Public
///
///	\param frameTime The elapsed frame time in seconds
///
///	Update the game.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameMgr::Update( float frameTime )
{
	// If the game is to be ended
	if( m_GameEndFlagged )
	{
		// The game is stopped
		m_IsInGame = false;

		// Tell the logic to clean up
		if( m_pGameLogic )
			m_pGameLogic->Term();

		// If this game was not a tutorial then display the results
		bool isTutorial = (GetGameType() & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial;
		isTutorial |= (GetGameType() & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial2;
		if( !isTutorial )
		{
			// Go back to the game results screen
			GUILayoutGameResults* pLayout = (GUILayoutGameResults*)GUIMgr::Get().GetLayoutByID( (uint32)GameDefines::MS_GameResults );
			bool goToResults = true;
			if( pLayout )
			{
				switch( GetGameType() & GameDefines::GT_Mask_Type )
				{
					case GameDefines::GT_PrimeTime:
					case GameDefines::GT_Ceiling:
						{
							// Display the game results
							GameLogicProduct* pCurLogic = (GameLogicProduct*)m_pGameLogic;
							pLayout->DisplayResults( pCurLogic->GetSessionStats() );
						}
						break;

					case GameDefines::GT_Fractions:
						{
							// Display the game results
							GameLogicFractions* pCurLogic = (GameLogicFractions*)m_pGameLogic;
							pLayout->DisplayResults( pCurLogic->GetSessionStats() );
						}
						break;

					case GameDefines::GT_AdditionAttack:
						{
							// Display the game results
							GameLogicAdd* pCurLogic = (GameLogicAdd*)m_pGameLogic;
							pLayout->DisplayResults( pCurLogic->GetSessionStats() );
						}
						break;

					default:
						goToResults = false;
						break;
				}
			}

			// If we should show results
			if( goToResults && pLayout )
				GUIMgr::Get().SetActiveLayout( GameDefines::MS_GameResults );
			// Otherwise go back to the main menu
			else
				GUIMgr::Get().SetActiveLayout( GameDefines::MS_TitleScreen );
		}
		// Else go back to the main menu
		else
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_TitleScreen );
		
		// Stop the music
		if( _bgm.GetObj() )
			AudioMgr::Get().FadeOutMusic( _bgm.GetObj(), 0.75f );
		_bgm.ReleaseHandle();

		if( _bgmOverlay.GetObj() )
			AudioMgr::Get().FadeOutMusic( _bgmOverlay.GetObj(), 0.75f );
		_bgmOverlay.ReleaseHandle();
		
		// Go back to menu music
		AudioMgr::Get().SetBGMusic( ResourceMgr::Get().GetMusicStream( RESID_MUSIC_NEWS_MUSIC ).GetObj() );

		// Free the data
		Clear();
		return;
	}

	// If a retry is flagged
	if( m_GameRetryFlagged )
	{
		m_GameRetryFlagged = false;
		m_TimePlayed = 0.0f;
	
		SetPauseState( false );
		if( m_pGameLogic )
			m_pGameLogic->Init( m_pGameLogic->GetGameType(), m_TimeLimit );
	}

	// If there is a message box
	if( m_pMsgBox )
	{
		// If the message box is completed
		if( m_pMsgBox->GetRetVal() != MsgBox::MBR_Error )
		{
			// Invoke the callback if there is one
			if( m_MsgBoxCallbackFunc )
				m_MsgBoxCallbackFunc( m_pMsgBox->GetRetVal(), m_pMsgBoxUserData );

			// Free the data
			GUIMgr::Get().CloseMessageBox( m_pMsgBox );
			delete m_pMsgBox;
			m_pMsgBox = 0;
			m_MsgBoxCallbackFunc = 0;
			m_pMsgBoxUserData = 0;

			// Unpause the game
			GameMgr::Get().SetPauseState( false );
		}

		return;
	}

	// If the game is not paused then update the time
	if( !m_IsPaused )
		m_TimePlayed += frameTime;
		
	// If there is game logic then update it
	if( m_pGameLogic )
		m_pGameLogic->Update( frameTime );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameMgr::GetGameType  Public
///
///	Get the gameplay type.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameDefines::EGameType GameMgr::GetGameType() const
{
	if( m_pGameLogic )
		return m_pGameLogic->GetGameType();
	return GameDefines::GT_Error;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameMgr::PopupMsgBox  Public
///
///	\param pMsgBox The message box to display
///	\param pRetFunc The function to call when the message box is closed
///	\param pUserData The user data to send to the callback
///
/// Display an in-game message box that is managed by this class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameMgr::PopupMsgBox( MsgBox* pMsgBox, MsgBoxCallback pRetFunc, void* pUserData )
{
	// If their is an active message box
	if( m_pMsgBox != 0 )
	{
		if( pMsgBox )
			MSG_LOGGER_OUT( MsgLogger::MI_Warning, L"Closing message because a new one is to be opened." );

		// Close the active message box
		GUIMgr::Get().CloseMessageBox( m_pMsgBox );
		delete m_pMsgBox;
		m_pMsgBox = 0;
		
		// Unpause the game
		GameMgr::Get().SetPauseState( false );

		// If there is a callback then send an error value
		if( m_MsgBoxCallbackFunc )
			m_MsgBoxCallbackFunc( MsgBox::MBR_Error, m_pMsgBoxUserData );
	}

	// Store the variables
	m_pMsgBox = pMsgBox;
	m_MsgBoxCallbackFunc = pRetFunc;
	m_pMsgBoxUserData = pUserData;

	// Display the message box if there is one
	if( m_pMsgBox )
	{
		GUIMgr::Get().PopupMessageBox( m_pMsgBox );

		// Pause the game
		GameMgr::Get().SetPauseState( true );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameMgr::SetActiveProfile  Public
///
///	\param pNewProfile The new profile to use.
///
///	Set the active profile and free, if needed, the current profile.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameMgr::SetActiveProfile( PlayerProfile* pNewProfile )
{
	if( m_pActiveProfile && pNewProfile != m_pActiveProfile )
	{
		m_pActiveProfile->SaveToFile();
		delete m_pActiveProfile;
		m_pActiveProfile = 0;
	}
	m_pActiveProfile = pNewProfile;
}