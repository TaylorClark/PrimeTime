//=================================================================================================
/*!
	\file GameFieldFractions.h
	Game Play Library
	Fractions Game Field Header
	\author Taylor Clark
	\date December 24, 2009

	This header contains the definition for the fractions mode game field object.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldFractions_h
#define __GameFieldFractions_h

#include "GameFieldMultBase.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldCeiling
	\brief Represents a game field used for the ceiling version of gameplay.

	This class defines the game field class that is used to encapsulate data related to the block
	layout of a game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldFractions : public GameFieldMultBase
{
private:
	struct FractionBlock
	{
		/// The bounding box in screen space
		Box2i BoundBox;

		int32 Value;

		std::wstring ValueString;

		TCFont* pValueFont;

		/// If the value needs two blocks to fit, i.e. is greater than 99
		bool IsWide;

		Vector2i TextOffset;

		FractionBlock() : Value( -1 ),
							pValueFont( 0 ),
							IsWide( false )
		{
		}
	};

	FractionBlock _numeratorBlock;

	FractionBlock _denominatorBlock;

	void SetupBlock( FractionBlock& block, int32 value );

	void DrawBlock( const Point2i& blockPos, const std::wstring& sText, bool isSelected ) const;

	void DrawBlock( const FractionBlock& block , bool isSelected ) const;

protected:

	virtual GameFieldBlock* GetBlockAtPos( const Point2i& fieldPos );

public:

	bool IsNumeratorSelected;

	Point2i NumeratorBlockCenterPos;

	/// A block that represents the numerator block in the OnBlockSelect function
	GameFieldBlockMultBase* pNumeratorPlaceholderBlock;
	
	/// A block that represents the denominator block in the OnBlockSelect function
	GameFieldBlockMultBase* pDenominatorPlaceholderBlock;

	/// The default constructor
	GameFieldFractions() : IsNumeratorSelected( true ),
							pNumeratorPlaceholderBlock( 0 ),
							pDenominatorPlaceholderBlock( 0 )
	{
	}

	/// The destructor
	virtual ~GameFieldFractions();

	/// Draw the game
	virtual void Draw() const;

	/// Initialize the field
	virtual bool Init();

	void SetSelectedNumerator(int32 value);

	void SetSelectedDenominator(int32 value);

	int32 GetSelectedNumerator() const { return _numeratorBlock.Value; }

	int32 GetSelectedDenominator() const { return _denominatorBlock.Value; }
};

#endif // __GameFieldFractions_h