//=================================================================================================
/*!
	\file GameNetMsg.cpp
	Network Test Application
	Game Manager Header
	\author Taylor Clark
	\date August 28, 2006

	This header file contains the game manager class declaration.
*/
//=================================================================================================

#pragma once
#ifndef __GameNetMsg_H
#define __GameNetMsg_H

#include "Base/Types.h"
#include "Network/NetMsg.h"
#include "Math/Point2.h"
#include "Math/Vector2.h"
#include <vector>

enum EGameMsgIDs
{
	EGMI_Error = -1,
	EGMI_LookingForGame,
	EGMI_GameRunning,
	EGMI_UpdateMyself,
	EGMI_UpdateGame,
	EGMI_COUNT
};


class GameNetMsgLookingForGame : public NetMsg
{
public:

	/// Get the size of the message data
	virtual uint32 GetMsgDataSize() const { return 0; }

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( DataBlock dataBlock )
	{
		if( dataBlock.GetRemainingBytes() > 0 )
			return 0;
		return new GameNetMsgLookingForGame();
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( DataBlockOut& ) const { return 0; };

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return EGMI_LookingForGame; }
};


class GameNetMsgRunningGame : public NetMsg
{
public:

	uint16 m_JoinPort;

	uint16 m_ClientListenPort;

	/// Get the size of the message data
	virtual uint32 GetMsgDataSize() const { return sizeof(uint16); }

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( DataBlock dataBlock )
	{
		if( dataBlock.GetRemainingBytes() != sizeof(uint16) * 2 )
			return 0;
		GameNetMsgRunningGame* pRetMsg = new GameNetMsgRunningGame();
		pRetMsg->m_JoinPort = dataBlock.ReadUint16();
		pRetMsg->m_ClientListenPort = dataBlock.ReadUint16();
		return pRetMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( DataBlockOut& dataOut ) const
	{
		dataOut.WriteVal( m_JoinPort );
		dataOut.WriteVal( m_ClientListenPort );
		return GetMsgDataSize();
	}

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return EGMI_GameRunning; }
};


class GameNetMsgUpdateMyself : public NetMsg
{
private:

	static int16 FloatToInt16( float32 fVal )
	{
		const float32 SHIFT = (float32)( 1 << 5 );
		
		int signBit = 0;
		if( fVal < 0 )
		{
			signBit = 1;
			fVal = 0.0f - fVal;
		}

		int16 val = (int16)( fVal * SHIFT );
		if( signBit )
			val |= 1 << 15;
		else
			val &= ~(1<<15);
		
		// Return the value
		return val;
	}

	static float32 Int16ToFloat( int16 val )
	{
		const float32 SHIFT = (float32)( 1 << 5 );

		bool isNeg = (val & (1 << 15)) != 0;
		val &= ~(1 << 15);

		float32 retVal = (float32)val;
		retVal /= SHIFT;

		if( isNeg )
			retVal = 0.0f - retVal;
		
		// Return the value
		return retVal;
	}

public:

	/// The position
	int16 m_X, m_Y;

	/// The direction in fixed point format in 1 sign and 10.5
	int16 m_dX, m_dY;

	static const uint32 MSG_DATA_SIZE = sizeof(int16) * 4;

	/// Get the size of the message data
	virtual uint32 GetMsgDataSize() const { return MSG_DATA_SIZE; }

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( DataBlock dataBlock )
	{
		if( dataBlock.GetRemainingBytes() != MSG_DATA_SIZE )
			return 0;

		// Initialize the message
		GameNetMsgUpdateMyself* pRetMsg = new GameNetMsgUpdateMyself();
		pRetMsg->m_X = dataBlock.ReadInt16();
		pRetMsg->m_Y = dataBlock.ReadInt16();
		pRetMsg->m_dX = dataBlock.ReadInt16();
		pRetMsg->m_dY = dataBlock.ReadInt16();

		// Return the message
		return (NetMsg*)pRetMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( DataBlockOut& dataOut ) const
	{
		// Ensure there is enough room
		if( GetMsgDataSize() > dataOut.GetRemainingBytes() )
			return 0;

		// Store the data
		dataOut.WriteVal( m_X );
		dataOut.WriteVal( m_Y );
		dataOut.WriteVal( m_dX );
		dataOut.WriteVal( m_dY );
		return sizeof(int16) * 4;
	};

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return EGMI_UpdateMyself; }

	void Init( Point2 pos, Vector2 dir )
	{
		// Convert the floating point to fixed point
		float32 vals[] = {pos.x, pos.y, dir.x, dir.y};
		const int NUM_VALS = sizeof(vals) / sizeof(vals[0]);
		for( int32 valIndex = 0; valIndex < NUM_VALS; ++valIndex )
		{
			// Store the value
			if( valIndex == 0 )
				m_X = FloatToInt16( vals[valIndex] );
			else if( valIndex == 1 )
				m_Y = FloatToInt16( vals[valIndex] );
			else if( valIndex == 2 )
				m_dX = FloatToInt16( vals[valIndex] );
			else if( valIndex == 3 )
				m_dY = FloatToInt16( vals[valIndex] );
		}
	}

	Point2 GetPos() const
	{
		return Point2( Int16ToFloat( m_X ), Int16ToFloat( m_Y ) );
	}

