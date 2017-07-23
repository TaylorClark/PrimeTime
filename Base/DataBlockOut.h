//=================================================================================================
/*!
	\file DataBlockOut.h
	Base Library
	Data Block Output Header
	\author Taylor Clark
	\date June 13, 2006

	This header contains the definition for the data block output object.
*/
//=================================================================================================

#pragma once
#ifndef __DataBlockOut_h
#define __DataBlockOut_h

#include "Types.h"
#include <string>
#include "StringFuncs.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class DataBlockOut
	\brief Represents a block of memory that can be read from easily to be used when loading data

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class DataBlockOut
{
private:

	/// The start of the block of data this object is representing
	const uint8* m_pDataStart;

	/// The amount of data this block represents in bytes
	const uint32 m_DataLen;

	/// The offset, from the memory block's first byte, from which we are currently reading data
	uint32 m_CurByteOffset;

	/// The assignement operator, private so it can not be used
	DataBlockOut& operator =( const DataBlockOut& ){}

public:

	/// The default constructor
	DataBlockOut( void* pData, uint32 dataLen ) : m_pDataStart( (uint8*)pData ),
								m_CurByteOffset( 0 ),
								m_DataLen( dataLen )
	{
	}

	/// The copy constructor
	DataBlockOut( const DataBlockOut& copyObj ) : m_pDataStart( copyObj.m_pDataStart ),
											m_DataLen( copyObj.m_DataLen ),
											m_CurByteOffset( copyObj.m_CurByteOffset )
	{
	}

	virtual ~DataBlockOut(){}

	/// Write out a value
	void WriteVal( int8 val ){ WriteVal_Defualt( val ); }
	void WriteVal( uint8 val ){ WriteVal_Defualt( val ); }
	virtual void WriteVal( wchar_t val ){ WriteVal_Defualt( val ); }
	virtual void WriteVal( int16 val ){ WriteVal_Defualt( val ); }
	virtual void WriteVal( uint16 val ){ WriteVal_Defualt( val ); }
	virtual void WriteVal( int32 val ){ WriteVal_Defualt( val ); }
	virtual void WriteVal( uint32 val ){ WriteVal_Defualt( val ); }
	
	void WriteVal( const std::string& val )
	{
		WriteValArray_Default( val.c_str(), static_cast<uint32>(val.length()) );
	}

	virtual void WriteVal( const std::wstring& val )
	{
		WriteValArray_Default( val.c_str(), static_cast<uint32>(val.length()) );
	}
	
	/// Write out an array
	void WriteVal( const uint8* pVals, uint32 numItems ){ WriteValArray_Default( pVals, numItems ); }
	//void WriteVal( const int8* pVals, uint32 numItems ){ WriteValArray_Default( pVals, numItems ); }
	void WriteChars( const char* pVals, uint32 numItems ){ WriteValArray_Default( pVals, numItems ); }
	virtual void WriteVal( const uint16* pVals, uint32 numItems ){ WriteValArray_Default( pVals, numItems ); }
	virtual void WriteChars( const wchar_t* pVals, uint32 numItems ){ WriteValArray_Default( pVals, numItems );	}
	
	/// Get the size of the data block
	uint32 GetSize() const { return m_DataLen; }

	/// Get the byte offfset within the data block
	uint32 GetCurPosition() const { return m_CurByteOffset; }

	/// Get the remaining bytes left
	uint32 GetRemainingBytes() const { return m_DataLen - m_CurByteOffset; }

protected:

	/// Write out a value
	template< class T >
	void WriteVal_Defualt( T val )
	{
		if( (m_CurByteOffset + sizeof(val)) > m_DataLen )
			throw( "Buffer overflow" );

		*( (T*)(m_pDataStart + m_CurByteOffset) ) = val;

		// Step ahead in the memory block
		m_CurByteOffset += sizeof(val);
	}

	/// Write out an array
	template< class T >
	void WriteValArray_Default( const T* pVals, uint32 numItems )
	{
		uint32 length = numItems * sizeof(T);
		if( (m_CurByteOffset + length) > m_DataLen )
			throw( "Buffer overflow" );

		T* pData = (T*)(m_pDataStart + m_CurByteOffset);
		for( uint32 arrayIndex = 0; arrayIndex < numItems; ++arrayIndex )
			pData[arrayIndex] = pVals[arrayIndex];

		// Step ahead in the memory block
		m_CurByteOffset += length;
	}
};

#endif // __DataBlockOut_h