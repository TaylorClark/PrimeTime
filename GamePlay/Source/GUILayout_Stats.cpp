//=================================================================================================
/*!
	\file GUILayout_Stats.cpp
	Game Play Library
	Game GUI Layouts Source
	\author Taylor Clark
	\date July 10, 2006

	This source file contains the implementation for the game GUI layout classes.
*/
//=================================================================================================

#include "../GUILayout_Game.h"
#include "GUI/GUICtrlLabel.h"
#include "../GameMgr.h"
#include "GUI/GUICtrlButton.h"
#include "Base/MsgLogger.h"
#include "../PlayerProfile.h"
#include "../ProfileSubsetMultiplication.h"
#include "../ProfileSubsetAddition.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Base/NumFuncs.h"

extern void LayoutCB_BackToTitle( GUICtrlButton* );

const int32 STAT_NAME_DIVIDER_X = 745;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutViewStats::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.  It goes through
///	the controls in the layout and finds the desired controls by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutViewStats::HookupCtrls()
{
	GameGUILayout::HookupCtrls();

	// Link up the play button
	GUIControl* pPlayCtrl = GetCtrlByName( CTRLNAME_BackButton );
	if( pPlayCtrl )
	{
		GUICtrlButton* pTypeList = (GUICtrlButton*)pPlayCtrl;
		pTypeList->SetClickCallback( LayoutCB_BackToTitle );
	}

	// Align the statistic name labels
	for( uint32 labelIndex = 0; labelIndex < NUM_STAT_LABELS; ++labelIndex )
	{
		if( !m_pStatValueLabels[ labelIndex ] )
			continue;

		Point2i newPos = m_pStatValueLabels[ labelIndex ]->GetPos();
		newPos.x = STAT_NAME_DIVIDER_X + 5;
		m_pStatValueLabels[ labelIndex ]->SetPos( newPos );
	}

	// Hook up the category buttons
	pPlayCtrl = GetCtrlByName( CTRLNAME_StatBtn_Main );
	if( pPlayCtrl )
	{
		GUICtrlButton* pButton = (GUICtrlButton*)pPlayCtrl;
		pButton->SetClickCallback( StatCatBtnCallback );
	}

	pPlayCtrl = GetCtrlByName( CTRLNAME_StatBtn_Game );
	if( pPlayCtrl )
	{
		GUICtrlButton* pButton = (GUICtrlButton*)pPlayCtrl;
		pButton->SetClickCallback( StatCatBtnCallback );
	}

	pPlayCtrl = GetCtrlByName( CTRLNAME_StatBtn_Mult );
	if( pPlayCtrl )
	{
		GUICtrlButton* pButton = (GUICtrlButton*)pPlayCtrl;
		pButton->SetClickCallback( StatCatBtnCallback );
	}

	pPlayCtrl = GetCtrlByName( CTRLNAME_StatBtn_Add );
	if( pPlayCtrl )
	{
		GUICtrlButton* pButton = (GUICtrlButton*)pPlayCtrl;
		pButton->SetClickCallback( StatCatBtnCallback );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutViewStats::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutViewStats::OnActivate()
{
	GameGUILayout::OnActivate();

	UpdateStats( SB_Main );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutViewStats::GetCatVal  Private
///
///	\param statType The stats to display
///
///	Get the category for a button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUILayoutViewStats::EStatBtn GUILayoutViewStats::GetCatVal( GUICtrlButton* pBtn )
{
	if( !pBtn )
		return SB_COUNT;

	if( pBtn->GetName() == CTRLNAME_StatBtn_Main )
		return SB_Main;
	else if( pBtn->GetName() == CTRLNAME_StatBtn_Game )
		return SB_Game;
	else if( pBtn->GetName() == CTRLNAME_StatBtn_Mult )
		return SB_Mult;
	else if( pBtn->GetName() == CTRLNAME_StatBtn_Add )
		return SB_Add;

	return SB_COUNT;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutViewStats::GetCatButton  Private
///
///	Get the button based on the selected type.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlButton* GUILayoutViewStats::GetCatButton( GUILayoutViewStats::EStatBtn stat )
{
	switch( stat )
	{
	case SB_Main:
		return (GUICtrlButton*)GetCtrlByName( CTRLNAME_StatBtn_Main );

	case SB_Mult:
		return (GUICtrlButton*)GetCtrlByName( CTRLNAME_StatBtn_Mult );

    case SB_Add:
		return (GUICtrlButton*)GetCtrlByName( CTRLNAME_StatBtn_Add );

	case SB_Game:
		return (GUICtrlButton*)GetCtrlByName( CTRLNAME_StatBtn_Game );
            
    default:
        break;
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutViewStats::StatCatBtnCallback  Static Private
///
///	A callback function for when the user presses a stat category button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutViewStats::StatCatBtnCallback( GUICtrlButton* pBtn )
{
	// Get the stats layout
	GUILayoutViewStats* pLayout = (GUILayoutViewStats*)pBtn->GetContainingLayout();
	if( !pLayout )
		return;

	GUICtrlButton* pOldSelButton = pLayout->GetCatButton( pLayout->m_CurCat );
	GUICtrlButton* pNewSelButton = pBtn;
	if( !pOldSelButton || !pNewSelButton )
		return;

	// Determine if the sprites need to be updated
	bool needsSpriteSwap = false;
	if( pOldSelButton != pNewSelButton )
		needsSpriteSwap = true;

	// Update the stats based on the pressed button
	pLayout->UpdateStats( pLayout->GetCatVal(pNewSelButton) );

	// Make the selected category button look selected if needed
	if( needsSpriteSwap )
	{
		RefSpriteHndl normSprite = pNewSelButton->GetIdleSprite();
		RefSpriteHndl overSprite = pNewSelButton->GetOverSprite();

		pOldSelButton->SetIdleSprite( normSprite );
		pNewSelButton->SetIdleSprite( overSprite );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutViewStats::UpdateStats  Public
///
///	\param statType The stats to display
///
///	Update the displayed stats based on the arithmetic type.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutViewStats::UpdateStats( GUILayoutViewStats::EStatBtn statType )
{
	// Store the stat type
	m_CurCat = statType;

	// Clear the current data
	ClearLabels();

	// Update the title
	if( m_pTitleLabel )
	{
		//TODO localize
		if( m_CurCat == SB_Main )
			m_pTitleLabel->SetText( L"Main Statistics" );
		else if( m_CurCat == SB_Game )
			m_pTitleLabel->SetText( L"Gameplay Statistics" );
		else if( m_CurCat == SB_Mult )
			m_pTitleLabel->SetText( L"Multiplication Statistics" );
		else if( m_CurCat == SB_Add )
			m_pTitleLabel->SetText( L"Addition Statistics" );

		// Center the text
		int32 newX = (g_pGraphicsMgr->GetDisplayDims().x - m_pTitleLabel->GetBoundBox().size.x) / 2;
		m_pTitleLabel->SetPos( Point2i( newX, m_pTitleLabel->GetPos().y ) );
	}

	// Get the active player profile
	PlayerProfile* pPlayerProfile = GameMgr::Get().GetActiveProfile();

	// If there is no player logged in then clear the data
	if( !pPlayerProfile )
	{
		//TODO localize
		// Clear the name
		if( m_pStatNameLabels[0] )
			m_pStatNameLabels[0]->SetText( L"You must log-in before you can view stats." );

		// Bail out
		return;
	}

	// Set up the layout based on the type of stats to display
	switch( statType )
	{
	case SB_Main:
		UpdateStats_Main();
		break;

	case SB_Game:
		UpdateStats_Game();
		break;

	case SB_Mult:
		UpdateStats_Mult();
		break;

	case SB_Add:
		UpdateStats_Add();
		break;
            
    default:
        // Stop warnings on Mac
        break;
	}

	// Right align the name labels
	RightAlignNames();
}

void GUILayoutViewStats::UpdateStats_Main()
{
	// Get the active player profile
	PlayerProfile* pPlayerProfile = GameMgr::Get().GetActiveProfile();
	if( !pPlayerProfile )
		return;

	// First go through the multiplication subset
	ProfileSubsetMultiplication* pMultSub = reinterpret_cast<ProfileSubsetMultiplication*>( pPlayerProfile->GetSubset( ProfileSubsetMultiplication::SUBSET_ID ) );
	
	// First go through the addition subset
	ProfileSubsetAddition* pAddSub = reinterpret_cast<ProfileSubsetAddition*>( pPlayerProfile->GetSubset( ProfileSubsetAddition::SUBSET_ID ) );

	// Display the number of games played
	if( m_pStatNameLabels[0] && m_pStatValueLabels[0] )
	{
		// Get the number of games played
		uint32 numGamesPlayed = 0;
		if( pMultSub )
			numGamesPlayed += pMultSub->GetNumSessions();
		if( pAddSub )
			numGamesPlayed += pAddSub->GetNumSessions();

		//TODO localize
		m_pStatNameLabels[0]->SetText( L"Number of games played:" );
		m_pStatValueLabels[0]->SetText( TCBase::EasyUIToA(numGamesPlayed) );
	}

	// Display the total time played
	if( m_pStatNameLabels[1] && m_pStatValueLabels[1] )
	{
		//TODO localize
		m_pStatNameLabels[1]->SetText( L"Total time played:" );

		// Get the total time played
		float32 fTotalTimePlayedSecs = 0;
		if( pMultSub )
		{
			const GameSessionStatsMult* pCurMultSession = pMultSub->GetFirstSession();
			while( pCurMultSession )
			{
				fTotalTimePlayedSecs += pCurMultSession->m_Duration;

				// Get the next session
				pCurMultSession = pMultSub->GetNextSession();
			}
		}
		if( pAddSub )
		{
			const GameSessionStatsAdd* pCurAddSession = pAddSub->GetFirstSession();
			while( pCurAddSession )
			{
				fTotalTimePlayedSecs += pCurAddSession->m_Duration;

				// Get the next session
				pCurAddSession = pAddSub->GetNextSession();
			}
		}

		uint32 totalTimePlayedSecs = (uint32)fTotalTimePlayedSecs;
		const uint32 BUF_SIZE = 32;
		wchar_t szBuffer[BUF_SIZE] = {0};

		const uint32 NUM_SECS_IN_DAY = 60 * 60 * 24;
		uint32 numDays = totalTimePlayedSecs / NUM_SECS_IN_DAY;
		totalTimePlayedSecs -= numDays * NUM_SECS_IN_DAY;
		
		uint32 numHours = totalTimePlayedSecs / (60 * 60);
		totalTimePlayedSecs -= numHours * (60 * 60);
		
		uint32 numMins = totalTimePlayedSecs / 60;

		//TODO localize
		// If there are days
		if( numDays > 0 )
			swprintf( szBuffer, BUF_SIZE, L"%u Days %u:%02u", numDays, numHours, numMins );
		else
			swprintf( szBuffer, BUF_SIZE, L"%u:%02u", numHours, numMins );

		// Set the text
		m_pStatValueLabels[1]->SetText( szBuffer );
	}
}

void GUILayoutViewStats::UpdateStats_Game()
{
}

void GUILayoutViewStats::UpdateStats_Mult()
{
	// Get the active player profile
	PlayerProfile* pPlayerProfile = GameMgr::Get().GetActiveProfile();
	if( !pPlayerProfile )
		return;

	// First go through the multiplication subset
	ProfileSubsetMultiplication* pMultSub = reinterpret_cast<ProfileSubsetMultiplication*>( pPlayerProfile->GetSubset( ProfileSubsetMultiplication::SUBSET_ID ) );
	if( !pMultSub )
		return;
	uint32 labelIndex = 0;

	// Get the average equation size
	if( m_pStatNameLabels[labelIndex] && m_pStatValueLabels[labelIndex] )
	{
		//TODO localize
		m_pStatNameLabels[labelIndex]->SetText( L"Average number of factors per equation:" );

		// Go through the sessions
		uint32 blockCount = 0;
		uint32 numClearedEqs = 0;
		const GameSessionStatsMult* pCurMultSession = pMultSub->GetFirstSession();
		while( pCurMultSession )
		{
			// Add up the blocks used this session and the number of equations cleared
			for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
				blockCount += pCurMultSession->m_ValidCounts[ primeIndex ];
			numClearedEqs += pCurMultSession->m_NumEqsCleared;

			// Get the next session
			pCurMultSession = pMultSub->GetNextSession();
		}

		// Get the average equation size
		uint32 avgEqSize = 0;
		if( numClearedEqs > 0 )
			avgEqSize = (uint32)( (float32)blockCount / (float32)numClearedEqs );

		// Set the text
		m_pStatValueLabels[labelIndex++]->SetText( TCBase::EasyUIToA(avgEqSize) );
	}

	// Display the average equations per minute
	if( m_pStatNameLabels[labelIndex] && m_pStatValueLabels[labelIndex] )
	{
		//TODO localize
		m_pStatNameLabels[labelIndex]->SetText( L"Average equations per minute:" );

		uint32 numClearedEqs = 0;
		float32 duration = 0;
		const GameSessionStatsMult* pCurMultSession = pMultSub->GetFirstSession();
		while( pCurMultSession )
		{
			numClearedEqs += pCurMultSession->m_NumEqsCleared;
			duration += pCurMultSession->m_Duration;

			// Get the next session
			pCurMultSession = pMultSub->GetNextSession();
		}

		// Calculate the value
		uint32 avgEqsPerMin = 0;
		if( duration > 0 )
			avgEqsPerMin = (uint32)( (float32)numClearedEqs / (duration / 60.0f) );

		// Set the text
		m_pStatValueLabels[labelIndex++]->SetText( TCBase::EasyUIToA(avgEqsPerMin) );
	}

	// Display the average mistakes per minute
	if( m_pStatNameLabels[labelIndex] && m_pStatValueLabels[labelIndex] )
	{
		//TODO localize
		m_pStatNameLabels[labelIndex]->SetText( L"Number of mistakes made:" );

		uint32 numFailedEqs = 0;
		const GameSessionStatsMult* pCurMultSession = pMultSub->GetFirstSession();
		while( pCurMultSession )
		{
			numFailedEqs += pCurMultSession->m_NumEqsFailed;
			
			// Get the next session
			pCurMultSession = pMultSub->GetNextSession();
		}

		// Set the text
		m_pStatValueLabels[labelIndex++]->SetText( TCBase::EasyUIToA(numFailedEqs) );
	}
}

void GUILayoutViewStats::UpdateStats_Add()
{
	// Get the active player profile
	PlayerProfile* pPlayerProfile = GameMgr::Get().GetActiveProfile();
	if( !pPlayerProfile )
		return;

	// First go through the multiplication subset
	ProfileSubsetAddition* pAddSub = reinterpret_cast<ProfileSubsetAddition*>( pPlayerProfile->GetSubset( ProfileSubsetAddition::SUBSET_ID ) );
	if( !pAddSub )
		return;
	uint32 labelIndex = 0;
	// Get the average equation size
	if( m_pStatNameLabels[labelIndex] && m_pStatValueLabels[labelIndex] )
	{
		//TODO localize
		m_pStatNameLabels[labelIndex]->SetText( L"Average sum size:" );

		// Go through the sessions
		uint32 numClearedEqs = 0;
		uint32 numSummandsEqs = 0;
		const GameSessionStatsAdd* pCurAddSession = pAddSub->GetFirstSession();
		while( pCurAddSession )
		{
			numClearedEqs += pCurAddSession->m_NumEqsCleared;
			numSummandsEqs += pCurAddSession->m_SummandsCleared;

			// Get the next session
			pCurAddSession = pAddSub->GetNextSession();
		}

		// Get the average equation size
		uint32 avgEqSize = 0;
		if( numClearedEqs > 0 )
			avgEqSize = (uint32)( (float32)numSummandsEqs / (float32)numClearedEqs );

		// Set the text
		m_pStatValueLabels[labelIndex++]->SetText( TCBase::EasyUIToA(avgEqSize) );
	}

	// Display the mistakes
	if( m_pStatNameLabels[labelIndex] && m_pStatValueLabels[labelIndex] )
	{
		//TODO localize
		m_pStatNameLabels[labelIndex]->SetText( L"Mistakes made:" );

		uint32 numInvalidEqs = 0;
		const GameSessionStatsAdd* pCurAddSession = pAddSub->GetFirstSession();
		while( pCurAddSession )
		{
			numInvalidEqs += pCurAddSession->m_NumEqsFailed;

			// Get the next session
			pCurAddSession = pAddSub->GetNextSession();
		}

		// Set the text
		m_pStatValueLabels[labelIndex++]->SetText( TCBase::EasyUIToA(numInvalidEqs) );
	}

	// Display the most number of sums cleared in 5 minutes
	if( m_pStatNameLabels[labelIndex] && m_pStatValueLabels[labelIndex] )
	{
		//TODO localize
		//TODO 5 minutes? Should be 3, I guess...
		m_pStatNameLabels[labelIndex]->SetText( L"Most sums cleared in 5 minutes:" );

		uint32 maxCleared = 0;
		const GameSessionStatsAdd* pCurAddSession = pAddSub->GetFirstSession();
		while( pCurAddSession )
		{
			if( pCurAddSession->m_TimeLimit == 5 * 60 )
			{
				if( pCurAddSession->m_NumEqsCleared > maxCleared )
					maxCleared = pCurAddSession->m_NumEqsCleared;
			}

			// Get the next session
			pCurAddSession = pAddSub->GetNextSession();
		}

		// Set the text
		m_pStatValueLabels[labelIndex++]->SetText( TCBase::EasyUIToA(maxCleared) );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutViewStats::RightAlignNames  Private
///
///	Right align the non-empty stat name labels.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutViewStats::RightAlignNames()
{
	// The right align position
	const int32 RIGHT_ALIGN_X = STAT_NAME_DIVIDER_X - 5;

	// Go through the name labels
	for( uint32 curLabelIndex = 0; curLabelIndex < NUM_STAT_LABELS; ++curLabelIndex )
	{
		// If the control is invalid or empty then skip it
		if( !m_pStatNameLabels[curLabelIndex] || m_pStatNameLabels[curLabelIndex]->GetTextStr().size() == 0 )
			continue;

		// Get the text width
		int32 strWidth = m_pStatNameLabels[curLabelIndex]->GetBoundBox().size.x;

		// Set the new position
		Point2i labelPos = m_pStatNameLabels[curLabelIndex]->GetPos();
		labelPos.x = RIGHT_ALIGN_X - strWidth;
		m_pStatNameLabels[curLabelIndex]->SetPos( labelPos );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutViewStats::ClearLabels  Private
///
///	Clear the name and value labels.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutViewStats::ClearLabels()
{
	//  Go through the labels
	for( uint32 labelIndex = 0; labelIndex < NUM_STAT_LABELS; ++labelIndex )
	{
		// Clear the value
		if( m_pStatNameLabels[labelIndex] )
			m_pStatNameLabels[labelIndex]->SetText( L"" );

		if( m_pStatValueLabels[labelIndex] )
			m_pStatValueLabels[labelIndex]->SetText( L"" );
	}
}