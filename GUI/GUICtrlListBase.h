//=================================================================================================
/*!
	\file GUICtrlListBase.h
	2D Game Engine
	Graphical User Interface List Control Base Class Header
	\author Taylor Clark
	\date August 15, 2006

	This file contains the definition for the GUI list control base class.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlListBase_h
#define __GUICtrlListBase_h

#include "GUIControl.h"
#include <string>
#include <vector>
#include "Graphics2D/TCFont.h"
#include "Graphics2D/CachedFontDraw.h"
#include "GUIListItem.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlListBase
	\brief The base class for list constrols
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlListBase : public GUIControl
{
protected:

	/// The list of items
	typedef std::vector< GUIListItem > ListItemVector;
	ListItemVector m_Items;

	/// If the mouse is currently inside the control
	bool m_MouseIsWithin;

	/// The font with which to render text
	TCFontHndl m_Font;

	/// The font with which to render text if it is selected
	TCFontHndl m_SelectedFont;

	/// Cache a list item
	void CacheListItem( uint32 itemIndex );

public:

	/// The default constructor
	GUICtrlListBase();

	/// The default destructor
	virtual ~GUICtrlListBase(){}

	/// Can the user interact with this control
	virtual bool IsInteractive() const { return false; }

	/// Load/save the control data
	virtual void SubclassTransferData( Serializer& serializer );

	/// Handle the mouse cursor entering this control
	virtual bool OnMouseEnter(){ m_MouseIsWithin = true; return true; }

	/// Handle the mouse cursor leaving this control
	virtual bool OnMouseLeave(){ m_MouseIsWithin = false; return true; }

	/// Update the control based on its parameters so it displays correctly
	virtual void ReformatControl();

	/// Get the number of items in the list
	uint32 GetNumListItems() const { return (uint32)m_Items.size(); }

	// Get a string from the list
	std::wstring GetItemString( uint32 strIndex ) const
	{
		if( strIndex >= (uint32)m_Items.size() )
			return std::wstring(L"");
		return m_Items[ strIndex ].sText;
	}

	/// Find the index of a string in the list
	int32 IndexOfString( const wchar_t* szString );

	/// Add a string to the list
	uint32 AddString( const wchar_t* szStr );

	/// Insert a string into the list
	void InsertString( const wchar_t* szStr, uint32 insertIndex );

	/// Remove an item
	virtual void RemoveString( uint32 itemIndex );

	/// Remove all items
	void Clear();

	/// Get if the mouse is within the list control
	bool IsMouseOver() const { return m_MouseIsWithin; }
};

#endif // __GUICtrlListBase_h