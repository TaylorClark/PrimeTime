//=================================================================================================
/*!
	\file GameSessionStatsMult.h
	Game Play Library
	Multiplication Game Session Object Header
	\author Taylor Clark
	\date August 24, 2007

	This header contains the definition for the multiplication game session class.
*/
//=================================================================================================

#pragma once
#ifndef __GameSessionStatsMult_h
#define __GameSessionStatsMult_h

#include "Base/Types.h"
#include "ProfileSubsetInGame.h"
#include <time.h>


//-------------------------------------------------------------------------------------------------
/*!
	\class GameSessionStatsMult
	\brief Represents the stats from one session of a multiplication game mode
*/
//-------------------------------------------------------------------------------------------------
class GameSessionStatsMult
{
public:

	/// The default constructor to clear the members
	GameSessionStatsMult()
	{
		Clear();
	}

	/// Clear the session data
	virtual void Clear()
	{
		m_Duration = 0;
		m_Score = 0;
		m_NumEqsCleared = 0;
		m_NumEqsFailed = 0;
		m_NumBlocksCleared = 0;
		for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
		{
			m_ValidCounts[primeIndex] = 0;
			m_ErrorCounts[primeIndex] = 0;
		}
		m_Difficulty = GameDefines::GPDL_4;
		m_GameType = GameDefines::GT_PrimeTime;
		m_NumCombos = 0;
		m_MaxCombo = 0;
		m_TimeLimit = 0;
		m_LevelReached = 0;
		m_ContextVal = 0;
		memset( &m_SessionStartTime, 0, sizeof(m_SessionStartTime) );
		m_GameEndReason = ProfileSubsetInGame::GER_COUNT;
	}

	/// Save/load this data structure. When creating a new version make sure to update the two
	/// references in ProfileSubsetMultiplication::Serialize
	bool Serialize_V1( Serializer& serializer, uint32 numPrimes );

	/// Called when the user selects a valid equation
	void OnValidEquation( const std::list<GameFieldBlock*>& blockList );

	/// Called when the user selects an equation with an error
	void OnErroneousEquation( const std::list<GameFieldBlock*>& blockList );

	/// Called when the user goes creates a combo
	void OnCombo( uint32 comboCount );

	/// Store the current time as the session start time
	void StoreStartTime();


	/// The length of the session in seconds
	float32 m_Duration;

	/// The score achieved
	uint32 m_Score;

	/// The number of equations cleared this session
	uint32 m_NumEqsCleared;

	/// The number of erroneous equations this session
	uint32 m_NumEqsFailed;

	/// The number of blocks cleared
	uint32 m_NumBlocksCleared;

	/// The number of occurances a prime factor was used in a valid equation
	uint32 m_ValidCounts[ GameDefines::NUM_PRIMES ];

	/// The number of occurances a prime factor was used in a erroneous equation
	uint32 m_ErrorCounts[ GameDefines::NUM_PRIMES ];

	/// The gameplay difficulty
	GameDefines::EGameplayDiffLevel m_Difficulty;

	/// The game type
	GameDefines::EGameType m_GameType;

	/// The number of combos made
	uint32 m_NumCombos;

	/// The max combo count
	uint32 m_MaxCombo;

	/// The time limit
	uint32 m_TimeLimit;

	/// The level reached this session
	mutable uint32 m_LevelReached;

	/// A value used based on the game type
	uint32 m_ContextVal;

	/// The date and time when the session was started
	struct tm m_SessionStartTime;

	/// The reason the game ended
	ProfileSubsetInGame::EGameEndReason m_GameEndReason;
};

#endif // __GameSessionStats_h