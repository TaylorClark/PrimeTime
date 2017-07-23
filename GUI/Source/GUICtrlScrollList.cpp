//=================================================================================================
/*!
	\file GUICtrlScrollList.cpp
	GUI Library
	Graphical User Interface List Control Header
	\author Taylor Clark
	\date February 18, 2006

	This source file contains the implementation for the GUI list control class.
*/
//=================================================================================================

#include "../GUICtrlScrollList.h"
//#include "../GUIMgr.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Resource/ResourceMgr.h"
#include "../GUICtrlButton.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::GUICtrlScrollList()  Public
///
///	\returns The rectangle the encloses the control
///
///	The default constructor
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlScrollList::GUICtrlScrollList() : m_SelItemIndex( -1 ),
								m_Dims( 200, 600 ),
								m_NumVisItems( 0 ),
								m_FirstVisIndex( 0 ),
								_OnItemSelectCallback( 0 )
{
#ifdef _DEBUG
	
	m_sName = L"Unnamed Scrollable List";
	
	// Add the dimensions property
	GUICtrlProperty* pNewProp = new GUICtrlPropInt( L"Width", &m_Dims.x );
	m_PropList.push_back( pNewProp );
	pNewProp = new GUICtrlPropInt( L"Height", &m_Dims.y );
	m_PropList.push_back( pNewProp );
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::InternalGetBoundBox()  Public
///
///	\returns The rectangle the encloses the control
///
///	Get the rectangle the encloses the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Box2i GUICtrlScrollList::InternalGetBoundBox() const
{
	return Box2i( m_Pos, m_Dims );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::OnMouseDown()  Public
///
///	\param UNNAMED The mouse button that was pressed
///	\param mousePos The position of the mouse when the button was pressed
///	\returns True if the message was handled by the control, false otherwise
///
///	A message handler called when a mouse button is pressed over the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlScrollList::OnMouseDown( GUIDefines::EMouseButtons, Point2i mousePos )
{
	SetSelItemIndex( GetItemFromPos( mousePos ) );

	// We handled the message
	return true;
}


/// Set the selected index
void GUICtrlScrollList::SetSelItemIndex( int32 newSelectedIndex )
{
	if( newSelectedIndex < 0 || (uint32)newSelectedIndex >= m_Items.size() )
		newSelectedIndex = -1;

	m_SelItemIndex = newSelectedIndex;

	if( _OnItemSelectCallback )
		_OnItemSelectCallback( this );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::RemoveString()  Public
///
///	\param itemIndex The index of the item to remove
///
/// Remove an item from the scrollable list.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::RemoveString( uint32 itemIndex )
{
	// If the item to remove is the selected item then clear the selection index
	bool selectionWasChanged = itemIndex == (uint32)m_SelItemIndex;
	if( selectionWasChanged )
		m_SelItemIndex = -1;

	// Remove the item
	GUICtrlListBase::RemoveString( itemIndex );

	if( selectionWasChanged && _OnItemSelectCallback )
		_OnItemSelectCallback( this );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::SetDims()  Public
///
///	\param dims The new dimensions of the scroll list
///
/// Set the dimensions of the list box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::SetDims( Vector2i dims )
{
	m_Dims = dims;
	if( !m_Font.GetObj() )
	{
		m_NumVisItems = 1;
		return;
	}

	// Calculate the number of list items that are visible
	int32 itemHeight = m_Font.GetObj()->GetCharHeight() + (m_Font.GetObj()->GetCharHeight() / 2);
	m_NumVisItems = m_Dims.y / itemHeight;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::ReformatControl()  Public
///
/// Update the control based on its parameters so it displays correctly.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::ReformatControl()
{
	// Reformat the base class
	GUICtrlListBase::ReformatControl();

	if( !m_Font.GetObj() )
		return;

	// Calculate the number of list items that are visible
	int32 itemHeight = m_Font.GetObj()->GetCharHeight() + (m_Font.GetObj()->GetCharHeight() / 2);
	m_NumVisItems = m_Dims.y / itemHeight;

	if( m_NumVisItems > m_Items.size() )
		m_FirstVisIndex = 0;

	CacheVisStrings();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::GetItemFromPos()  Public
///
///	\param screenPos The test position in screen coordinates
///	\returns The index of the item at the position, -1 if the position is invalid
///
/// Get an item index from a position.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 GUICtrlScrollList::GetItemFromPos( Point2i screenPos ) const
{
	if( !m_Font.GetObj() )
		return -1;

	// Get the font height
	int32 fontHeight = m_Font->GetCharHeight();

	// Get the position in control coordinates
	Point2i controlPos( screenPos.x - m_Pos.x, screenPos.y - (m_Pos.y - (fontHeight / 4)) );

	// Ensure a valid position
	Box2i boundBox = InternalGetBoundBox();
	if( controlPos.x < 0 || controlPos.y < 0 || controlPos.x > boundBox.size.x || controlPos.y > boundBox.size.y )
		return -1;

	// Get the item index
	int32 itemHeight = fontHeight + (fontHeight / 2);
	int32 itemIndex = (int32)( controlPos.y / itemHeight );
	if( itemIndex < 0 || itemIndex >= (int32)m_Items.size() )
		return -1;

	// Return the calculated index
	return m_FirstVisIndex + itemIndex;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::SubclassTransferData()  Public
///
/// \param serializer The serializer object that is used to read or write data
///
///	Load/save the sub-class control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::SubclassTransferData( Serializer& serializer )
{
	GUICtrlListBase::SubclassTransferData( serializer );

	// If we are loading then clear the data
	if( serializer.InReadMode() )
		m_SelItemIndex = 0;

	// The dimensions
	serializer.AddData( m_Dims.x );
	serializer.AddData( m_Dims.y );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::CacheVisStrings()  Public
///
///	Recalculate the string positions after scrolling.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::CacheVisStrings()
{
	// Get the bounding box
	Box2i boundBox = InternalGetBoundBox();

	// Go through each list item
	int32 fontHeight = m_Font->GetCharHeight();
	int32 itemHeight = fontHeight + (fontHeight / 2);
	int curY = m_Pos.y;
	for( int32 strIndex = m_FirstVisIndex; strIndex < (int32)m_Items.size() && (strIndex - m_FirstVisIndex) < (m_NumVisItems + 1); ++strIndex )
	{
		// Get the item rectangle
		Box2i itemBox( m_Pos.x + 3, curY + 1, m_Dims.x - 2, itemHeight - 2 );
		itemBox.pos.y = curY + (fontHeight / 4);
		itemBox.size.y = fontHeight;
		
		// Clip the item rectangle if needed
		if( itemBox.Bottom() >= boundBox.Bottom() )
			itemBox.size.y -= itemBox.Bottom() - boundBox.Bottom();

		// Cache the drawn text
		m_Items[strIndex].cachedDraw = g_pGraphicsMgr->CacheFontText( m_Font.GetObj(), m_Items[strIndex].sText.c_str(), itemBox );

		// Step down an item
		curY += itemHeight;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::Draw()  Public
///
///	Draw the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::Draw() const
{
	if( !m_Font.GetObj() )
		return;

	// Get the bounding box
	Box2i boundBox = InternalGetBoundBox();

	// Draw the border
	g_pGraphicsMgr->DrawRect( boundBox, 0x00AAAAAA );

	// Go through each list item
	int32 fontHeight = m_Font->GetCharHeight();
	int32 itemHeight = fontHeight + (fontHeight / 2);
	int curY = m_Pos.y;
	for( uint32 strIndex = m_FirstVisIndex; strIndex < m_Items.size() && (strIndex - m_FirstVisIndex) < (m_NumVisItems + 1); ++strIndex )
	{
		int lineColor = 0x00CCCCCC;
		if( strIndex == (uint32)m_SelItemIndex )
			lineColor = 0x0000FF00;

		// Get the item rectangle
		Box2i itemBox( m_Pos.x + 1, curY + 1, m_Dims.x - 2, itemHeight - 2 );

		// Clip the item rectangle if needed
		if( itemBox.Bottom() >= boundBox.Bottom() )
			itemBox.size.y -= itemBox.Bottom() - boundBox.Bottom();

		// Draw the item rectangle
		Box2i drawBox( itemBox );
		g_pGraphicsMgr->DrawRect( drawBox, lineColor );
		drawBox.Inflate( -2 );
		g_pGraphicsMgr->DrawRect( drawBox, (strIndex == (uint32)m_SelItemIndex) ? 0 : lineColor );
		drawBox.Inflate( -2 );
		g_pGraphicsMgr->DrawRect( drawBox, lineColor );

		// Draw the text
		g_pGraphicsMgr->DrawCachedFontText( m_Items[strIndex].cachedDraw );

		// Step down an item
		curY += itemHeight;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::Scroll()  Public
///
///	Draw the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::Scroll( int32 offset )
{
	// Offset the index
	if( offset < 0 )
	{
		if( m_FirstVisIndex < (uint32)-offset )
			m_FirstVisIndex = 0;
		else
			m_FirstVisIndex -= (uint32)-offset;
	}
	else
		m_FirstVisIndex += (uint32)offset;

	// Clamp the value so the box doesn't scroll past the last item
	if( m_FirstVisIndex + m_NumVisItems >= (uint32)m_Items.size() )
	{
		if( m_NumVisItems > (uint32)m_Items.size() )
			m_FirstVisIndex = 0;
		else
			m_FirstVisIndex = (uint32)m_Items.size() - m_NumVisItems;
	}

	// Update the graphics
	CacheVisStrings();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::ScrollUpCallback()  Static Private
///
///	The callback for scroll up button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::ScrollUpCallback( GUICtrlButton* pBtn )
{
	// If there is no button or the parent is not set up
	if( !pBtn || !pBtn->GetParentCtrl() )
		return;
	GUICtrlScrollList* pScrollList = (GUICtrlScrollList*)pBtn->GetParentCtrl();

	pScrollList->Scroll( -1 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::ScrollDownCallback()  Static Private
///
///	The callback for scroll down button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::ScrollDownCallback( GUICtrlButton* pBtn )
{
	// If there is no button or the parent is not set up
	if( !pBtn || !pBtn->GetParentCtrl() )
		return;
	GUICtrlScrollList* pScrollList = (GUICtrlScrollList*)pBtn->GetParentCtrl();

	pScrollList->Scroll( 1 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlScrollList::SetScrollButtons()  Public
///
///	Setup the scroll buttons.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlScrollList::SetScrollButtons( GUICtrlButton* pUpBtn, GUICtrlButton* pDownBtn )
{
	// Setup the up button
	if( pUpBtn )
	{
		// Set the parent, the callback, and set the scroll button functionality
		pUpBtn->SetParentCtrl( this );
		pUpBtn->SetClickCallback( ScrollUpCallback );
		pUpBtn->SetScrollButton( true );
	}

	// Setup the down button
	if( pDownBtn )
	{
		// Set the parent, the callback, and set the scroll button functionality
		pDownBtn->SetParentCtrl( this );
		pDownBtn->SetClickCallback( ScrollDownCallback );
		pDownBtn->SetScrollButton( true );
	}
}