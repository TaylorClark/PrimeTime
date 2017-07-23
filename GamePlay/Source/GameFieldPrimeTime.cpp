/*=================================================================================================

	\file GameFieldMult.cpp
	Game Play Library
	Prime Time Game Field Source
	\author Taylor Clark
	\Date December 24, 2009

	This source file contains the implementation of the Prime Time version of the game field class.

=================================================================================================*/

#include "../GameFieldPrimeTime.h"
#include "Base/TCAssert.h"
#include "../GameFieldBlock.h"
#include "../GameDefines.h"
#include "Graphics2D/GraphicsMgr.h"
#include "../GameFieldBlockProduct.h"
#include "../GameMgr.h"
#include "../GameLogic.h"
#include "Base/NumFuncs.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldPrimeTime::RemoveBlock  Private
///
///	\param pRemBlock The block to remove
///
///	Remove a block from the game field.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldPrimeTime::RemoveBlock( GameFieldBlockProduct* pRemBlock )
{
	TCASSERT( pRemBlock != NULL );

	// If this is the combo block
	if( pRemBlock == m_pComboBlock )
	{
		// If the block is selected then remove it from the selected list
		BlockList::iterator iterSelBlock;
		if( IsBlockSelected( pRemBlock, &iterSelBlock ) )
			m_SelBlocks.erase( iterSelBlock );

		// Remove it from the ID map
		m_BlockIDMap.erase( pRemBlock->GetID() );

		// Free the block
		delete pRemBlock;
		m_pComboBlock = 0;
	}
	// Else allow the base class to remove the block
	else
		GameFieldMultBase::RemoveBlock( pRemBlock );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldPrimeTime::GetBlockAtPos  Private
///
///	\param fieldPos The field position at which a block will be detected for intersection
///	\returns A pointer to the block at the input position, null if there is no block
///
/// Get the block at a position on the field
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlock* GameFieldPrimeTime::GetBlockAtPos( const Point2i& fieldPos )
{
	// If there is a combo block then check it
	if( m_pComboBlock )
		if( m_ComboBlockFieldBoundBox.IsPointInside( fieldPos ) )
			return m_pComboBlock;

	// Allow the base class to handle it
	return GameFieldMultBase::GetBlockAtPos( fieldPos );
}

/// Draw the game
void GameFieldPrimeTime::Draw() const
{
	GameFieldMultBase::Draw();

	// If there is a combo block and the current game is not the Prime Time demo then draw the
	// block
	if( m_pComboBlock && !GameMgr::Get().IsPTTutorial() )
	{
		TCFont* pBlockFont = GameDefines::GetBlockTextFont().GetObj();

		// Draw the block and text
		g_pGraphicsMgr->DrawSprite( m_ComboBlockBoundBox.pos, m_pComboBlock->sprite.GetObj() );
		g_pGraphicsMgr->DrawFontText( pBlockFont, m_pComboBlock->GetText(), m_ComboBlockBoundBox.pos + m_pComboBlock->GetTextOffset() );

		// If the block is selected
		if( IsBlockSelected( m_pComboBlock ) )
		{
			if( m_pComboBlock->size.x == 1 )
				g_pGraphicsMgr->DrawImage( m_ComboBlockBoundBox.pos, m_SelBracketSprite.GetImage(), m_SelBracketSprite.GetSrcCoords() );
			else
				g_pGraphicsMgr->DrawImage( m_ComboBlockBoundBox.pos, m_SelBracketSprite.GetImage(), m_WideSelBracketSprite.GetSrcCoords() );
		}
	}

	// Go through the combo blocks and draw the combo efect around each one
	for( MultBlockList::const_iterator iterBlock = m_ComboableProducts.begin(); iterBlock != m_ComboableProducts.end(); ++iterBlock )
	{
		const GameFieldBlockProduct* pBlock = (GameFieldBlockProduct*)(*iterBlock);

		// Get the position from game field coordinates to screen coordinates
		Point2i blockPos = BlockFieldToScreenPos( pBlock->colIndex, pBlock->yPos, pBlock->size.y );

		// Use the wide combo effect if the block is more than one tile wide
		AnimSprite animSpr;
		if( pBlock->size.x == 1 )
			animSpr = m_ComboSelection;
		else
			animSpr = m_WideComboSelection;

		// If this block is off the bottom
		if( pBlock->yPos < 0.0f )
		{
			Box2i destRect( blockPos.x, blockPos.y, GameDefines::BLOCK_WIDTH, GameDefines::BLOCK_HEIGHT + (int32)pBlock->yPos );
			Box2i srcRect( animSpr.GetSrcCoords() );
			srcRect.size.y = destRect.size.y;
			g_pGraphicsMgr->DrawImage( destRect.pos, animSpr.GetImage(), srcRect );
		}
		// Else just draw the cursor
		else
			g_pGraphicsMgr->DrawImage( blockPos, animSpr.GetImage(), animSpr.GetSrcCoords() );
	}
}

/// Update the game field
void GameFieldPrimeTime::SubclassUpdate( float frameTime )
{
	GameFieldMultBase::SubclassUpdate( frameTime );

	m_ComboSelection.Update( frameTime );
	m_WideComboSelection.Update( frameTime );
}

/// Set the combo block
void GameFieldPrimeTime::SetComboBlock( int32 blockValue )
{
	// Free the old block
	if( m_pComboBlock )
		RemoveBlock( m_pComboBlock );

	// Create the new block
	m_pComboBlock = new GameFieldBlockProduct( blockValue, true );

	if( blockValue < 100 )
		m_pComboBlock->sprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK );
	else
		m_pComboBlock->sprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DOUBLE_PRODUCT_BLOCK );

	// Update the bounding box
	m_pComboBlock->colIndex = -10;
	m_ComboBlockBoundBox.size.x = m_pComboBlock->size.x * GameDefines::BLOCK_WIDTH;
	m_ComboBlockBoundBox.size.y = m_pComboBlock->size.y * GameDefines::BLOCK_HEIGHT;
	m_ComboBlockFieldBoundBox.size.x = m_ComboBlockBoundBox.size.x;
	m_ComboBlockFieldBoundBox.size.y = m_ComboBlockBoundBox.size.y;

	// Store the block in the ID map
	m_BlockIDMap.insert( std::pair<uint32,GameFieldBlock*>(m_pComboBlock->GetID(),m_pComboBlock) );
}

