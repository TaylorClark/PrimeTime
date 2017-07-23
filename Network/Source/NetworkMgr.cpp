//=================================================================================================
/*!
	\file NetworkMgr.cpp
	Network Library
	Network Manager Source
	\author Taylor Clark
	\date August 27, 2006

	This source file contains the implementation for the network manager class.
*/
//=================================================================================================

#include "../NetworkMgr.h"
#include "SocketMgr.h"
#include "../NetMsg.h"
#include "Base/MsgLogger.h"
#include "../NetConnTCP.h"
#include "../NetConnLAN.h"
#include "../NetMsg.h"
#include "../HttpRequestMgr.h"
#include <process.h>

static CRITICAL_SECTION m_ActiveConnCritSec;
bool g_CloseAllThreads = false;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  WaitForTCPConn  Global
///
///	\param UNNAMED A pointer to conext sensitive data
///
///	Listen for TCP connections
///
///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI WaitForTCPConn( void* pVoidNetConnTCP )
{
	if( !pVoidNetConnTCP )
		return 0;

	// Place the connection
	NetConnTCP* pNetConnTCP = reinterpret_cast<NetConnTCP*>( pVoidNetConnTCP );
	if( !pNetConnTCP->m_Conn.Listen() )
		return 0;

	// Loop while the socket is listening
	pNetConnTCP->m_ConnState = NetConnTCP::CS_Listening;
	while( !g_CloseAllThreads && pNetConnTCP->m_Conn.IsValid() && pNetConnTCP->m_Conn.IsListening() )
	{
		// If the connection accepts
		if( pNetConnTCP->m_Conn.AcceptConnAsync() )
		{
			pNetConnTCP->m_ConnState = NetConnTCP::CS_Connected;
			break;
		}

		// Give up our time slice
		Sleep( 50 );
	}
	
	if( !pNetConnTCP->m_Conn.IsValid() )
		pNetConnTCP->m_ConnState = NetConnTCP::CS_Invalid;

	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ConnectTCPConn  Global
///
///	\param UNNAMED A pointer to conext sensitive data
///
///	Connect a TCP connection
///
///////////////////////////////////////////////////////////////////////////////////////////////////
//static DWORD WINAPI ConnectTCPConn( void* pVoidNetConnTCP )
//{
//	if( !pVoidNetConnTCP )
//		return 0;
//
//	NetConnTCP* pNetConnTCP = reinterpret_cast<NetConnTCP*>( pVoidNetConnTCP );
//
//	pNetConnTCP->m_Conn = SocketMgr::Get().ConnectToTCPSync( pNetConnTCP->m_ConnIP, pNetConnTCP->m_ConnPort );
//	pNetConnTCP->m_IsConnected = true;
//
//	return 0;
//}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  NetworkMgr::UpdateThread  Static Public
///
///	\param UNNAMED A pointer to conext sensitive data
///
///	Update the connections to receive messages if needed.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkMgr::UpdateThread( void* )
{
	NetworkMgr& networkMgr = NetworkMgr::Get();
	networkMgr.m_ReadThreadActive = true;
	while( !networkMgr.m_IsTerminated )
	{
		// Lock the active connection list
		EnterCriticalSection( &m_ActiveConnCritSec );

		// Go through the active connections
		for( ConnList::iterator iterConn = networkMgr.m_ActiveConns.begin(); iterConn != networkMgr.m_ActiveConns.end(); ++iterConn )
		{
			if( (*iterConn)->IsValid() )
				(*iterConn)->ReceiveData();
		}

		// Unlock the critical section
		LeaveCriticalSection( &m_ActiveConnCritSec );

		// Wait a bit (50ms being 20 updates per second)
		Sleep( 50 );
	}

	networkMgr.m_ReadThreadActive = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  NetworkMgr::Init  Public
///
///	\returns Whether or not the network manager was successfully initialized.
///
///	Initialize the network manager.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool NetworkMgr::Init()
{
	// Clear the flag just in case
	m_IsTerminated = false;
	g_CloseAllThreads = false;

	// Initialize the socket manager
	if( !SocketMgr::Get().Init() )
		return false;

	HttpRequestMgr::Init();

	// Store the local IP
	m_LocalIP = SocketMgr::Get().GetLocalIP();

	// Create the active connection mutex
	InitializeCriticalSection( &m_ActiveConnCritSec );

	// Create the thread to update connections
	//if( CreateThread( NULL, 0, UpdateThread, NULL, 0, NULL ) == NULL )
	/*
	if( _beginthread(UpdateThread, 0, 0 ) == 1 )
	{
		DeleteCriticalSection( &m_ActiveConnCritSec );
		SocketMgr::Get().Term();
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to create thread for NetworkMgr for updating active connections." );
		return false;
	}
	*/

	// Return success
	return true;
}

/// Shutdown the network manager and free any resources
void NetworkMgr::Term()
{
	// Flag the end for threads
	m_IsTerminated = true;
	g_CloseAllThreads = true;
	while( m_ReadThreadActive )
		Sleep( 5 );

	HttpRequestMgr::Term();

	// Free the critcal section
	DeleteCriticalSection( &m_ActiveConnCritSec );

	// Close any open connection
	for( ConnList::iterator iterConn = m_ActiveConns.begin(); iterConn != m_ActiveConns.end(); ++iterConn )
	{
		(*iterConn)->Close();
	}
	m_ActiveConns.clear();

	// Shutdown the network manager
	SocketMgr::Get().Term();
}

/// Close a connection
void NetworkMgr::CloseConnection( NetworkConnection* pNetConn )
{
	if( !pNetConn )
		return;

	EnterCriticalSection( &m_ActiveConnCritSec );

	// Remove the connection from the list
	for( ConnList::iterator iterConn = m_ActiveConns.begin(); iterConn != m_ActiveConns.end(); ++iterConn )
	{
		if( *iterConn == pNetConn )
		{
			m_ActiveConns.erase( iterConn );
			break;
		}
	}

	// Unlock the critical section
	LeaveCriticalSection( &m_ActiveConnCritSec );

	// Close the connection
	pNetConn->Close();
}

/// Create a network connection for sending broadcast messages over the network
NetConnLAN* NetworkMgr::CreateLANBroadcastConn( uint16 port )
{
	return CreateLANBroadcastConn( port, port );
}

/// Create a network connection for sending broadcast messages over the network
NetConnLAN* NetworkMgr::CreateLANBroadcastConn( uint16 sendToPort, uint16 listenPort )
{
	NetConnLAN* pLANConn = new NetConnLAN();
	pLANConn->m_ConnPort = sendToPort;

	pLANConn->m_RecvConn = SocketMgr::Get().CreateLANBroadcastListener( listenPort );
	pLANConn->m_SendConn = SocketMgr::Get().CreateLANBroadcastSender( sendToPort );

	if( pLANConn )
	{
		EnterCriticalSection( &m_ActiveConnCritSec );
		m_ActiveConns.push_back( pLANConn );
		LeaveCriticalSection( &m_ActiveConnCritSec );
	}

	return pLANConn;
}

/// Create a network connection for receiving LAN broadcast messages
NetConnLAN* NetworkMgr::CreateLANListenConn( uint16 listenPort )
{
	NetConnLAN* pLANConn = new NetConnLAN();
	
	pLANConn->m_RecvConn = SocketMgr::Get().CreateLANBroadcastListener( listenPort );
	pLANConn->m_ConnPort = pLANConn->m_RecvConn.GetPort();

	if( pLANConn )
	{
		EnterCriticalSection( &m_ActiveConnCritSec );
		m_ActiveConns.push_back( pLANConn );
		LeaveCriticalSection( &m_ActiveConnCritSec );
	}

	return pLANConn;
}

/// Create a network connection for sending LAN broadcast messages, not receiving
NetConnLAN* NetworkMgr::CreateLANSendConn( uint16 sendToPort )
{
	NetConnLAN* pLANConn = new NetConnLAN();
	pLANConn->m_ConnPort = sendToPort;

	pLANConn->m_SendConn = SocketMgr::Get().CreateLANBroadcastSender( sendToPort );

	// Since this is a sending connection then there is no need to add it to the list of
	// connections that need updating

	return pLANConn;
}

/// Create a network connection using TCP
NetConnTCP* NetworkMgr::ConnectToTCPConn( IPAddress connIP, uint16 connPort )
{
	// Create the network connection
	NetConnTCP* pTCPConn = new NetConnTCP();
	pTCPConn->m_ConnPort = connPort;
	pTCPConn->m_ConnIP = connIP;

	// Connect to the passed-in address
	pTCPConn->m_Conn = SocketMgr::Get().ConnectToTCPSync( connIP, connPort );
	if( pTCPConn->m_Conn.IsConnected() )
		pTCPConn->m_ConnState = NetConnTCP::CS_Connected;

	// Start the thread that will listen for the connection
	//if( CreateThread( NULL, 0, ::ConnectTCPConn, pTCPConn, 0, NULL ) == NULL )
	//	MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to create thread for TCP connection." );
	
	if( pTCPConn )
	{
		EnterCriticalSection( &m_ActiveConnCritSec );
		m_ActiveConns.push_back( pTCPConn );
		LeaveCriticalSection( &m_ActiveConnCritSec );
	}

	return pTCPConn;
}

/// Create a network connection using TCP and wait for a connection
NetConnTCP* NetworkMgr::WaitForTCPConn( uint16 listenPort, uint16 portRange )
{
	// Create the network connection
	NetConnTCP* pTCPConn = new NetConnTCP();
	pTCPConn->m_Conn = SocketMgr::Get().CreateListenTCP( listenPort, portRange );
	pTCPConn->m_ConnPort = pTCPConn->m_Conn.GetBoundPort();

	// Start the thread that will listen for the connection
	if( CreateThread( NULL, 0, ::WaitForTCPConn, pTCPConn, 0, 0 ) == NULL )
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to create thread to listen for TCP connection." );

	if( pTCPConn )
	{
		EnterCriticalSection( &m_ActiveConnCritSec );
		m_ActiveConns.push_back( pTCPConn );
		LeaveCriticalSection( &m_ActiveConnCritSec );
	}

	return pTCPConn;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	TestFirewall  Global
///
///	Test the firewall by sending and attempting to receive data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkMgr::TestFirewall()
{
	// Create a UDP broadcast connection
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Testing creation of a UDP sender/receiver connection" );
	NetConnLAN* pUDPConn = NetworkMgr::Get().CreateLANBroadcastConn( 1234, 12345 );

	// Close the game TCP connection
	if( pUDPConn )
	{
		NetworkMgr::Get().CloseConnection( pUDPConn );
		delete pUDPConn;
		pUDPConn = 0;
	}

	//MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Testing opening a connection for receiving data" );
}


/// Register a network message
void NetworkMgr::RegisterNetMsg( uint32 msgID, MsgCreateFunc pFunc )
{
	// If the message is already registered
	if( m_MsgCreateMap.find( msgID ) != m_MsgCreateMap.end() )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Message ID already registered a creation funcion." );
		m_MsgCreateMap[ msgID ] = pFunc;
	}
	// Else store the creation function
	else
		m_MsgCreateMap.insert( std::pair< uint32, MsgCreateFunc>( msgID, pFunc ) );
}

/// Create a message from data received over the network
NetMsg* NetworkMgr::CreateMsgFromData( NetDataBlock dataBlock, IPAddress senderIP, uint16 senderPort )
{
	// If there is not even enough data for the info then bail
	if( dataBlock.GetSize() < sizeof(uint16) * 2 )
		return 0;

	// The first 16-bit value is the count
	dataBlock.ReadUint16();

	// The next 16-bit value is the message ID
	uint16 msgID = dataBlock.ReadUint16();

	// Get the callback function
	MsgCreateMap::iterator iterFunc = m_MsgCreateMap.find( msgID );
	if( iterFunc == m_MsgCreateMap.end() )
		return 0;

	// Create the message
	NetMsg* pRetMsg = 0;
	try
	{
		pRetMsg = iterFunc->second( dataBlock );
	}
	catch( ... )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Bad network data received." );
	}

	// If a valid message was created then store the sender's address
	if( pRetMsg )
	{
		pRetMsg->m_SenderIP = senderIP;
		pRetMsg->m_SenderPort = senderPort;
	}

	// Return the message
	return pRetMsg;
}