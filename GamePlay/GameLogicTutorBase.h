//=================================================================================================
/*!
	\file GameLogicTutorBase.h
	Game Play Library
	Tutorial Base Game Logic Header
	\author Taylor Clark
	\date October 24, 2006

	This header contains the definition for the base tutorial logic.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicTutorBase_h
#define __GameLogicTutorBase_h

#include "GameLogicProduct.h"
#include "GameLogicFractions.h"
#include "GameDefines.h"

#include "GUI/MsgBox.h"
#include "GameMgr.h"
#include "GUI/GUIMgr.h"
#include "GUILayout_Game.h"
#include "GameFieldMultBase.h"
#include "GameFieldBlockProduct.h"
#include "Base/NumFuncs.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicMgrProduct
	\brief Represents the game manager

	This class defines the game manager class that is used to handle gameplay update.
*/
//-------------------------------------------------------------------------------------------------
template< class T >
class GameLogicTutorBase : public T
{
protected:

	enum EGoalType
	{
		GT_SelectBlock,
		GT_GenerateRows,
		GT_CloseMsgBox,
		GT_ClearProduct,
		GT_COUNT
	};

	struct Goal
	{
		/// The default constructor
		Goal(){}

		/// Initialize a goal
		Goal( EGoalType goalType, int32 val = 0 ) : goalType( goalType ),
												isCompleted( false )
		{
			data.selValue = val;
		}

		EGoalType goalType;

		union Data
		{
			int32 selValue;
			int32 clearProduct;
			uint32 rowsGenerated;
		} data;

		bool isCompleted;
	};

	struct Step
	{
		/// The default constructor
		Step(){}

		/// The constructor to initialize data
		Step( const wchar_t* szMsg, const wchar_t* szNote, Goal inGoal ) : goal( inGoal )
		{
			if( szMsg )
				sMsg = szMsg;
			if( szNote )
				sNoteText = szNote;
		}

		std::wstring sMsg;

		std::wstring sNoteText;

		Goal goal;
	};

	/// The message box to display instructions
	MsgBox* m_pMsgBox;

	/// The current step in the tutorial
	uint32 m_CurStepIndex;

	/// The number of rows generated
	uint32 m_RowsGenerated;

	/// The steps of the tutorial
	std::vector< Step > m_Steps;


	/// Update the GUI from the current step
	void OnNewStep()
	{
		if( m_CurStepIndex >= m_Steps.size() )
			return;

		// If there are more steps then display the next message box
		if( m_Steps[m_CurStepIndex].sMsg.length() > 0 )
		{
			// If there is a message box up then close it
			if( m_pMsgBox )
				CloseMsgBox();

			// Create a new message box
			m_pMsgBox = MsgBox::CreateMsgBox( m_Steps[ m_CurStepIndex ].sMsg.c_str() );
			GUIMgr::Get().PopupMessageBox( m_pMsgBox );
			GameMgr::Get().SetPauseState( true );
		}

		// Set the tutorial note
		if( T::m_pGameGUILayout )
			T::m_pGameGUILayout->SetNoteText( m_Steps[m_CurStepIndex].sNoteText.c_str() );

		// If there is no message box displayed then flash the note text
		if( !m_pMsgBox )
			T::m_pGameGUILayout->FlashTutorialNote();
	}

	/// Close the active message box
	void CloseMsgBox()
	{
		GameMgr::Get().SetPauseState( false );

		GUIMgr::Get().CloseMessageBox( m_pMsgBox );
		delete m_pMsgBox;
		m_pMsgBox = NULL;
	}

public:

	/// The constructor
	GameLogicTutorBase() : m_pMsgBox( 0 ),
						m_CurStepIndex( 0 ),
						m_RowsGenerated( 0 )
	{}

	/// The destructor
	virtual ~GameLogicTutorBase() {}

	/// A message handler for when a block is selected that contains the mode-specific logic
	virtual GameLogic::EBlockSelAction OnBlockSelect( GameFieldBlock* pBlock )
	{
		// If the current step is invalid or not a selection step then bail
		if( m_CurStepIndex >= m_Steps.size() )
			return T::OnBlockSelect( pBlock );
		
		// If the current step is not to select the block then bail
		EGoalType goalType = m_Steps[m_CurStepIndex].goal.goalType;
		if( goalType != GT_SelectBlock && goalType != GT_ClearProduct )
			return GameLogic::BSA_DeselectLast;

		// If the next step is to select a value
		if( goalType == GT_SelectBlock )
		{
			// If this block has the correct value then allow it to be selected
			if( pBlock->GetValue() == m_Steps[m_CurStepIndex].goal.data.selValue )
			{
				m_Steps[m_CurStepIndex].goal.isCompleted = true;
				return T::OnBlockSelect( pBlock );
			}

			// Don't allow the select since the block is not the right value
			return GameLogic::BSA_DeselectLast;
		}

		// Complete the step and allow the normal logic to handle the selection
		return T::OnBlockSelect( pBlock );
	}

	/// A message handler for when a block is deselected
	virtual GameLogic::EBlockSelAction OnBlockDeselect( GameFieldBlock* pBlock )
	{
		// Don't allow the user to deselect if the current step is to select
		if( m_CurStepIndex < m_Steps.size() && m_Steps[ m_CurStepIndex ].goal.goalType == GT_SelectBlock )
			return GameLogic::BSA_None;

		// Allow normal handling
		return T::OnBlockDeselect( pBlock );
	}	

	/// Allow a subclass to update data if needed
	virtual void SubclassUpdate( float32 frameTime )
	{
		// If the final step is reached then bail
		if( m_CurStepIndex >= m_Steps.size() )
		{
			GameMgr::Get().EndGame();
			return;
		}

		// If there is a message box open
		if( m_pMsgBox )
		{
			// If the box needs to be closed
			if( m_pMsgBox->GetRetVal() != MsgBox::MBR_Error )
			{
				CloseMsgBox();

				// If the current step is a message box close event
				if( m_Steps[m_CurStepIndex].goal.goalType == GT_CloseMsgBox )
					m_Steps[m_CurStepIndex].goal.isCompleted = true;
				else
					T::m_pGameGUILayout->FlashTutorialNote();
			}
			// Else do no further updating
			else
				return;
		}
		
		// If the current step has been satisfied
		if( m_CurStepIndex < m_Steps.size() && m_Steps[ m_CurStepIndex ].goal.isCompleted )
		{
			// Increment the step index
			m_CurStepIndex++;

			OnNewStep();

			// Bail out now so that we don't update the game in case the next step needs to display a
			// message box before moving on
			return;
		}

		// Update the game
		T::SubclassUpdate( frameTime );
	}
};

#endif // __GameLogicTutorBase_h