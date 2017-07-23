/*=================================================================================================

	\file GameFieldFractions.cpp
	Game Play Library
	Fractions Game Field Source
	\author Taylor Clark
	\Date December 24, 2009

	This source file contains the implementation of the fraction mode of the game field class.

=================================================================================================*/

#include "../GameFieldFractions.h"
#include "Graphics2D/GraphicsMgr.h"
#include "PrimeTime/ResourceIDs.h"
#include "../GameDefines.h"
#include "Base/NumFuncs.h"
#include "GUI/GUIMgr.h"
#include "../GameFieldBlockMultBase.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameFieldFractions::GetBlockAtPos  Public
///	\param fieldPos The position to test for a block in field coordinates
///	\returns A pointer to the block at the passed in position or NULL if there none
///
///	Find a block at a position in the field.
///////////////////////////////////////////////////////////////////////////////////////////////////
GameFieldBlock* GameFieldFractions::GetBlockAtPos( const Point2i& fieldPos )
{
	// Convert the field position into screen position since that is how we store the numerator
	// and denominator position
	Point2i screenPos = BlockFieldToScreenPos( fieldPos );

	// If the user clicked on the numerator
	if( _numeratorBlock.BoundBox.IsPointInside(screenPos) )
		return pNumeratorPlaceholderBlock;
	// Or if the user clicked on the denominator
	else if( _denominatorBlock.BoundBox.IsPointInside(screenPos) )
		return pDenominatorPlaceholderBlock;
	
	// Otherwise let the normal handling occur
	return GameFieldMultBase::GetBlockAtPos( fieldPos );
}

/// Draw the game
void GameFieldFractions::Draw() const
{
	GameFieldMultBase::Draw();
	
	DrawBlock( _numeratorBlock, IsNumeratorSelected );

	Box2i lineRect( NumeratorBlockCenterPos.x - GameDefines::BLOCK_WIDTH - 5, NumeratorBlockCenterPos.y + (GameDefines::BLOCK_HEIGHT / 2) + 5, (GameDefines::BLOCK_WIDTH * 2) + 10, 2 );
	g_pGraphicsMgr->DrawRect( lineRect, 0 );

	DrawBlock( _denominatorBlock, !IsNumeratorSelected );
}

/// The destructor
GameFieldFractions::~GameFieldFractions()
{
	if( pNumeratorPlaceholderBlock )
		delete pNumeratorPlaceholderBlock;
	pNumeratorPlaceholderBlock = 0;

	if( pDenominatorPlaceholderBlock )
		delete pDenominatorPlaceholderBlock;
	pDenominatorPlaceholderBlock = 0;
}


bool GameFieldFractions::Init()
{
	SetSelectedNumerator( -1 );
	SetSelectedDenominator( -1 );

	pNumeratorPlaceholderBlock = new GameFieldBlockMultBase( 1 );
	pDenominatorPlaceholderBlock = new GameFieldBlockMultBase( 1 );

	// Add the placeholder blocks to the ID map
	m_BlockIDMap.insert( std::pair<uint32,GameFieldBlock*>(pNumeratorPlaceholderBlock->GetID(),pNumeratorPlaceholderBlock) );
	m_BlockIDMap.insert( std::pair<uint32,GameFieldBlock*>(pDenominatorPlaceholderBlock->GetID(),pDenominatorPlaceholderBlock) );

	return GameFieldMultBase::Init();
}

void GameFieldFractions::SetSelectedNumerator(int32 value)
{
	SetupBlock( _numeratorBlock, value );
}

void GameFieldFractions::SetSelectedDenominator(int32 value)
{
	SetupBlock( _denominatorBlock, value );
	
	// Offset the block downwards since it it centered at the numerator's position right now
	_denominatorBlock.BoundBox.pos.y += GameDefines::BLOCK_HEIGHT + 10;
}

void GameFieldFractions::SetupBlock( FractionBlock& block, int32 value )
{
	block.Value = value;

	// Store the font
	if( block.Value != -1 )
	{
		block.pValueFont = GameDefines::GetBlockTextFont().GetObj();
		block.ValueString = TCBase::EasyIToA(value);
	}
	else
	{
		block.pValueFont = ResourceMgr::Get().GetTCFont( RESID_FONT_LETTEROMATIC ).GetObj();
		block.ValueString = L"?";
	}

	// If the box is wider than two digits
	if( block.ValueString.length() > 2 )
	{
		block.IsWide = true;

		RefSprite* pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK ).GetObj();

		block.BoundBox = pBlockSprite->GetSrcRect();
		block.BoundBox.pos = NumeratorBlockCenterPos - (block.BoundBox.size / 2);
	}
	else
	{
		block.IsWide = false;

		RefSprite* pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK ).GetObj();

		block.BoundBox = pBlockSprite->GetSrcRect();
		block.BoundBox.pos = NumeratorBlockCenterPos - (block.BoundBox.size / 2);
	}

	// Center the number text
	block.TextOffset.x = (block.BoundBox.size.x - block.pValueFont->CalcStringWidth( block.ValueString.c_str() )) / 2;
	block.TextOffset.y = (block.BoundBox.size.y - block.pValueFont->GetCharHeight()) / 2;
}


