//=================================================================================================
/*!
	\file GUICtrlLabel.h
	2D Game Engine
	Graphical User Interface Label Control Header
	\author Taylor Clark
	\date February 2, 2006

	This file contains the definition for the GUI label control class.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlLabel_h
#define __GUICtrlLabel_h

#include "GUIControl.h"
#include <string>
#include "Graphics2D/TCFont.h"
#include "Graphics2D/CachedFontDraw.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlLabel
	\brief The class that represents a text label in a GUI layout

	This class defines the GUI button control.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlLabel : public GUIControl
{
private:

	/// The text to display
	std::wstring m_Text;

	/// The font to render text with
	TCFontHndl m_Font;

	/// The cached text
	CachedFontDraw m_DrawnText;

	float32 m_FontScale;


	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const;

	/// Update the cached font data
	void UpdateCachedFont();

public:

	/// The default constructor
	GUICtrlLabel();

	/// The destructor, virtual to ensure derived classes are freed properly
	virtual ~GUICtrlLabel() {}

	/// Draw the control
	virtual void Draw() const;

	/// Load/save the control data
	virtual void SubclassTransferData( Serializer& serializer );

	/// Can the user interact with this control
	virtual bool IsInteractive() const { return false; }

	/// Update the control based on its parameters so it displays correctly
	virtual void ReformatControl()
	{
		UpdateCachedFont();
	}

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType()
	{
		return GUIDefines::CT_Label;
	}

	/// Set the label text
	void SetText( const wchar_t* szText )
	{
		m_Text = szText;
		UpdateCachedFont();
	}

	/// Set the label text
	void SetFont( TCFontHndl font )
	{
		m_Font = font;
		UpdateCachedFont();
	}

	/// Set the font scale
	void SetFontScale( float32 fontScale )
	{
		m_FontScale = fontScale;
		UpdateCachedFont();
	}

	/// Set the label text and font
	void SetData( const wchar_t* szText, TCFontHndl font )
	{
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

	/// Get the font
	const TCFont* GetFont() const { return m_Font.GetObj(); }

	/// Get the message box text
	const wchar_t* GetText() const { return m_Text.c_str(); }

	/// Get the message box text in a string object
	const std::wstring& GetTextStr() const { return m_Text; }
};

#endif // __GUICtrlLabel_h