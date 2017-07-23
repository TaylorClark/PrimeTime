/*=================================================================================================

	\file GameLogicTutorCeiling.cpp
	Game Play Library
	Game Field Source
	\author Taylor Clark
	\Date November 4, 2006

	This source file contains the implementation of the game logic for the ceiling tutorial.

=================================================================================================*/

#include "../GameLogicTutorCeiling.h"
#include "../GameFieldCeiling.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicTutorCeiling::SubclassInit  Protected
///
///	\param frameTime The elapsed frame time
///
///	Initialize a derived class's data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameLogicTutorCeiling::SubclassInit()
{
	m_Steps.resize( 5 );

	//m_Steps[1] = Step( L"Prime Time exemplifies the fundamental theorem of arithmetic which states that every positive non-prime integer can be written as a unique product of primes.", 0, Goal( GT_CloseMsgBox ) );
	
	int stpIdx = 0;
	m_Steps[stpIdx++] = Step( L"Ceiling mode is played the same as Prime Time mode, but requires you plan what blocks you clear carefully.", 0, Goal( GT_CloseMsgBox ) );
	m_Steps[stpIdx++] = Step( L"Notice the crate-like blocks at the top of the field. The goal of ceiling mode is to make the top of your field of blocks fit flush with these crate blocks.", 0, Goal( GT_CloseMsgBox ) );
	m_Steps[stpIdx++] = Step( L"Based on the crates presented to us, we need to clear one block from the first column and 2 blocks from the fourth column. Try clearing the 9.", L"Clear the 9", Goal( GT_ClearProduct, 9 ) );
	m_Steps[stpIdx++] = Step( L"Notice how your field will now fit into the crates, go ahead and make your blocks reach the top.", L"Make the blocks reach the top of the field.", Goal( GT_GenerateRows, 13 ) );
	m_Steps[stpIdx++] = Step( L"Great job! Now go give the actual ceiling mode a try.", 0, Goal( GT_CloseMsgBox ) );
	
	// Start at the first step
	m_CurStepIndex = 0;

	// Initialize the base class
	bool retVal = GameLogicProduct::SubclassInit();

	// Set the offsets
	int32 ceilingOffsets[ GameField::FIELD_WIDTH ] = {0};
	ceilingOffsets[0] = 1;
	ceilingOffsets[3] = 2;
	GameFieldCeiling* pCeilingField = (GameFieldCeiling*)m_pGameFieldMult;
	pCeilingField->SetOffsets( ceilingOffsets );

	// Update the interface
	OnNewStep();

	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicTutorCeiling::GenerateBlocksToFieldWidth  Public
///
///	\returns An array of field blocks representing the generated row
///
///	Generate blocks up to the field width.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlockMultBase** GameLogicTutorCeiling::GenerateBlocksToFieldWidth()
{
	static GameFieldBlockMultBase* s_FieldBlocks[ GameFieldMultBase::FIELD_WIDTH + 1 ] = {0};

	// If the player is on step 3 then don't allow the blocks to get too high
	if( m_CurStepIndex <= 2 )
	{
		static GameFieldBlockMultBase* s_EmptyBlocks[ GameFieldMultBase::FIELD_WIDTH + 1 ] = {0};
		if( m_RowsGenerated >= 8 )
			return s_EmptyBlocks;
	}

	// Create as many blocks as needed to fill a row
	const int32 NUM_ROWS = 13;
	const int32 BLOCKS[NUM_ROWS][GameField::FIELD_WIDTH] = {{2, 2, 2, 2, 5},
															{3, 5, 4, 3, 7},
															{2, 5, 15, 22, 5},
															{7, 2, 8, 9, 18},
															{2, 14, 11, 5, 2},
															{2, 4, 16, 7, 2},
															{20, 2, 2, 5, 2},
															{2, 4, 5, 2, 2},
															{2, 5, 2, 10, 12},
															{12, 2, 5, 5, 6},
															{7, 21, 7, 5, 2},
															{2, 2, 8, 5, 4},
															{2, 7, 18, 5, 6} };
	if( m_RowsGenerated < NUM_ROWS )
		GenerateRow( BLOCKS[m_RowsGenerated], s_FieldBlocks );
	else
		s_FieldBlocks[0] = 0;

	s_FieldBlocks[GameField::FIELD_WIDTH] = 0;

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