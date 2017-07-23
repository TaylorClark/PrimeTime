#include "../GameLogicTutorAdd.h"
#include "../GameFieldAdd.h"
#include "../GFIAddSummands.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicTutorAdd::SubclassInit  Protected
///	\param frameTime The elapsed frame time
///
///	Initialize a derived class's data.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameLogicTutorAdd::SubclassInit()
{
	m_Steps.push_back( Step( L"Addition Attack mode makes up for its simplicity with pure speed. The goal is to click on the numbers that add up to the sum on the right side of the field.", 0, Goal( GT_CloseMsgBox ) ) );
	m_Steps.push_back( Step( L"Let's trying clearing the sum of 39. There are usually many different solutions so just follow the instructions on the left of the screen.", L"Select a 33", Goal( GT_SelectBlock, 33 ) ) );
	m_Steps.push_back( Step( 0, L"Select a 5", Goal( GT_SelectBlock, 5 ) ) );
	m_Steps.push_back( Step( 0, L"Select a 1", Goal( GT_SelectBlock, 1 ) ) );
	m_Steps.push_back( Step( L"Now that the sum of the selected blocks equals 39, you can clear the blocks by clicking on the sum block to the right of the field.", L"Clear the 39 by clicking the sum block", Goal( GT_SelectBlock, 39 ) ) );
	m_Steps.push_back( Step( L"Great! Let's try clearing one more sum. Clear the 44.", L"Select a 22", Goal( GT_SelectBlock, 22 ) ) );
	m_Steps.push_back( Step( 0, L"Select another 22", Goal( GT_SelectBlock, 22 ) ) );
	m_Steps.push_back( Step( 0, L"Clear the 44 by clicking the sum block", Goal( GT_SelectBlock, 44 ) ) );
	m_Steps.push_back( Step( L"Nice job! You're ready to give Addition Attack mode a try.", 0, Goal( GT_CloseMsgBox ) ) );

	// Start at the first step
	m_CurStepIndex = 0;

	// Initialize the base class
	bool retVal = BaseType::SubclassInit();

	// Update the interface
	OnNewStep();

	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicTutorAdd::GenerateLackingBlocks  Protected
///
/// Generate blocks to fill up the field
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicTutorAdd::GenerateLackingBlocks()
{
	// Create as many blocks as needed to fill a row
	const uint32 NUM_BLOCKS = (uint32)( (GameField::FIELD_HEIGHT * GameField::FIELD_WIDTH) + 3 );
	const int32 BLOCKS[ NUM_BLOCKS ] = {33, 1, 26, 22, 26, 2,
										16, 33, 22, 13, 1, 27,
										5, 26, 22, 13, 33, 22,
										26, 26, 33, 2, 27, 33,
										1, 16, 33, 2, 12, 16,

										26, 16, 12, 27, 10, 13,
										13, 12, 22, 33, 5, 2,
										29, 10, 27, 5, 27, 33,
										22, 12, 13, 33, 16, 26,
										27, 26, 27, 33, 26, 1,

										13, 1, 22 };

	// Get the number of needed blocks
	uint32 neededBlocks = (uint32)(GameField::FIELD_HEIGHT * GameField::FIELD_WIDTH) - m_pGameFieldAdd->GetNumBlocks();
	if( neededBlocks > NUM_BLOCKS )
		neededBlocks = NUM_BLOCKS;

	// Make sure the values don't go out of bounds
	if( m_BlockGenCount + neededBlocks > NUM_BLOCKS )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Warning, L"Too many blocks getting generated" );
		m_BlockGenCount = NUM_BLOCKS - neededBlocks;
		if( m_BlockGenCount < 0 )
		{
			m_BlockGenCount = 0;
			neededBlocks = NUM_BLOCKS;
		}
	}

	// Generate the new blocks
	GFIAddSummands* pNewAddInst = new GFIAddSummands();
	for( uint32 blockIndex = 0; blockIndex < neededBlocks; ++blockIndex )
		pNewAddInst->m_SummandValues.push_back( BLOCKS[m_BlockGenCount + blockIndex] );
	m_BlockGenCount += neededBlocks;

	// Add the blocks to the field
	m_pGameFieldAdd->SendInstruction( pNewAddInst );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicTutorAdd::GenerateSum  Public
///
///	Generate a sum.
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 GameLogicTutorAdd::GenerateSum()
{
	// Add summands until we get a sum within the range
	const uint32 NUM_SUMS = 3;
	int32 RET_SUMS[ NUM_SUMS ] = { 39, 44, 65 };

	// Return the sum
	return RET_SUMS[ m_NumSumsGen++ ];
}