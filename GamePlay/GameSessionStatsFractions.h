//=================================================================================================
/*!
	\file GameSessionStatsFractions.h
	Game Play Library
	Fraction Game Session Object Header
	\author Taylor Clark
	\date August 24, 2007

	This header contains the definition for the fraction game session class.
*/
//=================================================================================================

#pragma once
#ifndef __GameSessionStatsFractions_h
#define __GameSessionStatsFractions_h

#include "Base/Types.h"
#include "ProfileSubsetInGame.h"
#include <time.h>


//-------------------------------------------------------------------------------------------------
/*!
	\class GameSessionStatsFractions
	\brief Represents the stats from one session of a fraction game mode
*/
//-------------------------------------------------------------------------------------------------
class GameSessionStatsFractions
{
public:
	/// The default constructor to clear the data
	GameSessionStatsFractions(){ Clear(); }

	/// Clear the data
	void Clear()
	{
		m_Duration = 0;
		m_Score = 0;
		m_NumFractionsCleared = 0;
		m_NumFractionsFailed = 0;
		m_Difficulty = GameDefines::GPDL_3;
		memset( &m_SessionStartTime, 0, sizeof(m_SessionStartTime) );
		m_EndReason = ProfileSubsetInGame::GER_COUNT;
	}

	/// Store the current time as the session start time
	void StoreStartTime()
	{
		time_t curTime;
		time( &curTime );
#ifdef WIN32
		localtime_s( &m_SessionStartTime, &curTime );
#else
		m_SessionStartTime = *localtime( &curTime );
#endif
	}


	/// The length of the session in seconds
	float32 m_Duration;

	/// The score achieved
	uint32 m_Score;

	/// The number of fractions cleared this session
	uint32 m_NumFractionsCleared;

	/// The number of erroneous fractions this session
	uint32 m_NumFractionsFailed;

	/// The gameplay difficulty
	GameDefines::EGameplayDiffLevel m_Difficulty;

	/// The game end reason
	ProfileSubsetInGame::EGameEndReason m_EndReason;

	/// The date and time when the session was started
	struct tm m_SessionStartTime;
};

#endif // __GameSessionStatsFractions_h