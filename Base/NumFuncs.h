//=================================================================================================
/*!
	\file NumFuncs.h
	Base Library
	Number Functions Header
	\author Taylor Clark
	\date May 30, 2006

	This header contains the declarations for the number utility functions.
*/
//=================================================================================================

#pragma once
#ifndef __NumFuncs_h
#define __NumFuncs_h

#include "Types.h"
#include <string>


namespace TCBase
{
	/// Find the left most one in an integer
	int32 GetLeftMost1Index( int32 num );

	/// Find the right most one in an integer
	int32 GetRightMost1Index( int32 num );

	/// Test two floats for 'nearly' equals
	template< class T >
	bool NearlyEquals( T num1, T num2, T range )
	{
		// Get the absolute value of the difference of the two numbers
		T diff = num1 - num2;
		if( diff < (T)0)
			diff = -diff;

		// If the difference is within our range then return true
		if( diff < range )
			return true;

		return false;
	}

	/// Get the number of characters needed to display a number
	int32 GetNumOfChars( int32 num );

	/// Convert a number to a string
	const wchar_t* EasyIToA( int32 num );

	/// Convert a number to a string
	const wchar_t* EasyUIToA( uint32 num );

	/// Convert a number to a hexidecimal string
	const wchar_t* EasyUIToAHex( uint32 num );

	/// Convert a number to a string
	const wchar_t* EasyUI64ToA( uint64 num );

	/// Convert a floating-point number to a string
	const wchar_t* EasyFToA( float32 num, int32 maxSigDigits = -1 );

	/// Convert a string to an integer
	int32 WToI( const wchar_t* szNumberString );

	/// Convert a duration to a string
	const wchar_t* EasyDurationToA( uint32 numMillis, bool showHundredths );

	/// Convert a duration to a string
	const wchar_t* EasyDurationToA( float32 numSeconds, bool showHundredths );

	/// Determine if a number is prime
	bool IsPrime( int32 num );
};

#endif // __NumFuncs_h