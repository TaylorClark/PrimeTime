//=================================================================================================
/*!
	\file NetDataBlock.h
	Base Library
	Network Data Block Header
	\author Taylor Clark
	\date September 21, 2006

	This header contains the definition for the network data block object.
*/
//=================================================================================================

#pragma once
#ifndef __NetDataBlock_h
#define __NetDataBlock_h

#include "Base/Types.h"
#include <string>


//-------------------------------------------------------------------------------------------------
/*!
	\class NetDataBlock
	\brief Represents a block of memory that can be read from easily to be used when loading data

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class NetDataBlock
{
private:

	/// The start of the block of data this object is representing
	const uint8* const m_pDataStart;

	/// The amount of data this block represents in bytes
	const uint32 m_DataLen;

	/// The offset, from the memory block's first byte, from which we are currently reading data
	uint32 m_CurByteOffset;

	/// The assignement operator, private so it can not be used
	NetDataBlock& operator =( const NetDataBlock& ){}

	template< class T >
	T ReadVal()
	{
		if( (m_CurByteOffset + sizeof(T)) > m_DataLen )
			throw( "Buffer overflow" );

		T retVal = *( (T* const)(m_pDataStart + m_CurByteOffset) );
		m_CurByteOffset += sizeof(T);
		return retVal;
	}

public:

	/// The default constructor
	NetDataBlock( void* const pData, uint32 dataLen ) : m_pDataStart( (uint8*)pData ),
								m_CurByteOffset( 0 ),
								m_DataLen( dataLen )
	{
	}

	/// The copy constructor
	NetDataBlock( const NetDataBlock& copyObj ) : m_pDataStart( copyObj.m_pDataStart ),
											m_DataLen( copyObj.m_DataLen ),
											m_CurByteOffset( copyObj.m_CurByteOffset )
	{
	}

	/// Retrieve an unsigned 32-bit integer
	uint32 ReadUint32();

	/// Retrieve an unsigned 16-bit integer
	uint16 ReadUint16();

	/// Retrieve an unsigned 8-bit integer
	uint8 ReadUint8();

	/// Retrieve a character
	std::wstring ReadString();

	/// Get the size of the data block
	uint32 GetSize() const { return m_DataLen; }

	/// Get the byte offfset within the data block
	uint32 GetCurPosition() const { return m_CurByteOffset; }

	/// Get the remaining bytes left
	uint32 GetRemainingBytes() const { return m_DataLen - m_CurByteOffset; }
};

#endif // __NetDataBlock_h