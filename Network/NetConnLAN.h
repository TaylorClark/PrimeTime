//=================================================================================================
/*!
	\file NetConnLAN.h
	Network Library
	Network Connection Header
	\author Taylor Clark
	\date August 28, 2006

	This file contains the definition for the LAN UDP broadcast network connection class.
*/
//=================================================================================================

#pragma once
#ifndef __NetConnLAN_h
#define __NetConnLAN_h

#include "Base/Types.h"
#include "NetworkConnection.h"
//#include "NetworkMgr.h"
#include "PrivateInclude/SocketConnectionBroadcast.h"


class NetConnLAN :  public NetworkConnection
{
public:

	/// The sending socket connection
	SocketConnectionBroadcast m_SendConn;

	/// The receiving socket connection
	SocketConnectionBroadcast m_RecvConn;

	/// Close the connection
	virtual void Close()
	{
		m_SendConn.Close();
		m_RecvConn.Close();
	}

public:

	/// Send a message on this connection
	virtual void SendNetMsg( const NetMsg* pMsg );

	/// Receive data waiting on the queue into a message
	virtual void ReceiveData();

	/// Prevent or allow incoming messages
	void SetAllowReceive( bool allowReceive );

	/// Determine if the connection is valid
	virtual bool IsValid() const;
};

#endif // __NetConnLAN_h