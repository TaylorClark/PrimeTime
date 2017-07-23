//=============================================================================
/*!
	\file ClosedPolygon.h
	Taylor Clark Math Library
	ClosedPolygon Object
	\author Taylor Clark

  This header contains the class declaration for the polygon class
*/
//=============================================================================

#pragma once

#ifndef __ClosedPolygon_h
#define __ClosedPolygon_h

#include "Ray2.h"
#include "Circle.h"
#include "Matrix3x3.h"


/*=============================================================================

  Class Definitions

=============================================================================*/

//-----------------------------------------------------------------------------
/*! 
	\class ClosedPolygon
	\brief This is a polygon class

	This class encapsulates the functionality of a polygon.
*/
//-----------------------------------------------------------------------------
class ClosedPolygon
{
	// Private so they can't be used right now
	ClosedPolygon( const ClosedPolygon& )
	{}

public:
	//! The contstructors
	ClosedPolygon() : m_NumEdges(0),
						m_pEdges(0)
	{}
	
	// The assigment operator
	const ClosedPolygon& operator = ( const ClosedPolygon& rhs );

	//! The destructor
	~ClosedPolygon()
	{
		if( m_pEdges )
			delete [] m_pEdges;
	}
	
	//! Set the values
	void SetEdges( const Ray2 *pEdges, int numEdges );
	void SetVertices( const Point2 *pVertices, int numVertices );

	//! Is a point inside the polygon
	bool IsPointInside( const Point2& pt ) const;

	//! Does a ray collide with the circle
	bool DoesRayIntersect( const Ray2& ray,
							float32 *pIntTime = 0,
							Ray2* pIntNorm = 0 ) const;

	//! Test for collision with another circle
	bool Intersects( const ClosedPolygon& testPoly ) const;

	//! Transform the edges
	void Transform( const Matrix3x3& transMat );

	//! Clear the data
	void Clear( void );

	//! Get and set an edge
	Ray2 GetEdge( int edgeIndex ) const
	{
		if( (edgeIndex < 0) || (edgeIndex >= m_NumEdges) )
			return Ray2();
		return m_pEdges[ edgeIndex ];
	}
	
	//! Get the number of edges
	int GetNumEdges( void ) const { return m_NumEdges; }

	//! Get the edge data
	const Ray2* GetEdges( void ) const { return m_pEdges; }

	//! Get the bounding circle
	Circle GetBoundingCircle( void ) const { return m_BoundCircle; }

	//! Get the closest point on the polygon to a point
	Point2 GetClosestPointOnPoly( const Point2& testPt ) const;

private:

	//! Does a ray intersect without bounding circle intersection
	bool DoesRayIntersectNoCircle( const Ray2& ray,
									float32 *pIntTime = 0,
									Ray2* pIntNorm = 0 ) const;
	
	//! Calculate the bounding circle for the polygon
	void GenerateBoundingCircle( void );

	//! The polygon edges
	Ray2* m_pEdges;

	//! The number of edges
	int m_NumEdges;

	//! The bounding circle
	Circle m_BoundCircle;
};

#endif	//__ClosedPolygon_h