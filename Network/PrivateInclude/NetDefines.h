//=================================================================================================
/*!
	\file NetDefines.h
	Network Library
	Network Defines Header
	\author Taylor Clark
	\date July 5, 2006

	This header contains defines and functions related to the networking class.
*/
//=================================================================================================

#pragma once
#ifndef __NetDefines_h
#define __NetDefines_h

#include "Base/Types.h"

namespace NetDefines
{
	/// Get a Winsock error descriptive error
	const wchar_t* GetWinsockErrorDesc( int errorCode );

	/// The maximum message size
	static const uint32 MSG_BUFFER_SIZE = 1024;
};

#endif // __NetDefines_h