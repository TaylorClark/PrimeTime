//=================================================================================================
/*!
	\file Network.h
	Network Library
	Network Manager Header
	\author Taylor Clark
	\date August 24, 2006

	This file contains the definition for the network manager class.
*/
//=================================================================================================

#pragma once
#ifndef __NetworkMgr_h
#define __NetworkMgr_h

#include <list>
#include "IPAddress.h"
#include "NetDataBlock.h"
#include <map>
class NetMsg;
class NetworkConnection;
class NetConnTCP;
class NetConnLAN;
//typedef void (*OnConnect)( NetConnection msg );


/// The callback function type for creating messages
typedef NetMsg* (*MsgCreateFunc)( NetDataBlock );


//-------------------------------------------------------------------------------------------------
/*!
	\class NetworkMgr
	\brief The class to handle network functionality.

	This class defines the manager for network message and connection related functionality.  This
	class follows the singleton pattern.
*/
//-------------------------------------------------------------------------------------------------
class NetworkMgr
{
public:

	/// The default constructor, private since this class follows the singleton pattern
	NetworkMgr() : m_IsTerminated( false ),
					m_ReadThreadActive( false )
	{
	}

	/// The map of network message callback functions
	typedef std::map< uint32, MsgCreateFunc> MsgCreateMap;
	MsgCreateMap m_MsgCreateMap;

	/// The active connections
	typedef std::list< NetworkConnection* > ConnList;
	ConnList m_ActiveConns;

	/// The flag that is set when the network manager is closed to let threads no to exit
	bool m_IsTerminated;

	/// Update the connections to receive messages if needed
	static void UpdateThread( void* pData );

	/// Whether or not the read thread is running
	bool m_ReadThreadActive;

	/// The local IP address
	IPAddress m_LocalIP;

public:

	/// Get the one and only network manager instance
	static NetworkMgr& Get()
	{
		static NetworkMgr s_Mgr;
		return s_Mgr;
	}

	/// Initialize the network manager
	bool Init();

	/// Shutdown the network manager and free any resources
	void Term();

	/// Create a network connection for sending and receivign broadcast messages over the LAN
	/// network
	NetConnLAN* CreateLANBroadcastConn( uint16 port );

	/// Create a network connection for sending and receivign broadcast messages over the LAN
	/// network
	NetConnLAN* CreateLANBroadcastConn( uint16 sendToPort, uint16 listenPort );

	/// Create a network connection for receiving LAN broadcast messages, not sending
	NetConnLAN* CreateLANListenConn( uint16 listenPort );

	/// Create a network connection for sending LAN broadcast messages, not receiving
	NetConnLAN* CreateLANSendConn( uint16 sendToPort );

	/// Create a network connection using TCP
	NetConnTCP* ConnectToTCPConn( IPAddress destIP, uint16 destPort );

	/// Create a network connection using TCP and wait for a connection
	NetConnTCP* WaitForTCPConn( uint16 listenPort, uint16 portRange = 1 );

	/// Create connections to test the Windows XP firewall
	void TestFirewall();

	/// Close a connection
	void CloseConnection( NetworkConnection* pNetConn );

	/// Register a network message
	void RegisterNetMsg( uint32 msgID, MsgCreateFunc pFunc );

	/// Create a message from data received over the network
	NetMsg* CreateMsgFromData( NetDataBlock dataBlock, IPAddress senderIP, uint16 senderPort );

	/// Get the local IP address of the network adapter being used
	IPAddress GetLocalIP() const { return m_LocalIP; }
};

#endif // __NetworkMgr_h