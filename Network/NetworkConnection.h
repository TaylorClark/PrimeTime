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
#ifndef __NetConn_h
#define __NetConn_h

#include "Base/Types.h"
#include <list>
class NetMsg;
class NetworkMgr;

class NetworkConnection
{
protected:
	friend NetworkMgr;

	typedef std::list< NetMsg* > NetMsgList;
	NetMsgList m_PendingReadMessages;

	/// Close the connection
	virtual void Close() = 0;

public:

	/// The port this connection is connected to or, in the case of a broadcasting LAN connection,
	///	sends to
	uint16 m_ConnPort;

	/// The mutex used externally for synchronizing updates to this connection such as receiving
	/// messages and closing the connection
	void* m_hMutex;

	/// The default constructor
	NetworkConnection();

	/// The default destructor
	virtual ~NetworkConnection();

	/// Check if there are any messages pending
	bool HasPendingMessages(){ return m_PendingReadMessages.size() != 0; }

	/// Get the next message.  Note that the user is responsible for freeing the memory after
	///	popping the message
	NetMsg* PopNextMessage()
	{ 
		NetMsg* pRetVal = 0;
		if( m_PendingReadMessages.size() > 0 )
		{
			pRetVal = m_PendingReadMessages.front();
			m_PendingReadMessages.pop_front();
		}
		return pRetVal;
	}

	/// Send a message over this connection
	virtual void SendNetMsg( const NetMsg* pMsg ) = 0;

	virtual void ReceiveData() = 0;

	/// Determine if the connection is valid
	virtual bool IsValid() const = 0;
};

#endif // __NetConn_h