//=================================================================================================
/*!
	\file ProfileSubsetAddition .h
	Game Play Library
	Addition Profile Subset Header
	\author Taylor Clark
	\date September 13, 2006

	This header contains the definition for the base player profile addition subset class.
*/
//=================================================================================================

#pragma once
#ifndef __ProfileSubsetAddition_h
#define __ProfileSubsetAddition_h

#include "ProfileSubsetInGame.h"
#include <time.h>
#include "GameSessionStatsAdd.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class ProfileSubset
	\brief Represents the profile subset base class
*/
//-------------------------------------------------------------------------------------------------
class ProfileSubsetAddition : public ProfileSubsetInGame
{
public:

	static const FourCC SUBSET_ID;

private:

	/// The list of recent sessions sorted with the most recent sessions at the front of the list
	typedef std::list< GameSessionStatsAdd > SessionList;
	SessionList m_Sessions;

	/// The iterator for stepping through sessions
	SessionList::iterator m_SessionIter;

public:

	/// Load/save the subset data
	virtual bool Serialize( Serializer& serializer );

	/// Get the FourCC for this subset
	static FourCC GetSubsetFourCC_Static(){ return SUBSET_ID; }

	/// Get the FourCC for this subset
	virtual FourCC GetSubsetFourCC() const { return GetSubsetFourCC_Static(); }

	/// Store a game session to the profile
	virtual void StoreSession( const GameSessionStatsAdd& sessionStats );

	/// Start iterating at the first session
	const GameSessionStatsAdd* GetFirstSession()
	{
		m_SessionIter = m_Sessions.begin();
		if( m_SessionIter == m_Sessions.end() )
			return 0;
		return &(*m_SessionIter);
	}

	/// Step through the sessions
	const GameSessionStatsAdd* GetNextSession()
	{
		// If the iterator is already at the end
		if( m_SessionIter == m_Sessions.end() )
			return 0;

		// Step forward
		m_SessionIter++;

		// If the iterator reached the end
		if( m_SessionIter == m_Sessions.end() )
			return 0;

		return &(*m_SessionIter);
	}

	/// Get the number of sessions
	uint32 GetNumSessions() const { return (uint32)m_Sessions.size(); }
};

#endif // __ProfileSubsetAddition_h