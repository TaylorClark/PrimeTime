/*=================================================================================================

	\file FSM.cpp
	Base Library
	Finite State Machine Source
	\author Taylor Clark
	\Date September 2, 2006

	This source file contains the implementation of the state machine class.

=================================================================================================*/

#include "../FSM.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  StateMachine::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the state machine.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void StateMachine::Update( float frameTime )
{
	// If the state needs to be changed
	if( m_DestStateID != INVALID_STATE_ID )
	{
		// Get a pointer to the destination state
		FSMState* pDestState = 0;
		StateMap::iterator iterDestState = m_StateMap.find( m_DestStateID );
		if( iterDestState != m_StateMap.end() )
			pDestState = iterDestState->second;

		// If a valid pointer was retrieved
		if( pDestState && pDestState != m_pCurState )
		{
			// If there is a current state then call the leave function
			if( m_pCurState )
				m_pCurState->OnLeaveState( pDestState );

			// Set the new current state
			FSMState* pOldState = m_pCurState;
			m_pCurState = pDestState;
			m_pCurState->OnEnterState( pOldState );
		}

		// Clear the need for a state change
		m_DestStateID = INVALID_STATE_ID;
	}

	// If there is a current state then update it
	if( m_pCurState )
		m_pCurState->Update( frameTime );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  StateMachine::AddManagedState  Public
///
///	\param pNewState The new state to add, if it's state ID overlaps another then the old state
///						is removed.
///
///	Add a state to the state machine and take care of freeing the memory.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void StateMachine::AddManagedState( FSMState* pNewState )
{
	// Don't add any NULL states
	if( !pNewState )
		return;

	// Find if the ID is already in use
	StateMap::iterator iterState = m_StateMap.find( pNewState->GetStateID() );
	if( iterState != m_StateMap.end() )
	{
		// Free the old state
		delete iterState->second;
		m_StateMap.erase( iterState );
	}

	// Add the new item
	m_StateMap[ pNewState->GetStateID() ] = pNewState;
	pNewState->m_pParentFSM = this;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  StateMachine::SetActiveState  Public
///
///	\param newStateID The state ID of the state to enter
///
///	Change to another state.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void StateMachine::SetActiveState( int32 newStateID )
{
	// Since this function can be called from a state's update function, the state change needs
	// to be delayed
	m_DestStateID = newStateID;

	// Test if the ID is known
	StateMap::iterator iterDestState = m_StateMap.find( m_DestStateID );

	// If the ID is unknown then do nothing
	if( iterDestState == m_StateMap.end() )
	{
		m_DestStateID = INVALID_STATE_ID;
		return;
	}

	// If the state is already active then do nothing
	if( m_pCurState && m_pCurState->GetStateID() == m_DestStateID )
	{
		m_DestStateID = INVALID_STATE_ID;
		return;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  StateMachine::Clear  Public
///
///	Clear all data in the state machine.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void StateMachine::Clear()
{
	// Close the current state
	if( m_pCurState )
	{
		m_pCurState->OnLeaveState( 0 );
		m_pCurState = 0;
	}

	// Free the state data
	for( StateMap::iterator iterState = m_StateMap.begin(); iterState != m_StateMap.end(); ++iterState )
		delete iterState->second;
	m_StateMap.clear();

	// Free the class data
	m_DestStateID = INVALID_STATE_ID;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  StateMachine::GetState  Public
///
///	\param stateID The state ID of the state to retrieve
///
///	Get a state.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
FSMState* StateMachine::GetState( int32 stateID )
{
	// Test if the ID is known
	StateMap::iterator iterState = m_StateMap.find( stateID );

	// If the ID is unknown then do nothing
	if( iterState == m_StateMap.end() )
		return 0;

	// Return the state
	return iterState->second;
}