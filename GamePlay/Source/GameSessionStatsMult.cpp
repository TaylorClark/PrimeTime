//=================================================================================================
/*!
	\file GameSessionStatsMult.cpp
	Game Play Library
	Multiplication Game Session Object Source
	\author Taylor Clark
	\date August 24, 2007

	This source file contains the implementation for the multiplication game session statistics
	object.
*/
//=================================================================================================

#include "../GameSessionStatsMult.h"
#include "Base/MsgLogger.h"
#include "../GameFieldBlockProduct.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsMult::Serialize_V1()  Private
///
///	\param serializer The serializer that reads and writes data
///	\returns True if the data is read successfully, otherwise false
///
///	Save/load a session data structure using the version 1 format
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameSessionStatsMult::Serialize_V1( Serializer& serializer, uint32 numPrimes )
{
	// The duration
	serializer.AddData( m_Duration );

	// The score
	serializer.AddData( m_Score );
	
	// The number equations cleared
	serializer.AddData( m_NumEqsCleared );
	serializer.AddData( m_NumEqsFailed );
	serializer.AddData( m_NumBlocksCleared );

	// The primes
	for( uint32 primeIndex = 0; primeIndex < numPrimes; ++primeIndex )
	{
		// If there are more primes in the profile than the game knows to recognize then bail
		// because this could lead to loss of data in the profile
		if( primeIndex >= GameDefines::NUM_PRIMES )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Warning, L"Unexpected number of primes in multiplication game session" );
			return false;
		}

		// The counts
		serializer.AddData( m_ValidCounts[primeIndex] );
		serializer.AddData( m_ErrorCounts[primeIndex] );
	}

	// If we know of more primes than this profile contains then clear the data
	if( numPrimes < GameDefines::NUM_PRIMES )
	{
		for( uint32 primeIndex = numPrimes; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
		{
			m_ValidCounts[primeIndex] = 0;
			m_ErrorCounts[primeIndex] = 0;
		}
	}

	// The rest of the session data
	uint16 tempDiff = static_cast<uint16>( m_Difficulty );
	serializer.AddData( tempDiff );
	m_Difficulty = static_cast<GameDefines::EGameplayDiffLevel>( tempDiff );

	uint16 tempGameType = static_cast<uint16>( m_GameType );
	serializer.AddData( tempGameType );
	m_GameType = static_cast<GameDefines::EGameType>( tempGameType );

	serializer.AddData( m_NumCombos );
	serializer.AddData( m_MaxCombo );
	serializer.AddData( m_TimeLimit );
	serializer.AddData( m_LevelReached );
	serializer.AddData( m_ContextVal );

	uint8 startTimeBytes[ sizeof(m_SessionStartTime) ];
	serializer.AddRawData( startTimeBytes, sizeof(m_SessionStartTime) );
	memcpy( &m_SessionStartTime, startTimeBytes, sizeof(m_SessionStartTime));

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsMult::OnValidEquation()  Public
///
///	\param blockList The list of blocks in the equation
///
///	A message handler called when the user selects a valid equation.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSessionStatsMult::OnValidEquation( const std::list<GameFieldBlock*>& blockList )
{
	m_NumEqsCleared++;
	m_NumBlocksCleared += (uint32)blockList.size();

	// Store the products
	for( std::list<GameFieldBlock*>::const_iterator iterBlock = blockList.begin(); iterBlock != blockList.end(); ++iterBlock )
	{
		// Get the multiplication block
		GameFieldBlockProduct* pBlock = reinterpret_cast<GameFieldBlockProduct*>( *iterBlock );

		// If this is a product block then it may be a combo block or may be the final product block
		if( pBlock->IsProductBlock() )
			continue;

		// Store the prime
		uint32 primeIndex = GameDefines::PrimeToIndex( pBlock->GetValue() );
		if( primeIndex == GameDefines::NUM_PRIMES )
			break;
		m_ValidCounts[primeIndex]++;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsMult::OnErroneousEquation()  Public
///
///	\param blockList The list of blocks in the equation
///
///	A message handler called when the user selects an erroneous equation.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSessionStatsMult::OnErroneousEquation( const std::list<GameFieldBlock*>& blockList )
{
	m_NumEqsFailed++;

	// Store the products
	for( std::list<GameFieldBlock*>::const_iterator iterBlock = blockList.begin(); iterBlock != blockList.end(); ++iterBlock )
	{
		// Get the multiplication block
		GameFieldBlockProduct* pBlock = reinterpret_cast<GameFieldBlockProduct*>( *iterBlock );

		// If this is a product block then it may be a combo block or may be the final product block
		if( pBlock->IsProductBlock() )
			continue;

		// Store the prime
		uint32 primeIndex = GameDefines::PrimeToIndex( pBlock->GetValue() );
		if( primeIndex == GameDefines::NUM_PRIMES )
			break;
		m_ErrorCounts[primeIndex]++;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsMult::OnCombo()  Public
///
///	Called when the user goes creates a combo.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSessionStatsMult::OnCombo( uint32 comboCount )
{
	m_NumCombos++;
	if( comboCount > m_MaxCombo )
		m_MaxCombo = comboCount;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameSessionStatsMult::StoreStartTime  Public
///
/// Store the current time as the session start time
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameSessionStatsMult::StoreStartTime()
{
	time_t curTime;
	time( &curTime );
#ifdef WIN32
	localtime_s( &m_SessionStartTime, &curTime );
#else
	m_SessionStartTime = *localtime( &curTime );
#endif
}