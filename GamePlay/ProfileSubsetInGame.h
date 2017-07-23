//=================================================================================================
/*!
	\file ProfileSubsetInGame.h
	Game Play Library
	Base Profile Subset Header
	\author Taylor Clark
	\date July 26, 2006

	This header contains the definition for the base player profile subset class.
*/
//=================================================================================================

#pragma once
#ifndef __ProfileSubsetInGame_h
#define __ProfileSubsetInGame_h

#include "Base/Serializer.h"
#include "PlayerProfile.h"
#include "ProfileSubset.h"

class GameFieldBlock;


//-------------------------------------------------------------------------------------------------
/*!
	\class ProfileSubsetInGame
	\brief Represents the in-game profile subset base class
*/
//-------------------------------------------------------------------------------------------------
class ProfileSubsetInGame : public ProfileSubset
{
protected:

	/// The amount of time, in seconds, that data is considered recent
	static const uint32 RECENT_TIME_CAP = 20 * 60;

public:

	enum EGameEndReason
	{
		GER_GameOver,
		GER_ManualQuit,
		GER_TimeOver,
		GER_COUNT
	};

	/// The destructor
	virtual ~ProfileSubsetInGame(){}

	/// Load/save the subset data
	virtual bool Serialize( Serializer& serializer ) = 0;

	/// Get the FourCC for this subset
	FourCC GetSubsetFourCC() const = 0;
};

#endif // __ProfileSubsetInGame_h