/*=================================================================================================

	\file GameFieldCeiling.cpp
	Game Play Library
	Ceiling Game Field Source
	\author Taylor Clark
	\Date December 24, 2009

	This source file contains the implementation of the ceiling version of the game field class.

=================================================================================================*/

#include "../GameFieldCeiling.h"
#include "Base/TCAssert.h"
#include "../GameFieldBlock.h"
#include "../GameDefines.h"
#include "Graphics2D/GraphicsMgr.h"
#include "../GameFieldBlockProduct.h"
#include "../GameMgr.h"
#include "../GameLogic.h"
#include "Base/NumFuncs.h"


float32 GameFieldCeiling::GetColumnOffsets(int32 offsets[]) const
{
	float32 topY[FIELD_WIDTH];
	float32 maxY = -1.0f;
	for( int32 columnIndex = 0; columnIndex < FIELD_WIDTH; ++columnIndex )
	{
		offsets[columnIndex] = -1;
		topY[columnIndex] = -1.0f;

		if( m_Cols[columnIndex].size() == 0 )
			continue;

		// Get the top block for this column that is not falling
		GameFieldBlockProduct* pTopBlock = NULL;
		for( MultBlockList::const_reverse_iterator iterBlock = m_Cols[columnIndex].rbegin(); iterBlock != m_Cols[columnIndex].rend(); ++iterBlock )
		{
			GameFieldBlockProduct* pCurBlock = static_cast<GameFieldBlockProduct*>(*iterBlock);
			if( pCurBlock->isFalling )
				continue;

			// This block is not falling so use it
			pTopBlock = pCurBlock;
			break;
		}
		if( !pTopBlock )
			continue;
		
		topY[columnIndex] = pTopBlock->yPos;

		// Keep track of the highest value because that is the value from which we calculate the
		// column offsets
		if( topY[columnIndex] > maxY )
			maxY = topY[columnIndex];
	}

	// Go through the top Y positions and form the offsets
	for( int32 columnIndex = 0; columnIndex < FIELD_WIDTH; ++columnIndex )
	{
		float32 columnOffset = maxY - topY[columnIndex];
		offsets[columnIndex] = (int32)( (columnOffset / (float32)GameDefines::BLOCK_HEIGHT) + 0.5f );
	}

	return maxY + (float32)GameDefines::BLOCK_HEIGHT;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameFieldCeiling::IsGameOver  Public
///
///	\returns True if the field is finished, false otherwise
///
///	Get if the game field is in a terminated state (The blocks are past the top).
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameFieldCeiling::IsGameOver() const
{
	// If the top of the field is above the top then use it as an offset.  This can happen if a
	// low frame rate is being experienced and the previous update pushes the blocks past the top.
	/*float32 topMostY = GetTopMostY();
	float32 blockYOffset = 0.0f;
	if( topMostY > (float32)m_FieldBox.size.y )
		blockYOffset = (float32)m_FieldBox.size.y - topMostY;*/

	int32 offsets[FIELD_WIDTH];
	float32 topMostY = GetColumnOffsets(offsets);

	// If the blocks at the top check then check for success
	if( topMostY >= m_FieldBox.size.y )
	{
		m_IsVictory = true;

		for( int32 columnIndex = 0; columnIndex < FIELD_WIDTH; ++columnIndex )
		{
			if( offsets[columnIndex] != m_CeilingOffset[columnIndex] )
			{
				m_IsVictory = false;
				break;
			}
		}

		// The field is done since the blocks hit the top
		return true;
	}
	// Or if the blocks are near the top then check for failure
	else if( topMostY >= _lowestBlockBottom )
	{
		for( int32 columnIndex = 0; columnIndex < FIELD_WIDTH; ++columnIndex )
		{
			// If this column has the potential to contact the crate
			if( offsets[columnIndex] < m_CeilingOffset[columnIndex] )
			{
				float32 columnTopY = topMostY - (float32)(GameDefines::BLOCK_HEIGHT * offsets[columnIndex]);

				float32 crateBottomY = (float32)( m_FieldBox.size.y - (m_CeilingOffset[columnIndex] * GameDefines::BLOCK_HEIGHT) );

				// If a column of blocks is contacting a crate
				if( columnTopY > crateBottomY )
				{
					// The player has failed and the game is over
					m_IsVictory = false;
					return true;
				}
			}
		}
	}

	// The field is not done yet
	return false;

	// Go through the columns and test if the top of the column touches the offset blocks
	//int32 lockedColumns = 0;
	//bool retVal = false;
	//for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
	//{
	//	if( m_Cols[colIndex].size() == 0 )
	//		continue;

	//	// Get the top block for this column that is not falling
	//	GameFieldBlockProduct* pTopBlock = NULL;
	//	for( MultBlockList::const_reverse_iterator iterBlock = m_Cols[colIndex].rbegin(); iterBlock != m_Cols[colIndex].rend(); ++iterBlock )
	//	{
	//		GameFieldBlockProduct* pCurBlock = static_cast<GameFieldBlockProduct*>(*iterBlock);
	//		if( pCurBlock->isFalling )
	//			continue;

	//		// This block is not falling so use it
	//		pTopBlock = pCurBlock;
	//		break;
	//	}
	//	if( !pTopBlock )
	//		continue;
	//	
	//	// Get the offset bottom for this column
	//	float32 offsetBottom = (float32)( m_FieldBox.size.y - (m_CeilingOffset[colIndex] * GameDefines::BLOCK_HEIGHT) );

	//	// Is this block touching the offset block
	//	float32 blockTop = pTopBlock->yPos + (float32)(pTopBlock->size.y * GameDefines::BLOCK_HEIGHT);
	//	blockTop += blockYOffset;
	//	if( TCBase::NearlyEquals<float32>( blockTop, offsetBottom, 2.0f ) )
	//		lockedColumns++;

	//	// Test for collision
	//	if( blockTop >= offsetBottom )
	//		retVal = true;
	//}

	//// If every column is in the proper place
	//if( lockedColumns == FIELD_WIDTH )
	//{
	//	m_IsVictory = true;
	//	return true;
	//}
	//
	//// Return if the field is in a finished state
	//return retVal;
}

void GameFieldCeiling::SetOffsets( int32 offsets[FIELD_WIDTH] )
{
	int32 maxOffset = 0;

	for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
	{
		m_CeilingOffset[colIndex] = offsets[colIndex];

		if( m_CeilingOffset[colIndex] > maxOffset )
			maxOffset = m_CeilingOffset[colIndex];
	}

	 _lowestBlockBottom = (float32)( m_FieldBox.size.y - (maxOffset * GameDefines::BLOCK_HEIGHT) );
}

/// Initialize the field
bool GameFieldCeiling::Init()
{
	bool retVal = GameFieldMultBase::Init();

	m_IsVictory = false;

	// Get the crate sprite
	m_OffsetSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_CRATE_BLOCK );

	return retVal;
}


