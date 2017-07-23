//=================================================================================================
/*!
	\file HttpRequestMgr.h
	Network Library
	Network Manager Header
	\author Taylor Clark
	\date March 30, 2011

	This file contains the definition for the HTTP request manager class.
*/
//=================================================================================================

#pragma once
#ifndef __HttpRequestMgr_h
#define __HttpRequestMgr_h

#include "Base/Types.h"
#include <string>
#include "Base/StringFuncs.h"

struct ReadBuffer
{
	ReadBuffer() : pBuffer(0),
					BufferSize( 0 ),
					BufferItemSize( 1 )
	{}

	ReadBuffer(uint8* pInBuffer, size_t bufferSize, uint8 itemSize) : pBuffer(pInBuffer),
					BufferSize( bufferSize ),
					BufferItemSize( itemSize )
	{}

	uint8* pBuffer;
	size_t BufferSize;

	uint8 BufferItemSize;

	void Free()
	{
		if( pBuffer )
			delete [] pBuffer;
		pBuffer = 0;
		BufferSize = 0;
	}

	ReadBuffer Clone()
	{
		ReadBuffer clone;
		clone.BufferItemSize = BufferItemSize;
		clone.BufferSize = BufferSize;

		clone.pBuffer = new uint8[ BufferSize ];
		memcpy( clone.pBuffer, pBuffer, BufferSize );

		return clone;
	}


	static size_t EnsureDataNullTerminated(uint8** ppBuffer, size_t bufferSize, uint8 bufferItemSize );

	void EnsureNullTerminated()
	{
		BufferSize = EnsureDataNullTerminated( &pBuffer, BufferSize, BufferItemSize );
	}

	/// Convert the data from single-byte characters to a null-termined 2-byte character array
	void Widen()
	{
		// If the item size is already 2 bytes then bail
		if( BufferItemSize == 2 )
			return;

		EnsureNullTerminated();

		std::wstring wideData = TCBase::Widen( (const char*)pBuffer );

		uint32 newBufferSize = (wideData.size() + 1) * 2;
		uint8* pNewBuffer = new uint8[ newBufferSize ];

		memcpy( pNewBuffer, wideData.c_str(), newBufferSize );

		memset( pNewBuffer + (newBufferSize - 2), 0, 2 );

		delete [] pBuffer;
		pBuffer = pNewBuffer;
		BufferSize = newBufferSize;
		BufferItemSize = 2;
	}
};


struct HttpRequestInfo
{
	HttpRequestInfo() : StatusCode(0),
						LocalRequestTime(0)
	{
	}

	std::wstring RequestedUri;

	/// The computer's time when this request was made
	time_t LocalRequestTime;

	ReadBuffer ReceivedData;
	uint32 StatusCode;
};

class HttpRequestMgr
{
private:

	/// A flag indicating if CURL, the library for HTTP requests, has been initialized
	static bool s_CurlHasBeenInitialized;

public:

	/// Called one time to prepare the network manager for making requests
	static void Init();

	/// Free resources associated with the network manager
	static void Term();

	/// Perform an asynchronous GET on a resource, but ignore the results. Useful for sending
	/// notifications via HTTP.
	static void GetAndForgetAsync( const wchar_t* szUri );

	/// Retrieve a resource by URI
	static HttpRequestInfo Get( const wchar_t* szUri );

	/// Retrieve a resource by URI
	static HttpRequestInfo Get( const char* szUri );
};

#endif // __HttpRequestMgr_h