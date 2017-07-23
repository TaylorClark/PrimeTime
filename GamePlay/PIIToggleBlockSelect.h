//=================================================================================================
/*!
	\file PIIToggleBlockSelect.h
	Game Play Library
	Player Input Instruction Select At Position Header
	\author Taylor Clark
	\date September 26, 2007

	This header contains the definition for the block select game field instruction.
*/
//=================================================================================================

#pragma once
#ifndef __PIIToggleBlockSelect_h
#define __PIIToggleBlockSelect_h

#include "GameFieldInstruction.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class PIIToggleBlockSelect
	\brief The player input instruction that toggles a block's selection status
*/
//-------------------------------------------------------------------------------------------------
class PIIToggleBlockSelect : public GameFieldInstruction
{
private:

	/// The block at the position
	uint32 m_BlockID;

	/// Serialize the instructions to a memory stream
	virtual void SubclassSerialize( Serializer& serializer )
	{
		serializer.AddData( m_BlockID );
	}

public:

	/// The default constructor
	PIIToggleBlockSelect(){}

	/// The constructor to initialize the instruction to toggle the selection of a block
	PIIToggleBlockSelect( uint32 blockID ) : m_BlockID( blockID )
	{
	}

	/// Get the type of instruction this object represents
	virtual EGameFieldInstruction GetType() const
	{
		return GFI_SelectBlock;
	}

	/// Get the selected block
	const uint32 GetBlockID() const { return m_BlockID; }
};

#endif // __PIIToggleBlockSelect_h