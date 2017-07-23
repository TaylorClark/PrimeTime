//=================================================================================================
/*!
	\file GameLogicTutorPT.h
	Game Play Library
	Prime Time Mode Tutorial Game Logic Header
	\author Taylor Clark
	\date October 24, 2006

	This header contains the definition for the Prime Time game mode tutorial logic.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicTutorPT_h
#define __GameLogicTutorPT_h

#include "GameLogicProduct.h"
#include "GameLogicTutorBase.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicMgrProduct
	\brief Represents the game manager

	This class defines the game manager class that is used to handle gameplay update.
*/
//-------------------------------------------------------------------------------------------------
class GameLogicTutorPT : public GameLogicTutorBase< GameLogicProduct >
{
	typedef GameLogicTutorBase< GameLogicProduct > BaseType;

protected:

	/// Initialize a derived class's data
	virtual bool SubclassInit();

public:
	
	/// The default constructor
	GameLogicTutorPT()
	{}

	/// The destructor
	virtual ~GameLogicTutorPT() {}

	/// A message handler for when a block is selected that contains the mode-specific logic
	virtual GameLogic::EBlockSelAction OnBlockSelect( GameFieldBlock* pBlock );

	/// Generate blocks up to a certain width
	virtual GameFieldBlockMultBase** GenerateBlocksToFieldWidth();

	/// Generate a row of blocks
	static void GenerateRow( const int32 vals[5], GameFieldBlockMultBase** pFieldBlocks );
};

#endif // __GameLogicTutorPT_h