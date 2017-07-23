/*=================================================================================================

	\file ProfileSubsetMultiplication.cpp
	Game Play Library
	Multiplication Profile Subset Source
	\author Taylor Clark
	\Date June 26, 2006

	This source file contains the implementation of the multiplication player profile subset.

=================================================================================================*/

#include "../ProfileSubsetMultiplication.h"
#include "../GameFieldBlockProduct.h"
#include "Base/NumFuncs.h"
#include "../GameMgr.h"
#include "Base/MsgLogger.h"

const FourCC ProfileSubsetMultiplication::SUBSET_ID("MULT");


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ProfileSubsetMultiplication::Serialize()  Public
///
///	\param serializer The serializer that reads and writes data
///	\returns True if the subset was loaded successfully, otherwise false
///
///	Transfer the subset data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfileSubsetMultiplication::Serialize( Serializer& serializer )
{
	// The version
	int version = 1;
	serializer.AddData( version );

	// If the version is unknown then bail
	if( version != 1 )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Warning, L"Unknown multiplication profile subset version" );
		return false;
	}

	// Transfer the number of primes
	uint32 numPrimes = GameDefines::NUM_PRIMES;
	serializer.AddData( numPrimes );

	// The session version
	int sessionVersion = 1;
	serializer.AddData( sessionVersion );

	// The number of sessions
	uint32 numRecentSessions = (uint32)m_Sessions.size();
	serializer.AddData( numRecentSessions );

	// If we the profile is being loaded then read in the sessions
	if( serializer.InReadMode() )
	{
		m_Sessions.clear();

		// Store the recent sessions
		for( uint32 sessionIndex = 0; sessionIndex < numRecentSessions; ++sessionIndex )
		{
			GameSessionStatsMult session;
			if( sessionVersion == 1 )
				session.Serialize_V1( serializer, numPrimes );
			m_Sessions.push_back( session );
		}
	}
	// Else we are saving
	else
	{
		// Go through the recent sessions
		for( SessionList::iterator iterSession = m_Sessions.begin(); iterSession != m_Sessions.end(); ++iterSession )
			iterSession->Serialize_V1( serializer, numPrimes );
	}

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ProfileSubsetMultiplication::StoreSession()  Public
///
///	\param sessionStats The session stats
///
///	Store a session to the profile
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ProfileSubsetMultiplication::StoreSession( const GameSessionStatsMult& sessionStats )
{
	// Store this session
	if( sessionStats.m_Duration > 0 )
		m_Sessions.push_front( sessionStats );
}