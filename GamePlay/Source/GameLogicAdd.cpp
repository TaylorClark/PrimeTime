/*=================================================================================================

	\file GameLogicAdd.cpp
	Game Play Library
	Addition Game Logic Source
	\author Taylor Clark
	\Date September 10, 2006

	This source file contains the implementation of the addition game logic manager class.

=================================================================================================*/

#include "../GameLogicAdd.h"
#include "../GameFieldAdd.h"
#include "../GameFieldBlockAdd.h"
#include "../GameMgr.h"
#include "../GUILayout_Game.h"
#include "../ProfileSubsetAddition.h"
#include "../GFISetSum.h"
#include "../GFIAddSummands.h"
#include "GUI/GUIMgr.h"

const uint32 NUM_SUMMANDS = 12;
const ResourceID RESID_SND_RIGHTPROD = 131;
const ResourceID RESID_SND_WRONGPROD = 132;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicAdd::~GameLogicAdd  Private
///
///	The destructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLogicAdd::~GameLogicAdd()
{
	// If there is a field then clear it
	if( m_pGameFieldAdd )
	{
		delete m_pGameFieldAdd;
		m_pGameFieldAdd = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicAdd::SubclassInit  Private
///
///	\returns Whether or not the logic was successfully initialized
///
///	Initialize the product game logic after initializing the base class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameLogicAdd::SubclassInit()
{
	// Clear data
	m_NumSumsCleared = 0;
	
	// If there is a field then clear it
	if( m_pGameFieldAdd )
	{
		delete m_pGameFieldAdd;
		m_pGameFieldAdd = 0;
	}

	// Create a new field
	m_pGameFieldAdd = new GameFieldAdd();
	if( !m_pGameFieldAdd )
		return false;

	// Initialize the field
	m_pGameFieldAdd->m_FieldBox.Set( 389, 61, GameField::FIELD_WIDTH * 55, GameField::FIELD_HEIGHT * 55 );
	m_pGameFieldAdd->Init();

	// Setup the numbers to generate
	InitNumbers();

	// Pre-cache the right and wrong sounds
	ResourceMgr::Get().GetAudioSample( RESID_SND_RIGHTPROD );
	ResourceMgr::Get().GetAudioSample( RESID_SND_WRONGPROD );

	// Initialize the stat tracking
	m_CurSessionStats.Clear();
	m_CurSessionStats.m_TimeLimit = GameMgr::Get().GetTimeLimit();
	m_CurSessionStats.m_Difficulty = GameMgr::Get().GetGameplayDiff();
	m_CurSessionStats.StoreStartTime();

	// Initialize the UI
	if( m_pGameGUILayout )
	{
		m_pGameGUILayout->InitFromGameType( m_GameType );

		m_pGameGUILayout->SetScore( 0 );
		m_pGameGUILayout->SetCount( 0 );

		if( m_TimeLimit > 0 )
			m_pGameGUILayout->SetTimePlayed( m_TimeLimit );
		else
			m_pGameGUILayout->SetTimePlayed( 0 );

		// Set the sum block position
		Point2i sumBlockPos = m_pGameGUILayout->GetSumBottomLeftPos();
		sumBlockPos.y += 5;
		m_pGameFieldAdd->SetSumBlockPos( sumBlockPos );
	}

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicAdd::Term  Public
///
/// Allow the logic to terminate gracefully
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicAdd::Term()
{
	m_CurSessionStats.m_Duration = GameMgr::Get().GetElapsedTime();

	// Get the profile
	PlayerProfile* pProfile = GameMgr::Get().GetActiveProfile();
	ProfileSubsetAddition* pAdditionProfile = 0;
	if( pProfile )
		pAdditionProfile = (ProfileSubsetAddition*)pProfile->GetSubset( ProfileSubsetAddition::SUBSET_ID );

	// End the session with the profile
	if( pAdditionProfile )
		pAdditionProfile->StoreSession( m_CurSessionStats );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicAdd::OnBlockSelect  Public
///
///	\param pBlock The block which was just selected
///	\returns The action to take to the selected blocks
///
///	A message handler for when a block is selected that contains the mode-specific logic
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLogic::EBlockSelAction GameLogicAdd::OnBlockSelect( GameFieldBlock* pBlock )
{
	// If the selected block is not the sum block then bail
	GameFieldBlockAdd* pSumBlock = m_pGameFieldAdd->GetSumBlock();
	if( pBlock != pSumBlock )
		return GameLogic::BSA_None;

	// Get the selected blocks
	const BlockList& selBlocks = m_pGameFieldAdd->GetSelectedBlocks();
	if( selBlocks.size() < 2 )
		return GameLogic::BSA_DeselectSelection;

	// Total up the blocks
	int32 sum = 0;
	for( BlockList::const_iterator iterBlock = selBlocks.begin(); iterBlock != selBlocks.end(); ++iterBlock )
	{
		GameFieldBlockAdd* pCurBlock = (GameFieldBlockAdd*)*iterBlock;
		if( pCurBlock == pSumBlock )
			break;

		sum += pCurBlock->GetValue();
	}
	
	// If the equation is invalid
	if( sum != pSumBlock->GetValue() )
	{
		m_CurSessionStats.OnErroneousEquation( selBlocks );

		// Play the failed sound
		AudioSampleHndl wrongSound = ResourceMgr::Get().GetAudioSample( RESID_SND_WRONGPROD );
		if( wrongSound.GetObj() )
			wrongSound.GetObj()->Play();

		return GameLogic::BSA_DeselectSelection;
	}

	// Update the field counts
	for( BlockList::const_iterator iterBlock = selBlocks.begin(); iterBlock != selBlocks.end(); ++iterBlock )
	{
		int32 summandIndex = SummandValueToIndex( (*iterBlock)->GetValue() );
		if( summandIndex == -1 )
			continue;

		m_Summands[summandIndex].fieldCount--;
	}

	// The equation is valid
	m_CurSessionStats.OnValidEquation( selBlocks );
	m_NumSumsCleared++;
	m_CurSessionStats.m_Score += sum;

	// Play the success sound
	AudioSampleHndl rightSound = ResourceMgr::Get().GetAudioSample( RESID_SND_RIGHTPROD );
	if( rightSound.GetObj() )
		rightSound.GetObj()->Play();

	// Update the UI
	if( m_pGameGUILayout )
	{
		m_pGameGUILayout->SetCount( m_NumSumsCleared );
		m_pGameGUILayout->SetScore( m_CurSessionStats.m_Score );
	}

	// Remove the blocks
	return GameLogic::BSA_RemoveEntireSelection;
}


int32 GameLogicAdd::SummandValueToIndex( int32 value )
{
	for( uint32 curSummandIndex = 0; curSummandIndex < m_Summands.size(); ++curSummandIndex )
	{
		if( m_Summands[curSummandIndex].value == value )
			return curSummandIndex;
	}

	return -1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicAdd::InitNumbers  Public
///
///	Initialize the summands to generate.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicAdd::InitNumbers()
{
	int32 gameplayDiffVal = (int32)GameMgr::Get().GetGameplayDiff() + 1;
	if( GameMgr::Get().GetGameplayDiff() > GameDefines::GPDL_6 )
		gameplayDiffVal = (int32)GameMgr::Get().GetGameplayDiff() * 3 + 1;

	_isInPractice10Mode = gameplayDiffVal == 1;
	if( _isInPractice10Mode )
	{
		m_MinSum = 10;
		m_MaxSum = 11;

		m_Summands.resize( 9 );
		m_Summands[0] = 1;
		m_Summands[1] = 2;
		m_Summands[2] = 3;
		m_Summands[3] = 4;
		m_Summands[4] = 5;
		m_Summands[5] = 6;
		m_Summands[6] = 7;
		m_Summands[7] = 8;
		m_Summands[8] = 9;

		return;
	}

	// Level 1 = 10 fixed
	// Level 2 = 10 to 20 (Pre-3rd)
	// Level 3 = 20 to 40 (3rd)
	// Level 4 = 30 to 60 (3rd)
	// Level 8 = 96 to 96*3.25 (Post-8th)
	// Break the min range into 12 pieces
	m_MinSum = (gameplayDiffVal - 1) * 10;
	if( GameMgr::Get().GetGameplayDiff() > GameDefines::GPDL_6 )
		m_MaxSum = (uint32)( (float32)m_MinSum * 3.25f );
	else
		m_MaxSum = (uint32)( (float32)m_MinSum * 2.0f );
	
	// Resize the summand array
	m_Summands.resize( NUM_SUMMANDS );
	m_Summands[0] = Summand( 1 );

	// If the minium sum is above 99 then we need to generate the summands skewed towards 99
	if( m_MinSum > 99 )
	{
		// Get the distribution midpoint
		int32 diffRange = (int32)GameDefines::GPDL_Post8 + 1 - (int32)GameDefines::GPDL_5;
		int32 ourVal = (int32)GameMgr::Get().GetGameplayDiff() - (int32)GameDefines::GPDL_5;
		float32 uniformDistMidPoint = (float32)ourVal / (float32)diffRange;
		uniformDistMidPoint *= 0.5f;
		uniformDistMidPoint += 0.5f;

		// Store the summands
		for( uint32 summandIndex = 1; summandIndex < m_Summands.size(); ++summandIndex )
		{
			float32 percent = (float32)summandIndex / (float32)m_Summands.size();
			
			if( percent < 0.5f )
				percent = (percent / 0.5f) * uniformDistMidPoint;
			else
				percent = ((percent - 0.5f) / 0.5f) * (1.0f - uniformDistMidPoint) + uniformDistMidPoint;
			
			// Store the value
			uint32 newVal = (uint32)(percent * 99.0f);
			m_Summands[summandIndex] = Summand( newVal );
		}
	}
	// Else compose a uniform distribution
	else
	{
		// Store the summands
		for( uint32 summandIndex = 1; summandIndex < m_Summands.size(); ++summandIndex )
		{
			// Calculate a percent of the minium sum
			float32 percent = (float32)summandIndex / (float32)m_Summands.size();
			percent += (float32)( (rand() % 13) - 6 ) / 100.0f;
			const float32 ONE_TWELTH = 0.083333f;
			const float32 ELEVEN_TWELTH = 0.9166667f;
			if( percent < ONE_TWELTH )
				percent = ONE_TWELTH;
			else if( percent > ELEVEN_TWELTH )
				percent = ELEVEN_TWELTH;

			// Store the summand value
			uint32 newVal = (uint32)(percent * (float32)m_MinSum);
			if( newVal < 1 )
				newVal = 1;
			m_Summands[summandIndex] = Summand( newVal );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicAdd::GenerateSum  Public
///
///	Generate a sum.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 GameLogicAdd::GenerateSum()
{
	if( _isInPractice10Mode )
		return 10;

	// Add summands until we get a sum within the range
	int32 sum = 0;
	while( sum < m_MinSum )
	{
		uint32 summandIndex = rand() % m_Summands.size();
		sum += m_Summands[summandIndex].value;
	}

	if( sum >= m_MaxSum )
		return m_MaxSum - 1;

	// 50% chance of adding more summands
	while( rand() % 2 )
	{
		// If the next summand will not put the sum greater than the max then add it
		uint32 summandIndex = rand() % m_Summands.size();
		if( sum + m_Summands[summandIndex].value < m_MaxSum )
			sum += m_Summands[summandIndex].value;
		else
			break;
	}

	// Return the sum
	return sum;
}


int32 GameLogicAdd::GetNextSummandIndex()
{
	// If we are in practice 10 mode then try to keep an even distribution of all summands
	if( _isInPractice10Mode )
	{
		uint32 minSummandIndex = 0;
		for( uint32 curSummandIndex = 1; curSummandIndex < m_Summands.size(); ++curSummandIndex )
		{
			if( m_Summands[curSummandIndex].fieldCount < m_Summands[minSummandIndex].fieldCount )
				minSummandIndex = curSummandIndex;
		}

		return minSummandIndex;
	}

	return rand() % m_Summands.size();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicAdd::SubclassUpdate  Public
///
///	\param frameTime The elapsed frame time
///
///	Generate the blocks to make up for any holes in the field
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicAdd::GenerateLackingBlocks()
{
	// Get the number of needed blocks
	uint32 neededBlocks = (uint32)(GameField::FIELD_HEIGHT * GameField::FIELD_WIDTH) - m_pGameFieldAdd->GetNumBlocks();

	// Generate the new blocks
	GFIAddSummands* pNewAddInst = new GFIAddSummands();
	for( uint32 blockIndex = 0; blockIndex < neededBlocks; ++blockIndex )
	{
		uint32 summandIndex = GetNextSummandIndex();

		m_Summands[summandIndex].fieldCount++;

		pNewAddInst->m_SummandValues.push_back( m_Summands[summandIndex].value );
	}

	// Add the blocks to the field
	m_pGameFieldAdd->SendInstruction( pNewAddInst );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicAdd::SubclassUpdate  Public
///
///	\param UNNAMED The elapsed frame time
///
///	Allow a subclass to update data if needed
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicAdd::SubclassUpdate( float32 frameTime )
{
	// If the game is paused then bail
	if( GameMgr::Get().IsPaused() )
		return;

	// If the field is lacking blocks
	if( m_pGameFieldAdd->GetNumBlocks() < (uint32)(GameField::FIELD_HEIGHT * GameField::FIELD_WIDTH) )
		GenerateLackingBlocks();

	// Update the field
	m_pGameFieldAdd->Update( frameTime );

	// If there is no sum
	if( !m_pGameFieldAdd->GetSumBlock() )
		m_pGameFieldAdd->SendInstruction( new GFISetSum( GenerateSum() ) );
}