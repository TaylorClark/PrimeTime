//=================================================================================================
/*!
	\file SocketMgr.cpp
	Network Library
	Socket Manager Source
	\author Taylor Clark
	\date July 3, 2006

	This source file contains the implementation for the socket manager class.
*/
//=================================================================================================
#include "SocketMgr.h"
#include <Limits.h>
#include <iostream>
#include <winsock2.h>
#include "NetDefines.h"
#include <iphlpapi.h>
#include "Base/MsgLogger.h"
using namespace std;


#define OUTPUT_ERROR_MSG( msg ) MSG_LOGGER_OUT( MsgLogger::MI_Error, L"%s (%s)", msg, NetDefines::GetWinsockErrorDesc(WSAGetLastError()) );


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SocketMgr::Init()  Public
///
///	\returns True if the socket manager was initialized successfully otherwise false
///
///	Initialize the socket manager.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool SocketMgr::Init()
{
	// Startup winsock
	WSADATA wsaData;
	int startupResult = WSAStartup( MAKEWORD(1,1), &wsaData );
	if( startupResult != 0 )
	{
		OUTPUT_ERROR_MSG( L"Failed to initialize sockets :" );
		return false;
	}

	// Test the version we received
	if( LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1 )
	{
		OUTPUT_ERROR_MSG( L"Failed to retrieve desired winsock version :" );
		WSACleanup();
		return false;
	}

	// Get this computer's name
	const int HOST_NAME_LEN = 80;
	char szHostName[ HOST_NAME_LEN  ] = {0};
	gethostname( szHostName, HOST_NAME_LEN );

	// Get the address from the host name
	hostent* pHostData = gethostbyname( szHostName );
	if( pHostData )
	{
		for( int addrIndex = 0; pHostData->h_addr_list[addrIndex] != 0; ++addrIndex )
		{
			in_addr addr;
			memcpy( (void*)&addr, (void*)pHostData->h_addr_list[addrIndex], sizeof(in_addr) );
			m_LocalIP = IPAddress( (uint32)addr.S_un.S_addr );
		}
	}

	// Initialize the data to retrieve the adapter information
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}

	// Get the adapter info
	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
	
		// We could loop through all of the adapters, but we only want the first
		while( pAdapter )
		{
			if( pAdapter->Type == MIB_IF_TYPE_ETHERNET ) {}
			IPAddress curSubnetMask( pAdapter->IpAddressList.IpMask.String );
			if( curSubnetMask.GetVal() != 0 )
			{
				m_SubnetMask = curSubnetMask;
				break;
			}
			
			// Step to the next adapter
			pAdapter = pAdapter->Next;
		}
	}
	free(pAdapterInfo);

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SocketMgr::Term  Public
///
///	Close down the socket manager.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void SocketMgr::Term()
{
	WSACleanup();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SocketMgr::CreateListenTCP  Public
///
///	\param startPort The port to start from when attempting to listen
///	\param range The number of ports to try in case the startPort is in use
///	\returns pFunc A callback function for when a connection is made
///
///	Listen for a TCP connection.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
SocketConnectionTCP SocketMgr::CreateListenTCP( uint16 startPort, uint16 range )
{
	uint16 port = startPort;

	// Store the end port index and avoid overflow
	uint16 endPort = startPort + range;
	if( endPort < startPort )
		endPort = USHRT_MAX;

	// Create the socket
	SOCKET sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( sock == INVALID_SOCKET )
	{
		OUTPUT_ERROR_MSG( L"Failed to create socket, error :" );
		return SocketConnectionTCP();
	}

	// Get the buffer size and ensure it is big enough
	uint32 buffSize = 0;
	int32 buffSizeSize = (int32)sizeof(buffSize);
	if( getsockopt( sock, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&buffSize, &buffSizeSize ) == SOCKET_ERROR )
	{
		OUTPUT_ERROR_MSG( L"Failed to obtain maximum buffer size, error :" );
	}
	// Else if the buffer size is too small
	else if( buffSize < NetDefines::MSG_BUFFER_SIZE )
	{
		wcout << L"Message buffer size is too small for PrimeTime" << endl;
		return SocketConnectionTCP();
	}

	bool successfulBind = false;
	do
	{
		// Attempt to create the connection
		sockaddr_in connInfo;
		connInfo.sin_addr.s_addr = m_LocalIP.GetVal();//inet_addr( "127.0.0.1" );
		connInfo.sin_family = AF_INET;
		connInfo.sin_port = htons( (unsigned short)port );

		// Attempt to bind the connection
		if( bind( sock, (SOCKADDR*)&connInfo, sizeof(connInfo) ) == SOCKET_ERROR )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Failed to bind socket (port %d) for listening for TCP connection : %s", port, NetDefines::GetWinsockErrorDesc( WSAGetLastError() ) );
			++port;
			if( startPort == USHRT_MAX )
				break;
		}
		else
			successfulBind = true;

	}while( port < endPort && !successfulBind );

	// If we couldn't bind to a valid port
	if( !successfulBind )
		return SocketConnectionTCP();

	return SocketConnectionTCP( sock, port );
}


