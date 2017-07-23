//=================================================================================================
/*!
	\file PIISetPushSpeed.h
	Game Play Library
	Player Input Instruction Set Block Push Speed Header
	\author Taylor Clark
	\date September 30, 2007

	This header contains the definition for the player input instruction to adjust the push speed.
*/
//=================================================================================================

#pragma once
#ifndef __PIISetPushSpeed_h
#define __PIISetPushSpeed_h

#include "GameFieldInstruction.h"

class GameFieldBlock;


//-------------------------------------------------------------------------------------------------
/*!
	\class PIISetPushSpeed
	\brief The player input instruction that sets the block push speed
*/
//-------------------------------------------------------------------------------------------------
class PIISetPushSpeed : public GameFieldInstruction
{
private:

	/// The new block push speed
	float32 m_NewPushSpeed;

	/// Serialize the instruction to a memory stream
	virtual void SubclassSerialize( Serializer& serializer )
	{
		serializer.AddData( OldPushSpeed );
		serializer.AddData( m_NewPushSpeed );
	}

public:

	/// The push speed before the change
	float32 OldPushSpeed;

	/// The default constructor
	PIISetPushSpeed(){}

	/// The constructor to initialize the instruction to toggle the selection of a block
	PIISetPushSpeed( float32 pushSpeed ) : m_NewPushSpeed( pushSpeed ),
											OldPushSpeed( pushSpeed )
	{
	}

	/// Get the type of instruction this object represents
	virtual EGameFieldInstruction GetType() const
	{
		return GFI_SetPushSpeed;
	}

	/// Get the new push speed
	float32 GetNewPushSpeed() const { return m_NewPushSpeed; }
};

#endif // __PIISetPushSpeed_h