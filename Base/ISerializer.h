//=============================================================================
/*!
	\file Serializer.h
	Base Library
	Serializer Base ClassHeader
	\author Taylor Clark
	\date April 14, 2010

  This header contains the base class definition for serializer classes. This
  class provides one interface for saving data to and loading data from files.
  This allows one function to be created for saving and loading an object.
*/
//=============================================================================

#pragma once

#ifndef __ISerializer_h
#define __ISerializer_h

#include "Types.h"
#include <string>
#include <list>


namespace TCBase
{

//-----------------------------------------------------------------------------
/*! 
	\class ISerializer
	\brief Simplify loading from and saving data to a file.

	This is the abstract base class to provide an interface for saving and
	loading so serialization of data is simpler.
*/
//-----------------------------------------------------------------------------
class ISerializer
{	
public:
	
	// Accessors
	virtual bool InReadMode() const = 0;
	virtual bool InWriteMode() const = 0;
	
	virtual void AddRawData( uint8* pData, uint32 dataLen ) = 0;
	
	virtual uint32 GetOffset() const = 0;
	
	virtual void Seek( uint32 byteOffset ) = 0;
	
	virtual uint32 GetInputLength() = 0;
	
	virtual void AddData( float32& obj ) = 0;

	//virtual void AddData( tm& obj ) = 0;

	virtual void AddData( int8& obj ) = 0;

	virtual void AddData( uint8& obj ) = 0;

	virtual void AddData( int16& obj ) = 0;

	virtual void AddData( uint16& obj ) = 0;

	// Provided to simplify character reading/writing
	virtual void AddData( uint16*& obj, uint32 count ) = 0;

	virtual void AddData( int32& obj ) = 0;

	virtual void AddData( uint32& obj ) = 0;

	virtual void AddData( bool& obj ) = 0;

	virtual void AddData( std::string& obj ) = 0;

	virtual void AddData( std::wstring& obj ) = 0;

	//! The specialized list function
	//template<class T>
	//virtual void AddListData( std::list<T>& obj ) = 0;
	
	// The generic data reader/writer for arrays
	//template< class T >
	//virtual void AddData( T*& obj, uint32 count ) = 0;
};
	
};

#endif // __ISerializer_h