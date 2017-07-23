//=================================================================================================
/*!
	\file GameLogicProduct.h
	Game Play Library
	Product Mode Game Logic Header
	\author Taylor Clark
	\date June 22, 2006

	This header contains the definition for the product mode game logic manager.
*/
//=================================================================================================

#pragma once
#ifndef __GameLogicProduct_h
#define __GameLogicProduct_h

#include "GameLogicNormalBase.h"
#include "GameDefines.h"
#include <vector>
#include <list>
#include "GUI/MsgBox.h"
#include "GameSessionStatsMult.h"
#include "GameFieldBlock.h"

class GameFieldMultBase;
class GameFieldBlockMultBase;
class GameFieldBlockProduct;
class ProfileSubsetMultiplication;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameLogicProduct
	\brief Represents the logic for multiplication game modes

	This class defines the game logic that is used for multiplication modes.
*/
//-------------------------------------------------------------------------------------------------
class GameLogicProduct : public GameLogicNormalBase
{
protected:
	/// The number of completed equations
	int32 m_NumCompletedEquations;

	/// The multiplication version of the game fiel
	GameFieldMultBase* m_pGameFieldMult;

	/// A flag for if the current game is done
	bool m_IsGameDone;

	/// The current score
	uint32 m_Score;

	/// The current combo chain count
	uint32 m_ComboCount;
	
	/// The current session's statistics
	GameSessionStatsMult m_CurSessionStats;

	/// The current array of products that can be generated
	std::vector<GameDefines::ProductEntry> m_GeneratableProducts;

	/// The message box for moving to the next ceiling level
	//MsgBox* m_pNextLevelMsgBox;

	/// The product filter
	GameDefines::ProductSubsetFilter m_ProdFilter;

	/// If this is practice mode
	bool m_IsPractice;

	/// If the product preview label should be used
	bool m_ShowProductPreview;

	/// The flag for when a 0x block should be generated
	bool m_ReadyFor0xBlock;

	/// The number of product blocks on the field
	int32 m_NumProducts;

	/// The number of prime blocks on the field
	int32 m_NumPrimes;

	/// The offset of the required-number-of-primes-to-clear from what's out on the field.  A
	/// negative number means more of that prime are needed in order for the field to be
	/// completely clear able.
	int32 m_PrimeOffsets[ GameDefines::NUM_PRIMES ];

	/// The current range to use when generating a weighted prime.  This value is calculated by
	/// the sum of the weights in the current range of generatable primes.
	uint32 m_PrimeRandRange;


	/// Generate a new block
	virtual GameFieldBlockProduct* GenerateBlock( int32 maxWidth );

	/// Close the logic
	virtual void Term();

	// Increase the difficulty level
	void SetDifficulty( int32 level );

	/// Handle a completed equation
	EBlockSelAction HandleEquation( const BlockList& selBlocks );

	/// Get the value of the selected product
	int32 GetSelectedProduct(GameFieldBlockProduct* pSkipBlock) const;

	/// Setup the ceiling field
	void SetupCeilingField();

	/// The message box for callback when the ceiling mode goes up a level
	static void OnNextLevelMBCallback( MsgBox::EMsgBoxReturn retVal, void* pUserData );

protected:
	
	/// Initialize a derived class's data
	virtual bool SubclassInit();

public:

	/// The default constructor
	GameLogicProduct() : GameLogicNormalBase(),
							m_NumProducts( 0 ),
							m_NumPrimes( 0 ),
							m_PrimeRandRange( 1 ),
							m_NumCompletedEquations( 0 ),
							m_Score( 0 ),
							m_ComboCount( 0 ),
							m_pGameFieldMult( 0 ),
							m_IsPractice( false ),
							m_ShowProductPreview( false ),
							m_ReadyFor0xBlock( false )
	{
		// Clear the prime offset counts
		for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
			m_PrimeOffsets[ primeIndex ] = 0;
	}

	/// The destructor
	virtual ~GameLogicProduct();

	/// Initialize a practice game
	void InitPractice( GameDefines::ProductSubsetFilter prodFilter );

	/// A message handler for when a block is selected that contains the mode-specific logic
	virtual EBlockSelAction OnBlockSelect( GameFieldBlock* pBlock );

	/// A message handler for when a block is deselected
	virtual EBlockSelAction OnBlockDeselect( GameFieldBlock* );

	/// Generate blocks up to a certain width
	virtual GameFieldBlockMultBase** GenerateBlocksToFieldWidth();

	static GameDefines::ProductSubsetFilter GetFilterFromDiff( GameDefines::EGameplayDiffLevel diff, int32 level );
	
	/// Get the game field
	virtual GameField* GetGameField(){ return (GameField*)m_pGameFieldMult; }

	/// Is the game over
	virtual bool IsGameOver();

	/// Get the logic type
	virtual ELogicType GetLogicType() const { return LT_Product; }

	/// Get the session stats
	const GameSessionStatsMult& GetSessionStats() const { return m_CurSessionStats; }
};

#endif // __GameLogicProduct_h