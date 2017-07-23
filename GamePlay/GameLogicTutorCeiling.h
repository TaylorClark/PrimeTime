//=================================================================================================
/*!
	\file GameLogicTutorCeiling.h
	Game Play Library
	Ceiling Mode Tutorial Game Logic Header
	\author Taylor Clark
	\date October 24, 2006

	This header contains the definition for the ceiling mode tutorial logic.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicTutorCeiling_h
#define __GameLogicTutorCeiling_h

#include "GameLogicTutorPT.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicTutorCeiling
	\brief Represents the logic for the ceiling tutorial

	This class defines the game logic for the tutorial that teaches players how to win at ceiling
	mode.
*/
//-------------------------------------------------------------------------------------------------
class GameLogicTutorCeiling : public GameLogicTutorPT
{
protected:

	/// Initialize a derived class's data
	virtual bool SubclassInit();

	/// Generate blocks up to a certain width
	virtual GameFieldBlockMultBase** GenerateBlocksToFieldWidth();
};

#endif // __GameLogicTutorCeiling_h