#include "../GameLogicTutorPTCombo.h"
#include "../GameFieldMultBase.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicTutorPT::SubclassInit  Protected
///
///	\param frameTime The elapsed frame time
///
///	Initialize a derived class's data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameLogicTutorPTCombo::SubclassInit()
{
	m_Steps.resize( 8 );

	//m_Steps[1] = Step( L"Prime Time exemplifies the fundamental theorem of arithmetic which states that every positive non-prime integer can be written as a unique product of primes.", 0, Goal( GT_CloseMsgBox ) );
	
	int stpIdx = 0;
	m_Steps[stpIdx++] = Step( L"Combos are a way of improving your score exponentially. If you have played the normal Prime Time mode you may have noticed that sometimes products get a TV static border around them.", 0, Goal( GT_CloseMsgBox ) );
	m_Steps[stpIdx++] = Step( L"The static effect alerts you to which products will start or continue a combo chain. Let's give an example, start by clearing the 12.", L"Clear the 12", Goal( GT_ClearProduct, 12 ) );
	m_Steps[stpIdx++] = Step( L"Notice that the product blocks that were touching the 12 when it was cleared now have a static border. Let's clear the 4 and see what happens.", L"Clear the 4", Goal( GT_ClearProduct, 4 ) );
	m_Steps[stpIdx++] = Step( L"Now that you have cleared a static block you have begun your combo chain. You can see how big your chain is in the top right of the screen. Continue the chain by clearing the 14.", L"Clear the 14", Goal( GT_ClearProduct, 14 ) );
	m_Steps[stpIdx++] = Step( L"By clearing another static block, your combo chain was increased. Your active combo chain gives you a score multiplier, the bigger the better!", 0, Goal( GT_CloseMsgBox ) );
	
	m_Steps[stpIdx++] = Step( L"You can also continue your chain by using the combo block located near the top left of the field. Use that block like a factor by clicking on it to clear the 28.", L"Clear the 28 by using the combo block to the upper left of the field", Goal( GT_ClearProduct, 28 ) );
	m_Steps[stpIdx++] = Step( L"If you make a mistake or clear a non-static product then the chain will end. Clear the 18, which is not static, to see the combo end.", L"Clear the 18", Goal( GT_ClearProduct, 18 ) );
	m_Steps[stpIdx++] = Step( L"Before you go, here's a tip: try to group as many products together as possible before clearing, this way it will be easier to grow the chain.", 0, Goal( GT_CloseMsgBox ) );
	
	// Start at the first step
	m_CurStepIndex = 0;

	// Initialize the base class
	bool retVal = GameLogicProduct::SubclassInit();

	// Update the interface
	OnNewStep();

	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicTutorPTCombo::GenerateBlocksToFieldWidth  Public
///
///	\returns An array of field blocks representing the generated row
///
///	Generate blocks up to the field width.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlockMultBase** GameLogicTutorPTCombo::GenerateBlocksToFieldWidth()
{
	static GameFieldBlockMultBase* s_FieldBlocks[ GameFieldMultBase::FIELD_WIDTH + 1 ] = {0};

	// Create as many blocks as needed to fill a row
	const int32 BLOCKS[10][5] = {{12, 4, 15, 2, 2},
								{6, 2, 3, 5, 2},
								{9, 3, 2, 2, 5},	
								{14, 2, 3, 7, 28},
								{10, 2, 3, 5, 2},
								{15, 5, 2, 18, 2},
								{20, 3, 2, 5, 2},
								{3, 2, 2, 2, 2},
								{5, 3, 11, 3, 24},
								{2, 3, 22, 5, 2} };
	if( m_RowsGenerated < 10 )
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