//=============================================================================
/*!
	\file ClosedPolygon.cpp
	Taylor Clark Math Library
	ClosedPolygon Object
	\author Taylor Clark

  This source file contains the polygon object implementation.
*/
//=============================================================================

#include "../Polygon.h"
#include <list>


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::operator =() Public
/*!
//	\param rhs The polygon data to be equal to
//
//	Copy a polygon.
*/
///////////////////////////////////////////////////////////////////////////////
const ClosedPolygon& ClosedPolygon::operator = ( const ClosedPolygon& rhs )
{
	// Set our data
	SetEdges( rhs.GetEdges(), rhs.GetNumEdges() );

	// Return ourself
	return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::SetEdges( ) Public
///
///	\param pEdges The edges data to copy
///	\param numEdges The number of edges
///
///	Set the polygon edge data.
///
///////////////////////////////////////////////////////////////////////////////
void ClosedPolygon::SetEdges( const Ray2* pEdges, int32 numEdges )
{
	// If we have an invalid number of edges
	if( numEdges < 1 )
		return;

	// If we don't have the same number of edges currently allocated
	if( m_NumEdges != numEdges )
	{
		// Free our memory
		if( m_pEdges )
			delete [] m_pEdges;

		// Store the number of edges
		m_NumEdges = numEdges;

		// Allocate the new memory
		m_pEdges = new Ray2[ m_NumEdges ];
		//m_pEdges = (Ray2*)malloc( sizeof(Ray2) * m_NumEdges );
	}

    // Store the data
	for( int32 edgeIndex = 0; edgeIndex < m_NumEdges; ++edgeIndex )
		m_pEdges[edgeIndex] = pEdges[edgeIndex];

	// Generate the bounding circle
	GenerateBoundingCircle();
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::SetPoints( ) Public
///
///	\param pEdges The points that represent the vertices of the polygon
///	\param numEdges The number of vertices
///
///	Set the polygon edge data.
///
///////////////////////////////////////////////////////////////////////////////
void ClosedPolygon::SetVertices( const Point2 *pVertices, int32 numVertices )
{
	// If we have an invalid number of vertices
	if( numVertices < 2 )
		return;

	// If we don't have the same number of edges currently allocated
	if( m_NumEdges != numVertices )
	{
		// Free our memory
		if( m_pEdges )
			delete [] m_pEdges;

		// Store the number of edges
		m_NumEdges = numVertices;

		// Allocate the new memory
		m_pEdges = new Ray2[ m_NumEdges ];
		//m_pEdges = (Ray2*)malloc( sizeof(Ray2) * m_NumEdges );
	}

    // Set the edges
	int32 edgeIndex = 0;
	for( ; edgeIndex < numVertices - 1; ++edgeIndex )
	{
		m_pEdges[edgeIndex].pos = pVertices[edgeIndex];
		m_pEdges[edgeIndex].dir = pVertices[edgeIndex+1] - pVertices[edgeIndex];
	}

	// Set the last edge
	m_pEdges[edgeIndex].pos = pVertices[edgeIndex];
	m_pEdges[edgeIndex].dir = pVertices[0] - pVertices[edgeIndex];

	// Generate the bounding circle
	GenerateBoundingCircle();
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::IsPointInside( ) Public
/*!
//	\param pt The point to test
//
//	Is a point inside the polygon.
*/
///////////////////////////////////////////////////////////////////////////////
bool ClosedPolygon::IsPointInside( const Point2& pt ) const
{
	// If the point is outside the bounding circle
	if( !m_BoundCircle.IsPointInside(pt) )
		return false;

	// Create a ray, 3 times the radius for coverage, and see how many times
	// it intersects the collision edges
	Ray2 testRay( pt, Vector2(m_BoundCircle.radius * 3, 0) );

	// Go through each edge and ensure the point exists outside
	int32 intCount = 0;
	for( int32 edgeIndex = 0; edgeIndex < GetNumEdges(); ++edgeIndex )
	{
		// Test for intersection
		if( testRay.Intersects(GetEdge(edgeIndex)) )
			++intCount;
	}

	// If our number of collisions is odd, it is inside
	if( intCount & 1 )
		return true;

	// It is outside
	return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::DoesRayIntersect( ) Public
/*!
//	\param ray The ray to test for intersection
//	\param pTime A pointer if the time of intersection should be stored
//	\param pIntNorm The normal if you should be stored
//
//	Does a ray intersect the polygon.
*/
///////////////////////////////////////////////////////////////////////////////
bool ClosedPolygon::DoesRayIntersect( const Ray2& ray,
								float32 *pIntTime,
								Ray2* pIntNorm ) const
{
	// If we have less than 5 edges, it is more efficient to skip the circle
	// test
	if( GetNumEdges() > 4 )
	{
		// Construct a bound circle for the ray
		Point2 rayMidPt(	ray.pos.x + (ray.dir.x * 0.5f),
							ray.pos.y + (ray.dir.y * 0.5f) );

		Circle rayCircle( rayMidPt, ray.dir.GetMag() * 0.5f );
		//Circle rayCircle( rayMidPt, ray.dir.GetMagSq() * 0.5f );

		// Test for bound circle collision with the ray
		if( !m_BoundCircle.Intersects( rayCircle ) )
			return false;
	}

	// Does the ray intersect
	return DoesRayIntersectNoCircle( ray, pIntTime, pIntNorm );
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::Intersects( ) Public
/*!
//	\param ray The ray to test for intersection
//	\param pTime
//
//	Test for intersection with another polygon.
*/
///////////////////////////////////////////////////////////////////////////////
bool ClosedPolygon::Intersects( const ClosedPolygon& testPoly ) const
{
	// If the bounding circles don't overlap, then they won't intersect
	if( !m_BoundCircle.Intersects(testPoly.m_BoundCircle) )
		return false;

	// Go through all of this polygon's edges
	for( int32 edgeIndex = 0; edgeIndex < GetNumEdges(); ++edgeIndex )
	{
		// See if this edge intersects
		if( testPoly.DoesRayIntersectNoCircle(m_pEdges[edgeIndex]) )
		{
			// Return intersection
			return true;
		}
	}

	// Return no intersection
	return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::Transform( ) Public
/*!
//	Apply a transformation to the edges.
*/
///////////////////////////////////////////////////////////////////////////////
void ClosedPolygon::Transform( const Matrix3x3& transMat )
{
	// Go through all of the edges
	for( int32 edgeIndex = 0; edgeIndex < GetNumEdges(); ++edgeIndex )
	{
		// Transform the edge
		m_pEdges[edgeIndex].pos = transMat.Compose( m_pEdges[edgeIndex].pos );
		m_pEdges[edgeIndex].dir = transMat.Compose( m_pEdges[edgeIndex].dir );
	}

	// Generate the new bounding circle
	GenerateBoundingCircle();
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::Clear( ) Public
/*!
//	Clear the polygon data.
*/
///////////////////////////////////////////////////////////////////////////////
void ClosedPolygon::Clear( void )
{
	// If we have edge data
	if( m_pEdges )
		delete [] m_pEdges;
	m_pEdges = 0;

	// Clear data
	m_NumEdges = 0;
	m_BoundCircle.Set( Point2(0,0), 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::DoesRayIntersectNoCircle( ) Private
/*!
//	Does a ray intersect the polygon without bounding circle testing.
*/
///////////////////////////////////////////////////////////////////////////////
bool ClosedPolygon::DoesRayIntersectNoCircle( const Ray2& ray,
										float32 *pIntTime,
										Ray2* pIntNorm ) const
{
	// Go through each edge and see if the ray intersect
	float32 minTime = 2.0f;
	for( int32 edgeIndex = 0; edgeIndex < GetNumEdges(); ++edgeIndex )
	{
		// If this edge intersects
		if( ray.Intersects(m_pEdges[edgeIndex]) )
		{
			// Get the time of intersection with this edges
			float32 intTime = ray.GetTimeIntersectWith( m_pEdges[edgeIndex] );

			// If this is a smaller time
			if( intTime < minTime )
			{
				// Store the time
				minTime = intTime;

				// If we are storing a normal
				if( pIntNorm )
				{
					pIntNorm->pos = ray.GetPoint( minTime );
					pIntNorm->dir = m_pEdges[edgeIndex].dir.GetOrtho90();
				}
			}
		}
	}

	// If we have a time to store
	if( pIntTime )
		*pIntTime = minTime;

	// Return no intersection
	if( minTime >= 1.0f )
		return false;

	// If we need to normalize the normal
	if( pIntNorm )
		pIntNorm->dir.Normalize();

	// Return intersection
	return true;
}

	
///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::GenerateBoundingCircle( ) Private
/*!
//	Generate a circle to bound the polygon.
*/
///////////////////////////////////////////////////////////////////////////////
void ClosedPolygon::GenerateBoundingCircle( void )
{
	if( !m_pEdges )
		return;

	// Go through all of the edges for find the extents
	Point2 minPt( m_pEdges[0].pos ),
			maxPt( m_pEdges[0].pos );
	for( int32 edgeIndex = 0;
			edgeIndex < m_NumEdges;
			++edgeIndex )
	{
		// Test point values
		if( m_pEdges[edgeIndex].pos.x < minPt.x )
			minPt.x = m_pEdges[edgeIndex].pos.x;
		if( m_pEdges[edgeIndex].pos.x > maxPt.x )
			maxPt.x = m_pEdges[edgeIndex].pos.x;

		if( m_pEdges[edgeIndex].pos.y < minPt.y )
			minPt.y = m_pEdges[edgeIndex].pos.y;
		if( m_pEdges[edgeIndex].pos.y > maxPt.y )
			maxPt.y = m_pEdges[edgeIndex].pos.y;

		// Get the edge end point
		Point2 endPt( m_pEdges[edgeIndex].pos + m_pEdges[edgeIndex].dir );
		
		// If we have new extents
		if( endPt.x < minPt.x )
			minPt.x = endPt.x;
		if( endPt.x > maxPt.x )
			maxPt.x = endPt.x;

		if( endPt.y < minPt.y )
			minPt.y = endPt.y;
		if( endPt.y > maxPt.y )
			maxPt.y = endPt.y;
	}

	// Get the middle of the min and max
	Point2 centerPt( (minPt.x + maxPt.x) / 2.0f, (minPt.y + maxPt.y) / 2.0f );
	
	// Set the position
	m_BoundCircle.center = centerPt;
	
	// Go through all of the edges to find the greatest distance from the
	// center
	float32 curMaxDistSq = 0.0f;
	for( int32 edgeIndex = 0; edgeIndex < m_NumEdges; ++edgeIndex )
	{
		// Get the distance to this corner
		Vector2 distVect( m_pEdges[edgeIndex].pos - centerPt );

		// If this is a new max
		if( distVect.GetMagSq() > curMaxDistSq )
			curMaxDistSq = distVect.GetMagSq();

		// Get the end point
		Point2 endPt( m_pEdges[edgeIndex].pos + m_pEdges[edgeIndex].dir );
		distVect = endPt - centerPt;
		
		// If this is a new max
		if( distVect.GetMagSq() > curMaxDistSq )
			curMaxDistSq = distVect.GetMagSq();
	}

	// Set the radius
	m_BoundCircle.radius = sqrtf( curMaxDistSq );
}


///////////////////////////////////////////////////////////////////////////////
//
//	ClosedPolygon::GetClosestPointOnPoly( ) Private
/*!
//	\param testPt The world point to test to find the closest point on this
//					polygon
//
//	Get the closest point on the polygon to a point.
*/
///////////////////////////////////////////////////////////////////////////////
Point2 ClosedPolygon::GetClosestPointOnPoly( const Point2& testPt ) const
{
	Point2 retPt;

	// If we have no edges, bail
	if( m_NumEdges < 1 )
		return retPt;

	// Go through the edges
	retPt = m_pEdges[0].pos;
	float32 minDistSq = (testPt - retPt).GetMagSq();
	for( int32 edgeIndex = 0; edgeIndex < m_NumEdges; ++edgeIndex )
	{
		// Project the point onto this edge
		Vector2 toPt( testPt - m_pEdges[edgeIndex].pos );
		float32 scalar = toPt.Dot( m_pEdges[edgeIndex].dir );
		scalar /= m_pEdges[edgeIndex].dir.Dot( m_pEdges[edgeIndex].dir );

		// Clamp the scalar
		if( scalar < 0.0f )
			scalar = 0.0f;
		else if( scalar > 1.0f )
			scalar = 1.0f;

		// Is this point closer than our current closest
		toPt = testPt - m_pEdges[edgeIndex].GetPoint(scalar);
		if( toPt.GetMagSq() < minDistSq )
		{
			// Store the new closer point
			minDistSq = toPt.GetMagSq();
			retPt = m_pEdges[edgeIndex].GetPoint(scalar);
		}

	}

	// Return the point
	return retPt;
}
