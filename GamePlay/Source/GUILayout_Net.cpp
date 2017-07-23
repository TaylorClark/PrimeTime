//=================================================================================================
/*!
	\file GUILayout_Net.cpp
	Game Play Library
	Network Related Game GUI Layouts Source
	\author Taylor Clark
	\date July 10, 2006

	This source file contains the implementation for the network related game GUI layout classes.
*/
//=================================================================================================

#include "../GUILayout_Game.h"
#include "GUI/GUICtrlLabel.h"
#include "GUI/GUICtrlScrollList.h"
#include "GUI/GUICtrlButton.h"
#include "GUI/GUICtrlList.h"
#include "GUI/GUIMgr.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Network/NetworkMgr.h"
#include "Network/NetConnLAN.h"
#include "Network/NetConnTCP.h"
#include "../GameMgr.h"
#include "GUI/MsgBox.h"
#include "../ProfileSubsetSettings.h"

// The amount of time, in seconds, between the LAN lobby sending out a message to find open games
const float32 SEND_FINDGAME_TIME = 0.75f;

// The time interval of the server updating clients about an active game
const float32 SERVER_SEND_ACTIVE_GAME_TIME = 0.75f;

// The port for listening for or sending on messages about active LAN games
const uint16 LAN_ACTIVEGAME_PORT = 32053;

// The amount of time, in seconds, before a game is considered inactive
const float32 GAME_INACTIVE = 10.0f;

// The default port to listen on for TCP connections in the pre-net game layout
const uint16 DEF_TCP_LISTEN_PORT = 55231;

