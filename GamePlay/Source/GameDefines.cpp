/*=================================================================================================

	\file GameDefines.cpp
	Game Play Library
	Game Manager Source
	\author Taylor Clark
	\Date June 12, 2006

	This source file contains the implementation of the game defines functions.

=================================================================================================*/

#include "../GameDefines.h"
#include "Base/TCAssert.h"
#include "Resource/ResourceMgr.h"
#include <fstream>
#include "Base/MsgLogger.h"
#include "Base/FileFuncs.h"
#include <algorithm>
#include "Base/StringFuncs.h"
#include "Base/NetSafeSerializer.h"
#include "PrimeTime/ApplicationBase.h"


/// The default game font
static TCFontHndl g_DefaultGameFont;

/// The default sprites for message box buttons
static RefSpriteHndl g_NormalMsgBoxButtonSpr;
static RefSpriteHndl g_OverMsgBoxButtonSpr;
static RefSpriteHndl g_PressedMsgBoxButtonSpr;

static RefSpriteHndl g_MsgBoxBGSpr;

static RefSpriteHndl g_StaticSprite;


/// The array of products and prime factors that make them up
static GameDefines::ProductArray sg_ProductArray;

/// The font used to draw block text
static TCFontHndl g_BlockTextFont;


static void LoadNumberFile();


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameDefines::InitializeBaseObjects  Static Public
///
///	\returns Returns true on successful initialization, false otherwise
///
///	Initialize the data needed by miscellaneous game objects in Prime Time.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameDefines::InitializeBaseObjects()
{
	// Load the game font
	g_DefaultGameFont = ResourceMgr::Get().GetTCFont( RESID_FONT_LETTEROMATIC );
	if( !g_DefaultGameFont.GetObj() )
		return false;

	// Load the message box button sprites
	g_NormalMsgBoxButtonSpr = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DEFBTNNORM );
	g_OverMsgBoxButtonSpr = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DEFBTNOVER );
	g_PressedMsgBoxButtonSpr = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DEFBTNPRESSED );
	if( !g_NormalMsgBoxButtonSpr.GetObj() )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to load the normal message box buttons" );		
		return false;
	}

	// Load the message box background sprite
	g_MsgBoxBGSpr = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_MESSAGE_BOX_BG );
	if( !g_MsgBoxBGSpr.GetObj() )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to load the message box sprite" );
		return false;
	}

	// Load the non-prime block text font
	g_BlockTextFont = ResourceMgr::Get().GetTCFont( RESID_FONT_TWO_DIGIT );
	if( !g_BlockTextFont.GetObj() )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to load the non-prime block text font" );
		return false;
	}

	// Get the static sprites
	g_StaticSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_MENU_STATIC );
	if( !g_StaticSprite.GetObj() )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_CriticalError, L"Failed to load TV static sprite" );
		return false;
	}

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameDefines::FreeBaseObjects  Static Public
///
///	\returns Returns true on successful initialization, false otherwise
///
///	Free the base game data
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameDefines::FreeBaseObjects()
{
	g_DefaultGameFont.ReleaseHandle();
	g_NormalMsgBoxButtonSpr.ReleaseHandle();
	g_OverMsgBoxButtonSpr.ReleaseHandle();
	g_PressedMsgBoxButtonSpr.ReleaseHandle();
	g_MsgBoxBGSpr.ReleaseHandle();
	g_BlockTextFont.ReleaseHandle();
	g_StaticSprite.ReleaseHandle();
}

/// The default game font
TCFontHndl GameDefines::GetDefaultGameFont()
{
	return g_DefaultGameFont;
}

/// The default unpressed sprite for a message box button
RefSpriteHndl GameDefines::GetNormalMsgBoxButtonSpr()
{
	return g_NormalMsgBoxButtonSpr;
}

/// The default mouse-over sprite for a message box button
RefSpriteHndl GameDefines::GetOverMsgBoxButtonSpr()
{
	return g_OverMsgBoxButtonSpr;
}

/// The default pressed sprite for a message box button
RefSpriteHndl GameDefines::GetPressedMsgBoxButtonSpr()
{
	return g_PressedMsgBoxButtonSpr;
}

/// The sprite for the message box background
RefSpriteHndl GameDefines::GetMsgBoxBGSpr()
{
	return g_MsgBoxBGSpr;
}

