//=============================================================================
/*!
	\file Serializer.h
	Base Library
	Serializer Header
	\author Taylor Clark
	\date April 25, 2004

  This header contains the class definition for serializer class.  This class
  provides one interface for saving data to and loading data from files.  This
  allows one function to be created for saving and loading an object.
*/
//=============================================================================

#pragma once

#ifndef __Serializer_h
#define __Serializer_h

#include "ISerializer.h"
#include "DataBlock.h"
#include "DataBlockOut.h"
#include <iostream>
#include <fstream>

//namespace TCBase
//{
//-----------------------------------------------------------------------------
/*! 
	\class Serializer
	\brief Simplify loading from and saving data to a file.

	The class provides one interface for saving and loading so your
	serialization of data is easier.
*/
//-----------------------------------------------------------------------------
class Serializer : public TCBase::ISerializer
{
protected:

	/// The input and output file streams
	std::ostream* m_pOutStream;
	std::istream* m_pInStream;

	/// The input and output memory streams
	DataBlock* m_pInMemStream;
	DataBlockOut* m_pOutMemStream;
	
public:
	/// Constructors
	Serializer( std::ostream* pOutStream ) : m_pInStream(0),
											m_pInMemStream(0),
											m_pOutMemStream(0)
	{
		m_pOutStream = pOutStream;
	}
	Serializer( std::istream* pInStream ) : m_pOutStream(0),
											m_pInMemStream(0),
											m_pOutMemStream(0)
	{
		m_pInStream = pInStream;
	}
	Serializer( DataBlock* pDataBlock ) : m_pInStream(0),
											m_pOutStream(0),
											m_pOutMemStream(0)
	{
		m_pInMemStream = pDataBlock;
	}
	Serializer( DataBlockOut* pDataBlock ) : m_pInStream(0),
											m_pOutStream(0),
											m_pInMemStream(0)
	{
		m_pOutMemStream = pDataBlock;
	}

	virtual ~Serializer() {}

	// Accessors
	bool InReadMode() const { return m_pOutStream == 0 && m_pOutMemStream == 0; }
	bool InWriteMode() const { return m_pInStream == 0 && m_pInMemStream == 0; }
	
public:
	
	void AddRawData( uint8* pData, uint32 dataLen );
	
	uint32 GetOffset() const;
	
	void Skip( uint32 numBytesToSkip );

	void Seek( uint32 byteOffsetFromBeginning );
	
	uint32 GetInputLength();
	
	void AddData( float32& obj );

	/*void AddData( tm& obj )
	{
		AddData_Default( obj );
	}*/

	void AddData( int8& obj )
	{
		AddData_Default( obj );
	}

	void AddData( uint8& obj )
	{
		AddData_Default( obj );
	}

	virtual void AddData( int16& obj );

	virtual void AddData( uint16& obj );

	void AddData( uint16*& obj, uint32 count )
	{
		for( uint32 index = 0; index < count; ++index )
			AddData( obj[ index ] );		
	}

	virtual void AddData( int32& obj );

	virtual void AddData( uint32& obj );

	virtual void AddData( bool& obj )
	{
		AddData_Default( obj );
	}

	void AddData( std::string& obj )
	{
		AddStringData( obj );
	}

	virtual void AddData( std::wstring& obj )
	{
		AddStringData( obj );
	}

