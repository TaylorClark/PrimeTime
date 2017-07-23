//=================================================================================================
/*!
	\file NetConnTCP.h
	Network Library
	Network Connection Header
	\author Taylor Clark
	\date August 28, 2006

	This file contains the definition for the TCP network connection class.
*/
//=================================================================================================

#pragma once
#ifndef __NetConnTCP_h
#define __NetConnTCP_h

#include "Base/Types.h"
#include "NetworkConnection.h"
#include "PrivateInclude/SocketConnectionTCP.h"


class NetConnTCP :  public NetworkConnection
{
public:

	enum ConnState
	{
		CS_Invalid = 0,
		CS_Listening,
		CS_Connected,
		CS_Disconnected
	};

	/// The socket connection
	SocketConnectionTCP m_Conn;

	/// The connection state
	ConnState m_ConnState;

	/// If this connection is waiting to connected or is connected
	//bool m_IsConnected;

	/// A flag representing if this connection is waiting for a connection
	//bool m_IsWaitingForConn;

	/// The IP address to which this connection sends
	IPAddress m_ConnIP;

	/// The port on which this connection listens
	uint16 m_LocalPort;

public:

	/// The default constructor
	NetConnTCP() : m_ConnState( CS_Invalid )
	{
	}

	/// Get the connection state
	ConnState GetConnState() const { return m_ConnState; }

	/// Check if this connection is connected yet
	//bool IsConnected() const { return m_IsConnected; }

	/// Get port this connection listens on
	uint16 GetListenPort() const { return m_Conn.GetBoundPort(); }

	/// Send a message over this connection
	virtual void SendNetMsg( const NetMsg* pMsg );

	/// Close the connection
	virtual void Close();

	/// Receive any data that might be waiting on the socket
	virtual void ReceiveData();

	/// Determine if the connection is valid
	virtual bool IsValid() const;
};

#endif // __NetConnTCP_h