// The callback for layouts that go back to the title screen
extern void LayoutCB_BackToTitle( GUICtrlButton* );


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  LayoutCB_BackToMPType  Static Global
///
///	\param UNNAMED The button that sent this message
///
///	A message handler called when the user presses the back button to go back to the multiplayer
///	type selection.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static void LayoutCB_BackToMPType( GUICtrlButton* )
{
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_MultiplayerType );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutMultiplayType::TypeSelCallback  Static Private
///
///	\param selIndex The index of the selected list item
///	\param UNNAMED The list box that sent this message
///
///	A message handler called when the user selects an item on the multiplayer type screen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutMultiplayType::TypeSelCallback( uint32 selIndex, GUICtrlList* )
{
	// If the user chose to play PrimeTime mode
	if( selIndex == LISTITEMINDEX_LAN )
	{
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_LANLobby );
	}
	// Else if the user chose to play PrimeTime mode
	else if( selIndex == LISTITEMINDEX_IP )
	{
		// Go to the create game screen and set that it is a LAN game
		GUILayoutPreNetGame* pPreNetGameLayout = (GUILayoutPreNetGame*)GUIMgr::Get().GetLayoutByID( GameDefines::MS_PreNetGame );
		if( !pPreNetGameLayout )
			return;
		pPreNetGameLayout->m_IsLANGame = false;
		pPreNetGameLayout->m_IsHost = true;	
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_PreNetGame );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutMultiplayType::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.  It goes through
///	the controls in the layout and finds the desired controls by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutMultiplayType::HookupCtrls()
{
	// Allow the base class to hook up the selected controls
	GameGUILayout::HookupCtrls();

	// Link up the back button
	GUICtrlButton* pBackButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_BackButton );
	if( pBackButton )
		pBackButton->SetClickCallback( LayoutCB_BackToTitle );

	// Link up the main list
	if( m_pTypeList )
		m_pTypeList->SetCallback( TypeSelCallback );

	// Resize the label box
	if( m_pHelpLabel && pBackButton )
	{
		// Set the position
		m_HelpLabelBox.pos.x = g_pGraphicsMgr->GetDisplayDims().x / 10;
		m_HelpLabelBox.pos.y = m_pHelpLabel->GetBoundBox().pos.y;
		m_pHelpLabel->SetPos( m_HelpLabelBox.pos );

		// Set the dimensions
		m_HelpLabelBox.size.x = (g_pGraphicsMgr->GetDisplayDims().x / 5) * 4;
		m_HelpLabelBox.size.y = pBackButton->GetPos().y - m_HelpLabelBox.pos.y;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutMultiplayType::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutMultiplayType::OnActivate()
{
	GameGUILayout::OnActivate();

	// Clear the help label
	if( m_pHelpLabel )
		m_pHelpLabel->SetText( L"" );

	// Register the network messages
	NetworkMgr::Get().RegisterNetMsg( GNMI_GameRunning, NetMsgGameRunning::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( GNMI_GameClosed, NetMsgGameClosed::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( GNMI_SendMeYourInfo, NetMsgSendMeYourInfo::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( GNMI_PlayerInfo, NetMsgPlayerInfo::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( GNMI_GameInfo, NetMsgGameInfo::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( GNMI_StartGameQuery, NetMsgStartGameQuery::CreateMsg );
	NetworkMgr::Get().RegisterNetMsg( GNMI_StartGameResponse, NetMsgStartGameResponse::CreateMsg );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutMultiplayType::Update  Public
///
///	\param frametime The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutMultiplayType::Update( float32 frameTime )
{
	GameGUILayout::Update( frameTime );

	// If the pointers to the mode list or help label are null then bail
	if( !m_pTypeList || !m_pHelpLabel )
		return;

	// Get the current list item
	int32 curListItem = m_pTypeList->GetMouseOverItem();

	// If the selection has not changed then bail
	if( curListItem == m_CurListItem )
		return;

	// Store the new item
	m_CurListItem = curListItem;

	// If the mouse is over nothing then clear the help lable
	if( m_CurListItem == -1 )
		m_pHelpLabel->SetText( L"" );
	else
	{
		std::wstring sText;
		//TODO localize
		if( m_CurListItem == 0 )
			sText = L"Challenge other players on your local area network (LAN).  This is the option to use if you want to play someone within the same building as you.";
		else if( m_CurListItem == 1 )
			sText = L"Play against someone across the internet by directly connecting to their computer.  Use this option if you want play against someone outside of your building.";

		// Wrap the text and display it
		if( m_pHelpLabel->GetFont() )
			sText = m_pHelpLabel->GetFont()->GetWordWrapString( sText.c_str(), m_HelpLabelBox );
		m_pHelpLabel->SetText( sText.c_str() );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::CreateGameCB  Static Private
///
///	\param UNNAMED The button that invoked this callback
///
/// The callback to create a game in LAN lobby.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLANLobby::CreateGameCB( GUICtrlButton* )
{
	// Go to the create game screen and set that it is a LAN game
	GUILayoutPreNetGame* pPreNetGameLayout = (GUILayoutPreNetGame*)GUIMgr::Get().GetLayoutByID( GameDefines::MS_PreNetGame );
	if( !pPreNetGameLayout )
		return;
	pPreNetGameLayout->m_IsLANGame = true;
	pPreNetGameLayout->m_IsHost = true;
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_PreNetGame );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::JoinGameCB  Static Private
///
///	\param pButton The button that invoked this callback
///
/// The callback to join a game in LAN lobby.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLANLobby::JoinGameCB( GUICtrlButton* pButton )
{
	if( !pButton || !pButton->GetContainingLayout() )
		return;

	// Go to the create game screen and set that it is a LAN game
	GUILayoutLANLobby* pLayout = (GUILayoutLANLobby*)pButton->GetContainingLayout();
	if( !pLayout->m_pGamesList )
		return;

	// Get the selected item
	int32 selIndex = pLayout->m_pGamesList->GetSelItemIndex();
	if( selIndex == -1 )
	{
		//TODO localize
		pLayout->m_pMsgBox = MsgBox::CreateMsgBox( L"You must select a game from the list in order to join." );
		return;
	}

	// Get the game information
	ActiveGameList::iterator iterGame;
	for( iterGame = pLayout->m_ActiveGames.begin(); iterGame != pLayout->m_ActiveGames.end(); ++iterGame )
	{
		if( iterGame->m_ListItemIndex == (uint32)selIndex )
			break;
	}

	// If the game was not in the list
	if( iterGame == pLayout->m_ActiveGames.end() )
	{
		pLayout->m_pGamesList->RemoveString( (uint32)selIndex );
		//TODO localize
		pLayout->m_pMsgBox = MsgBox::CreateMsgBox( L"The selected game is no longer available." );
		return;
	}

	// Attempt to connect to the server
	NetConnTCP* pGameConn = NetworkMgr::Get().ConnectToTCPConn( iterGame->m_ServerIP, iterGame->m_ServerPort );
	if( !pGameConn || pGameConn->GetConnState() != NetConnTCP::CS_Connected )
	{
		//TODO localize
		pLayout->m_pMsgBox = MsgBox::CreateMsgBox( L"Cannot join selected server." );
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"No PreNetGame layout found." );
		return;
	}

	// Join the game
	GUILayoutPreNetGame* pPreNetLayout = (GUILayoutPreNetGame*)GUIMgr::Get().GetLayoutByID( GameDefines::MS_PreNetGame );
	if( !pPreNetLayout )
	{
		//TODO localize
		pLayout->m_pMsgBox = MsgBox::CreateMsgBox( L"Cannot join games due to an invalid GUI.rdb file." );
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"No PreNetGame layout found." );
		return;
	}

	// Create the TCP connection
	pPreNetLayout->SetHostName( iterGame->sCreatorName.c_str() );
	pPreNetLayout->m_pGameConn = pGameConn;
	pPreNetLayout->m_IsLANGame = true;
	pPreNetLayout->m_IsHost = false;

	// Go to the pre-net-game layout
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_PreNetGame );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::FindActiveGame  Public
///
///	\param ip The IP address of the server
///	\param port The port to which the client is to connect to join the game
///	\returns An iterator of the active game or one past the last element if the game is unknown
///
///	Find an active game by IP address and port.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUILayoutLANLobby::ActiveGameList::iterator GUILayoutLANLobby::FindActiveGame( IPAddress ip, uint16 port )
{
	for( ActiveGameList::iterator retIter = m_ActiveGames.begin(); retIter != m_ActiveGames.end(); ++retIter )
	{
		// If this is the server then return the iterator
		if( retIter->m_ServerIP == ip && retIter->m_ServerPort == port )
			return retIter;
	}

	// Return the end since the server is known
	return m_ActiveGames.end();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.  It goes through
///	the controls in the layout and finds the desired controls by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLANLobby::HookupCtrls()
{
	// Allow the base class to hook up the selected controls
	GameGUILayout::HookupCtrls();

	// Link up the back button
	GUIControl* pPlayCtrl = GetCtrlByName( CTRLNAME_BackButton );
	if( pPlayCtrl )
	{
		GUICtrlButton* pBackButton = (GUICtrlButton*)pPlayCtrl;
		pBackButton->SetClickCallback( LayoutCB_BackToMPType );
	}

	// Link up the create game button
	pPlayCtrl = GetCtrlByName( CTRLNAME_CreateGame );
	if( pPlayCtrl )
	{
		GUICtrlButton* pCreateButton = (GUICtrlButton*)pPlayCtrl;
		pCreateButton->SetClickCallback( CreateGameCB );
	}

	// Link up the join game button
	pPlayCtrl = GetCtrlByName( CTRLNAME_JoinButton );
	if( pPlayCtrl )
	{
		GUICtrlButton* pJoinButton = (GUICtrlButton*)pPlayCtrl;
		pJoinButton->SetClickCallback( JoinGameCB );
	}

	// Link up the scroll buttons
	GUICtrlButton* pScrollGamesUpBtn = (GUICtrlButton*)GetCtrlByName( CTRLNAME_GamesList_ScrollUp );
	GUICtrlButton* pScrollGamesDownBtn = (GUICtrlButton*)GetCtrlByName( CTRLNAME_GamesList_ScrollDown );
	if( m_pGamesList )
		m_pGamesList->SetScrollButtons( pScrollGamesUpBtn, pScrollGamesDownBtn );
}


/// Close the LAN connection
void GUILayoutLANLobby::CloseConnection()
{
	if( m_pGameListener )
	{
		m_pGameListener->Close();
		NetworkMgr::Get().CloseConnection( m_pGameListener );
		delete m_pGameListener;
		m_pGameListener = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLANLobby::OnActivate()
{
	GameGUILayout::OnActivate();
	
	// Close the connection before creating another
	CloseConnection();
	ClearActiveGames();

	// Create the LAN listen connection
	m_pGameListener = NetworkMgr::Get().CreateLANListenConn( LAN_ACTIVEGAME_PORT );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::OnDeactivate  Public
///
///	A message handler called when the layout is deactivated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLANLobby::OnDeactivate()
{
	CloseConnection();
	ClearActiveGames();

	// If there is a message box then close it
	if( m_pMsgBox )
	{
		GUIMgr::Get().CloseMessageBox( m_pMsgBox );
		delete m_pMsgBox;
		m_pMsgBox = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::ClearActiveGames  Private
///
///	Clear the active games.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLANLobby::ClearActiveGames()
{
	m_ActiveGames.clear();
	if( m_pGamesList )
		m_pGamesList->Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::RemoveActiveGame  Public
///
///	\param ip The IP address of the server
///	\param port The port the server was listening on
///
///	Remove an active game.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLANLobby::RemoveActiveGame( IPAddress ip, uint16 port )
{
	ActiveGameList::iterator iterGame = FindActiveGame( ip, port );
	if( iterGame == m_ActiveGames.end() )
		return;

	// Remove the item from the scroll list
	uint32 itemIndex = iterGame->m_ListItemIndex;
	if( m_pGamesList )
	{
		m_pGamesList->RemoveString( itemIndex );

		// Update the remaining list item indices
		for( ActiveGameList::iterator iterCurGame = m_ActiveGames.begin(); iterCurGame != m_ActiveGames.end(); ++iterCurGame )
		{
			if( iterCurGame->m_ListItemIndex > itemIndex )
				--iterCurGame->m_ListItemIndex;
		}
	}

	// Remove the list item
	m_ActiveGames.erase( iterGame );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLANLobby::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLANLobby::Update( float32 frameTime )
{
	// If there is a message box
	if( m_pMsgBox )
	{
		// If the message box is not ready to be closed
		if( m_pMsgBox->GetRetVal() == MsgBox::MBR_Error )
			return;

		// Close the message box
		GUIMgr::Get().CloseMessageBox( m_pMsgBox );
		delete m_pMsgBox;
		m_pMsgBox = 0;
	}
	
	GameGUILayout::Update( frameTime );

	// Check for game messages
	if( m_pGameListener )
	{
		while( NetMsg* pMsg = m_pGameListener->PopNextMessage() )
		{
			// If the message is a game running message
			if( pMsg->GetMsgTypeID() == GNMI_GameRunning )
			{
				NetMsgGameRunning* pGameRunMsg = reinterpret_cast<NetMsgGameRunning*>( pMsg );

				// Create the game object
				// Set the frame time to -frameTime so that when the games are update it
				// becomes 0 for this frame
				ActiveGame newGame;
				newGame.m_GameType = (GameDefines::EGameType)pGameRunMsg->m_GameType;
				newGame.m_HostDiff = (GameDefines::EGameplayDiffLevel)pGameRunMsg->m_HostDiff;
				newGame.m_LastMsgHeard = -frameTime;
				newGame.m_ServerPort = pGameRunMsg->m_ServerListenPort;
				newGame.sCreatorName = pGameRunMsg->m_sHostName;
				newGame.m_ServerIP = pGameRunMsg->m_SenderIP;

				// Find if this game is known
				ActiveGameList::iterator iterGame = FindActiveGame( pGameRunMsg->m_SenderIP, pGameRunMsg->m_ServerListenPort );

				// If the game is known
				if( iterGame != m_ActiveGames.end() )
				{
					iterGame->m_HostDiff = newGame.m_HostDiff;
					iterGame->m_GameType = newGame.m_GameType;
					iterGame->m_LastMsgHeard = newGame.m_LastMsgHeard;
				}
				// Else the game is new
				else
				{
					if( m_pGamesList )
					{
						//TODO localize
						std::wstring sListItem( pGameRunMsg->m_sHostName );
						sListItem += L"'s ";
						sListItem += GameDefines::GetGameTypeDesc( pGameRunMsg->m_GameType );
						sListItem += L" game";
						newGame.m_ListItemIndex = m_pGamesList->AddString( sListItem.c_str() );
						m_ActiveGames.push_back( newGame );
						m_pGamesList->ReformatControl();
					}
				}
			}
			// Else if this a game close message
			else if( pMsg->GetMsgTypeID() == GNMI_GameClosed )
			{
				// Remove the game
				NetMsgGameClosed* pGameCloseMsg = reinterpret_cast<NetMsgGameClosed*>( pMsg );
				RemoveActiveGame( pGameCloseMsg->m_SenderIP, pGameCloseMsg->m_ServerListenPort );
			}

			// Free the message
			delete pMsg;
		}
	}

	// Go through the active games and remove the old ones
	ActiveGameList::iterator iterGame = m_ActiveGames.begin();
	while( iterGame != m_ActiveGames.end() )
	{
		iterGame->m_LastMsgHeard += frameTime;

		// If it has been a long time since an update
		if( iterGame->m_LastMsgHeard > GAME_INACTIVE )
		{
			// Remove the item
			if( iterGame == m_ActiveGames.begin() )
			{
				RemoveActiveGame( iterGame->m_ServerIP, iterGame->m_ServerPort );
				iterGame = m_ActiveGames.begin();
			}
			else
			{
				ActiveGameList::iterator iterNext = iterGame;
				++iterNext;
				RemoveActiveGame( iterGame->m_ServerIP, iterGame->m_ServerPort );
				iterGame = iterNext;
			}

			continue;
		}

		// Advance a game
		++iterGame;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::~GUILayoutPreNetGame  Public
///
///	The destructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUILayoutPreNetGame::~GUILayoutPreNetGame()
{
	m_ServerData.Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::SetHostName  Public
///
///	\param szName The host name
///	
///	Set the host name and update the label.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::SetHostName( const wchar_t* szName )
{
	if( m_pHostNameLabel )
		m_pHostNameLabel->SetText( szName );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.  It goes through
///	the controls in the layout and finds the desired controls by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::HookupCtrls()
{
	// Allow the base class to hook up the selected controls
	GameGUILayout::HookupCtrls();

	// Set the callback for the difficulty buttons
	GUICtrlButton* pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_DiffUpBtn );
	if( pButton )
		pButton->SetClickCallback( OnDiffChangeCB );
	pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_DiffDownBtn );
	if( pButton )
		pButton->SetClickCallback( OnDiffChangeCB );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::OnActivate()
{
	GameGUILayout::OnActivate();

	m_ServerData.Clear();
	m_ClientData.Clear();

	// Link up the back button
	GUICtrlButton* pBackButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_BackButton );
	if( pBackButton )
	{
		if( m_IsLANGame )
			pBackButton->SetClickCallback( BackToLobbyCB );
		else
			pBackButton->SetClickCallback( LayoutCB_BackToMPType );
	}

	// If this computer is host
	const int32 DIFF_BTN_XOFFSET = 180;
	if( m_IsHost )
	{
		// Create the LAN sender if needed
		if( m_IsLANGame )
			m_ServerData.m_pLANConn = NetworkMgr::Get().CreateLANSendConn( LAN_ACTIVEGAME_PORT );

		// Create the TCP connection
		m_pGameConn = NetworkMgr::Get().WaitForTCPConn( DEF_TCP_LISTEN_PORT, 500 );

		// Setup the active game network broadcast message
		m_ServerData.m_GameRunningMsg.m_sHostName = L"Unknown";
		if( GameMgr::Get().GetActiveProfile() )
			m_ServerData.m_GameRunningMsg.m_sHostName = GameMgr::Get().GetActiveProfile()->GetName();
		m_ServerData.m_GameRunningMsg.m_GameType = GameDefines::GT_PrimeTime;
		m_ServerData.m_GameRunningMsg.m_HostDiff = GameDefines::GPDL_4;

		// Get the logged-in user's difficulty setting for this game type
		if( GameMgr::Get().GetActiveProfile() )
		{
			ProfileSubsetSettings* pProfileSettinsg = (ProfileSubsetSettings*)GameMgr::Get().GetActiveProfile()->GetSubset( ProfileSubsetSettings::SUBSET_ID );
			m_ServerData.m_GameRunningMsg.m_HostDiff = pProfileSettinsg->GetSettings( m_ServerData.m_GameRunningMsg.m_GameType ).diffLevel;
		}

		// Store the listen port
		if( m_pGameConn )
			m_ServerData.m_GameRunningMsg.m_ServerListenPort = m_pGameConn->GetListenPort();

		// Hide the ready text
		GUICtrlLabel* pReadyText = (GUICtrlLabel*)GetCtrlByName( CTRLNAME_ReadyHelpText );
		if( pReadyText )
			pReadyText->SetVisible( false );
		pReadyText = (GUICtrlLabel*)GetCtrlByName( CTRLNAME_ReadyHelpText2 );
		if( pReadyText )
			pReadyText->SetVisible( false );

		// Position the difficulty buttons
		GUICtrlButton* pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_DiffUpBtn );
		if( pButton && m_pHostDiffLabel )
		{
			Point2i btnPos = pButton->GetPos();
			btnPos.x = m_pHostDiffLabel->GetPos().x + DIFF_BTN_XOFFSET;
			pButton->SetPos( btnPos );
		}
		pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_DiffDownBtn );
		if( pButton && m_pHostDiffLabel )
		{
			Point2i btnPos = pButton->GetPos();
			btnPos.x = m_pHostDiffLabel->GetPos().x + DIFF_BTN_XOFFSET;
			pButton->SetPos( btnPos );
		}

		// Set the play button
		pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_ReadyPlayButton );
		if( pButton )
		{
			//TODO localize
			pButton->SetText( L"Play" );
			pButton->SetClickCallback( PlayButtonCB );
		}
	}
	// Else this computer is the client
	else
	{
		// Show the ready text and button
		GUICtrlLabel* pReadyText = (GUICtrlLabel*)GetCtrlByName( CTRLNAME_ReadyHelpText );
		if( pReadyText )
			pReadyText->SetVisible( true );
		pReadyText = (GUICtrlLabel*)GetCtrlByName( CTRLNAME_ReadyHelpText2 );
		if( pReadyText )
			pReadyText->SetVisible( true );

		// Position the difficulty buttons
		GUICtrlButton* pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_DiffUpBtn );
		if( pButton && m_pClientDiffLabel )
		{
			Point2i btnPos = pButton->GetPos();
			btnPos.x = m_pClientDiffLabel->GetPos().x + DIFF_BTN_XOFFSET;
			pButton->SetPos( btnPos );
		}
		pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_DiffDownBtn );
		if( pButton && m_pClientDiffLabel )
		{
			Point2i btnPos = pButton->GetPos();
			btnPos.x = m_pClientDiffLabel->GetPos().x + DIFF_BTN_XOFFSET;
			pButton->SetPos( btnPos );
		}

		// Set the ready button
		pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_ReadyPlayButton );
		if( pButton )
		{
			//TODO localize
			pButton->SetText( L"Ready" );
			pButton->SetClickCallback( PlayButtonCB );
		}
	}
	
	// Set the name label
	if( m_IsHost )
	{
		if( m_pHostNameLabel )
		{
			if( GameMgr::Get().GetActiveProfile() )
				m_pHostNameLabel->SetText( GameMgr::Get().GetActiveProfile()->GetName().c_str() );
			//TODO localize
			else
				m_pHostNameLabel->SetText( L"Unknown" );

			// Align the text near the versus label
			if( m_pVersusLabel )
				m_pHostNameLabel->SetPos( Point2i( m_pVersusLabel->GetPos().x - m_pHostNameLabel->GetBoundBox().size.x - 10, m_pVersusLabel->GetPos().y ) );
		}
	}
	// Else this player is the client
	else
	{
		if( m_pClientNameLabel )
		{
			if( GameMgr::Get().GetActiveProfile() )
				m_pClientNameLabel->SetText( GameMgr::Get().GetActiveProfile()->GetName().c_str() );
			//TODO localize
			else
				m_pClientNameLabel->SetText( L"Unknown" );

			// Align the text near the versus label
			if( m_pVersusLabel )
				m_pClientNameLabel->SetPos( Point2i( m_pVersusLabel->GetBoundBox().Right() + 10, m_pVersusLabel->GetPos().y ) );
		}
	}

	// Update the difficulty labels
	SetDiffLabels();
}


/// Update the difficulty level labels
void GUILayoutPreNetGame::SetDiffLabels()
{
	// Update the host label
	if( m_pHostDiffLabel )
	{
		if( m_IsHost )
			m_pHostDiffLabel->SetText( GameDefines::GetDifficultyDesc( m_LocalDiff ) );
		else
			m_pHostDiffLabel->SetText( GameDefines::GetDifficultyDesc( m_OpponentInfo.m_Diff ) );
	}

	// Update the client label
	if( m_pClientDiffLabel )
	{
		// If this player is the host then display the opponents difficulty in the client label
		if( m_IsHost )
			m_pClientDiffLabel->SetText( GameDefines::GetDifficultyDesc( m_OpponentInfo.m_Diff ) );
		else
			m_pClientDiffLabel->SetText( GameDefines::GetDifficultyDesc( m_LocalDiff ) );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::OnDeactivate  Public
///
///	A message handler called when the layout is deactivated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::OnDeactivate()
{
	// If this is a LAN game then send a closed game message
	if( m_IsHost && m_ServerData.m_pLANConn )
	{
		// Send the active game message
		NetMsgGameClosed closeGameMsg;
		closeGameMsg.m_ServerListenPort = m_pGameConn->GetListenPort();
		m_ServerData.m_pLANConn->SendNetMsg( &closeGameMsg );
	}

	// Close the game TCP connection
	if( m_pGameConn )
	{
		NetworkMgr::Get().CloseConnection( m_pGameConn );
		delete m_pGameConn;
		m_pGameConn = 0;
	}

	m_ServerData.Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::OnDiffChangeCB  Static Private
///
///	\param pButton The button that invoked this callback
///
///	The button callback for when the difficulty has been changed
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::OnDiffChangeCB( GUICtrlButton* pButton )
{
	// Get a pointer to this layout
	if( !pButton->GetContainingLayout() )
		return;
	GUILayoutPreNetGame* pLayout = (GUILayoutPreNetGame*)pButton->GetContainingLayout();

	// Determine if this is the up or down button
	bool isUpButton = pButton->GetName() == pLayout->CTRLNAME_DiffUpBtn;

	// Adjust the local difficulty
	if( isUpButton && pLayout->m_LocalDiff < GameDefines::GPDL_Post8 )
		pLayout->m_LocalDiff = (GameDefines::EGameplayDiffLevel)((int32)pLayout->m_LocalDiff + 1);
	else if( !isUpButton && pLayout->m_LocalDiff > GameDefines::GPDL_Pre3 )
		pLayout->m_LocalDiff = (GameDefines::EGameplayDiffLevel)((int32)pLayout->m_LocalDiff - 1);

	// Update the text
	pLayout->SetDiffLabels();

	// Send the update
	if( pLayout->m_pGameConn && pLayout->m_pGameConn->GetConnState() == NetConnTCP::CS_Connected )
	{
		// Create a message with our information
		NetMsgPlayerInfo playerInfoMsg;
		//TODO localize
		playerInfoMsg.m_sPlayerName = L"Unknown";
		if( GameMgr::Get().GetActiveProfile() )
			playerInfoMsg.m_sPlayerName = GameMgr::Get().GetActiveProfile()->GetName();
		playerInfoMsg.m_CurDiff = pLayout->m_LocalDiff;
		if( pLayout->m_IsHost )
			playerInfoMsg.m_IsReady = true;
		else
			playerInfoMsg.m_IsReady = pLayout->m_ClientData.m_IsReady;

		// Send the message with our information
		pLayout->m_pGameConn->SendNetMsg( &playerInfoMsg );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::BackToLobbyCB  Static Private
///
///	\param UNNAMED The button that invoked this callback
///
///	Go back to the LAN lobby.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::BackToLobbyCB( GUICtrlButton* )
{
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_LANLobby );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::PlayButtonCB  Static Private
///
///	\param pButton The button that invoked this callback
///
///	The play/ready button callback.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::PlayButtonCB( GUICtrlButton* pButton )
{
	// Get the layout
	if( !pButton->GetContainingLayout() )
		return;
	GUILayoutPreNetGame* pLayout = (GUILayoutPreNetGame*)pButton->GetContainingLayout();

	// If the player is the host
	if( pLayout->m_IsHost )
	{
		// If there is a connection
		if( pLayout->m_pGameConn && pLayout->m_pGameConn->GetConnState() == NetConnTCP::CS_Connected )
		{
			// Send the message to play the game
			NetMsgStartGameQuery startGame;
			pLayout->m_pGameConn->SendNetMsg( &startGame );
		}
	}
	// Else the player is the client
	else
	{
		// Toggle the client's ready state
		pLayout->m_ClientData.m_IsReady = !pLayout->m_ClientData.m_IsReady;

		//TODO localize
		// Set the button text
		pButton->SetText( pLayout->m_ClientData.m_IsReady ? L"Wait" : L"Ready" );

		// If there is a connection
		if( pLayout->m_pGameConn && pLayout->m_pGameConn->GetConnState() == NetConnTCP::CS_Connected )
		{
			// Send the ready message
			NetMsgPlayerInfo playerInfoMsg;
			//TODO localize
			playerInfoMsg.m_sPlayerName = L"Unknown";
			if( GameMgr::Get().GetActiveProfile() )
				playerInfoMsg.m_sPlayerName = GameMgr::Get().GetActiveProfile()->GetName();
			playerInfoMsg.m_CurDiff = pLayout->m_LocalDiff;
			playerInfoMsg.m_IsReady = pLayout->m_ClientData.m_IsReady;

			// Send the message with our information
			pLayout->m_pGameConn->SendNetMsg( &playerInfoMsg );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::ServerData::Clear  Public
///
///	Clear the server data for the pre-net game layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::ServerData::Clear()
{
	m_sClientName = L"";
	if( m_pLANConn )
	{
		NetworkMgr::Get().CloseConnection( m_pLANConn );
		delete m_pLANConn;
		m_pLANConn = 0;
	}
	m_GameRunningMsg.m_sHostName = L"";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPreNetGame::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPreNetGame::Update( float32 frameTime )
{
	GameGUILayout::Update( frameTime );

	if( !m_pGameConn )
	{
		// Create the TCP connection
		if( m_IsHost )
			m_pGameConn = NetworkMgr::Get().WaitForTCPConn( DEF_TCP_LISTEN_PORT, 500 );
		else
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_LANLobby );

		return;
	}

	// If this computer is the host
	if( m_IsHost )
	{
		// If the TCP connection is connected
		if( m_pGameConn && m_pGameConn->GetConnState() == NetConnTCP::CS_Connected )
		{
			// Receive messages
			while( NetMsg* pMsg = m_pGameConn->PopNextMessage() )
			{
				// If this is a request player information message
				if( pMsg->GetMsgTypeID() == GNMI_SendMeYourInfo )
				{
					// Send a message with our information
					NetMsgPlayerInfo playerInfoMsg;
					//TODO localize
					playerInfoMsg.m_sPlayerName = L"Unknown";
					if( GameMgr::Get().GetActiveProfile() )
						playerInfoMsg.m_sPlayerName = GameMgr::Get().GetActiveProfile()->GetName();
					playerInfoMsg.m_CurDiff = m_LocalDiff;
					playerInfoMsg.m_IsReady = true;

					m_pGameConn->SendNetMsg( &playerInfoMsg );
				}
				// Else if this is the other player's info
				else if( pMsg->GetMsgTypeID() == GNMI_PlayerInfo )
				{
					NetMsgPlayerInfo* pInfoMsg = (NetMsgPlayerInfo*)pMsg;
					m_OpponentInfo.sName = pInfoMsg->m_sPlayerName;
					if( m_pClientNameLabel )
						m_pClientNameLabel->SetText( m_OpponentInfo.sName.c_str() );

					// Store and update the difficulty
					m_OpponentInfo.m_Diff = pInfoMsg->m_CurDiff;
					SetDiffLabels();

					m_OpponentInfo.isReady = pInfoMsg->m_IsReady;
				}
				// Else if the client responded that he/she is ready to start
				else if( pMsg->GetMsgTypeID() == GNMI_StartGameResponse )
				{
					// If the client is not ready then don't allow it
					NetMsgStartGameResponse* pStartMsg = (NetMsgStartGameResponse*)pMsg;
					if( !pStartMsg->m_AllowStart )
					{

					}
					else
					{
						// The game can start!
					}
				}

				// Free the message
				delete pMsg;
			}

			// If the client's information has not been received yet
			if( m_ServerData.m_sClientName == L"" && m_TimeSinceSend > 0.5f )
			{
				NetMsgSendMeYourInfo msg;
				m_pGameConn->SendNetMsg( &msg );
				m_TimeSinceSend = 0.0f;
			}
		}
		// Else the TCP is not connected yet
		else
		{
			// If this is a LAN game and no one has connected yet
			if( m_ServerData.m_pLANConn && m_ServerData.m_sClientName == L"" )
			{
				// If this computer is the host then send an update message if it is time
				if( m_TimeSinceSend > SERVER_SEND_ACTIVE_GAME_TIME )
				{
					m_ServerData.m_pLANConn->SendNetMsg( &m_ServerData.m_GameRunningMsg );
					m_TimeSinceSend = 0.0f;
				}
			}
		}
	}
	// Else this computer is the client
	else
	{
		// If the client is not connected then bail
		if( m_pGameConn->GetConnState() != NetConnTCP::CS_Connected )
		{
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_LANLobby );
			return;
		}

		// Receive messages
		while( NetMsg* pMsg = m_pGameConn->PopNextMessage() )
		{
			// If this is a request player information message
			if( pMsg->GetMsgTypeID() == GNMI_SendMeYourInfo )
			{
				// Send a message with our information
				NetMsgPlayerInfo playerInfoMsg;
				//TODO localize
				playerInfoMsg.m_sPlayerName = L"Unknown";
				if( GameMgr::Get().GetActiveProfile() )
					playerInfoMsg.m_sPlayerName = GameMgr::Get().GetActiveProfile()->GetName();
				playerInfoMsg.m_CurDiff = m_LocalDiff;
				playerInfoMsg.m_IsReady = m_ClientData.m_IsReady;

				m_pGameConn->SendNetMsg( &playerInfoMsg );
			}
			// Else if this is the other player's info
			else if( pMsg->GetMsgTypeID() == GNMI_PlayerInfo )
			{
				NetMsgPlayerInfo* pInfoMsg = (NetMsgPlayerInfo*)pMsg;
				m_OpponentInfo.sName = pInfoMsg->m_sPlayerName;
				if( m_pHostNameLabel )
					m_pHostNameLabel->SetText( m_OpponentInfo.sName.c_str() );
				m_OpponentInfo.m_Diff = pInfoMsg->m_CurDiff;
				m_OpponentInfo.isReady = pInfoMsg->m_IsReady;
			}
			// Else if the server wants to send the start game message
			else if( pMsg->GetMsgTypeID() == GNMI_StartGameQuery )
			{
				NetMsgStartGameResponse startMsg;
				
				// Set if the client is ready
				startMsg.m_AllowStart = m_ClientData.m_IsReady;

				// Send the message
				m_pGameConn->SendNetMsg( &startMsg );
			}

			// Free the message
			delete pMsg;
		}
	}

	// Update the send time, the update is down here just to make the coad a little easier to read
	m_TimeSinceSend += frameTime;
}