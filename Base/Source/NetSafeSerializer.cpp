//=============================================================================
/*!
	\file Serializer.cpp
	Base Library
	Serializer Source
	\author Taylor Clark
	\date June 15, 2006

  This file contains the implementation of the serializer class.
*/
//=============================================================================

#include "../NetSafeSerializer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "../StringFuncs.h"

#ifdef WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif


void NetSafeSerializer::AddData( int16& obj )
{
	// If we are writing then convert the value to network byte order
	uint16 tempVal = (uint16)obj;
	AddData( tempVal );

	if( InReadMode() )
		obj = (int16)tempVal;
}

void NetSafeSerializer::AddData( uint16& obj )
{
	// If we are writing then convert the value to network byte order
	uint16 tempVal = obj;
	if( InWriteMode() )
		tempVal = ntohs( obj );

	AddData_Default( tempVal );

	// If we read in a value then convert it to host byte order
	if( InReadMode() )
	{
		tempVal = htons( tempVal );
		obj = tempVal;
	}
}

void NetSafeSerializer::AddData( int32& obj )
{
	// If we are writing then convert the value to network byte order
	uint32 tempVal = (uint32)obj;
	AddData( tempVal );
	obj = (int32)tempVal;
}

void NetSafeSerializer::AddData( uint32& obj )
{
	// If we are writing then convert the value to network byte order
	uint32 tempVal = obj;
	if( InWriteMode() )
		tempVal = ntohl( obj );

	AddData_Default( tempVal );

	// If we read in a value then convert it to host byte order
	if( InReadMode() )
	{
		tempVal = ntohl( tempVal );
		obj = tempVal;
	}
}

void NetSafeSerializer::AddData( bool& obj )
{
	uint8 tempVal = obj ? 1 : 0;
	AddData( tempVal );
	obj = tempVal == 1;
}


/// wchar_t is 4 bytes on Mac, 2 bytes on PC, and the flag to change the size of Mac has issues so
/// we need to special case it.
void NetSafeSerializer::AddData( std::wstring& obj )
{
	// Read/write the number of characters
	uint32 numChars = (uint32)obj.size();
	AddData( numChars );

	if( m_pOutStream )
	{
		// Write the number of characters
		//numChars = (uint32)obj.size();
		//m_pOutStream->write( (const char*)&numChars, sizeof(numChars) );
		
		// Write the string
		TCBase::WriteChars( *this, obj );
	}
	else if( m_pOutMemStream )
	{
		// Write the number of characters
		//numChars = (uint32)obj.size();
		//m_pOutMemStream->WriteVal( numChars );
		
		// Make sure the characters are 2-bytes
		uint16* pTempBuffer = new uint16[ numChars ];

		for( uint32 curCharIndex = 0; curCharIndex < numChars; ++curCharIndex )
			pTempBuffer[curCharIndex] = static_cast<uint16>( obj[curCharIndex] );

		m_pOutMemStream->WriteVal( pTempBuffer, numChars );

		delete [] pTempBuffer;
	}
	else if( m_pInStream )
	{
		// Read in the number of characters
		//numChars = 0;
		//m_pInStream->read( (char*)&numChars, sizeof(numChars) );
		
		// If there are no characters, bail
		if( numChars == 0 )
		{
			obj.clear();
			return;
		}
		
		obj = TCBase::ReadChars( *this, numChars );
	}
	else if( m_pInMemStream )
	{
		// Read in the number of characters
		//numChars = m_pInMemStream->ReadUint32();
		
		// If there are no characters, bail
		if( numChars == 0 )
		{
			obj.clear();
			return;
		}

		obj = TCBase::ReadChars( *this, numChars );
	}
}