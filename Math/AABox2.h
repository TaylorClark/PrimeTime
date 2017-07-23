//=============================================================================
/*!
	\file AABox2.h
	Taylor Clark Math Library
	2-Dimensional Axis Aligned Box
	\author Taylor Clark

  This header contains the class declaration for a 2-D box that is aligned to
  2 axes.
*/
//=============================================================================

#pragma once

#ifndef __AABox2_h
#define __AABox2_h

#include "Base/Types.h"
#include "libTCMath/Point2.h"


/*=============================================================================

  Class Definitions

=============================================================================*/


//-----------------------------------------------------------------------------
/*! 
	\class AABox2
	\brief This is the 2D axis aligned box.

	This class encapsulates the functionality of a axis aligned 2-D box.
*/
//-----------------------------------------------------------------------------
class AABox2
{
public:
	//! The contstructor
	AABox2(){ }
	AABox2( const AABox2& copy )
	{
		m_Min = copy.m_Min; m_Max = copy.m_Max;
		CalcWorkingPoints();
	}
	AABox2( const Point2& min, const Point2& max ) : m_Min( min ), m_Max( max )
	{
	CalcWorkingPoints();
	}
	const AABox2& operator = ( const AABox2& rhs )
	{
		m_Min = rhs.m_Min; m_Max = rhs.m_Max;
		CalcWorkingPoints();
		return *this;
	}
	
	//! Is a point within the box
	bool IsPointInside( const Point2& pt ) const
	{
		// Test the x component
		if( (pt.x < m_WorkingMin.x) || (pt.x > m_WorkingMax.x) )
			return false;

		// Test the y component
		if( (pt.y < m_WorkingMin.y) || (pt.y > m_WorkingMax.y) )
			return false;

		// The point is inside
		return true;
	}

	//! Get the X and Y
	float32 GetX() const { return m_WorkingMin.x; }
	float32 GetY() const { return m_WorkingMin.y; }

	//! Get the width and height
	float32 GetWidth() const { return m_WorkingMax.x - m_WorkingMin.x; }
	float32 GetHeight() const { return m_WorkingMax.y - m_WorkingMin.y; }

	//! Set the points, ensures values are in order
	void SetMin( const Point2& newMin )
	{
		m_Min = newMin;
		CalcWorkingPoints();
	}
	void SetMax( const Point2& newMax )
	{
		m_Max = newMax;
		CalcWorkingPoints();
	}
	Point2 GetMin( void ) const { return m_Min; }
	Point2 GetMax( void ) const { return m_Max; }

private:
	
	//! Get the working min and max for the bound point
	//! We need working points since the user can specify the min and max
	//! points, but they may not be in the right order for testing if a point
	//! is contained in the box.  I keep the original user-specified points
	//! in case the user sets a new min/max point and the points may change
	//! order.
	void CalcWorkingPoints( void )
	{
		// Start at the min
		m_WorkingMin = m_Min;
		m_WorkingMax = m_Max;

		// If the X values are off
		if( m_WorkingMin.x > m_WorkingMax.x )
		{
			float32 temp = m_WorkingMax.x;
			m_WorkingMax.x = m_WorkingMin.x;
			m_WorkingMin.x = temp;
		}

		// If the Y values are off
		if( m_WorkingMin.y > m_WorkingMax.y )
		{
			float32 temp = m_WorkingMax.y;
			m_WorkingMax.y = m_WorkingMin.y;
			m_WorkingMin.y = temp;
		}
	}

	//! The minimum point of the box
	Point2 m_Min;

	//! The maximum point of the box
	Point2 m_Max;

	//! The working min and max for bounding test
	Point2 m_WorkingMin, m_WorkingMax;
};

#endif	//__AABox2_h