	Vector2 GetDir() const
	{
		return Vector2( Int16ToFloat( m_dX ), Int16ToFloat( m_dY ) );
	}
};


class GameNetMsgUpdateGame : public NetMsg
{
	static int16 FloatToInt16( float32 fVal )
	{
		const float32 SHIFT = (float32)( 1 << 5 );
		
		int signBit = 0;
		if( fVal < 0 )
		{
			signBit = 1;
			fVal = 0.0f - fVal;
		}

		int16 val = (int16)( fVal * SHIFT );
		if( signBit )
			val |= 1 << 15;
		else
			val &= ~(1<<15);
		
		// Return the value
		return val;
	}

	static float32 Int16ToFloat( int16 val )
	{
		const float32 SHIFT = (float32)( 1 << 5 );

		bool isNeg = (val & (1 << 15)) != 0;
		val &= ~(1 << 15);

		float32 retVal = (float32)val;
		retVal /= SHIFT;

		if( isNeg )
			retVal = 0.0f - retVal;
		
		// Return the value
		return retVal;
	}

public:

	/// The number of players
	uint8 m_NumPlayers;

	struct Player
	{
		int16 x,y;
		int16 dx,dy;
		uint32 ip;
	};

	/// The players
	std::vector< Player > m_PlayerData;

	/// Get the size of the message data
	virtual uint32 GetMsgDataSize() const { return 0; }

	/// Create this message upon receiving data
	static NetMsg* CreateMsg( DataBlock dataBlock )
	{
		if( dataBlock.GetRemainingBytes() < 1 )
			return 0;

		uint8 numPlayers = dataBlock.ReadUint8();
		
		if( dataBlock.GetRemainingBytes() < numPlayers * sizeof(Player) )
			return 0;

		GameNetMsgUpdateGame* pNewMsg = new GameNetMsgUpdateGame();
		pNewMsg->m_NumPlayers = numPlayers;

		pNewMsg->m_PlayerData.resize( numPlayers );
		for( uint8 playerIndex = 0; playerIndex < numPlayers; ++playerIndex )
		{
			pNewMsg->m_PlayerData[playerIndex].x = dataBlock.ReadInt16();
			pNewMsg->m_PlayerData[playerIndex].y = dataBlock.ReadInt16();
			pNewMsg->m_PlayerData[playerIndex].dx = dataBlock.ReadInt16();
			pNewMsg->m_PlayerData[playerIndex].dy = dataBlock.ReadInt16();
			pNewMsg->m_PlayerData[playerIndex].ip = dataBlock.ReadUint32();
		}
		
		return pNewMsg;
	}

	/// Fill in a buffer with the message data
	virtual uint32 StoreInBuffer( DataBlockOut& dataBlock) const
	{
		// Write the number of players
		uint32 bytesWritten = 1;
		dataBlock.WriteVal( (uint8)m_PlayerData.size() );

		// Write the player data
		for( unsigned int playerIndex = 0; playerIndex < m_PlayerData.size(); ++playerIndex )
		{
			bytesWritten += sizeof( int16 ) * 4 + sizeof(uint32);
			dataBlock.WriteVal( m_PlayerData[playerIndex].x );
			dataBlock.WriteVal( m_PlayerData[playerIndex].y );
			dataBlock.WriteVal( m_PlayerData[playerIndex].dx );
			dataBlock.WriteVal( m_PlayerData[playerIndex].dy );
			dataBlock.WriteVal( m_PlayerData[playerIndex].ip );
		}

		return bytesWritten;
	}

	/// Get the message type ID
	virtual uint16 GetMsgTypeID() const { return EGMI_UpdateGame; }

	void UpdatePlayer( Point2 pos, Vector2 dir, IPAddress ip )
	{
		for( unsigned int playerIndex = 0; playerIndex < m_PlayerData.size(); ++playerIndex )
		{
			if( m_PlayerData[playerIndex].ip != ip.GetVal() )
				continue;

			Player& player = m_PlayerData[playerIndex];
			player.x = FloatToInt16( pos.x );
			player.y = FloatToInt16( pos.y );
			player.dx = FloatToInt16( dir.x );
			player.dy = FloatToInt16( dir.y );
			return;
		}

		Player newPlayer;
		newPlayer.x = FloatToInt16( pos.x );
		newPlayer.y = FloatToInt16( pos.y );
		newPlayer.dx = FloatToInt16( dir.x );
		newPlayer.dy = FloatToInt16( dir.y );
		newPlayer.ip = ip.GetVal();

		m_PlayerData.push_back( newPlayer );
	}

	Point2 GetPos( unsigned int index )
	{
		Point2 retPos;
		if( index < 0 || index >= m_PlayerData.size() )
			return retPos;

		retPos.x = Int16ToFloat( m_PlayerData[index].x );
		retPos.y = Int16ToFloat( m_PlayerData[index].y );
		return retPos;
	}

	Vector2 GetDir( unsigned int index )
	{
		Vector2 retDir;
		if( index < 0 || index >= m_PlayerData.size() )
			return retDir;

		retDir.x = Int16ToFloat( m_PlayerData[index].dx );
		retDir.y = Int16ToFloat( m_PlayerData[index].dy );
		return retDir;
	}
};


#endif // __GameNetMsg_H