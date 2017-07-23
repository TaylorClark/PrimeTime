//=================================================================================================
/*!
	\file GameFieldAdd.h
	Game Play Library
	Game Field Addition Header
	\author Taylor Clark
	\date September 3, 2006

	This header contains the definition for the addition base class game field object.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldAdd_h
#define __GameFieldAdd_h

#include "GameField.h"

class GameFieldBlock;
class GameMgr;
class GameFieldBlockAdd;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldAdd
	\brief Represents a game field of falling blocks.

	This class defines the game field class that is used to encapsulate data related to the block
	layout of a game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldAdd : public GameField
{
private:

	static const int32 UPPER_INDEX = 0;
	static const int32 LOWER_INDEX = 1;

	typedef std::list< GameFieldBlockAdd* > AddBlockList;

	// The list the field blocks, sorted from middle outward
	AddBlockList m_BlockCols[2][FIELD_WIDTH];

	/// An array storing which columns have blocks that need updating
	bool m_UpdateCols[ FIELD_WIDTH ];

	/// The Y position, in field coordinates, where lower blocks push up to and high blocks land
	/// on
	float32 m_YMidPoint;

	/// The number of blocks currently on the field
	uint32 m_NumBlocks;

	/// The number of blocks in one column of the top half of the field
	uint32 m_MaxColBlocks[2];

	/// The sum block
	GameFieldBlockAdd* m_pSumBlock;

	/// The position of the sum block
	Point2i m_SumBlockPos;


	/// Go through one column moved blocks and check for collision
	void Update_CheckForCollision( AddBlockList& blockCol, bool isFalling );

	/// Go through the moved blocks and check for collision
	void Update_CheckForCollision();

	/// Remove a block
	virtual void RemoveBlock( GameFieldBlock* pRemBlock );

	/// Get the iterator for a block in the column
	AddBlockList::iterator FindBlockInCol( GameFieldBlockAdd* pBlock, AddBlockList& blockList );

	/// Get the block at a position
	virtual GameFieldBlock* GetBlockAtPos( const Point2i& fieldPos );

	/// Handle an instruction
	virtual bool HandleInstruction( GameFieldInstruction* pInstruction );

	/// Update the game field
	virtual void SubclassUpdate( float frameTime );

	/// Set the sum block
	void SetSumBlock( int32 sumValue );

public:

	/// The default constructor
	GameFieldAdd();

	/// The destructor
	virtual ~GameFieldAdd(){ ClearField(); }

	/// Initialize the field
	virtual bool Init();

	/// Get if the game is over
	virtual bool IsGameOver() const;

	/// Draw the game
	virtual void Draw() const;

	/// Clear the file blocks
	virtual void ClearField();

	/// Get the block count
	uint32 GetNumBlocks() const { return m_NumBlocks; }

	/// Set the sum block position
	void SetSumBlockPos( const Point2i& pos ){ m_SumBlockPos = pos; }

	/// Get the sum block
	GameFieldBlockAdd* GetSumBlock() { return m_pSumBlock; }

	/// Add blocks to the field
	void AddBlocks( const AddBlockList& blocks );
};

#endif // __GameFieldAdd_h