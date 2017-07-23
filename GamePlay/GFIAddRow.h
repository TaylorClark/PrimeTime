//=================================================================================================
/*!
	\file GFIAddRow.h
	Game Play Library
	Game Field Instruction Add Row Header
	\author Taylor Clark
	\date September 24, 2007

	This header contains the definition for the block row add game field instruction.
*/
//=================================================================================================

#pragma once
#ifndef __GFIAddRow_h
#define __GFIAddRow_h

#include "GameFieldInstruction.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GFIAddRow
	\brief The instruction that adds a row of blocks to the field
*/
//-------------------------------------------------------------------------------------------------
class GFIAddRow : public GameFieldInstruction
{
private:

	/// The row of blocks
	GameFieldBlock** m_ppBlocks;

public:

	/// The constructor to initialize the instruction
	GFIAddRow( GameFieldBlock** ppBlocks )
	{
		m_ppBlocks = ppBlocks;
	}

	/// Get the type of instruction this object represents
	virtual EGameFieldInstruction GetType() const
	{
		return GFI_AddRow;
	}
};

#endif // __GFIAddRow_h