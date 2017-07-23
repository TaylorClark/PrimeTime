//=================================================================================================
/*!
	\file GameLogicFractions.h
	Game Play Library
	Fractions Mode Game Logic Header
	\author Taylor Clark
	\date December 24, 2009

	This header contains the definition for the produect mode game logic manager.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicFractions_h
#define __GameLogicFractions_h

#include "GameLogicNormalBase.h"
#include "FractionModeSettings.h"
#include "GameSessionStatsFractions.h"

class GameFieldFractions;
class GameFieldBlockMultBase;
class GameFieldBlockFraction;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicFractions
	\brief Represents the logic for fraction game mode

	This class defines the game logic that is used for the fractions mode. The game logic objects
	are used to control the field (Add rows of blocks), the UI (What text is displayed), and the
	input (Remove a valid equation).
*/
//-------------------------------------------------------------------------------------------------
class GameLogicFractions : public GameLogicNormalBase
{
private:

	Fraction _equivalentFraction;

	/// The number of solutions that exist on the field for each possible fraction in the current
	/// difficulty
	std::vector<uint8> _solutionFractionPossibilities;

	/// The number of blocks on the field of each number
	std::vector<uint8> _fractionValueCounts;

	const FractionModeSettings* _pCurModeSettings;

	GameSessionStatsFractions _curSessionStats;

	/// Generate a new block
	GameFieldBlockFraction* GenerateBlock( int32 maxWidth );

	int32 GetNextFractionBlockValue(int32 maxBlockWidth) const;

protected:
	
	/// Get the next fraction for the player to solve
	virtual Fraction GetNextFraction() const;

	GameFieldFractions* _pGameFieldFractions;

	/// Initialize a derived class's data
	virtual bool SubclassInit();

	void SetDifficulty( int32 difficulty );

	void CalculatePossibleSolutionCounts();

	/// Close the logic
	virtual void Term();

public:

	GameLogicFractions() : _pGameFieldFractions(0),
							_equivalentFraction(1,2),
							_pCurModeSettings( 0 )
	{
	}

	/// The destructor
	virtual ~GameLogicFractions();

	/// Generate blocks up to a certain width
	GameFieldBlockMultBase** GenerateBlocksToFieldWidth();

	/// A message handler for when a block is selected
	virtual EBlockSelAction OnBlockSelect( GameFieldBlock* pBlock );

	/// A message handler for when a block is deselected
	virtual EBlockSelAction OnBlockDeselect( GameFieldBlock* );

	/// Is the game over
	virtual bool IsGameOver();

	/// Get the game field
	virtual GameField* GetGameField(){ return (GameField*)_pGameFieldFractions; }

	/// Get the logic type
	virtual ELogicType GetLogicType() const { return LT_Fraction; }

	/// Get the session stats
	const GameSessionStatsFractions& GetSessionStats() const { return _curSessionStats; }
};

#endif // __GameLogicFractions_h