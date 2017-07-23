//=================================================================================================
/*!
	\file GameMgrCtrl.h
	Game Play Library
	Game Manager Control Header
	\author Taylor Clark
	\date March 25, 2007

	This header contains the definition for the game manger GUI control object that is used to
	update and draw the game manager's data.
*/
//=================================================================================================

#pragma once
#ifndef __GameMgrCtrl_h
#define __GameMgrCtrl_h

#include "GUI/GUIControl.h"

class MsgBox;
class GUIMessage;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameMgrCtrl
	\brief Represents a GUI control for rendering the game in a manner coherent with the rest of
			the UI.
*/
//-------------------------------------------------------------------------------------------------
class GameMgrCtrl : public GUIControl
{
public:
	static const wchar_t* CTRL_NAME;

private:

	MsgBox* m_pExitMsgBox;

	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const;

public:

	/// The default constructor
	GameMgrCtrl() : m_pExitMsgBox( 0 )
	{
		m_sName = CTRL_NAME;
	}

	/// The destructor
	virtual ~GameMgrCtrl();

	/// Draw the control
	virtual void Draw() const;

	/// Update the control
	virtual void Update( float32 frameTime );

	/// Can the user interact with this control
	virtual bool IsInteractive() const { return true; }

	/// Handle a mouse button being pressed
	virtual bool OnMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos );

	/// Handle a key being pressed down
	virtual bool OnKeyDown( unsigned int windowsKeyCode );
	
	/// Handle a key being released
	virtual bool OnKeyUp( unsigned int windowsKeyCode );

	/// Get if this control can receive focus
	virtual bool CanReceiveFocus() const { return true; }

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType(){ return GUIDefines::CT_GameMgr; }
};

#endif //__GameMgrCtrl_h