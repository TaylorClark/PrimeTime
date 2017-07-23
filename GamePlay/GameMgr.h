//=================================================================================================
/*!
	\file GameMgr.h
	Game Play Library
	Game Manager Header
	\author Taylor Clark
	\date May 29, 2006

	This header contains the definition for the game manager.
*/
//=================================================================================================

#pragma once
#ifndef __GameMgr_h
#define __GameMgr_h

#include "Base/Types.h"
#include "Math/Point2i.h"
#include "GUI/IGUIMsgHandler.h"
#include "GameDefines.h"
#include "Audio/AudioMgr.h"
#include "GUI/MsgBox.h"

class GameField;
class GameLogic;
class PlayerProfile;

typedef void (*MsgBoxCallback)( MsgBox::EMsgBoxReturn retVal, void* pUserData );


//-------------------------------------------------------------------------------------------------
/*!
	\class GameMgr
	\brief Represents the game manager

	This class defines the game manager class that is used to handle gameplay update.
*/
//-------------------------------------------------------------------------------------------------
class GameMgr
{
private:
	/// The amount of time played in seconds
	float32 m_TimePlayed;

	/// The game logic manager
	GameLogic* m_pGameLogic;

	/// If a game is currently being played
	bool m_IsInGame;

	/// Set if the game should be ended
	bool m_GameEndFlagged;

	/// Set if the game should be restarted
	bool m_GameRetryFlagged;

	/// If the game is paused
	bool m_IsPaused;

	/// The active player profile
	PlayerProfile* m_pActiveProfile;

	/// Whether or not we are playing a single player game or multiplayer game
	bool m_InSinglePlayerGame;

	/// The current gameplay difficulty level
	GameDefines::EGameplayDiffLevel m_GameDiff;

	/// The time limit
	uint32 m_TimeLimit;

	/// The active message box
	MsgBox* m_pMsgBox;

	/// The callback function invoked when the message box is closed
	MsgBoxCallback m_MsgBoxCallbackFunc;

	/// The context sensitive data to send to the message box callback
	void* m_pMsgBoxUserData;


	/// The default constructor, private since this class is a singleton
	GameMgr() : m_TimePlayed( 0.0f ),
				m_IsInGame( false ),
				m_pGameLogic(0),
				m_pActiveProfile( 0 ),
				m_GameEndFlagged( false ),
				m_GameRetryFlagged( false ),
				m_InSinglePlayerGame( false ),
				m_IsPaused( false ),
				m_GameDiff( GameDefines::GPDL_4 ),
				m_TimeLimit( 0 ),
				m_pMsgBox( 0 ),
				m_MsgBoxCallbackFunc( 0 ),
				m_pMsgBoxUserData( 0 )
	{}

public:

	/// The channel that the background music is playing on
	MusicStreamHndl _bgm;

	/// The overlay background music
	MusicStreamHndl _bgmOverlay;

	/// The accessor for the one and only instance of the class
	static GameMgr& Get()
	{
		static GameMgr s_Mgr;
		return s_Mgr;
	}

	/// The default destructor
	~GameMgr(){ Clear(); }

	/// Initialize a new game
	void StartGame( GameDefines::EGameType gameType, GameDefines::ProductSubsetFilter filter = GameDefines::ProductSubsetFilter() );
	
	/// End the current game
	void EndGame(){ m_GameEndFlagged = true; }

	/// Clear the data used by the game manager
	void Clear();

	/// Update the game
	virtual void Update( float32 frameTime );

	/// Pause or unpause the game
	void SetPauseState( bool setPause ){ m_IsPaused = setPause; }

	/// Get the paused state
	bool IsPaused() const { return m_IsPaused; }

	/// Get if we are currently playing a game
	bool IsPlaying() const { return m_IsInGame; }

	/// Determine if we are currently playing a single or multiplayer game
	bool IsSinglePlayerGame() const { return m_InSinglePlayerGame; }

	/// Get the logic component of the game
	GameLogic* GetLogic() { return m_pGameLogic; }

	/// Get the active profile
	PlayerProfile* GetActiveProfile() { return m_pActiveProfile; }

	/// Set the active profile
	void SetActiveProfile( PlayerProfile* pNewProfile );

	/// Get the elapsed time played
	float32 GetElapsedTime() const { return m_TimePlayed; }

	/// Get the gameplay difficulty
	GameDefines::EGameplayDiffLevel GetGameplayDiff() const { return m_GameDiff; }

	/// Set the gameplay difficulty
	void SetGameplayDiff( GameDefines::EGameplayDiffLevel diff ) { m_GameDiff = diff; }

	/// Set the timelimit
	void SetTimeLimit( uint32 timeLimitInSecs ){ m_TimeLimit = timeLimitInSecs; }

	/// Set the timelimit
	uint32 GetTimeLimit() const { return m_TimeLimit; }

	/// Get the gameplay type
	GameDefines::EGameType GetGameType() const;

	/// Display an in-game message box
	void PopupMsgBox( MsgBox* pMsgBox, MsgBoxCallback pRetFunc, void* pUserData );

	/// Determine if the current game is the Prime Time tutorial
	bool IsPTTutorial() const
	{
		bool isPTTutorial = (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime;
		return isPTTutorial && (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial;
	}

	/// Determine if the current game is the addition attack tutorial
	bool IsAdditionTutorial() const
	{
		bool isPTTutorial = (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Type) == GameDefines::GT_AdditionAttack;
		return isPTTutorial && (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial;
	}

	/// Restart the game
	void RestartGame(){ m_GameRetryFlagged = true; }
};

#endif // __GameMgr_h