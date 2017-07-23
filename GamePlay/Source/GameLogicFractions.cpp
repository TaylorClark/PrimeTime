/*=================================================================================================

	\file GameLogicFractions.cpp
	Game Play Library
	Fractions Game Logic Source
	\author Taylor Clark
	\Date December 24, 2009

	This source file contains the implementation of the fractions game logic manager class.

=================================================================================================*/

#include "../GameLogicFractions.h"
#include "../GameFieldFractions.h"
#include "../GameFieldBlockFraction.h"
#include "../PIISetPushSpeed.h"
#include "../GUILayout_Game.h"
#include "GUI/GUIMgr.h"
#include "../GameMgr.h"
#include "Base/NumFuncs.h"

const ResourceID RESID_SND_RIGHTPROD = 131;
const ResourceID RESID_SND_WRONGPROD = 132;

static std::vector<FractionModeSettings> g_FractionModeSettings(  (int32)GameDefines::GPDL_COUNT );

static void PopulateFractionModes();


bool IsEquivalentFraction( int32 numerator1, int32 denominator1, int32 numerator2, int32 denominator2 )
{
	if( denominator1 == 0 || denominator2 == 0 )
		return false;

	return (numerator1 * denominator2) == (numerator2 * denominator1);
}

bool IsEquivalentFraction( Fraction lhs, Fraction rhs )
{
	return IsEquivalentFraction(lhs.Numerator, lhs.Denominator, rhs.Numerator, rhs.Denominator);
}

template<class T>
T GetRandomEntry( const std::list<T>& list )
{
	uint32 randIndex = rand() % list.size();

	uint32 curIndex = 0;
	typename std::list<T>::const_iterator iterItem = list.begin();
	for( ; curIndex < randIndex; ++iterItem )
	{
		++curIndex;
	}		

	return *iterItem;
}

Fraction GameLogicFractions::GetNextFraction() const
{
	// Find the most common fraction
	/*
	uint32 selectedFractionIndex = 0;
	for( uint32 fractionIndex = 1; fractionIndex < _pCurModeSettings->AvailableFractions.size(); ++fractionIndex )
	{
		if( _solutionFractionPossibilities[fractionIndex] > _solutionFractionPossibilities[selectedFractionIndex] )
			selectedFractionIndex = fractionIndex;
	}
	*/

	std::list<uint32> possibleNextFractions;
	for( uint32 fractionIndex = 0; fractionIndex < _pCurModeSettings->AvailableFractions.size(); ++fractionIndex )
	{
		if( _solutionFractionPossibilities[fractionIndex] > 0 )
			possibleNextFractions.push_back( fractionIndex );
	}

	if( possibleNextFractions.size() == 0 )
		return Fraction(1,2);

	return _pCurModeSettings->GetRandomFraction( GetRandomEntry(possibleNextFractions) );
}


