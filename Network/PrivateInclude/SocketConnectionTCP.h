//=================================================================================================
/*!
	\file SocketConnectionTCP.h
	Network Library
	Socket Connection Header
	\author Taylor Clark
	\date July 5, 2006

	This file contains the definition for the socket connection class.
*/
//=================================================================================================

#pragma once
#ifndef __SocketConnectionTCP_h
#define __SocketConnectionTCP_h

#include "SocketConnectionBase.h"

class NetMsg;


// Suppress warning c4512 that an assignment operator could not be generated
//#pragma warning( push )
//#pragma warning( disable : 4512 )

//-------------------------------------------------------------------------------------------------
/*!
	\class SocketConnectionTCP
	\brief The class to handle a socket connection.

	This class encapsulates the functionality for sending and receiving data.
*/
//-------------------------------------------------------------------------------------------------
class SocketConnectionTCP : public SocketConnectionBase
{
protected:

	/// The flag for if this connection is listening
	bool m_IsListening;

	/// The port this connection is connected to
	uint16 m_BoundPort;

public:

	/// The constructor to initialize from a socket
	SocketConnectionTCP( SOCKET socket = 0, uint16 boundPort = 0 );

	/// The destructor
	virtual ~SocketConnectionTCP();

	/// Close the connection
	virtual void Close();

	/// Tell the connection to start listening for a connection
	bool Listen();

	/// Stop listening
	void StopListening();

	/// Listen for a TCP connection blocking until complete
	void AcceptConnSync();

	/// Listen for a TCP connection but does not blocl
	bool AcceptConnAsync();

	/// Send a message over this connection
	virtual bool SendData( const NetMsg* pMsg );

	/// Get the bound port
	uint16 GetBoundPort() const { return m_BoundPort; }

	/// Check if this object is currently connection
	bool IsConnected() const { return m_Socket != 0; }

	/// Get if the socket is listening
	bool IsListening() const { return m_IsListening; }
};

//#pragma warning( pop )

#endif // __SocketConnectionTCP_h