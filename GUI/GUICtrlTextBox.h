//=================================================================================================
/*!
	\file GUICtrlTextBox.h
	GUI Library
	Graphical User Interface Text Box Control Header
	\author Taylor Clark
	\date February 2, 2006

	This file contains the definition for the GUI text box control class.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlTextBox_h
#define __GUICtrlTextBox_h

#include "GUIControl.h"
#include <string>
#include "Graphics2D/TCFont.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlTextBox
	\brief The class that represents a edittable text box in a GUI layout

	This class defines the GUI text box control.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlTextBox : public GUIControl
{
public:

	/// The type of input that this text box allows
	enum InputFilter
	{
		IF_None = 0,
		IF_PositiveWholeNumbers,
		IF_Numbers,
		IF_COUNT
	};

	/// The callback function prototype for when a button is pressed
	typedef void (*OnTextEditCallback)( GUICtrlTextBox* );

private:

	/// The text to display
	std::wstring m_Text;

	/// The dimensions of the box
	Vector2i m_Dims;

	/// The font to render text with
	TCFontHndl m_Font;

	/// The index of the character which the caret is in front of
	uint32 m_CaretPos;

	/// The index within the string of the first visible character
	uint32 m_FirstVisCharIndex;

	/// The number of characters this text box can fully display
	uint32 m_NumVisChars;

	/// The maximum number of characters allowed in this text box
	uint32 m_MaxLen;

	/// The current frame time counter used to blink the cursor, mutable since it is updated
	/// in the draw function
	mutable float m_CursorVisTime;

	/// The input filtering type of the text box
	InputFilter m_InputFilter;

	/// The callback function for the text changing
	OnTextEditCallback m_pTextEditCallback;


	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const;

	/// Get the offset from the left of the caret
	int32 GetCaretXOffset() const;

	/// Check if a character is valid for input to this text box
	bool IsCharValid( wchar_t inputChar );

public:

	/// The default constructor
	GUICtrlTextBox();

	/// Draw the control
	virtual void Draw() const;

	/// Handle a key being pressed down
	virtual bool OnKeyDown( unsigned int windowsKeyCode );
	
	/// Handle a keyboard character pressed and released
	virtual bool OnKeyChar( wchar_t charKey );

	/// Load/save the control data
	virtual void SubclassTransferData( Serializer& serializer );

	/// Can the user interact with this control
	virtual bool IsInteractive() const { return true; }

	/// Get the text
	const std::wstring& GetText() const { return m_Text; }

	/// Set the text
	void SetText( const wchar_t* szText, bool callCallback = false );

	/// Set the callback function for when the text changes
	void SetCallback( OnTextEditCallback pCallbackFunc )
	{
		m_pTextEditCallback = pCallbackFunc;
	}

	/// Set the input filter type
	void SetInputFilter( InputFilter filterType );

	/// Set the maximum length
	void SetMaxAllowedLength( int32 maxLen );

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType()
	{
		return GUIDefines::CT_TextBox;
	}

	/// Get if this control can receive focus
	virtual bool CanReceiveFocus() const { return true; }

	/// Set the data
	void SetData( const wchar_t* szName, TCFontHndl font, Box2i boundBox )
	{
		m_sName = szName;

		m_Font = font;

		m_Dims = boundBox.size;
		m_Pos = boundBox.pos;
	}

	/// Update the control based on its parameters so it displays correctly
	virtual void ReformatControl();
};

#endif // __GUICtrlTextBox_h