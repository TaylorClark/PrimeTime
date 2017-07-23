//=================================================================================================
/*!
	\file GUICtrlList.h
	2D Game Engine
	Graphical User Interface List Control Header
	\author Taylor Clark
	\date February 18, 2006

	This file contains the definition for the GUI list control class.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlList_h
#define __GUICtrlList_h

#include "GUICtrlListBase.h"
#include "Graphics2D/RefSprite.h"
#include "GamePlay/AnimSprite.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlList
	\brief The class that represents a list control in a GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlList : public GUICtrlListBase
{
public:
	/// The callback function prototype for when an item is selected from a list
	typedef void (*OnListSelCallback)( uint32, GUICtrlList* );

private:

	/// The image to use when drawing the selection cursor
	RefSpriteHndl m_CursorSprite;

	/// The animated sprite data
	mutable AnimSprite m_AnimSprite;

	/// The callback function for when a list item is selected
	OnListSelCallback m_pSelCallbackFunc;

	/// The bounding box, cached to avoid calculation every frame
	Box2i m_BoundBox;

	/// The index of the list item that is selected
	int32 m_CurListItem;

	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const;

	/// Calculate the bounding box the the control
	Box2i CalculateBoundBox() const;

	/// Get an item index from a mouse position
	int32 GetItemFromPos( Point2i screenPos ) const;

public:

	/// The default constructor
	GUICtrlList();

	/// Draw the control
	virtual void Draw() const;

	/// Load/save the control data
	virtual void SubclassTransferData( Serializer& serializer );

	/// Update the control
	virtual void Update( float32 );

	/// Set the callback function for when a list item is selected
	void SetCallback( OnListSelCallback pCallbackFunc )
	{
		m_pSelCallbackFunc = pCallbackFunc;
	}

	/// Handle a mouse button being pressed
	virtual bool OnMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos );

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType()
	{
		return GUIDefines::CT_List;
	}

	/// Update the control based on its parameters so it displays correctly
	virtual void ReformatControl();

	/// Get the index of the item the mouse is currently over
	int32 GetMouseOverItem() const { return m_CurListItem; }
};

#endif // __GUICtrlList_h