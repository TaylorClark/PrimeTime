//=================================================================================================
/*!
	\file IPAddress.h
	Network Library
	Network Connection Header
	\author Taylor Clark
	\date July 5, 2006

	This file contains the definition for the network connection class.
*/
//=================================================================================================

#pragma once
#ifndef __IPAddress_h
#define __IPAddress_h

#include "Base/Types.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class IPAddress
	\brief The class to stores an IPv4 IP address.
*/
//-------------------------------------------------------------------------------------------------
class IPAddress
{
private:

	/// The IP address in network ordered value
	uint32 m_IPAddress;

public:

	/// Create an IP address from a dotted address string
	explicit IPAddress( const char* szIPAddress );

	/// Create an IP address from a 32-bit network ordered value
	explicit IPAddress( uint32 ipAddress = 0 ) : m_IPAddress( ipAddress )
	{
	}

	/// Retrieve the value as a string
	const char* ToString() const;

	/// Equality
	bool operator == ( const IPAddress& testIP )
	{
		return m_IPAddress == testIP.m_IPAddress;
	}

	/// Retrieve the value as a string
	uint32 GetVal() const
	{
		return m_IPAddress;
	} 
};

#endif // __IPAddress_h