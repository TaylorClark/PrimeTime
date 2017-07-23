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

#include "SocketConnectionBase.h"
#include "SocketMgr.h"
#include "NetDefines.h"
#include <windows.h>
#include "../NetMsg.h"
#include <iostream>
#include "Base/MsgLogger.h"
#include "../NetworkMgr.h"
using namespace std;


NetMsg* SocketConnectionBase::ReceiveData()
{
	if( m_Socket == 0 )
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

	// See if data is ready to be read
	int selectRetVal = select( 0, &socketSet, NULL, NULL, &timeout );
	if( selectRetVal == -1 )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to check is socket is waiting for data: %s", NetDefines::GetWinsockErrorDesc(WSAGetLastError()) );
		return 0;
	}

	// If the socket is ready to read then receive the data
	if( selectRetVal == 1 )
		return ReceiveDataWait();

	// Can't seem to do anything
	return 0;
}

NetMsg* SocketConnectionBase::ReceiveDataWait()
{
	if( m_Socket == 0 )
		return 0;

	// Read the message and store the sender's address
	sockaddr_in sockAddr;
	memset( &sockAddr, 0, sizeof(sockAddr) );
	int sockAddrSize = sizeof(sockAddr);
	int bytesRead = recvfrom( m_Socket, (char*)m_ReadBuffer, NetDefines::MSG_BUFFER_SIZE, 0, (sockaddr*)&sockAddr, &sockAddrSize );
	if( bytesRead == SOCKET_ERROR )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to receive data : %s", NetDefines::GetWinsockErrorDesc(WSAGetLastError()) );
		Close();
		return 0;
	}
	
	// If the IP address is our own
	IPAddress senderIP( (uint32)sockAddr.sin_addr.S_un.S_addr );
	if( senderIP == SocketMgr::Get().GetLocalIP() )
		return 0;

	// Attempt to create the message and return it
	return NetworkMgr::Get().CreateMsgFromData( NetDataBlock( m_ReadBuffer, bytesRead ), senderIP, sockAddr.sin_port );
}

void SocketConnectionBase::Close()
{
	if( m_Socket != 0 )
		closesocket( m_Socket );
	m_Socket = 0;
}