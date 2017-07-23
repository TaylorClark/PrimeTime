//=================================================================================================
/*!
	\file SocketMgr.h
	Network Library
	Socket Manager Header
	\author Taylor Clark
	\date July 3, 2006

	This file contains the definition for the socket manager class.
*/
//=================================================================================================

#pragma once
#ifndef __SocketMgr_h
#define __SocketMgr_h

#include "Base/Types.h"
#include "Base/DataBlock.h"
#include "../NetMsg.h"
#include "SocketConnectionTCP.h"
#include "SocketConnectionBroadcast.h"
#include "../IPAddress.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class SocketMgr
	\brief The class to handle socket functionality.

	This class defines the manager for all socket related functionality.  It uses the singleton
	pattern so that there is one and only instance of the class.
*/
//-------------------------------------------------------------------------------------------------
class SocketMgr
{
private:

	/// The local machine's IP address
	IPAddress m_LocalIP;

	/// The local machine's subnet mask
	IPAddress m_SubnetMask;

	/// The default constructor, private since this is a singleton
	SocketMgr() : m_LocalIP( "127.0.0.1" ),
					m_SubnetMask("255.255.255.255")
	{}

public:

	/// Get the one and only socket manager instance
	static SocketMgr& Get()
	{
		static SocketMgr s_SocketMgr;
		return s_SocketMgr;
	}

	/// The default destructor
	~SocketMgr() { Term(); }

	/// Initialize the socket manager
	bool Init();

	/// Clean up the network manager
	void Term();

	/// Create a TCP socket for listening
	SocketConnectionTCP CreateListenTCP( uint16 startPort, uint16 range );

	/// Create a connection to another computer synchronously
	SocketConnectionTCP ConnectToTCPSync( IPAddress foreignIP, uint16 foreignPort );

	/// Create a LAN connection for broadcasting messages
	SocketConnectionBroadcast CreateLANBroadcastSender( uint16 foreignPort );

	/// Create a LAN connection for receiving UDP messages
	SocketConnectionBroadcast CreateLANBroadcastListener( uint16 localPort );

	/// Calculate the broadcast IP for a computer on the network
	static IPAddress CalculateBroadcastIP( IPAddress cpuIP, IPAddress cpuSubnetMask );

	/// Get the local IP address
	IPAddress GetLocalIP() const { return m_LocalIP; }

	/// Get the local subnet masn
	IPAddress GetLocalSubnetMask() const { return m_SubnetMask; }
};

#endif // __SocketMgr_h