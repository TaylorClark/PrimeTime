//=================================================================================================
/*!
	\file NetDataBlockOut.h
	Base Library
	Network Data Block Output Header
	\author Taylor Clark
	\date June 13, 2006

	This header contains the definition for the network data block output object.
*/
//=================================================================================================

#pragma once
#ifndef __NetDataBlockOut_h
#define __NetDataBlockOut_h

#include "Base/Types.h"
#include <string>

//-------------------------------------------------------------------------------------------------
/*!
	\class NetDataBlockOut
	\brief Represents a block of memory that can be read from easily to be used when loading data

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class NetDataBlockOut
{
private:

	/// The start of the block of data this object is representing
	const uint8* m_pDataStart;

	/// The amount of data this block represents in bytes
	const uint32 m_DataLen;

	/// The offset, from the memory block's first byte, from which we are currently reading data
	uint32 m_CurByteOffset;

	/// The assignement operator, private so it can not be used
	NetDataBlockOut& operator =( const NetDataBlockOut& ){}

	/// Write out a value
	template< class T >
	void WriteVal( T val )
	{
		if( (m_CurByteOffset + sizeof(val)) > m_DataLen )
			throw( "Buffer overflow" );

		*( (T*)(m_pDataStart + m_CurByteOffset) ) = val;

		// Step ahead in the memory block
		m_CurByteOffset += sizeof(val);
	}

public:

	/// The default constructor
	NetDataBlockOut( void* pData, uint32 dataLen ) : m_pDataStart( (uint8*)pData ),
								m_CurByteOffset( 0 ),
								m_DataLen( dataLen )
	{
	}

	/// The copy constructor
	NetDataBlockOut( const NetDataBlockOut& copyObj ) : m_pDataStart( copyObj.m_pDataStart ),
											m_DataLen( copyObj.m_DataLen ),
											m_CurByteOffset( copyObj.m_CurByteOffset )
	{
	}

	/// Write out a 32-bit value
	void WriteVal32( uint32 val );

	/// Write out a 16-bit value
	void WriteVal16( uint16 val );

	/// Write out an 8-bit value
	void WriteVal8( uint8 val );

	// Write out a string
	void WriteString( std::wstring str );

	/// Get the size of the data block
	uint32 GetSize() const { return m_DataLen; }

	/// Get the byte offfset within the data block
	uint32 GetCurPosition() const { return m_CurByteOffset; }

	/// Get the remaining bytes left
	uint32 GetRemainingBytes() const { return m_DataLen - m_CurByteOffset; }
};

#endif // __NetDataBlockOut_h