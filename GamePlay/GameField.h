//=================================================================================================
/*!
	\file GameField.h
	Game Play Library
	Game Field Header
	\author Taylor Clark
	\date May 12, 2006

	This header contains the definition for the multiplication base class game field object.
*/
//=================================================================================================

#pragma once
#ifndef __GameField_h
#define __GameField_h

#include <list>
#include <map>
#include "Base/Types.h"
#include "GUI/GUIControl.h"
#include "Math/Box2i.h"
#include "GamePlay/AnimSprite.h"
#include "Resource/ResourceMgr.h"
#include "GameFieldBlock.h"

class GameMgr;
class GameFieldInstruction;
class PIIToggleBlockSelect;
class InstructionStream;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameField
	\brief Represents a game field of falling blocks.

	This class defines the game field class that is used to encapsulate data related to the block
	layout of a game field.
*/
//-------------------------------------------------------------------------------------------------
class GameField
{
public:

	static const int32 FIELD_WIDTH = 5;
	static const int32 FIELD_HEIGHT = 12;

private:

	/// The amount time the field has been active
	float32 m_FieldElapsedTime;

public:

	/// The playing field bounding box in screen coordinates, the position is the top left
	Box2i m_FieldBox;

	/// The list of selected blocks
	BlockList m_SelBlocks;

	/// The offset from the playing field at which to draw the border
	Vector2i m_BorderOffset;

	/// The bottom most y-position on the field
	float32 m_LowestY;

	/// The select sound
	AudioSampleHndl m_SelectSound;

	/// The deselect sound
	AudioSampleHndl m_DeselectSound;

	/// The block falling sound
	AudioSampleHndl m_BlockFallSound;

	/// The selection bracket sprite
	AnimSprite m_SelBracketSprite;

	/// The wide selection bracket sprite
	AnimSprite m_WideSelBracketSprite;

	/// The source for game field instructions
	InstructionStream* m_pInstStream;

	/// The map of block IDs to blocks, used by instructions
	std::map< uint32, GameFieldBlock* > m_BlockIDMap;

	/// The ID of the next block
	uint32 m_NextBlockID;


	/// Get if a block is selected
	bool IsBlockSelected( GameFieldBlock* pBlock, BlockList::const_iterator* pSaveBlockIter = NULL ) const;

	bool IsBlockSelected( GameFieldBlock* pBlock, BlockList::iterator* pSaveBlockIter = NULL );

	/// Get the block at a position on the field
	virtual GameFieldBlock* GetBlockAtPos( const Point2i& fieldPos ) = 0;

	/// Handle a mouse click in the update function
	void Update_HandleClick( PIIToggleBlockSelect* pInstruction );

	/// Handle an instruction
	virtual bool HandleInstruction( GameFieldInstruction* pInstruction );

	/// Remove a block
	virtual void RemoveBlock( GameFieldBlock* pRemBlock ) = 0;

	/// Allow derived classes to update
	virtual void SubclassUpdate( float frameTime ) = 0;

public:

	GameField() : m_LowestY( 0.0f ),
					m_FieldElapsedTime( 0.0f ),
					m_pInstStream( 0 ),
					m_NextBlockID( 1 )
	{
	}

	/// The destructor, virtual to ensure derived classes are freed properly
	virtual ~GameField();

	/// Get if the game is over
	virtual bool IsGameOver() const = 0;

	/// Initialize the field
	virtual bool Init();

	/// Update the game field
	virtual void Update( float frameTime );

	/// Draw the game
	virtual void Draw() const = 0;

	/// Add an instruction for processing
	virtual void SendInstruction( GameFieldInstruction* pInstruction );

	/// Get the blocks that intersect a rectangle
	BlockList GetBlocksInRectangle( const Box2i& box );

	/// Get the list of selected blocks
	const BlockList& GetSelectedBlocks() const { return m_SelBlocks; }

	/// Receive a mouse click
	void ClickAtPoint( const Point2i& cursorPos, GUIDefines::EMouseButtons btn );

	/// Convert a block position on the field to screen coordinates
	Point2i BlockFieldToScreenPos( int32 colIndex, float32 yPos, int32 ySize ) const;

	/// Convert a position on the field to screen coordinates
	Point2i BlockFieldToScreenPos( const Point2i& fieldPos ) const;

	/// Convert a position from screen coordinates to game field coordinates.
	Point2i ScreenPosToBlockField( const Point2i& screenPos ) const;

	/// Clear the field
	virtual void ClearField() = 0;

	/// Get the field box
	Box2i GetFieldRect() const { return m_FieldBox; }

	/// Get the field's elapsed time
	float32 GetFieldElapsedTime() const { return m_FieldElapsedTime; }
};

#endif // __GameField_h