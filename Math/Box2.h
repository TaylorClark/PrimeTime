//=============================================================================
/*!
	\file Box2.h
	Taylor Clark Math Library
	2-Dimensional Axis Aligned Box
	\author Taylor Clark

  This header contains the class definition for a 2-D box that is aligned to
  2 axes.
*/
//=============================================================================

#pragma once

#ifndef __Box2_h
#define __Box2_h

#include "Base/Types.h"
#include "Point2.h"
#include "Vector2.h"


/*=============================================================================

  Class Definitions

=============================================================================*/


//-----------------------------------------------------------------------------
/*! 
	\class Box2
	\brief This is the 2D axis aligned box.

	This class encapsulates the functionality of a axis aligned 2-D box.
*/
//-----------------------------------------------------------------------------
class Box2
{
public:
	//! The contstructor
	Box2(){ }
	Box2( const Box2& copy )
	{
		pos = copy.pos; size = copy.size;
	}
	Box2( const Point2& inPos, const Vector2& inSize ) : pos( inPos ), size( inSize )
	{}
	Box2( float32 x, float32 y, float32 width, float32 height ) : pos( x, y ), size( width, height )
	{}
	const Box2& operator = ( const Box2& rhs )
	{
		pos = rhs.pos; size = rhs.size;
		return *this;
	}

	//! Set all of the data
	void Set( float32 x, float32 y, float32 width, float32 height )
	{
		pos.Set( x, y );
		size.Set( width, height );
	}
	
	//! Does this box intersect another box
	bool DoesIntersectBox( const Box2& testBox ) const;

	//! Is a point within the box
	bool IsPointInside( const Point2& pt ) const
	{
		// If we have a negative dimension, check differently
		if( size.x < 0.0f )
		{
			if( (pt.x > pos.x) || (pt.x < Right()) )
				return false;
		}
		// Else check normally
		else
		{
			if( (pt.x < pos.x) || (pt.x > Right()) )
				return false;
		}

		// Test the y component
		if( size.y < 0.0f )
		{
			if( (pt.y > pos.y) || (pt.y < Bottom()) )
				return false;
		}
		// Else check normally
		else
		{
			if( (pt.y < pos.y) || (pt.y > Bottom()) )
				return false;
		}

		// The point is inside
		return true;
	}

	//! Snap this box to another
	bool IntelliSnapToBox( const Box2& snapBox, const float32 threshold = 10.0f );

	//! Access a vertex bases on quadrants (0 is top right, 1 is top left,
	//! 2 is bottom left, and 3 is bottom right)
	Point2 operator[]( int32 index ) const
	{
		if( index == 0 )
			return Point2( Right(), pos.y );
		else if( index == 1 )
			return pos;
		else if( index == 2 )
			return Point2( pos.x, Bottom() );

		return Point2( Right(), Bottom() );
	}

	//! Get the right and bottom
	float32 Left() const { return pos.x; }
	float32 Top() const { return pos.y; }
	float32 Right() const { return pos.x + size.x; }
	float32 Bottom() const { return pos.y + size.y; }

	//! The position the box
	Point2 pos;

	//! The size of the box
	Vector2 size;
};

#endif	//__Box2_h