//=================================================================================================
/*!
	\file GFIAddSummands.h
	Game Play Library
	Game Field Instruction Set Sub Header
	\author Taylor Clark
	\date January 11, 2008

	This header contains the definition for the set sub game field instruction.
*/
//=================================================================================================

#pragma once
#ifndef __GFIAddSummands_h
#define __GFIAddSummands_h

#include "GameFieldInstruction.h"
#include <list>


//-------------------------------------------------------------------------------------------------
/*!
	\class GFIAddSummands
	\brief The instruction to add summands to the Addition Attack field
*/
//-------------------------------------------------------------------------------------------------
class GFIAddSummands : public GameFieldInstruction
{
private:

	/// Serialize the instruction to a memory stream
	virtual void SubclassSerialize( Serializer& serializer );

public:

	/// The summand values
	std::list<int32> m_SummandValues;

	/// Get the type of instruction this object represents
	virtual EGameFieldInstruction GetType() const
	{
		return GFI_AddSummands;
	}
};

#endif // __GFIAddSummands_h