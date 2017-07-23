//=================================================================================================
/*!
	\file GameLogicAdd.h
	Game Play Library
	Addition Mode Game Logic Header
	\author Taylor Clark
	\date September 10, 2006

	This header contains the definition for the game logic manager.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicAdd_h
#define __GameLogicAdd_h

#include "GameLogic.h"
#include "GameDefines.h"
#include "GameSessionStatsAdd.h"
#include <vector>
#include <list>

class GameFieldBlockAdd;
class GameFieldAdd;
class GUILayoutInGame;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicAdd
	\brief Represents the addition game logic

	This class defines the game manager class that is used to handle addition gameplay updating.
*/
//-------------------------------------------------------------------------------------------------
class GameLogicAdd : public GameLogic
{
protected:

	/// A structure for a usable summand
	struct Summand
	{
		Summand( int32 val = 0 ) : value( val ),
								fieldCount( 0 )
		{}

		/// The value of the summand
		int32 value;

		/// The number of this summand on the field
		int32 fieldCount;
	};

	/// The addition game field
	GameFieldAdd* m_pGameFieldAdd;
	
	/// The profile data to update while playing
	//ProfileSubsetMultiplication* m_pMultiplicationProfile;

	/// The list of indices into the summand array to generate
	std::list< int32 > m_UpcomingSummandIndices;

	/// The array of summands to choose from
	std::vector< Summand > m_Summands;

	/// The minimum sum, inclusive, generated
	int32 m_MinSum;

	/// The maximum sum, exclusive, generated
	int32 m_MaxSum;

	/// The number of sums cleared
	uint32 m_NumSumsCleared;

	/// The score
	uint32 m_Score;

	/// The current session's statistics
	GameSessionStatsAdd m_CurSessionStats;

	bool _isInPractice10Mode;


	/// Initialize the summands to generate
	void InitNumbers();

	/// Generate a sum
	virtual int32 GenerateSum();

	/// Initialize a derived class's data
	virtual bool SubclassInit();

	/// Is the game over
	virtual bool IsGameOver() { return false; }

	/// Allow a subclass to update data if needed
	virtual void SubclassUpdate( float32 frameTime );

	/// A message handler called when the game field is in a finished state
	virtual void OnGameFieldEnd() {}

	/// Generate blocks
	virtual void GenerateLackingBlocks();

	int32 SummandValueToIndex( int32 value );

	int32 GetNextSummandIndex();

public:

	/// The default constructor
	GameLogicAdd() : m_pGameFieldAdd( 0 ),
					m_MinSum( 12 ),
					m_MaxSum( 30 ),
					m_NumSumsCleared( 0 ),
					m_Score( 0 ),
					_isInPractice10Mode( false )
	{
	}

	/// The destructor
	virtual ~GameLogicAdd();

	/// Close the logic
	virtual void Term();

	/// A message handler for when a block is selected that contains the mode-specific logic
	virtual EBlockSelAction OnBlockSelect( GameFieldBlock* pBlock );

	/// Get the game field
	virtual GameField* GetGameField(){ return (GameField*)m_pGameFieldAdd; }

	/// Get the logic type
	virtual ELogicType GetLogicType() const { return LT_Addition; }

	/// Get the session stats
	const GameSessionStatsAdd& GetSessionStats() const { return m_CurSessionStats; }
};

#endif // __GameLogicAdd_h