//=================================================================================================
/*!
	\file FSM.h
	Base Library
	Finite State Machine Header
	\author Taylor Clark
	\date September 2 2006

	This header contains the declarations for the finite state machine class.
*/
//=================================================================================================

#pragma once
#ifndef __FSM_h
#define __FSM_h

#include "Types.h"
#include <map>

class StateMachine;


//-------------------------------------------------------------------------------------------------
/// 
///	\class FSMState
///	\brief Encapsulates a finite state machine state.
///
//-------------------------------------------------------------------------------------------------
class FSMState
{
protected:
	friend class StateMachine;

	/// The state machine to which this state belongs
	StateMachine* m_pParentFSM;

public:

	/// The default constructor
	FSMState() : m_pParentFSM( 0 )
	{}

	/// The default destructor
	virtual ~FSMState() {}

	/// Update the state
	virtual void Update( float frameTime ) = 0;

	// Disable the unused formal parameter warning
#pragma warning( push )
#pragma warning( disable:4100 )

	/// A message handler for switching to this state
	virtual void OnEnterState( FSMState* pPrevState ){}

	/// A message handler for switching to another state
	virtual void OnLeaveState( FSMState* pNewState ){}
#pragma warning( pop )

	/// Get the state ID
	virtual int32 GetStateID() const = 0;
};


//-------------------------------------------------------------------------------------------------
/// 
///	\class FSM
///	\brief Encapsulates working with a finite state machine.
///
///	The StateMachine class defines an object representing a finite state machine and its
///	functionality.
///
//-------------------------------------------------------------------------------------------------
class StateMachine
{
private:
	/// The map of states
	typedef std::map< int32, FSMState* > StateMap;
	StateMap m_StateMap;

	/// The current state
	FSMState* m_pCurState;

	/// The destination stateID
	int32 m_DestStateID;

public:

	/// The default constructor
	StateMachine() : m_pCurState( 0 ),
					m_DestStateID( INVALID_STATE_ID )
	{}

	/// The default destructor
	~StateMachine(){ Clear(); }

	/// Update the current state
	void Update( float32 frameTime );

	/// Add a state to the state machine and take care of freeing the memory
	void AddManagedState( FSMState* pNewState );

	/// Switch to another state
	void SetActiveState( int32 newStateID );

	/// Clear all data in the state machine
	void Clear();

	/// Get the active state ID
	int32 GetActiveStateID() const
	{
		if( m_pCurState )
			return m_pCurState->GetStateID();
		return INVALID_STATE_ID;
	}

	/// Get a state
	FSMState* GetState( int32 stateID );

	/// The invalid state ID constant
	static const int32 INVALID_STATE_ID = -1;
};

#endif // __FSM_h