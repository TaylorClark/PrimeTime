//=================================================================================================
/*!
	\file GUILayout.h
	GUI Library
	Graphical User Interface Message Box Header
	\author Taylor Clark
	\date July 18, 2006

	This file contains the definition for the GUI message box class.
*/
//=================================================================================================

#pragma once
#ifndef __MsgBox_h
#define __MsgBox_h

#include "GUILayout.h"
#include "Math/Box2i.h"

class GUICtrlButton;
class GUIMessage;
class GUICtrlTextBox;


//-------------------------------------------------------------------------------------------------
/*!
	\class MsgBox
	\brief The class that encapsulates a GUI message box

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class MsgBox : public GUILayout
{
public:

	/// Message box button combinations
	enum EButtonLayout
	{
		BL_Error = -1,
		BL_OK,
		BL_OKCancel,
		BL_YesNo,
		BL_TextEntry,
		BL_Custom,
		BL_COUNT
	};

	/// Message box return values
	enum EMsgBoxReturn
	{
		MBR_Error = -1,
		MBR_OK,
		MBR_Yes,
		MBR_No,
		MBR_Cancel,
		MBR_Option_1,
		MBR_Option_2,
		MBR_Option_3,
		MBR_COUNT
	};

private:
	
	/// The message box return value, MBR_Error if the box is still active
	EMsgBoxReturn m_RetVal;

	/// The message box rectangle in screen coordinates
	Box2i m_WinRect;

	/// The user area of the message box (The window rectangle minus the border
	Box2i m_ClientRect;

	/// The string entered by the user if this message box contains a text box
	std::wstring m_sEnteredText;

	/// The button layout type
	EButtonLayout m_ButtonLayoutType;

	/// The callback function for the buttons
	static void ButtonCallback( GUICtrlButton* pButton );

	/// The default constructor, private so that the show functions must be used to create one
	MsgBox() : m_RetVal( MBR_Error ),
				m_ButtonLayoutType( BL_OK )
	{}

	/// Add a button to the message box
	GUICtrlButton* AddButton( const wchar_t* szText, const wchar_t* szCtrlName, int32 buttonIndex, int32 numButtons );

public:

	/// Create the message box
	static MsgBox* CreateMsgBox( const wchar_t* szMsg, EButtonLayout layout = BL_OK );

	/// Create the message box
	static MsgBox* CreateMsgBoxCustom( const wchar_t* szMsg, const wchar_t* szOption1, const wchar_t* szOption2 = 0, const wchar_t* szOption3 = 0 );

	/// Get the text box control if there is one
	GUICtrlTextBox* GetTextBoxCtrl();

	/// Get the result
	EMsgBoxReturn GetRetVal() const { return m_RetVal; }

	/// Handle a GUI message
	virtual bool HandleMessage( GUIMessage* pMsg );

	/// Draw the message box
	virtual void Draw() const;

	/// Get the user entered text
	const std::wstring& GetEnteredText() const { return m_sEnteredText; }
};

#endif // __MsgBox_h