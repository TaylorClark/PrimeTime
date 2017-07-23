/*=================================================================================================

	\file NumFuncs.cpp
	Base Library
	Number Functions Source
	\author Taylor Clark
	\Date May 30, 2006

	This source file contains the implementation of the number utility functions.

=================================================================================================*/

#include "../NumFuncs.h"
#include <math.h>
#include "../StringFuncs.h"
#include <stdlib.h>


/// Determine if a number is prime
bool TCBase::IsPrime( int32 num )
{
	// If the number is negative then bail
	if( num < 2 )
		return false;

	// If the number is 2, the only even prime, then return success
	if( num == 2 )
		return true;

	// If the number is even and is not 2 then bail
	if( (num % 2) == 0 )
		return false;

	// If the number is less than 35 then use a 32-bit bit field
	if( num < 35 )
	{
		// The number 0x14114515 contains 1's for prime indices with bit 0 being the prime 3
		if( 0x14114515 & (1 << (num - 3)) )
			return true;
		return false;
	}

	// Finally, use the slow long method to test for factors
	for( int32 factor = 3; factor < num / 2; ++factor )
	{
		// If this factor divides into the number then the number is not prime
		if( num % factor == 0 )
			return false;
	}

	// The number has no factors besides 1 and itself so return success
	return true;
}

/// Get the left most 1 bit in a number (The most significant 1)
int TCBase::GetLeftMost1Index( int num )
{
	int bitPos = 32;
	while( --bitPos >= 0 )
	{
		if( num & (1 << bitPos) )
			return bitPos;
	}

	return -1;
}

