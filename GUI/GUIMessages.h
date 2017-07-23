//=================================================================================================
/*!
	\file GUIMessages.h
	2D Game Engine
	Graphical User Interface Messages Header
	\author Taylor Clark
	\date February 3, 2006

	This file contains the definition for the GUI message classes.
*/
//=================================================================================================

#pragma once
#ifndef __GUIMessages_h
#define __GUIMessages_h

#include "GUIDefines.h"
#include "Math/Point2i.h"
#include <vector>

class GUIMessageMgr;


//-------------------------------------------------------------------------------------------------
/*!
	\class GUIMessage

	The base GUI message class.
*/
//-------------------------------------------------------------------------------------------------
class GUIMessage
{
public:

    /// Virtual to ensure the destructor gets called on all derived classes
    virtual ~GUIMessage(){}
    
	/// Get the type of GUI message this is
	virtual GUIDefines::EGUIMessageType GetType() const = 0;
};

typedef std::list< GUIMessage* > GUIMessageList;


//-------------------------------------------------------------------------------------------------
/*!
	\class GUIMessageKeyDown

	The key down GUI message class.
*/
//-------------------------------------------------------------------------------------------------
class GUIMessageKeyDown : public GUIMessage
{
	friend class GUIMessageMgr;

private:

	/// The key code of the button pressed
	unsigned int m_WindowsKeyCode;

	/// The default constructor
	GUIMessageKeyDown( unsigned int keyCode ) : m_WindowsKeyCode( keyCode )
	{
	}
	
public:

	/// Get the windows key code of the key pressed
	unsigned int GetKeyCode() const { return m_WindowsKeyCode; }

	/// Get the type of GUI message this is
	virtual GUIDefines::EGUIMessageType GetType() const { return GUIDefines::GMT_KeyDown; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUIMessageKeyUp

	The key up GUI message class.
*/
//-------------------------------------------------------------------------------------------------
class GUIMessageKeyUp : public GUIMessage
{
	friend class GUIMessageMgr;

private:

	/// The key code of the button released
	unsigned int m_WindowsKeyCode;

	/// The default constructor
	GUIMessageKeyUp( unsigned int keyCode ) : m_WindowsKeyCode( keyCode )
	{
	}
	
public:

	/// Get the windows key code of the key pressed
	unsigned int GetKeyCode() const { return m_WindowsKeyCode; }

	/// Get the type of GUI message this is
	virtual GUIDefines::EGUIMessageType GetType() const { return GUIDefines::GMT_KeyUp; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUIMessageKeyChar

	The key character GUI message class.
*/
//-------------------------------------------------------------------------------------------------
class GUIMessageKeyChar : public GUIMessage
{
	friend class GUIMessageMgr;

private:

	/// The character being pressed
	wchar_t m_Char;

	/// The default constructor
	GUIMessageKeyChar( wchar_t charPress ) : m_Char( charPress )
	{
	}
	
public:

	/// Get the character pressed
	wchar_t GetChar() const { return m_Char; }

	/// Get the type of GUI message this is
	virtual GUIDefines::EGUIMessageType GetType() const { return GUIDefines::GMT_KeyChar; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUIMessageMouseDown

	The mouse down GUI message class.
*/
//-------------------------------------------------------------------------------------------------
class GUIMessageMouseDown : public GUIMessage
{
	friend class GUIMessageMgr;

private:

	/// The button being pressed
	GUIDefines::EMouseButtons m_Button;

	/// The position of the cursor when the button was pressed
	Point2i m_MousePos;

	/// The default constructor
	GUIMessageMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos ) : m_Button( btn ),
																			m_MousePos( mousePos )
	{
	}

public:

	/// Get the mouse button released
	GUIDefines::EMouseButtons GetButton() const { return m_Button; }

	/// Get the position of the cursor when the button was pressed
	const Point2i& GetPos() const { return m_MousePos; }
	
	/// Get the type of GUI message this is
	virtual GUIDefines::EGUIMessageType GetType() const { return GUIDefines::GMT_MouseDown; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUIMessageMouseUp

	The mouse up GUI message class.
*/
//-------------------------------------------------------------------------------------------------
class GUIMessageMouseUp : public GUIMessage
{
	friend class GUIMessageMgr;

private:

	/// The button being released
	GUIDefines::EMouseButtons m_Button;

	/// The position of the cursor when the button was pressed
	Point2i m_MousePos;

	/// The default constructor
	GUIMessageMouseUp( GUIDefines::EMouseButtons btn, Point2i mousePos ) : m_Button( btn ),
																			m_MousePos( mousePos )
	{
	}
	
public:

	/// Get the mouse button released
	GUIDefines::EMouseButtons GetButton() const { return m_Button; }

	/// Get the position of the cursor when the button was pressed
	const Point2i& GetPos() const { return m_MousePos; }

	/// Get the type of GUI message this is
	virtual GUIDefines::EGUIMessageType GetType() const { return GUIDefines::GMT_MouseUp; }
};


class GUIMessageMgr
{
private:

	/// The default constructor, private since it is a singleton
	GUIMessageMgr(){}

	std::vector< GUIMessageMouseUp > m_MouseUpMsgs;

public:

	static GUIMessageMgr& Get()
	{
		static GUIMessageMgr m_Mgr;
		return m_Mgr;
	}

	GUIMessageKeyDown* CreateKeyDown( unsigned int keyCode )
	{
		return new GUIMessageKeyDown( keyCode );
	}

	GUIMessageKeyUp* CreateKeyUp( unsigned int keyCode )
	{
		return new GUIMessageKeyUp( keyCode );
	}

	GUIMessageKeyChar* CreateKeyChar( wchar_t keyChar )
	{
		return new GUIMessageKeyChar( keyChar );
	}

	GUIMessageMouseUp* CreateMouseUp( GUIDefines::EMouseButtons btn, Point2i mousePos )
	{
		return new GUIMessageMouseUp( btn, mousePos );
	}

	GUIMessageMouseDown* CreateMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos )
	{
		return new GUIMessageMouseDown( btn, mousePos );
	}
};

#endif // __GUIMessages_h