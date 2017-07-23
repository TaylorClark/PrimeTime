//=================================================================================================
/*!
	\file CachedFontDraw.h
	2D Graphics Engine
	Cached Drawn Font Text Header
	\author Taylor Clark
	\date September 9, 2006

	This file contains the definition for the cached drawn font text class.
*/
//=================================================================================================

#pragma once
#ifndef __CachedFontDraw_h
#define __CachedFontDraw_h

#include <vector>
#include "TCFont.h"
#include "Math/Point2i.h"
#include "Math/Vector2i.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class CachedFontDraw
	\brief A cached version of text that is drawn

	This object stores the information required when drawing a string in a optimized format.
*/
//-------------------------------------------------------------------------------------------------
struct CachedFontDraw
{
	struct Char
	{
		Point2i screenPos;
		Box2i screenRect;
		Box2i srcRect;
	};

	/// The destructor
	virtual ~CachedFontDraw() {}

	/// The list of characters to draw
	std::vector<Char> Chars;

	/// The font to use
	TCFontHndl m_Font;

	/// The string dimensions
	Vector2i m_Size;

	/// Get the starting position of the text
	virtual Point2i GetPos() const;

	/// Get the maximum dimensions
	virtual Vector2i GetSize() const { return m_Size; }

	/// Offset all of the character's positions by a value
	virtual void OffsetCharacters( Vector2i offset );
	
	/// Clear the data stored
	virtual void Clear();
};

#endif // __CachedFontDraw_h