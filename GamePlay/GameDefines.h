//=================================================================================================
/*!
	\file GameDefines.h
	2D Game Engine
	Graphical User Interface Defines Header
	\author Taylor Clark
	\date June 29, 2006

	This file contains defines related to the gameplay library.
*/
//=================================================================================================

#pragma once
#ifndef __GameDefines_h
#define __GameDefines_h

#include "Base/Types.h"
#include <vector>
#include "Graphics2D/TCFont.h"
#include "Graphics2D/RefSprite.h"
#include "PrimeTime/ResourceIDs.h"
#include "Base/Serializer.h"


namespace GameDefines
{
	/// A product and the prime factors that multiply to produce it
	struct ProductEntry
	{
		/// The product's value
		int32 product;

		/// The product's prime factors sorted ascendingly
		std::vector<int32> primeFactors;

		/// The flags the represent what primes are used by this product
		uint32 usedPrimeFlags;

		/// The default constructor
		ProductEntry() : product( 0 ), usedPrimeFlags(0)
		{}
	};

	/// An array of products, sorted ascendingly
	typedef std::vector< ProductEntry > ProductArray;

	/// The prime factors
	static const int32 PRIMES[] = { 2,3,5,7,11,13,17,19 };
	static const uint32 NUM_PRIMES = sizeof(PRIMES) / sizeof(PRIMES[0]);

	/// The data used when creating a subset of the availabe products
	struct ProductSubsetFilter
	{
		ProductSubsetFilter() : minValue( 0 ),
								maxValue( 10000 ),
								minNumFactors( 2 ),
								maxNumFactors( 100 ),
								useablePrimes( (uint32)~0 ),
								requiredPrimes( 0 )
		{}

		void SetMaxPrimeByIndex( uint32 maxPrimeIndex )
		{
			// Set every bit below the max allowed product
			// Ex. 7 has the index of 3
			// 1 << 3 = 01000     01000 - 1 = 00111
			// (00111 << 1) + 1 = 01111
			useablePrimes = (1 << maxPrimeIndex) - 1;
			useablePrimes = (useablePrimes << 1) + 1;
		}

		void SetPrimeUsage( uint32 primeIndex, bool usePrime )
		{
			if( usePrime )
				useablePrimes |= 1 << primeIndex;
			else
				useablePrimes &= ~(1 << primeIndex);
		}

		/// Save/load the filter from file
		void Serialize(	Serializer& serializer )
		{
			// Write the size to determine the version
			uint32 objVersion = 1;
			serializer.AddData( objVersion );

			if( objVersion == 1 )
			{
				serializer.AddData( useablePrimes );
				serializer.AddData( useablePrimes );
				serializer.AddData( requiredPrimes );
				serializer.AddData( minValue );
				serializer.AddData( maxValue );
				serializer.AddData( minNumFactors );
				serializer.AddData( maxNumFactors );
			}
		}

		/// The flags the represent which primes should be used
		uint32 useablePrimes;

		/// The flags the represent which primes should be used
		uint32 requiredPrimes;

		/// The minimum product allowed
		int32 minValue;

		/// The maximum product allowed
		int32 maxValue;

		/// The minimum number of factors a product must have to be returned
		uint32 minNumFactors;

		/// The maximum number of factors allowed for the returned products
		uint32 maxNumFactors;
	};

	/// The type of Prime Time gameplay mode
	enum EPrimeTimeGameMode
	{
		PTGM_Normal = 0,
		PTGM_Practice,
		PTGM_LearningAssistance
	};

	/// The weight to generate the primes
	static const uint32 PRIME_WEIGHTS[] = { 16, 9, 7, 5, 4, 3, 2, 1 };

	/// The sprite resource ID for the prime block
	static const uint32 PRIME_SPRITE_ID[] = { RESID_SPRITE_2_BLOCK,
												RESID_SPRITE_3_BLOCK,
												RESID_SPRITE_5_BLOCK,
												RESID_SPRITE_7_BLOCK,
												RESID_SPRITE_11_BLOCK,
												RESID_SPRITE_13_BLOCK,
												RESID_SPRITE_17_BLOCK,
												RESID_SPRITE_19_BLOCK };

