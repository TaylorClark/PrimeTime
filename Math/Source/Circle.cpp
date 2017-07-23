//=============================================================================
/*!
	\file Circle.cpp
	Taylor Clark Math Library
	Circle Object Class
	\author Taylor Clark

  This source file contains the circle object implementation.
*/
//=============================================================================

#include "../Circle.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Circle::RayCollides( ) Public
/*!
//	\param pt The point to test
//
//	Is a point within the circle.
*/
///////////////////////////////////////////////////////////////////////////////
bool Circle::IsPointInside( const Point2& pt ) const
{
	// Get the vector to the point
	Vector2 toPt( pt - center );

	// See if the point is close enough
	if( toPt.GetMagSq() < (radius * radius) )
		return true;

	// The point is outside
	return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Circle::GetClosestPoint( ) Public
/*!
//	\param edge A ray representing an edge.
//
//	Get the closest point on the circle to an edge represented by a ray.
*/
///////////////////////////////////////////////////////////////////////////////
Point2 Circle::GetClosestPoint( const Ray2& edge ) const
{
	// Get a vector from the edge to the circle
	Vector2 toCircle( center - edge.pos );

	// Project the vector onto the edge
	float32 scalar = toCircle.Dot( edge.dir ) / edge.dir.Dot( edge.dir );
	Point2 pointOnEdge( edge.pos + (edge.dir * scalar) );

	// Create a ray from the circle's center to the closest point on the edge
    Vector2 toPt( pointOnEdge - center );

	// Normalize and scale by the radius
	toPt.Normalize();
	toPt *= radius;

	// Return the point
	return Point2( center + toPt );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Circle::GetTimeRayIntersect( ) Public
/*!
//	\param ray The ray to test
//	\param pTime1 If not null, stores the time of the first intersection.
//					This parameter defaults to null.
//	\param pTime2 If not null, stores the time of the second intersection
//					This parameter defaults to null.
//
//	Tests if a ray collide with the circle.
*/
///////////////////////////////////////////////////////////////////////////////
bool Circle::GetTimeRayIntersect( const Ray2& ray,
									float32 *pTime1,
									float32 *pTime2 ) const
{
	// If there is no ray
	if( ray.dir.GetMagSq() == 0 )
		return false;

	// Calculate the A, B, and C for the quadratic equation
	float32 A = ray.dir.GetMagSq();
	float32 B = 2 * ( (ray.pos - center).Dot( ray.dir ) );
	float32 C = (ray.pos - center).GetMagSq() - (radius * radius);

	// Get the discriminate
	float32 disc = (B * B) - (4 * A * C);

	// If the discriminate is negative there is no collision
	if( disc < 0 )
		return false;

	// Get the square root of the discriminate
	disc = sqrtf( disc );

	// Get the first time values
	float32 timePlus = ( (-B) + disc ) / ( 2 * A );

	// If this number is less than 0 then there is no collision
	if( timePlus < 0.0f )
		return false;

	// Get the second time value
	float32 timeMinus = ( (-B) - disc ) / ( 2 * A );

	// If this time is less than 0 (i.e. invalid ), use the first value
	if( timeMinus < 0.0f )
	{
		// Store the collision time
		if( pTime1 )
			*pTime1 = timePlus;

		if( pTime2 )
			*pTime2 = -1.0f;
	}
	// Else use the second time
	else
	{
		// Store the collision time
		if( pTime1 )
			*pTime1 = timeMinus;

		if( pTime2 )
			*pTime2 = timePlus;
	}

	// The ray intersects
	return true;
}