//=================================================================================================
/*!
	\file GUICtrlLabel.cpp
	GUI Library
	Graphical User Interface Label Control Header
	\author Taylor Clark
	\date February 2, 2006

	This source file contains the implementation for the GUI label control class.
*/
//=================================================================================================

#include "../GUICtrlLabel.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Resource/ResourceMgr.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlLabel::GUICtrlLabel  Public
///
///	The default constructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlLabel::GUICtrlLabel() : m_Font( NULL ),
								m_FontScale( 1.0f )
{
#ifdef _DEBUG
	
	// Add the name
	m_sName = L"Unnamed Label";
	GUICtrlProperty* pNewProp = new GUICtrlPropString( L"Text", &m_Text );
	m_PropList.push_back( pNewProp );

	// Add the font property
	pNewProp = new GUICtrlPropFont( L"Font", &m_Font );
	m_PropList.push_back( pNewProp );
	
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlLabel::InternalGetBoundBox  Public
///
///	\returns The rectangle the encloses the control
///
///	Get the rectangle the encloses the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Box2i GUICtrlLabel::InternalGetBoundBox() const
{
	Box2i retBox;
	retBox.pos = m_Pos;
	
	// If there is no font then bail
	if( !m_Font.GetObj() )
		return retBox;

	// Get the font height
	retBox.size = m_DrawnText.GetSize();

	// Return the box
	return retBox;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlLabel::SubclassTransferData  Public
///
/// \param serializer The serializer object that is used to read or write data
///
///	Load/save the sub-class control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlLabel::SubclassTransferData( Serializer& serializer )
{
	// The text
	serializer.AddData( m_Text );

	// The font
	ResourceID resID = 0;
	if( m_Font.GetObj() )
		resID = m_Font->GetResID();
	serializer.AddData( resID );

	// If we are loading then load the sprite
	if( serializer.InReadMode() )
		m_Font = ResourceMgr::Get().GetTCFont( resID );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlLabel::UpdateCachedFont  Private
///
/// \param szText The text to display in the label
///
///	Update the cached font data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlLabel::UpdateCachedFont()
{
	Box2i destRect( m_Pos, Vector2i(g_pGraphicsMgr->GetDisplayDims()) );

	// Get the cached font data
	m_DrawnText = g_pGraphicsMgr->CacheFontText( m_Font.GetObj(), m_Text.c_str(), destRect, m_FontScale );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlLabel::Draw  Public
///
///	Draw the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlLabel::Draw() const
{
	// Draw the text
	g_pGraphicsMgr->DrawCachedFontText( m_DrawnText );
}