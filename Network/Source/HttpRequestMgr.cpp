#include "../HttpRequestMgr.h"
#include "Base/XPThreads.h"

#ifndef WIN32
#include <unistd.h>
#endif

#define CURL_STATICLIB
//#include <curl/curl.h>

bool HttpRequestMgr::s_CurlHasBeenInitialized = false;


///////////////////////////////////////////////////////////////////////////////////////////////////
//  HttpRequestMgr::Init  Static Public
///
/// Called one time to prepare the network manager for making requests
///////////////////////////////////////////////////////////////////////////////////////////////////
void HttpRequestMgr::Init()
{
	//curl_global_init( CURL_GLOBAL_WIN32 );

	HttpRequestMgr::s_CurlHasBeenInitialized = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  HttpRequestMgr::Term  Static Public
///
/// Free resources associated with the network manager
///////////////////////////////////////////////////////////////////////////////////////////////////
void HttpRequestMgr::Term()
{
	//curl_global_cleanup();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  write_data  Global
///	\param pBuffer The buffer from which data is read
///	\param size The size of each element in the buffer
///	\param nmemb The number of elements in the buffer
///	\param userp A pointer to a user defined object
///	\returns The number of bytes handled, if this isn't equal to size * nmemb then it is taken as
///				an error condition and transfer is stopeed
///	\brief The callback for libCurl's data transfer
///
///	This function is used by libCurl to allow data to be handled however we see fit.
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t write_data(void *pBuffer, size_t size, size_t nmemb, void *userp)
{
	size_t bytesToStore = size * nmemb;

	HttpRequestInfo* pRequestInfo = static_cast<HttpRequestInfo*>( userp );

	pRequestInfo->ReceivedData.BufferItemSize = (uint8)size;

	size_t newBufferSize = pRequestInfo->ReceivedData.BufferSize + bytesToStore;

	// Allocate a new buffer that will hold the old and new data
	uint8* pNewBuffer = new uint8[ newBufferSize ];
	if( pNewBuffer == NULL )
	{
		// We are out of memory so free our memory and bail
		if(  pRequestInfo->ReceivedData.pBuffer )
			delete [] pRequestInfo->ReceivedData.pBuffer;
		 pRequestInfo->ReceivedData.pBuffer = 0;
		 pRequestInfo->ReceivedData.BufferSize = 0;

		 // Return 0 to indicate transfer has failed
		 return 0;
	}

	// Copy the existing data into the buffer
	if( pRequestInfo->ReceivedData.pBuffer )
	{
		memcpy( pNewBuffer, pRequestInfo->ReceivedData.pBuffer, pRequestInfo->ReceivedData.BufferSize );
		delete [] pRequestInfo->ReceivedData.pBuffer;
	}

	// Copy the new data into the buffer
	memcpy( pNewBuffer + pRequestInfo->ReceivedData.BufferSize, pBuffer, bytesToStore );

	pRequestInfo->ReceivedData.pBuffer = pNewBuffer;
	pRequestInfo->ReceivedData.BufferSize = newBufferSize;

	// Return the number of bytes read
	return bytesToStore;
}


std::wstring g_TempGetAndForgetUri;

#ifdef WIN32
void GetAndForgetCallback( void* )
#else
void* GetAndForgetCallback( void* )
#endif
{
	// Make a local copy of the string
	std::wstring sUri = g_TempGetAndForgetUri;

	// Indicate we've made a copy of the string by clearing it
	g_TempGetAndForgetUri.clear();

	HttpRequestInfo requestInfo = HttpRequestMgr::Get( sUri.c_str() );
	requestInfo.ReceivedData.Free();
	
#ifndef WIN32
	return 0;
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  HttpRequestMgr::GetAndForgetAsync  Static Public
///	\param szUri The URI to retrieve (Or notify in this case)
///
/// Perform an asynchronous GET on a resource, but ignore the results. Useful for sending
/// notifications via HTTP.
///////////////////////////////////////////////////////////////////////////////////////////////////
void HttpRequestMgr::GetAndForgetAsync( const wchar_t* szUri )
{
	XPThreads thread( GetAndForgetCallback );
	g_TempGetAndForgetUri = szUri;
	thread.Run( NULL );

	/*
	// Wait until the new thread copies the URI, which is indicated by clearing the global variable
	while( g_TempGetAndForgetUri.length() > 0 )
	{
#ifdef WIN32
		Sleep(10);
#else
        // usleep takes microseconds
		usleep(10 * 1000);
#endif
	}
    */
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  HttpRequestMgr::Get  Static Public
///	\param szUri The URI to retrieve
///
/// Retrieve a resource by URI, taking into account the last time we retrieved the resource
///////////////////////////////////////////////////////////////////////////////////////////////////
HttpRequestInfo HttpRequestMgr::Get( const wchar_t* szUri )
{
	return Get( TCBase::Narrow(szUri).c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  HttpRequestMgr::Get  Static Public
///	\param szUri The URI to retrieve
///	\param szModifiedSince The date string to use for the client request's "modified-since" header
///
/// Retrieve a resource by URI, taking into account the last time we retrieved the resource
///////////////////////////////////////////////////////////////////////////////////////////////////
HttpRequestInfo HttpRequestMgr::Get( const char* szUri )
{
	// 2023/07/28 - Simply disable network requests for now
	szUri;

	HttpRequestInfo requestInfo;
	return requestInfo;
	/*
	CURL* pEasyCurl = curl_easy_init();

	curl_easy_setopt(pEasyCurl, CURLOPT_URL, szUri);

	// Reduce the connection timeout to 10 seconds
	curl_easy_setopt(pEasyCurl, CURLOPT_CONNECTTIMEOUT, 4);
	curl_easy_setopt(pEasyCurl, CURLOPT_TIMEOUT, 10);
	
	HttpRequestInfo requestInfo;
	requestInfo.RequestedUri = TCBase::Widen(szUri);

	curl_easy_setopt(pEasyCurl, CURLOPT_WRITEDATA, &requestInfo);
	curl_easy_setopt(pEasyCurl, CURLOPT_WRITEFUNCTION, write_data);

	// Store the time at which we performed this request
	requestInfo.LocalRequestTime = time(NULL);

	curl_easy_perform( pEasyCurl );

	// Get the HTTP status code
	long httpStatusCode = 0;
	curl_easy_getinfo (pEasyCurl, CURLINFO_RESPONSE_CODE, &httpStatusCode);
	requestInfo.StatusCode = (uint32)httpStatusCode;

	curl_easy_cleanup(pEasyCurl);
	pEasyCurl = NULL;

	// If we got a local file, then simulate some server data
	if( TCBase::StartsWith(szUri,"file://") )
		requestInfo.StatusCode = 200;

	return requestInfo;
	*/
}