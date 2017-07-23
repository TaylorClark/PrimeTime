//=================================================================================================
/*!
	\file GameFieldBlockAdd.h
	Game Play Library
	Game Field Addition Block Header
	\author Taylor Clark
	\date September 9, 2006

	This header contains the definition for the addition game field block object.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldBlockAdd_h
#define __GameFieldBlockAdd_h

#include "GameFieldBlock.h"
#include <stdio.h>

class GameFieldAdd;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldBlockAdd
	\brief Represents a block in a addition-mode game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldBlockAdd : public GameFieldBlock
{
	friend class GameFieldAdd;

private:

	/// Set the value
	void SetValue( int32 value, bool isSum )
	{
		m_Value = value;

		// Clear the text buffer
		for( int charIndex = 0; charIndex < TEXT_BUF_SIZE; ++charIndex )
			m_TextChars[ charIndex ] = 0;

		// If this is a summand then clamp the value
		if( !isSum && m_Value > 99 )
			m_Value = 99;

		// If the value is larger than 2 digits then use a bigger box
		int32 boxWidth = GameDefines::BLOCK_WIDTH;
		if( m_Value > 99 )
			boxWidth = GameDefines::BLOCK_WIDTH * 2;
		
		// Make sure the value doesn't exceed what can fit
		if( m_Value > 9999 )
			m_Value = 9999;

		// Create the text string
		swprintf( m_TextChars, TEXT_BUF_SIZE, L"%d", m_Value );

		// Get the text offset
		m_TextOffset.x = (boxWidth - GameDefines::GetBlockTextFont()->CalcStringWidth( m_TextChars )) / 2;
		m_TextOffset.y = (GameDefines::BLOCK_HEIGHT - GameDefines::GetBlockTextFont()->GetCharHeight()) / 2;
	}

	/// The constructor to initialize the block data
	GameFieldBlockAdd( uint32 blockID, int32 value, bool isSum = false ) : GameFieldBlock( value )
	{
		m_BlockID = blockID;
		SetValue( value, isSum );
	}

public:

	/// Get the text to be displayed on the block
	virtual const wchar_t* GetText() const
	{
		return m_TextChars;
	}

	/// Get the offset from the top left of the block to the top left of the text in screen
	/// coordinates
	virtual Vector2i GetTextOffset() const { return m_TextOffset; }
};

#endif // __GameFieldBlockAdd_h