//=================================================================================================
/*!
	\file GameLogicTutorFractions.cpp
	Game Play Library
	Fraction Mode Tutorial Game Logic Source
	\author Taylor Clark
	\date January 30, 2010

	This source file contains the definition for the Prime Time fraction mode tutorial logic.
*/
//=================================================================================================

#include "../GameLogicTutorFractions.h"
#include "../GameFieldFractions.h"
#include "../GameFieldBlockFraction.h"

static const uint32 NUM_ROWS = 5;
	

///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicTutorFractions::SubclassInit  Protected
///	\param frameTime The elapsed frame time
///
///	Initialize a derived class's data.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameLogicTutorFractions::SubclassInit()
{
	m_Steps.resize( 6 );

	int stpIdx = 0;
	m_Steps[stpIdx++] = Step( L"On the left you will see the fraction for which you must find an equivalent fraction.", 0, Goal( GT_CloseMsgBox ) );
	m_Steps[stpIdx++] = Step( L"We need to find a fraction that is eqaul to one third. Four twelfths is equivalent so let's clear that.", 0, Goal( GT_CloseMsgBox ) );

	m_Steps[stpIdx++] = Step( L"By default you select the numerator first. So let's select the 4.", L"Select the 4", Goal( GT_SelectBlock, 4 ) );
	m_Steps[stpIdx++] = Step( L"Now let's select the denominator.", L"Select the 12", Goal( GT_SelectBlock, 12 ) );

	m_Steps[stpIdx++] = Step( L"Great job! Now the 4 and 12 will be removed and you will get a new fraction for which you must find an equivalent. Go give it a try. One more tip...", 0, Goal( GT_CloseMsgBox ) );

	m_Steps[stpIdx++] = Step( L"If you would rather select the denominator first, you can click on the denominator block to the right of the field, then the number block you want. Good luck!", 0, Goal( GT_CloseMsgBox ) );

	
	// Start at the first step
	m_CurStepIndex = 0;

	// Initialize the base class
	bool retVal = GameLogicFractions::SubclassInit();

	// Add the rest of the lines to numbers
	for( int32 rowIndex = 3; rowIndex < NUM_ROWS; ++rowIndex )
		_pGameFieldFractions->DropNewBlocks( GenerateBlocksToFieldWidth() );

	// Update the interface
	OnNewStep();

	return retVal;
}


/// A message handler for when a block is selected that contains the mode-specific logic
GameLogic::EBlockSelAction GameLogicTutorFractions::OnBlockSelect( GameFieldBlock* pBlock )
{
	return GameLogicTutorBase< GameLogicFractions >::OnBlockSelect( pBlock );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GenerateRow  Global
///	\param vals The values to use for the block values
///	\param pFieldBlocks The array of field blocks to fill in.
///
///	Generate a row of blocks.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GenerateRow( const int32 vals[5], GameFieldBlockMultBase** pFieldBlocks )
{
	//int32 vals[5] = { val0, val1, val2, val3, val4 };
	for( int32 blockIndex = 0; blockIndex < 5; ++blockIndex )	
	{
		pFieldBlocks[blockIndex] = new GameFieldBlockFraction( vals[blockIndex] );

		if( !pFieldBlocks[blockIndex] )
			break;

		pFieldBlocks[blockIndex]->sprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK );

		pFieldBlocks[blockIndex]->colIndex = blockIndex;
	}
}

/// Get the next fraction for the player to solve
Fraction GameLogicTutorFractions::GetNextFraction() const
{
	return Fraction( 1, 3 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicTutorFractions::GenerateBlocksToFieldWidth  Public
///
///	\returns An array of field blocks representing the generated row
///
///	Generate blocks up to the field width.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlockMultBase** GameLogicTutorFractions::GenerateBlocksToFieldWidth()
{
	static GameFieldBlockMultBase* s_FieldBlocks[ GameFieldMultBase::FIELD_WIDTH + 1 ] = {0};

	// The rows are in reverse order, last row first
	const int32 BLOCKS[NUM_ROWS][5] = {{10, 2, 3, 5, 2},
								{15, 5, 2, 18, 2},
								{6, 2, 2, 5, 2},
								{3, 4, 15, 2, 2},
								{9, 3, 12, 2, 5} };
	if( m_RowsGenerated < NUM_ROWS )
		GenerateRow( BLOCKS[m_RowsGenerated], s_FieldBlocks );
	else
		s_FieldBlocks[0] = 0;

	s_FieldBlocks[5] = 0;

	// Increment the number of rows generated
	m_RowsGenerated++;

	// If the current step is a row generate step
	if( m_CurStepIndex < m_Steps.size() && m_Steps[ m_CurStepIndex ].goal.goalType == GT_GenerateRows )
	{
		// If the correct number is generated then mark completed
		if( m_RowsGenerated >= m_Steps[ m_CurStepIndex ].goal.data.rowsGenerated )
			m_Steps[ m_CurStepIndex ].goal.isCompleted = true;
	}

	// Return the array
	return s_FieldBlocks;
}