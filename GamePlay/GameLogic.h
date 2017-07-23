//=================================================================================================
/*!
	\file GameLogic.h
	Game Play Library
	Game Logic Header
	\author Taylor Clark
	\date June 22, 2006

	This header contains the definition for the game logic manager.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogic_h
#define __GameLogic_h

#include "Base/Types.h"
#include "GameDefines.h"

class GameFieldBlock;
class GameField;
class PlayerProfile;
class GUILayoutInGame;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogic
	\brief Represents the game logic manager

	This class defines the game manager class that is used to handle gameplay update.
*/
//-------------------------------------------------------------------------------------------------
class GameLogic
{
public:

	/// The game type being played
	GameDefines::EGameType m_GameType;

	enum ELogicType
	{
		LT_Product,
		LT_Addition,
		LT_Fraction,
		LT_COUNT
	};

	/// The enum of actions to take when the user selects a block
	enum EBlockSelAction
	{
		BSA_None = 0,
		BSA_RemoveSelected, // Remove the last selected block
		BSA_RemoveEntireSelection, // Remove all selected blocks
		BSA_DeselectSelection, // Deselect all selected blocks
		BSA_DeselectLast
	};

protected:

	/// Initialize a derived class's data
	virtual bool SubclassInit() { return true; }

	/// Allow a subclass to update data if needed
	virtual void SubclassUpdate( float32 ) {}

	/// Is the game over
	virtual bool IsGameOver() = 0;

	/// The in-game GUI layout
	GUILayoutInGame* m_pGameGUILayout;

	/// The one second counter used to update the time-played text
	float m_SecondCount;

	/// The time limit
	uint32 m_TimeLimit;

	static float32 GetPushUpSpeed( int32 level );

public:

	/// The default constructor
	GameLogic() : m_GameType( GameDefines::GT_Error ),
					m_pGameGUILayout( 0 ),
					m_SecondCount( 0.0f ),
					m_TimeLimit( 0 )
	{
	}

	/// The destructor
	virtual ~GameLogic(){}

	/// Initialize the logic
	bool Init( GameDefines::EGameType gameType, uint32 timeLimit = 0 );

	/// Close the logic
	virtual void Term() {}

	/// Update the game logic
	virtual void Update( float32 frameTime );

	/// A message handler for when a block is selected
	virtual EBlockSelAction OnBlockSelect( GameFieldBlock* pBlock ) = 0;

	/// A message handler for when a block is deselected
	virtual EBlockSelAction OnBlockDeselect( GameFieldBlock* ) { return GameLogic::BSA_DeselectLast; }

	/// Get the game type
	GameDefines::EGameType GetGameType() const { return m_GameType; }

	/// Get the game field
	virtual GameField* GetGameField() = 0;

	/// Get the logic type
	virtual ELogicType GetLogicType() const = 0;
};

#endif // __GameLogic_h