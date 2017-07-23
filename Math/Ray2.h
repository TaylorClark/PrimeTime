//=============================================================================
/*!
	\file Ray2.h
	Taylor Clark Math Library
	2-Dimensional Vector with Position Header
	\author Taylor Clark

  This header contains the class declaration for a 2-D vector that has a
  position, and/or a ray.
*/
//=============================================================================

#pragma once

#ifndef __Ray2_h
#define __Ray2_h

#include "Vector2.h"
#include "Point2.h"


/*=============================================================================

  Class Definitions

=============================================================================*/


//-----------------------------------------------------------------------------
/*! 
	\class Ray2
	\brief This is the 2D translated vector class.

	This class encapsulates the functionality of a vector with the position
	of a point.
*/
//-----------------------------------------------------------------------------
class Ray2
{
public:
	//! The contstructor
	Ray2(){ }
	Ray2( const Ray2& copy ) { dir = copy.dir; pos = copy.pos; }
	Ray2( const Point2& pos, const Vector2& vec ) : pos( pos ), dir( vec ) { }
	const Ray2& operator = ( const Ray2& rhs ) { dir = rhs.dir; pos = rhs.pos; return *this; }

	//! If two position vectors overlap
	bool Intersects( const Ray2& rhs ) const
	{
		// If both position vectors intersect each other as rays, they
		// intersect
		if( RayIntersect( rhs ) && rhs.RayIntersect( *this ) )
			return true;

		// Return they do no intersect
		return false;
	}

	/*
	//! Get intersection with a plane
	float32 GetTimeIntersectWith( const Point2& planePos,
								const Vector2& planeNorm ) const
	{
		// Get the dot product to test if the rays are parallel
		float32 dotVal = dir.Dot( planeNorm );
		if( dotVal == 0.0f )
			return -1.0f;

		// Get the intersection time
		float32 time = -(pos - planePos).Dot( planeNorm ) / dotVal;
		
		// Return the time
		return time;
	}
	*/

	//! Get intersection with a circle
	bool Intersects( const Point2& center, float32 radius )
	{
		// Get the vector from the ray to the circle
		Vector2 toPt( center - pos );

		// If there is no ray direction, see if the ray is inside
		if( dir.GetMagSq() == 0.0f )
		{
			if( toPt.GetMagSq() < (radius * radius) )
				return true;
			return false;
		}

		// Get the scalar of projection
		float32 scalar = toPt.Dot( dir ) / dir.GetMagSq();

		// Clamp the scalar
		if( scalar < 0 )
			scalar = 0;
		else if( scalar > 1.0f )
			scalar = 1.0f;

		// See if this point in the circle
		toPt = (pos + (dir * scalar)) - center;
		if( toPt.GetMagSq() < (radius * radius) )
			return true;
		return false;
	}

	//! Get the time of intersection between 2 rays
	float32 GetTimeIntersectWith( const Ray2& rhs ) const
	{
		// Get the normal for the ray
		Vector2 v2Norm = rhs.dir.GetOrthoN90();

		// Get the dot product to test if the rays are parallel
		float32 dotVal = dir.Dot( v2Norm );
		if( dotVal == 0.0f )
			return -1.0f;

		// Get the intersection time
		float32 time = -(pos - rhs.pos).Dot( v2Norm ) / dotVal;

		/*
		// Get the intersection point
		Point2 intPt = pos + (dir * time);

		// Ensure the intersection point on the second ray is in its direction
		Vector2 intVec = intPt - rhs.pos;
		if( intVec.Dot( rhs.dir ) < 0.0f )
			return -1.0f;
		*/

		// Return the time
		return time;

		// 4 floats on stack, 4 multiply, 1 divide
	}

	// Get a point from time
	Point2 GetPoint( float32 time ) const
	{
		return pos + (dir * time);
	}

private:
	//! See if the position vector would intersect another if it had infinite
	//! magnitude
	bool RayIntersect( const Ray2& rhs ) const
	{
		// Get an orthogonal clockwise vector - 2 floats
		Vector2 cwOrthogonal( dir.GetOrtho90() );

		// Get the vector from this start point to the second's end point
		// 2 floats, 2 subtracts
		Vector2 v1sv2e( (rhs.pos + rhs.dir) - pos );

		// Get the vector from this start point to the second's start point
		// 2 floats, 2 subtracts
		Vector2 v1sv2s( rhs.pos - pos );

		// Get the dot products
		//float32 dot1 = v1sv2e.Dot( cwOrthogonal );
		//float32 dot2 = v1sv2s.Dot( cwOrthogonal );

		// If the dots change, then they intersect
		//if( ((dot1 > 0) && (dot2 <= 0))
		//	|| ((dot1 < 0) && (dot2 >= 0)) )
		if( (v1sv2e.Dot( cwOrthogonal ) * v1sv2s.Dot( cwOrthogonal )) < 0.0f )
		{
			return true;
		}
		// 6 floats on stack, 4 subtracts, 4 multiplies
		/*
		// Get the endpoints for the test vector - 2 floats
		Point2 rhsEndPt( rhs.pos + rhs.dir );

		// Get the vectors to test with - 4 floats 4 subtracts
		Vector2 toRhsStartPt( rhs.pos - pos );
		Vector2 toRhsEndPt( rhsEndPt - pos );

		// Get the cross values - 2 float32 4 mult 2 subtract
		float32 crossStart = dir.Cross( toRhsStartPt );
		float32 crossEnd = dir.Cross( toRhsEndPt );

		// If the crosses change sign, the ray would intersect
		//if( ((crossStart >= 0.0f) && (crossEnd < 0.0f))
		//	|| ((crossStart < 0.0f) && (crossEnd >= 0.0f)) )
		if( (crossStart * crossEnd) < 0.0f )
		{
			// They intersect
			return true;
		}
		// 8 floats on stack, 6 subtracts, 4 multiplies
		*/

		// They don't intersect
		return false;
	}
	
public:
	//! The 2D vector
	Vector2 dir;

	//! The 2D position
	Point2 pos;
};

#endif	//__Ray2_h