void GameFieldFractions::DrawBlock( const FractionBlock& block , bool isSelected ) const
{
	// Get the cursor position so we can hi-lite blocks
	Point2i cursorScreenPos = GUIMgr::Get().GetCursorPos();

	// If the user has selected a numerator
	RefSprite* pBlockSprite;
	if( block.IsWide )
	{
		// If the mouse is over the box
		if( block.BoundBox.IsPointInside(cursorScreenPos) )
			pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DBL_PRDCT_BLOCK_HILITED ).GetObj();
		else
			pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DOUBLE_PRODUCT_BLOCK ).GetObj();
	}
	else
	{
		// If the mouse is over the box
		if( block.BoundBox.IsPointInside(cursorScreenPos) )
			pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK_HILITED ).GetObj();
		else
			pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK ).GetObj();
	}

	// Draw the block
	g_pGraphicsMgr->DrawSprite( block.BoundBox.pos, pBlockSprite );

	// Draw the text
	g_pGraphicsMgr->DrawFontText( block.pValueFont, block.ValueString.c_str(), block.BoundBox.pos + block.TextOffset );

	if( isSelected )
	{
		if( block.IsWide )
			g_pGraphicsMgr->DrawImage( block.BoundBox.pos, m_WideSelBracketSprite.GetImage(), m_WideSelBracketSprite.GetSrcCoords() );
		else
			g_pGraphicsMgr->DrawImage( block.BoundBox.pos, m_SelBracketSprite.GetImage(), m_SelBracketSprite.GetSrcCoords() );
	}
}

void GameFieldFractions::DrawBlock( const Point2i& blockCenterPos, const std::wstring& sText, bool isSelected ) const
{
	TCFont* pBlockFont;
	if( sText == L"?" )
		pBlockFont = ResourceMgr::Get().GetTCFont( RESID_FONT_LETTEROMATIC ).GetObj();
	else
		pBlockFont = GameDefines::GetBlockTextFont().GetObj();

	// Get the cursor position so we can hi-lite blocks
	Point2i cursorScreenPos = GUIMgr::Get().GetCursorPos();

	// If the user has selected a numerator
	RefSprite* pBlockSprite;
	Box2i boxSpriteRect;
	bool isWide;
	if( sText.length() > 2 )
	{
		isWide = true;

		pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DOUBLE_PRODUCT_BLOCK ).GetObj();

		boxSpriteRect = pBlockSprite->GetSrcRect();
		boxSpriteRect.pos = blockCenterPos - (boxSpriteRect.size / 2);

		// If the mouse is over the box
		if( boxSpriteRect.IsPointInside(cursorScreenPos) )
			pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_DBL_PRDCT_BLOCK_HILITED ).GetObj();
	}
	else
	{
		isWide = false;

		pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK ).GetObj();

		boxSpriteRect = pBlockSprite->GetSrcRect();
		boxSpriteRect.pos = blockCenterPos - (boxSpriteRect.size / 2);

		// If the mouse is over the box
		if( boxSpriteRect.IsPointInside(cursorScreenPos) )
			pBlockSprite = ResourceMgr::Get().GetRefSprite( RESID_SPRITE_PRODUCT_BLOCK_HILITED ).GetObj();
	}

	// Draw the block
	g_pGraphicsMgr->DrawSprite( boxSpriteRect.pos, pBlockSprite );

	// Center the number text
	Vector2i textOffset;
	textOffset.x = (boxSpriteRect.size.x - pBlockFont->CalcStringWidth( sText.c_str() )) / 2;
	textOffset.y = (boxSpriteRect.size.y - pBlockFont->GetCharHeight()) / 2;
	
	// Draw the text
	g_pGraphicsMgr->DrawFontText( pBlockFont, sText.c_str(), boxSpriteRect.pos + textOffset );

	if( isSelected )
	{
		if( isWide )
			g_pGraphicsMgr->DrawImage( boxSpriteRect.pos, m_WideSelBracketSprite.GetImage(), m_WideSelBracketSprite.GetSrcCoords() );
		else
			g_pGraphicsMgr->DrawImage( boxSpriteRect.pos, m_SelBracketSprite.GetImage(), m_SelBracketSprite.GetSrcCoords() );
	}
}