bool IsFractionWithinLength( float32 ratio, int32 length )
{
	// 2^-17
	const float32 MIN_VALUE = 0.00000762939453125f;

	float32 powOfTen = powf( 10.0f, (float)length );

	float32 flooredVal = floorf( ratio * powOfTen ) / powOfTen;
	float32 diff = fabs( flooredVal - ratio );

	return diff < MIN_VALUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicFractions::OnBlockSelect  Public
///	\param pBlock The block which was just selected
///	\returns The action to take to the selected blocks
///
///	A message handler for when a block is selected that contains the mode-specific logic
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLogic::EBlockSelAction GameLogicFractions::OnBlockSelect( GameFieldBlock* pBlock )
{
	// If the player clicked on the numerator block off to the side
	if( pBlock == _pGameFieldFractions->pNumeratorPlaceholderBlock )
	{
		_pGameFieldFractions->IsNumeratorSelected = true;

		// If the user has a selected numerator value then clear it
		if( _pGameFieldFractions->GetSelectedNumerator() != -1 )
		{
			_pGameFieldFractions->SetSelectedNumerator( -1 );
			return GameLogic::BSA_DeselectSelection;
		}
		// Otherwise deselect the last block because the numerator is not a valid block for
		// selection
		else		
			return GameLogic::BSA_DeselectLast;
	}
	// Or if the player clicked on the numerator block off to the side
	else if( pBlock == _pGameFieldFractions->pDenominatorPlaceholderBlock )
	{
		_pGameFieldFractions->IsNumeratorSelected = false;

		// If the user has a selected numerator value then clear it
		if( _pGameFieldFractions->GetSelectedDenominator() != -1 )
		{
			_pGameFieldFractions->SetSelectedDenominator( -1 );
			return GameLogic::BSA_DeselectSelection;
		}
		// Otherwise deselect the last block because the denominator is not a valid block for
		// selection
		else		
			return GameLogic::BSA_DeselectLast;
	}

	// If the player is setting the numerator
	if( _pGameFieldFractions->IsNumeratorSelected )
	{
		_pGameFieldFractions->SetSelectedNumerator( pBlock->GetValue() );

		_pGameFieldFractions->IsNumeratorSelected = false;
	}
	// Otherwise the player is setting the denominator
	else
	{
		_pGameFieldFractions->SetSelectedDenominator( pBlock->GetValue() );

		_pGameFieldFractions->IsNumeratorSelected = true;
	}

	// If we have a numerator and denominator then let's test the fraction
	if( _pGameFieldFractions->GetSelectedBlocks().size() >= 2 )
	{
		Fraction selectedFraction(_pGameFieldFractions->GetSelectedNumerator(), _pGameFieldFractions->GetSelectedDenominator() );

		bool isValidFraction = IsEquivalentFraction( _equivalentFraction.Numerator, _equivalentFraction.Denominator, selectedFraction.Numerator, selectedFraction.Denominator );

		// Whether or not the fraction is correct we need to clear the selection
		_pGameFieldFractions->SetSelectedNumerator( -1 );
		_pGameFieldFractions->SetSelectedDenominator( -1 );
		_pGameFieldFractions->IsNumeratorSelected = true;

		if( isValidFraction )
		{
			// Update the possible solution count
			for( uint32 fractionIndex = 0; fractionIndex < _pCurModeSettings->AvailableFractions.size(); ++fractionIndex )
			{
				// If we just cleared a fraction equivalent to this fraction then decrement the count
				if( IsEquivalentFraction( _pCurModeSettings->AvailableFractions[fractionIndex].FractionValue, _equivalentFraction ) )
				{
					--_solutionFractionPossibilities[fractionIndex];
					break;
				}
			}

			// Update the stats
			_curSessionStats.m_NumFractionsCleared++;
			_curSessionStats.m_Score += selectedFraction.Numerator + selectedFraction.Denominator;
			m_pGameGUILayout->SetScore( _curSessionStats.m_Score );

			m_pGameGUILayout->SetCount( _curSessionStats.m_NumFractionsCleared );

			// If the player cleared the certain number of fractions then increase the speed
			if( _curSessionStats.m_NumFractionsCleared % GameDefines::NUM_CLEAR_PER_LEVEL == 0 )
			{
				++m_CurrentLevel;
				m_pGameGUILayout->SetLevel( m_CurrentLevel );
				_pGameFieldFractions->SendInstruction( new PIISetPushSpeed( GameLogic::GetPushUpSpeed(m_CurrentLevel) ) );
			}

			Fraction nextFraction = GetNextFraction();

			_equivalentFraction = nextFraction;

			float32 ratio = (float32)_equivalentFraction.Numerator / (float32)_equivalentFraction.Denominator;

			bool showDecimal = ((rand() % 2) == 0) && IsFractionWithinLength( ratio, _pCurModeSettings->DecimalPlaces );

			// Get the string showing the next fraction
			std::wstring sNextString;
			if( showDecimal )
			{
				wchar_t numBuffer[8] = {0};

				swprintf( numBuffer, 8, L"%.*g", _pCurModeSettings->DecimalPlaces, ratio );

				sNextString = numBuffer;
			}
			else
			{
				sNextString = TCBase::EasyIToA( _equivalentFraction.Numerator );
				sNextString += L"/";
				sNextString += TCBase::EasyIToA( _equivalentFraction.Denominator );
			}

			// If the string is changing then flash it to make it more noticeable
			if( m_pGameGUILayout->GetLabelText(GUILayoutInGame::L_CurrentProd) != sNextString )
				m_pGameGUILayout->FlashLabel(GUILayoutInGame::L_CurrentProd);

			m_pGameGUILayout->SetLabelText( GUILayoutInGame::L_CurrentProd, sNextString.c_str() );

			// Update the value counts
			const BlockList& selectedBlocks = _pGameFieldFractions->GetSelectedBlocks();
			for( BlockList::const_iterator iterCurBlock = selectedBlocks.begin(); iterCurBlock != selectedBlocks.end(); ++iterCurBlock )
				--_fractionValueCounts[ (*iterCurBlock)->GetValue() ];

			// Play the success sound
			AudioSampleHndl rightSound = ResourceMgr::Get().GetAudioSample( RESID_SND_RIGHTPROD );
			if( rightSound.GetObj() )
				rightSound.GetObj()->Play();

			return GameLogic::BSA_RemoveEntireSelection;
		}
		// Otherwise the fraction is invalid
		else
		{
			_curSessionStats.m_NumFractionsFailed++;

			// Play the failed sound
			AudioSampleHndl wrongSound = ResourceMgr::Get().GetAudioSample( RESID_SND_WRONGPROD );
			if( wrongSound.GetObj() )
				wrongSound.GetObj()->Play();

			// The selection wasn't equivalent so deselect everything
			return GameLogic::BSA_DeselectSelection;
		}
	}

	return GameLogic::BSA_None;
}


void GameLogicFractions::SetDifficulty( int32 difficulty )
{
	PopulateFractionModes();
	
	if( difficulty < 0 )
		difficulty = 0;
	else if( difficulty >= (int32)g_FractionModeSettings.size() )
		difficulty = g_FractionModeSettings.size() - 1;

	_pCurModeSettings = &g_FractionModeSettings[ difficulty ];

	int32 numFractions = _pCurModeSettings->AvailableFractions.size();
	int32 maxFractionValue = _pCurModeSettings->MaxValue;

	_solutionFractionPossibilities.resize( numFractions );
	_fractionValueCounts.resize( maxFractionValue );

	// Clear the possible solution counts
	for( int32 fractionIndex = 0; fractionIndex < numFractions; ++fractionIndex )
		_solutionFractionPossibilities[fractionIndex] = 0;

	// Clear the fraction value counts
	for( int32 fractionValueIndex = 0; fractionValueIndex < maxFractionValue; ++fractionValueIndex )
		_fractionValueCounts[fractionValueIndex] = 0;
}


void GameLogicFractions::CalculatePossibleSolutionCounts()
{
	// Go through each fraction for the current difficulty
	for( uint32 curFractionIndex = 0; curFractionIndex < _pCurModeSettings->AvailableFractions.size(); ++curFractionIndex )
	{
		_solutionFractionPossibilities[curFractionIndex] = 0;

		const FractionModeSettings::FractionInfo& curFractionInfo = _pCurModeSettings->AvailableFractions[curFractionIndex];

		for( std::set<int32>::const_iterator iterNumerator = curFractionInfo.ValuesUpToMax.begin();
				iterNumerator != curFractionInfo.ValuesUpToMax.end();
				++iterNumerator )
		{
			// If there are no blocks of this value then skip it
			if( _fractionValueCounts[*iterNumerator] == 0 )
				continue;

			std::set<int32>::const_iterator iterDenominator = iterNumerator;
			for( ++iterDenominator;
				iterDenominator != curFractionInfo.ValuesUpToMax.end();
				++iterDenominator )
			{
				// If there are no blocks of this value then skip it
				if( _fractionValueCounts[*iterDenominator] == 0 )
					continue;

				// If this combination of numerator and denominator values isn't equivalent to the
				// fraction we are testing then skip it
				if( !IsEquivalentFraction(*iterNumerator, *iterDenominator, curFractionInfo.FractionValue.Numerator, curFractionInfo.FractionValue.Denominator ) )
					continue;

				// Get the number of possible fractions that can be selected for the current fraction
				uint8 minFractionValueCount = std::min(_fractionValueCounts[*iterNumerator], _fractionValueCounts[*iterDenominator]);

				_solutionFractionPossibilities[curFractionIndex] += minFractionValueCount;
			}
		}
	}
}



/// A message handler for when a block is deselected
GameLogic::EBlockSelAction GameLogicFractions::OnBlockDeselect( GameFieldBlock* )
{
	// If the numerator has a value and we just cleared the block selection then it must be the
	// denominator's value that we cleared
	if( _pGameFieldFractions->IsNumeratorSelected )
		_pGameFieldFractions->SetSelectedDenominator( -1 );
	else
		_pGameFieldFractions->SetSelectedNumerator( -1 );

	_pGameFieldFractions->IsNumeratorSelected = !_pGameFieldFractions->IsNumeratorSelected;
	
	return GameLogic::BSA_DeselectLast;
}


bool GameLogicFractions::SubclassInit()
{
	// Clear the logic data
	m_CurrentLevel = 1;

	// If there is a field then clear it
	if( _pGameFieldFractions )
	{
		delete _pGameFieldFractions;
		_pGameFieldFractions = NULL;
	}

	_pGameFieldFractions = new GameFieldFractions();
	
	// We need to set the numerator block position before Init is called on the field
	if( m_pGameGUILayout )
	{
		Box2i selectionLabelBoundBox = m_pGameGUILayout->GetLabelBoundBox( GUILayoutInGame::L_SumLabel );

		// Set where the player's current selection will display
		_pGameFieldFractions->NumeratorBlockCenterPos.x = selectionLabelBoundBox.GetCenter().x;
		_pGameFieldFractions->NumeratorBlockCenterPos.y = selectionLabelBoundBox.Bottom() + 10 + (GameDefines::BLOCK_HEIGHT / 2);
	}

	// Initialize the field
	_pGameFieldFractions->m_FieldBox.Set( 389, 61, GameField::FIELD_WIDTH * 55, GameField::FIELD_HEIGHT * 55 );
	_pGameFieldFractions->Init();
	_pGameFieldFractions->SendInstruction( new PIISetPushSpeed( GameLogic::GetPushUpSpeed(1) ) );

	// Pre-cache the raise, right, and wrong sound
	//ResourceMgr::Get().GetAudioSample( RESID_SND_RAISE );
	ResourceMgr::Get().GetAudioSample( RESID_SND_RIGHTPROD );
	ResourceMgr::Get().GetAudioSample( RESID_SND_WRONGPROD );

	// Initialize the stat tracking
	_curSessionStats.Clear();
	_curSessionStats.m_Difficulty = GameMgr::Get().GetGameplayDiff();
	_curSessionStats.StoreStartTime();

	// Initialize the UI
	if( m_pGameGUILayout )
	{
		m_pGameGUILayout->SetScore( 0 );
		m_pGameGUILayout->SetLevel( m_CurrentLevel );
		m_pGameGUILayout->SetCount( 0 );

		if( m_TimeLimit > 0 )
			m_pGameGUILayout->SetTimePlayed( m_TimeLimit );
		else
			m_pGameGUILayout->SetTimePlayed( 0 );

		// Display the combo block label if this is Prime Time mode
		m_pGameGUILayout->InitFromGameType( m_GameType );

		//TODO localize
		m_pGameGUILayout->SetLabelText( GUILayoutInGame::L_SumLabel, L"Your Selection" );
	}

	SetDifficulty( GameMgr::Get().GetGameplayDiff() );

	_pGameFieldFractions->PushNewBlocks( GenerateBlocksToFieldWidth() );
	_pGameFieldFractions->DropNewBlocks( GenerateBlocksToFieldWidth() );
	_pGameFieldFractions->DropNewBlocks( GenerateBlocksToFieldWidth() );

	//TODO localize
	m_pGameGUILayout->SetLabelText( GUILayoutInGame::L_CurrentProdName, L"Find An Equivalent Of:" );

	_equivalentFraction = GetNextFraction();

	std::wstring sNextString = TCBase::EasyIToA( _equivalentFraction.Numerator );
	sNextString += L"/";
	sNextString += TCBase::EasyIToA( _equivalentFraction.Denominator );

	m_pGameGUILayout->SetLabelText( GUILayoutInGame::L_CurrentProd, sNextString.c_str() );

	return true;
}

/// Close the logic
void GameLogicFractions::Term()
{
	_curSessionStats.m_Duration = GameMgr::Get().GetElapsedTime();
}

/// Is the game over
bool GameLogicFractions::IsGameOver()
{
	if( !_pGameFieldFractions )
		return false;

	return _pGameFieldFractions->IsGameOver();
}

/// Generate a new block
GameFieldBlockFraction* GameLogicFractions::GenerateBlock( int32 maxBlockWidth )
{
	// Create the new block
	GameFieldBlockFraction* pNewBlock = new GameFieldBlockFraction( GetNextFractionBlockValue(maxBlockWidth) );

	ResourceID spriteResID = RESID_SPRITE_PRODUCT_BLOCK;
	if( pNewBlock->GetValue() > 99 )
		spriteResID = RESID_SPRITE_DOUBLE_PRODUCT_BLOCK;

	// Set the new block's sprite
	pNewBlock->sprite = ResourceMgr::Get().GetRefSprite( spriteResID );

	_fractionValueCounts[ pNewBlock->GetValue() ]++;

	// Return the new block
	return pNewBlock;
}

template<class T>
int32 PickRandom(T values)
{
	uint32 randIndex = (uint32)rand() % values.size();

	uint32 valueIndex = 0;
	for( typename T::iterator iterValue = values.begin(); iterValue != values.end(); ++iterValue )
	{
		if( valueIndex == randIndex )
			return *iterValue;

		++valueIndex;
	}

	return -1;
}

int32 GameLogicFractions::GetNextFractionBlockValue( int32 maxBlockWidth ) const
{
	// Get the fraction with the lowest options on the field
	int32 lowestChanceFractionIndex = 0;
	uint8 lowestChanceFractionCount = _solutionFractionPossibilities[0];
	for( uint32 curFractionIndex = 1; curFractionIndex < _pCurModeSettings->AvailableFractions.size(); ++curFractionIndex )
	{
		if( _solutionFractionPossibilities[curFractionIndex] < lowestChanceFractionCount )
		{
			lowestChanceFractionIndex = curFractionIndex;
			lowestChanceFractionCount = _solutionFractionPossibilities[curFractionIndex];
		}
	}

	const FractionModeSettings::FractionInfo& fraction = _pCurModeSettings->AvailableFractions[lowestChanceFractionIndex];
	
	uint8 lowestValueCount = 255;
	std::list<int32> lowestValues;

	for( std::set<int32>::const_iterator iterValue = fraction.ValuesUpToMax.begin(); iterValue != fraction.ValuesUpToMax.end(); ++iterValue )
	{
		if( maxBlockWidth < 2 && *iterValue > 99 )
			continue;

		// Scale the value count by the number of times it's need by the fractions
		uint8 numOfThisValueOnTheFieldScaled = _fractionValueCounts[*iterValue];
		numOfThisValueOnTheFieldScaled /= fraction.ValueScalars.find(*iterValue)->second;
		
		if( numOfThisValueOnTheFieldScaled < lowestValueCount )
		{
			lowestValues.clear();
			lowestValues.push_back( *iterValue );
			lowestValueCount = numOfThisValueOnTheFieldScaled;
		}
		else if( numOfThisValueOnTheFieldScaled == lowestValueCount )
			lowestValues.push_back( *iterValue );
	}

	if( lowestValues.size() == 0 )
		return PickRandom(fraction.ValuesUpToMax);

	// Pick one of the values at ranom
	return PickRandom(lowestValues);
}


/// Generate blocks up to a certain width
GameFieldBlockMultBase** GameLogicFractions::GenerateBlocksToFieldWidth()
{
	static GameFieldBlockMultBase* s_FieldBlocks[ GameFieldMultBase::FIELD_WIDTH + 1 ] = {0};

	CalculatePossibleSolutionCounts();

	// Create as many blocks as needed to fill a row
	int32 curWidth = 0;
	int32 curBlockIndex = 0;
	while( curWidth < GameFieldMultBase::FIELD_WIDTH )
	{
		// If it is time for a 0x block
		GameFieldBlockFraction* pNewBlock = GenerateBlock( GameFieldMultBase::FIELD_WIDTH - curWidth );

		// Set the position
		pNewBlock->colIndex = curWidth;

		// Store the new block
		s_FieldBlocks[curBlockIndex++] = pNewBlock;

		// Add in the new block's width to the current width
		curWidth += pNewBlock->size.x;
	}

	// Clear the rest of the game field block pointers in the static arrays for safety
	do
	{
		s_FieldBlocks[ curBlockIndex ] = NULL;
	}while( ++curBlockIndex < GameFieldMultBase::FIELD_WIDTH );

	CalculatePossibleSolutionCounts();

	// Return the array
	return s_FieldBlocks;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicFractions::~GameLogicFractions  Public
///
/// The destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLogicFractions::~GameLogicFractions()
{
	// If we have field then free it
	if( _pGameFieldFractions )
	{
		delete _pGameFieldFractions;
		_pGameFieldFractions = NULL;
	}
}

void PopulateFractionModes()
{
	int32 minDiffOffset = (int32)GameDefines::GPDL_Pre3;

	for( int32 diffValue = (int32)GameDefines::GPDL_Pre3; diffValue < (int32)GameDefines::GPDL_5; ++diffValue )
	{
		std::list<Fraction> fractions;
		fractions.push_back( Fraction(1,3) );

		int32 maxValue = 9;
		if( diffValue < (int32)GameDefines::GPDL_3 )
		{
			fractions.push_back( Fraction(1,2) );
		}
		else
		{
			fractions.push_back( Fraction(2,4) );
		
			if( diffValue == (int32)GameDefines::GPDL_3 )
				maxValue = 13;
			else if( diffValue == (int32)GameDefines::GPDL_4 )
			{
				maxValue = 16;
				fractions.push_back( Fraction(1,4) );
			}
		}

		g_FractionModeSettings[diffValue - minDiffOffset] = FractionModeSettings(1, maxValue, fractions );
	}

	for( int32 diffValue = (int32)GameDefines::GPDL_5; diffValue < (int32)GameDefines::GPDL_8; ++diffValue )
	{
		std::list<Fraction> fractions;
		fractions.push_back( Fraction(1,5) );
		fractions.push_back( Fraction(1,6) );
		fractions.push_back( Fraction(2,6) );
		fractions.push_back( Fraction(3,6) );
		fractions.push_back( Fraction(4,6) );
		fractions.push_back( Fraction(2,8) );
		fractions.push_back( Fraction(6,9) );

		int32 maxValue = 30;

		if( diffValue >= (int32)GameDefines::GPDL_6 )
		{
			maxValue = 36;
			fractions.push_back( Fraction(6,8) );
		}
		
		// Not an else if because we want to add on to the previous if statement
		if( diffValue == (int32)GameDefines::GPDL_7 )
		{
			maxValue = 41;
			fractions.push_back( Fraction(5,6) );
		}

		g_FractionModeSettings[diffValue - minDiffOffset] = FractionModeSettings(2, maxValue, fractions );
	}

	for( int32 diffValue = (int32)GameDefines::GPDL_8; diffValue <= (int32)GameDefines::GPDL_Secret2; ++diffValue )
	{
		std::list<Fraction> fractions;
		fractions.push_back( Fraction(1,8) );
		fractions.push_back( Fraction(3,8) );
		fractions.push_back( Fraction(5,8) );
		fractions.push_back( Fraction(7,8) );
		fractions.push_back( Fraction(1,7) );
		fractions.push_back( Fraction(2,7) );
		fractions.push_back( Fraction(3,7) );
		fractions.push_back( Fraction(4,7) );
		fractions.push_back( Fraction(6,7) );
		fractions.push_back( Fraction(1,9) );
		fractions.push_back( Fraction(2,9) );
		fractions.push_back( Fraction(4,9) );
		fractions.push_back( Fraction(5,9) );
		
		

		int32 maxValue = 54;

		if( diffValue >= (int32)GameDefines::GPDL_Post8 )
		{
			maxValue = 61;
			fractions.push_back( Fraction(5,7) );
		}

		if( diffValue >= (int32)GameDefines::GPDL_Secret1 )
		{
			maxValue = 110;
			fractions.push_back( Fraction(7,9) );
		}

		if( diffValue >= (int32)GameDefines::GPDL_Secret2 )
		{
			maxValue = 150;
			fractions.push_back( Fraction(8,9) );
		}

		g_FractionModeSettings[diffValue - minDiffOffset] = FractionModeSettings(3, maxValue, fractions );
	}	
}