//=================================================================================================
/*!
	\file GameFieldCeiling.h
	Game Play Library
	Ceiling Game Field Header
	\author Taylor Clark
	\date December 24, 2009

	This header contains the definition for the ceiling mode game field object.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldCeiling_h
#define __GameFieldCeiling_h

#include "GameFieldMultBase.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldCeiling
	\brief Represents a game field used for the ceiling version of gameplay.

	This class defines the game field class that is used to encapsulate data related to the block
	layout of a game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldCeiling : public GameFieldMultBase
{
private:

	/// The maximum number of ceiling blocks allowed in one column
	static const int32 MAX_CEILING_OFFSET = 4;

	/// The number of celiing blocks there are in each column
	int32 m_CeilingOffset[FIELD_WIDTH];

	float32 _lowestBlockBottom;

	/// The crate sprite
	RefSpriteHndl m_OffsetSprite;

	/// Set when the field is in a finished state and the player successfully made the skyline of
	///	their field fit into the crate blocks, mutable so it can be modified in the IsGameOver
	/// funcion
	mutable bool m_IsVictory;

	float32 GetColumnOffsets(int32 offsets[]) const;

public:

	/// The default constructor
	GameFieldCeiling() : GameFieldMultBase(),
						m_IsVictory( false ),
						_lowestBlockBottom( 0.0f )
	{
		for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
			m_CeilingOffset[colIndex] = 0;
	}

	/// Initialize the field
	virtual bool Init();

	/// Get if the game is over
	virtual bool IsGameOver() const;

	/// Draw the game
	virtual void Draw() const;

	/// Set the offset blocks
	void SetOffsets( int32 offsets[FIELD_WIDTH] );

	/// Check if the field was in a victorious state when finished
	bool IsVictorious() const { return m_IsVictory; }
};

#endif // __GameFieldCeiling_h