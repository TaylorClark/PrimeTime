/*=================================================================================================

	\file GameFieldAdd.cpp
	Game Play Library
	Addition Game Field Source
	\author Taylor Clark
	\Date September 3, 2006

	This source file contains the implementation of the addition version of the game field class.

=================================================================================================*/

#include "../GameFieldAdd.h"
#include "../GameDefines.h"
#include "../GameFieldBlockAdd.h"
#include "Graphics2D/GraphicsMgr.h"
#include "../GameMgr.h"
#include "../GameLogicAdd.h"
#include "Base/TCAssert.h"
#include "../GFISetSum.h"
#include "../GFIAddSummands.h"
#include "GUI/GUIMgr.h"

const float32 GRAVITY_ACCEL = 700.0f;


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::GameFieldAdd  Public
///
///	The default constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldAdd::GameFieldAdd() : m_NumBlocks( 0 ),
								m_pSumBlock( 0 )
{
	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::Init  Public
///
/// Allow the field to run initialization code
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameFieldAdd::Init()
{
	// Get the Y position
	m_YMidPoint = (float32)((FIELD_HEIGHT / 2) * GameDefines::BLOCK_HEIGHT);

	// Get the number of blocks that fit in columns in the upper and lower parts of the field
	m_MaxColBlocks[UPPER_INDEX] = (uint32)( (m_FieldBox.size.y - m_YMidPoint) / GameDefines::BLOCK_HEIGHT );
	m_MaxColBlocks[LOWER_INDEX] = (uint32)( m_YMidPoint / GameDefines::BLOCK_HEIGHT );

	// Get the sum block position
	m_SumBlockPos.x = m_FieldBox.Right() + GameDefines::BLOCK_WIDTH;
	m_SumBlockPos.y = m_FieldBox.pos.y + (m_FieldBox.size.y / 2) - (GameDefines::BLOCK_HEIGHT / 2);
	
	return GameField::Init();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::IsGameOver  Public
///
///	Get if the game is over.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameFieldAdd::IsGameOver() const
{
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::SubclassUpdate  Public
/// \param frameTime The elapsed game time this frame
///
///	Update the game field.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldAdd::SubclassUpdate( float frameTime )
{
	// Go through the columns that need updating
	const float32 fallAccel = GRAVITY_ACCEL * frameTime;
	for( int32 halfIndex = 0; halfIndex < 2; ++halfIndex )
	{
		for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
		{
			// If this column doesn't need updating then skip it
			if( !m_UpdateCols[colIndex] )
				continue;

			// Run through the blocks in this column that push up
			AddBlockList& curCol = m_BlockCols[halfIndex][colIndex];
			for( AddBlockList::iterator iterBlock = curCol.begin(); iterBlock != curCol.end(); ++iterBlock )
			{
				// If this block is not moving then skip it
				if( !(*iterBlock)->isFalling )
					continue;

				// Update the movment amount
				(*iterBlock)->yFallingSpeed += fallAccel;

				// If this block is a faller
				if( halfIndex == UPPER_INDEX )
					(*iterBlock)->yPos -= (*iterBlock)->yFallingSpeed * frameTime;
				// Else it is pushing up
				else
					(*iterBlock)->yPos += (*iterBlock)->yFallingSpeed * frameTime;
			}
		}
	}

	// Check for intra-block collision after they have been moved
	Update_CheckForCollision();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::HandleInstruction  Private
///	\param pInstruction The instruction to be handled
///	\returns True if the instruction was handled, false if it was not
///
/// Handle an instruction
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameFieldAdd::HandleInstruction( GameFieldInstruction* pInstruction )
{
	if( GameField::HandleInstruction(pInstruction) )
		return true;

	// Handle the type of instruction
	switch( pInstruction->GetType() )
	{
	case GFI_SetSum:
		SetSumBlock( ((GFISetSum*)pInstruction)->GetSumValue() );
		break;

	case GFI_AddSummands:
		{
			GFIAddSummands* pAddInst = (GFIAddSummands*)pInstruction;

			// Create the addition blocks
			AddBlockList newBlocks;
			for( std::list<int32>::iterator iterSummand = pAddInst->m_SummandValues.begin();
				iterSummand != pAddInst->m_SummandValues.end();
				++iterSummand )
			{
				GameFieldBlockAdd* pNewBlock = new GameFieldBlockAdd( m_NextBlockID++, *iterSummand );
				newBlocks.push_back( pNewBlock );
			}

			AddBlocks( newBlocks );
		}
		break;

	default:
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::RemoveBlock  Private
///	\param pRemBlock The block to remove
///
///	Remove a block from the field
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldAdd::RemoveBlock( GameFieldBlock* pRemBlock )
{
	TCASSERT( pRemBlock != NULL );
	if( !pRemBlock )
		return;

	// Remove the block from the ID map
	m_BlockIDMap.erase( pRemBlock->GetID() );

	// If the block is selected then remove it from the selected list
	BlockList::iterator iterSelBlock;
	if( IsBlockSelected( pRemBlock, &iterSelBlock ) )
		m_SelBlocks.erase( iterSelBlock );

	// If this is the sum block then bail
	if( pRemBlock == m_pSumBlock )
	{
		delete m_pSumBlock;
		m_pSumBlock = 0;
		return;
	}

	// If the block is in an invalid column then bail
	if( pRemBlock->colIndex < 0 || pRemBlock->colIndex >= FIELD_WIDTH )
	{
		--m_NumBlocks;
		delete pRemBlock;
		return;
	}

	// Get the iterator for the block in the column
	GameFieldBlockAdd* pRemAddBlock = (GameFieldBlockAdd*)pRemBlock;
	int32 halfIndex = UPPER_INDEX;
	AddBlockList::iterator iterBlock = FindBlockInCol( pRemAddBlock, m_BlockCols[UPPER_INDEX][ pRemBlock->colIndex ] );
	if( iterBlock == m_BlockCols[UPPER_INDEX][ pRemBlock->colIndex ].end() )
	{
		halfIndex = LOWER_INDEX;
		iterBlock = FindBlockInCol( pRemAddBlock, m_BlockCols[LOWER_INDEX][ pRemBlock->colIndex ] );
		if( iterBlock == m_BlockCols[LOWER_INDEX][ pRemBlock->colIndex ].end() )
		{
			--m_NumBlocks;
			delete pRemBlock;
			return;
		}
	}

	// Get the block after this one
	AddBlockList::iterator iterNextBlock = iterBlock;
	++iterNextBlock;
	GameFieldBlockAdd* pNextBlock = NULL;
	if( iterNextBlock != m_BlockCols[halfIndex][ pRemBlock->colIndex ].end() )
		pNextBlock = *iterNextBlock;

	// Remove this block from the column
	m_BlockCols[halfIndex][ pRemBlock->colIndex ].erase( iterBlock );
	m_UpdateCols[ pRemBlock->colIndex ] = true; 

	// Update the blocks in this column
	while( iterNextBlock != m_BlockCols[halfIndex][ pRemBlock->colIndex ].end() )
	{
		if( !(*iterNextBlock)->isFalling )
		{
			(*iterNextBlock)->isFalling = true;
			(*iterNextBlock)->yFallingSpeed = 0.0f;
		}
		++iterNextBlock;
	}

	// Free the block
	--m_NumBlocks;
	delete pRemBlock;
}

/// Get the iterator for a block in the column
GameFieldAdd::AddBlockList::iterator GameFieldAdd::FindBlockInCol( GameFieldBlockAdd* pBlock, AddBlockList& blockList )
{
	for( AddBlockList::iterator iterBlock = blockList.begin(); iterBlock != blockList.end(); ++iterBlock )
	{
		if( *iterBlock == pBlock )
			return iterBlock;
	}

	return blockList.end();
}


/// Go through one column moved blocks and check for collision
void GameFieldAdd::Update_CheckForCollision( AddBlockList& blockCol, bool isFalling )
{
	// Go through column starting with the pushing blocks and go from bottom to top
	GameFieldBlockAdd* pPrevBlock = 0;
	float32 stoppedFallingSpeed = 0.0f;
	for( AddBlockList::iterator iterBlock = blockCol.begin(); iterBlock != blockCol.end(); ++iterBlock )
	{
		GameFieldBlockAdd* pCurBlock = *iterBlock;
		float32 blockTop = pCurBlock->yPos + (float32)GameDefines::BLOCK_HEIGHT;
	
		// If the block is moving then check against the midpoint
		if( isFalling )
		{
			if( pCurBlock->isFalling && pCurBlock->yPos < m_YMidPoint )
			{
				pCurBlock->yPos = m_YMidPoint;
				pCurBlock->isFalling = false;
				stoppedFallingSpeed = pCurBlock->yFallingSpeed;
			}
		}
		// Else the block is pushing up
		else
		{
			if( pCurBlock->isFalling && blockTop > m_YMidPoint )
			{
				pCurBlock->yPos = m_YMidPoint - GameDefines::BLOCK_HEIGHT;
				pCurBlock->isFalling = false;
				stoppedFallingSpeed = pCurBlock->yFallingSpeed;
			}
		}
		
		// If there is a previous block (For pushing up this would be above and for falling this
		// would be below)
		if( pPrevBlock )
		{
			if( isFalling )
			{
				// If this block is lower than the top of the block below it then land it on top
				float32 prevBlockTop = pPrevBlock->yPos + (float32)(GameDefines::BLOCK_HEIGHT);
				if( pCurBlock->yPos < prevBlockTop )
				{
					// Make this block sit on top of the block below it
					pCurBlock->yPos = prevBlockTop;

					// Store the speed at which we stopped the block for the sound to play
					stoppedFallingSpeed = pCurBlock->yFallingSpeed;

					// Make this block only fall as fast as the block below it
					pCurBlock->yFallingSpeed = pPrevBlock->yFallingSpeed;

					// If the block below it isn't falling then this block should not be either
					if( !pPrevBlock->isFalling )
						pCurBlock->isFalling = false;
				}
			}
			// Else the block is pushing up
			else
			{
				// If this block is higher than the top of the block below it then land it on top
				if( blockTop > pPrevBlock->yPos )
				{
					// Make this block sit on top of the block below it
					pCurBlock->yPos = pPrevBlock->yPos - (float32)(GameDefines::BLOCK_HEIGHT);

					// Store the speed at which we stopped the block for the sound to play
					stoppedFallingSpeed = pCurBlock->yFallingSpeed;

					// Make this block only fall as fast as the block below it
					pCurBlock->yFallingSpeed = pPrevBlock->yFallingSpeed;

					// If the block above it isn't pushing then this block should not be either
					if( !pPrevBlock->isFalling )
						pCurBlock->isFalling = false;
				}
			}			
		}

		// Store the previous block pointer
		pPrevBlock = *iterBlock;
	}
	
	// If we stopped a falling block then play the sound
	if( stoppedFallingSpeed > 0.0f )
	{
		// Calculate the fall sound volume based on the fall speed
		const float32 FALLING_SPEED_MIN = 200.0f;
		const float32 FALLING_SOUND_MIN = 0.0625f;
		const float32 FALLING_SPEED_MAX = 1200.0f;
		const float32 FALLING_SOUND_MAX = 1.0f;
		
		float fallVolume = FALLING_SOUND_MAX;
		if( stoppedFallingSpeed < FALLING_SPEED_MIN )
			fallVolume = FALLING_SOUND_MIN;
		else if( stoppedFallingSpeed > FALLING_SPEED_MAX )
			fallVolume = FALLING_SOUND_MAX;
		else
		{
			fallVolume = (stoppedFallingSpeed - FALLING_SPEED_MIN) / (FALLING_SPEED_MAX - FALLING_SPEED_MIN);
			fallVolume = FALLING_SOUND_MIN + (FALLING_SOUND_MAX - FALLING_SOUND_MIN) * fallVolume;
		}

		// Play the block-fall sound
		if( m_BlockFallSound.GetObj() )
		{
			SoundInstance fallSound = m_BlockFallSound.GetObj()->Play();
			fallSound.SetVolume( fallVolume );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::Update_CheckForCollision  Private
///
///	Check for intra-block collision in the update function after the blocks have been moved.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldAdd::Update_CheckForCollision()
{
	// Go through the halfs
	for( int32 halfIndex = 0; halfIndex < 2; ++halfIndex )
	{
		// Now that all of the blocks have been moved let's check if any of the falling blocks landed
		for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
		{
			// If this column doesn't need updating then skip it
			if( !m_UpdateCols[colIndex] )
				continue;
			Update_CheckForCollision( m_BlockCols[halfIndex][ colIndex ], halfIndex == UPPER_INDEX );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::GetBlockAtPos  Public
///	\param fieldPos The position to test for a block in field coordinates
///	\returns A pointer to the block at the passed in position or NULL if there none
///
///	Find a block at a position in the field.
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlock* GameFieldAdd::GetBlockAtPos( const Point2i& fieldPos )
{
	// See if the sum block was clicked on
	if( m_pSumBlock )
	{
		int32 sumBlockWidth = GameDefines::BLOCK_WIDTH;
		if( m_pSumBlock->GetValue() > 99 )
			sumBlockWidth = GameDefines::BLOCK_WIDTH * 2;

		Box2i sumBlockRect( ScreenPosToBlockField( m_SumBlockPos ), Vector2i( sumBlockWidth, -GameDefines::BLOCK_HEIGHT ) );
		if( sumBlockRect.IsPointInside( fieldPos ) )
			return m_pSumBlock;
	}

	if( fieldPos.x < 0 || fieldPos.x > (FIELD_WIDTH * GameDefines::BLOCK_WIDTH)
		|| fieldPos.y < 0 || fieldPos.y > (FIELD_HEIGHT * GameDefines::BLOCK_WIDTH))
		return NULL;

	// Get the column clicked on
	int clickedColIndex = fieldPos.x / GameDefines::BLOCK_WIDTH;
	if( clickedColIndex < 0 || clickedColIndex >= FIELD_WIDTH )
		return NULL;

	// If the position is above the mid-point
	int32 halfIndex = LOWER_INDEX;
	if( fieldPos.y > m_YMidPoint )
		halfIndex = UPPER_INDEX;

	// Get the column list
	AddBlockList& blockCol = m_BlockCols[ halfIndex ][ clickedColIndex ];

	// Go through blocks
	float32 floatMouseY = (float32)fieldPos.y;
	for( AddBlockList::iterator iterBlock = blockCol.begin(); iterBlock != blockCol.end(); ++iterBlock )
	{
		// If the block's bottom is above the mouse then no need to continue checking
		if( floatMouseY > (*iterBlock)->yPos && floatMouseY < (*iterBlock)->yPos + GameDefines::BLOCK_HEIGHT )
			return *iterBlock;
	}

	// There was no block at this position
	return NULL;
}


/// Draw the game
void GameFieldAdd::Draw() const
{
	// Get the number font
	TCFont* pBlockFont = GameDefines::GetBlockTextFont().GetObj();

	// Get the block sprites
	RefSprite* pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK ).GetObj();
	if( !pBlockSprite )
		return;
	
	RefSpriteHndl hilitedBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK_HILITED );
	if( !hilitedBlockSprite.GetObj() )
		return;
	RefSprite* pHilitedBlockSprite = hilitedBlockSprite.GetObj();

	// Get the cursor position so we can hi-lite blocks
	Point2i cursorScreenPos = GUIMgr::Get().GetCursorPos();

	// Go through the upper and lower half
	for( int32 halfIndex = 0; halfIndex < 2; ++halfIndex )
	{
		// Go through the columns
		for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )	
		{
			// Get the column list
			const AddBlockList& blockCol = m_BlockCols[ halfIndex ][ colIndex ];

			// Go through blocks
			for( AddBlockList::const_iterator iterBlock = blockCol.begin(); iterBlock != blockCol.end(); ++iterBlock )
			{
				// Get the block's position
				GameFieldBlockAdd* pBlock = *iterBlock;
				
				// If this block is coming from the top
				if( halfIndex == UPPER_INDEX )
				{
					if( pBlock->yPos > (float32)m_FieldBox.size.y )
						continue;
				}
				// Else the block is being pushed from the bottom
				else
				{
					if( pBlock->yPos < (float32)-GameDefines::BLOCK_HEIGHT )
						continue;
				}

				// Get the position from game field coordinates to screen coordinates
				Point2i blockPos = BlockFieldToScreenPos( colIndex, pBlock->yPos, 1 );

				// If this block is coming from the top and it is off the top
				if( halfIndex == UPPER_INDEX && pBlock->yPos > (m_FieldBox.size.y - GameDefines::BLOCK_HEIGHT) )
				{
					Box2i srcRect = pBlockSprite->GetSrcRect();
					srcRect.size.y = m_FieldBox.size.y - (int32)pBlock->yPos;
					srcRect.pos.y += GameDefines::BLOCK_HEIGHT - srcRect.size.y;
					Box2i destRect( blockPos.x, m_FieldBox.pos.y, GameDefines::BLOCK_WIDTH, srcRect.size.y );
					g_pGraphicsMgr->DrawImage( destRect.pos, pBlockSprite->GetImage(), srcRect );

					// If the block has text then draw it
					Box2i clipRect( blockPos + pBlock->GetTextOffset(), Vector2i(GameDefines::BLOCK_WIDTH, pBlockFont->GetCharHeight()) );
					Point2i destPos = clipRect.pos;
					if( clipRect.pos.y < m_FieldBox.pos.y )
					{
						clipRect.size.y -= m_FieldBox.pos.y - clipRect.pos.y;
						clipRect.pos.y = m_FieldBox.pos.y;
					}
					g_pGraphicsMgr->DrawFontTextClipped( pBlockFont, pBlock->GetText(), destPos, clipRect );
				}
				// Else if this block is pushing up and is off the bottom
				else if( halfIndex == LOWER_INDEX && pBlock->yPos < 0.0f )
				{
					Box2i destRect( blockPos.x, blockPos.y, GameDefines::BLOCK_WIDTH, GameDefines::BLOCK_HEIGHT + (int32)pBlock->yPos );
					g_pGraphicsMgr->DrawSpriteUnscaled( destRect, pBlockSprite );

					// If the block has text then draw it
					Box2i clipRect( blockPos + pBlock->GetTextOffset(), Vector2i(GameDefines::BLOCK_WIDTH, pBlockFont->GetCharHeight()) );
					if( clipRect.Bottom() > m_FieldBox.Bottom() )
						clipRect.size.y = m_FieldBox.Bottom() - clipRect.pos.y;
					g_pGraphicsMgr->DrawFontTextClipped( pBlockFont, pBlock->GetText(), clipRect );
				}
				// Else draw the block normally
				else
				{
					// If the cursor is in the block
					if( Box2i( blockPos, Vector2i(GameDefines::BLOCK_WIDTH, GameDefines::BLOCK_HEIGHT)).IsPointInside( cursorScreenPos ) )
						g_pGraphicsMgr->DrawSprite( blockPos, pHilitedBlockSprite );
					else
						g_pGraphicsMgr->DrawSprite( blockPos, pBlockSprite );

					g_pGraphicsMgr->DrawFontText( pBlockFont, pBlock->GetText(), blockPos + pBlock->GetTextOffset() );
				}
			}
		}
	}

	// Draw the selection frames
	if( m_SelBracketSprite.GetImage() )
	{
		for( BlockList::const_iterator iterBlock = m_SelBlocks.begin(); iterBlock != m_SelBlocks.end(); ++iterBlock )
		{
			const GameFieldBlockAdd* pBlock = (GameFieldBlockAdd*)(*iterBlock);

			// Get the position from game field coordinates to screen coordinates
			Point2i blockPos = BlockFieldToScreenPos( pBlock->colIndex, pBlock->yPos, 1 );

			// If this block is off the bottom
			if( pBlock->yPos < 0.0f )
			{
				Box2i destRect( blockPos.x, blockPos.y, GameDefines::BLOCK_WIDTH, GameDefines::BLOCK_HEIGHT + (int32)pBlock->yPos );
				Box2i srcRect( m_SelBracketSprite.GetSrcCoords() );
				srcRect.size.y = destRect.size.y;
				g_pGraphicsMgr->DrawImage( destRect.pos, m_SelBracketSprite.GetImage(), srcRect );
			}
			// Else if the block is off the top
			else if( pBlock->yPos > m_FieldBox.size.y - GameDefines::BLOCK_HEIGHT )
			{
				Box2i destRect( blockPos.x, m_FieldBox.pos.y, GameDefines::BLOCK_WIDTH, m_FieldBox.size.y - (int32)pBlock->yPos );
				Box2i srcRect( m_SelBracketSprite.GetSrcCoords() );
				srcRect.pos.y += GameDefines::BLOCK_HEIGHT - destRect.size.y;
				srcRect.size.y = destRect.size.y;
				g_pGraphicsMgr->DrawImage( destRect.pos, m_SelBracketSprite.GetImage(), srcRect );
			}
			// Else just draw the cursor
			else
				g_pGraphicsMgr->DrawImage( blockPos, m_SelBracketSprite.GetImage(), m_SelBracketSprite.GetSrcCoords() );
		}
	}

	// If there is a sum block
	if( m_pSumBlock )
	{
		// If the cursor is in the block
		bool useHilite = false;
		int32 sumBlockWidth = (m_pSumBlock->GetValue() > 99 ? 2 : 1) * GameDefines::BLOCK_WIDTH;
		Box2i sumBlockRect( ScreenPosToBlockField( m_SumBlockPos ), Vector2i( sumBlockWidth, -GameDefines::BLOCK_HEIGHT ) );
		if( sumBlockRect.IsPointInside( ScreenPosToBlockField(cursorScreenPos) ) )
			useHilite = true;

		RefSprite* pSumBlockSprite = 0;
		if( m_pSumBlock->GetValue() > 99 )
		{
			if( useHilite )
				pSumBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DBL_PRDCT_BLOCK_HILITED ).GetObj();
			else
				pSumBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DOUBLE_PRODUCT_BLOCK ).GetObj();
		}
		else
		{
			if( useHilite )
				pSumBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK_HILITED ).GetObj();
			else
				pSumBlockSprite = pBlockSprite;
		}

		g_pGraphicsMgr->DrawSprite( m_SumBlockPos, pSumBlockSprite );

		g_pGraphicsMgr->DrawFontText( pBlockFont, m_pSumBlock->GetText(), m_SumBlockPos + m_pSumBlock->GetTextOffset() );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::ClearField  Public
///
///	Clear the game field blocks and free any associated memory
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldAdd::ClearField()
{
	// Go through the blocks
	for( int32 halfIndex = 0; halfIndex < 2; ++halfIndex )
	{
		for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )	
		{
			// Get the column list
			AddBlockList& blockCol = m_BlockCols[ halfIndex ][ colIndex ];

			// Go through blocks
			for( AddBlockList::iterator iterBlock = blockCol.begin(); iterBlock != blockCol.end(); ++iterBlock )
				delete *iterBlock;

			// Clear the list
			blockCol.clear();
		}
	}

	// Clear the remaining data
	m_SelBlocks.clear();
	if( m_pSumBlock )
		delete m_pSumBlock;
	m_pSumBlock = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::SetSumBlock  Public
///	\param sumValue The value to use for the sum block
///
///	Set the sum block.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldAdd::SetSumBlock( int32 sumValue )
{
	// Create the new sum block
	GameFieldBlockAdd* pBlock = new GameFieldBlockAdd( m_NextBlockID++, sumValue, true );

	// Remove the current sum block, if any
	if( m_pSumBlock )
		RemoveBlock( m_pSumBlock );

	// Store the new block
	m_pSumBlock = pBlock;
	m_BlockIDMap.insert( std::pair<uint32,GameFieldBlock*>(m_pSumBlock->GetID(),m_pSumBlock) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldAdd::AddBlocks  Public
///	\param blocks The blocks to add to the field in no particular order.
///
///	Add blocks to the game field.  The memory passed in to this function is handled by the field.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldAdd::AddBlocks( const AddBlockList& blocks )
{
	if( blocks.size() == 0 )
		return;

	// Go through the blocks to add
	AddBlockList::const_iterator iterBlock = blocks.begin();
	for( int32 halfIndex = 0; halfIndex < 2; ++halfIndex )
	{
		for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )	
		{
			// Get the column list
			AddBlockList& blockCol = m_BlockCols[ halfIndex ][ colIndex ];
			
			// Add until the column is full
			while( blockCol.size() < m_MaxColBlocks[halfIndex] )
			{
				// Add the next block to this column
				GameFieldBlockAdd* pNewBlock = *iterBlock;
				pNewBlock->isFalling = true;
				pNewBlock->yFallingSpeed = 0.0f;
				pNewBlock->colIndex = colIndex;
				if( halfIndex == UPPER_INDEX )
					pNewBlock->yPos = (float32)m_FieldBox.size.y;
				else
					pNewBlock->yPos = -GameDefines::BLOCK_HEIGHT;

				// Ensure the block is added to the edge
				if( blockCol.size() > 0 )
				{
					GameFieldBlockAdd* pLastBlock = blockCol.back();

					if( halfIndex == UPPER_INDEX )
					{
						if( (pLastBlock->yPos + GameDefines::BLOCK_HEIGHT) > pNewBlock->yPos )
							pNewBlock->yPos = (pLastBlock->yPos + GameDefines::BLOCK_HEIGHT);
					}
					else
					{
						if( pLastBlock->yPos < 0 )
							pNewBlock->yPos = (pLastBlock->yPos - GameDefines::BLOCK_HEIGHT);
					}
				}

				// Add the block
				blockCol.push_back( pNewBlock );
				m_UpdateCols[ colIndex ] = true;
				++m_NumBlocks;

				// Store the block in the ID map
				m_BlockIDMap.insert( std::pair<uint32,GameFieldBlock*>(pNewBlock->GetID(),pNewBlock) );

				// Step to the next block
				++iterBlock;
				if( iterBlock == blocks.end() )
					goto loop_end;
			}
		}
	}
loop_end:

	// If there are still blocks left to add then free them
	while( iterBlock != blocks.end() ) 
	{
		delete *iterBlock;
		++iterBlock;
	}
}