/// Get the block text font
TCFontHndl GameDefines::GetBlockTextFont()
{
	return g_BlockTextFont;
}

/// Get the static sprites
RefSpriteHndl GameDefines::GetStaticSprite()
{
	return g_StaticSprite;
}

/// Get the array index of a prime
uint32 GameDefines::PrimeToIndex( int32 prime )
{
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		if( PRIMES[ primeIndex ] == prime )
			return primeIndex;
	}

	TCBREAKX( L"Invalid prime passed to PrimeToIndex function." );
	return GameDefines::NUM_PRIMES;
}

/// Get a range of numbers
GameDefines::ProductArray& GameDefines::GetProductArray()
{
	// If the product array needs to be loaded
	if( sg_ProductArray.size() == 0 )
		LoadNumberFile();
	return sg_ProductArray;
}


/// Get a subset of the product array
GameDefines::ProductArray GameDefines::GetProductArraySubset( const GameDefines::ProductSubsetFilter& filter, bool onlyRetrieveSize, uint32* pNumProd )
{
	// If the product array needs to be loaded
	if( sg_ProductArray.size() == 0 )
		LoadNumberFile();

	// If there is a variable to store the number of products
	if( pNumProd )
		*pNumProd = 0;

	// Loop through the products, most likely the internal conditionals will exit this for loop
	// before the for loop's conditional
	ProductArray retArray;
	uint32 disallowedPrimeFactors = ~filter.useablePrimes;
	for( uint32 productIndex = 0; productIndex < sg_ProductArray.size(); ++productIndex )
	{
		ProductEntry curProduct = sg_ProductArray[ productIndex ];
		
		// If the value is less than the minimum then continue
		if( curProduct.product < filter.minValue )
			continue;

		// If the value is beyond the maximum then there are no more products to search
		if( curProduct.product > filter.maxValue )
			break;

		// Ensure there aren't more or less factors than allowed
		if( curProduct.primeFactors.size() < filter.minNumFactors || curProduct.primeFactors.size() > filter.maxNumFactors )
			continue;

		// Ensure the product does not contain any primes greater than our max
		if( (curProduct.usedPrimeFlags & disallowedPrimeFactors) != 0 )
			continue;

		// Ensure this product includes all of the required primes
		if( (curProduct.usedPrimeFlags & filter.requiredPrimes) != filter.requiredPrimes )
			continue;

		// Store the number, this operation is best suited for a list, but it seems like it would
		// be even more overhead to make a list then copy it to a vector
		if( !onlyRetrieveSize )
			retArray.push_back( curProduct );
		if( pNumProd )
			*pNumProd = *pNumProd + 1;
	}

	// Return the array of valid products
	return retArray;
}


void LoadNumberFile()
{
	// Open the file
	std::ifstream inFile( TCBase::Narrow(ApplicationBase::GetResourcePath() + L"numbers.rdb").c_str(), std::ios_base::in | std::ios_base::binary );
	if( !inFile )
		return;
	NetSafeSerializer serializer( &inFile );

	// Read in the number of products
	uint32 numProducts = 0;
	serializer.AddData( numProducts );

	// Ensure a valid number (Less than a million in this case)
	if( numProducts > 1000000 )
	{
		inFile.close();
		return;
	}

	// Resize the array
	sg_ProductArray.resize( numProducts );

	// Read in each product
	for( uint32 productIndex = 0; productIndex < numProducts; ++productIndex )
	{
		// Read in the product value
		serializer.AddData( sg_ProductArray[productIndex].product );
		
		// Read in the number of factors
		uint8 numFactors = 0;
		serializer.AddData( numFactors );

		// If the number of factors is invalid then the file is corrupt
		if( numFactors > 60 )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Products file contains the product %d with more than 60 factors.", sg_ProductArray[productIndex].product );
			sg_ProductArray.resize(productIndex + 1);
			inFile.close();
			return;
		}

		// Read in the factors
		sg_ProductArray[productIndex].primeFactors.resize( numFactors );
		for( uint8 factorIndex = 0; factorIndex < numFactors; ++factorIndex )
		{
			// Read in the unsigned 8-bit factor and store it as a 32-bit value
			uint8 primeFactor = 0;
			serializer.AddData( primeFactor );
			sg_ProductArray[productIndex].primeFactors[factorIndex] = static_cast<int32>( primeFactor );

			// Mark the bit used
			uint32 primeIndex = GameDefines::PrimeToIndex( (int32)primeFactor );
			sg_ProductArray[productIndex].usedPrimeFlags |= 1 << primeIndex;
		}
	}
}


