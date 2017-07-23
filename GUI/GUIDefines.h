//=================================================================================================
/*!
	\file GUIDefines.h
	2D Game Engine
	Graphical User Interface Defines Header
	\author Taylor Clark
	\date January 31, 2006

	This file contains defines related to the GUI library.
*/
//=================================================================================================

#pragma once
#ifndef __GUIDefines_h
#define __GUIDefines_h

class GUICtrlScrollList;
class GUICtrlButton;


namespace GUIDefines
{
	/// Mouse button constants for mouse down/up messages
	enum EMouseButtons
	{
		MB_Left,
		MB_Right,
		MB_Middle,
		MB_COUNT
	};

	/// The message types that are sent within a GUI
	enum EGUIMessageType
	{
		GMT_Error = -1,
		GMT_KeyDown,
		GMT_KeyUp,
		GMT_KeyChar,
		GMT_MouseDown,
		GMT_MouseUp,
		GMT_COUNT
	};

	/// The different types of controls available
	enum EControlType
	{
		CT_Error = -1,
		CT_Button,
		CT_Label,
		CT_List,
		CT_Sprite,
		CT_TextBox,
		CT_ScrollList,
		CT_CheckBox,
		CT_GameMgr,
		CT_COUNT
	};

	/// The different type or properties that can be set on a control when editting a layout
	enum EPropType
	{
		PT_Error = -1,
		PT_Int,
		PT_String,
		PT_StringList,
		PT_Font,
		PT_Sprite,
		PT_Boolean,
		PT_COUNT
	};
};

#endif // __GUIDefines_h