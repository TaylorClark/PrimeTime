//=================================================================================================
/*!
	\file GameFieldBlockMultBase.h
	Game Play Library
	Game Field Header
	\author Taylor Clark
	\date May 12, 2006

	This header contains the definition for the product game field block object base class.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldBlockMultBase_h
#define __GameFieldBlockMultBase_h

#include "GameFieldBlock.h"
#include <stdio.h>


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldBlockMultBase
	\brief Represents a block in a multiplication game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldBlockMultBase : public GameFieldBlock
{
public:

	/// The size of the block in tiles
	Vector2i size;

	/// The sprite for this block
	RefSpriteHndl sprite;

public:

	/// The default constructor
	GameFieldBlockMultBase( int32 value ) : GameFieldBlock( value ),
															size(1,1)
	{
		// Clear the text buffer
		for( int charIndex = 0; charIndex < TEXT_BUF_SIZE; ++charIndex )
			m_TextChars[ charIndex ] = 0;

		// If the value is more than 2 digits then increse the width
		if( value > 99 )
			size.x = 2;

		// Make sure the value doesn't exceed what can fit
		if( m_Value > 9999 )
			m_Value = 9999;

		// Create the text string
		swprintf( m_TextChars, TEXT_BUF_SIZE, L"%d", m_Value );

		// Center the number text
		m_TextOffset.x = ((GameDefines::BLOCK_WIDTH * size.x) - GameDefines::GetBlockTextFont()->CalcStringWidth( m_TextChars )) / 2;
		m_TextOffset.y = ((GameDefines::BLOCK_HEIGHT * size.y) - GameDefines::GetBlockTextFont()->GetCharHeight()) / 2;
	}

	/// Get the text to be displayed on the block
	virtual const wchar_t* GetText() const
	{
		return m_TextChars;
	}

	/// Get the offset from the top left of the block to the top left of the text in screen
	/// coordinates
	virtual Vector2i GetTextOffset() const { return m_TextOffset; }
};

#endif // __GameFieldBlockMultBase_h