	/// The hilited sprite resource ID for the prime block
	static const uint32 PRIME_HILITED_SPRITE_ID[] = { RESID_SPRITE_2_BLOCK_HILITED,
												RESID_SPRITE_3_BLOCK_HILITED,
												RESID_SPRITE_5_BLOCK_HILITED,
												RESID_SPRITE_7_BLOCK_HILITED,
												RESID_SPRITE_11_BLOCK_HILITED,
												RESID_SPRITE_13_BLOCK_HILITED,
												RESID_SPRITE_17_BLOCK_HILITED,
												RESID_SPRITE_19_BLOCK_HILITED };

	/// The sprite ID for the 0x block
	static const uint32 PRIME_SPRITE_0x = RESID_SPRITE_0X;

	/// A hard-coded-but-shouldnt-be value of the maximum number of factors needed to generate the
	/// maximim product
	static const uint32 MAX_PRIME_FACTORS_NEEDED = 12;

	/// The number of products/fractions cleared needed to progress a level
	static const int32 NUM_CLEAR_PER_LEVEL = 10;

	/// The key code for the key that will speed up the block production (VK_CONTROL = 17)
	static const int32 KEY_CODE_FASTPUSH = 17;

	/// The size of a block in pixels
	static const int32 BLOCK_WIDTH = 55;
	static const int32 BLOCK_HEIGHT = 55;

	/// Initialize the game data
	bool InitializeBaseObjects();

	/// Free the game data
	void FreeBaseObjects();

	/// Get the array index of a prime
	uint32 PrimeToIndex( int32 prime );

	/// Get a range of numbers
	ProductArray& GetProductArray();

	/// Get a subset of the product array
	ProductArray GetProductArraySubset( const ProductSubsetFilter& filter, bool onlyRetrieveSize = false, uint32* pNumProd = 0 );

	/// The default game font
	TCFontHndl GetDefaultGameFont();

	/// The default unpressed sprite for a message box button
	RefSpriteHndl GetNormalMsgBoxButtonSpr();

	/// The default mouse-over sprite for a message box button
	RefSpriteHndl GetOverMsgBoxButtonSpr();

	/// The default pressed sprite for a message box button
	RefSpriteHndl GetPressedMsgBoxButtonSpr();

	/// The sprite for the message box background
	RefSpriteHndl GetMsgBoxBGSpr();

	/// Get the block text font
	TCFontHndl GetBlockTextFont();

	/// Get the TV static sprite
	RefSpriteHndl GetStaticSprite();

	/// The different menu screens NOTE: Only add to the end of this list
	enum EMenuScreens
	{
		MS_Error = -1,
		MS_TitleScreen,
		MS_ProfileSelection,
		MS_MultiplayerType,
		MS_LearningAssistance,
		MS_ModeSelect,
		MS_Tutorials,
		MS_ModeOptions,
		MS_GameResults,
		MS_InGame1Player,
		MS_CreateProfile,
		MS_ViewStats,
		MS_LANLobby,
		MS_PreNetGame,
		MS_HelpMeLearn,
		MS_PracticeSetup,
		MS_DemoExit,
		MS_COUNT
	};

	/// The types of gameplay
#ifdef WIN32
	enum EGameType : uint16
#else
	enum EGameType
#endif
	{
		GT_Error = 0,
		GT_PrimeTime = 0x0001,
		GT_Ceiling = 0x0002,
		GT_AdditionAttack = 0x0003,
		GT_Fractions = 0x0004,
		GT_Num_Types = 4,
		GT_Mask_Type = 0x0FFF,
		GT_Mod_Normal = 0x0000,
		GT_Mod_Practice = 0x1000,
		GT_Mod_Tutorial = 0x2000,
		GT_Mod_LearningAssitance = 0x3000,
		GT_Mod_Tutorial2 = 0x4000,
		GT_Mask_Mod = 0xF000,
		GT_Mask_LS_Offset = 12,
		GT_Mod_NumBits = 4
	};

	/// The gameplay difficulty level
	enum EGameplayDiffLevel
	{
		GPDL_Pre3 = 0,
		GPDL_3,
		GPDL_4,
		GPDL_5,
		GPDL_6,
		GPDL_7,
		GPDL_8,
		GPDL_Post8,
		GPDL_Secret1,
		GPDL_Secret2,
		GPDL_COUNT
	};

	/// Get a descriptive string for the menu 
	const wchar_t* GetMenuScreenDesc( EMenuScreens menuIndex );

	/// Get a descriptive string for the game type
	const wchar_t* GetGameTypeDesc( EGameType gameType );

	/// Get a descriptive string for the gameplay difficulty levels
	const wchar_t* GetDifficultyDesc( EGameplayDiffLevel difficulty );
};

#endif // __GameDefines_h