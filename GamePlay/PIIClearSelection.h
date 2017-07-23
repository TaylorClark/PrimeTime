//=================================================================================================
/*!
	\file PIIClearSelection.h
	Game Play Library
	Player Input Instruction Clear Selection Header
	\author Taylor Clark
	\date September 26, 2007

	This header contains the definition for the block select game field instruction.
*/
//=================================================================================================

#pragma once
#ifndef __PIIClearSelection_h
#define __PIIClearSelection_h

#include "GameFieldInstruction.h"
#include <list>

class GameFieldBlock;


//-------------------------------------------------------------------------------------------------
/*!
	\class PIIClearSelection
	\brief The player input instruction that unselects all selected blocks
*/
//-------------------------------------------------------------------------------------------------
class PIIClearSelection : public GameFieldInstruction
{
private:

	/// Serialize the instructions to a memory stream
	virtual void SubclassSerialize( Serializer& ){}

public:

	/// The blocks that lost selection
	std::list<uint32> DeselectedBlockIDs;

	/// Get the type of instruction this object represents
	virtual EGameFieldInstruction GetType() const
	{
		return GFI_ClearSelection;
	}
};

#endif // __PIIClearSelection_h