/// Draw the game
void GameFieldCeiling::Draw() const
{
	GameFieldMultBase::Draw();

	// Draw the ceiling tiles
	for( int32 colIndex = 0; colIndex < FIELD_WIDTH; ++colIndex )
	{
		int32 blockPosY = m_FieldBox.pos.y;
		const int32 blockPosX = m_FieldBox.pos.x + (colIndex * GameDefines::BLOCK_WIDTH);

		// Go through each offset block and draw it
		if( m_OffsetSprite.GetObj() )
		{
			for( int32 offsetIndex = 0; offsetIndex < m_CeilingOffset[colIndex]; ++offsetIndex )
			{
				g_pGraphicsMgr->DrawSprite( Point2i(blockPosX, blockPosY), m_OffsetSprite.GetObj() );
				blockPosY += GameDefines::BLOCK_HEIGHT;
			}
		}
		// Else if the block sprite didn't load then draw black boxes
		else
		{
			for( int32 offsetIndex = 0; offsetIndex < m_CeilingOffset[colIndex]; ++offsetIndex )
			{
				g_pGraphicsMgr->FillRect( Box2i(blockPosX, blockPosY, GameDefines::BLOCK_WIDTH, GameDefines::BLOCK_HEIGHT), 0xFFDDDDDD );
				blockPosY += GameDefines::BLOCK_HEIGHT;
			}
		}
	}
}