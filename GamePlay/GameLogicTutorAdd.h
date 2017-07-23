//=================================================================================================
/*!
	\file GameLogicTutorAdd.h
	Game Play Library
	Addtion Attack Mode Tutorial Game Logic Header
	\author Taylor Clark
	\date November 7, 2006

	This header contains the definition for the addition attack game mode tutorial logic.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicTutorAdd_h
#define __GameLogicTutorAdd_h

#include "GameLogicAdd.h"
#include "GameLogicTutorBase.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicTutorAdd
	\brief Represents the game logic for the addition attack tutorial
*/
//-------------------------------------------------------------------------------------------------
class GameLogicTutorAdd : public GameLogicTutorBase< GameLogicAdd >
{
	typedef GameLogicTutorBase< GameLogicAdd > BaseType;
private:

	/// The number of blocks of added to the field
	int32 m_BlockGenCount;

	/// The number of sums generated
	int32 m_NumSumsGen;

protected:

	/// Initialize a derived class's data
	virtual bool SubclassInit();

	/// Generate blocks
	virtual void GenerateLackingBlocks();

	/// Generate a sum
	virtual int32 GenerateSum();

public:
	
	/// The default constructor
	GameLogicTutorAdd() : m_BlockGenCount( 0 ),
							m_NumSumsGen( 0 )
	{}

	/// The destructor
	virtual ~GameLogicTutorAdd() {}
};

#endif // __GameLogicTutorAdd_h