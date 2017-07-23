//=================================================================================================
/*!
	\file GUIListItem.h
	2D Game Engine
	Graphical User Interface List Control Header
	\author Taylor Clark
	\date February 18, 2006

	This file contains the definition for the GUI list item class.
*/
//=================================================================================================

#pragma once
#ifndef __GUIListItem_h
#define __GUIListItem_h

#include <string>
#include "Graphics2D/CachedFontDraw.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlList
	\brief The class that represents an item in a GUI list item
*/
//-------------------------------------------------------------------------------------------------
struct GUIListItem
{
	/// The default constructor
	GUIListItem() {}

	/// The constructor to initialize the text for the item
	GUIListItem( const wchar_t* szText ) : sText( szText )
	{}

	/// The constructor to initialize the text for the item
	GUIListItem( std::wstring text ) : sText( text )
	{}

	virtual ~GUIListItem() {}

	std::wstring sText;

	CachedFontDraw cachedDraw;
};

#endif // __GUIListItem_h