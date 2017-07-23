/*=================================================================================================

	\file ProfileSubsetAddition.cpp
	Game Play Library
	Addition Profile Subset Source
	\author Taylor Clark
	\Date September 13, 2006

	This source file contains the implementation of the game defines functions.

=================================================================================================*/

#include "../ProfileSubsetAddition.h"
#include "../GameMgr.h"
#include "Base/MsgLogger.h"

const FourCC ProfileSubsetAddition::SUBSET_ID("ADDI");


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ProfileSubsetAddition::Serialize  Public
///
///	\param serializer The serializer responsible for reading/writing the data
///	\returns True on a successful load/save, otherwise false
///
/// Load/save the subset data
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfileSubsetAddition::Serialize( Serializer& serializer )
{
	// The version
	int version = 1;
	serializer.AddData( version );

	// If the version is unknown then bail
	if( version != 1 )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Warning, L"Unknown addition profile subset version" );
		return false;
	}

	// The session version
	int sessionVersion = 1;
	serializer.AddData( sessionVersion );

	// The number of sessions
	uint32 numRecentSessions = (uint32)m_Sessions.size();
	serializer.AddData( numRecentSessions );

	// If we are loading then read in the sessions
	if( serializer.InReadMode() )
	{
		m_Sessions.clear();

		// Store the recent sessions
		for( uint32 sessionIndex = 0; sessionIndex < numRecentSessions; ++sessionIndex )
		{
			GameSessionStatsAdd session;
			session.Serialize_V1( serializer );
			m_Sessions.push_back( session );
		}
	}
	// Else we are saving
	else
	{
		// Go through the recent sessions
		for( SessionList::iterator iterSession = m_Sessions.begin(); iterSession != m_Sessions.end(); ++iterSession )
			iterSession->Serialize_V1( serializer );
	}

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ProfileSubsetAddition::StoreSession  Public
///
///	\param sessionStats The session to store
///	\returns True on a successful load/save, otherwise false
///
/// Store a session to the profile
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ProfileSubsetAddition::StoreSession( const GameSessionStatsAdd& sessionStats )
{
	// Store this session
	if( sessionStats.m_Duration > 0 )
		m_Sessions.push_front( sessionStats );
}