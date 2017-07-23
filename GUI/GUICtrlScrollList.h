//=================================================================================================
/*!
	\file GUICtrlScrollList.h
	2D Game Engine
	Graphical User Interface List Control Header
	\author Taylor Clark
	\date February 18, 2006

	This file contains the definition for the GUI list control class.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlScrollList_h
#define __GUICtrlScrollList_h

#include "GUICtrlListBase.h"
#include "Graphics2D/RefSprite.h"
#include "GamePlay/AnimSprite.h"

class GUICtrlButton;


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlScrollList
	\brief The class that represents a list control in a GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlScrollList : public GUICtrlListBase
{
public:
	/// The callback function prototype for when an item is selected
	typedef void (*ScrollListSelectCallback)( GUICtrlScrollList* );

private:
	/// The bounding box
	Vector2i m_Dims;

	/// The index of the first visible item
	uint32 m_FirstVisIndex;
	
	/// The selected item index
	int32 m_SelItemIndex;

	/// The selected item index
	int32 m_MouseOverIndex;

	/// The number of fully visible items
	uint32 m_NumVisItems;

	ScrollListSelectCallback _OnItemSelectCallback;

	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const;

	/// Calculate the bounding box the the control
	Box2i CalculateBoundBox() const;

	/// Recalculate the string positions after scrolling
	void CacheVisStrings();

	/// The callback for scrolling up
	static void ScrollUpCallback( GUICtrlButton* pBtn );

	/// The callback for scrolling down
	static void ScrollDownCallback( GUICtrlButton* pBtn );

public:

	/// The default constructor
	GUICtrlScrollList();

	/// Draw the control
	virtual void Draw() const;

	/// Load/save the control data
	virtual void SubclassTransferData( Serializer& serializer );

	/// Handle a mouse button being pressed
	virtual bool OnMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos );

	/// Remove an item
	virtual void RemoveString( uint32 itemIndex );

	/// Get an item index from a mouse position
	int32 GetItemFromPos( Point2i screenPos ) const;

	/// Set the dimensions of the list box
	void SetDims( Vector2i dims );

	/// Get the selected item index
	int32 GetSelItemIndex() const { return m_SelItemIndex; }

	/// Set the selected index
	void SetSelItemIndex( int32 newSelectedIndex );

	/// Remove all of the strings from the list box
	void ClearStrings()
	{
		m_Items.clear();
		m_FirstVisIndex = 0;
		m_SelItemIndex = -1;
		m_NumVisItems = 0;
	}

	/// Update the control based on its parameters so it displays correctly
	virtual void ReformatControl();

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType()
	{
		return GUIDefines::CT_ScrollList;
	}

	/// Scroll up
	void Scroll( int32 offset );

	/// Setup the scroll buttons
	void SetScrollButtons( GUICtrlButton* pUpBtn, GUICtrlButton* pDownBtn );

	/// Add a callback that gets invoked when a list item is selected
	void SetItemSelectCallback( ScrollListSelectCallback callbackFunc )
	{
		_OnItemSelectCallback = callbackFunc;
	}
};

#endif // __GUICtrlScrollList_h