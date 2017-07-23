//=============================================================================
/*!
	\file Point2i.h
	Taylor Clark Math Library
	2-Dimensional Point With Integer Components
	By Taylor Clark

  This header contains the declaration for the 2D point class.
*/
//=============================================================================

#pragma once
 
#ifndef __Point2i_h
#define __Point2i_h

#include "Base/Types.h"
#include "Vector2i.h"


/*=============================================================================

  Class Definitions

=============================================================================*/


//-----------------------------------------------------------------------------
/*! 
	\class Point2
	\brief This is the 2-Dimensional point class.

	The 2-Dimensional point class.
*/
//-----------------------------------------------------------------------------
class Point2i
{
public:
	int32 x, y;

public:

	//! Constructors
	explicit Point2i( int32 nX = 0, int32 nY = 0 ) : x(nX), y(nY) { }
	Point2i( const Point2i& rhs ) : x(rhs.x), y(rhs.y) { }
	const Point2i& operator =( const Point2i& rhs ){ x = rhs.x; y = rhs.y; return *this; }
	void Set( int32 nX, int32 nY ){ x = nX; y = nY; }

	//! Access
	int32 operator[]( int32 index )
	{
		if( index == 0 )
			return x;
		return y;
	}

	/// Comparison
	bool operator ==( const Point2i& rhs ) const { return x == rhs.x && y == rhs.y; }

	//! Arithmetic operators
	Point2i operator +( const Vector2i& rhs ) const { return Point2i( x + rhs.x, y + rhs.y ); }
	const Point2i& operator +=( const Vector2i& rhs ){ x += rhs.x; y += rhs.y; return *this; }
	
	Vector2i operator -( const Point2i& rhs ) const { return Vector2i( x - rhs.x, y - rhs.y ); }
	Point2i operator -( const Vector2i& rhs ) const { return Point2i( x - rhs.x, y - rhs.y ); }
	const Point2i& operator -=( const Vector2i& rhs ){ x -= rhs.x; y -= rhs.y; return *this; }
};


#endif //__Point2i_h
