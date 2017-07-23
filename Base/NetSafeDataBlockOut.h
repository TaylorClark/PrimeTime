//=================================================================================================
/*!
	\file NetSafeDataBlockOut.h
	Base Library
	Network Safe Data Block Output Header
	\author Taylor Clark
	\date April 15, 2010

	This header contains the definition for the data block output object that takes into account
	network byte-order.
*/
//=================================================================================================

#pragma once
#ifndef __NetSafeDataBlockOut_h
#define __NetSafeDataBlockOut_h

#include "DataBlockOut.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class NetSafeDataBlockOut
	\brief Represents a block of memory that can be read from easily to be used when loading data

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class NetSafeDataBlockOut : public DataBlockOut
{
private:
	/// The assignement operator, private so it can not be used
	NetSafeDataBlockOut& operator =( const NetSafeDataBlockOut& ){}

public:

	/// The default constructor
	NetSafeDataBlockOut( void* pData, uint32 dataLen ) : DataBlockOut( pData, dataLen )
	{
	}

	/// The copy constructor
	NetSafeDataBlockOut( const NetSafeDataBlockOut& copyObj ) : DataBlockOut( copyObj )
	{
	}

	virtual ~NetSafeDataBlockOut(){}

	/// Write out a value
	virtual void WriteVal( wchar_t val ){ WriteVal( static_cast<uint16>(val) ); }
	virtual void WriteVal( int16 val ){ WriteVal( static_cast<uint16>(val) ); }
	virtual void WriteVal( uint16 val );
	virtual void WriteVal( int32 val ){ WriteVal( static_cast<uint32>(val) ); }
	virtual void WriteVal( uint32 val );

	virtual void WriteVal( const uint16* pVals, uint32 numItems )
	{
		for( uint32 curItemIndex = 0; curItemIndex < numItems; ++curItemIndex )
			WriteVal( pVals[curItemIndex] );
	}
	
	virtual void WriteChars( const wchar_t* pVals, uint32 numItems )
	{
		uint16* pChars = TCBase::StringToArray( pVals, numItems );
		WriteVal( pChars, numItems );
		delete [] pChars;
	}

	virtual void WriteVal( const std::wstring& val )
	{
		WriteChars( val.c_str(), val.length() );
	}
};

#endif // __NetSafeDataBlockOut_h