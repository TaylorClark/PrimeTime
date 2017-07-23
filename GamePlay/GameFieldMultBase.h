//=================================================================================================
/*!
	\file GameFieldMultBase.h
	Game Play Library
	Game Field Multiplication Header
	\author Taylor Clark
	\date May 12, 2006

	This header contains the definition for the multiplication base class game field object.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldMultBase_h
#define __GameFieldMultBase_h

#include "GameField.h"

class GameFieldBlock;
class GameMgr;
class GameFieldBlockMultBase;
class PIIToggleBlockSelect;

typedef std::list<GameFieldBlockMultBase*> MultBlockList;

//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldMult
	\brief Represents a game field of falling blocks.

	This class defines the game field class that is used to encapsulate data related to the block
	layout of a game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldMultBase : public GameField
{
private:
	friend class GameMgr;

protected:

	/// The speed at which boxes come up in pixels per second
	float32 m_PushUpSpeed;

	/// The columns of blocks, ordered from bottom to top
	/// This is a list of all blocks within this column.  If a block spans multiple columns then it
	/// will be in multiple column lists.  These lists will never have a NULL value in them.
	MultBlockList m_Cols[FIELD_WIDTH];

	/// The list of all unique blocks in this game field
	MultBlockList m_ActiveBlocks;

	/// If the gamefield is done
	bool m_IsFieldDone;

	/// The time at which the game field was last shaken
	float32 m_LastShakeTime;

	/// The height of each column in percent of field height
	float32 m_ColumnHeight[FIELD_WIDTH];

	
	/// Move the blocks up
	void MoveBlocksUp( float moveAmt );

	/// Remove blocks
	virtual void RemoveBlock( GameFieldBlock* pBlock );

	/// Check for block collision in the update function after the blocks are moved
	void Update_CheckForCollision();

	/// Check for block collision in a specfic column
	void Update_CheckForCollision_Column( GameFieldBlockMultBase* pFirstBlock, int32 colIndex );

	/// The recursive function to update blocks above a newly falling block
	void Recurse_UpdateFalling( GameFieldBlockMultBase* pBlock );

	/// Get the Y positition of the top of the top-most non-falling block
	float32 GetTopMostY() const;

	/// Get the block at a position on the field
	virtual GameFieldBlock* GetBlockAtPos( const Point2i& fieldPos );

	/// Handle an instruction
	virtual bool HandleInstruction( GameFieldInstruction* pInstruction );

	/// Update the game field
	virtual void SubclassUpdate( float frameTime );

public:

	/// The default constructor
	GameFieldMultBase() : m_PushUpSpeed( 1.0f ),
						m_IsFieldDone( false ),
						m_LastShakeTime( 0.0f )
	{
	}

	/// The destructor
	virtual ~GameFieldMultBase();

	/// Draw the game
	virtual void Draw() const;

	/// Initialize the field
	virtual bool Init()
	{
		m_IsFieldDone = false;
		return GameField::Init();
	}

	/// Set the rate at which blocks push up in pixels per second
	float32 GetBlockPushUpSpeed() const { return m_PushUpSpeed; }

	/// Drop a new block on the field
	void DropNewBlock( GameFieldBlockMultBase* pNewBlock );

	/// Drop a new block on the field
	void DropNewBlocks( GameFieldBlockMultBase** ppNewBlocks );

	/// Push up a new row of blocks
	void PushNewBlocks( GameFieldBlockMultBase** ppNewBlock );

	/// Get the danger level for the field
	float32 GetDangerLevel() const;

	/// Get the iterator of a block within a column
	MultBlockList::iterator FindBlockInCol( GameFieldBlockMultBase* pBlock, MultBlockList& blockCol );

	/// Clear the field
	virtual void ClearField();

	/// Get if the game is over
	virtual bool IsGameOver() const { return m_IsFieldDone; }

	/// Get the y-position of the bottom most block on the field
	float32 GetBottomMostBlockY() const { return m_LowestY; }
};

#endif // __GameFieldMultBase_h