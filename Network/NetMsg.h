//=================================================================================================
/*!
	\file NetMsg.h
	Network Library
	Base Network Message Header
	\author Taylor Clark
	\date July 8, 2006

	This file contains the definition for the animated sprite class.  This stores a reference
	animated sprite and the information needed to play correctly.
*/
//=================================================================================================

#pragma once
#ifndef __NetMsg_h
#define __NetMsg_h

#include "Base/Types.h"
#include "NetDataBlock.h"
#include "NetDataBlockOut.h"
#include "IPAddress.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsg
	\brief The base class for network messages
*/
//-------------------------------------------------------------------------------------------------
class NetMsg
{
public:

	/// The default destructor
	virtual ~NetMsg(){}

	/// The IP address of the sender of this message
	IPAddress m_SenderIP;

	/// The port of the sender of this message
	uint16 m_SenderPort;

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& dataOut ) const = 0;

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const = 0;
};

#endif // __NetMsg_h