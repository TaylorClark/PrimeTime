//=================================================================================================
/*!
	\file ProfileSubsetMultiplication.h
	Game Play Library
	Multiplication Profile Subset Header
	\author Taylor Clark
	\date July 26, 2006

	This header contains the definition for the player profile class.
*/
//=================================================================================================

#pragma once
#ifndef __ProfileSubsetMultiplication_h
#define __ProfileSubsetMultiplication_h

#include "ProfileSubsetInGame.h"
#include <list>
#include "GameSessionStatsMult.h"

class GameFieldBlock;


//-------------------------------------------------------------------------------------------------
/*!
	\class ProfileSubsetMultiplication
	\brief Represents a profile subset for multiplication

	This class defines a player profile object that store miscellaneous stats.
*/
//-------------------------------------------------------------------------------------------------
class ProfileSubsetMultiplication : public ProfileSubsetInGame
{
public:

	static const FourCC SUBSET_ID;

	/// Profile session context flags
	static const uint32 PRO_SESS_TYPEMASK = 0x000000FF;
	static const uint32 PRO_SESS_TYPE_PRMIMPV = 0x00000001;
	static const uint32 PRO_SESS_TYPE_INCDIFF = 0x00000002;

	static const uint32 PRO_SESS_PRMIMPV_VAL_MASK = 0x00000F00;
	static const uint32 PRO_SESS_PRMIMPV_VAL_OFFSET = 8;

	static const uint32 PRO_SESS_DIFFINC_VAL_MASK = 0x00000F00;
	static const uint32 PRO_SESS_DIFFINC_VAL_OFFSET = 8;

private:

	struct PTSettings
	{
		/// The gameplay difficulty
		GameDefines::EGameplayDiffLevel m_Difficulty;

		/// The time limit
		uint32 m_TimeLimit;
	};

	/// The list of recent sessions sorted with the most recent sessions at the front of the list
	typedef std::list< GameSessionStatsMult > SessionList;
	SessionList m_Sessions;

	/// The iterator for stepping through sessions
	SessionList::iterator m_SessionIter;

public:

	/// The default constructor
	ProfileSubsetMultiplication()
	{
		m_SessionIter = m_Sessions.end();
	}

	/// The destructor
	virtual ~ProfileSubsetMultiplication() {}

	/// Transfer the subset data
	virtual bool Serialize( Serializer& serializer );

	/// Get the FourCC for this subset
	virtual FourCC GetSubsetFourCC() const { return SUBSET_ID; }

	/// Store a game session to the profile
	void StoreSession( const GameSessionStatsMult& sessionStats );

	/// Start iterating at the first session
	const GameSessionStatsMult* GetFirstSession()
	{
		m_SessionIter = m_Sessions.begin();
		if( m_SessionIter == m_Sessions.end() )
			return 0;
		return &(*m_SessionIter);
	}

	/// Step through the sessions
	const GameSessionStatsMult* GetNextSession()
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

#endif // __ProfileSubsetMultiplication_h