	//! The specialized list function
	template<class T>
	void AddListData( std::list<T>& obj )
	{
		uint32 numItems = 0;
		if( m_pOutStream )
		{
			// Write the number of characters
			numItems = (uint32)obj.size();
			m_pOutStream->write( (const char*)&numItems, sizeof(numItems) );
			
			// Write the items
			typename std::list<T>::iterator iterObj;
			for( iterObj = obj.begin(); iterObj != obj.end(); ++iterObj )
			{
				typename std::list<T>::value_type curVal = *iterObj;
				
				AddData( curVal );
			}
		}
		else if( m_pOutMemStream )
		{
			// Write the number of characters
			numItems = (uint32)obj.size();
			m_pOutMemStream->WriteVal( numItems );
			
			// Write the list items
			for( typename std::list<T>::iterator iterObj = obj.begin(); iterObj != obj.end(); ++iterObj )
				m_pOutMemStream->WriteVal( *iterObj );
		}
		else if( m_pInStream )
		{
			// Read in the number of characters
			numItems = 0;
			m_pInStream->read( (char*)&numItems, sizeof(numItems) );
			
			// If there are no characters, bail
			if( numItems == 0 )
			{
				obj.clear();
				return;
			}
			
			// Read in the list items
			for( uint32 curItemIndex = 0; curItemIndex < numItems; ++curItemIndex )
			{
				typename std::list<T>::value_type curVal;
				m_pInStream->read( (char*)&curVal, sizeof(typename std::list<T>::value_type) );
				
				obj.push_back( curVal );
			}
		}
		else if( m_pInMemStream )
		{
			// Read in the number of characters
			numItems = m_pInMemStream->ReadUint32();
			
			// If there are no characters, bail
			if( numItems == 0 )
			{
				obj.clear();
				return;
			}
			
			// Read in the list items
			for( uint32 curItemIndex = 0; curItemIndex < numItems; ++curItemIndex )
			{
				typename std::list<T>::value_type* pNewItem = (typename std::list<T>::value_type*)m_pInMemStream->ReadData( sizeof(typename std::list<T>::value_type) );
				obj.push_back( *pNewItem );
			}
		}
	}
	
	// The generic data reader/writer for arrays
	/*template< class T >
	void AddData( T*& obj, uint32 count )
	{
		for( uint32 index = 0; index < count; ++index )
			AddData( obj[ index ] );		
	}*/

protected:

	//! The generic data reader/writer
	template< class T >
	void AddData_Default( T& obj )
	{
		if( m_pOutStream )
			m_pOutStream->write( (const char*)&obj, sizeof(T) );
		else if( m_pOutMemStream )
			m_pOutMemStream->WriteVal( obj );
		else if( m_pInStream )
			m_pInStream->read( (char*)&obj, sizeof(T) );
		else if( m_pInMemStream )
		{
			void* pData = m_pInMemStream->ReadData( sizeof(T) );
			obj = *( (T*)pData );
		}
	}
	
	//! The specialized string function
	template< class T >
	void AddStringData( std::basic_string<T>& obj )
	{
		uint32 numChars = 0;
		if( m_pOutStream )
		{
			// Write the number of characters
			numChars = (uint32)obj.size();
			m_pOutStream->write( (const char*)&numChars, sizeof(numChars) );
			
			// Write the string
			m_pOutStream->write( (char*)obj.c_str(), numChars * sizeof(typename std::basic_string<T>::value_type) );
		}
		else if( m_pOutMemStream )
		{
			// Write the number of characters
			numChars = (uint32)obj.size();
			m_pOutMemStream->WriteVal( numChars );
			
			// Write the string
			m_pOutMemStream->WriteChars( (typename std::basic_string<T>::value_type*)obj.c_str(), numChars );
		}
		else if( m_pInStream )
		{
			// Read in the number of characters
			numChars = 0;
			m_pInStream->read( (char*)&numChars, sizeof(numChars) );
			
			// If there are no characters, bail
			if( numChars == 0 )
			{
				obj.clear();
				return;
			}
			
			// Create a buffer to hold the string
			typename std::basic_string<T>::allocator_type alloc = obj.get_allocator();
			typename std::basic_string<T>::value_type* szBuffer = alloc.allocate( numChars );
			
			// Read the string
			m_pInStream->read( (char*)szBuffer, numChars * sizeof(typename std::basic_string<T>::value_type) );
			
			// Store the string and free the buffer
			obj.assign( szBuffer, numChars );
			alloc.deallocate( szBuffer, numChars );
		}
		else if( m_pInMemStream )
		{
			// Read in the number of characters
			numChars = m_pInMemStream->ReadUint32();
			
			// If there are no characters, bail
			if( numChars == 0 )
			{
				obj.clear();
				return;
			}
			
			// Read in the characters
			void* pStrData = m_pInMemStream->ReadData( numChars * sizeof(typename std::basic_string<T>::value_type) );
			obj.assign( (typename std::basic_string<T>::value_type*)pStrData, numChars );
		}
	}
};
	
//};

#endif // __Serializer_h