//=============================================================================
/*!
	\file Point2.h
	Taylor Clark Math Library
	2-Dimensional Point
	By Taylor Clark

  This header contains the declaration for the 2D point class.
*/
//=============================================================================

#pragma once

#ifndef __Point2_h
#define __Point2_h

#include "Base/Types.h"
#include "Vector2.h"


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
class Point2
{
public:
	float32 x, y;

public:

	//! Constructors
	explicit Point2( float32 nX = 0.0f, float32 nY = 0.0f ) : x(nX), y(nY) { }
	Point2( const Point2& rhs ) : x(rhs.x), y(rhs.y) { }
	const Point2& operator =( const Point2& rhs ){ x = rhs.x; y = rhs.y; return *this; }
	void Set( float32 nX, float32 nY ){ x = nX; y = nY; }

	//! Access
	float32 operator[]( int32 index )
	{
		if( index == 0 )
			return x;
		return y;
	}

	//! Arithmetic operators
	Point2 operator +( const Vector2& rhs ) const { return Point2( x + rhs.x, y + rhs.y ); }
	const Point2& operator +=( const Vector2& rhs ){ x += rhs.x; y += rhs.y; return *this; }
	
	Vector2 operator -( const Point2& rhs ) const { return Vector2( x - rhs.x, y - rhs.y ); }
	const Point2& operator -=( const Vector2& rhs ){ x -= rhs.x; y -= rhs.y; return *this; }
};


#endif //__Point2_h
