//=================================================================================================
/*!
	\file DataBlock.h
	Base Library
	Data Block Header
	\author Taylor Clark
	\date February 27, 2006

	This header contains the definition for the data block object.
*/
//=================================================================================================

#pragma once
#ifndef __DataBlock_h
#define __DataBlock_h

#include "Types.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class DataBlock
	\brief Represents a block of memory that can be read from easily to be used when loading data

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class DataBlock
{
protected:

	/// The start of the block of data this object is representing
	const uint8* const m_pDataStart;

	/// The amount of data this block represents in bytes
	const uint32 m_DataLen;

	/// The offset, from the memory block's first byte, from which we are currently reading data
	uint32 m_CurByteOffset;

	/// The assignement operator, private so it can not be used
	DataBlock& operator =( const DataBlock& ){}

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
	DataBlock() : m_pDataStart( 0 ),
								m_CurByteOffset( 0 ),
								m_DataLen( 0 )
	{
	}

	/// The constructor to initialize the data
	DataBlock( const void* const pData, uint32 dataLen ) : m_pDataStart( (uint8*)pData ),
								m_CurByteOffset( 0 ),
								m_DataLen( dataLen )
	{
	}

	/// The copy constructor
	DataBlock( const DataBlock& copyObj ) : m_pDataStart( copyObj.m_pDataStart ),
											m_DataLen( copyObj.m_DataLen ),
											m_CurByteOffset( copyObj.m_CurByteOffset )
	{
	}

	virtual ~DataBlock(){}

	/// Retrieve a 32-bit integer
	virtual int32 ReadInt32(){ return ReadVal<int32>(); }

	/// Retrieve an unsigned 32-bit integer
	virtual uint32 ReadUint32(){ return ReadVal<uint32>(); }

	/// Retrieve a signed 16-bit integer
	virtual int16 ReadInt16(){ return ReadVal<int16>(); }

	/// Retrieve an unsigned 16-bit integer
	virtual uint16 ReadUint16(){ return ReadVal<uint16>(); }

	/// Retrieve a signed 8-bit integer
	int8 ReadInt8()
	{
		return ReadVal<int8>();
	}

	/// Retrieve an unsigned 8-bit integer
	uint8 ReadUint8()
	{
		return ReadVal<uint8>();
	}

	/// Retrieve a character
	wchar_t ReadChar()
	{
		return static_cast<wchar_t>( ReadUint16() );
	}

	/// Read a block of data
	void* const ReadData( unsigned int numBytes )
	{
		if( (m_CurByteOffset + numBytes) > m_DataLen )
			throw( "Buffer overflow" );

		void* const pRetVal = (void* const)(m_pDataStart + m_CurByteOffset);
		m_CurByteOffset += numBytes;
		return pRetVal;
	}

	/// Get the size of the data block
	uint32 GetSize() const { return m_DataLen; }

	/// Get the byte offfset within the data block
	uint32 GetCurPosition() const { return m_CurByteOffset; }

	/// Get the remaining bytes left
	uint32 GetRemainingBytes() const { return m_DataLen - m_CurByteOffset; }

	const void* GetPtr() const { return m_pDataStart; }
};

#endif // __DataBlock_h