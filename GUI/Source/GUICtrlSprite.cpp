//=================================================================================================
/*!
	\file GUICtrlSprite.cpp
	2D Graphics Engine
	Graphical User Interface Sprite Control Header
	\author Taylor Clark
	\date June 12, 2006

	This source file contains the implementation for the GUI sprite control class.
*/
//=================================================================================================

#include "../GUICtrlSprite.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Resource/ResourceMgr.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlSprite::GUICtrlSprite()  Public
///
///	The default constructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlSprite::GUICtrlSprite() : m_Sprite( 0 )
{
#ifdef _DEBUG
	
	m_sName = L"Unnamed Sprite";
	GUICtrlProperty* pNewProp = new GUICtrlPropSprite( L"Sprite", &m_Sprite.m_RefSprite );
	m_PropList.push_back( pNewProp );
	
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlSprite::InternalGetBoundBox()  Public
///
///	\returns The rectangle the encloses the control
///
///	Get the rectangle the encloses the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Box2i GUICtrlSprite::InternalGetBoundBox() const
{
	Box2i retBox;
	retBox.pos = m_Pos;
	
	// If there is no sprite then bail
	if( !m_Sprite.GetSprite() )
		return retBox;

	// Get the image size
	retBox.size.x = m_Sprite.GetSprite()->GetSrcRect().size.x;
	retBox.size.y = m_Sprite.GetSprite()->GetSrcRect().size.y;

	// Return the box
	return retBox;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlSprite::SubclassTransferData()  Public
///
/// \param serializer The serializer object that is used to read or write data
///
///	Load/save the sub-class control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlSprite::SubclassTransferData( Serializer& serializer )
{
	// The normal sprite
	ResourceID resID = 0;
	if( m_Sprite.GetSprite() )
		resID = m_Sprite.GetSprite()->GetResID();
	serializer.AddData( resID );

	// If we are loading then load the resource
	if( serializer.InReadMode() )
	{
		m_Sprite = AnimSprite( ResourceMgr::Get().GetRefSprite( resID ) );
		m_Sprite.Play();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlSprite::Update()  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlSprite::Update( float32 frameTime )
{
	m_Sprite.Update( frameTime );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlSprite::Draw()  Public
///
///	Draw the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlSprite::Draw() const
{
	// Draw the sprite
	g_pGraphicsMgr->DrawImage( m_Pos, m_Sprite.GetImage(), m_Sprite.GetSrcCoords() );
}