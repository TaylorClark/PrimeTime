#include "../GameLogicTutorPT.h"
#include "GUI/MsgBox.h"
#include "../GameFieldPrimeTime.h"
#include "../GameFieldBlockProduct.h"
#include "Base/NumFuncs.h"
#include "../GameMgr.h"
#include "GUI/GUIMgr.h"
#include "../GUILayout_Game.h"

const uint32 NUM_ROWS = 6;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicTutorPT::SubclassInit  Protected
///
///	\param frameTime The elapsed frame time
///
///	Initialize a derived class's data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameLogicTutorPT::SubclassInit()
{
	m_Steps.push_back( Step( L"Let's start by clearing the 6, which is the product of the primes 2 and 3. Start by clicking on a 2 with the left mouse button.", L"Select a 2", Goal( GT_SelectBlock, 2 ) ) );
	m_Steps.push_back( Step( L"Next, select a 3 by also clicking on it. Be aware that you can select blocks anywhere on the field.", L"Select a 3", Goal( GT_SelectBlock, 3 ) ) );
	m_Steps.push_back( Step( L"Now that the prime factors have been selected, the 6 can be cleared by clicking on it.", L"Select the 6", Goal( GT_SelectBlock, 6 ) ) );
	m_Steps.push_back( Step( L"Great! The 2, 3, and 6 blocks are removed from the field. You can deselect a block by clicking on it a second time or right-click to deselect all.", 0, Goal( GT_CloseMsgBox) ) );
	m_Steps.push_back( Step( L"The static effect you see around the 4 and 22 signifies a combo block, you can learn more about combos from the combo tutorial, but ignore it for now.", 0, Goal( GT_CloseMsgBox) ) );
	m_Steps.push_back( Step( L"Notice that the blocks with a white face are all prime numbers, these are your factors.", 0, Goal( GT_CloseMsgBox ) ) );
	
	m_Steps.push_back( Step( L"Your goal is to clear the products, which are the blocks with a black face, by selecting them after first selecting their prime factors. Let's try one more example.", 0, Goal( GT_CloseMsgBox ) ) );
	m_Steps.push_back( Step( L"Let's clear an 18 whose prime factors are 3, 3, and 2.", L"Select a 3", Goal( GT_SelectBlock, 3 ) ) );
	m_Steps.push_back( Step( 0, L"Select another 3", Goal( GT_SelectBlock, 3 ) ) );
	m_Steps.push_back( Step( 0, L"Select a 2", Goal( GT_SelectBlock, 2 ) ) );
	m_Steps.push_back( Step( 0, L"Select the 18", Goal( GT_SelectBlock, 18 ) ) );
	
	m_Steps.push_back( Step( L"Nice job, that's all there is to it! Give the normal game a try for a while and, when you're ready, come back for the more advanced tutorials. One more tip...", 0, Goal( GT_CloseMsgBox ) ) );
	//m_Steps.push_back( Step( L"Normally the blocks rise from the bottom at a steady speed. If the blocks ascend too slowly for you then try using the CTRL key to make them rise faster. Good luck!", 0, Goal( GT_CloseMsgBox ) ) );
	m_Steps.push_back( Step( L"Normally blocks rise from the bottom at a steady speed. If you feel they ascend too slowly you can use the CTRL key or the round, blue button to make them rise faster. Good luck!", 0, Goal( GT_CloseMsgBox ) ) );
	
	// Start at the first step
	m_CurStepIndex = 0;

	// Initialize the base class
	bool retVal = GameLogicProduct::SubclassInit();

	// Add the rest of the lines to numbers
	for( int32 rowIndex = 3; rowIndex < NUM_ROWS; ++rowIndex )
		m_pGameFieldMult->DropNewBlocks( GenerateBlocksToFieldWidth() );

	// Update the interface
	OnNewStep();

	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicTutorPT::GenerateRow  Static Private
///
///	\param pFieldBlocks The array of field blocks to fill in.
///
///	Generate a row of blocks.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicTutorPT::GenerateRow( const int32 vals[5], GameFieldBlockMultBase** pFieldBlocks )
{
	//int32 vals[5] = { val0, val1, val2, val3, val4 };
	for( int32 blockIndex = 0; blockIndex < 5; ++blockIndex )	
	{
		bool isPrime = TCBase::IsPrime(vals[blockIndex]);
		pFieldBlocks[blockIndex] = new GameFieldBlockProduct( vals[blockIndex], !isPrime );

		if( !pFieldBlocks[blockIndex] )
			break;

		pFieldBlocks[blockIndex]->colIndex = blockIndex;

		if( isPrime )
			pFieldBlocks[blockIndex]->sprite = ResourceMgr::Get().GetRefSprite( GameDefines::PRIME_SPRITE_ID[ GameDefines::PrimeToIndex(vals[blockIndex]) ] );
		else
			pFieldBlocks[blockIndex]->sprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicTutorPT::GenerateBlocksToFieldWidth  Public
///
///	\returns An array of field blocks representing the generated row
///
///	Generate blocks up to the field width.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlockMultBase** GameLogicTutorPT::GenerateBlocksToFieldWidth()
{
	static GameFieldBlockMultBase* s_FieldBlocks[ GameFieldMultBase::FIELD_WIDTH + 1 ] = {0};

	// Create as many blocks as needed to fill a row
	const int32 BLOCKS[NUM_ROWS][5] = { {2, 3, 2, 10, 12},
								{2, 2, 2, 2, 5},
								{3, 3, 4, 6, 7},
								{3, 5, 15, 22, 9},
								{7, 2, 8, 3, 18},
								{2, 14, 11, 5, 2}};
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


/// A message handler for when a block is selected that contains the mode-specific logic
GameLogic::EBlockSelAction GameLogicTutorPT::OnBlockSelect( GameFieldBlock* pBlock )
{
	// If the current step is invalid or not a selection step then do nothing special
	if( m_CurStepIndex >= m_Steps.size() )
		return BaseType::OnBlockSelect( pBlock );
	
	// Get the product version of the field block
	GameFieldBlockProduct* pProductBlock = reinterpret_cast<GameFieldBlockProduct*>( pBlock );
	
	// If the next step is to clear a product
	if( m_Steps[m_CurStepIndex].goal.goalType == GT_ClearProduct )
	{
		// If the product was comboable then up the count
		GameFieldPrimeTime* pPTGameField = reinterpret_cast<GameFieldPrimeTime*>( m_pGameFieldMult );
		
		// If this block is not a product or is the correct value then allow it to be selected
		if( !pProductBlock->IsProductBlock() || pProductBlock == pPTGameField->GetComboBlock() )
			return BaseType::OnBlockSelect( pBlock );

		// Get the selected blocks
		const BlockList& selBlocks = m_pGameFieldMult->GetSelectedBlocks();

		// Get the product
		int32 calcProduct = 1;
		for( BlockList::const_iterator iterBlock = selBlocks.begin(); iterBlock != selBlocks.end(); ++iterBlock )
		{
			GameFieldBlockProduct* pCurBlock = reinterpret_cast<GameFieldBlockProduct*>( *iterBlock );

			// If this is the product block then bail
			if( pCurBlock == pProductBlock )
				break;

			calcProduct *= pCurBlock->GetValue();
		}

		// If this block is the correct value then allow it to be selected
		if( calcProduct == pProductBlock->GetValue() && pProductBlock->GetValue() == m_Steps[m_CurStepIndex].goal.data.selValue )
		{
			// The step is completed
			m_Steps[m_CurStepIndex].goal.isCompleted = true;
			return BaseType::OnBlockSelect( pBlock );
		}

		// Don't allow the select
		return GameLogic::BSA_DeselectLast;
	}

	// Complete the step and allow the normal logic to handle the selection
	return BaseType::OnBlockSelect( pBlock );
}