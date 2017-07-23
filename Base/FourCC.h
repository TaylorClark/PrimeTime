//=================================================================================================
/*!
	\file FourCC.h
	Base Library
	Four Character Code Object Header
	\author Taylor Clark
	\date March 3, 2006

	This header contains the definition for the FourCC object class.
*/
//=================================================================================================

#pragma once
#ifndef __FourCC_h
#define __FourCC_h

#include "Types.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class FourCC
	\brief Represents a 32-bit integer that can be used as a sequence of 4 1-byte characters
			easily.
*/
//-------------------------------------------------------------------------------------------------
class FourCC
{
private:

	/// The fourCC value
	int32 m_FourCCVal;

public:

	/// The default constructor
	FourCC() : m_FourCCVal(0)
	{
	}

	/// The constructor from a 32-bit value
	explicit FourCC( int32 val ) : m_FourCCVal( val )
	{
	}

	/// The constructor from 4 characters
	FourCC( char a, char b, char c, char d )
	{
		m_FourCCVal = (d << 24) | (c << 16) | (b << 8) | a;
	}

	/// The constructor from 4 characters
	FourCC( char chars[4] )
	{
		m_FourCCVal = (chars[3] << 24) | (chars[2] << 16) | (chars[1] << 8) | chars[0];
	}

	/// The constructor from a string
	FourCC( const char* pStr )
	{
		FromString( pStr );
	}

	/// Set the FourCC from a 32-bit integer
	void FromInt32( int32 val )
	{
		m_FourCCVal = val;
	}

	/// Set the FourCC from a string
	void FromString( const char* pStr )
	{
		// If we have a NULL string then clear the value
		if( pStr == 0 )
		{
			m_FourCCVal = 0;
			return;
		}

		// Store the first character no matter what
		m_FourCCVal = (int)pStr[0];

		// If the first character wasn't the terminating 0, store the next character and so on
		if( pStr[0] != 0 )
			m_FourCCVal |= (pStr[1] << 8);
		if( pStr[1] != 0 )
			m_FourCCVal |= (pStr[2] << 16);
		if( pStr[2] != 0 )
			m_FourCCVal |= (pStr[3] << 24);
	}

	/// Compare 2 FourCCs
	bool operator == ( const FourCC& rhs ) const
	{
		return m_FourCCVal == rhs.m_FourCCVal;
	}

	/// Test 2 FourCCs for inequality
	bool operator != ( const FourCC& rhs ) const
	{
		return m_FourCCVal != rhs.m_FourCCVal;
	}

	/// Compare 2 FourCCs
	bool operator < (const FourCC& rhs ) const
	{
		return m_FourCCVal < rhs.m_FourCCVal;
	}

	/// Get the value in a 32-bit integer
	int32 ToInt32() const
	{
		return m_FourCCVal;
	}

	/// Get the FourCC in a static temporary string for reading
	const char* GetTempStr() const
	{
		static char s_szRetStr[5] = {0,0,0,0,0};
		s_szRetStr[3] = (char)((m_FourCCVal >> 24) & 255);
		s_szRetStr[2] = (char)((m_FourCCVal >> 16) & 255);
		s_szRetStr[1] = (char)((m_FourCCVal >> 8) & 255);
		s_szRetStr[0] = (char)(m_FourCCVal & 255);
		return s_szRetStr;
	}
};

#endif // __FourCC_h