//=================================================================================================
/*!
	\file GameNetMsgs.h
	Game Play Library
	Auto-Generated Game Network Message Header
	\author The NetMsgGen application, but that is by Taylor Clark
	\date Sat Sep 23 15:18:22 2006


	This file is auto-generated contains the definitions for the game network message objects.  It
	should not be editted directly, but rather the source file should be altered and this file
	regenerated.
*/
//=================================================================================================

#pragma once
#ifndef __GameNetMsgs_h
#define __GameNetMsgs_h

#include "Network/NetMsg.h"#include "Network/NetMsg.h"

/// The game network message IDs
enum EGameNetMsgIDs
{
	GNMI_Error = -1,
	GNMI_GameRunning,
	GNMI_GameClosed,
	GNMI_COUNT
};


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsgGameRunning
	\brief The message to alert clients that a game is running
*/
//-------------------------------------------------------------------------------------------------
class NetMsgNetMsgGameRunning : public NetMsg
{
public:

	/// The port to connect to the server to in order to join the game
	uint16 m_ServerListenPort;

	GameDefines::EGameType m_GameType;

	GameDefines::EGameplayDiffLevel m_HostDiff;

	std::wstring m_sHostName;

	/// The default constructor
	NetMsgGameRunning()
	{
	}

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( NetDataBlock dataBlock )
	{
		// Ensure the data block contains the right amount of data
		if( dataBlock.GetRemainingBytes() !=  + sizeof((null))sizeof((null))sizeof((null))sizeof((null)) )
			return 0;

		// Create the new message and fill in the data
		NetMsgGameRunning* pNewMsg = new NetMsgGameRunning();
		pNewMsg->m_ServerListenPort = (uint16)dataBlock.ReadUint16();
		pNewMsg->m_GameType = (GameDefines::EGameType)dataBlock.ReadUint32();
		pNewMsg->m_HostDiff = (GameDefines::EGameplayDiffLevel)dataBlock.ReadUint32();
		pNewMsg->m_sHostName = dataBlock.ReadString();

		// Return the message
		return pNewMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& dataOut ) const
	{
		uint32 startPos = dataOut.GetCurPosition();

		dataOut.WriteVal16( m_ServerListenPort );
		dataOut.WriteString( m_sHostName );

		return dataOut.GetCurPosition() - startPos;
	}

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return GNMI_GameRunning; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsgGameClosed
	\brief The message to alert clients that a game can no longer be joined
*/
//-------------------------------------------------------------------------------------------------
class NetMsgNetMsgGameClosed : public NetMsg
{
public:

	/// The port the server was listening on before closing
	uint16 m_ServerListenPort;

	/// The default constructor
	NetMsgGameClosed()
	{
	}

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( NetDataBlock dataBlock )
	{
		// Ensure the data block contains the right amount of data
		if( dataBlock.GetRemainingBytes() !=  + sizeof((null)) )
			return 0;

		// Create the new message and fill in the data
		NetMsgGameClosed* pNewMsg = new NetMsgGameClosed();
		pNewMsg->m_ServerListenPort = (uint16)dataBlock.ReadUint16();

		// Return the message
		return pNewMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& dataOut ) const
	{
		uint32 startPos = dataOut.GetCurPosition();

		dataOut.WriteVal16( m_ServerListenPort );

		return dataOut.GetCurPosition() - startPos;
	}

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return GNMI_GameClosed; }
};

#endif // __GameNetMsgs_h