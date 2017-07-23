//=================================================================================================
/*!
	\file GameMgr.cpp
	Network Test Application
	Game Manager Header
	\author Taylor Clark
	\date August 28, 2006

	This header file contains the game manager class declaration.
*/
//=================================================================================================

#pragma once
#ifndef __GameMgr_H
#define __GameMgr_H

#include "Base/Types.h"
#include "Math/Point2i.h"
#include "Math/Point2.h"
#include "Math/Vector2.h"
#include "Network/IPAddress.h"
#include <list>
#include "Base/FSM.h"
#include "GameNetMsg.h"

class NetworkConnection;
class NetConnTCP;
class NetConnLAN;


struct Player
{
	Point2 m_Pos;

	Vector2 m_Dir;

	// The IP address of the player
	IPAddress m_IP;
};


class GameMgr
{
private:

	typedef std::list< Player > PlayerList;

	enum EGameState
	{
		GS_LookingForGame,
		GS_HostingGame,
		GS_InGame,
		GS_COUNT
	};

	StateMachine m_StateMachine;

	struct GameData
	{
		GameMgr* m_pGameMgr;

		/// The list of players
		PlayerList m_Players;

		/// The connection when looking for/responding to games
		NetConnLAN* m_pGameConn;

		Point2i m_MousePos;


		void UpdateFromIP( uint32 ip, Point2 pos, Vector2 dir )
		{
			for( PlayerList::iterator iterPlayer = m_Players.begin(); iterPlayer != m_Players.end(); ++iterPlayer )
			{
				if( iterPlayer->m_IP.GetVal() != ip )
					continue;

				iterPlayer->m_Pos = pos;
				iterPlayer->m_Dir = dir;
				return;
			}

			// Add the player
			Player newPlayer;
			newPlayer.m_Pos = pos;
			newPlayer.m_Dir = dir;
			newPlayer.m_IP = IPAddress( ip );
			m_Players.push_back( newPlayer );
		}

		GameData() : m_pGameConn( 0 ),
						m_pGameMgr( 0 )
		{}

		Player GetMyPlayer();
	};

	struct FSMGameState : public FSMState
	{
		GameData* m_pGameData;

		FSMGameState() : m_pGameData( 0 )
		{}

		virtual ~FSMGameState() {}
	};



	struct GSLooking : public FSMGameState
	{
		float m_LookingForGameTime;
		float m_CreateGameTime;

		uint16 m_ServerSendToPort;
		uint16 m_ClientReceiveOnPort;

		GSLooking() : m_LookingForGameTime( 0.0f ),
					m_CreateGameTime( 0.0f ),
					m_ServerSendToPort( 0 ),
					m_ClientReceiveOnPort( 0 )
		{}

		/// A message handler for switching to this state
		virtual void OnEnterState( FSMState* pPrevState );

		/// Update the state
		virtual void Update( float32 frameTime );

		/// Get the state ID
		virtual int32 GetStateID() const{ return (int32)GS_LookingForGame; }
	};



	struct GSHosting : public FSMGameState
	{
		float32 m_LastUpdateTime;

		NetConnLAN* m_pHostConn;

		uint16 m_ListenPort;

		uint16 m_ClientSendToPort;

		GameNetMsgUpdateGame m_UpdateMsg;

		/// The default constructor
		GSHosting() : m_LastUpdateTime( 0.0f ),
						m_pHostConn( 0 ),
						m_ListenPort( 38293 ),
						m_ClientSendToPort( 45123 )
		{}

		void ClearConn();

		/// Update the state
		virtual void Update( float32 frameTime );

		/// Get the state ID
		virtual int32 GetStateID() const{ return (int32)GS_HostingGame; }

		/// A message handler for switching to this state
		virtual void OnEnterState( FSMState* pPrevState );

		/// A message handler for switching to another state
		virtual void OnLeaveState( FSMState* pNewState );
	};



	struct GSInGame : public FSMGameState
	{
		NetConnLAN* m_pClientConn;

		float32 m_SendUpdateTime;

		float32 m_TimeSinceServerUpdate;

		GSInGame() : m_pClientConn( 0 ),
						m_SendUpdateTime( 0.0f ),
						m_TimeSinceServerUpdate( 0.0f )
		{}

		virtual ~GSInGame(){ ClearConn(); }

		void ClearConn();

		/// Update the state
		virtual void Update( float32 frameTime );

		/// Get the state ID
		virtual int32 GetStateID() const{ return (int32)GS_InGame; }

		/// A message handler for switching to this state
		virtual void OnEnterState( FSMState* pPrevState );

		/// A message handler for switching to another state
		virtual void OnLeaveState( FSMState* pNewState );
	};

	uint16 m_ServerPort;

	GameData m_GameData;

	void UpdatePlayers( float32 frameTime );

public:

	GameMgr() : m_ServerPort( 0 )
	{
	}

	~GameMgr() { Term(); }

	bool Init();

	void Term();

	void Update( float32 frameTime, Point2i mousePos );

	void Draw() const;
};

#endif // __GameMgr_H