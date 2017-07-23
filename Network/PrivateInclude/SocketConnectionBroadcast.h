//=================================================================================================
/*!
	\file NetConnectionBroadcast.h
	Network Library
	Broadcast Network Connection Header
	\author Taylor Clark
	\date July 7, 2006

	This file contains the definition for the UDP broadcast network connection class.
*/
//=================================================================================================

#pragma once
#ifndef __SocketConnectionBroadcast_h
#define __SocketConnectionBroadcast_h

#include "SocketConnectionBase.h"
#include "../IPAddress.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class SocketConnectionBroadcast
	\brief The class to handle a UDP broadcast network connection.

	This class encapsulates the functionality for sending and receiving data.
*/
//-------------------------------------------------------------------------------------------------
class SocketConnectionBroadcast : public SocketConnectionBase
{
private:

	/// The port this connection sends to/or receives from
	uint16 m_Port;

	/// The IP address to which we are broadcasting
	IPAddress m_SendToIP;

public:

	/// The constructor to initialize from a socket
	SocketConnectionBroadcast( SOCKET socket = 0, uint16 port = 0 );

	/// Send a message over this connection
	virtual bool SendData( const NetMsg* pMsg );

	/// Get the port used by this connection
	uint16 GetPort() const { return m_Port; }
};

#endif // __SocketConnectionBroadcast_h