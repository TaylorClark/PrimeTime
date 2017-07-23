//=================================================================================================
/*!
	\file GameLogicTutorFractions.h
	Game Play Library
	Fraction Mode Tutorial Game Logic Header
	\author Taylor Clark
	\date January 30, 2010

	This header contains the definition for the Prime Time fraction mode tutorial logic.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicTutorFractions_h
#define __GameLogicTutorFractions_h

#include "GameLogicTutorBase.h"


class GameLogicTutorFractions : public GameLogicTutorBase< GameLogicFractions >
{
protected:

	/// Initialize a derived class's data
	virtual bool SubclassInit();

	/// Get the next fraction for the player to solve
	virtual Fraction GetNextFraction() const;

public:
	
	/// The default constructor
	GameLogicTutorFractions()
	{}

	/// The destructor
	virtual ~GameLogicTutorFractions() {}

	/// A message handler for when a block is selected that contains the mode-specific logic
	virtual GameLogic::EBlockSelAction OnBlockSelect( GameFieldBlock* pBlock );

	/// Generate blocks up to a certain width
	virtual GameFieldBlockMultBase** GenerateBlocksToFieldWidth();
};

#endif // __GameLogicTutorFractions_h