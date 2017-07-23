/*=================================================================================================

	\file GameFieldMult.cpp
	Game Play Library
	Game Field Source
	\author Taylor Clark
	\Date May 12, 2006

	This source file contains the implementation of the multiplication version of the game field
	class.

=================================================================================================*/

#include "../GameFieldMultBase.h"
#include "../GameFieldBlock.h"
#include "../GameDefines.h"
#include "Graphics2D/GraphicsMgr.h"
#include "GUI/GUIMgr.h"
#include "Base/MsgLogger.h"
#include "Base/TCAssert.h"
#include "Base/NumFuncs.h"
#include <math.h>
#include "Graphics2D/GraphicsDefines.h"
#include "../GameFieldBlockMultBase.h"
#include "../GameFieldBlockProduct.h"
#include "../GameMgr.h"
#include "../GameLogic.h"
#include "../PIISetPushSpeed.h"

const float32 GRAVITY_ACCEL = 700.0f;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::GetTopMostY  Public
///
///	Get the Y positition of the top of the top-most non-falling block.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
float32 GameFieldMultBase::GetTopMostY() const
{
	// Get the top-most block
	float32 topMostY = m_ColumnHeight[0];
	for( uint32 colIndex = 1; colIndex < FIELD_WIDTH; ++colIndex )
	{
		// Is this block above the current top
		if( m_ColumnHeight[colIndex] > topMostY )
			topMostY = m_ColumnHeight[colIndex];
	}

	// Return the top-most Y position
	return topMostY;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::DropNewBlock  Public
///
///	\param pNewBlock The block to drop from the top
///
///	Get the danger level for the field.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
float32 GameFieldMultBase::GetDangerLevel() const
{
	// Get the top-most block
	float32 topMostY = GetTopMostY();

	// Get the percentage up the field the blocks are
	float32 amountUp = topMostY / ((float32)m_FieldBox.size.y * 0.9f);
	if( amountUp > 1.0f )
		amountUp = 1.0f;

	// If the blocks are below a certain point then the player is in no danger
	const float32 NO_DANGER_THRESHOLD = 0.75f;
	if( amountUp < NO_DANGER_THRESHOLD )
		return 0.0f;

	// Scale the remaining amount
	amountUp = (amountUp - NO_DANGER_THRESHOLD) * (1.0f / (1.0f - NO_DANGER_THRESHOLD));
	return amountUp;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::DropNewBlock  Public
///
///	\param pNewBlock The block to drop from the top
///
///	Add a new block to be dropped from the top
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::DropNewBlock( GameFieldBlockMultBase* pNewBlock )
{
	TCASSERT( pNewBlock );
	if( !pNewBlock )
		return;

	// Ensure a valid position
	int endColIndex = pNewBlock->colIndex + pNewBlock->size.x;
	if( endColIndex < 0 || endColIndex > FIELD_WIDTH )
	{
		TCBREAKX( L"Dropping new block that exceeds the right of the game field" );
		return;
	}

	// Set the block's data
	pNewBlock->yPos = (float32)m_FieldBox.size.y;
	pNewBlock->yFallingSpeed = 0.0f;
	pNewBlock->isFalling = true;

	// In debug mode let's check to see if the block is already known
#ifdef _DEBUG
	for( MultBlockList::iterator iterBlock = m_ActiveBlocks.begin(); iterBlock != m_ActiveBlocks.end(); ++iterBlock )
	{
		// If thi
		if( pNewBlock == *iterBlock )
			MsgLogger::Get().Output( L"Dropping new block that is already in active block list" );
	}
#endif

	// Add it to the unique list
	m_ActiveBlocks.push_back( pNewBlock );

	// Store the block in the ID map
	m_BlockIDMap.insert( std::pair<uint32,GameFieldBlock*>(pNewBlock->GetID(),pNewBlock) );

	// Add the block to each column it is in.
	for( int colIndex = pNewBlock->colIndex; colIndex < endColIndex; ++colIndex )
		m_Cols[colIndex].push_back( pNewBlock );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::DropNewBlocks  Public
///
///	\param ppNewBlocks The block to drop from the top
///
///	Add a new row of blocks to be dropped from the top
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::DropNewBlocks( GameFieldBlockMultBase** ppNewBlocks )
{
	TCASSERT( ppNewBlocks );
	if( !ppNewBlocks )
		return;

	// Go through each block
	int32 colIndex = 0;
	for( int blockIndex = 0; ppNewBlocks[ blockIndex ] != 0 && colIndex < FIELD_WIDTH; ++blockIndex )
	{
		// Get the block
		GameFieldBlockMultBase* pCurBlock = ppNewBlocks[ blockIndex ];
		
		// Set the block's data
		pCurBlock->yPos = (float32)m_FieldBox.size.y;
		pCurBlock->yFallingSpeed = 0.0f;
		pCurBlock->isFalling = true;

		// Add it to the unique list
		m_ActiveBlocks.push_back( pCurBlock );

		// Store the block in the ID map
		m_BlockIDMap.insert( std::pair<uint32,GameFieldBlock*>(pCurBlock->GetID(),pCurBlock) );

		// Add the block to each column it is in
		int32 endColIndex = colIndex + pCurBlock->size.x;
		for( int curColIndex = colIndex; curColIndex < endColIndex; ++curColIndex )
			m_Cols[curColIndex].push_back( pCurBlock );

		// Advance the column index
		colIndex += pCurBlock->size.x;
	}	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldMultBase::PushNewBlocks  Public
///	\param ppNewBlocks An array of block pointers that represent ths blocks to drop down, the
///						array is read from until it reads enough blocks to fill up a row
///
///	Add new blocks to be pushed up from the bottom.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::PushNewBlocks( GameFieldBlockMultBase** ppNewBlocks )
{
	// If this is debug mode then ensure there are enough blocks to make up a full row and no
	// NULL blocks
#ifdef _DEBUG
	int32 tempWidth = 0;
	for( uint32 blockIndex = 0; ppNewBlocks[blockIndex] != NULL && tempWidth != FIELD_WIDTH; ++blockIndex )
	{
		GameFieldBlockMultBase* pCurBlock = ppNewBlocks[ blockIndex ];
		TCASSERTX( pCurBlock != NULL, L"NULL block in list of blocks to be pushed up." );
		tempWidth += pCurBlock->size.x;
	}
	TCASSERTX( tempWidth == FIELD_WIDTH, L"Row of blocks to be pushed up does not fill an entire row." );
#endif

	// Get the bottom position for the row
	float32 newRowYPos = (float32)-GameDefines::BLOCK_HEIGHT;
	for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
	{
		// If the list is empty then skip it
		if( m_Cols[ colIndex ].size() == 0 )
			continue;

		// Get the bottom block on this list
		if( m_Cols[ colIndex ].front()->yPos < 0.0f )
		{
			newRowYPos = m_Cols[ colIndex ].front()->yPos - (float32)GameDefines::BLOCK_HEIGHT;
			break;
		}
	}
	m_LowestY = newRowYPos;

	// Add the blocks to the columns they intersect
	for( uint32 curBlockIndex = 0; ppNewBlocks[ curBlockIndex ] != NULL; ++curBlockIndex )
	{
		// Get a pointer to the current block for ease of coding
		GameFieldBlockMultBase* pCurBlock = ppNewBlocks[ curBlockIndex ];

		// In debug mode, check if the block is already known
#ifdef _DEBUG
		for( MultBlockList::iterator iterBlock = m_ActiveBlocks.begin(); iterBlock != m_ActiveBlocks.end(); ++iterBlock )
			TCASSERTX( pCurBlock != *iterBlock, L"Pushing new block that is already in active block list" );
#endif

		// Set the block's data
		pCurBlock->yPos = newRowYPos;
		pCurBlock->yFallingSpeed = 0.0f;
		pCurBlock->isFalling = false;

		// Add the block to the active list
		m_ActiveBlocks.push_back( (GameFieldBlockMultBase*)pCurBlock );

		// Store the block in the ID map
		m_BlockIDMap.insert( std::pair<uint32,GameFieldBlock*>(pCurBlock->GetID(),pCurBlock) );

		// Add the block to the columns it is in
		for( int colIndex = 0; colIndex < pCurBlock->size.x; ++colIndex )
			m_Cols[ pCurBlock->colIndex + colIndex ].push_front( pCurBlock );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::GetBlockAtPos  Public
///
///	\param fieldPos The position to test for a block in field coordinates
///	\returns A pointer to the block at the passed in position or NULL if there none
///
///	Find a block at a position in the field.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlock* GameFieldMultBase::GetBlockAtPos( const Point2i& fieldPos )
{
	if( fieldPos.x < 0 || fieldPos.x > (FIELD_WIDTH * GameDefines::BLOCK_WIDTH)
		|| fieldPos.y < 0 || fieldPos.y > (FIELD_HEIGHT * GameDefines::BLOCK_WIDTH))
		return NULL;

	// Get the column clicked on
	int clickedColIndex = fieldPos.x / GameDefines::BLOCK_WIDTH;
	if( clickedColIndex < 0 || clickedColIndex >= FIELD_WIDTH )
		return NULL;

	// Get the column list
	MultBlockList& blockCol = m_Cols[ clickedColIndex ];

	// Go through blocks
	float32 floatMouseY = (float32)fieldPos.y;
	for( MultBlockList::iterator iterBlock = blockCol.begin(); iterBlock != blockCol.end(); ++iterBlock )
	{
		// If the block's bottom is above the mouse then no need to continue checking
		if( (*iterBlock)->yPos > floatMouseY )
			return NULL;

		// If the top of the box is above the mouse then the mouse is in the box
		float32 blockTopY = ((*iterBlock)->yPos + (GameDefines::BLOCK_HEIGHT * (*iterBlock)->size.y));
		if( blockTopY > floatMouseY )
			return *iterBlock;
	}

	// There was no block at this position
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::~GameFieldMult  Public
///
///	The destructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldMultBase::~GameFieldMultBase()
{
	ClearField();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::FindBlockInCol  Private
///
///	\param pBlock The block that is to be found
///	\param blockCol The column block list to search within
///	\returns An iterator of the block in the list or the column list end iterator if the block
///				could not be found in the column.
///
///	Get the iterator of a block within a column.  This function must be used with care as
///	iterators can become invalid if list contents change.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MultBlockList::iterator GameFieldMultBase::FindBlockInCol( GameFieldBlockMultBase* pBlock, MultBlockList& blockCol )
{
	if( !pBlock )
		return blockCol.end();
	
	// Go through the blocks in this column
	for( MultBlockList::iterator iterBlock = blockCol.begin(); iterBlock != blockCol.end(); ++iterBlock )
	{
		// If this is our block
		if( *iterBlock == pBlock )
			return iterBlock;
	}

	// There was no block so return the end iterator
	return blockCol.end();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::Recurse_UpdateFalling  Private
///
///	\param pBlock The block that is to be tested for falling
///
///	Make a block drop if possible.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::Recurse_UpdateFalling( GameFieldBlockMultBase* pBlock )
{
	// If the block is NULL or it is already falling
	if( !pBlock || pBlock->isFalling )
		return;

	// Go the width of the block and check for blocks under it
	bool existsSupportingBlock = false;
	for( int32 colIndex = pBlock->colIndex; colIndex < pBlock->colIndex + pBlock->size.x; ++colIndex )
	{
		MultBlockList& curCol = m_Cols[ colIndex ];

		// Find the block in this column
		MultBlockList::iterator iterBlock = FindBlockInCol( pBlock, curCol );
		if( iterBlock == curCol.end() )
		{
			MsgLogger::Get().Output( MsgLogger::MI_Error, L"Block not found in column list that it intersects." );
			continue;
		}

		// If there is no block below this one
		if( iterBlock == curCol.begin() )
			continue;

		// Get the block below this one
		MultBlockList::iterator belowBlock = iterBlock;
		--belowBlock;
		GameFieldBlockMultBase* pBelowBlock = *belowBlock;

		// If this block is falling then it can't support this block
		if( pBelowBlock->isFalling )
			continue;

		// Get the top of this block
		float32 belowBlockTopY = pBelowBlock->yPos + (float32)(pBelowBlock->size.y * GameDefines::BLOCK_HEIGHT);

		// If this block is sitting on the one below it then bail
		if( TCBase::NearlyEquals<float32>( pBlock->yPos, belowBlockTopY, 1.0f )
			|| pBlock->yPos < belowBlockTopY )
		{
			// If the block below this is not falling, then it is supporting this block
			if( !pBelowBlock->isFalling )
			{
				existsSupportingBlock = true;
				break;
			}
		}
	}

	// If the test block is sitting on another block then the test block can't fall so bail
	if( existsSupportingBlock )
		return;

	// Allow this block to drop
	pBlock->isFalling = true;
	pBlock->yFallingSpeed = 0.0f;

	// Go the width of the block and update blocks above it
	float32 thisBlockTopY = pBlock->yPos + (float32)(pBlock->size.y * GameDefines::BLOCK_HEIGHT);
	for( int32 colIndex = pBlock->colIndex; colIndex < pBlock->colIndex + pBlock->size.x; ++colIndex )
	{
		MultBlockList& curCol = m_Cols[ colIndex ];

		// Find the block in this column
		MultBlockList::iterator iterBlock = FindBlockInCol( pBlock, curCol );
		if( iterBlock == curCol.end() )
			continue;

		// Get the block above this block
		++iterBlock;
		if( iterBlock == curCol.end() )
			continue;

		// If this block is not sitting on top of our block then don't update it
		if( !TCBase::NearlyEquals<float32>( thisBlockTopY, (*iterBlock)->yPos, 1.0f ) )
			continue;

		// Update the block
		Recurse_UpdateFalling( *iterBlock );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::ClearField  Public
///
///	Clear all of the blocks from the field.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::ClearField()
{
	// Clear all block related data
	for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
		m_Cols[colIndex].clear();

	// Free the blocks
	for( MultBlockList::iterator iterBlock = m_ActiveBlocks.begin(); iterBlock != m_ActiveBlocks.end(); ++iterBlock )
		delete *iterBlock;
	m_ActiveBlocks.clear();
	m_BlockIDMap.clear();

	m_LowestY = 0.0f;
	m_SelBlocks.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::SubclassUpdate  Private
///
///	\param frameTime The elapsed frame time in seconds
///
///	Update the game field by pushing blocks up, dropping blocks, and checking for collision.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::SubclassUpdate( float frameTime )
{
	if( GameMgr::Get().IsPaused() )
		return;

	static Vector2i s_FieldShakeOffset;

	// Clear the current shake offset
	m_FieldBox.pos -= s_FieldShakeOffset;
	s_FieldShakeOffset.Set(0, 0);

	// Update the time since the last shake was applied
	m_LastShakeTime += frameTime;

	// If the field is high enough to shake
	const float32 DANGER_SHAKE_START = 0.6f;
	float32 shakeAmount = (GetDangerLevel() - DANGER_SHAKE_START) / (1.0f - DANGER_SHAKE_START);
	if( shakeAmount > 0.0f )
	{
		// If it is time to shake the field
		const float32 MIN_SHAKE_INTERVAL = 0.015625;
		float32 shakeInterval = (1.0f - shakeAmount) + MIN_SHAKE_INTERVAL;
		if( m_LastShakeTime > shakeInterval )
		{
			s_FieldShakeOffset.Set( (rand() % 3) - 1, (rand() % 3) - 1 );
			m_FieldBox.pos += s_FieldShakeOffset;
			m_LastShakeTime = 0.0f;
		}
	}

	// Get the amount to move the tiles
	float32 moveYAmt = m_PushUpSpeed * frameTime;

	// Loop through the columns and determine if the block should be pushed up less and the
	// height of each column
	bool isPushingUp = false;
	for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
	{
		// If the list is empty then skip it
		if( m_Cols[ colIndex ].size() == 0 )
		{
			m_ColumnHeight[colIndex] = 0.0f;
			continue;
		}

		// Get the bottom block on this list
		if( !isPushingUp && m_Cols[ colIndex ].front()->yPos < 0.0f )
		{
			isPushingUp = true;

			// Change the amount to move up to have the bottom block aligned with the bottom
			if( moveYAmt > -m_Cols[ colIndex ].front()->yPos )
				moveYAmt = -m_Cols[ colIndex ].front()->yPos;
		}

		// Get the height of this column
		for( MultBlockList::reverse_iterator iterBlock = m_Cols[colIndex].rbegin(); iterBlock != m_Cols[colIndex].rend(); ++iterBlock )
		{
			// If the block is falling then don't use it
			if( (*iterBlock)->isFalling )
				continue;
			
			// Get the top block
			m_ColumnHeight[colIndex] = (*iterBlock)->yPos + (float32)((*iterBlock)->size.y * GameDefines::BLOCK_HEIGHT);
			 
			// The top block has been found
			break;
		}
	}

	// If the blocks are not being pushed up and there are no blocks then ensure the lowest Y gets
	// updated so added blocks display correctly
	if( !isPushingUp && m_ActiveBlocks.size() == 0 )
		isPushingUp = true;

	// If blocks are pushing up then update the lowest Y position
	if( isPushingUp )
		m_LowestY += moveYAmt;

	// Go through the blocks and move all of the blocks that aren't falling up and the ones that
	// are falliing down
	float fallAccell = GRAVITY_ACCEL * frameTime;
	for( MultBlockList::iterator iterBlock = m_ActiveBlocks.begin(); iterBlock != m_ActiveBlocks.end(); ++iterBlock )
	{
		GameFieldBlockMultBase* pBlock = *iterBlock;

		// If this block is falling
		if( pBlock->isFalling )
		{
			// Update the falling speed
			pBlock->yFallingSpeed += fallAccell;

			// Move this block down
			pBlock->yPos -= pBlock->yFallingSpeed * frameTime;
		}
		// Else this block is not falling
		else
		{
			// If this block is above the top then flag the end
			float32 blockTopY = pBlock->yPos + (pBlock->size.y * GameDefines::BLOCK_HEIGHT);
			if( blockTopY > m_FieldBox.size.y )
				m_IsFieldDone = true;

			// If the blocks are pushing blocks up and this block is under the bottom
			if( isPushingUp && pBlock->yPos < 0.0f )
				pBlock->yPos += moveYAmt;
		}
	}

	// Check for inter-block collision after they have been moved
	Update_CheckForCollision();	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldMultBase::HandleInstruction  Private
///	\param pInstruction The instruction to be handled
///	\returns True if the instruction was handled, false if it was not
///
/// Handle an instruction
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameFieldMultBase::HandleInstruction( GameFieldInstruction* pInstruction )
{
	if( GameField::HandleInstruction(pInstruction) )
		return true;

	// Handle the type of instruction
	switch( pInstruction->GetType() )
	{
	case GFI_SetPushSpeed:
		{
			PIISetPushSpeed* pSpeedInst = (PIISetPushSpeed*)pInstruction;
			pSpeedInst->OldPushSpeed = m_PushUpSpeed;
			m_PushUpSpeed = pSpeedInst->GetNewPushSpeed();
		}
		break;

	default:
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::Update_CheckForCollision_Column  Private
///
/// Check for block collision in a specfic column
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::Update_CheckForCollision_Column( GameFieldBlockMultBase* pFirstBlock, int32 colIndex )
{
	MultBlockList& curCol = m_Cols[ colIndex ];
	GameFieldBlockMultBase* pPrevBlock = 0;
	bool alreadyPlayedForColumn = false;

	MultBlockList::iterator iterBlock = FindBlockInCol( pFirstBlock, curCol );
	for( ; iterBlock != curCol.end(); ++iterBlock )
	{
		GameFieldBlockMultBase* pCurBlock = *iterBlock;

		float stoppedFallingSpeed = 0.0f;

		// If it is falling then check against the bottom
		if( pCurBlock->isFalling && pCurBlock->yPos < 0.0f )
		{
			pCurBlock->yPos = 0.0f;
			pCurBlock->isFalling = false;
			stoppedFallingSpeed = pCurBlock->yFallingSpeed;
		}
		
		// If there is a block below this one then check it
		if( pPrevBlock )
		{
			// If this block is lower than the top of the block below it then land it on top
			float32 prevBlockTop = pPrevBlock->yPos + (float32)(GameDefines::BLOCK_HEIGHT * pPrevBlock->size.y);
			if( pCurBlock->yPos < prevBlockTop )
			{
				// Make this block sit on top of the block below it
				pCurBlock->yPos = prevBlockTop;

				// If this block is more than 1 tile wide and this column is not the left-most tile
				if( pCurBlock->size.x > 1 && pCurBlock->colIndex != colIndex && colIndex > 0 )
				{
					// Update the previous column
					Update_CheckForCollision_Column( pCurBlock, colIndex - 1 );
				}

				// Store the speed at which we stopped the block for the sound to play
				stoppedFallingSpeed = pCurBlock->yFallingSpeed;

				// Make this block only fall as fast as the block below it
				pCurBlock->yFallingSpeed = pPrevBlock->yFallingSpeed;

				// If the block below it isn't falling then this block should not be either
				if( !pPrevBlock->isFalling )
					pCurBlock->isFalling = false;
			}
		}

		// If we stopped a falling block then play the sound
		if( stoppedFallingSpeed > 0.0f && !alreadyPlayedForColumn )
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
			alreadyPlayedForColumn = true;
		}

		// Store the previous block pointer
		pPrevBlock = *iterBlock;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::Update_CheckForCollision  Private
///
///	Check for intra-block collision in the update function after the blocks have been moved.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::Update_CheckForCollision()
{
	// Now that all of the blocks have been moved let's check if any of the falling blocks landed
	for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
	{
		if( m_Cols[ colIndex ].size() == 0 )
			continue;
		Update_CheckForCollision_Column( *(m_Cols[ colIndex ].begin()), colIndex );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldMultBase::RemoveBlock  Private
///
///	\param pRemBlock The block to remove
///
///	Remove a block from the game field.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::RemoveBlock( GameFieldBlock* pRemBaseBlock )
{
	TCASSERT( pRemBaseBlock != NULL );
	GameFieldBlockMultBase* pRemBlock = (GameFieldBlockMultBase*)pRemBaseBlock;

	// Remove the block from active list
	m_ActiveBlocks.remove( pRemBlock );
	m_BlockIDMap.erase( pRemBlock->GetID() );

	// If the block is selected then remove it from the selected list
	BlockList::iterator iterSelBlock;
	if( IsBlockSelected( pRemBlock, &iterSelBlock ) )
		m_SelBlocks.erase( iterSelBlock );

	// If the block is in an invalid column then bail
	if( pRemBlock->colIndex < 0 || pRemBlock->colIndex + pRemBlock->size.x > FIELD_WIDTH )
	{
		delete pRemBlock;
		return;
	}

	// Go through each column this block intersects
	for( int32 colIndex = pRemBlock->colIndex; colIndex < pRemBlock->colIndex + pRemBlock->size.x; ++colIndex )
	{
		// Get the iterator for the block in the column
		MultBlockList::iterator iterBlock = FindBlockInCol( pRemBlock, m_Cols[ colIndex ] );
		if( iterBlock == m_Cols[ colIndex ].end() )
			continue;

		// Get the block above this one
		MultBlockList::iterator iterAboveBlock = iterBlock;
		++iterAboveBlock;
		GameFieldBlockMultBase* pAboveBlock = NULL;
		if( iterAboveBlock != m_Cols[ colIndex ].end() )
			pAboveBlock = *iterAboveBlock;

		// Remove this block from the column
		m_Cols[ colIndex ].erase( iterBlock );

		// Update the falling
		Recurse_UpdateFalling( pAboveBlock );
	}

	// Free the block
	delete pRemBlock;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldMultBase::Draw  Public
///
///	Draw the game field.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameFieldMultBase::Draw() const
{
	TCFont* pBlockFont = GameDefines::GetBlockTextFont().GetObj();

	// Get the cursor position
	Point2i cursorScreenPos = GUIMgr::Get().GetCursorPos();

#ifdef _DEBUG
	// Go through the blocks and draw them
	for( MultBlockList::const_iterator iterBlock = m_ActiveBlocks.begin(); iterBlock != m_ActiveBlocks.end(); ++iterBlock )
	{
		const GameFieldBlockMultBase* pBlock = *iterBlock;

		// Get the position from game field coordinates to screen coordinates
		Point2i blockPos = BlockFieldToScreenPos( pBlock->colIndex, pBlock->yPos, pBlock->size.y );

		// If the cursor is in the block
		const RefSprite* pBlockSprite = pBlock->sprite.GetObj();
		if( Box2i( blockPos, Vector2i(GameDefines::BLOCK_WIDTH, GameDefines::BLOCK_HEIGHT)).IsPointInside( cursorScreenPos ) )
		{
			bool showPrimeBlock = false;

			if( (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime
				|| (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Type) == GameDefines::GT_Ceiling )
			{
				const GameFieldBlockProduct* pCurProductBlock = static_cast<const GameFieldBlockProduct*>( pBlock );

				showPrimeBlock = !pCurProductBlock->IsProductBlock();
			}

			// If the block is a prime then show the specialized sprite for the prime
			if( showPrimeBlock )
			{
				uint32 primeIndex = GameDefines::PrimeToIndex( pBlock->GetValue() );
				if( primeIndex < GameDefines::NUM_PRIMES )
					pBlockSprite = ResourceMgr::Get().GetRefSprite(GameDefines::PRIME_HILITED_SPRITE_ID[primeIndex]).GetObj();
			}
			else
			{
				if( pBlock->size.x == 1 )
					pBlockSprite = ResourceMgr::Get().GetRefSprite(RESID_SPRITE_PRODUCT_BLOCK_HILITED).GetObj();
				else
					pBlockSprite = ResourceMgr::Get().GetRefSprite(RESID_SPRITE_DBL_PRDCT_BLOCK_HILITED).GetObj();
			}
		}

		// Draw the block
		g_pGraphicsMgr->DrawSprite( blockPos, pBlockSprite );
	}


    // Go through the blocks and draw them
	for( MultBlockList::const_iterator iterBlock = m_ActiveBlocks.begin(); iterBlock != m_ActiveBlocks.end(); ++iterBlock )
	{
		const GameFieldBlockMultBase* pBlock = *iterBlock;

		// Get the position from game field coordinates to screen coordinates
		Point2i blockPos = BlockFieldToScreenPos( pBlock->colIndex, pBlock->yPos, pBlock->size.y );
        
		// If the block has text then draw it
		if( pBlock->GetText() )
			g_pGraphicsMgr->DrawFontText( pBlockFont, pBlock->GetText(), blockPos + pBlock->GetTextOffset() );

		// If this block is falling
		if( pBlock->isFalling )
		{
			Box2i blockRect( blockPos, Vector2i( pBlock->size.x * GameDefines::BLOCK_WIDTH, pBlock->size.y * GameDefines::BLOCK_HEIGHT) );
			blockRect.Inflate( -10 );
			g_pGraphicsMgr->FillRect( blockRect, 0xFFFF00 );
		}
	}
#else
	
	// Go through the blocks and draw them
	for( MultBlockList::const_iterator iterBlock = m_ActiveBlocks.begin(); iterBlock != m_ActiveBlocks.end(); ++iterBlock )
	{
		const GameFieldBlockMultBase* pBlock = *iterBlock;

		// If this block is off the bottom
		if( pBlock->yPos < (float32)-GameDefines::BLOCK_HEIGHT )
			continue;

		// Get the position from game field coordinates to screen coordinates
		Point2i blockPos = BlockFieldToScreenPos( pBlock->colIndex, pBlock->yPos, pBlock->size.y );

		// If the cursor is in the block
		const RefSprite* pBlockSprite = pBlock->sprite.GetObj();
		bool cursorIsOver = Box2i( blockPos, Vector2i(GameDefines::BLOCK_WIDTH, GameDefines::BLOCK_HEIGHT)).IsPointInside( cursorScreenPos );
		
		// If the cursor is below the field then don't highlight anything
		if( cursorScreenPos.y > m_FieldBox.Bottom() )
			cursorIsOver = false;
		
		if( cursorIsOver )
		{
			bool showPrimeBlock = false;

			if( (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime
				|| (GameMgr::Get().GetGameType() & GameDefines::GT_Mask_Type) == GameDefines::GT_Ceiling )
			{
				const GameFieldBlockProduct* pCurProductBlock = static_cast<const GameFieldBlockProduct*>( pBlock );

				showPrimeBlock = !pCurProductBlock->IsProductBlock();
			}

			// If the block is a prime then show the specialized sprite for the prime
			if( showPrimeBlock )
			{
				uint32 primeIndex = GameDefines::PrimeToIndex( pBlock->GetValue() );
				if( primeIndex < GameDefines::NUM_PRIMES )
					pBlockSprite = ResourceMgr::Get().GetRefSprite(GameDefines::PRIME_HILITED_SPRITE_ID[primeIndex]).GetObj();
			}
			// Otherise use the empty block
			else
			{
				if( pBlock->size.x == 1 )
					pBlockSprite = ResourceMgr::Get().GetRefSprite(RESID_SPRITE_PRODUCT_BLOCK_HILITED).GetObj();
				else
					pBlockSprite = ResourceMgr::Get().GetRefSprite(RESID_SPRITE_DBL_PRDCT_BLOCK_HILITED).GetObj();
			}
		}
		if( !pBlockSprite )
			continue;

		// If this block is off the bottom
		if( pBlock->yPos < 0.0f )
		{
			Box2i destRect( blockPos.x, blockPos.y, pBlock->size.x * GameDefines::BLOCK_WIDTH, (int32)pBlock->yPos + (pBlock->size.y * GameDefines::BLOCK_HEIGHT) );
			g_pGraphicsMgr->DrawSpriteUnscaled( destRect, pBlockSprite );
		}
		// Else draw the block normally
		else
		{
			g_pGraphicsMgr->DrawSprite( blockPos, pBlockSprite );
		}
	}

    // Go through the blocks and draw them
	for( MultBlockList::const_iterator iterBlock = m_ActiveBlocks.begin(); iterBlock != m_ActiveBlocks.end(); ++iterBlock )
	{
		const GameFieldBlockMultBase* pBlock = *iterBlock;

		// If this block is off the bottom
		if( pBlock->yPos < (float32)-GameDefines::BLOCK_HEIGHT )
			continue;

		// Get the position from game field coordinates to screen coordinates
		Point2i blockPos = BlockFieldToScreenPos( pBlock->colIndex, pBlock->yPos, pBlock->size.y );

		// If this block is off the bottom
		if( pBlock->yPos < 0.0f )
		{
			Box2i destRect( blockPos.x, blockPos.y, pBlock->size.x * GameDefines::BLOCK_WIDTH, (int32)pBlock->yPos + (pBlock->size.y * GameDefines::BLOCK_HEIGHT) );
			
			// If the block has text then draw it
			Box2i clipRect( blockPos + pBlock->GetTextOffset(), Vector2i(destRect.size.x,GameDefines::GetBlockTextFont()->GetCharHeight()) );
			if( clipRect.Bottom() > m_FieldBox.Bottom() )
				clipRect.size.y = m_FieldBox.Bottom() - clipRect.pos.y;
			if( pBlock->GetText() )
				g_pGraphicsMgr->DrawFontTextClipped( pBlockFont, pBlock->GetText(), clipRect );
		}
		// Else draw the block normally
		else
		{
			// If the block has text then draw it
			if( pBlock->GetText() )
				g_pGraphicsMgr->DrawFontText( pBlockFont, pBlock->GetText(), blockPos + pBlock->GetTextOffset() );
		}
	}
#endif

	// Draw the selection frames
	if( m_SelBracketSprite.GetImage() )
	{
		for( BlockList::const_iterator iterBlock = m_SelBlocks.begin(); iterBlock != m_SelBlocks.end(); ++iterBlock )
		{
			const GameFieldBlockMultBase* pBlock = (GameFieldBlockMultBase*)(*iterBlock);

			if( pBlock->colIndex < 0 )
				continue;

			// Get the position from game field coordinates to screen coordinates
			Point2i blockPos = BlockFieldToScreenPos( pBlock->colIndex, pBlock->yPos, pBlock->size.y );

			AnimSprite animSpr;
			if( pBlock->size.x == 1 )
				animSpr = m_SelBracketSprite;
			else
				animSpr = m_WideSelBracketSprite;

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

	//g_pGraphicsMgr->DrawRect( m_FieldBox, 0x00FF0000 );
}