/*=================================================================================================

	\file GameMgrCtrl.cpp
	Game Play Library
	Game Manager GUI Control Source
	\author Taylor Clark
	\Date March 25, 2006

	This source file contains the implementation of the game manager GUI control class.

=================================================================================================*/

#include "../GameMgrCtrl.h"
#include "../GameMgr.h"
#include "../GameLogicProduct.h"
#include "../GameField.h"
#include "GUI/GUIMgr.h"
#include "GUI/GUIMessages.h"
#include "Graphics2D/GraphicsMgr.h"
#include "GUI/MsgBox.h"

const wchar_t* GameMgrCtrl::CTRL_NAME = L"gamefield_ctrl";
// VK_ESCAPE = 0x1B = 27
const uint32 KEYCODE_ESCAPE = 27;

/// Get the bounding box for this control
Box2i GameMgrCtrl::InternalGetBoundBox() const
{
	return Box2i( Point2i(), g_pGraphicsMgr->GetDisplayDims() );
}

void GameMgrCtrl::Update( float32 )
{
	// If the exit message box is still up then do nothing
	if( !m_pExitMsgBox || m_pExitMsgBox->GetRetVal() == MsgBox::MBR_Error )
		return;

	// Get the return value and close the box
	MsgBox::EMsgBoxReturn msgBoxRet = m_pExitMsgBox->GetRetVal();
	GUIMgr::Get().CloseMessageBox( m_pExitMsgBox );
	delete m_pExitMsgBox;
	m_pExitMsgBox = 0;

	// If the user wants to quit
	if( msgBoxRet == MsgBox::MBR_Yes )
		GameMgr::Get().EndGame();
	else
		GameMgr::Get().SetPauseState( false );
}

GameMgrCtrl::~GameMgrCtrl()
{
	if( m_pExitMsgBox )
	{
		GUIMgr::Get().CloseMessageBox( m_pExitMsgBox );
		delete m_pExitMsgBox;
		m_pExitMsgBox = 0;
	}
}

/// Draw the control
void GameMgrCtrl::Draw() const
{
	if( GameMgr::Get().GetLogic() )
	{
		if( GameMgr::Get().GetLogic()->GetGameField() )
			GameMgr::Get().GetLogic()->GetGameField()->Draw();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameMgrCtrl::OnMouseDown  Public
///
///	\returns True if the message was handled, otherwise false
///
/// Handle a mouse button being pressed
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameMgrCtrl::OnMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos )
{
	GameLogic* pGameLogic = GameMgr::Get().GetLogic();
	if( pGameLogic )
	{
		if( pGameLogic->GetGameField() )
			pGameLogic->GetGameField()->ClickAtPoint( mousePos, btn );
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameMgrCtrl::OnKeyDown  Public
///
///	\returns True if the message was handled, otherwise false
///
/// Handle a key being pressed down
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GameMgrCtrl::OnKeyDown( unsigned int windowsKeyCode )
{
	GameLogic* pGameLogic = GameMgr::Get().GetLogic();
	if( windowsKeyCode == KEYCODE_ESCAPE && !m_pExitMsgBox )
	{
		// Popup a message box
		GameMgr::Get().SetPauseState( true );
		m_pExitMsgBox = MsgBox::CreateMsgBox( L"Would you like to quit the current game and exit back to the main menu?", MsgBox::BL_YesNo );
		GUIMgr::Get().PopupMessageBox( m_pExitMsgBox );
	}
	else if( windowsKeyCode == GameDefines::KEY_CODE_FASTPUSH
			&& pGameLogic
			&& (pGameLogic->GetLogicType() == GameLogic::LT_Product || pGameLogic->GetLogicType() == GameLogic::LT_Fraction) )
	{
		GameLogicNormalBase* pMultLogic = static_cast<GameLogicNormalBase*>(pGameLogic);
		pMultLogic->OnSpeedButtonDown();
	}
	else
		return false;
	
	return true;
}

/// Handle a key being released
bool GameMgrCtrl::OnKeyUp( unsigned int windowsKeyCode )
{
	GameLogic* pGameLogic = GameMgr::Get().GetLogic();
	if( windowsKeyCode == GameDefines::KEY_CODE_FASTPUSH
		&& pGameLogic
		&& (pGameLogic->GetLogicType() == GameLogic::LT_Product || pGameLogic->GetLogicType() == GameLogic::LT_Fraction))
	{
		GameLogicNormalBase* pMultLogic = static_cast<GameLogicNormalBase*>( pGameLogic );
		pMultLogic->OnSpeedButtonUp();
	}
	
	return true;
}