//=================================================================================================
/*!
	\file IUpdatable.h
	Base Library
	Updatable Interface Header
	\author Taylor Clark
	\date May 13, 2006

	This header contains the interface definition for the updatable base class.
*/
//=================================================================================================

#pragma once
#ifndef __DynamicBuffer_h
#define __DynamicBuffer_h

#include "Base/Types.h"
#include <malloc.h>


//-------------------------------------------------------------------------------------------------
/*!
	\class DynamicBuffer
	\brief Represents an object that can store a dynamic amount of data in memory 
*/
//-------------------------------------------------------------------------------------------------
class DynamicBuffer
{
private:

	/// The number of bytes of data stored in the buffer
	uint32 m_ByteOffset;

	/// The number of bytes allocated by the buffer
	uint32 m_BufferSize;

	/// The current offset, within the buffer, of one byte past the last written byte
	/// The buffer
	uint8* m_pBuffer;

public:

	/// The default constructor
	DynamicBuffer() : m_ByteOffset( 0 ),
						m_BufferSize( 0 ),
						m_pBuffer( 0 )
	{}

	/// The destructor
	~DynamicBuffer()
	{
		Free();
	}

	// Free the internal data
	void Free()
	{
		if( m_pBuffer )
			delete [] m_pBuffer;
		m_pBuffer = 0;
		m_BufferSize = 0;
		m_ByteOffset = 0;
	}

	/// Write data to the buffer
	void Write( void* pData, uint32 numBytes )
	{
		// If there is no buffer size then allocate the initial buffer
		if( !m_pBuffer )
		{
			uint32 initialSize = numBytes * 4;
			m_pBuffer = (uint8*)malloc( (size_t)initialSize );
		}

		// If the buffer is too small
		if( (m_ByteOffset + numBytes) > m_BufferSize )
		{
			// Calculate the new buffer size
			uint32 newBufferSize = m_BufferSize + numBytes + 1024;

			// Allocate a new buffer
			uint8* pNewBuffer = (uint8*)realloc( m_pBuffer, newBufferSize );

			// If the allocation failed
			if( !pNewBuffer )
				throw( "Failed to allocate buffer, out of memory" );

			// Store the new buffer
			m_pBuffer = pNewBuffer;
			m_BufferSize = newBufferSize;
		}

		// Write the data
		memcpy_s( m_pBuffer + m_ByteOffset, (m_BufferSize - m_ByteOffset), pData, numBytes );
		m_ByteOffset += numBytes;
	}

	/// Get the buffer pointer
	void* GetBuffer() { return m_pBuffer; }

	/// Get the number of bytes written to the buffer
	uint32 GetDataSize() const { return m_ByteOffset; }

	/// Release the buffer and allow the caller to manage the memory
	void* Release()
	{
		// Reallocate the memory down to the minimum amount needed
		void* pRetVal = realloc( m_pBuffer, m_ByteOffset );

		// Clear the internals
		m_BufferSize = 0;
		m_ByteOffset = 0;
		m_pBuffer = 0;

		// Return the buffer
		return pRetVal;
	}
};

#endif // __DynamicBuffer_h