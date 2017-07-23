//=================================================================================================
/*!
	\file GUICtrlSprite.h
	2D Game Engine
	Graphical User Interface Sprite Control Header
	\author Taylor Clark
	\date June 11, 2006

	This file contains the definition for the GUI sprite control class.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlSprite_h
#define __GUICtrlSprite_h

#include "GUIControl.h"
#include <string>
#include <vector>
#include "GamePlay/AnimSprite.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlSprite
	\brief The class that represents a sprite image in a GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlSprite : public GUIControl
{
public:

	/// The image to draw
	AnimSprite m_Sprite;

	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const;

public:

	/// The default constructor
	GUICtrlSprite();

	/// Draw the control
	virtual void Draw() const;

	/// Update the control
	virtual void Update( float32 frameTime );

	/// Can the user interact with this control
	virtual bool IsInteractive() const { return false; }

	/// Load/save the control data
	virtual void SubclassTransferData( Serializer& serializer );

	void SetSprite( const AnimSprite& sprite )
	{
		m_Sprite = sprite;
	}

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType()
	{
		return GUIDefines::CT_Sprite;
	}
};

#endif // __GUICtrlSprite_h