/// Find the right most one in an integer
int32 TCBase::GetRightMost1Index( int32 num )
{
	int bitPos = 0;
	do
	{
		if( num & (1 << bitPos) )
			return bitPos;
	}while( ++bitPos < 32 );

	return -1;
}

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


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetNumOfChars()  Global
///
///	\param num The number to test
///	\returns The number of characters needed to display the passed-in number.
///
///	Get the number of characters needed to display a number.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 TCBase::GetNumOfChars( int32 num )
{
	int32 charCount = 0;

	// If the number is negative
	if( num < 0 )
	{
		// Add a character for the minus sign and make the value positive
		charCount++;
		num = 0 - num;
	}

	// Get the basic count
	if( num < 10 )
		charCount += 1;
	else if( num < 100 )
		charCount += 2;
	else if( num < 1000 )
		charCount += 3;
	else
		charCount += (int32)log( (double)num );

	// Return the character count
	return charCount;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EasyIToA()  Global
///
///	\param num The number to convert
///	\returns A pointer to a static buffer that contains the number in string form
///
///	Convert a number to a string.  A helper function that is not thread-safe.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* TCBase::EasyIToA( int32 num )
{
	const uint32 BUFFER_SIZE = 16;
	static wchar_t s_szNumBuffer[ BUFFER_SIZE ] ={0};

	// Print the number to the buffer
	swprintf( s_szNumBuffer, BUFFER_SIZE, L"%d", num );

	// Return the string
	return s_szNumBuffer;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EasyUIToAHex  Global
///
///	\param num The number to convert
///	\returns A pointer to a static buffer that contains the number in string form
///
///	Convert a number to a hexidecimal string.  A helper function that is not thread-safe.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* TCBase::EasyUIToAHex( uint32 num )
{
	const uint32 BUFFER_SIZE = 18;
	static wchar_t s_szNumBuffer[ BUFFER_SIZE ] ={0};

	// Print the number to the buffer
	swprintf( s_szNumBuffer, BUFFER_SIZE, L"0x%04X", num );

	// Return the string
	return s_szNumBuffer;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EasyUIToA()  Global
///
///	\param num The unsigned number to convert
///	\returns A pointer to a static buffer that contains the number in string form
///
///	Convert an unsigned number to a string.  A helper function that is not thread-safe.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* TCBase::EasyUIToA( uint32 num )
{
	// 2^32 = 4294967296 is only 10 characters long
	const uint32 BUFFER_SIZE = 16;
	static wchar_t s_szNumBuffer[ BUFFER_SIZE ] ={0};

	// Print the number to the buffer
	swprintf( s_szNumBuffer, BUFFER_SIZE, L"%u", num );

	// Return the string
	return s_szNumBuffer;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EasyUI64ToA()  Global
///
///	\param num The unsigned 64-bit number to convert
///	\returns A pointer to a static buffer that contains the number in string form
///
///	Convert an unsigned number to a string.  A helper function that is not thread-safe.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* TCBase::EasyUI64ToA( uint64 num )
{
	// 2^64 = 18446744073709551616, is 20 characters long
	const uint32 BUFFER_SIZE = 32;
	static wchar_t s_szNumBuffer[ BUFFER_SIZE ] ={0};

	// Print the number to the buffer
#ifdef WIN32
	swprintf( s_szNumBuffer, BUFFER_SIZE, L"%I64u", num );
#else
	swprintf( s_szNumBuffer, BUFFER_SIZE, L"%llu", num );
#endif
	
	// Return the string
	return s_szNumBuffer;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EasyFToA()  Global
///
///	\param num The floating-point number to convert
///	\param maxSigDigits The maximum number of significant digits to display, negative for as many
///						as possible. Defaults to -1
///	\returns A pointer to a static buffer that contains the number in string form
///
///	Convert a floating-point number to a string.  A helper function that is not thread-safe.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* TCBase::EasyFToA( float32 num, int32 maxSigDigits )
{
	// If you update the buffer size then update the swprintf_s below
	const uint32 BUFFER_SIZE = 32;
	static wchar_t s_szNumBuffer[ BUFFER_SIZE ] ={0};

	// Print the number to the buffer
	if( maxSigDigits < 0 || maxSigDigits > 16 )
		swprintf( s_szNumBuffer, BUFFER_SIZE, L"%.16f", num );
	else
	{
		// Create the format string based on the input number of significant digits
		std::wstring sFormat = L"%.";
		sFormat += EasyIToA( maxSigDigits );
		sFormat += L"f";

		// Create the output string
		swprintf( s_szNumBuffer, BUFFER_SIZE, sFormat.c_str(), num );
	}

	// Return the string
	return s_szNumBuffer;
}


int32 TCBase::WToI( const wchar_t* szNumberString )
{
#ifdef WIN32
	return _wtoi( szNumberString );
#else
	return atoi( TCBase::Narrow(szNumberString).c_str() );
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EasyDurationToA()  Global
///
///	\param numMillis The milliseconds to convert
///	\param showHundredths Setting this to true will concatenate the hundredths of seconds to the
///							output string
///	\returns A pointer to a static buffer that contains the duration in string form, 0:00:00:00
///
/// Convert a duration to a string
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* TCBase::EasyDurationToA( uint32 numMillis, bool showHundredths )
{
	// 0:00:00.00 is only 10 characters long
	const uint32 BUFFER_SIZE = 16;
	static wchar_t s_szTimeBuffer[ BUFFER_SIZE ] = {0};

	uint32 hours = ((numMillis / 1000) / 60) / 60;
	uint32 minutes = ((numMillis / 1000) / 60) % 60;
	uint32 secs = (numMillis / 1000) % 60;

	if( showHundredths )
	{
		uint32 hundredths = (numMillis % 1000) / 10;
		swprintf( (wchar_t*)s_szTimeBuffer, BUFFER_SIZE, L"%u:%02u:%02u.%02u", hours, minutes, secs, hundredths );
	}
	else
		swprintf( (wchar_t*)s_szTimeBuffer, BUFFER_SIZE, L"%u:%02u:%02u", hours, minutes, secs );

	return s_szTimeBuffer;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EasyDurationToA()  Global
///
///	\param numSeconds The seconds to convert
///	\param showHundredths Setting this to true will concatenate the hundredths of seconds to the
///							output string
///	\returns A pointer to a static buffer that contains the duration in string form, 0:00:00:00
///
/// Convert a duration to a string
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* TCBase::EasyDurationToA( float32 numSeconds, bool showHundredths )
{
	return EasyDurationToA( (uint32)((numSeconds * 1000.0f) + 0.5f), showHundredths );
}