//=================================================================================================
/*!
	\file CachedFontDraw.cpp
	2D Graphics Engine
	Cached Drawn Font Text Source
	\author Taylor Clark
	\date September 9, 2006

	This file contains the implementation for the cached drawn font text class methods.
*/
//=================================================================================================

#include "../CachedFontDraw.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CachedFontDraw::GetPos  Public
///
///	\returns The starting position of the text
///
/// Get the starting position of the text
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Point2i CachedFontDraw::GetPos() const
{
	if( Chars.size() == 0 )
		return Point2i();
	return Chars.begin()->screenPos;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CachedFontDraw::OffsetCharacters  Public
///
///	\param offset The amount to offset each of there characters in this cached draw
///
/// Offset all of the character's positions by a value
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CachedFontDraw::OffsetCharacters( Vector2i offset )
{
	//for( std::list<Char>::iterator iterChar = Chars.begin(); iterChar != Chars.end(); ++iterChar )
	//	iterChar->screenPos += offset;
	for( uint32 charIndex = 0; charIndex < Chars.size(); ++charIndex )
	{
		Chars[charIndex].screenPos += offset;
		Chars[charIndex].screenRect.pos += offset;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CachedFontDraw::Clear  Public
///
/// Clear the data stored
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CachedFontDraw::Clear()
{
	Chars.clear();
	m_Font.ReleaseHandle();
}