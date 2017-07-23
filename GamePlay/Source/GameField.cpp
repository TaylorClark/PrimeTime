/*=================================================================================================

	\file GameFieldMult.cpp
	Game Play Library
	Game Field Source
	\author Taylor Clark
	\Date May 12, 2006

	This source file contains the implementation of the multiplication version of the game field
	class.

=================================================================================================*/

#include "../GameField.h"
#include "../GameFieldBlock.h"
#include "../GameDefines.h"
#include "Graphics2D/GraphicsMgr.h"
#include "GUI/GUIMgr.h"
#include "Base/MsgLogger.h"
#include "Base/TCAssert.h"
#include "Base/NumFuncs.h"
#include <math.h>
#include "Graphics2D/GraphicsDefines.h"
#include "../GameMgr.h"
#include "../GameLogic.h"
#include "Audio/AudioMgr.h"
#include "../PIIToggleBlockSelect.h"
#include "../PIIClearSelection.h"
#include "../GFIRemoveBlocks.h"
#include "../InstructionStreamPlay.h"
#include "../InstructionStreamReplay.h"
#include "Base/FileFuncs.h"
#include <fstream>


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::IsBlockSelected  Private
///
///	\param pBlock The block to test for existance in the selected list
///	\param pSaveBlockIter A pointer to a BlockList iterator that will be used to store the
///							iterator to the block in the m_SelBlocks if it exists
///
///	Get if a block is selected.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameField::IsBlockSelected( GameFieldBlock* pBlock, BlockList::const_iterator* pSaveBlockIter ) const
{
	// Go through the currently selected blocks
	for( BlockList::const_iterator iterBlock = m_SelBlocks.begin(); iterBlock != m_SelBlocks.end(); ++iterBlock )
	{
		// If this is our block
		if( *iterBlock == pBlock )
		{
			// If the iterator is to be saved
			if( pSaveBlockIter )
				*pSaveBlockIter = iterBlock;
			return true;
		}
	}

	// If the iterator is to be saved then save the end iterator since the block could not be
	// found in the lists
	if( pSaveBlockIter )
		*pSaveBlockIter = m_SelBlocks.end();

	// The block does not exist in the selection list
	return false;
}


