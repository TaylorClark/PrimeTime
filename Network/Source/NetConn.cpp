//=================================================================================================
/*!
	\file NetConn.cpp
	Network Library
	Network Connections Source
	\author Taylor Clark
	\date August 28, 2006

	This source file contains the implementation for the LAN network connection class.
*/
//=================================================================================================

#include "../NetConnLAN.h"
#include "../NetConnTCP.h"
#include "../NetworkMgr.h"
#include "../NetMsg.h"


/// The default constructor
NetworkConnection::NetworkConnection() : m_ConnPort( 0 )
{
	m_hMutex = CreateMutex( NULL, FALSE, NULL );
}

/// The default destructor
NetworkConnection::~NetworkConnection()
{
	// Free any remaining read messages
	for( NetMsgList::iterator iterMsg = m_PendingReadMessages.begin(); iterMsg != m_PendingReadMessages.end(); ++iterMsg )
		delete *iterMsg;
	m_PendingReadMessages.clear();

	// Free the mutex
	if( m_hMutex )
	{
		CloseHandle( m_hMutex );
		m_hMutex = NULL;
	}
}

/// Close the connection
void NetConnTCP::Close()
{
	m_Conn.Close();
	while( m_ConnState == CS_Listening )
		Sleep( 50 );
	m_ConnState = CS_Invalid;
}


/// Send a message over this connection
void NetConnTCP::SendNetMsg( const NetMsg* pMsg )
{
	if( !pMsg )
		return;
	m_Conn.SendData( pMsg );
}

/// Send a message over this connection
void NetConnLAN::SendNetMsg( const NetMsg* pMsg )
{
	if( !pMsg )
		return;
	m_SendConn.SendData( pMsg );
}

void NetConnLAN::ReceiveData()
{
	NetMsg* pNewMsg = m_RecvConn.ReceiveData();
	if( pNewMsg )
		m_PendingReadMessages.push_back( pNewMsg );
}

void NetConnTCP::ReceiveData()
{
	NetMsg* pNewMsg = m_Conn.ReceiveData();

	// If a message was received
	if( pNewMsg )
		m_PendingReadMessages.push_back( pNewMsg );
	// Otherwise check if there was an error
	else
	{
		if( !m_Conn.IsValid() )
			m_ConnState = CS_Invalid;
	}
}

/// Determine if the connection is valid
bool NetConnLAN::IsValid() const
{
	return m_SendConn.IsValid() || m_RecvConn.IsValid();
}

bool NetConnTCP::IsValid() const
{
	return m_ConnState != CS_Invalid;
}