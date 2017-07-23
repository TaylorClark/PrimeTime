//=================================================================================================
/*!
	\file GUICtrlList.cpp
	2D Graphics Engine
	Graphical User Interface List Control Sourc
	\author Taylor Clark
	\date February 18, 2006

	This source file contains the implementation for the GUI list control class.
*/
//=================================================================================================

#include "../GUICtrlList.h"
#include "../GUIMgr.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Graphics2D/GraphicsDefines.h"
#include "Resource/ResourceMgr.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::GUICtrlList  Public
///
///	The default constructor
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlList::GUICtrlList() : m_CursorSprite( NULL ),
								m_pSelCallbackFunc( 0 ),
								m_AnimSprite( m_CursorSprite ),
								m_CurListItem( -1 )
{
#ifdef _DEBUG
	
	m_sName = L"Unnamed List";
	
	// Add the cursor sprite property
	GUICtrlProperty* pNewProp = new GUICtrlPropSprite( L"Sprite", &m_CursorSprite );
	m_PropList.push_back( pNewProp );
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::InternalGetBoundBox  Public
///
///	\returns The rectangle the encloses the control
///
///	Get the rectangle the encloses the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Box2i GUICtrlList::InternalGetBoundBox() const
{
	if( m_BoundBox.size.x * m_BoundBox.size.y == 0 )
		return CalculateBoundBox();

	return m_BoundBox;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::CalculateBoundBox  Public
///
///	\returns The rectangle the encloses the control
///
///	Calculate the bounding box the the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Box2i GUICtrlList::CalculateBoundBox() const
{
	Box2i retBox;
	retBox.pos = m_Pos;
	
	// If there is no font then bail
	if( !m_Font.GetObj() || m_Items.size() == 0 )
		return retBox;
	retBox.pos.y = m_Items[0].cachedDraw.GetPos().y;

	// Get the widest string
	for( ListItemVector::const_iterator iterItem = m_Items.begin(); iterItem != m_Items.end(); ++iterItem )
	{
		if( iterItem->cachedDraw.GetSize().x > retBox.size.x )
		{
			retBox.pos.x = iterItem->cachedDraw.GetPos().x;
			retBox.size.x = iterItem->cachedDraw.GetSize().x;
		}
	}

	// Get the bottom position of the final item
	const GUIListItem& lastItem = m_Items[ m_Items.size() - 1 ];
	retBox.size.y = lastItem.cachedDraw.GetPos().y + lastItem.cachedDraw.GetSize().y;
	retBox.size.y -= m_Items[0].cachedDraw.GetPos().y;

	//// Go through each list item
	//int fontHeight = m_Font->GetCharHeight();
	//for( ListItemVector::const_iterator iterItem = m_Items.begin();
	//		iterItem != m_Items.end();
	//		++iterItem )
	//{
	//	// If this is not the first item
	//	if( iterItem != m_Items.begin() )
	//	{
	//		// Add a half-font-height vertical seperation
	//		retBox.size.y += fontHeight / 2;
	//	}

	//	// Add in this line's height
	//	retBox.size.y += fontHeight;

	//	// Get the width of this line
	//	int itemWidth = iterItem->cachedDraw.m_Size.x;

	//	// If this item's width is greater than the current max width then store it
	//	if( itemWidth > retBox.size.x )
	//		retBox.size.x = itemWidth;
	//}

	// Return the box
	return retBox;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::OnMouseDown  Public
///
///	\param UNNAMED The mouse button that was pressed
///	\param mousePos The position of the mouse when the button was pressed
///	\returns True if the message was handled by the control, false otherwise
///
///	A message handler called when a mouse button is pressed over the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlList::OnMouseDown( GUIDefines::EMouseButtons, Point2i mousePos )
{
	int32 itemIndex = GetItemFromPos( mousePos );
	if( itemIndex < 0 )
		return false;

	// Execute the callback function if there is one
	if( m_pSelCallbackFunc )
		m_pSelCallbackFunc( (uint32)itemIndex, this );

	// We handled the message
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::SubclassTransferData  Public
///
/// \param serializer The serializer object that is used to read or write data
///
///	Load/save the sub-class control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlList::SubclassTransferData( Serializer& serializer )
{
	GUICtrlListBase::SubclassTransferData( serializer );

	// The cursor sprite
	ResourceID resID = 0;
	if( m_CursorSprite.GetObj() )
		resID = m_CursorSprite->GetResID();
	serializer.AddData( resID );
	if( serializer.InReadMode() )
		m_CursorSprite = ResourceMgr::Get().GetRefSprite( resID );

	// If we are loading then load the sprite
	if( serializer.InReadMode() )
	{
		m_CursorSprite = ResourceMgr::Get().GetRefSprite( resID );
		m_AnimSprite = AnimSprite( m_CursorSprite );
		m_AnimSprite.Play();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::Update  Public
///
///	\param UNNAMED The elapsed frame time
///
/// Update the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlList::Update( float32 )
{
	// Store which item the mouse cursor overlaps
	m_CurListItem = GetItemFromPos( GUIMgr::Get().GetCursorPos() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::GetItemFromPos  Private
///
///	\param screenPos The test position in screen coordinates
///	\returns The index of the item at the position, -1 if the position is invalid
///
/// Get an item index from a position.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 GUICtrlList::GetItemFromPos( Point2i screenPos ) const
{
	if( !m_Font.GetObj() )
		return -1;

	// Get the font height
	int32 fontHeight = m_Font->GetCharHeight();

	// Get the position in control coordinates
	Box2i boundBox = InternalGetBoundBox();
	Point2i controlPos( screenPos.x - boundBox.pos.x, screenPos.y - boundBox.pos.y );

	// Ensure a valid position
	if( controlPos.x < 0 || controlPos.y < 0 || controlPos.x > boundBox.size.x || controlPos.y > boundBox.size.y )
		return -1;

	// Get the item index
	int32 itemHeight = fontHeight + (fontHeight / 2);
	int32 itemIndex = (int32)( controlPos.y / itemHeight );
	if( itemIndex < 0 || itemIndex >= (int32)m_Items.size() )
		return -1;

	// Return the calculated index
	return itemIndex;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::ReformatControl  Public
///
///	Update the control based on its parameters so it displays correctly
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlList::ReformatControl()
{
	// Reformat the base class
	GUICtrlListBase::ReformatControl();

	// Update the cursor
	m_AnimSprite = AnimSprite( m_CursorSprite );
	m_AnimSprite.Play();

	// Get the widest string
	int32 widestString = 0;
	for( ListItemVector::iterator iterItem = m_Items.begin(); iterItem != m_Items.end(); ++iterItem )
	{
		if( iterItem->cachedDraw.GetSize().x > widestString )
			widestString = iterItem->cachedDraw.GetSize().x;
	}

	// If there is no font then bail
	if( !m_Font.GetObj() )
		return;

	// Go through the items and position them
	int fontHeight = m_Font->GetCharHeight();
	Vector2i curOffset( 0, m_Pos.y );
	for( ListItemVector::iterator iterItem = m_Items.begin(); iterItem != m_Items.end(); ++iterItem )
	{
		// If this is not the first item then add a half font height vertical seperation
		if( iterItem != m_Items.begin() )
			curOffset.y += fontHeight / 2;

		// Get the width of this line
		int itemWidth = iterItem->cachedDraw.GetSize().x;

		// Get the left position of the text
		curOffset.x = m_Pos.x + ((widestString - itemWidth) / 2);

		// Offset the text
		iterItem->cachedDraw.OffsetCharacters( curOffset );

		// Step down a line
		curOffset.y += fontHeight;
	}

	// Store the bounding box
	m_BoundBox = CalculateBoundBox();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlList::Draw  Public
///
///	Draw the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlList::Draw() const
{
	if( !m_Font.GetObj() )
		return;

	// Get the bounding box
	Box2i boundBox = InternalGetBoundBox();
	const int32 fontHeight = m_Font->GetCharHeight();

	// Update the animation if there is one
	m_AnimSprite.Update( GUIControl::sm_FrameTime );

	// Draw the list item text	
	int32 itemIndex = 0;
	for( ListItemVector::const_iterator iterItem = m_Items.begin(); iterItem != m_Items.end(); ++iterItem, ++itemIndex )
	{
		if( itemIndex == m_CurListItem && m_SelectedFont.GetObj() )
		{
			g_pGraphicsMgr->DrawFontText( m_SelectedFont.GetObj(), iterItem->sText.c_str(), iterItem->cachedDraw.GetPos() );
		}
		else
			g_pGraphicsMgr->DrawCachedFontText( iterItem->cachedDraw );
	}

	// If the mouse is not over this control or don't have a cursor sprite then bail or if the
	// mouse is over an item
	if( !m_MouseIsWithin || !m_CursorSprite.GetObj() || m_CurListItem < 0 || m_CurListItem >= (int32)m_Items.size() )
		return;

	// Get the vertical center position of the selected item
	int32 itemHeight = fontHeight + (fontHeight / 2);
	int32 itemPosY = (m_CurListItem * itemHeight) - (fontHeight / 4);
	itemPosY += (itemHeight / 2);

	// Get the Y position of the sprite
	int32 spriteY = itemPosY - (m_CursorSprite.GetObj()->GetSrcRect().size.y / 2);
	spriteY += m_Pos.y;


	// Get the width of this line
	int itemWidth = m_Items[m_CurListItem].cachedDraw.GetSize().x;

	// Get the left position of the text
	int textXPos = m_Pos.x + ((boundBox.size.x - itemWidth) / 2);


	// Draw the left mouse over sprite
	g_pGraphicsMgr->DrawImage( Point2i( textXPos - m_CursorSprite->GetSrcRect().size.x, spriteY ), m_AnimSprite.GetImage(), m_AnimSprite.GetSrcCoords() );

	// Draw the right mouse over sprite
	g_pGraphicsMgr->DrawImageEx( Box2i( Point2i(textXPos + itemWidth, spriteY), Vector2i(m_AnimSprite.GetSrcCoords().size) ), m_AnimSprite.GetImage(), m_AnimSprite.GetSrcCoords(), GraphicsDefines::DEF_FlipHoriz );
}