/// Connect to an computer
SocketConnectionTCP SocketMgr::ConnectToTCPSync( IPAddress foreignIP, uint16 foreignPort )
{
	// Create the socket
	SOCKET sock;
	sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( sock == INVALID_SOCKET )
	{
		OUTPUT_ERROR_MSG( L"Failed to create socket, error :" );
		return SocketConnectionTCP();
	}

	// Attempt to create the connection
	sockaddr_in connInfo;
	connInfo.sin_family = AF_INET;
	connInfo.sin_addr.s_addr = foreignIP.GetVal();
	connInfo.sin_port = htons( foreignPort );

	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Attempting TCP connection at IP %s:%d", foreignIP.ToString(), foreignPort );
	// Attempt to connect, waiting if necessary
	if( connect( sock, (SOCKADDR*)&connInfo, sizeof(connInfo) ) == SOCKET_ERROR )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to connect TCP connection" );
		sock = 0;
	}
	else
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Connected TCP connection" );

	return SocketConnectionTCP( sock );
}


SocketConnectionBroadcast SocketMgr::CreateLANBroadcastSender( uint16 foreignPort )
{
	// Create the socket
	SOCKET sock;
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( sock == INVALID_SOCKET )
	{
		OUTPUT_ERROR_MSG( L"Failed to create socket, error :" );
		return SocketConnectionBroadcast();
	}

	// Allow broadcasting
	BOOL optVal = TRUE;
	if( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (const char*)&optVal, sizeof(optVal) ) == SOCKET_ERROR )
	{
		OUTPUT_ERROR_MSG( L"Failed to set socket options, error :" );
		return SocketConnectionBroadcast();
	}

	// Return the connection
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Create LAN broadcaster to send to port %d", foreignPort );
	return SocketConnectionBroadcast( sock, foreignPort );
}


SocketConnectionBroadcast SocketMgr::CreateLANBroadcastListener( uint16 localPort )
{
	// Create the socket
	SOCKET sock;
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( sock == INVALID_SOCKET )
	{
		OUTPUT_ERROR_MSG( L"Failed to create socket, error :" );
		return SocketConnectionBroadcast();
	}

	// Allow broadcasting
	BOOL optVal = TRUE;
	if( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (const char*)&optVal, sizeof(optVal) ) == SOCKET_ERROR )
	{
		OUTPUT_ERROR_MSG( L"Failed to set socket options, error :" );
		return SocketConnectionBroadcast();
	}

	// Initialize the address structure for this socket to use
	sockaddr_in msgAddr;
	msgAddr.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
	msgAddr.sin_family = AF_INET;
	msgAddr.sin_port = localPort;

	// Bind the port
	if( bind( sock, (sockaddr*)&msgAddr, sizeof(msgAddr) ) == SOCKET_ERROR )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to bind broadcast listener socket (port %u) :", localPort, NetDefines::GetWinsockErrorDesc(WSAGetLastError()) );
		closesocket( sock );
		return SocketConnectionBroadcast();
	}

	// Return the connection
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Created LAN listener on port %d", localPort );
	return SocketConnectionBroadcast( sock, localPort );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SocketMgr::CalculateBroadcastIP  Public
///
///	\param cpuIP The IP of the computer
///	\param cpuSubnetMask The subnet mask of the computer
///	\returns The IP address that can be used to broadcast a message to all computers on the local
///				network.
///
///	Calculate the broadcast IP for a computer on the network.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
IPAddress SocketMgr::CalculateBroadcastIP( IPAddress cpuIP, IPAddress cpuSubnetMask )
{
	// Create the LAN IP
	uint32 sendAddr = cpuIP.GetVal() & cpuSubnetMask.GetVal();
	sendAddr = sendAddr | (~cpuSubnetMask.GetVal());

	return IPAddress( sendAddr );
}