bool GameField::IsBlockSelected( GameFieldBlock* pBlock, BlockList::iterator* pSaveBlockIter )
{
	// Go through the currently selected blocks
	for( BlockList::iterator iterBlock = m_SelBlocks.begin(); iterBlock != m_SelBlocks.end(); ++iterBlock )
	{
		// If this is our block
		if( *iterBlock == pBlock )
		{
			// If the iterator is to be saved
			if( pSaveBlockIter )
				*pSaveBlockIter = iterBlock;
			return true;
		}
	}

	// If the iterator is to be saved then save the end iterator since the block could not be
	// found in the lists
	if( pSaveBlockIter )
		*pSaveBlockIter = m_SelBlocks.end();

	// The block does not exist in the selection list
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::Init  Public
///
///	\returns Whether or not the field is ready for play, false if an fatal error occured
///
///	Initialize the data needed for the game field.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameField::Init()
{
	if( m_pInstStream )
		delete m_pInstStream;

	//if( TCBase::DoesFileExist( L"c:\\TestGame.dat" ) )
	//{
	//	m_pInstStream = new InstructionStreamReplay();

	//	std::ifstream inFile( L"c:\\TestGame.dat", std::ios_base::in | std::ios_base::binary );
	//	Serializer serializer( &inFile );
	//	((InstructionStreamReplay*)m_pInstStream)->Init( serializer );

	//	inFile.close();
	//	//_wremove( L"c:\\TestGame.dat" );
	//}
	//else
	{
		m_pInstStream = new InstructionStreamPlay();
		((InstructionStreamPlay*)m_pInstStream)->Init( false );
	}

	m_pInstStream->m_pGameField = this;

	m_SelBracketSprite = AnimSprite( ResourceMgr::Get().GetRefSprite( RESID_SPRITE_SELECTED_BLOCK_FRAME ) );
	m_SelBracketSprite.Play();

	m_WideSelBracketSprite = AnimSprite( ResourceMgr::Get().GetRefSprite( RESID_SPRITE_SELECTED_DBLBLOCK_FRAME ) );
	m_WideSelBracketSprite.Play();

	m_SelectSound = ResourceMgr::Get().GetAudioSample( RESID_SOUND_SELECT_BLOCK );

	m_DeselectSound = ResourceMgr::Get().GetAudioSample( RESID_SOUND_DESELECT_BLOCK );

	m_BlockFallSound = ResourceMgr::Get().GetAudioSample( RESID_SOUND_BLOCK_FALL );
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::SendInstruction  Public
///
///	\param pInstruction The new instruction
///
/// Add an instruction for processing
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameField::SendInstruction( GameFieldInstruction* pInstruction )
{
	TCASSERT( pInstruction != NULL );
	m_pInstStream->AddPendingInstruction( pInstruction );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::Update  Public
///
///	\param frameTime The elapsed frame time in seconds
///
///	Update the game field by pushing blocks up, dropping blocks, and checking for collision.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameField::Update( float frameTime )
{
	if( GameMgr::Get().IsPaused() )
		return;

	// Update the block selection sprite animation
	m_SelBracketSprite.Update( frameTime );
	m_WideSelBracketSprite.Update( frameTime );

	m_pInstStream->Update( m_FieldElapsedTime, frameTime );

	// Process instructions
	float32 prevUpdateTime = m_FieldElapsedTime;
	m_FieldElapsedTime += frameTime;
	while( m_pInstStream->HasPendingInstructions() )
	{
		// Get the next instruction
		GameFieldInstruction* pInstruction = m_pInstStream->PopPendingInstruction();

		if( pInstruction->executionTime > prevUpdateTime )
		{
			// Update the field to sync up with the next instruction
			float32 catchUpDelta = pInstruction->executionTime - prevUpdateTime;
			SubclassUpdate( catchUpDelta );
			frameTime -= catchUpDelta;
			prevUpdateTime += catchUpDelta;
		}

		HandleInstruction( pInstruction );

		// Process the handled instruction
		m_pInstStream->ProcessHandledInstruction( pInstruction );
	}

	// Allow derived classes to update
	SubclassUpdate( frameTime );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::HandleInstruction  Private
///
///	\param pInstruction The instruction to be handled
///	\returns True if the instruction was handled, false if it was not
///
/// Handle an instruction
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameField::HandleInstruction( GameFieldInstruction* pInstruction )
{
	// Handle the type of instruction
	switch( pInstruction->GetType() )
	{
	case GFI_SelectBlock:
		{
			Update_HandleClick( (PIIToggleBlockSelect*)pInstruction );
		}
		break;

	case GFI_ClearSelection:
		{
			// Store the IDs of the selected blocks
			PIIClearSelection* pClearInst = (PIIClearSelection*)pInstruction;
			pClearInst->DeselectedBlockIDs.clear();
			for( BlockList::iterator iterBlock = m_SelBlocks.begin(); iterBlock != m_SelBlocks.end(); ++iterBlock )
				pClearInst->DeselectedBlockIDs.push_back( (*iterBlock)->GetID() );

			// Clear the selection
			m_SelBlocks.clear();
		}
		break;

	case GFI_RemoveBlocks:
		{
			// Remove the blocks
			GFIRemoveBlocks* pRemoveInst = (GFIRemoveBlocks*)pInstruction;
			for( std::list<uint32>::iterator iterBlockID = pRemoveInst->m_RemovedBlockIDs.begin();
					iterBlockID != pRemoveInst->m_RemovedBlockIDs.end();
					++iterBlockID )
			{
				// Get the block that was clicked on
				if( m_BlockIDMap.find( *iterBlockID ) == m_BlockIDMap.end() )
					continue;

				GameFieldBlock* pRemoveBlock = (GameFieldBlock*)m_BlockIDMap[ *iterBlockID ];
				if( !pRemoveBlock )
					continue;

				RemoveBlock( pRemoveBlock );
			}
		}
		break;

	default:
		return false;
	}

	return true;
}

/// The destructor, virtual to ensure derived classes are freed properly
GameField::~GameField()
{
	if( m_pInstStream )
		delete m_pInstStream;
	m_pInstStream = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::ClickAtPoint  Public
///
///	\param cursorPos The position of the cursor at the time of click in screen coordinates
///
///	Register a mouse click at a position on the game field.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameField::ClickAtPoint( const Point2i& cursorPos, GUIDefines::EMouseButtons btn )
{
	GameFieldInstruction* pSelectInst = 0;

	if( btn == GUIDefines::MB_Right )
	{
		// Don't allow deselect in the basic tutorial
		if( !GameMgr::Get().IsPTTutorial() && !GameMgr::Get().IsAdditionTutorial() )
			pSelectInst = new PIIClearSelection();
	}
	else
	{
		// Get the clicked block
		GameFieldBlock* pSelBlock = GetBlockAtPos( ScreenPosToBlockField( cursorPos ) );

		// If there was a block under the mouse then create an instruction to toggle that block's
		// selection status
		if( pSelBlock )
			pSelectInst = new PIIToggleBlockSelect( pSelBlock->GetID() );
	}

	// Add the instruction
	if( pSelectInst != 0 )
		SendInstruction( pSelectInst );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::Update_HandleClick  Private
///
///	\param pInstruction The block selection instruction
///
///	Handle a mouse click from within the Update function.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameField::Update_HandleClick( PIIToggleBlockSelect* pInstruction )
{
	// Get the block that was clicked on
	if( m_BlockIDMap.find( pInstruction->GetBlockID() ) == m_BlockIDMap.end() )
		return;
	GameFieldBlock* pClickBlock = (GameFieldBlock*)m_BlockIDMap[ pInstruction->GetBlockID() ];
	if( !pClickBlock )
		return;

	// See if the block is in the selected list
	BlockList::iterator iterSelBlock;
	if( IsBlockSelected( pClickBlock, &iterSelBlock ) )
	{
		// Remove the selection
		GameLogic::EBlockSelAction selAction = GameMgr::Get().GetLogic()->OnBlockDeselect( pClickBlock );
		if( selAction == GameLogic::BSA_DeselectLast )
			m_SelBlocks.erase( iterSelBlock );

		if( m_DeselectSound.GetObj() )
			m_DeselectSound.GetObj()->Play();
	}
	// Else the block is not yet selected
	else
	{
		// Store the selected block
		m_SelBlocks.push_back( pClickBlock );

		// Handle selection
		GameLogic::EBlockSelAction selAction = GameMgr::Get().GetLogic()->OnBlockSelect( pClickBlock );
		if( selAction == GameLogic::BSA_RemoveSelected )
			RemoveBlock( pClickBlock );
		// Else if we should remove all of the selected blocks
		else if( selAction == GameLogic::BSA_RemoveEntireSelection )
		{
			// Remove the selected blocks
			while( m_SelBlocks.size() > 0 )
				RemoveBlock( m_SelBlocks.front() );
		}
		// Else if all blocks should be deselected
		else if( selAction == GameLogic::BSA_DeselectSelection )
			m_SelBlocks.clear();
		// Else if the block should not be selected
		else if( selAction == GameLogic::BSA_DeselectLast )
			m_SelBlocks.pop_back();
		// Else select the new block
		else
		{
			if( m_SelectSound.GetObj() )
				m_SelectSound.GetObj()->Play();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameField::GetBlocksInRectangle  Public
///
/// Get the blocks that intersect a rectangle
///////////////////////////////////////////////////////////////////////////////////////////////////
BlockList GameField::GetBlocksInRectangle( const Box2i& box )
{
	BlockList retList;

	for( int curX = box.Left(); curX <= box.Right(); curX += GameDefines::BLOCK_WIDTH )
	{
		// Go from bottom to top (Y = 0 is the bottom of the field and increases upward)
		for( int curY = box.pos.y; curY <= box.Bottom(); curY += GameDefines::BLOCK_HEIGHT )
		{
			Point2i testFieldPos(curX,curY);

			GameFieldBlock* pTestBlock = this->GetBlockAtPos( testFieldPos );

			// If we found a block and it's not already in the list
			if( pTestBlock && !retList.ContainsBlock(pTestBlock) )
				retList.push_back( pTestBlock );
		}
	}

	return retList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::BlockFieldToScreenPos  Public
///
///	Convert a block position on the field to screen coordinates.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Point2i GameField::BlockFieldToScreenPos( int32 colIndex, float32 yPos, int32 ySize ) const
{
	// Get the position from game field coordinates to screen coordinates
	Point2i blockPos;
	blockPos.x = m_FieldBox.pos.x + (colIndex * GameDefines::BLOCK_WIDTH);
	blockPos.y = m_FieldBox.pos.y + m_FieldBox.size.y - (int32)yPos - (ySize * GameDefines::BLOCK_HEIGHT);

	return blockPos;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameField::BlockFieldToScreenPos  Public
///
///	Convert a position on the field to screen coordinates.
///////////////////////////////////////////////////////////////////////////////////////////////////
Point2i GameField::BlockFieldToScreenPos( const Point2i& fieldPos ) const
{
	// Get the position from game field coordinates to screen coordinates
	Point2i blockPos;
	blockPos.x = m_FieldBox.pos.x + fieldPos.x;
	blockPos.y = m_FieldBox.pos.y + m_FieldBox.size.y - fieldPos.y;

	return blockPos;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameField::ScreenPosToBlockField  Public
///
///
///	Convert a position from screen coordinates to game field coordinates.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Point2i GameField::ScreenPosToBlockField( const Point2i& screenPos ) const
{
	Point2i fieldPos;
	
	// Convert the X position to field coordinates
	fieldPos.x = screenPos.x - m_FieldBox.pos.x;

	// Convert the Y
	fieldPos.y = screenPos.y - m_FieldBox.pos.y;
	fieldPos.y = m_FieldBox.size.y - fieldPos.y;

	return fieldPos;
}