//=================================================================================================
/*!
	\file GameSessionStatsAdd.h
	Game Play Library
	Addition Game Session Object Header
	\author Taylor Clark
	\date August 24, 2007

	This header contains the definition for the addition game session class.
*/
//=================================================================================================

#pragma once
#ifndef __GameSessionStatsAdd_h
#define __GameSessionStatsAdd_h

#include "Base/Types.h"
#include "ProfileSubsetInGame.h"
#include <time.h>


//-------------------------------------------------------------------------------------------------
/*!
	\class GameSessionStatsAdd
	\brief Represents the stats from one session of a addition game mode
*/
//-------------------------------------------------------------------------------------------------
class GameSessionStatsAdd
{
public:
	/// The default constructor to clear the data
	GameSessionStatsAdd(){ Clear(); }

	/// Clear the data
	void Clear()
	{
		m_Duration = 0;
		m_Score = 0;
		m_NumEqsCleared = 0;
		m_NumEqsFailed = 0;
		m_SummandsCleared = 0;
		m_Difficulty = GameDefines::GPDL_3;
		memset( &m_SessionStartTime, 0, sizeof(m_SessionStartTime) );
		m_TimeLimit = 0;
		m_EndReason = ProfileSubsetInGame::GER_COUNT;
	}

	/// Save/load this data structure
	bool Serialize_V1( Serializer& serializer );

	/// Called when the user selects a valid equation
	void OnValidEquation( const std::list<GameFieldBlock*>& blockList );

	/// Called when the user selects an equation with an error
	void OnErroneousEquation( const std::list<GameFieldBlock*>& blockList );

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

	/// The number of summands cleared
	uint32 m_SummandsCleared;

	/// The time limit in seconds
	uint32 m_TimeLimit;

	/// The gameplay difficulty
	GameDefines::EGameplayDiffLevel m_Difficulty;

	/// The game end reason
	ProfileSubsetInGame::EGameEndReason m_EndReason;

	/// The date and time when the session was started
	struct tm m_SessionStartTime;
};

#endif // __GameSessionStatsAdd_h