//=================================================================================================
/*!
	\file GFISetSub.h
	Game Play Library
	Game Field Instruction Set Sub Header
	\author Taylor Clark
	\date January 11, 2008

	This header contains the definition for the set sub game field instruction.
*/
//=================================================================================================

#pragma once
#ifndef __GFISetSum_h
#define __GFISetSum_h

#include "GameFieldInstruction.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GFISetSum
	\brief The instruction that sets the sum in a game of Addition Attack
*/
//-------------------------------------------------------------------------------------------------
class GFISetSum : public GameFieldInstruction
{
private:

	/// The sum
	int32 m_SumValue;

	/// Serialize the instruction to a memory stream
	virtual void SubclassSerialize( Serializer& serializer )
	{
		serializer.AddData( m_SumValue );
	}

public:

	/// The default constructor
	GFISetSum(){}

	/// The constructor to initialize the instruction
	GFISetSum( int32 sumValue )
	{
		m_SumValue = sumValue;
	}

	/// Get the sum block value
	int32 GetSumValue() const{ return m_SumValue; }


	/// Get the type of instruction this object represents
	virtual EGameFieldInstruction GetType() const
	{
		return GFI_SetSum;
	}
};

#endif // __GFISetSum_h