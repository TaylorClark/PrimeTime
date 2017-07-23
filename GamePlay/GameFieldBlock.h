//=================================================================================================
/*!
	\file GameFieldBlock.h
	Game Play Library
	Game Field Header
	\author Taylor Clark
	\date May 12, 2006

	This header contains the definition for the game field object.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldBlock_h
#define __GameFieldBlock_h

#include "Base/Types.h"
#include "Graphics2D/RefSprite.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldBlock
	\brief Represents a block in a game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldBlock
{
public:

	/// The number on the block
	int32 m_Value;

	/// The Y position of the bottom of the block in field coordinates, where 0 is the bottom of
	/// the field and increases upwards
	float32 yPos;

	/// The speed at which this block is falling
	float32 yFallingSpeed;

	/// The column index of this block
	int32 colIndex;

	/// If this block is falling
	bool isFalling;

	/// The unique ID of this block
	uint32 m_BlockID;

	/// The size of the text buffer for block display text
	static const int TEXT_BUF_SIZE = 16;

	/// The buffer for the text to display on the block
	wchar_t m_TextChars[ TEXT_BUF_SIZE ];

	/// The offset from the top-left of the block at which the text is to be drawn
	Vector2i m_TextOffset;

public:

	/// The default constructor
	GameFieldBlock( int32 value ) : m_Value( value ),
						yPos(0.0f),
						colIndex(0),
						yFallingSpeed(0.0f),
						isFalling( false ),
						m_BlockID( 0 )
	{
		static uint32 s_NextBlockID = 1;
		m_BlockID = s_NextBlockID++;
	}

	/// The destructor, virtual to ensure derived classes are freed properly
	virtual ~GameFieldBlock() {}

	/// Get the text to be displayed on the block
	virtual const wchar_t* GetText() const { return (wchar_t*)0; }

	/// Get the value on the block
	int32 GetValue() const { return m_Value; }

	/// Get the block's unique ID
	uint32 GetID() const { return m_BlockID; }

	/// Get the offset from the top left of the block to the top left of the text in screen
	/// coordinates
	virtual Vector2i GetTextOffset() const { return Vector2i(); }
};


//typedef std::list<GameFieldBlock*> BlockList;
class BlockList : public std::list<GameFieldBlock*>
{
public:
	/// Determine if a block is in this list
	bool ContainsBlock( GameFieldBlock* pTestBlock ) const
	{
		for( BlockList::const_iterator curBlock = begin(); curBlock != end(); ++curBlock )
		{
			if( *curBlock == pTestBlock )
				return true;
		}

		// The block was not found in the list
		return false;
	}
};

#endif // __GameFieldBlock_h