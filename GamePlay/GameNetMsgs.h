//=================================================================================================
/*!
	\file GameNetMsgs.h
	Game Play Library
	Auto-Generated Game Network Message Header
	\author The NetMsgGen application, but that is by Taylor Clark
	\date Thu Oct 05 18:01:43 2006


	This file is auto-generated contains the definitions for the game network message objects.  It
	should not be editted directly, but rather the source file should be altered and this file
	regenerated.
*/
//=================================================================================================

#pragma once
#ifndef __GameNetMsgs_h
#define __GameNetMsgs_h

#include "Network/NetMsg.h"
#include "GameDefines.h"

/// The game network message IDs
enum EGameNetMsgIDs
{
	GNMI_Error = -1,
	GNMI_GameRunning,
	GNMI_GameClosed,
	GNMI_SendMeYourInfo,
	GNMI_PlayerInfo,
	GNMI_GameInfo,
	GNMI_StartGameQuery,
	GNMI_StartGameResponse,
	GNMI_COUNT
};


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsgGameRunning
	\brief The message to alert clients that a game is running
*/
//-------------------------------------------------------------------------------------------------
class NetMsgGameRunning : public NetMsg
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
		// Ensure the data block contains the right amount of data (Calculated during message
		// generation)
		if( dataBlock.GetRemainingBytes() < 6 )
			return 0;

		// Create the new message and fill in the data
		NetMsgGameRunning* pNewMsg = new NetMsgGameRunning();
		pNewMsg->m_ServerListenPort = (uint16)dataBlock.ReadUint16();
		pNewMsg->m_GameType = (GameDefines::EGameType)dataBlock.ReadUint16();
		pNewMsg->m_HostDiff = (GameDefines::EGameplayDiffLevel)dataBlock.ReadUint8();
		pNewMsg->m_sHostName = dataBlock.ReadString();

		// Return the message
		return pNewMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& dataOut ) const
	{
		uint32 startPos = dataOut.GetCurPosition();

		dataOut.WriteVal16( (uint16)m_ServerListenPort );
		dataOut.WriteVal8( (uint8)m_GameType );
		dataOut.WriteVal8( (uint8)m_HostDiff );
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
class NetMsgGameClosed : public NetMsg
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
		// Ensure the data block contains the right amount of data (Calculated during message
		// generation)
		if( dataBlock.GetRemainingBytes() < 2 )
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

		dataOut.WriteVal16( (uint16)m_ServerListenPort );

		return dataOut.GetCurPosition() - startPos;
	}

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return GNMI_GameClosed; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsgSendMeYourInfo
	\brief The message for a player to send to another player to get their information
*/
//-------------------------------------------------------------------------------------------------
class NetMsgSendMeYourInfo : public NetMsg
{
public:

	/// The default constructor
	NetMsgSendMeYourInfo()
	{
	}

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( NetDataBlock dataBlock )
	{
		if( dataBlock.GetRemainingBytes() > 0 )
			return 0;
		return new NetMsgSendMeYourInfo();
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& ) const { return 0; }

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return GNMI_SendMeYourInfo; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsgPlayerInfo
	\brief A message containing a player's data
*/
//-------------------------------------------------------------------------------------------------
class NetMsgPlayerInfo : public NetMsg
{
public:

	/// The player's name
	std::wstring m_sPlayerName;

	GameDefines::EGameplayDiffLevel m_CurDiff;

	bool m_IsReady;

	/// The default constructor
	NetMsgPlayerInfo()
	{
	}

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( NetDataBlock dataBlock )
	{
		// Ensure the data block contains the right amount of data (Calculated during message
		// generation)
		if( dataBlock.GetRemainingBytes() < 4 )
			return 0;

		// Create the new message and fill in the data
		NetMsgPlayerInfo* pNewMsg = new NetMsgPlayerInfo();
		pNewMsg->m_sPlayerName = dataBlock.ReadString();
		pNewMsg->m_CurDiff = (GameDefines::EGameplayDiffLevel)dataBlock.ReadUint8();
		pNewMsg->m_IsReady = dataBlock.ReadUint8() != 0;

		// Return the message
		return pNewMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& dataOut ) const
	{
		uint32 startPos = dataOut.GetCurPosition();

		dataOut.WriteString( m_sPlayerName );
		dataOut.WriteVal8( (uint8)m_CurDiff );
		dataOut.WriteVal8( (uint8)m_IsReady );

		return dataOut.GetCurPosition() - startPos;
	}

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return GNMI_PlayerInfo; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsgGameInfo
	\brief The pre-net-game message storing the settings
*/
//-------------------------------------------------------------------------------------------------
class NetMsgGameInfo : public NetMsg
{
public:

	/// The type of game to play
	GameDefines::EGameType m_GameType;

	/// The time limit for the game
	uint16 m_TimeLimit;

	/// The default constructor
	NetMsgGameInfo()
	{
	}

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( NetDataBlock dataBlock )
	{
		// Ensure the data block contains the right amount of data (Calculated during message
		// generation)
		if( dataBlock.GetRemainingBytes() < 3 )
			return 0;

		// Create the new message and fill in the data
		NetMsgGameInfo* pNewMsg = new NetMsgGameInfo();
		pNewMsg->m_GameType = (GameDefines::EGameType)dataBlock.ReadUint16();
		pNewMsg->m_TimeLimit = (uint16)dataBlock.ReadUint16();

		// Return the message
		return pNewMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& dataOut ) const
	{
		uint32 startPos = dataOut.GetCurPosition();

		dataOut.WriteVal8( (uint8)m_GameType );
		dataOut.WriteVal16( (uint16)m_TimeLimit );

		return dataOut.GetCurPosition() - startPos;
	}

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return GNMI_GameInfo; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsgStartGameQuery
	\brief The message sent by the server telling the client that a game is to be started
*/
//-------------------------------------------------------------------------------------------------
class NetMsgStartGameQuery : public NetMsg
{
public:

	/// The default constructor
	NetMsgStartGameQuery()
	{
	}

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( NetDataBlock dataBlock )
	{
		if( dataBlock.GetRemainingBytes() > 0 )
			return 0;
		return new NetMsgStartGameQuery();
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& ) const { return 0; }

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return GNMI_StartGameQuery; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class NetMsgStartGameResponse
	\brief The message sent by the client in response to the server's StartGameQuery message
*/
//-------------------------------------------------------------------------------------------------
class NetMsgStartGameResponse : public NetMsg
{
public:

	/// If the client is allowing the start
	bool m_AllowStart;

	/// The default constructor
	NetMsgStartGameResponse()
	{
	}

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( NetDataBlock dataBlock )
	{
		// Ensure the data block contains the right amount of data (Calculated during message
		// generation)
		if( dataBlock.GetRemainingBytes() < 1 )
			return 0;

		// Create the new message and fill in the data
		NetMsgStartGameResponse* pNewMsg = new NetMsgStartGameResponse();
		pNewMsg->m_AllowStart = dataBlock.ReadUint8() != 0;

		// Return the message
		return pNewMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( NetDataBlockOut& dataOut ) const
	{
		uint32 startPos = dataOut.GetCurPosition();

		dataOut.WriteVal8( (uint8)m_AllowStart );

		return dataOut.GetCurPosition() - startPos;
	}

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return GNMI_StartGameResponse; }
};

#endif // __GameNetMsgs_h