/// Initialize the field
bool GameFieldPrimeTime::Init()
{
	// Get the static selection sprite
	m_ComboSelection = AnimSprite( ResourceMgr::Get().GetRefSprite( RESID_SPRITE_STATIC_BLOCK ) );
	m_ComboSelection.Play();
	m_WideComboSelection = AnimSprite( ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DOUBLE_STATIC_BLOCK ) );
	m_WideComboSelection.Play();

	// Store the combo block position
	m_ComboBlockBoundBox.pos = Point2i( m_FieldBox.pos.x - (GameDefines::BLOCK_WIDTH * 3), m_FieldBox.pos.y );
	
	/// Convert a position on the field to screen coordinates
	m_ComboBlockFieldBoundBox.pos = Point2i( 0 - (GameDefines::BLOCK_WIDTH * 3), m_FieldBox.size.y - GameDefines::BLOCK_HEIGHT );

	return GameFieldMultBase::Init();
}


/// Get the blocks that surrounding a block
BlockList GameFieldPrimeTime::GetSurroundingBlocks( GameFieldBlockProduct* pBlock )
{
	BlockList retList;

	// Create a box that represents the product block that is being cleared, then inflate it by
	// one block so it intersects the blocks around it
	Box2i blockRect( pBlock->colIndex * GameDefines::BLOCK_WIDTH, (int32)pBlock->yPos, pBlock->size.x * GameDefines::BLOCK_WIDTH, pBlock->size.y * GameDefines::BLOCK_HEIGHT );
	blockRect.Inflate( GameDefines::BLOCK_WIDTH, GameDefines::BLOCK_HEIGHT );

	return this->GetBlocksInRectangle( blockRect );
	/*
	// Go along the left side, from bottom to top
	Point2i testFieldPos(blockRect.pos.x,blockRect.pos.y);
	GameFieldBlockProduct* pTestBlock = NULL;
	while( testFieldPos.y < blockRect.Top() )
	{
		pTestBlock = reinterpret_cast<GameFieldBlockProduct*>( this->GetBlockAtPos( testFieldPos ) );
		if( pTestBlock )
		{
			retList.push_back( pTestBlock );

			// Move up a block
			testFieldPos.y += pTestBlock->size.y * GameDefines::BLOCK_HEIGHT;
		}
		else
			testFieldPos.y += GameDefines::BLOCK_HEIGHT;
	}

	// Go along the right side, from bottom to top
	testFieldPos.Set(blockRect.Right(),blockRect.pos.y);
	while( testFieldPos.y < blockRect.Top() )
	{
		pTestBlock = reinterpret_cast<GameFieldBlockProduct*>( this->GetBlockAtPos( testFieldPos ) );
		if( pTestBlock )
		{
			retList.push_back( pTestBlock );

			// Move up a block
			testFieldPos.y += pTestBlock->size.y * GameDefines::BLOCK_HEIGHT;
		}
		else
			testFieldPos.y += GameDefines::BLOCK_HEIGHT;
	}

	// Go along the bottom side, from left to right
	testFieldPos.Set(blockRect.pos.x + GameDefines::BLOCK_WIDTH,blockRect.pos.y);
	while( testFieldPos.x < blockRect.Right() - GameDefines::BLOCK_WIDTH )
	{
		pTestBlock = reinterpret_cast<GameFieldBlockProduct*>( this->GetBlockAtPos( testFieldPos ) );
		if( pTestBlock )
		{
			retList.push_back( pTestBlock );

			// Move over a block
			testFieldPos.x += pTestBlock->size.x * GameDefines::BLOCK_WIDTH;
		}
		else
			testFieldPos.y += GameDefines::BLOCK_WIDTH;
	}

	return retList;
	*/

	/*
	// Get the upper left block
	testFieldPos = blockCenter;
	testFieldPos.x -= GameDefines::BLOCK_WIDTH;
	testFieldPos.y += GameDefines::BLOCK_HEIGHT;
	GameFieldBlock* pTestBlock = this->GetBlockAtPos( testFieldPos );
	if( pTestBlock )
		retList.push_back( pTestBlock );

	// Get the left block
	testFieldPos = blockCenter;
	testFieldPos.x -= GameDefines::BLOCK_WIDTH;
	pTestBlock = this->GetBlockAtPos( testFieldPos );
	if( pTestBlock )
		retList.push_back( pTestBlock );

	// Get the lower left block
	testFieldPos = blockCenter;
	testFieldPos.x -= GameDefines::BLOCK_WIDTH;
	testFieldPos.y -= GameDefines::BLOCK_HEIGHT;
	pTestBlock = this->GetBlockAtPos( testFieldPos );
	if( pTestBlock )
		retList.push_back( pTestBlock );

	// Get the lower block
	testFieldPos = blockCenter;
	testFieldPos.y -= GameDefines::BLOCK_HEIGHT;
	pTestBlock = this->GetBlockAtPos( testFieldPos );
	if( pTestBlock )
		retList.push_back( pTestBlock );

	// Get the lower right block
	testFieldPos = blockCenter;
	testFieldPos.x += GameDefines::BLOCK_WIDTH;
	testFieldPos.y -= GameDefines::BLOCK_HEIGHT;
	pTestBlock = this->GetBlockAtPos( testFieldPos );
	if( pTestBlock )
		retList.push_back( pTestBlock );

	// Get the right block
	testFieldPos = blockCenter;
	testFieldPos.x += GameDefines::BLOCK_WIDTH;
	pTestBlock = this->GetBlockAtPos( testFieldPos );
	if( pTestBlock )
		retList.push_back( pTestBlock );

	// Get the upper right block
	testFieldPos = blockCenter;
	testFieldPos.x += GameDefines::BLOCK_WIDTH;
	testFieldPos.y += GameDefines::BLOCK_HEIGHT;
	pTestBlock = this->GetBlockAtPos( testFieldPos );
	if( pTestBlock )
		retList.push_back( pTestBlock );

	// Get the upper block
	testFieldPos = blockCenter;
	testFieldPos.y += GameDefines::BLOCK_HEIGHT;
	pTestBlock = this->GetBlockAtPos( testFieldPos );
	if( pTestBlock )
		retList.push_back( pTestBlock );

	return retList;
	*/

	/*
	const float32 BLOCK_TOUCHING_THRESH = 2.0f;
	SurroundingBlocks retData;
	
	if( !pBlock )
		return retData;

	// Get the right block
	int32 testCol = pBlock->colIndex + 1;
	if( testCol < FIELD_WIDTH )
	{
		// Go through the column and find a block at the same position as this block
		for( MultBlockList::iterator iterBlock = m_Cols[testCol].begin(); iterBlock != m_Cols[testCol].end(); ++iterBlock )
		{
			GameFieldBlockProduct* pCurBlock = reinterpret_cast< GameFieldBlockProduct* >( *iterBlock );

			if( TCBase::NearlyEquals( pCurBlock->yPos, pBlock->yPos, BLOCK_TOUCHING_THRESH ) )
			{
				retData.blocks[BD_Right] = reinterpret_cast< GameFieldBlockProduct* >( pCurBlock );
				break;
			}

			// The blocks go from bottom to top so if the current block is above the test block
			// then bail
			if( pCurBlock->yPos > pBlock->yPos )
				break;
		}
	}

	// Get the top and bottom block
	MultBlockList& colList = m_Cols[pBlock->colIndex];
	MultBlockList::iterator blockIter = FindBlockInCol( pBlock, colList );
	if( blockIter != colList.end() )
	{
		// Find the top block
		MultBlockList::iterator testIter = blockIter;
		testIter++;
		if( testIter != colList.end() )
		{
			// If the test block is sitting on top
			float32 testBlockTop = pBlock->yPos + (float32)(pBlock->size.y * GameDefines::BLOCK_HEIGHT);
			if( TCBase::NearlyEquals( (*testIter)->yPos, testBlockTop, BLOCK_TOUCHING_THRESH ) )
				retData.blocks[BD_Up] = (*testIter);
		}

		// Find the bottom block
		if( blockIter != colList.begin() )
		{
			testIter = blockIter;
			testIter--;

			// If the test block is sitting on top of the block below it
			GameFieldBlockProduct* pBelowBlock = static_cast<GameFieldBlockProduct*>(*testIter);
			float32 belowBlockTop = pBelowBlock->yPos + (float32)(pBelowBlock->size.y * GameDefines::BLOCK_HEIGHT);
			if( TCBase::NearlyEquals( pBlock->yPos, belowBlockTop, BLOCK_TOUCHING_THRESH ) )
				retData.blocks[BD_Down] = (*testIter);
		}
	}

	// Get the left block
	testCol = pBlock->colIndex - 1;
	if( testCol >= 0 )
	{
		// Go through the column and find a block at the same position as this block
		for( MultBlockList::iterator iterBlock = m_Cols[testCol].begin(); iterBlock != m_Cols[testCol].end(); ++iterBlock )
		{
			GameFieldBlockProduct* pCurBlock = reinterpret_cast< GameFieldBlockProduct* >( *iterBlock );

			if( TCBase::NearlyEquals( pCurBlock->yPos, pBlock->yPos, BLOCK_TOUCHING_THRESH ) )
			{
				retData.blocks[BD_Left] = pCurBlock;
				break;
			}

			// The blocks go from bottom to top so if the current block is above the test block
			// then bail
			if( pCurBlock->yPos > pBlock->yPos )
				break;
		}
	}

	return retData;
	*/
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldPrimeTime::ClearField  Public
///
///	Clear all of the blocks from the field.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldPrimeTime::ClearField()
{
	// Clear the base class
	GameFieldMultBase::ClearField();

	// Free the combo block
	if( m_pComboBlock )
		delete m_pComboBlock;
	m_pComboBlock = 0;
}

/// A message handler called when a successful equation is selected
void GameFieldPrimeTime::AddToComboBlocksFromSelection()
{
	if( m_SelBlocks.size() == 0 )
		return;

	// Get the last block, the product block
	GameFieldBlockProduct* pProdBlock = reinterpret_cast< GameFieldBlockProduct* >( m_SelBlocks.back() );

	// If this block is not comboable then clear the comboable list
	if( !IsBlockComboable( pProdBlock ) )
		m_ComboableProducts.clear();
		
	// Find products around this block
	BlockList surroundingBlocks = GetSurroundingBlocks( pProdBlock );

	// Go through the surrounding blocks
	for( BlockList::iterator iterBlock = surroundingBlocks.begin(); iterBlock != surroundingBlocks.end(); ++iterBlock )
	{
		// Ignore blocks we are going to remove
		if( IsBlockSelected( *iterBlock ) || *iterBlock == pProdBlock )
			continue;

		// If the block is a factor then it can't be comboable
		GameFieldBlockProduct* pAdjacentProdBlock = reinterpret_cast< GameFieldBlockProduct* >( *iterBlock );
		if( !pAdjacentProdBlock->IsProductBlock() )
			continue;

		// If the block is already comboable then do nothing
		if( IsBlockComboable( pAdjacentProdBlock ) )
			continue;

		// Store the block
		m_ComboableProducts.push_back( pAdjacentProdBlock );
	}

	// If this block is comboable then remove it from the list
	MultBlockList::iterator iterBlock;
	if( IsBlockComboable( pProdBlock, &iterBlock ) )
	{
		m_ComboableProducts.erase( iterBlock );
	}
}

bool GameFieldPrimeTime::IsBlockComboable( GameFieldBlockProduct* pBlock, MultBlockList::const_iterator* pIterBlock ) const
{
	for( MultBlockList::const_iterator iterBlock = m_ComboableProducts.begin(); iterBlock != m_ComboableProducts.end(); ++iterBlock )
	{
		if( *iterBlock == pBlock )
		{
			if( pIterBlock )
				*pIterBlock = iterBlock;
			return true;
		}
	}

	return false;
}

bool GameFieldPrimeTime::IsBlockComboable( GameFieldBlockProduct* pBlock, MultBlockList::iterator* pIterBlock )
{
	for( MultBlockList::iterator iterBlock = m_ComboableProducts.begin(); iterBlock != m_ComboableProducts.end(); ++iterBlock )
	{
		if( *iterBlock == pBlock )
		{
			if( pIterBlock )
				*pIterBlock = iterBlock;
			return true;
		}
	}

	return false;
}