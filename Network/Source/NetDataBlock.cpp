#include "../NetDataBlock.h"
#include "../NetDataBlockOut.h"
#include <Winsock2.h>


/// Retrieve an unsigned 32-bit integer
uint32 NetDataBlock::ReadUint32() throw(...)
{
	return (uint32)ntohl( ReadVal<u_long>() );
}

/// Retrieve an unsigned 16-bit integer
uint16 NetDataBlock::ReadUint16() throw(...)
{
	return (uint16)ntohs( ReadVal<u_short>() );
}

/// Retrieve an unsigned 8-bit integer
uint8 NetDataBlock::ReadUint8() throw(...)
{
	return ReadVal<uint8>();
}

/// Retrieve a character
std::wstring NetDataBlock::ReadString() throw(...)
{
	// Read the string length
	uint16 strLen = (uint16)ntohs( ReadVal<u_short>() );
	if( strLen == 0 )
		return std::wstring();

	// Check if there is enough data left in the block for this string
	if( (m_CurByteOffset + sizeof(wchar_t) * strLen) > m_DataLen )
		throw( "Buffer overflow" );

	// Read in the characters, one at a time
	std::wstring retStr;
	for( uint16 charIndex = 0; charIndex < strLen; ++charIndex )
	{
		wchar_t curChar = (wchar_t)ntohs( ReadVal<u_short>() );
		retStr += curChar;
	}

	// Return the string
	return retStr;
}


/// Write out a 32-bit value
void NetDataBlockOut::WriteVal32( uint32 val ) throw(...)
{
	WriteVal( htonl( (u_long)val ) );
}

/// Write out a 16-bit value
void NetDataBlockOut::WriteVal16( uint16 val ) throw(...)
{
	WriteVal( htons( (u_short)val ) );
}

/// Write out an 8-bit value
void NetDataBlockOut::WriteVal8( uint8 val ) throw(...)
{
	WriteVal( (uint8)val );
}

// Write out a string
void NetDataBlockOut::WriteString( std::wstring str ) throw(...)
{
	uint16 numChars = (uint16)str.length();

	uint32 length = (uint32)numChars * sizeof(wchar_t);
	if( (m_CurByteOffset + length) > m_DataLen )
		throw( "Buffer overflow" );

	// Write the length
	WriteVal16( numChars );

	// Write the characters
	wchar_t* pData = (wchar_t*)(m_pDataStart + m_CurByteOffset);
	for( uint16 arrayIndex = 0; arrayIndex < numChars; ++arrayIndex )
		pData[arrayIndex] = (wchar_t)htons( str[arrayIndex] );

	// Step ahead in the memory block
	m_CurByteOffset += length;
}