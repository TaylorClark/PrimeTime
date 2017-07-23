//=============================================================================
/*!
	\file Interval.h
	Math Library
	Interval
	\author Taylor Clark

  This header contains the declaration for the interval class and its related
  funtions.  Due to MSVC not supporting the definition of templatized class
  funtions outside the declaration, this header is bloated.
*/
//=============================================================================

#pragma once

#ifndef __Interval_h
#define __Interval_h

#include "Base/Types.h"


/*=============================================================================

  Class Definitions

=============================================================================*/

//-----------------------------------------------------------------------------
/*! 
	\class Interval
	\brief This is interval class.

	This class encapsulates the functionality of a mathematical interval.  An
	interval is a subset of the real line and contains all real numbers lying
	between its boundary points, or endpoints.  An interval is said to be
	closed if it includes its boundary points, half-open if it contains one
	boundary point, and open if it does not include either boundary point.
*/
//-----------------------------------------------------------------------------
namespace TCBase
{
template< class T >
class Interval
{
private:
	
	Interval() : m_IsMinClosed( false ),
					m_IsMaxClosed( false ),
					m_IsMinNegInfinite( false ),
					m_IsMaxInfinite( false )
	{}

	// Defined here so there is no need to include algorithm
	static void swap( T &left, T &right )
	{
		T temp = left;
		left = right;
		right = temp;
	}

public:

	//! Create an interval with both boundary points closed.  The min and
	//! max will be sorted automatically.
	static Interval<T> MakeClosed( const T &val1, const T &val2 )
	{
		Interval<T> retInterval;

		// Set the closed values
		retInterval.m_IsMinClosed = true;
		retInterval.m_IsMaxClosed = true;

		// Store the values
		retInterval.m_Min = val1;
		retInterval.m_Max = val2;

		// Ensure the values are in the right order
		if( retInterval.m_Min > retInterval.m_Max )
			swap( retInterval.m_Min, retInterval.m_Max );

		return retInterval;
	}

	//! Create an interval that has one closed boundary point and one open boundary
	//! point.  It will automatically sort the min and max.
	static Interval<T> MakeHalfOpen( const T &closedEndpoint, const T &openEndpoint )
	{
		Interval<T> retInterval;

		// If the closed endpoint is the min
		if( closedEndpoint < openEndpoint )
		{
			// Set the min to be closed
			retInterval.m_IsMinClosed = true;
			
			// Store the values
			retInterval.m_Min = closedEndpoint;
			retInterval.m_Max = openEndpoint;
		}
		// Else the closed endpoint is the max
		else
		{
			// Set the max to be closed
			retInterval.m_IsMaxClosed = true;
			
			// Store the values
			retInterval.m_Min = openEndpoint;
			retInterval.m_Max = closedEndpoint;
		}

		return retInterval;
	}

	//! Create an interval that includes neither boundary point.
	static Interval<T> MakeOpen( const T &val1, const T &val2 )
	{
		Interval<T> retInterval;

		// Store the values
		retInterval.m_Min = val1;
		retInterval.m_Max = val2;

		// Ensure the values are in the right order
		if( retInterval.m_Min > retInterval.m_Max )
			swap( retInterval.m_Min, retInterval.m_Max );

		return retInterval;
	}

	//! Create an interval with an open enpoint and infinite endpoint
	static Interval<T> MakeOpenInfinite( const T &openVal, bool isNegInfinite )
	{
		Interval<T> retInterval;

		// Store the values, we can store the value in both since setting an
		// endpoint to infinite will ignore the value
		retInterval.m_Min = openVal;
		retInterval.m_Max = openVal;
		
		// Set the infinite flag
		if( isNegInfinite )
			retInterval.m_IsMinNegInfinite = true;
		else
			retInterval.m_IsMaxInfinite = true;

		return retInterval;
	}

	//! Create an interval with a closed enpoint and infinite endpoint
	static Interval<T> MakeClosedInfinite( const T &closedVal, bool isNegInfinite )
	{
		Interval<T> retInterval;

		// Store the values, we can store the value in both since setting an
		// endpoint to infinite will ignore the value
		retInterval.m_Min = closedVal;
		retInterval.m_Max = closedVal;
		
		// Set the infinite flag
		if( isNegInfinite )
		{
			retInterval.m_IsMinNegInfinite = true;
			retInterval.m_IsMaxClosed = true;
		}
		else
		{
			retInterval.m_IsMaxInfinite = true;
			retInterval.m_IsMinClosed = true;
		}

		return retInterval;
	}

	//! Does the interval contain a value
	bool IsValueContained( const T &value ) const
	{
		bool satisfiesMin = false, satisfiesMax = false;

		// If the min is infinite then we are greater than the min
		if( m_IsMinNegInfinite )
			satisfiesMin = true;
		// Else if we are greater than the min
		else if( value > m_Min )
			satisfiesMin = true;
		// Else if we include the min value
		else if( m_IsMinClosed && value >= m_Min )
			satisfiesMin = true;

		// If the max is infinite then we are less than the max
		if( m_IsMaxInfinite )
			satisfiesMax = true;
		// Else if we are less than the max
		else if( value < m_Max )
			satisfiesMax = true;
		// Else if we include the max value
		else if( m_IsMaxClosed && value <= m_Max )
			satisfiesMax = true;

		// Return is we are contained
		return satisfiesMin && satisfiesMax;
	}

	//! Does this interval intersect another
	bool IsIntervalIntersecting( const Interval<T> &interval ) const
	{
		// Run the internal versions
		return IsIntervalIntersectingInternal( interval ) || interval.IsIntervalIntersectingInternal( *this );
	}

private:

	bool IsIntervalIntersectingInternal( const Interval<T> &interval ) const
	{
		return IsValueContained( interval.m_Min ) || IsValueContained( interval.m_Max );
	}

	//! The range
	T m_Min, m_Max;

	//! If the boundary points are closed
	bool m_IsMinClosed, m_IsMaxClosed;

	//! If the min or max is infinite
	bool m_IsMinNegInfinite, m_IsMaxInfinite;
};
}

// Common intervals
typedef TCBase::Interval<int32> IntervalInt;
typedef TCBase::Interval<float32> IntervalFloat;

#endif	//__Interval_h