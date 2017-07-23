//=================================================================================================
/*!
	\file NetConnection.cpp
	Network Library
	Network Connection Source
	\author Taylor Clark
	\date July 5, 2006

	This source file contains the implementation for the network connection class.
*/
//=================================================================================================

#include "SocketConnectionBroadcast.h"
#include "SocketMgr.h"
#include "NetDefines.h"
#include "../IPAddress.h"
#include "Base/MsgLogger.h"
using namespace std;



/// The constructor to initialize from a socket
SocketConnectionBroadcast::SocketConnectionBroadcast( SOCKET socket, uint16 port ) : m_Port( port )
{
	m_Socket = socket;
	//m_BoundPort = boundPort;
	m_SendToIP = SocketMgr::CalculateBroadcastIP( SocketMgr::Get().GetLocalIP(), SocketMgr::Get().GetLocalSubnetMask() );
}

bool SocketConnectionBroadcast::SendData( const NetMsg* pMsg )
{
	if( m_Socket == 0 || !pMsg )
		return false;

	sockaddr_in toAddress;
	toAddress.sin_family = AF_INET;
	toAddress.sin_addr.S_un.S_addr = m_SendToIP.GetVal();
	toAddress.sin_port = m_Port;
	
	// Store the message count
	NetDataBlockOut dataOut( m_SendBuffer, NetDefines::MSG_BUFFER_SIZE );
	uint16 msgCount = 0;
	dataOut.WriteVal16( msgCount );
	dataOut.WriteVal16( pMsg->GetMsgTypeID() );
	pMsg->StoreInBuffer( dataOut );

	// Send the message
	if( sendto( m_Socket, (const char*)m_SendBuffer, (int)dataOut.GetCurPosition(), 0, (sockaddr*)&toAddress, (int)sizeof(toAddress) ) == SOCKET_ERROR )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to send data: %s", NetDefines::GetWinsockErrorDesc(WSAGetLastError()) );
		return false;
	}
	
	return true;
}