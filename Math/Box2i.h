//=============================================================================
/*!
	\file Box2i.h
	Taylor Clark Math Library
	2-Dimensional Axis Aligned Box with Integer Components
	\author Taylor Clark

  This header contains the class definition for a 2-D box that is aligned to
  2 axes.
*/
//=============================================================================

#pragma once

#ifndef __Box2i_h
#define __Box2i_h

#include "Base/Types.h"
#include "Point2i.h"
#include "Vector2i.h"
#include "Interval.h"


/*=============================================================================

  Class Definitions

=============================================================================*/


//-----------------------------------------------------------------------------
/*! 
	\class Box2i
	\brief This is the 2D axis aligned box.

	This class encapsulates the functionality of a axis aligned 2-D box.
*/
//-----------------------------------------------------------------------------
class Box2i
{
public:
	//! The contstructor
	Box2i(){ }
	Box2i( const Box2i& copy )
	{
		pos.x = copy.pos.x;
		pos.y = copy.pos.y;
		size.x = copy.size.x;
		size.y = copy.size.y;
	}
	Box2i( const Point2i& inPos, const Vector2i& inSize ) : pos( inPos ), size( inSize )
	{}
	Box2i( int32 x, int32 y, int32 width, int32 height ) : pos( x, y ), size( width, height )
	{}
	const Box2i& operator = ( const Box2i& rhs )
	{
		pos = rhs.pos; size = rhs.size;
		return *this;
	}

	//! Set all of the data
	void Set( int32 x, int32 y, int32 width, int32 height )
	{
		pos.Set( x, y );
		size.Set( width, height );
	}
	
	/// Increse the size of the box, but keep it centered
	void Inflate( int32 absAmt )
	{
		Inflate( absAmt, absAmt );
	}

	/// Increse the size of the box, but keep it centered
	void Inflate( int32 absXAmt, int32 absYAmt )
	{
		pos.x -= (absXAmt / 2);
		pos.y -= (absYAmt / 2);
		size.x += absXAmt;
		size.y += absYAmt;
	}

	/// Get the center point of the box
	Point2i GetCenter()
	{
		return Point2i( pos.x + (size.x / 2), pos.y + (size.y / 2) );
	}

	/// Comparison
	bool operator ==( const Box2i& rhs ) const { return (pos == rhs.pos) && (size == rhs.size); }

	//! Is a point within the box
	bool IsPointInside( const Point2i& pt ) const
	{
		// If we have a negative dimension, check differently
		if( size.x < 0 )
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
		if( size.y < 0 )
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

	bool Intersects( const Box2i& rhs ) const
	{
		// Treat the box dimensions as intervals and see if the horizontal and
		// vertical interval intersect
		IntervalInt thisVertInterval = IntervalInt::MakeClosed( pos.y, pos.y + size.y );
		IntervalInt thisHorzInterval = IntervalInt::MakeClosed( pos.x, pos.x + size.x );
		IntervalInt testVertInterval = IntervalInt::MakeClosed( rhs.pos.y, rhs.pos.y + rhs.size.y );
		IntervalInt testHorzInterval = IntervalInt::MakeClosed( rhs.pos.x, rhs.pos.x + rhs.size.x );

		return thisVertInterval.IsIntervalIntersecting( testVertInterval )
				&& thisHorzInterval.IsIntervalIntersecting( testHorzInterval );
	}

	//! Access a vertex bases on quadrants (0 is top right, 1 is top left,
	//! 2 is bottom left, and 3 is bottom right)
	Point2i operator[]( int32 index ) const
	{
		if( index == 0 )
			return Point2i( Right(), pos.y );
		else if( index == 1 )
			return pos;
		else if( index == 2 )
			return Point2i( pos.x, Bottom() );

		return Point2i( Right(), Bottom() );
	}

	//! Get the right and bottom
	int32 Left() const { return pos.x; }
	int32 Top() const { return pos.y; }
	int32 Right() const { return pos.x + size.x; }
	int32 Bottom() const { return pos.y + size.y; }

	//! The position the box
	Point2i pos;

	//! The size of the box
	Vector2i size;
};

#endif	//__Box2_h