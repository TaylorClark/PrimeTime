//=================================================================================================
/*!
	\file GFIRemoveBlocks.h
	Game Play Library
	Game Logic Instruction Remove Blocks Header
	\author Taylor Clark
	\date September 30, 2007

	This header contains the definition for the game logic instruction to remove blocks.
*/
//=================================================================================================

#pragma once
#ifndef __GFIRemoveBlocks_h
#define __GFIRemoveBlocks_h

#include "GameFieldInstruction.h"
#include <list>

class GameFieldBlock;


//-------------------------------------------------------------------------------------------------
/*!
	\class GFIRemoveBlocks
	\brief The game logic input instruction that remove blocks
*/
//-------------------------------------------------------------------------------------------------
class GFIRemoveBlocks : public GameFieldInstruction
{
private:

	/// Serialize the instruction to a memory stream
	virtual void SubclassSerialize( Serializer& serializer );

public:
	
	/// The blocks that were removed
	std::list< uint32 > m_RemovedBlockIDs;

	/// The default constructor
	GFIRemoveBlocks(){}

	/// The constructor to initialize the instruction to toggle the selection of a block
	GFIRemoveBlocks( std::list<GameFieldBlock*> blocks );

	/// The constructor to initialize the instruction to toggle the selection of a block
	GFIRemoveBlocks( GameFieldBlock* pBlock );

	/// Get the type of instruction this object represents
	virtual EGameFieldInstruction GetType() const
	{
		return GFI_RemoveBlocks;
	}
};

#endif // __GFIRemoveBlocks_h