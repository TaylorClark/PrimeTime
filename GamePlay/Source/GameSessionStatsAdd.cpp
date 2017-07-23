//=================================================================================================
/*!
	\file GameSessionStatsAdd.cpp
	Game Play Library
	Addition Game Session Object Source
	\author Taylor Clark
	\date August 24, 2007

	This source file contains the implementation for the addition game session statistics object.
*/
//=================================================================================================

#include "../GameSessionStatsAdd.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsAdd::Serialize_V1()  Private
///
///	\param serializer The serializer that reads and writes data
///	\returns True if the data is read successfully, otherwise false
///
///	Save/load a session data structure using the version 1 format
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameSessionStatsAdd::Serialize_V1( Serializer& serializer )
{
	serializer.AddData( m_Duration );
	serializer.AddData( m_Score );	
	serializer.AddData( m_NumEqsCleared );
	serializer.AddData( m_NumEqsFailed );
	serializer.AddData( m_SummandsCleared );
	serializer.AddData( m_TimeLimit );
	
	uint32 tempDiff = static_cast<uint32>( m_Difficulty );
	serializer.AddData( tempDiff );
	m_Difficulty = static_cast<GameDefines::EGameplayDiffLevel>( tempDiff );

	uint8 startTimeBytes[ sizeof(m_SessionStartTime) ];
	serializer.AddRawData( startTimeBytes, sizeof(m_SessionStartTime) );
	memcpy( &m_SessionStartTime, startTimeBytes, sizeof(m_SessionStartTime));

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsAdd::OnValidEquation  Public
///
///	\param blockList The list of selected blocks, with the sub being the last item
///
/// Called when the user selects a valid equation
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSessionStatsAdd::OnValidEquation( const std::list<GameFieldBlock*>& blockList )
{
	if( blockList.size() == 0 )
		return;

	// Store a victory
	++m_NumEqsCleared;
	m_SummandsCleared += (uint32)blockList.size() - 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsAdd::OnErroneousEquation  Public
///
///	\param blockList The list of selected blocks, with the sub being the last item
///
/// Called when the user selects an equation with an error
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSessionStatsAdd::OnErroneousEquation( const std::list<GameFieldBlock*>& )
{
	// Store a failure
	++m_NumEqsFailed;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsAdd::StoreStartTime  Public
///
/// Store the current time as the session start time
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSessionStatsAdd::StoreStartTime()
{
	time_t curTime;
	time( &curTime );
#ifdef WIN32
	localtime_s( &m_SessionStartTime, &curTime );
#else
	m_SessionStartTime = *localtime( &curTime );
#endif
}