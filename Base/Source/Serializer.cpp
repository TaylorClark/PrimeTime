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

#include "../Serializer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "../StringFuncs.h"


void Serializer::AddData( float32& obj )
{
	float32 temp = obj;
	uint32 uintFloat = *reinterpret_cast<uint32*>( reinterpret_cast<uint32*>(((void*)&temp)) );

	AddData_Default( uintFloat );
	
	obj = *reinterpret_cast<float32*>( (void*)&uintFloat );
}

void Serializer::AddData( int16& obj )
{
	AddData_Default( obj );
}

void Serializer::AddData( uint16& obj )
{
	AddData_Default( obj );
}

void Serializer::AddData( int32& obj )
{
	AddData_Default( obj );
}

void Serializer::AddData( uint32& obj )
{
	AddData_Default( obj );
}


/// The generic chunk reader/writer
void Serializer::AddRawData( uint8* pData, uint32 dataLen )
{
	if( m_pOutStream )
		m_pOutStream->write( (const char*)pData, dataLen );
	else if( m_pOutMemStream )
		m_pOutMemStream->WriteVal( pData, dataLen );
	else if( m_pInStream )
		m_pInStream->read( (char*)pData, dataLen );
	else if( m_pInMemStream )
		memcpy( pData, m_pInMemStream->ReadData( dataLen ), dataLen );
}

/// Get the offset from the start of the stream
uint32 Serializer::GetOffset() const
{
	if( m_pOutStream )
		return (uint32)m_pOutStream->tellp();
	else if( m_pInStream )
		return (uint32)m_pInStream->tellg();
	
	throw L"Seeking unsupported on memory streams.";
}

/// Skip some data
void Serializer::Skip( uint32 numBytesToSkip )
{
	if( m_pOutStream )
		m_pOutStream->seekp( numBytesToSkip, std::ios_base::cur );
	else if( m_pInStream )
		m_pInStream->seekg( numBytesToSkip, std::ios_base::cur );
	else
		throw L"Seeking unsupported on memory streams.";
}


/// Seek within the stream
void Serializer::Seek( uint32 byteOffsetFromBeginning )
{
	if( m_pOutStream )
		m_pOutStream->seekp( byteOffsetFromBeginning, std::ios_base::beg );
	else if( m_pInStream )
		m_pInStream->seekg( byteOffsetFromBeginning, std::ios_base::beg );
	else
		throw L"Seeking unsupported on memory streams.";
}

/// If this serializer is reading, get the length of the input stream
uint32 Serializer::GetInputLength()
{
	// Only retrieve the length when reading
	if( !InReadMode() )
		return 0;
	
	uint32 retLen = 0;
	if( m_pInStream )
	{
		// Store the current offset
		uint32 origOffset = (uint32)m_pInStream->tellg();
		
		// Seek to the end and store the offset
		m_pInStream->seekg( 0, std::ios_base::end );
		retLen = (uint32)m_pInStream->tellg();
		
		// Seek back to the original offset
		m_pInStream->seekg( origOffset, std::ios_base::beg );
	}
	// Else if this is a memory stream then simple store the length
	else if( m_pInMemStream )
		retLen = m_pInMemStream->GetSize();
	else
		throw L"Invalid input stream.";
	
	return retLen;
}