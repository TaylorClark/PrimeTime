//=================================================================================================
/*!
	\file GUICtrlListBase.cpp
	2D Graphics Engine
	Graphical User Interface List Control Base Sourc
	\author Taylor Clark
	\date August 15, 2006

	This source file contains the functions for the GUI list control base class.
*/
//=================================================================================================

#include "../GUICtrlListBase.h"
#include "Resource/ResourceMgr.h"
#include "Graphics2D/GraphicsMgr.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::GUICtrlListBase  Public
///
///	The default constructor
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlListBase::GUICtrlListBase() : m_Font( NULL ),
								m_MouseIsWithin( false )
{
#ifdef _DEBUG
	
	GUICtrlProperty* pNewProp = new GUICtrlPropStringList( L"Items", &m_Items );
	m_PropList.push_back( pNewProp );

	// Add the font property
	pNewProp = new GUICtrlPropFont( L"Font", &m_Font );
	m_PropList.push_back( pNewProp );

	// Add the selected font property
	pNewProp = new GUICtrlPropFont( L"Selected Font", &m_SelectedFont );
	m_PropList.push_back( pNewProp );
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::SubclassTransferData  Public
///
/// \param serializer The serializer object that is used to read or write data
///
///	Load/save the sub-class control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlListBase::SubclassTransferData( Serializer& serializer )
{
	// The number of list items
	uint32 numListItems = (uint32)m_Items.size();
	serializer.AddData( numListItems );
	
	// If we are loading then resize the vector
	if( serializer.InReadMode() )
		m_Items.resize( numListItems );

	// Go through each list item
	for( uint32 listItemIndex = 0; listItemIndex < numListItems; ++listItemIndex )
		serializer.AddData( m_Items[listItemIndex].sText );

	// The font
	ResourceID fontResID = 0;
	if( m_Font.GetObj() )
		fontResID = m_Font->GetResID();
	serializer.AddData( fontResID );

	// The selected font
	ResourceID selFontResID = 0;
	if( m_SelectedFont.GetObj() )
		selFontResID = m_SelectedFont->GetResID();
	serializer.AddData( selFontResID );

	// If we are loading then load the fonts
	if( serializer.InReadMode() )
	{
		m_Font = ResourceMgr::Get().GetTCFont( fontResID );
		m_SelectedFont = ResourceMgr::Get().GetTCFont( selFontResID );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::IndexOfString  Public
///	\param szString The string for which to search
///	\returns The index of the string in the list or -1 if it's not found
///
/// Find the index of a string in the list
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 GUICtrlListBase::IndexOfString( const wchar_t* szString )
{
	for( uint32 itemIndex = 0; itemIndex < m_Items.size(); ++itemIndex )
	{
		if( m_Items[itemIndex].sText == szString )
			return itemIndex;
	}

	// The string wasn't found
	return -1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::ReformatControl  Public
///
///	Update the control based on its parameters so it displays correctly.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlListBase::ReformatControl()
{
	for( uint32 itemIndex = 0; itemIndex < m_Items.size(); ++itemIndex )
		CacheListItem( itemIndex );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::AddString  Public
///
///	\param szStr The null-terminated string to add
///	\returns The index at which the item was added
///
///	Add a string to the list.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
uint32 GUICtrlListBase::AddString( const wchar_t* szStr )
{
	if( !szStr )
		return 0;
	GUIListItem newItem;
	newItem.sText = szStr;
	m_Items.push_back( newItem );

	// Cache the new item
	CacheListItem( (uint32)m_Items.size() - 1 );

	return (uint32)( m_Items.size() - 1 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::CacheListItem  Private
///
///	\param insertIndex The zero-based index of the item to cache
///
///	Cache a list item so it can be drawn faster.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlListBase::CacheListItem( uint32 itemIndex )
{
	if( itemIndex >= m_Items.size() )
		return;

	m_Items[itemIndex].cachedDraw = g_pGraphicsMgr->CacheFontText( m_Font.GetObj(), m_Items[itemIndex].sText.c_str(), Box2i( Point2i(0,0), g_pGraphicsMgr->GetDisplayDims() ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::InsertString  Public
///
///	\param szStr The null-terminated string to add
///	\param insertIndex The zero-based index at which to insert the item
///
///	Insert a string into the list.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlListBase::InsertString( const wchar_t* szStr, uint32 insertIndex )
{
	// If the insert index is too great then cap it
	if( insertIndex > m_Items.size() )
		insertIndex = (uint32)m_Items.size();

	// Increase the size by 1
	m_Items.resize( m_Items.size() + 1 );

	// Starting from the back, shift the items back one at a time up until the insert index
	for( uint32 curIndex = (uint32)m_Items.size() - 1; curIndex > insertIndex; --curIndex )
		m_Items[curIndex] = m_Items[curIndex - 1];

	// Store the new string
	GUIListItem newItem;
	newItem.sText = szStr;
	m_Items[insertIndex] = newItem;

	// Cache the new item
	CacheListItem( insertIndex );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::RemoveString  Public
///
///	\param itemIndex The zero-based index of the item to remove
///
///	Remove an item from the list.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlListBase::RemoveString( uint32 itemIndex )
{
	// If the index is invalid then bail
	if( itemIndex >= m_Items.size() )
		return;

	// Starting from the index, shift the items forward one at a time up until the insert index
	for( uint32 curIndex = itemIndex; curIndex < (uint32)(m_Items.size() - 1); ++curIndex )
		m_Items[curIndex] = m_Items[curIndex + 1];

	// Resize
	m_Items.resize( m_Items.size() - 1 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlListBase::Clear  Public
///
///	Remove all items.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlListBase::Clear()
{
	m_Items.clear();
}