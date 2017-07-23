//=================================================================================================
/*!
	\file SocketConnectionBase.h
	Network Library
	Socket Connection Header
	\author Taylor Clark
	\date July 5, 2006

	This file contains the definition for the socket connection class.
*/
//=================================================================================================

#pragma once
#ifndef __SocketConnectionBase_h
#define __SocketConnectionBase_h

#include "Base/Types.h"
#include "NetDefines.h"
#include <winsock2.h>

class NetMsg;


// Suppress warning c4512 that an assignment operator could not be generated
//#pragma warning( push )
//#pragma warning( disable : 4512 )

//-------------------------------------------------------------------------------------------------
/*!
	\class SocketConnectionBase
	\brief The base class to handle a socket connection.

	This class encapsulates the functionality for sending and receiving data.
*/
//-------------------------------------------------------------------------------------------------
class SocketConnectionBase
{
protected:

	/// The socket this connection uses to communicate
	SOCKET m_Socket;

	/// The read buffer
	uint8 m_ReadBuffer[ NetDefines::MSG_BUFFER_SIZE ];

	/// The send buffer
	uint8 m_SendBuffer[ NetDefines::MSG_BUFFER_SIZE ];

public:

	/// The destructor
	virtual ~SocketConnectionBase() {}

	/// Close the connection
	virtual void Close();

	/// Send a message over this connection
	virtual bool SendData( const NetMsg* pMsg ) = 0;

	/// Receive a message if one is available
	virtual NetMsg* ReceiveData();

	/// Wait until a message is available and receive it
	virtual NetMsg* ReceiveDataWait();

	/// Check if this object is currently connection
	bool IsValid() const { return m_Socket != 0; }
};

//#pragma warning( pop )

#endif // __SocketConnectionBase_h