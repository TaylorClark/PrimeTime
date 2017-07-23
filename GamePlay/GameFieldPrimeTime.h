//=================================================================================================
/*!
	\file GameFieldPrimeTime.h
	Game Play Library
	Game Field Multiplication Header
	\author Taylor Clark
	\date May 12, 2006

	This header contains the definition for the Prime Time mode game field object.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldPrimeTime_h
#define __GameFieldPrimeTime_h

#include "GameFieldMultBase.h"

class GameFieldBlockProduct;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldPrimeTime
	\brief Represents a game field used for the normal mode of gameplay.

	This class defines the game field class that is used to encapsulate data related to the block
	layout of a game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldPrimeTime : public GameFieldMultBase
{
private:

	enum EBlockDir
	{
		BD_Right = 0,
		BD_Up,
		BD_Left,
		BD_Down
	};

	struct SurroundingBlocks
	{
		SurroundingBlocks()
		{
			for( int32 blockIndex = 0; blockIndex < NumSurrounding; ++blockIndex )
				blocks[blockIndex] = 0;
		}

		static const int32 NumSurrounding = 8;

		GameFieldBlock* blocks[NumSurrounding];
	};

	/// The combo block bounding box
	Box2i m_ComboBlockBoundBox;

	/// The combo block bounding box in field coordinates
	Box2i m_ComboBlockFieldBoundBox;

	/// The combo block, NULL if there is none
	GameFieldBlockProduct* m_pComboBlock;

	MultBlockList m_ComboableProducts;

	AnimSprite m_ComboSelection;
	AnimSprite m_WideComboSelection;


	/// Get the blocks that surrounding a block
	BlockList GetSurroundingBlocks( GameFieldBlockProduct* pBlock );

	/// Remove blocks
	virtual void RemoveBlock( GameFieldBlockProduct* pBlock );

	/// Update the game field
	virtual void SubclassUpdate( float frameTime );

public:
	
	/// The default constructor
	GameFieldPrimeTime() : m_pComboBlock( 0 )
	{}

	/// The destructor
	virtual ~GameFieldPrimeTime() { ClearField(); }

	/// Initialize the field
	virtual bool Init();

	/// Draw the game
	virtual void Draw() const;

	/// Get the block at a position on the field
	virtual GameFieldBlock* GetBlockAtPos( const Point2i& fieldPos );

	/// Determine if a block is comboable
	bool IsBlockComboable( GameFieldBlockProduct* pBlock, MultBlockList::const_iterator* pIterBlock = NULL ) const;
	bool IsBlockComboable( GameFieldBlockProduct* pBlock, MultBlockList::iterator* pIterBlock = NULL );

	/// Set the combo block
	void SetComboBlock( int32 blockValue );

	/// Get the combo block
	GameFieldBlockProduct* GetComboBlock() { return reinterpret_cast<GameFieldBlockProduct*>(m_pComboBlock); }

	/// Add comboable blocks to the list
	void AddToComboBlocksFromSelection();

	/// Clear the comboable blocks
	void ClearComboable() { m_ComboableProducts.clear(); }

	/// Clear the field
	virtual void ClearField();
};

#endif // __GameFieldPrimeTime_h