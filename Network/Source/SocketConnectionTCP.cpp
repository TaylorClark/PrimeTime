//=================================================================================================
/*!
	\file SocketConnection.cpp
	Network Library
	Network Connection Source
	\author Taylor Clark
	\date July 5, 2006

	This source file contains the implementation for the network connection class.
*/
//=================================================================================================

#include "SocketConnectionTCP.h"
#include "SocketMgr.h"
#include "NetDefines.h"
#include <windows.h>
#include "../NetMsg.h"
#include <iostream>
#include "Base/MsgLogger.h"
#include "../NetworkMgr.h"
using namespace std;


/// The constructor to initialize from a socket
SocketConnectionTCP:: SocketConnectionTCP( SOCKET socket, uint16 boundPort ) : m_BoundPort( boundPort ),
														m_IsListening( false )
{
	m_Socket = socket;
}

SocketConnectionTCP::~SocketConnectionTCP()
{
}

/// Tell the connection to start listening for a connection
bool SocketConnectionTCP::Listen()
{
	// If the socket is already listening then do nothgin
	if( m_IsListening )
		return true;

	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Listening on port %d for TCP connection", m_BoundPort );
	if( listen( m_Socket, 1 ) == SOCKET_ERROR )
	{
		// We failed to start listening
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to start listening : %s", NetDefines::GetWinsockErrorDesc(WSAGetLastError()) );
		m_IsListening = false;
		return false;
	}
	else
		m_IsListening = true;

	// Return the listening state
	return m_IsListening;
}

/// Stop listening
void SocketConnectionTCP::StopListening()
{
	if( !m_IsListening )
		return;

	m_IsListening = false;
	
}

/// Listen for a TCP connection and block execution until one is made
void SocketConnectionTCP::AcceptConnSync()
{
	// If the connection is not listening then bail
	if( !m_IsListening )
		return;

	// Accept connections
	SOCKET acceptSocket = (SOCKET)SOCKET_ERROR;
	sockaddr_in sockAddr;
	int sockAddrSize = sizeof(sockAddr);
	acceptSocket = accept( m_Socket, (SOCKADDR*)&sockAddr, &sockAddrSize );
	if( acceptSocket == INVALID_SOCKET )
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to accept TCP connection : %s", NetDefines::GetWinsockErrorDesc(WSAGetLastError()) );
	else
	{
		m_Socket = acceptSocket;
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Accepted TCP connection" );
	}
}

bool SocketConnectionTCP::AcceptConnAsync()
{
	// If the connection is not listening then bail
	if( !m_IsListening || m_Socket == 0 )
		return false;

	// Set the timeout be be instant
	TIMEVAL timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	// Add our socket to be checked
	fd_set socketSet;
	memset( &socketSet, 0, sizeof(socketSet) );

// Suppress warning c4127 conditional express is constant
#pragma warning( push )
#pragma warning( disable : 4127 )
	FD_SET( m_Socket, &socketSet );
#pragma warning( pop )

	// See if data is ready to be read, meaning that a connection is available
	int selectRetVal = select( 0, &socketSet, NULL, NULL, &timeout );
	if( selectRetVal == SOCKET_ERROR )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to check is socket is waiting for data: %s", NetDefines::GetWinsockErrorDesc(WSAGetLastError()) );
		return false;
	}
	else if( selectRetVal == 0 )
		return false;

	// Accept the connection
	AcceptConnSync();
	return true;
}

bool SocketConnectionTCP::SendData( const NetMsg* pMsg )
{
	if( m_Socket == 0 || !pMsg )
		return false;

	// Fill in the send buffer
	NetDataBlockOut dataOut( m_SendBuffer, NetDefines::MSG_BUFFER_SIZE );
	uint16 msgCount = 0;
	dataOut.WriteVal16( msgCount );
	dataOut.WriteVal16( pMsg->GetMsgTypeID() );
	pMsg->StoreInBuffer( dataOut );

	// Send the message
	if( send( m_Socket, (const char*)m_SendBuffer, (int)dataOut.GetCurPosition(), 0 ) == SOCKET_ERROR )
	{
		int errorCode = WSAGetLastError();
		if( errorCode == WSAENOTCONN )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Lost TCP connection" );
		}
		else
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to send data on TCP socket: %s", NetDefines::GetWinsockErrorDesc(errorCode) );
		
		return false;
	}
	
	return true;
}

void SocketConnectionTCP::Close()
{
	SocketConnectionBase::Close();
	m_IsListening = false;
}