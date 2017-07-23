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

#ifndef __NetSafeSerializer_h
#define __NetSafeSerializer_h

#include "Serializer.h"
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
class NetSafeSerializer : public Serializer
{	
public:
	/// Constructors
	NetSafeSerializer( std::ostream* pOutStream ) : Serializer(pOutStream)
	{
	}

	NetSafeSerializer( std::istream* pInStream ) : Serializer(pInStream)
	{
	}

	NetSafeSerializer( DataBlock* pDataBlock ) : Serializer(pDataBlock)
	{
	}

	NetSafeSerializer( DataBlockOut* pDataBlock ) : Serializer(pDataBlock)
	{
	}
	
public:

	using Serializer::AddData;
	
	virtual void AddData( bool& obj );

	virtual void AddData( int16& obj );

	virtual void AddData( uint16& obj );

	virtual void AddData( int32& obj );

	virtual void AddData( uint32& obj );

	virtual void AddData( std::wstring& obj );
};
	
//};

#endif // __NetSafeSerializer_h