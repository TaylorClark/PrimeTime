//=================================================================================================
/*!
	\file GameFieldBlockAdd.h
	Game Play Library
	Game Field Fraction Block Header
	\author Taylor Clark
	\date December 29, 2009

	This header contains the definition for the game field block object during fraction mode.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldBlockFraction_h
#define __GameFieldBlockFraction_h

#include "GameFieldBlockMultBase.h"

class GameFieldFractions;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldBlockFraction
	\brief Represents a block in a fraction-mode game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldBlockFraction : public GameFieldBlockMultBase
{
	friend class GameFieldFractions;

public:

	/// The size of the block in tiles
	Vector2i tileSize;

public:

	/// The constructor to initialize the block data
	GameFieldBlockFraction( int32 value ) : GameFieldBlockMultBase( value )
	{
	}
};

#endif // __GameFieldBlockFraction_h