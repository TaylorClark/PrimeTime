//=================================================================================================
/*!
	\file GameLogicTutorPTCombo.h
	Game Play Library
	Prime Time Combo Tutorial Game Logic Header
	\author Taylor Clark
	\date October 24, 2006

	This header contains the definition for the Prime Time combo tutorial logic.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicTutorPTCombo_h
#define __GameLogicTutorPTCombo_h

#include "GameLogicTutorPT.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicTutorPTCombo
	\brief Represents the logic for the combo tutorial

	This class defines the game logic for the tutorial to teach players how to take advantage of
	combos.
*/
//-------------------------------------------------------------------------------------------------
class GameLogicTutorPTCombo : public GameLogicTutorPT
{
protected:

	/// Initialize a derived class's data
	virtual bool SubclassInit();

	/// Generate blocks up to a certain width
	virtual GameFieldBlockMultBase** GenerateBlocksToFieldWidth();
};

#endif // __GameLogicTutorPTCombo_h