//=================================================================================================
/*!
	\file GameMgr.cpp
	Network Test Application
	Game Manager Source
	\author Taylor Clark
	\date August 28, 2006

	This source file contains the game manager class definition.
*/
//=================================================================================================

#include "../GameMgr.h"
#include "Graphics2D/Graphics2D.h"
#include "Base/MsgLogger.h"
#include "Network/NetworkMgr.h"
#include "Network/NetConnLAN.h"

#define MSGLOG_NOTE(x) MsgLogger::Get().Output( MsgLogger::MI_Note, (x) )
static const float32 UPDATE_PLAYERS_TIME = 0.2f;
static const uint16 FIND_GAME_PORT = 12346;

bool GameMgr::Init()
{
	m_GameData.m_pGameMgr = this;

	// Create the connection
	m_GameData.m_pGameConn = NetworkMgr::Get().CreateLANBroadcastConn( FIND_GAME_PORT );

	NetworkMgr::Get().RegisterNetMsg( EGMI_LookingForGame, GameNetMsgLookingForGame::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( EGMI_GameRunning, GameNetMsgRunningGame::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( EGMI_UpdateMyself, GameNetMsgUpdateMyself::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( EGMI_UpdateGame, GameNetMsgUpdateGame::CreateMsg );

	// Add the game state
	m_StateMachine.AddManagedState( new GSLooking() );
	m_StateMachine.AddManagedState( new GSHosting() );
	m_StateMachine.AddManagedState( new GSInGame() );

	((FSMGameState*)m_StateMachine.GetState( GS_LookingForGame ))->m_pGameData = &m_GameData;
	((FSMGameState*)m_StateMachine.GetState( GS_HostingGame ))->m_pGameData = &m_GameData;
	((FSMGameState*)m_StateMachine.GetState( GS_InGame ))->m_pGameData = &m_GameData;

	m_StateMachine.SetActiveState( GS_LookingForGame );

	Player newPlayer;
	newPlayer.m_IP = NetworkMgr::Get().GetLocalIP();
	m_GameData.m_Players.push_back( newPlayer );

	return true;
}

void GameMgr::GSLooking::Update( float32 frameTime )
{
	// See if anyone has responded to our looking message
	NetMsg* pMsg = 0;
	GameNetMsgRunningGame* pLookMsg = 0;
	while( pMsg = m_pGameData->m_pGameConn->PopNextMessage() )
	{
		if( pMsg->GetMsgTypeID() == EGMI_GameRunning )
			pLookMsg = reinterpret_cast<GameNetMsgRunningGame*>( pMsg );
		else
			delete pMsg;
	}

	// If a running game message was received
	if( pLookMsg )
	{
		MSGLOG_NOTE( L"Found server, joining" );

		m_ServerSendToPort = pLookMsg->m_JoinPort;
		m_ClientReceiveOnPort = pLookMsg->m_ClientListenPort;

		if( m_pParentFSM )
			m_pParentFSM->SetActiveState( GS_InGame );

		delete pLookMsg;
	}

	m_LookingForGameTime += frameTime;
	m_CreateGameTime += frameTime;

	if( m_LookingForGameTime > 1.0f )
	{
		if( m_pGameData->m_pGameConn )
			m_pGameData->m_pGameConn->SendNetMsg( GameNetMsgLookingForGame() );
		m_LookingForGameTime = 0.0f;
	}

	if( m_CreateGameTime > 4.0 )
	{
		if( m_pParentFSM )
		{
			MSGLOG_NOTE( L"Becomming server" );
			m_pParentFSM->SetActiveState( GS_HostingGame );
		}
	}
}

/// A message handler for switching to this state
void GameMgr::GSLooking::OnEnterState( FSMState* pPrevState )
{
	// If there is a previous state
	if( pPrevState )
	{
	}

	MSGLOG_NOTE( L"Entering looking state..." );

	// Clear data
	m_LookingForGameTime = 0.0f;
	m_CreateGameTime = 0.0f;
}



void GameMgr::GSHosting::Update( float32 frameTime )
{
	m_LastUpdateTime += frameTime;

	m_pGameData->m_pGameMgr->UpdatePlayers( frameTime );

	// Go through the pending messages from the connection for players looking for games
	NetMsg* pMsg = 0;
	while( pMsg = m_pGameData->m_pGameConn->PopNextMessage() )
	{
		// If this message is from a client who would like to join a game
		if( pMsg->GetMsgTypeID() == EGMI_LookingForGame )
		{
			//GameNetMsgLookingForGame* pLookMsg = reinterpret_cast<GameNetMsgLookingForGame*>( pMsg );
			
			GameNetMsgRunningGame joinMeMsg;
			joinMeMsg.m_JoinPort = m_ListenPort;
			joinMeMsg.m_ClientListenPort = m_ClientSendToPort;
			m_pGameData->m_pGameConn->SendNetMsg( joinMeMsg );
		}

		delete pMsg;
	}

	// Loop through messges for player updates
	while( pMsg = m_pHostConn->PopNextMessage() )
	{
		// Else if this message is a client updating himself
		if( pMsg->GetMsgTypeID() == EGMI_UpdateMyself )
		{
			GameNetMsgUpdateMyself* pUpdateMsg = reinterpret_cast<GameNetMsgUpdateMyself*>( pMsg );
			
			// Get the IP
			bool foundPlayer = false;
			for( PlayerList::iterator iterPlayer = m_pGameData->m_Players.begin(); iterPlayer != m_pGameData->m_Players.end(); ++iterPlayer )
			{
				// If this is the player
				if( iterPlayer->m_IP == pUpdateMsg->m_SenderIP )
				{
					iterPlayer->m_Pos = pUpdateMsg->GetPos();
					iterPlayer->m_Dir = pUpdateMsg->GetDir();
					foundPlayer = true;
					m_pGameData->UpdateFromIP( iterPlayer->m_IP.GetVal(), iterPlayer->m_Pos, iterPlayer->m_Dir );
					break;
				}
			}

			// If the player wasn't found then add it
			if( !foundPlayer )
				m_pGameData->UpdateFromIP( pUpdateMsg->m_SenderIP.GetVal(), pUpdateMsg->GetPos(), pUpdateMsg->GetDir() );
		}

		// Free the message
		delete pMsg;
	}

	// Update the players if needed
	if( m_LastUpdateTime > UPDATE_PLAYERS_TIME )
	{
		for( PlayerList::iterator iterPlayer = m_pGameData->m_Players.begin(); iterPlayer != m_pGameData->m_Players.end(); ++iterPlayer )
			m_UpdateMsg.UpdatePlayer( iterPlayer->m_Pos, iterPlayer->m_Dir, iterPlayer->m_IP );
		m_pHostConn->SendNetMsg( m_UpdateMsg );
		m_LastUpdateTime = 0;
	}
}

void GameMgr::GSHosting::ClearConn()
{
	if( !m_pHostConn )
		return;

	NetworkMgr::Get().CloseConnection( m_pHostConn );
	delete m_pHostConn;
	m_pHostConn = 0;
}

/// A message handler for switching to this state
void GameMgr::GSHosting::OnEnterState( FSMState* )
{
	ClearConn();
	m_ListenPort = (uint16)(rand() % 5000) + 50000;
	m_ClientSendToPort = (uint16)(rand() % 5000) + 50000;
	
	// Create a connection for the game
	m_pHostConn = NetworkMgr::Get().CreateLANBroadcastConn( m_ClientSendToPort, m_ListenPort, 100 );
}

/// A message handler for switching to another state
void GameMgr::GSHosting::OnLeaveState( FSMState* )
{
	ClearConn();
}



Player GameMgr::GameData::GetMyPlayer()
{
	for( PlayerList::iterator iterPlayer = m_Players.begin(); iterPlayer != m_Players.end(); ++iterPlayer )
	{
		if( iterPlayer->m_IP == NetworkMgr::Get().GetLocalIP() )
			return *iterPlayer;
	}
	return Player();
}



void GameMgr::GSInGame::ClearConn()
{
	if( !m_pClientConn )
		return;

	NetworkMgr::Get().CloseConnection( m_pClientConn );
	delete m_pClientConn;
	m_pClientConn = 0;
}

void GameMgr::GSInGame::Update( float32 frameTime )
{
	m_pGameData->m_pGameMgr->UpdatePlayers( frameTime );

	m_TimeSinceServerUpdate += frameTime;

	// Check for messages
	NetMsg* pMsg = 0;
	while( pMsg = m_pClientConn->PopNextMessage() )
	{
		if( pMsg->GetMsgTypeID() == EGMI_UpdateGame )
		{
			m_TimeSinceServerUpdate = 0.0f;

			GameNetMsgUpdateGame* pUpdateMsg = reinterpret_cast<GameNetMsgUpdateGame*>( pMsg );

			for( unsigned int playerIndex = 0; playerIndex < pUpdateMsg->m_PlayerData.size(); ++playerIndex )
			{
				uint32 ipVal = pUpdateMsg->m_PlayerData[playerIndex].ip;
				m_pGameData->UpdateFromIP( ipVal, pUpdateMsg->GetPos( playerIndex ), pUpdateMsg->GetDir( playerIndex ) );
			}
		}
		delete pMsg;
	}

	// If it has been too long, then look for another game
	if( m_TimeSinceServerUpdate > 5.0f )
	{
		m_pGameData->m_pGameMgr->m_StateMachine.SetActiveState( GS_LookingForGame );
	}
	
	// If it is time to send an update
	m_SendUpdateTime += frameTime;
	if( m_SendUpdateTime > UPDATE_PLAYERS_TIME )
	{
		// Create the update message
		GameNetMsgUpdateMyself updateSelf;
		Player localPlayer = m_pGameData->GetMyPlayer();
		updateSelf.Init( localPlayer.m_Pos, localPlayer.m_Dir );

		m_pClientConn->SendNetMsg( updateSelf );
		
		m_SendUpdateTime = 0;
	}
}

/// A message handler for switching to this state
void GameMgr::GSInGame::OnEnterState( FSMState* pPrevState )
{
	m_SendUpdateTime = 0.0f;
	m_TimeSinceServerUpdate = 0.0f;

	// If the previous state was looking then get the port to listen on
	if( pPrevState && pPrevState->GetStateID() == GS_LookingForGame )
	{
		GSLooking* pLooking = reinterpret_cast<GSLooking*>( pPrevState );
		m_pClientConn = NetworkMgr::Get().CreateLANBroadcastConn( pLooking->m_ServerSendToPort, pLooking->m_ClientReceiveOnPort, 1 );
	}
}

/// A message handler for switching to another state
void GameMgr::GSInGame::OnLeaveState( FSMState* )
{
	ClearConn();
}



void GameMgr::Update( float32 frameTime, Point2i mousePos )
{
	m_GameData.m_MousePos = mousePos;
	m_StateMachine.Update( frameTime );
}

void GameMgr::Term()
{
	m_StateMachine.Clear();

	if( m_GameData.m_pGameConn )
	{
		NetworkMgr::Get().CloseConnection( m_GameData.m_pGameConn );
		delete m_GameData.m_pGameConn;
		m_GameData.m_pGameConn = 0;
	}
}

void GameMgr::UpdatePlayers( float32 frameTime )
{
	for( PlayerList::iterator iterPlayer = m_GameData.m_Players.begin(); iterPlayer != m_GameData.m_Players.end(); ++iterPlayer )
	{
		// If this player is ourself
		if( iterPlayer->m_IP == NetworkMgr::Get().GetLocalIP() )
		{
			Vector2 newDir( (float32)m_GameData.m_MousePos.x - iterPlayer->m_Pos.x, (float32)m_GameData.m_MousePos.y - iterPlayer->m_Pos.y );
			newDir.Normalize();
			newDir *= 100.0f;

			iterPlayer->m_Dir = newDir;
		}

		iterPlayer->m_Pos += iterPlayer->m_Dir * frameTime;
	}
}

void GameMgr::Draw() const
{
	GraphicsMgr::Get().FillRect( Box2i( 10, 10, 50, 50 ), 0x00FF00FF );

	const int32 PLAYER_SIZE = 10;
	const Vector2i PLAYER_DIMS( PLAYER_SIZE, PLAYER_SIZE );
	for( PlayerList::const_iterator iterPlayer = m_GameData.m_Players.begin(); iterPlayer != m_GameData.m_Players.end(); ++iterPlayer )
	{
		Point2i playerPos( (int32)iterPlayer->m_Pos.x, (int32)iterPlayer->m_Pos.y );
		playerPos.x -= PLAYER_SIZE / 2;
		playerPos.y -= PLAYER_SIZE / 2;
		
		GraphicsMgr::Get().FillRect( Box2i( playerPos, PLAYER_DIMS ), 0x00FF00FF );
	}
}