//=================================================================================================
/*!
	\file GUICtrlCheckBox.cpp
	GUI Library
	Check Box Control Source
	\author Taylor Clark
	\date September 7, 2006

	This source file contains the implementation for the GUI check box control class.
*/
//=================================================================================================

#include "../GUICtrlCheckBox.h"
#include "Resource/ResourceMgr.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Graphics2D/TCFont.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUICtrlCheckBox::GUICtrlCheckBox()  Public
///
///	The default constructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlCheckBox::GUICtrlCheckBox() : m_pOnCheckedChangedCB( 0 ),
					m_MouseIsOver( false ),
					m_IsChecked( false )
{
#ifdef _DEBUG
	
	m_sName = L"Unnamed Check Box";
	
	// Add the normal sprite property
	GUICtrlProperty* pNewProp = new GUICtrlPropSprite( L"Idle Sprite", &m_NormalImage );
	m_PropList.push_back( pNewProp );

	// Add the mouse over property
	pNewProp = new GUICtrlPropSprite( L"Mouse Over Sprite", &m_OverImage );
	m_PropList.push_back( pNewProp );

	// Add the check mark sprite property
	pNewProp = new GUICtrlPropSprite( L"Check Mark Sprite", &m_CheckMarkImage );
	m_PropList.push_back( pNewProp );	
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlCheckBox::Draw()  Public
///
///	Draw the button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlCheckBox::Draw() const
{
	if( m_MouseIsOver && m_OverImage.GetObj() )
		g_pGraphicsMgr->DrawSprite( m_Pos, m_OverImage.GetObj() );
	else if( m_NormalImage.GetObj() )
		g_pGraphicsMgr->DrawSprite( m_Pos, m_NormalImage.GetObj() );

	// If the box is checked and there is a check mark image
	if( m_IsChecked && m_CheckMarkImage.GetObj() )
		g_pGraphicsMgr->DrawSprite( m_Pos + m_CheckedImageOffset, m_CheckMarkImage.GetObj() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlCheckBox::CalcCheckSpriteOffset()  Private
///
///	Calculate the check mark sprite offset.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlCheckBox::CalcCheckSpriteOffset()
{
	// If the sprites are empty then bail
	if( !m_CheckMarkImage.GetObj() || !m_NormalImage.GetObj() )
	{
		m_CheckedImageOffset.Set( 0,0 );
		return;
	}

	// Center the check mark over the box
	m_CheckedImageOffset = (m_NormalImage->GetSrcRect().size - m_CheckMarkImage->GetSrcRect().size) * 0.5f;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlCheckBox::SubclassTransferData()  Public
///
/// \param serializer The serializer object that is used to read or write data
///
///	Load/save the sub-class control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlCheckBox::SubclassTransferData( Serializer& serializer )
{
	// Clear the data if we are loading
	if( serializer.InReadMode() )
	{
		m_IsChecked = false;
		m_MouseIsOver = false;
		m_pOnCheckedChangedCB = NULL;
	}

	// The normal sprite
	ResourceID resID = 0;
	if( m_NormalImage.GetObj() )
		resID = m_NormalImage->GetResID();
	serializer.AddData( resID );

	// If we are loading then load the sprite
	if( serializer.InReadMode() )
		m_NormalImage = ResourceMgr::Get().GetRefSprite( resID );

	// The mouse-over sprite
	resID = 0;
	if( m_OverImage.GetObj() )
		resID = m_OverImage->GetResID();
	serializer.AddData( resID );

	// If we are loading then load the sprite
	if( serializer.InReadMode() )
		m_OverImage = ResourceMgr::Get().GetRefSprite( resID );

	// The check mark sprite
	resID = 0;
	if( m_CheckMarkImage.GetObj() )
		resID = m_CheckMarkImage->GetResID();
	serializer.AddData( resID );

	// If we are loading then load the sprite
	if( serializer.InReadMode() )
		m_CheckMarkImage = ResourceMgr::Get().GetRefSprite( resID );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlCheckBox::OnMouseDown()  Public
///
///	\param btn Which mouse button was pressed
///	\param UNNAMED The position, in client coordinates, of the mouse when the mouse button was
///					pressed
///	\returns True if the message was handled, false if it should be passed on to the layout
///
///	Handle a mouse button being pressed.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlCheckBox::OnMouseDown( GUIDefines::EMouseButtons btn, Point2i )
{
	// We only handle the left mouse button
	if( btn != GUIDefines::MB_Left )
		return true;

	// Toggle the checked state and execute the callback
	m_IsChecked = !m_IsChecked;
	if( m_pOnCheckedChangedCB )
		m_pOnCheckedChangedCB( this );

	// We handled the message
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlCheckBox::OnMouseEnter()  Public
///
///	\returns True if the message was handled, false if it should be passed on to the layout
///
///	Handle the mouse cursor entering this control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlCheckBox::OnMouseEnter()
{
	m_MouseIsOver = true;

	// The message has been handled
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlCheckBox::OnMouseLeave()  Public
///
///	\returns True if the message was handled, false if it should be passed on to the layout
///
///	Handle the mouse cursor leaving this control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlCheckBox::OnMouseLeave()
{
	m_MouseIsOver = false;
	
	// The message has been handled
	return true;
}