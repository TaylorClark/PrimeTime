/*=================================================================================================

	\file GameLogicProduct.cpp
	Game Play Library
	Product Game Logic Source
	\author Taylor Clark
	\Date June 22, 2006

	This source file contains the implementation of the product game logic manager class.

=================================================================================================*/

#include "../GameLogicProduct.h"
#include "../GameFieldBlockProduct.h"
#include "../GameFieldPrimeTime.h"
#include "../GameFieldCeiling.h"
#include "../GameMgr.h"
#include "Base/MsgLogger.h"
#include "Base/TCAssert.h"
#include "../ProfileSubsetMultiplication.h"
#include "../GUILayout_Game.h"
#include "../PIISetPushSpeed.h"

const ResourceID RESID_SND_RIGHTPROD = 131;
const ResourceID RESID_SND_WRONGPROD = 132;

const uint32 NUM_COMBO_FOR_0x = 5;

bool RandomSuccess( int32 percentChance )
{
	if( (rand() % 100) < percentChance )
		return true;

	return false;
}


/// The destructor
GameLogicProduct::~GameLogicProduct()
{
	// If we have field then free it
	if( m_pGameFieldMult )
	{
		delete m_pGameFieldMult;
		m_pGameFieldMult = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::GetSelectedProduct  Private
///
///	\param pSkipBlock A block that should not be included in the calculation
///	\returns The product from the current selection or -1 on overflow
///
///	Get the product of the player's selection
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 GameLogicProduct::GetSelectedProduct( GameFieldBlockProduct* pSkipBlock ) const
{
	const BlockList& selBlocks = m_pGameFieldMult->GetSelectedBlocks();

	// Calculate the current product
	int32 product = 1;
	for( BlockList::const_iterator iterSelBlock = selBlocks.begin(); iterSelBlock != selBlocks.end(); ++iterSelBlock )
	{
		// Get a pointer to a product block
		GameFieldBlockProduct* pCurProdBlock = reinterpret_cast<GameFieldBlockProduct*>( *iterSelBlock );
		if( pCurProdBlock == pSkipBlock )
			continue;

		// If the factor will cause an overflow then bail
		if( product * pCurProdBlock->GetValue() < product )
			return -1;

		// Multiply the factor into the product
		product *= pCurProdBlock->GetValue();
	}

	return product;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::OnBlockDeselect  Public
///
///	\param pDeslectedBlock The block that was deselected
///
///	Handle a game field block being deselected.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLogic::EBlockSelAction GameLogicProduct::OnBlockDeselect( GameFieldBlock* pDeslectedBlock )
{
	// If the product preview is active and the in-game layout is valid
	if( m_ShowProductPreview && m_pGameGUILayout )
	{
		int32 product = GetSelectedProduct((GameFieldBlockProduct*)pDeslectedBlock);
		if( product < 0 )
			m_pGameGUILayout->SetProduct( 0 );
		else
			m_pGameGUILayout->SetProduct( product );
	}

	return BSA_DeselectLast;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::SubclassOnBlockSelect  Public
///
///	\param pBlock The block which was just selected
///	\returns The action to take to the selected blocks
///
///	A message handler for when a block is selected that contains the mode-specific logic
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLogic::EBlockSelAction GameLogicProduct::OnBlockSelect( GameFieldBlock* pBlock )
{
	if( !pBlock )
		return GameLogic::BSA_None;

	// A redundant check to ensure something is selected
	const BlockList& selBlocks = m_pGameFieldMult->GetSelectedBlocks();
	if( selBlocks.size() == 0 )
		return GameLogic::BSA_None;

	// If we are in prime time mode then get a pointer to Prime Time field pointer
	GameFieldPrimeTime* pPTGameField = 0;
	GameFieldBlockProduct* pComboBlock = 0;
	if( (m_GameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime )
	{
		pPTGameField = reinterpret_cast<GameFieldPrimeTime*>( m_pGameFieldMult );
		pComboBlock = pPTGameField->GetComboBlock();
	}

	// Get a pointer to a product block
	GameFieldBlockProduct* pProductBlock = reinterpret_cast<GameFieldBlockProduct*>( pBlock );

	// If this block is not a product block or it is the combo block
	if( !pProductBlock->IsProductBlock() || pProductBlock == pComboBlock )
	{
		// If the product preview is active and the in-game layout is valid
		if( m_ShowProductPreview && m_pGameGUILayout )
		{
			// Calculate the current product
			int32 product = GetSelectedProduct( 0 );
			if( product < 0 )
				m_pGameGUILayout->SetProduct( 0 );
			else
				m_pGameGUILayout->SetProduct( product );
		}

		// Do not modify the selection
		return BSA_None;
	}

	// Hide the product preview
	if( m_ShowProductPreview && m_pGameGUILayout )
		m_pGameGUILayout->SetProduct( 0 );

	// Handle an equation selection
	return HandleEquation( selBlocks );
}


#ifdef _PT_DEMO
static void OnEndDemoMsgBoxCB( MsgBox::EMsgBoxReturn, void* )
{
	GameMgr::Get().EndGame();
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::HandleEquation  Protected
///
///	\param selBlocks The selected blocks with the product being the last block in the list
///	\returns How to handle the selected blocks
///
///	Handle a valid completed equation.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLogic::EBlockSelAction GameLogicProduct::HandleEquation( const BlockList& selBlocks )
{
	// Get the product block, it is always the last block
	GameFieldBlockProduct* pProductBlock = reinterpret_cast<GameFieldBlockProduct*>( selBlocks.back() );

	// If we are in prime time mode then get a pointer to Prime Time field pointer
	GameFieldPrimeTime* pPTGameField = 0;
	GameFieldBlockProduct* pComboBlock = 0;
	if( (m_GameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime )
	{
		pPTGameField = reinterpret_cast<GameFieldPrimeTime*>( m_pGameFieldMult );
		pComboBlock = pPTGameField->GetComboBlock();
	}

	// Go through all of the selected blocks
	int32 curProduct = 1;
	uint32 calcScore = 0;
	uint32 comboBlocksUsed = 0;
	bool equationContains0x = false;
	for( BlockList::const_iterator iterSelBlock = selBlocks.begin(); iterSelBlock != selBlocks.end(); ++iterSelBlock )
	{
		// Get a pointer to a product block
		GameFieldBlockProduct* pCurSelProdBlock = reinterpret_cast<GameFieldBlockProduct*>( *iterSelBlock );
		if( pCurSelProdBlock->GetValue() == 0 )
		{
			equationContains0x = true;
			break;
		}

		// If this is final product block and is not the combo block then don't multiply it in
		if( pCurSelProdBlock == pProductBlock )
			break;

		// If this is the combo block
		if( pCurSelProdBlock == pComboBlock )
		{
			// If it equals the product block then do nothing
			if( pCurSelProdBlock->GetValue() == pProductBlock->GetValue() )
			{
				// Log the erroneous equation
				m_CurSessionStats.OnErroneousEquation( selBlocks );

				return BSA_DeselectSelection;
			}

			// Up the combo count
			comboBlocksUsed++;
		}

		//calcScore += pCurSelProdBlock->GetValue() * 10 * blockIndex++;
		calcScore += pCurSelProdBlock->GetValue();

		// Multiply the value into the running product
		curProduct *= pCurSelProdBlock->GetValue();
	}

	// If the equation contains the 0x block
	if( equationContains0x )
	{
		// If there are not 2 numbers then this can't work
		if( selBlocks.size() != 2 )
			return BSA_DeselectSelection;

		// Remove the blocks
		return BSA_RemoveEntireSelection;
	}

	// Multiply the score by the number of factors
	calcScore *= (uint32)selBlocks.size() - 1;

	bool isSuccessfulEquation = curProduct == pProductBlock->GetValue();

	// If the game is in Prime Time mode
	if( pPTGameField )
	{
		// If the product was comboable then up the count
		if( pPTGameField->IsBlockComboable( pProductBlock ) )
			comboBlocksUsed++;

		// Scale the score
		float32 scoreMult = 1.0f + ((float32)m_ComboCount / 10.0f);
		calcScore = (uint32)( (float32)calcScore * scoreMult );

		// If no combo blocks were used then clear the count
		if( comboBlocksUsed == 0 )
		{
			m_ComboCount = 0;
			pPTGameField->ClearComboable();
		}
		// Else combo blocks were used
		else
			m_ComboCount += comboBlocksUsed;


		// On a successful equation then update the comboable blocks
		if( isSuccessfulEquation )
		{
			m_CurSessionStats.OnCombo( m_ComboCount );

			pPTGameField->AddToComboBlocksFromSelection();

			// COMMENTED OUT UNTIL 0 BLOCK CAN BE REDESIGNED
			// If this is the fifth combo then mark the flag to generate a 0x block
			//if( m_ComboCount > 0 && m_ComboCount % NUM_COMBO_FOR_0x == 0 )
			//	m_ReadyFor0xBlock = true;
		}
		// Else the equation is erroneous then clear the count and the comboable blocks
		else
		{
			m_ComboCount = 0;
			pPTGameField->ClearComboable();
		}

		// Update the score multiplier
		scoreMult = 1.0f + ((float32)m_ComboCount / 10.0f);
		if( m_pGameGUILayout )
			m_pGameGUILayout->SetComboCount( m_ComboCount );
	}

	// If the product doesn't match the product block product
	if( !isSuccessfulEquation )
	{
		// Log the erroneous equation
		m_CurSessionStats.OnErroneousEquation( selBlocks );

		// Play the failed sound
		AudioSampleHndl wrongSound = ResourceMgr::Get().GetAudioSample( RESID_SND_WRONGPROD );
		if( wrongSound.GetObj() )
			wrongSound.GetObj()->Play();
		
		// If it wasn't successful then deselect everything
		return BSA_DeselectSelection;
	}

	// Play the success sound
	AudioSampleHndl rightSound = ResourceMgr::Get().GetAudioSample( RESID_SND_RIGHTPROD );
	if( rightSound.GetObj() )
		rightSound.GetObj()->Play();

	// Update the block counts and score
	m_NumProducts--;
	m_NumPrimes -= (int32)selBlocks.size() - 1;
	m_CurSessionStats.m_Score += calcScore;
	m_NumCompletedEquations++;

	// Update the in-game layout
	if( m_pGameGUILayout )
	{
		m_pGameGUILayout->SetCount( m_NumCompletedEquations );
		m_pGameGUILayout->SetScore( m_CurSessionStats.m_Score );
	}
	
	// Set the combo block
	if( pPTGameField )
		pPTGameField->SetComboBlock( pProductBlock->GetValue() );
	
	// If we have completed enough equations to level up
	if( m_NumCompletedEquations % GameDefines::NUM_CLEAR_PER_LEVEL == 0 && (m_GameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime )
	{
#ifdef _PT_DEMO
		if( m_CurrentLevel >= 3 )
		{
			// End the game and display a message box
			MsgBox* pDemoOverBox = MsgBox::CreateMsgBox( L"That's the end of the demo. Please visit\nwww.PrimeTimeMath.net\nto continue the adventure by ordering the full version. Thanks for playing!" );
			GameMgr::Get().PopupMsgBox( pDemoOverBox, OnEndDemoMsgBoxCB, NULL );

			//GameMgr::Get().Se
			return BSA_RemoveEntireSelection;
		}
#endif
		// Up the difficulty
		SetDifficulty( m_CurrentLevel + 1 );
	}

	// Log the valid equation
	m_CurSessionStats.OnValidEquation( selBlocks );

	// Remove the selected blocks
	return BSA_RemoveEntireSelection;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::GetFilterFromDiff  Static Public
///
///	\param diff The difficulty level
///	\param level The game mode dependent level
///	\returns A filter to be used to generate products
///
///	Get a product filter based on a gameplay difficulty level
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameDefines::ProductSubsetFilter GameLogicProduct::GetFilterFromDiff( GameDefines::EGameplayDiffLevel diff, int32 level )
{
	GameDefines::ProductSubsetFilter retFilter;

	retFilter.minNumFactors = 1;
	retFilter.minValue = 0;

	// Calculate the number variables based on level
	int32 gameplayDiffVal = (int32)diff;
	uint32 maxPrimeIndex = GameDefines::NUM_PRIMES;
	switch( diff )
	{
		case GameDefines::GPDL_Pre3:
			{
				retFilter.maxNumFactors = 2;

				retFilter.maxValue = 10 + (5 * level);

				if( level > 8 )
				{
					retFilter.maxNumFactors = 5;
					maxPrimeIndex = (GameDefines::NUM_PRIMES * 3) / 4;
				}
				else if( level > 5 )
				{
					retFilter.maxNumFactors = 4;
					maxPrimeIndex = GameDefines::NUM_PRIMES / 2;
				}
				else if( level > 2 )
					maxPrimeIndex = 3;
				else if( level > 1 )
					maxPrimeIndex = 2;
				else
					maxPrimeIndex = 1;
			}
			break;

		case GameDefines::GPDL_3:
			{
				retFilter.maxNumFactors = 3;

				retFilter.maxValue = 12 + (7 * level);

				if( level > 8 )
					maxPrimeIndex = (GameDefines::NUM_PRIMES * 3) / 4;
				else if( level > 6 )
					maxPrimeIndex = GameDefines::NUM_PRIMES / 2;
				else if( level > 4 )
					maxPrimeIndex = 4;
				else if( level > 2 )
					maxPrimeIndex = 3;
				else
					maxPrimeIndex = 2;
			}
			break;

		case GameDefines::GPDL_4:
		case GameDefines::GPDL_5:
		case GameDefines::GPDL_6:
			{
				retFilter.maxNumFactors = (gameplayDiffVal - (int32)GameDefines::GPDL_4) + 3;

				retFilter.maxValue = 30 + (3 * gameplayDiffVal) * level;

				if( level > 15 )
					maxPrimeIndex = (GameDefines::NUM_PRIMES * 4) / 5;
				else if( level > 12 )
					maxPrimeIndex = (GameDefines::NUM_PRIMES * 3) / 4;
				else if( level > 9 )
					maxPrimeIndex = GameDefines::NUM_PRIMES / 3;
				else if( level > 6 )
					maxPrimeIndex = GameDefines::NUM_PRIMES / 4;
				else
					maxPrimeIndex = 2;
			}
			break;

		case GameDefines::GPDL_7:
		case GameDefines::GPDL_8:
			{
				retFilter.maxNumFactors = 9;

				int32 levelMult = level;
				levelMult += (gameplayDiffVal - (int32)GameDefines::GPDL_7) * 2;

				retFilter.maxValue = 75 + (20 * levelMult);

				if( level > 6 )
					maxPrimeIndex = GameDefines::NUM_PRIMES - 1;
				if( level > 3 )
					maxPrimeIndex = GameDefines::NUM_PRIMES - 2;
				else
					maxPrimeIndex = GameDefines::NUM_PRIMES - 3;
			}
			break;

		case GameDefines::GPDL_Post8:
		default:
			{
				retFilter.maxNumFactors = 20;

				retFilter.maxValue = 100 + (40 * level);

				maxPrimeIndex = GameDefines::NUM_PRIMES;
			}
			break;

		case GameDefines::GPDL_Secret1:
			{
				retFilter.maxNumFactors = 20;

				retFilter.maxValue = 500 + (75 * level);

				maxPrimeIndex = GameDefines::NUM_PRIMES;
			}
			break;

		case GameDefines::GPDL_Secret2:
			{
				retFilter.maxNumFactors = 20;

				retFilter.maxValue = 1200 + (100 * level);

				maxPrimeIndex = GameDefines::NUM_PRIMES;
			}
			break;
	}

	// Ensure a valid prime index
	if( maxPrimeIndex < 1 )
		maxPrimeIndex = 1;
	else if( maxPrimeIndex >= GameDefines::NUM_PRIMES )
		 maxPrimeIndex = GameDefines::NUM_PRIMES - 1;
	retFilter.SetMaxPrimeByIndex( maxPrimeIndex );

	// Return the calculated filter
	return retFilter;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::SetDifficulty  Protected
///
///	\param level The game mode dependent difficulty level
///
///	Setup the product generation filter based on the gameplay level.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicProduct::SetDifficulty( int32 level )
{
	// If this is practice then do nothing
	if( m_IsPractice )
		return;

	// Store the level
	if( level < 1 )
		level = 1;
	m_CurrentLevel = level;
	if( m_pGameGUILayout )
		m_pGameGUILayout->SetLevel( level );
	m_CurSessionStats.m_LevelReached = level;

	// If the game is in ceiling mode then double the effective level for block generation
	if( (m_GameType & GameDefines::GT_Mask_Type) == GameDefines::GT_Ceiling )
		level *= 2;
	// Otherwise the game is not in ceiling mode so increase the speed
	else
	{
		// Update the push speed
		m_pGameFieldMult->SendInstruction( new PIISetPushSpeed( GameLogic::GetPushUpSpeed( m_CurrentLevel ) ) );
	}

	// Get the new product filter
	m_ProdFilter = GetFilterFromDiff( GameMgr::Get().GetGameplayDiff(), level );

	// Add up the prime weights to be used when generating a prime
	m_PrimeRandRange = 0;
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		if( (m_ProdFilter.useablePrimes & (1 << primeIndex)) != 0 )
			m_PrimeRandRange += GameDefines::PRIME_WEIGHTS[primeIndex];
	}

	// Get the new array of products
	m_GeneratableProducts = GameDefines::GetProductArraySubset( m_ProdFilter );

	// Ensure a valid array
	if( m_GeneratableProducts.size() == 0 )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to generate a valid subset." );

		// Add a product so something can be generated
		GameDefines::ProductEntry tempProd;
		tempProd.product = 12;
		tempProd.primeFactors.push_back( 2 );
		tempProd.primeFactors.push_back( 2 );
		tempProd.primeFactors.push_back( 3 );
		m_GeneratableProducts.push_back( tempProd );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::SubclassInit  Private
///
///	\returns Whether or not the logic was successfully initialized
///
///	Initialize the product game logic after initializing the base class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameLogicProduct::SubclassInit()
{
	// Clear the data
	m_NumProducts = 0;
	m_NumPrimes = 0;
	m_NumCompletedEquations = 0;
	m_ComboCount = 0;
	m_CurrentLevel = 1;
	m_ReadyFor0xBlock = false;
	
	// If there is a field then clear it
	if( m_pGameFieldMult )
	{
		delete m_pGameFieldMult;
		m_pGameFieldMult = 0;
	}

	// Create a new field
	if( (m_GameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime )
		m_pGameFieldMult = new GameFieldPrimeTime();
	else
		m_pGameFieldMult = new GameFieldCeiling();
	if( !m_pGameFieldMult )
		return false;

	// Initialize the field
	m_pGameFieldMult->m_FieldBox.Set( 389, 61, GameField::FIELD_WIDTH * 55, GameField::FIELD_HEIGHT * 55 );
	m_pGameFieldMult->Init();
	m_pGameFieldMult->SendInstruction( new PIISetPushSpeed( GameLogic::GetPushUpSpeed(1) ) );

	// Initialize the stats
	m_CurSessionStats.Clear();
	m_CurSessionStats.m_GameType = GameMgr::Get().GetGameType();
	m_CurSessionStats.m_Difficulty = GameMgr::Get().GetGameplayDiff();
	m_CurSessionStats.m_TimeLimit = GameMgr::Get().GetTimeLimit();
	m_CurSessionStats.StoreStartTime();

	// Set the difficulty
	SetDifficulty( 1 );

	// Pre-cache the raise, right, and wrong sound
	//ResourceMgr::Get().GetAudioSample( RESID_SND_RAISE );
	ResourceMgr::Get().GetAudioSample( RESID_SND_RIGHTPROD );
	ResourceMgr::Get().GetAudioSample( RESID_SND_WRONGPROD );

	// Initialize the UI
	if( m_pGameGUILayout )
	{
		m_pGameGUILayout->SetScore( 0 );
		m_pGameGUILayout->SetLevel( 1 );
		m_pGameGUILayout->SetCount( 0 );

		if( m_TimeLimit > 0 )
			m_pGameGUILayout->SetTimePlayed( m_TimeLimit );
		else
			m_pGameGUILayout->SetTimePlayed( 0 );

		// Display the combo block label if this is Prime Time mode
		m_pGameGUILayout->InitFromGameType( m_GameType );
	}

	/// Setup the ceiling field
	if( (m_GameType & GameDefines::GT_Mask_Type) == GameDefines::GT_Ceiling )
		SetupCeilingField();

	m_pGameFieldMult->DropNewBlocks( GenerateBlocksToFieldWidth() );
	m_pGameFieldMult->DropNewBlocks( GenerateBlocksToFieldWidth() );
	m_pGameFieldMult->DropNewBlocks( GenerateBlocksToFieldWidth() );

	// Return success
	return true;
}


/// Setup the ceiling field
void GameLogicProduct::SetupCeilingField()
{
	// If the game type is not ceiling then bail
	if( (m_GameType & GameDefines::GT_Mask_Type) != GameDefines::GT_Ceiling )
		return;

	const int32 MAX_CEILING_OFFSET = 3;
	int32 ceilingOffsets[ GameField::FIELD_WIDTH ] = {0};
	GameFieldCeiling* pCeilingField = (GameFieldCeiling*)m_pGameFieldMult;
	if( !pCeilingField )
		return;

	int32 numTries = 0;
	const int32 MAX_TRIES = 3;
	while( numTries++ < MAX_TRIES )
	{
		// Generate the random values for the offsets
		int32 minColOffset = MAX_CEILING_OFFSET;
		for( int32 colIndex = 0; colIndex < GameField::FIELD_WIDTH; ++colIndex )
		{
			ceilingOffsets[colIndex] = rand() % MAX_CEILING_OFFSET;
			if( ceilingOffsets[colIndex] < minColOffset )
				minColOffset = ceilingOffsets[colIndex];
		}

		// We need to at least have one column empty
		while( minColOffset-- > 0 )
			for( int32 colIndex = 0; colIndex < GameField::FIELD_WIDTH; ++colIndex )
				ceilingOffsets[colIndex]--;

		// If all of the columns are empty then try again
		int32 numOffset = 0;
		for( int32 colIndex = 0; colIndex < GameField::FIELD_WIDTH; ++colIndex )
				numOffset += ceilingOffsets[colIndex];
		if( numOffset != 0 )
			break;
	}

	// If we unsuccessfully set up the field then make a default setup
	if( numTries >= MAX_TRIES )
	{
		ceilingOffsets[0] = 0;
		ceilingOffsets[1] = 2;
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Failed to setup ceiling field successfully." );
	}

	pCeilingField->SetOffsets( ceilingOffsets );
}


/// Is the game over
bool GameLogicProduct::IsGameOver()
{
	// If there is a field then check it
	bool isGameOver = false;
	if( m_pGameFieldMult )
	{
		isGameOver = m_pGameFieldMult->IsGameOver();

		// If we are in ceiling mode
		if( isGameOver && (m_GameType & GameDefines::GT_Mask_Type) == GameDefines::GT_Ceiling )
		{
			// Stop the raise sound
			if( _playingRaiseSound.IsValid() )
				_playingRaiseSound.StopAndRelease();

			// Was the field successful
			GameFieldCeiling* pCeilingField = reinterpret_cast<GameFieldCeiling*>( m_pGameFieldMult );
			if( pCeilingField && pCeilingField->IsVictorious() )
			{
				isGameOver = false;

				m_CurSessionStats.m_LevelReached = m_CurrentLevel + 1;

				GameMgr::Get().SetPauseState( true );

				// Create a message box
				MsgBox* pNextLevelMsgBox = MsgBox::CreateMsgBox( L"Level Completed!", MsgBox::BL_OK );
				GameMgr::Get().PopupMsgBox( pNextLevelMsgBox, OnNextLevelMBCallback, this );
			}
		}
	}

	return isGameOver;
}


void GameLogicProduct::OnNextLevelMBCallback( MsgBox::EMsgBoxReturn, void* pUserData )
{
	GameLogicProduct* pLogic = (GameLogicProduct*)pUserData;
	if( !pLogic )
		return;

	// Advance the level
	pLogic->SetDifficulty( pLogic->m_CurrentLevel + 1 );
	pLogic->m_pGameFieldMult->ClearField();
	pLogic->m_pGameFieldMult->Init();
	pLogic->OnSpeedButtonUp();

	// Refresh the ceiling blocks
	pLogic->SetupCeilingField();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::Term  Private
///
///	Close down the game logic.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicProduct::Term()
{
	m_CurSessionStats.m_Duration = GameMgr::Get().GetElapsedTime();

	// Get the profile
	PlayerProfile* pProfile = GameMgr::Get().GetActiveProfile();
	ProfileSubsetMultiplication* pMultiplicationProfile = 0;
	if( pProfile )
		pMultiplicationProfile = (ProfileSubsetMultiplication*)pProfile->GetSubset( ProfileSubsetMultiplication::SUBSET_ID );

	// End the session with the profile
	if( pMultiplicationProfile )
		pMultiplicationProfile->StoreSession( m_CurSessionStats );

	// Stop the raise sound if there is one
	if( _playingRaiseSound.IsValid() )
		_playingRaiseSound.StopAndRelease();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::GenerateBlocksToFieldWidth()  Public
///
///	\returns A null-terminated array of field blocks that make up a full field row
///
///	Generate a row of blocks.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlockMultBase** GameLogicProduct::GenerateBlocksToFieldWidth()
{
	static GameFieldBlockMultBase* s_FieldBlocks[ GameField::FIELD_WIDTH + 1 ] = {0};

	// If it is time to generate a 0x block then determine what index to place the block
	int32 block0xPos = GameField::FIELD_WIDTH;
	if( m_ReadyFor0xBlock )
		block0xPos = rand() % GameField::FIELD_WIDTH;

	// Create as many blocks as needed to fill a row
	int32 curWidth = 0;
	int32 curBlockIndex = 0;
	while( curWidth < GameField::FIELD_WIDTH )
	{
		// If it is time for a 0x block
		GameFieldBlockProduct* pNewBlock = 0;
		if( curWidth >= block0xPos )
		{
			// Clear the flag and position
			m_ReadyFor0xBlock = false;
			block0xPos = GameField::FIELD_WIDTH;

			// Create the block
			pNewBlock = new GameFieldBlockProduct( 0, false );

			// Set the new block's sprite
			pNewBlock->sprite = ResourceMgr::Get().GetRefSprite( GameDefines::PRIME_SPRITE_0x );
		}
		// Else generate a new block
		else
			pNewBlock = GenerateBlock( GameField::FIELD_WIDTH - curWidth );

		// Set the position
		pNewBlock->colIndex = curWidth;

		// Store the new clock
		s_FieldBlocks[curBlockIndex++] = pNewBlock;

		// Add in the new block's width to the current width
		curWidth += pNewBlock->size.x;
	}

	// Clear the rest of the game field block pointers in the static arrays for safety
	do
	{
		s_FieldBlocks[ curBlockIndex ] = NULL;
	}while( ++curBlockIndex < GameField::FIELD_WIDTH );

	// Return the array
	return s_FieldBlocks;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::InitPractice()  Public
///
/// Initialize a practice game.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicProduct::InitPractice( GameDefines::ProductSubsetFilter prodFilter )
{
	// Set the flag
	m_IsPractice = true;
	m_ShowProductPreview = true;

	// Store the filter
	m_ProdFilter = prodFilter;

	// Generate the products
	m_GeneratableProducts = GameDefines::GetProductArraySubset( m_ProdFilter );
	if( m_GeneratableProducts.size() == 0 )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to generate a valid subset for practice mode." );

		// Add a product so something can be generated
		GameDefines::ProductEntry tempProd;
		tempProd.product = 4;
		tempProd.primeFactors.push_back( 2 );
		tempProd.primeFactors.push_back( 2 );
		m_GeneratableProducts.push_back( tempProd );
	}

	// Update the layout
	//if( m_pGameGUILayout )
	//	m_pGameGUILayout->UpdateForPrimeTimePractice();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicProduct::GenerateBlock()  Public
///	\param UNNAMED The elapsed frame time in seconds
///
///	Update the user interface.
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlockProduct* GameLogicProduct::GenerateBlock( int32 maxWidth )
{
	// Get the value
	int32 value = 2;
	ResourceID spriteResID = RESID_SPRITE_2_BLOCK;

	const int32 CHANCE_OF_NEEDED = 75;

	// 75% of the time we will generate a block that helps the player
	bool forceGenerateProduct = false;
	uint32 forceGeneratePrimeIndex = GameDefines::NUM_PRIMES;
	if( m_IsPractice || RandomSuccess(CHANCE_OF_NEEDED) )
	{
		// Go through the primes and see which are in demand
		for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
		{
			// If the block is not needed on the field then skip it
			if( m_PrimeOffsets[ primeIndex ] >= 0 )
				continue;

			// If we already have a factor that is needed
			if( forceGeneratePrimeIndex != GameDefines::NUM_PRIMES )
			{
				// If this factor is needed more than the previous
				if( m_PrimeOffsets[ primeIndex ] < m_PrimeOffsets[forceGeneratePrimeIndex] )
					forceGeneratePrimeIndex = primeIndex;
				// Else if the two are equal then give a 50% chance to use this prime
				else if( m_PrimeOffsets[ primeIndex ] == m_PrimeOffsets[forceGeneratePrimeIndex] && RandomSuccess(50) )
					forceGeneratePrimeIndex = primeIndex;
			}
			// Else store this index
			else
				forceGeneratePrimeIndex = primeIndex;
		}

		// If there is no factor needed then generate a product
		if( forceGeneratePrimeIndex == GameDefines::NUM_PRIMES )
			forceGenerateProduct = true;
	}

	// A 25% chance to make a product
	bool shouldMakeProduct = (RandomSuccess(25) || forceGenerateProduct) && forceGeneratePrimeIndex == GameDefines::NUM_PRIMES;
	if( shouldMakeProduct )
	{
		// Use the empty block sprite
		spriteResID = RESID_SPRITE_PRODUCT_BLOCK;
		uint32 productIndex = (uint32)(rand() % m_GeneratableProducts.size());
		
		// Store the value
		value = m_GeneratableProducts[ productIndex ].product;
		if( value > 99 )
		{
			// If the width needs to be capped then don't allow a large number
			if( maxWidth < 2 )
			{
				productIndex = 0;
				value = m_GeneratableProducts[ productIndex ].product;

				// If the backup product is still too large
				if( value > 99 )
				{
					// Generate a prime
					shouldMakeProduct = false;
					goto MakePrime;
				}
			}
			else
				spriteResID = RESID_SPRITE_DOUBLE_PRODUCT_BLOCK;
		}

		// If a product is still being generated
		if( shouldMakeProduct )
		{
			// Update the prime value counts by decrement the offset.  We added a product so we
			// decrement the count to mean we need one more.
			for( uint32 factorIndex = 0; factorIndex < m_GeneratableProducts[ productIndex ].primeFactors.size(); ++factorIndex )
			{
				uint32 primeIndex = GameDefines::PrimeToIndex( m_GeneratableProducts[ productIndex ].primeFactors[factorIndex] );
				m_PrimeOffsets[ primeIndex ]--;
			}

			// Keep track of the number of products
			m_NumProducts++;
		}
	}
	// Else we are generating a prime number
	else
	{
MakePrime:
		uint32 createPrimeIndex = 0;
		// If a specific prime is to be generated
		if( forceGeneratePrimeIndex != GameDefines::NUM_PRIMES )
			createPrimeIndex = forceGeneratePrimeIndex;
		// Else randomly generate the prime
		else
		{
			uint32 randValue = (uint32)rand() % m_PrimeRandRange;
			uint32 randSum = 0;
			for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
			{
				// If this product is not used then skip it
				if( (m_ProdFilter.useablePrimes & (1 << primeIndex)) == 0 )
					continue;

				randSum += GameDefines::PRIME_WEIGHTS[primeIndex];

				// If the value is in this range then use the prime
				if( randSum > randValue )
				{
					createPrimeIndex = primeIndex;
					break;
				}
			}
		}

		// Store the value
		value = GameDefines::PRIMES[ createPrimeIndex ];
		spriteResID = GameDefines::PRIME_SPRITE_ID[ createPrimeIndex ];

		// Increment the offset needed since we just added one, meaning we need one less
		m_PrimeOffsets[ createPrimeIndex ]++;

		// Keep track of the number of primes
		m_NumPrimes++;
	}

	// Create the new block
	GameFieldBlockProduct* pNewBlock = new GameFieldBlockProduct( value, shouldMakeProduct );

	// Set the new block's sprite
	pNewBlock->sprite = ResourceMgr::Get().GetRefSprite( spriteResID );


	// Return the new block
	return pNewBlock;
}