//=============================================================================
/*!
	\file Circle.h
	Taylor Clark Math Library
	Circle Object
	\author Taylor Clark

  This header contains the class declaration for a circle class
*/
//=============================================================================

#pragma once

#ifndef __Circle_h
#define __Circle_h

#include "Base/Types.h"
#include "Point2.h"
#include "Vector2.h"
#include "Ray2.h"


/*=============================================================================

  Class Definitions

=============================================================================*/


//-----------------------------------------------------------------------------
/*! 
	\class Circle
	\brief This is a circle class

	This class encapsulates the functionality of a circle.
*/
//-----------------------------------------------------------------------------
class Circle
{
public:
	//! The contstructors
	Circle() : radius(0.0f) { }
	Circle( const Circle& copy )
	{
		center = copy.center; radius = copy.radius;
	}
	Circle( const Point2& cCenter, float32 cRadius )
			: center( cCenter ), radius( cRadius )
	{}
	const Circle& operator = ( const Circle& rhs )
	{
		center = rhs.center; radius = rhs.radius;
		return *this;
	}
	
	//! Set the values
	void Set( const Point2& newCenter, float32 newRadius )
	{
		center = newCenter;
		radius = newRadius;
	}

	//! Is a point within the circle
	bool IsPointInside( const Point2& pt ) const;

	//! Does a ray collide with the circle
	bool GetTimeRayIntersect( const Ray2& ray,
								float32 *pTime1 = 0,
								float32 *pTime2 = 0 ) const;

	//! Get the closest point on the circle to an edge
	Point2 GetClosestPoint( const Ray2& edge ) const;

	//! Test for collision with another circle
	bool Intersects( const Circle& testCircle ) const
	{
		// Get a vector between the circle center
		Vector2 centVect( center - testCircle.center );

		// Get the radius sum
		float32 radiiSum = radius + testCircle.radius;

		// See if the circles are close enough
		if( centVect.GetMagSq() < (radiiSum * radiiSum) )
			return true;

		// The circles are too far apart
		return false;
	}

	//! The center of the circle
	Point2 center;

	//! The radius of the circle
	float32 radius;
};

#endif	//__Circle_h