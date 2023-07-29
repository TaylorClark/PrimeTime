/*=================================================================================================

	\file GameLogic.cpp
	Game Play Library
	Product Game Logic Source
	\author Taylor Clark
	\Date June 24, 2006

	This source file contains the implementation of the game logic manager class.

=================================================================================================*/

#include "../GameLogic.h"
#include "../GameMgr.h"
#include "../GameFieldMultBase.h"
#include "../GameDefines.h"
#include "../GUILayout_Game.h"
#include "GUI/GUIMgr.h"
#include "GUI/MsgBox.h"
#include "Base/NumFuncs.h"
#include "PrimeTime/ApplicationBase.h"

const float32 BASE_SPEED = (float32)GameDefines::BLOCK_HEIGHT + 0.1f;


void OnGameOverMBClose( MsgBox::EMsgBoxReturn retVal, void* )
{
	// If the player wants to quit
	if( retVal == MsgBox::MBR_Option_1 )
		GameMgr::Get().RestartGame();
	else if( retVal == MsgBox::MBR_Option_2 )
		GameMgr::Get().EndGame();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogic::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the game.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogic::Update( float32 frameTime )
{
	bool gameIsOver = false;

	// Update the time counter
	if( m_pGameGUILayout && !GameMgr::Get().IsPaused() )
	{
		// Update the one second timer
		m_SecondCount += frameTime;

		// If the timer has gone passed 1 second then update
		if( m_SecondCount >= 1.0f )
		{
			uint32 gameTime = (uint32)GameMgr::Get().GetElapsedTime();

			// If there is a time limit
			if( m_TimeLimit > 0 )
			{
				// If the game should end due to the time limit
				if( gameTime >= m_TimeLimit )
				{
					gameIsOver = true;
					m_pGameGUILayout->SetTimePlayed( 0 );
				}
				else
					m_pGameGUILayout->SetTimePlayed( m_TimeLimit - gameTime );
			}
			// Else just display the elapsed time
			else
				m_pGameGUILayout->SetTimePlayed( gameTime );

			// Clear the mini-timer
			m_SecondCount = 0.0f;
		}
	}

	// If the game is being played and the time isn't over
	if( !GameMgr::Get().IsPaused() && !gameIsOver )
	{
		// If the game has ended
		if( IsGameOver() )
			gameIsOver = true;
	}

	// If the game is over
	if( gameIsOver )
	{
		// Display the end message box
		GameMgr::Get().SetPauseState( true );

		uint32 elapsedTime = (uint32)GameMgr::Get().GetElapsedTime();
		std::wstring endGameMessage = L"eg,";
		endGameMessage += TCBase::EasyUIToA( elapsedTime );
		ApplicationBase::SendAppConnectMessage( endGameMessage.c_str() );

		MsgBox* pGameOverMsgBox;

		//TODO localize
#ifdef EVALUATION
		if( elapsedTime >= m_TimeLimit )
			pGameOverMsgBox = MsgBox::CreateMsgBoxCustom( L"Time's up! Evaluation mode games are limited to 3 minutes. Don't worry, the full version has no time limit!", L"Retry", L"Quit" );
		else
			pGameOverMsgBox = MsgBox::CreateMsgBoxCustom( L"Great Job!", L"Retry", L"Quit" );
#else
		pGameOverMsgBox = MsgBox::CreateMsgBoxCustom( L"Great Job!", L"Retry", L"Quit" );
#endif
		
		GameMgr::Get().PopupMsgBox( pGameOverMsgBox, OnGameOverMBClose, NULL );
	}

	// Allow any derived classes to update if needed
	SubclassUpdate( frameTime );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogic::GetPushUpSpeed  Static Protected
///	\param level The current game level
///	\returns The block speed for the passed-in level
///
///	Get the block push speed based on game level
///////////////////////////////////////////////////////////////////////////////////////////////////
float32 GameLogic::GetPushUpSpeed( int32 level )
{
	// If we are in fraction mode then we start slower, but increase faster
	bool inFractionMode = (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Type) == GameDefines::GT_Fractions;
	//if( inFractionMode )
	//	level *= 2;

	float32 retSpeed = BASE_SPEED * ((float32)level / 20.0f);

	// Slow down fraction mode
	if( inFractionMode )
		retSpeed *= 0.8125f;

	return retSpeed;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogic::Init  Public
///
///	\param gameType The game type being played
///	\param timeLimit The time limit for the games in seconds, 0 means no time limit
///
///	Initialize the game logic.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameLogic::Init( GameDefines::EGameType gameType, uint32 timeLimit )
{
	// Store the game type
	m_GameType = gameType;

	// Store the time limit
	m_TimeLimit = timeLimit;

#ifdef EVALUATION
	// Limit games to 3 minutes in evaluation mode
	if( (gameType & GameDefines::GT_Mask_Mod) != GameDefines::GT_Mod_Tutorial )
		m_TimeLimit = 2 * 60;
#endif

	// Store the in-game GUI layout
	m_pGameGUILayout = reinterpret_cast<GUILayoutInGame*>( GUIMgr::Get().GetLayoutByID( GameDefines::MS_InGame1Player ) );

	// Initialize the subclass data
	return SubclassInit();
}