/// Get a descriptive string for the menu 
const wchar_t* GameDefines::GetMenuScreenDesc( EMenuScreens menuIndex )
{
	switch( menuIndex )
	{
	case GameDefines::MS_TitleScreen:
		return L"Title Screen";

	case GameDefines::MS_ProfileSelection:
		return L"Profile Selection";

	case GameDefines::MS_MultiplayerType:
		return L"Multiplayer Type";

	case GameDefines::MS_LearningAssistance:
		return L"Learning Assistance";

	case GameDefines::MS_ModeSelect:
		return L"Mode Select";

	case GameDefines::MS_ModeOptions:
		return L"Mode Options";

	case GameDefines::MS_InGame1Player:
		return L"In Game";

	case GameDefines::MS_CreateProfile:
		return L"Create Profile";

	case GameDefines::MS_ViewStats:
		return L"View Stats";

	case GameDefines::MS_LANLobby:
		return L"LAN Lobby";
	
	case GameDefines::MS_PreNetGame:
		return L"Pre-net-game";

	case GameDefines::MS_HelpMeLearn:
		return L"Help Me Learn";

	case GameDefines::MS_PracticeSetup:
		return L"Practice Setup";

	case GameDefines::MS_Tutorials:
		return L"Tutorials";

	case GameDefines::MS_GameResults:
		return L"Game Results";

	case GameDefines::MS_DemoExit:
        return L"Demo Exit";
            
    default:
        // Stop warnings on mac
        break;
	}

	return L"Unknown Menu Screen";
}


//TODO localize
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameDefines::GetGameTypeDesc  Static Public
///
///	Get a descriptive string for the game type.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* GameDefines::GetGameTypeDesc( EGameType gameType )
{
	switch( gameType & GT_Mask_Type )
	{
	case GT_PrimeTime:
	{
		// Handle the mods
		switch( gameType & GT_Mask_Mod )
		{
			case GT_Mod_Tutorial:
				return L"Game Play Tutorial";
			
			case GT_Mod_Tutorial2:
				return L"Combo Tutorial";
				
			case GT_Mod_Practice:
				return L"Prime Time Practice";

			case GT_Mod_LearningAssitance:
				return L"Learning Assistance";
		}
		
		// If there is no mod
		if( (gameType & GT_Mask_Mod) == 0 )
			return L"Prime Time";
	}
	break;

	case GT_Ceiling:
	{
		if( (gameType & GT_Mask_Mod) == GT_Mod_Tutorial )
			return L"Ceiling Tutorial";

		return L"Ceiling";
	}
	break;

	case GT_AdditionAttack:
	{
		if( (gameType & GT_Mask_Mod) == GT_Mod_Tutorial )
			return L"Addition Attack Tutorial";

		return L"Addition Attack";
	}
	break;

	case GT_Fractions:
	{
		if( (gameType & GT_Mask_Mod) == GT_Mod_Tutorial )
			return L"Fraction Fumble Tutorial";

		return L"Fraction Fumble";
	}
	break;

	}

	TCBREAKX( L"Invalid game type" );
	return L"Unknown Game Type";
}


//TODO localize
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameDefines::GetDifficultyDesc  Static Public
///
///	Get a descriptive string for the gameplay difficulty levels.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* GameDefines::GetDifficultyDesc( EGameplayDiffLevel difficulty )
{
	switch( difficulty )
	{
		case GPDL_Pre3:
			return L"Super Easy";

		case GPDL_3:
			return L"Really Easy";

		case GPDL_4:
			return L"Easy";

		case GPDL_5:
			return L"Normal";

		case GPDL_6:
			return L"Tough";

		case GPDL_7:
			return L"Hard";

		case GPDL_8:
			return L"Really Hard";

		case GPDL_Post8:
			return L"Super Hard";

		case GPDL_Secret1:
			return L"Secret Hard";

		case GPDL_Secret2:
			return L"Super Secret Hard";
            
        default:
            break;
	}

	return L"Unknown Difficulty";
}