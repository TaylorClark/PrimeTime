//=================================================================================================
/*!
	\file GUICtrlButton.cpp
	2D Graphics Engine
	Button Control Source
	\author Taylor Clark

	This source file contains the implementation for the GUI button control class.
*/
//=================================================================================================

#include "../GUICtrlButton.h"
#include "Resource/ResourceMgr.h"
#include "Graphics2D/GraphicsMgr.h"
#include "../GUIMgr.h"
#include "Graphics2D/TCFont.h"

/// The time between auto-callbacks for scroll buttons
const float32 SCROLL_CB_DELAY = 0.1875f;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUICtrlButton::GUICtrlButton()  Public
///
///	The default constructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlButton::GUICtrlButton() : m_IsDepressed( false ),
					m_CursorPressedDown( false ),
					m_pOnPressCallback( 0 ),
					m_pOnButtonDownCallback( 0 ),
					m_pOnButtonUpCallback( 0 ),
					m_Font( NULL ),
					m_NormalImage( 0 ),
					m_DepressedImage( 0 ),
					m_MouseIsOver( false ),
					m_pParentCtrl( 0 ),
					m_IsScrollButton( false ),
					m_ScrollCallbackTimer( 0.0f )
{
#ifdef _DEBUG
	
	m_sName = L"Unnamed Button";
	
	GUICtrlProperty* pNewProp = new GUICtrlPropString( L"Text", &m_Text );
	m_PropList.push_back( pNewProp );

	// Add the font property
	pNewProp = new GUICtrlPropFont( L"Font", &m_Font );
	m_PropList.push_back( pNewProp );

	// Add the normal sprite property
	pNewProp = new GUICtrlPropSprite( L"Normal Sprite", &m_NormalImage );
	m_PropList.push_back( pNewProp );

	// Add the mouse over property
	pNewProp = new GUICtrlPropSprite( L"Mouse Over Sprite", &m_OverImage );
	m_PropList.push_back( pNewProp );

	// Add the depressed sprite property
	pNewProp = new GUICtrlPropSprite( L"Pressed Sprite", &m_DepressedImage );
	m_PropList.push_back( pNewProp );	
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlButton::Draw()  Public
///
///	Draw the button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlButton::Draw() const
{
	// If the button is depressed
    bool isTextOnly = false;
	if( m_IsDepressed && m_DepressedImage.GetObj() )
		g_pGraphicsMgr->DrawSprite( m_Pos, m_DepressedImage.GetObj() );
	else if( m_MouseIsOver && m_OverImage.GetObj() )
		g_pGraphicsMgr->DrawSprite( m_Pos, m_OverImage.GetObj() );
	else if( m_NormalImage.GetObj() )
		g_pGraphicsMgr->DrawSprite( m_Pos, m_NormalImage.GetObj() );
    // Otherwise this is a text-only button
    else
        isTextOnly = true;

	// If we have a font to draw text
	//if( m_Font.GetObj() )
	//{
	//	// Center the text on the button
	//	Box2i boundBox = GetBoundBox();
	//	Point2i textPos( m_Pos );
	//	textPos.y += (boundBox.size.y - m_Font->GetCharHeight()) / 2;
	//	int strWidth = m_Font->CalcStringWidth( m_Text.c_str() );
	//	textPos.x += (boundBox.size.x - strWidth) / 2;

	//	// If the button is down the move it a bit
	//	if( m_IsDepressed )
	//	{
	//		textPos.x += 3;
	//		textPos.y += 3;
	//	}

	//	// Draw the text
	//	g_pGraphicsMgr->DrawFontText( m_Font.GetObj(), m_Text.c_str(), m_Pos );
	//}

    if( isTextOnly )
    {
        if( m_Font.GetObj() )
        {
            //g_pGraphicsMgr->DrawCachedFontText( _cachedText, 0xff0000ff );
            g_pGraphicsMgr->DrawFontText( m_Font.GetObj(), m_Text.c_str(), m_Pos, 0xFF2222FF );

            Box2i underlineRect( m_Pos.x, m_Pos.y + m_Font.GetObj()->GetCharHeight() + 1, m_Font.GetObj()->CalcStringWidth( m_Text.c_str() ), 2 );
            g_pGraphicsMgr->FillRect( underlineRect, 0xff0000ff );
        }
    }
    else
	    g_pGraphicsMgr->DrawCachedFontText( _cachedText );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUICtrlButton::UpdateCachedFont  Protected
///
/// Get the bounding box for this control
///////////////////////////////////////////////////////////////////////////////////////////////////
Box2i GUICtrlButton::InternalGetBoundBox() const
{
	// Store the bounding box
	Box2i retBox;
	if( m_IsDepressed && m_DepressedImage.GetObj() )
		retBox.size = m_DepressedImage.GetObj()->GetSrcRect().size;
	else if( !m_IsDepressed && m_NormalImage.GetObj() )
		retBox.size = m_NormalImage.GetObj()->GetSrcRect().size;
    else
    {
        retBox.size.x = m_Font->CalcStringWidth( m_Text.c_str() );
        retBox.size.y = m_Font->GetCharHeight();
    }

	retBox.pos = m_Pos;

	// Return the bounding box
	return retBox;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUICtrlButton::UpdateCachedFont  Private
///
///	Update the cached font data.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlButton::UpdateCachedFont()
{
	if( !m_Font.GetObj() )
		return;

	// Center the text on the button
	Box2i boundBox = GetBoundBox();
	Point2i textPos( m_Pos );
	textPos.y += (boundBox.size.y - m_Font->GetCharHeight()) / 2;
	int strWidth = m_Font->CalcStringWidth( m_Text.c_str() );
	textPos.x += (boundBox.size.x - strWidth) / 2;

	Box2i destRect( textPos, Vector2i(g_pGraphicsMgr->GetDisplayDims()) );

	// Get the cached font data
	_cachedText = g_pGraphicsMgr->CacheFontText( m_Font.GetObj(), m_Text.c_str(), destRect );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUICtrlButton::SetIsDepressed()  Private
/// \param newDepressedValue The new state of the button
///
///	Set the state indicating if the button should look up or down.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlButton::SetIsDepressed( bool newDepressedValue )
{
	// If the state isn't changing then do nothing
	if( m_IsDepressed == newDepressedValue )
		return;

	// If the button is now being depressed
	if( newDepressedValue )
		_cachedText.OffsetCharacters( Vector2i(3,3) );
	// Else the button is being released
	else
		_cachedText.OffsetCharacters( Vector2i(-3,-3) );

	// Store the new value
	m_IsDepressed = newDepressedValue;

	// If there is a down handler then call it
	if( m_IsDepressed && m_pOnButtonDownCallback )
		m_pOnButtonDownCallback( this );
	// Or if there is an up handler
	else if( !m_IsDepressed && m_pOnButtonUpCallback )
		m_pOnButtonUpCallback( this );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlButton::SubclassTransferData()  Public
///
/// \param serializer The serializer object that is used to read or write data
///
///	Load/save the sub-class control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlButton::SubclassTransferData( Serializer& serializer )
{
	// Clear the data if we are loading
	if( serializer.InReadMode() )
	{
		m_CursorPressedDown = false;
		m_IsDepressed = false;
		m_pOnPressCallback = NULL;
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

	// The depressed sprite
	resID = 0;
	if( m_DepressedImage.GetObj() )
		resID = m_DepressedImage->GetResID();
	serializer.AddData( resID );

	// If we are loading then load the sprite
	if( serializer.InReadMode() )
		m_DepressedImage = ResourceMgr::Get().GetRefSprite( resID );

	// The font
	resID = 0;
	if( m_Font.GetObj() )
		resID = m_Font->GetResID();
	serializer.AddData( resID );

	// If we are loading then load the font
	if( serializer.InReadMode() )
		m_Font = ResourceMgr::Get().GetTCFont( resID );

	// The text
	serializer.AddData( m_Text );

	if( serializer.InReadMode() )
		UpdateCachedFont();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlButton::Update()  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlButton::Update( float32 frameTime )
{
	// If the button is not a scroll button or it is not down, or the mouse it not over
	if( !m_IsScrollButton || !m_IsDepressed || !m_CursorPressedDown )
		return;

	// Update the callback timer
	m_ScrollCallbackTimer += frameTime;

	// If it is time to execute the callback
	if( m_ScrollCallbackTimer > SCROLL_CB_DELAY )
	{
		m_ScrollCallbackTimer = 0.0f;

		// Call the callback if there is one
		if( m_pOnPressCallback )
			m_pOnPressCallback( this );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlButton::OnMouseDown()  Public
///
///	\param btn Which mouse button was pressed
///	\param UNNAMED The position, in client coordinates, of the mouse when the mouse button was
///					pressed
///	\returns True if the message was handled, false if it should be passed on to the layout
///
///	Handle a mouse button being pressed.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlButton::OnMouseDown( GUIDefines::EMouseButtons btn, Point2i )
{
	// We only handle the left mouse button
	if( btn != GUIDefines::MB_Left )
		return true;

	// Set button to be pressed
	m_CursorPressedDown = true;
	SetIsDepressed( true );

	// If this is a scroll button
	if( m_IsScrollButton )
	{
		// Clear the timer
		m_ScrollCallbackTimer = -SCROLL_CB_DELAY;

		// Call the callback
		if( m_pOnPressCallback )
			m_pOnPressCallback( this );
	}

	// We handled the message
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlButton::OnMouseUp()  Public
///
///	\param btn Which mouse button was released
///	\param UNNAMED The position, in client coordinates, of the mouse when the mouse button was
///					released
///	\returns True if the message was handled, false if it should be passed on to the layout
///
///	Handle a mouse button being released.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlButton::OnMouseUp( GUIDefines::EMouseButtons btn, Point2i )
{
	// We only handle the left mouse button
	if( btn != GUIDefines::MB_Left )
		return true;

	// If the cursor was pressed down on top of the button, the button has a callback
	// function, and it is not a scroll button then call it because the button has been
	// legitamately pressed
	if( m_CursorPressedDown && m_pOnPressCallback && !m_IsScrollButton )
		m_pOnPressCallback( this );

	// The mouse is released
	m_CursorPressedDown = false;
	SetIsDepressed( false );

	// We handled the message
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlButton::OnMouseUpNotOver()  Public
///
///	\param UNNAMED The mouse button released
///	\param UNNAMED The position of the mouse cursor when this message was created
///	\returns True if the message was handled, false if it should be passed on to the layout
///
///	Handle a mouse button being released after a mouse down message, but the cursor is not over
/// the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlButton::OnMouseUpNotOver( GUIDefines::EMouseButtons, Point2i )
{
	// The mouse is released
	m_CursorPressedDown = false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlButton::OnMouseEnter()  Public
///
///	\returns True if the message was handled, false if it should be passed on to the layout
///
///	Handle the mouse cursor entering this control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlButton::OnMouseEnter()
{
	m_MouseIsOver = true;

	// If the left mouse button is no longer held down then clear the flag. This can happen if the
	// player clicks down in the game window, then moves the cursor outside of the window, then
	// releases the button, and brings the cursor back into the window
	if( !GUIMgr::Get().IsMouseDown( GUIDefines::MB_Left ) )
		m_CursorPressedDown = false;

	// If the cursor was pressed down over the button then show that it is pressed if brought back
	// over the button
	if( m_CursorPressedDown )
		SetIsDepressed( true );

	// We handled the message
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlButton::OnMouseLeave()  Public
///
///	\returns True if the message was handled, false if it should be passed on to the layout
///
///	Handle the mouse cursor leaving this control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlButton::OnMouseLeave()
{
	m_MouseIsOver = false;

	// If the mouse cursor leaves the button it can't be down
	SetIsDepressed( false );
	
	// We handled the message
	return true;
}