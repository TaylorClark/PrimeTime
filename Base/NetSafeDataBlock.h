//=================================================================================================
/*!
	\file NetSafeDataBlock.h
	Base Library
	Data Block Header
	\author Taylor Clark
	\date April 14, 2010

	This header contains the definition for the data block object.
*/
//=================================================================================================

#pragma once
#ifndef __NetSafeDataBlock_h
#define __NetSafeDataBlock_h

#include "DataBlock.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class NetSafeDataBlock
	\brief Represents a block of memory that can be read from easily to be used when loading data

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class NetSafeDataBlock : public DataBlock
{
public:

	/// The default constructor
	NetSafeDataBlock() : DataBlock()
	{
	}

	/// The constructor to initialize the data
	NetSafeDataBlock( const void* const pData, uint32 dataLen ) : DataBlock(pData, dataLen)
	{
	}

	/// The copy constructor
	NetSafeDataBlock( const NetSafeDataBlock& copyObj ) : DataBlock(copyObj)
	{
	}

	/// Retrieve a 32-bit integer
	virtual int32 ReadInt32(){ return static_cast<int32>( ReadUint32() ); }

	/// Retrieve an unsigned 32-bit integer
	virtual uint32 ReadUint32();

	/// Retrieve a signed 16-bit integer
	virtual int16 ReadInt16(){ return static_cast<int16>( ReadUint16() ); }

	/// Retrieve an unsigned 16-bit integer
	virtual uint16 ReadUint16();
};

#endif // __NetSafeDataBlock_h