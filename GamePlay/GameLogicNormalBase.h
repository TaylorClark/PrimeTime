//=================================================================================================
/*!
	\file GameLogicNormalBase.h
	Game Play Library
	Normal Game Logic Base Class Header
	\author Taylor Clark
	\date December 29, 2009

	This header contains the definition for the base class to game logics that involve pushing rows
	up.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicNormalBase_h
#define __GameLogicNormalBase_h

#include "GameLogic.h"
#include "Audio/SoundMusic.h"

class GameField;
class GameFieldBlockMultBase;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicNormalBase
	\brief Represents the logic for multiplication game modes

	This class defines the game logic that is used for multiplication modes.
*/
//-------------------------------------------------------------------------------------------------
class GameLogicNormalBase : public GameLogic
{
protected:
	
	/// The level currently reached by clearing blocks
	int32 m_CurrentLevel;

	/// If the block generation is currently accelerated by the player
	bool m_IsPushAccelerated;
	
	/// The raise sound channel, -1 if it is not playing
	SoundInstance _playingRaiseSound;

	/// Allow a subclass to update data if needed
	virtual void SubclassUpdate( float32 frameTime );

public:

	/// The default constructor
	GameLogicNormalBase() : m_IsPushAccelerated( false ),
							m_CurrentLevel( 1 )
	{
	}

	/// A message handler called when the users presses the keyboard key to speed up block
	/// production
	virtual void OnSpeedButtonDown();

	/// A message handler called when the users release the keyboard key that speeds up block
	/// production
	virtual void OnSpeedButtonUp();

	/// Generate blocks up to a certain width
	virtual GameFieldBlockMultBase** GenerateBlocksToFieldWidth() = 0;

	/// Is the game over
	virtual bool IsGameOver() = 0;

	/// Get the game field
	virtual GameField* GetGameField() = 0;

	/// Get the logic type
	virtual ELogicType GetLogicType() const = 0;
};

#endif // __GameLogicNormalBase_h