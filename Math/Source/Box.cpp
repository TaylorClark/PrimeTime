//=============================================================================
/*!
	\file Box.cpp
	Taylor Clark Math Library
	Box Object Class
	\author Taylor Clark

  This source file contains the box object implementation.
*/
//=============================================================================

#include "../Box.h"
#include <Float.h>

template< class T >
T max( T a, T b ){ if( b > a ) return b; return a; }

template< class T >
T min( T a, T b ){ if( b < a ) return b; return a; }


///////////////////////////////////////////////////////////////////////////////
//
//	Box::RayCollides( ) Public
/*!
//	\param rayPos The start position of the ray.
//	\param rayDir The ray's direction
//	\param rIntersectPt The point of intersection if one occurs
//	\param rIntNormal The normal at the point of intersection
//	\param rColor The variable that gets filled in with the color of the
//					object hit
//
//	See if a ray collides with the box
*/
///////////////////////////////////////////////////////////////////////////////
bool Box::RayCollides( const Ray3& ray, float* pTime ) const
{
	// Get ready to test for collision
	CollideSetup();

	Vector3 rayDir( ray.dir );
	Point3 rayPos( ray.pos );

	// The collision times
	float timeMin = 0.0f, timeMax = FLT_MAX, timeInt = 0;

	// Calculate collision times
	int sideIndex = 0;
	int minIntersectionSideIndex = -1;
	int maxIntersectionSideIndex = -1;
	while( (sideIndex < 6) && (timeMin < timeMax) )
	{
		// If the ray is going the opposite direction of the normal
		float dotVal = rayDir.Dot( m_SideNormal[ sideIndex ] );
		if( dotVal < 0 )
		{
			// Get the intersection time
			timeInt = -(rayPos - m_SidePt[sideIndex]).Dot(m_SideNormal[sideIndex])
									/ dotVal;

			timeMin = max( timeMin, timeInt );
			if( timeMin == timeInt )
			{
				// Store the intersection side
				minIntersectionSideIndex = sideIndex;
			}
		}
		// Else if the ray is going the same direction as the normal
		else if( dotVal > 0 )
		{
			// Get the intersection time
			timeInt = -(rayPos - m_SidePt[sideIndex]).Dot(m_SideNormal[sideIndex])
									/ dotVal;
			timeMax = min( timeMax, timeInt );
			if( timeMax == timeInt )
			{
				// Store the intersection side
				maxIntersectionSideIndex = sideIndex;
			}
		}
		// Else if the vector from the side to our ray is going the same
		// direction as the normal (i.e. the ray is not inside)
		else if( (rayPos - m_SidePt[sideIndex]).Dot(m_SideNormal[sideIndex]) > 0 )
		{
			// Make us stop looping
			timeMin = timeMax + 1;
		}

		// Increment to the next side
		++sideIndex;
	}

	// If we don't have valid collision
	if( timeMin > timeMax )
		return false;

	// If the min time is invalid
	if( (timeMin <= 0.0f) && (pTime) )
		*pTime = timeMax;
	else if( pTime )
		*pTime = timeMin;

	// Return collision
	return true;
}


///////////////////////////////////////////////////////////////////////////////
/*
//	Box::PreRenderSetup( ) Public
//
//	Calculate anything that can be calculated before render time.
*/
///////////////////////////////////////////////////////////////////////////////
void Box::CollideSetup( void ) const
{
	// Generate the normals and side points
	m_SidePt[0] = m_CornerPoint;
	m_SideNormal[0] = m_Length.Cross( m_Height );
	m_SidePt[1] = m_CornerPoint + m_Width;
	m_SideNormal[1] = -m_SideNormal[0];

	m_SidePt[2] = m_CornerPoint;
	m_SideNormal[2] = m_Height.Cross( m_Width );
	m_SidePt[3] = m_CornerPoint + m_Length;
	m_SideNormal[3] = -m_SideNormal[2];

	m_SidePt[4] = m_CornerPoint;
	m_SideNormal[4] = m_Width.Cross( m_Length );
	m_SidePt[5] = m_CornerPoint + m_Height;
	m_SideNormal[5] = -m_SideNormal[4];
}
	