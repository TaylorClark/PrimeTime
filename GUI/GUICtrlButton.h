//=================================================================================================
/*!
	\file GUICtrlButton.h
	2D Game Engine
	Graphical User Interface Button Control Header
	\author Taylor Clark
	\date February 2, 2006

	This file contains the definition for the GUI button control class.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlButton_h
#define __GUICtrlButton_h

#include "Graphics2D/RefSprite.h"
#include "Graphics2D/TCFont.h"
#include "GUIControl.h"
#include <string>
#include "Graphics2D/CachedFontDraw.h"

class GUICtrlButton;


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlButton
	\brief The class that represents a button in a GUI layout

	This class defines the GUI button control.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlButton : public GUIControl
{
public:
	/// The callback function prototype for when a button is pressed
	typedef void (*OnButtonEventCallback)( GUICtrlButton* );

private:

	/// The parent control that is linked to this button
	GUIControl* m_pParentCtrl;
 
	/// The image to use normal
	RefSpriteHndl m_NormalImage;

	/// The image to use when the mouse is over the button
	RefSpriteHndl m_OverImage;

	/// The image to use when pressed down
	RefSpriteHndl m_DepressedImage;

	/// The string and font to display text on the button
	TCFontHndl m_Font;
	std::wstring m_Text;

	/// The cached text
	CachedFontDraw _cachedText;
	
	/// If the cursor was pressed down over the button
	bool m_CursorPressedDown;

	/// If the button is currently down
	bool m_IsDepressed;

	/// Whether or not the mouse is over the button
	bool m_MouseIsOver;

	/// If this button uses scroll button functionality rather than normal functionality meaning
	/// that it will repeatedly call the click handler if held down
	bool m_IsScrollButton;

	/// The timer for auto-calling the callback for scroll button functionality
	float32 m_ScrollCallbackTimer;

	/// The callback function for when the button is clicked
	OnButtonEventCallback m_pOnPressCallback;

	/// The callback function for when the button changes to look like it is down
	OnButtonEventCallback m_pOnButtonDownCallback;

	/// The callback function for when the button changes to look like it is up
	OnButtonEventCallback m_pOnButtonUpCallback;

	/// Update the cached font data
	void UpdateCachedFont();

	void SetIsDepressed( bool newDepressedValue );

	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const;

public:

	/// The default constructor
	GUICtrlButton();

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType()
	{
		return GUIDefines::CT_Button;
	}

	/// Load/save the control data
	virtual void SubclassTransferData( Serializer& serializer );

	/// Set the callback function for when the button is pressed
	void SetClickCallback( OnButtonEventCallback pCallbackFunc )
	{
		m_pOnPressCallback = pCallbackFunc;
	}

	/// Set the callback function for when the button is pressed
	void SetButtonDownCallback( OnButtonEventCallback pCallbackFunc )
	{
		m_pOnButtonDownCallback = pCallbackFunc;
	}

	/// Set the callback function for when the button is pressed
	void SetButtonUpCallback( OnButtonEventCallback pCallbackFunc )
	{
		m_pOnButtonUpCallback = pCallbackFunc;
	}

	/// Handle a mouse button being pressed
	virtual bool OnMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos );

	/// Handle a mouse button being released
	virtual bool OnMouseUp( GUIDefines::EMouseButtons btn, Point2i mousePos );

	/// Handle a mouse button being released after a mouse down message, but the cursor not over
	/// the control
	virtual bool OnMouseUpNotOver( GUIDefines::EMouseButtons btn, Point2i mousePos );

	/// Handle the mouse cursor entering this control
	virtual bool OnMouseEnter();

	/// Handle the mouse cursor leaving this control
	virtual bool OnMouseLeave();

	/// Draw the control
	virtual void Draw() const;

	/// Force a call to the callback function
	void ExecuteCallback()
	{
		if( m_pOnPressCallback )
			m_pOnPressCallback( this );
	}

	/// Can the user interact with this control
	virtual bool IsInteractive() const { return true; }

	/// Set the button text
	void SetText( const wchar_t* szText )
	{
		m_Text = szText;
		UpdateCachedFont();
	}

	/// Set the button data
	void SetData( const wchar_t* szName, RefSpriteHndl upSprite, RefSpriteHndl overSprite, RefSpriteHndl downSprite, const wchar_t* szText, TCFontHndl font )
	{
		m_sName = szName;

		m_NormalImage = upSprite;
		m_OverImage = overSprite;
		m_DepressedImage = downSprite;

		m_Text = szText;
		m_Font = font;

		UpdateCachedFont();
	}

	/// Set the position
	virtual void SetPos( const Point2i& pos )
	{
		GUIControl::SetPos( pos );

		UpdateCachedFont();
	}

	/// Get the button text
	std::wstring GetText(){ return m_Text; }
	
	/// Get the idle sprite
	RefSpriteHndl GetIdleSprite(){ return m_NormalImage; }

	/// Get the image to use when the mouse is over the button
	RefSpriteHndl GetOverSprite(){ return m_OverImage; }

	/// Get the image to use when pressed down
	RefSpriteHndl GetPressedSprite(){ return m_DepressedImage; }

	/// Update the control
	virtual void Update( float32 frameTime );

	/// Set the idle sprite
	void SetIdleSprite( RefSpriteHndl newSprite ){ m_NormalImage = newSprite; }

	/// Set the image to use when the mouse is over the button
	void SetOverSprite( RefSpriteHndl newSprite ){ m_OverImage = newSprite; }

	/// Set the image to use when pressed down
	void SetPressedSprite( RefSpriteHndl newSprite ){ m_DepressedImage = newSprite; }

	/// Set the parent control
	void SetParentCtrl( GUIControl* pCtrl ){ m_pParentCtrl = pCtrl; }

	/// Get the parent control
	GUIControl* GetParentCtrl(){ return m_pParentCtrl; }

	/// Set the scroll button functionality
	void SetScrollButton( bool isScrollButton ){ m_IsScrollButton = isScrollButton; }
};

#endif // __GUICtrlButton_h