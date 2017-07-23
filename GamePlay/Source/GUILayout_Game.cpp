//=================================================================================================
/*!
	\file GUILayout_Game.cpp
	Game Play Library
	Game GUI Layouts Source
	\author Taylor Clark
	\date July 10, 2006

	This source file contains the implementation for the game GUI layout classes.
*/
//=================================================================================================

#include "../GUILayout_Game.h"
#include "GUI/GUICtrlList.h"
#include "GUI/GUICtrlLabel.h"
#include "GUI/GUICtrlSprite.h"
#include "../GameMgr.h"
#include "GUI/GUICtrlButton.h"
#include "GUI/GUICtrlCheckBox.h"
#include "GUI/GUICtrlTextBox.h"
#include "Base/MsgLogger.h"
#include "GUI/MsgBox.h"
#include "GUI/GUIMgr.h"
#include "../PlayerProfile.h"
#include "GUI/GUICtrlScrollList.h"
#include "GUI/GUIMessages.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Base/NumFuncs.h"
#include <time.h>
#include "../GameLogicProduct.h"
#include "../GameMgrCtrl.h"
#include "../GameField.h"
#include "../ProfileSubsetSettings.h"
#include "PrimeTime/ApplicationBase.h"
#include "Base/FileFuncs.h"
#include <sstream>

const int32 AdditionAttackPlayTimeInSec = 60 * 3;

struct ExpectedControl
{
	const wchar_t* szName;
	GUIDefines::EControlType ctrlType;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  LayoutCB_BackToTitle  Static Global
///
///	\param UNNAMED The button that sent this message
///
///	A message handler called when the user presses the back button to go back to the title screen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void LayoutCB_BackToTitle( GUICtrlButton* )
{
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_TitleScreen );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTitleScreen::TitleScreenCB_MainListSel  Static Private
///
///	\param selIndex The index of the selected list item
///	\param UNNAMED The list box that sent this message
///
///	A message handler called when the user selects an item on the main title screen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::TitleScreenCB_MainListSel( uint32 selIndex, GUICtrlList* pList )
{
	if( !pList || !pList->GetContainingLayout() )
		return;
	GUILayoutTitleScreen* pLayout = (GUILayoutTitleScreen*)pList->GetContainingLayout();

	// If single player was selected
	if( selIndex == LISTITEMINDEX_1PLAYER )
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_ModeSelect );
	// Else if 2-player was selected
	/*else if( selIndex == LISTITEMINDEX_2PLAYER )
	{
		if( GameMgr::Get().GetActiveProfile() )
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_MultiplayerType );
		else
		{
			//TODO localize
			pLayout->m_pMustLogInMsgBox = MsgBox::CreateMsgBox( L"You must be logged in to play a 2 player game." );
			GUIMgr::Get().PopupMessageBox( pLayout->m_pMustLogInMsgBox );
		}
	}*/
#ifndef NOPROFILES
	// Else if the player wants to log in
	else if( selIndex == LISTITEMINDEX_LogIn )
	{
		// If the game is being run off the CD then don't allow profiles
		if( ApplicationBase::Get()->IsRunningOffCD() )
		{
			//TODO localize
			pLayout->m_pMustLogInMsgBox = MsgBox::CreateMsgBox( L"You cannot use profiles when playing from the CD." );
			GUIMgr::Get().PopupMessageBox( pLayout->m_pMustLogInMsgBox );
		}
		// Else just go to the profile selection screen
		else
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_ProfileSelection );
	}
	else if( selIndex == LISTITEMINDEX_HELPMELEARN )
#else
	else if( selIndex == LISTITEMINDEX_HELPMELEARN - 1 )
#endif
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_HelpMeLearn );
	/*else if( selIndex == LISTITEMINDEX_MYSTATS )
	{
		if( GameMgr::Get().GetActiveProfile() )
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_ViewStats );
		else
		{
			//TODO localize
			pLayout->m_pMustLogInMsgBox = MsgBox::CreateMsgBox( L"You must be logged in to view stats." );
			GUIMgr::Get().PopupMessageBox( pLayout->m_pMustLogInMsgBox );
		}
		
	}*/
	// Else no one is logged in

	// If the last item was selected then exit
	else if( selIndex == pList->GetNumListItems() - 1 )
		ApplicationBase::Get()->SetAppToExit();

}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTitleScreen::TitleScreenCB_MainListSel_Demo  Static Private
///
///	\param selIndex The index of the selected list item
///	\param UNNAMED The list box that sent this message
///
///	A message handler called when the user selects an item on the main title screen for the demo
///	version of the game.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::TitleScreenCB_MainListSel_Demo( uint32 selIndex, GUICtrlList* )
{
	if( selIndex == 0 )
	{
		GameMgr::Get().SetGameplayDiff( GameDefines::GPDL_3 );
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( GameDefines::GT_PrimeTime );
	}
	else if( selIndex == 1 )
	{
		GameMgr::Get().SetGameplayDiff( GameDefines::GPDL_7 );
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( GameDefines::GT_PrimeTime );
	}
	// Else go to tutorials
	else if( selIndex == 2 )
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_Tutorials );
	else
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_DemoExit );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutTitleScreen::TitleScreenCB_AdjustVolumeClick  Static Private
///
///	\param UNNAMED The button that sent this message
///
///	A message handler called when the user clicks the button to adjust the volume
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::TitleScreenCB_AdjustVolumeClick( GUICtrlButton* )
{
    float32 masterVolume = AudioMgr::Get().GetMasterVolume();
	if( masterVolume < 0.1f )
		AudioMgr::Get().SetMasterVolume( 1.0f );
	else if( masterVolume < 0.6f )
		AudioMgr::Get().SetMasterVolume( 0.0f );
	else
		AudioMgr::Get().SetMasterVolume( 0.5f );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutTitleScreen::HandleMessage  Public
///
///	\param pMsg The GUI message to handle
///	\returns Whether or not the message was handled
///
///	Handle a GUI message.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUILayoutTitleScreen::HandleMessage( GUIMessage* pMsg )
{
	// Allow the normal message handling to process
	if( GameGUILayout::HandleMessage( pMsg ) )
		return true;

	// If the unhandled message is a keydown message
    bool toggleVideo = false;
	if( pMsg->GetType() == GUIDefines::GMT_KeyDown )
	{
		GUIMessageKeyDown* pKeyMsg = reinterpret_cast<GUIMessageKeyDown*>( pMsg );
		
		// If 'V' was pushed then toggle the video
		toggleVideo = pKeyMsg->GetKeyCode() == (uint32)'V';
	}
    else if( pMsg->GetType() == GUIDefines::GMT_KeyChar )
	{
		GUIMessageKeyChar* pKeyMsg = reinterpret_cast<GUIMessageKeyChar*>( pMsg );
		
		// If 'V' was pushed then toggle the video
		toggleVideo = pKeyMsg->GetChar() == L'V' || pKeyMsg->GetChar() == L'v';
	}

    if( toggleVideo )
        TitleScreenCB_AdjustVideoClick( NULL );

	return toggleVideo;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutTitleScreen::TitleScreenCB_AdjustVideoClick  Static Private
///
///	\param UNNAMED The button that sent this message
///
///	A message handler called when the user clicks the button to adjust the video resolution
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::TitleScreenCB_AdjustVideoClick( GUICtrlButton* )
{
    ApplicationBase::Get()->CycleToNextDisplayMode();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTitleScreen::SetCustomText  Public
///
///	\param szText The text to display on the title screen, NULL to clear the text
///
/// Set the custom text
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::SetCustomText( const wchar_t* szText )
{
	if( !m_pCustomTextLabel )
		return;

	// If the label should be hidden
	if( !szText )
		m_pCustomTextLabel->SetVisible( false );
	else
	{
		m_pCustomTextLabel->SetVisible( true );
		m_pCustomTextLabel->SetText( szText );

		// Center the text
		Point2i labelPos = m_pCustomTextLabel->GetPos();
		labelPos.x = (g_pGraphicsMgr->GetDisplayDims().x - m_pCustomTextLabel->GetBoundBox().size.x) / 2;
		m_pCustomTextLabel->SetPos( labelPos );
	}
}


/// Set the custom picture
void GUILayoutTitleScreen::SetCustomPic( TCImage* pCustomPicImage, const wchar_t* szText )
{
	if( m_pCustomPicMsgLabel )
	{
		// If no message was provided then hide the label
		if( !szText )
			m_pCustomPicMsgLabel->SetVisible( false );
		else
		{
			m_pCustomPicMsgLabel->SetVisible( true );
			m_pCustomPicMsgLabel->SetText( szText );

			int32 leftX = m_pCustomPicMsgLabel->GetPos().x + 64;
			if( m_pCustomPicSprite )
				leftX = m_pCustomPicSprite->GetPos().x + 64;
			leftX -= m_pCustomPicMsgLabel->GetBoundBox().size.x / 2;

			// Center the text under the image
			Point2i labelPos = m_pCustomPicMsgLabel->GetPos();
			labelPos.x = leftX;
			m_pCustomPicMsgLabel->SetPos( labelPos );
		}
	}

	// If we found the custom pic sprite control
	if( m_pCustomPicSprite )
	{
		// If an image was provided
		if( pCustomPicImage )
		{
			RefSprite* pSourceSprite = g_pGraphicsMgr->CreateSpriteForImage( pCustomPicImage );
			AnimSprite newSprite(pSourceSprite);
			m_pCustomPicSprite->SetSprite( newSprite );

			m_pCustomPicSprite->SetVisible( true );
		}
		else
			m_pCustomPicSprite->SetVisible( false );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTitleScreen::SetGameVersionText  Public
///
///	\param gameVersion The game version times 10. For example, version 1.1 is passed in as 11
///
/// Set the game version label
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::SetGameVersionText( int gameVersion )
{
	if( !m_pGameVersionLabel )
		return;

	// Split the version up into its components
	int majorVersion = gameVersion / 10;
	int minorVersion = gameVersion % 10;

	// Create the string
	std::wstring sVersionText = L"v";
	sVersionText += TCBase::EasyIToA( majorVersion );
	sVersionText += L".";
	sVersionText += TCBase::EasyIToA( minorVersion );

	// Set the text
	m_pGameVersionLabel->SetText( sVersionText.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTitleScreen::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::Update( float32 frameTime )
{
	GameGUILayout::Update( frameTime );

	// If there is a message box telling the user to log-in
	if( m_pMustLogInMsgBox )
	{
		// If it is ready to close
		if( m_pMustLogInMsgBox->GetRetVal() != MsgBox::MBR_Error )
		{
			// Close the message box
			GUIMgr::Get().CloseMessageBox( m_pMustLogInMsgBox );
			delete m_pMustLogInMsgBox;
			m_pMustLogInMsgBox = 0;
		}
	}

	// If there is a message box asking if the user wants to try the tutorial
	if( m_pAskTryTutorialMsgBox )
	{
		// If it is ready to close
		if( m_pAskTryTutorialMsgBox->GetRetVal() != MsgBox::MBR_Error )
		{
			// Store the result
			MsgBox::EMsgBoxReturn retVal = m_pAskTryTutorialMsgBox->GetRetVal();

			// Close the message box
			GUIMgr::Get().CloseMessageBox( m_pAskTryTutorialMsgBox );
			delete m_pAskTryTutorialMsgBox;
			m_pAskTryTutorialMsgBox = 0;

			// If the player wants to try the tutorial
			if( retVal == MsgBox::MBR_Yes )
			{
				GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
				GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_PrimeTime | GameDefines::GT_Mod_Tutorial) );
			}
			// Else just play
			else
				GUIMgr::Get().SetActiveLayout( GameDefines::MS_ModeSelect );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTitleScreen::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.  It goes through
///	the controls in the layout and finds the desired controls by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::HookupCtrls()
{
	// Allow the base class to hook up the selected controls
	GameGUILayout::HookupCtrls();

	// Link up the main list
	if( m_pMainList )
	{
#ifdef _PT_DEMO
		m_pMainList->SetCallback( TitleScreenCB_MainListSel_Demo );
#else
		m_pMainList->SetCallback( TitleScreenCB_MainListSel );
#endif
	}

    // Make the volume and video buttons react
    if( m_pAdjustVolumeButton )
        m_pAdjustVolumeButton->SetClickCallback( TitleScreenCB_AdjustVolumeClick );

    if( m_pAdjustVideoButton )
        m_pAdjustVideoButton->SetClickCallback( TitleScreenCB_AdjustVideoClick );
}


//TODO localize
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTitleScreen::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTitleScreen::OnActivate()
{
	GameGUILayout::OnActivate();

#ifdef _PT_DEMO
	if( m_pMainList )
	{
		m_pMainList->Clear();
		m_pMainList->AddString( L"Easy Demo" );
		m_pMainList->AddString( L"Hard Demo" );
		m_pMainList->AddString( L"How To Play" );
		m_pMainList->AddString( L"Exit" );
		
		CenterCtrlHorz( m_pMainList );
		m_pMainList->ReformatControl();
	}

	if( m_pLoggedInLabel )
		m_pLoggedInLabel->SetVisible( false );

#else
	if( m_pMainList )
	{
		m_pMainList->Clear();
		m_pMainList->AddString( L"1 Player" );
		
#ifdef NOPROFILES
		if( m_pLoggedInLabel )
			m_pLoggedInLabel->SetVisible( false );
#else
		if( GameMgr::Get().GetActiveProfile() )
			m_pMainList->AddString( L"Log In or Out" );
		else
			m_pMainList->AddString( L"Log In" );
#endif
		m_pMainList->AddString( L"Help Me Learn" );
		m_pMainList->AddString( L"Exit" );
		
		CenterCtrlHorz( m_pMainList );
		m_pMainList->ReformatControl();
	}

	// If a user is logged in that allow statistics to be selected
	PlayerProfile* pCurProfile = GameMgr::Get().GetActiveProfile();
	if( pCurProfile )
	{
		// Update the logged-in label
		if( m_pLoggedInLabel )
		{
			std::wstring sMsg( L"You are logged in as " );
			sMsg += pCurProfile->GetName();
			m_pLoggedInLabel->SetText( sMsg.c_str() );
		}
	}
	// Else no one is logged in
	else
	{
		if( m_pLoggedInLabel )
			m_pLoggedInLabel->SetText( L"No player is logged in" );
	}
#endif //_PT_DEMO
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutPlayMode::ModeSelCallback  Static Private
///	\param selIndex The index of the selected list item
///	\param UNNAMED The list box that sent this message
///
///	A message handler called when the player selects a polay mode on the play mode screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPlayMode::ModeSelCallback( uint32 selIndex, GUICtrlList* pList )
{
	GameMgr::Get().SetTimeLimit( 0 );

	// Get the game type based on selection and bail on an invalid value
	GameDefines::EGameType gameType = GameDefines::GT_PrimeTime;
	if( selIndex == LISTITEMINDEX_PRIMETIME )
	{
		gameType = GameDefines::GT_PrimeTime;

		if( GUIMgr::Get().IsShiftDown() )
		{
			// Get the options layout
			GUILayoutModeOptions* pOptionsLayout = reinterpret_cast<GUILayoutModeOptions*>( GUIMgr::Get().GetLayoutByID( GameDefines::MS_ModeOptions ) );
			if( pOptionsLayout )
			{
				// Set the game type and difficulty then go to the options screen
				pOptionsLayout->SetGameType( gameType );
				
				GUILayoutPlayMode* pCurLayout = reinterpret_cast<GUILayoutPlayMode*>( pList->GetContainingLayout() );
				if( pCurLayout )
					pOptionsLayout->SetGameDifficulty( pCurLayout->m_GameDiff );
				
				GUIMgr::Get().SetActiveLayout( GameDefines::MS_ModeOptions );

				return;
			}
		}
	}
	else if( selIndex == LISTITEMINDEX_CEILING )
		gameType = GameDefines::GT_Ceiling;
	else if( selIndex == LISTITEMINDEX_ADDITIONATTACK )
	{
		gameType = GameDefines::GT_AdditionAttack;
		GameMgr::Get().SetTimeLimit( AdditionAttackPlayTimeInSec );
	}
	else if( selIndex == LISTITEMINDEX_FRACTIONS )
		gameType = GameDefines::GT_Fractions;
	//else if( selIndex == LISTITEMINDEX_CUSTOM )
	//{
	//	// Get the options layout
	//	GUILayoutModeOptions* pOptionsLayout = reinterpret_cast<GUILayoutModeOptions*>( GUIMgr::Get().GetLayoutByID( GameDefines::MS_ModeOptions ) );
	//	if( pOptionsLayout )
	//	{
	//		// Set the game type and go to the options screen
	//		pOptionsLayout->SetGameType( gameType );
	//		GUIMgr::Get().SetActiveLayout( GameDefines::MS_ModeOptions );
	//	}

	//	return;
	//}
	else
		return;

	// If the player has not been prompted to try the tutorial for the selected game mode yet
	bool hasbeenPromoptedForTutorial = ApplicationBase::Get()->Settings.PromptedForTutorial[(int32)gameType - 1];
	if( !hasbeenPromoptedForTutorial )
	{
		GUILayoutPlayMode* pLayout = reinterpret_cast<GUILayoutPlayMode*>( pList->GetContainingLayout() );

		// Store which game type the player picked
		pLayout->_selectedGameType = gameType;

		//TODO localize
		std::wostringstream outStr;
		outStr << L"Would you like to quickly play the " << GameDefines::GetGameTypeDesc(gameType) << L" tutorial to learn how to play? It takes under 1 minute and will really help!";
			
		pLayout->m_pAskTryTutorialMsgBox = MsgBox::CreateMsgBox( outStr.str().c_str(), MsgBox::BL_YesNo );
		GUIMgr::Get().PopupMessageBox( pLayout->m_pAskTryTutorialMsgBox );

		// Set the flag and remember it
		ApplicationBase::Get()->Settings.PromptedForTutorial[(int32)gameType - 1] = true;
		ApplicationBase::Get()->Settings.SaveToFile();
	}
	// Otherwise the player has been prompted so don't ask
	else
	{
		// Start the game!
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( gameType );
	}
}

//const wchar_t* GetDifficultyName( GameDefines::EGameType gameType, GameDefines::EGameplayDiffLevel difficulty )
//{
//	return GameDefines::GetDifficultyDesc( difficulty );
//}
//
//const wchar_t* GetDifficultyDescription( GameDefines::EGameType gameType, GameDefines::EGameplayDiffLevel difficulty )
//{
//	switch( gameType & GameDefines::GT_Mask_Type )
//	{
//	case GameDefines::GT_PrimeTime:
//		{
//			switch( difficulty )
//			{
//			case GameDefines::GPDL_Pre3:
//				return L"";
//			}
//		}
//		break;
//
//	case GameDefines::GT_Ceiling:
//		{
//		}
//		break;
//
//	case GameDefines::GT_AdditionAttack:
//		{
//		}
//		break;
//
//	case GameDefines::GT_Fractions:
//		{
//
//		}
//		break;
//	}
//
//	return L"";
//}

void GUILayoutPlayMode::AdjustDifficulty( int32 difficultyChange )
{
	// Increase the difficulty
	if( difficultyChange > 0 )
	{
		const int32 MaxDiff = (int32)GameDefines::GPDL_Post8;

		if( (int32)m_GameDiff + difficultyChange > MaxDiff )
			m_GameDiff = GameDefines::GPDL_Post8;
		else
			m_GameDiff = (GameDefines::EGameplayDiffLevel)( (int32)m_GameDiff + difficultyChange );
	}
	else
	{
		const int32 MinDiff = (int32)GameDefines::GPDL_Pre3;

		if( (int32)m_GameDiff + difficultyChange < MinDiff )
			m_GameDiff = GameDefines::GPDL_Pre3;
		else
			m_GameDiff = (GameDefines::EGameplayDiffLevel)( (int32)m_GameDiff + difficultyChange );
	}

	// Update the label control
	if( m_pDiffLabel )
	{
		m_pDiffLabel->SetText( GameDefines::GetDifficultyDesc( m_GameDiff ) );

		// Right-align the difficulty text
		if( m_pDifficultyUpButton )
		{
			Point2i newPos = m_pDiffLabel->GetPos();
			newPos.x = m_pDifficultyUpButton->GetBoundBox().pos.x - m_pDiffLabel->GetBoundBox().size.x - 5;

			m_pDiffLabel->SetPos( newPos );
		}
	}

	GameMgr::Get().SetGameplayDiff( m_GameDiff );
}

void GUILayoutPlayMode::OnDiffChange( GUICtrlButton* pButton )
{
	// Get the mode options layout
	if( !pButton->GetContainingLayout() )
		return;
	GUILayoutPlayMode* pLayout = reinterpret_cast<GUILayoutPlayMode*>(pButton->GetContainingLayout());

	pLayout->AdjustDifficulty( pButton == pLayout->m_pDifficultyUpButton ? 1 : -1 );	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPlayMode::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.  It goes through
///	the controls in the layout and finds the desired controls by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPlayMode::HookupCtrls()
{
	// Allow the base class to hook up the selected controls
	GameGUILayout::HookupCtrls();

	// Link up the main list callback
	if( m_pModeList )
		m_pModeList->SetCallback( ModeSelCallback );

	// Link up the back button
	GUIControl* pPlayCtrl = GetCtrlByName( CTRLNAME_BackButton );
	GUICtrlButton* pBackButton = 0;
	if( pPlayCtrl )
	{
		pBackButton = (GUICtrlButton*)pPlayCtrl;
		pBackButton->SetClickCallback( LayoutCB_BackToTitle );
	}

	// Link up the help label button
	//pPlayCtrl = GetCtrlByName( CTRLNAME_HelpLabel );
	//f( pPlayCtrl )
	//	m_pHelpLabel = (GUICtrlLabel*)pPlayCtrl;

	// Resize the label box
	if( m_pHelpLabel && pBackButton )
	{
		// Set the position
		m_HelpLabelBox.pos.x = 97;
		m_HelpLabelBox.pos.y = 380;
		m_pHelpLabel->SetPos( m_HelpLabelBox.pos );

		// Set the dimensions
		m_HelpLabelBox.size.x = (g_pGraphicsMgr->GetDisplayDims().x / 5) * 4;
		m_HelpLabelBox.size.x /= 2;
		m_HelpLabelBox.size.x -= 30;
		m_HelpLabelBox.size.y = pBackButton->GetPos().y - m_HelpLabelBox.pos.y;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPlayMode::Update  Public
///
///	\param frametime The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPlayMode::Update( float32 frametime )
{
	GameGUILayout::Update( frametime );

	// If there is a message box asking if the user wants to try the tutorial
	if( m_pAskTryTutorialMsgBox )
	{
		// If it is ready to close
		if( m_pAskTryTutorialMsgBox->GetRetVal() != MsgBox::MBR_Error )
		{
			// Store the result
			MsgBox::EMsgBoxReturn retVal = m_pAskTryTutorialMsgBox->GetRetVal();

			// Close the message box
			GUIMgr::Get().CloseMessageBox( m_pAskTryTutorialMsgBox );
			delete m_pAskTryTutorialMsgBox;
			m_pAskTryTutorialMsgBox = 0;

			GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
			
			// If the player wants to try the tutorial
			if( retVal == MsgBox::MBR_Yes )
				GameMgr::Get().StartGame( (GameDefines::EGameType)(_selectedGameType | GameDefines::GT_Mod_Tutorial) );
			// Else just play
			else
				GameMgr::Get().StartGame( _selectedGameType );
		}

		return;
	}

	// If the pointers to the mode list or help label are null then bail
	if( !m_pModeList || !m_pHelpLabel )
		return;

	// Get the current list item
	int32 curListItem = m_pModeList->GetMouseOverItem();

	// If the selection has not changed then bail
	if( curListItem == m_CurListItem )
		return;

	// Store the new item
	m_CurListItem = curListItem;

	// If the mouse is over nothing then clear the help label
	if( m_CurListItem == -1 )
		m_pHelpLabel->SetText( L"" );
	else
	{
		//TODO localize
		std::wstring sText;
		if( m_CurListItem == 0 )
			sText = L"The standard play style where you select prime number blocks (the white blocks) then select the block that contains the product of the selected primes (the black blocks).";
		else if( m_CurListItem == 1 )
			sText = L"Similar to Prime Time mode, but there are crates at the top of the playfield. You must clear pieces to try and fit the playfield into the top of the screen perfectly.";
		else if( m_CurListItem == 2 )
			sText = L"This game mode challenges your addition skills at insane speeds. You must select blocks on the field that add up to the sum block alongside the field. Clear as many sums as you can before time runs out!";
		else if( m_CurListItem == 3 )
			sText = L"Improve your fraction skills with this tough mode. On the left of the screen you will be shown a fraction or decimal and it's your job to find equivalent fractions on the field.";

		// Wrap the text and display it
		if( m_pHelpLabel->GetFont() )
			sText = m_pHelpLabel->GetFont()->GetWordWrapString( sText.c_str(), m_HelpLabelBox );
		m_pHelpLabel->SetText( sText.c_str() );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPlayMode::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPlayMode::OnActivate()
{
	GameGUILayout::OnActivate();

	// Clear the help label
	if( m_pHelpLabel )
		m_pHelpLabel->SetText( L"" );

	if( m_pDifficultyUpButton )
		m_pDifficultyUpButton->SetClickCallback( OnDiffChange );

	if( m_pDifficultyDownButton )
		m_pDifficultyDownButton->SetClickCallback( OnDiffChange );

	// Initialize the difficulty
	AdjustDifficulty( 0 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutModeOptions::BackButtonCB  Static Private
///
///	The callback for when the user presses back from the mode options layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutModeOptions::BackButtonCB( GUICtrlButton* )
{
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_ModeSelect );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutModeOptions::PlayButtonCB  Static Private
///
///	The callback for when the user presses play from the mode options layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutModeOptions::PlayButtonCB( GUICtrlButton* pButton )
{
	// Get the layout
	GUILayoutModeOptions* pOptionsLayout = reinterpret_cast<GUILayoutModeOptions*>( pButton->GetContainingLayout() );
	if( !pOptionsLayout )
		return;
	
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
	GameMgr::Get().StartGame( pOptionsLayout->m_GameType );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutModeOptions::DiffScrollUpCB  Static Private
///
///	The callback for when the user presses the up scroll button for difficulty on the game mode
///	options.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutModeOptions::DiffScrollUpCB( GUICtrlButton* pButton )
{
	// Get the mode options layout
	if( !pButton->GetContainingLayout() )
		return;
	GUILayoutModeOptions* pLayout = reinterpret_cast<GUILayoutModeOptions*>(pButton->GetContainingLayout());

	// Increment the difficulty
	if( pLayout->m_GameDiff < GameDefines::GPDL_Post8 )
		pLayout->SetGameDifficulty( (GameDefines::EGameplayDiffLevel)( (int32)pLayout->m_GameDiff + 1 ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutModeOptions::DiffScrollDownCB  Static Private
///
///	The callback for when the user presses the down scroll button for difficulty on the game mode
///	options.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutModeOptions::DiffScrollDownCB( GUICtrlButton* pButton )
{
	// Get the mode options layout
	if( !pButton->GetContainingLayout() )
		return;
	GUILayoutModeOptions* pLayout = reinterpret_cast<GUILayoutModeOptions*>(pButton->GetContainingLayout());

	// Decrement the time limit
	if( pLayout->m_GameDiff > GameDefines::GPDL_Pre3 )
		pLayout->SetGameDifficulty( (GameDefines::EGameplayDiffLevel)( (int32)pLayout->m_GameDiff - 1 ) );
}

void GUILayoutModeOptions::TimeScrollUpCB( GUICtrlButton* pButton )
{
	// Get the mode options layout
	if( !pButton->GetContainingLayout() )
		return;
	GUILayoutModeOptions* pLayout = reinterpret_cast<GUILayoutModeOptions*>(pButton->GetContainingLayout());

	// Increment the time limit
	if( pLayout->m_TimeLimit < 60 * 60)
	{
		if( pLayout->m_TimeLimit < 10 * 60 )
			pLayout->m_TimeLimit += 60;
		else
			pLayout->m_TimeLimit += 60 * 5;
	}

	// Update the time limit
	pLayout->UpdateTimeLimit();
}

void GUILayoutModeOptions::TimeScrollDownCB( GUICtrlButton* pButton )
{
	// Get the mode options layout
	if( !pButton->GetContainingLayout() )
		return;
	GUILayoutModeOptions* pLayout = reinterpret_cast<GUILayoutModeOptions*>(pButton->GetContainingLayout());

	// Decrement the time limit
	if( pLayout->m_TimeLimit > 60 )
	{
		if( pLayout->m_TimeLimit <= 10 * 60 )
			pLayout->m_TimeLimit -= 60;
		else
			pLayout->m_TimeLimit -= 60 * 5;
	}

	// Update the time limit
	pLayout->UpdateTimeLimit();
}

// The check box callback
void GUILayoutModeOptions::TimeLimitCheckCB( GUICtrlCheckBox* pCheckBox )
{
	// Get the mode options layout
	if( !pCheckBox->GetContainingLayout() )
		return;
	GUILayoutModeOptions* pLayout = reinterpret_cast<GUILayoutModeOptions*>(pCheckBox->GetContainingLayout());

	// If the time limit hasn't been set yet, default it to 2 minutes
	if( pCheckBox->GetChecked() && pLayout->m_TimeLimit == 0 )
		pLayout->m_TimeLimit = 2 * 60;

	// Update the time limit
	pLayout->UpdateTimeLimit();
}


/// Update the time limit
void GUILayoutModeOptions::UpdateTimeLimit()
{
	// If there is a time label then update it
	if( m_pTimeLimitLabel )
	{
		// Create the time string
		const uint32 TIME_BUF_SIZE = 16;
		wchar_t* szTimeStr[ TIME_BUF_SIZE ] = {0};
		swprintf( (wchar_t*)szTimeStr, TIME_BUF_SIZE, L"%u:00", m_TimeLimit / 60 );

		// Set the label text
		m_pTimeLimitLabel->SetText( (const wchar_t*)szTimeStr );
	}

	// Get if the time limit is enabled
	bool setVis = false;
	if( m_pTimeLimitCheckBox && m_pTimeLimitCheckBox->GetChecked() )
		setVis = true;
	
	// Set the visibility for the controls
	if( m_pTimeLimitLabel )
		m_pTimeLimitLabel->SetVisible( setVis );
	if( m_pTimeScrollUpBtn )
		m_pTimeScrollUpBtn->SetVisible( setVis );
	if( m_pTimeScrollDownBtn )
		m_pTimeScrollDownBtn->SetVisible( setVis );

	// Store the settings if possible
	if( m_pSettingsProfile )
		m_pSettingsProfile->SetSettings( m_GameType, setVis ? m_TimeLimit : 0 , m_GameDiff );

	// Update the game manager
	GameMgr::Get().SetTimeLimit( setVis ? m_TimeLimit : 0 );
}

/// Set the game type
void GUILayoutModeOptions::SetGameType( GameDefines::EGameType gameType )
{
	m_GameType = gameType;
}

/// Set the game difficulty
void GUILayoutModeOptions::SetGameDifficulty( GameDefines::EGameplayDiffLevel gameDiff )
{
	m_GameDiff = gameDiff;

	if( m_pDiffLabel )
		m_pDiffLabel->SetText( GameDefines::GetDifficultyDesc( m_GameDiff ) );

	// Store the settings if possible
	if( GameMgr::Get().GetActiveProfile() )
	{
		ProfileSubsetSettings* pSettingsProfile = (ProfileSubsetSettings*)GameMgr::Get().GetActiveProfile()->GetSubset( ProfileSubsetSettings::SUBSET_ID );
		pSettingsProfile->SetSettings( m_GameType, m_TimeLimit, m_GameDiff );
	}

	GameMgr::Get().SetGameplayDiff( m_GameDiff );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutModeOptions::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutModeOptions::OnActivate()
{
	GameGUILayout::OnActivate();

	// Find the title label
	GUICtrlLabel* pTitleLabel = (GUICtrlLabel*)GetCtrlByName( CTRLNAME_TitleLabel );
	if( pTitleLabel )
	{
		// Get the game type
		std::wstring sTitle;
		sTitle = GameDefines::GetGameTypeDesc( m_GameType );
		//TODO localize
		sTitle += L" Game Mode Options";
		pTitleLabel->SetText( sTitle.c_str() );

		// Center the text
		int32 centeredX = (g_pGraphicsMgr->GetDisplayDims().x - pTitleLabel->GetBoundBox().size.x) / 2;
		pTitleLabel->SetPos( Point2i( centeredX, pTitleLabel->GetPos().y ) );
	}

	// Check if there is an active profile
	if( GameMgr::Get().GetActiveProfile() )
	{
		// Store the profile settings object
		m_pSettingsProfile = (ProfileSubsetSettings*)GameMgr::Get().GetActiveProfile()->GetSubset( ProfileSubsetSettings::SUBSET_ID );

		// Get the settings
		ProfileSubsetSettings::GameModeSettings curModeSettings = m_pSettingsProfile->GetSettings( m_GameType );

		// Set the time limit and check the box if there is one
		m_TimeLimit = curModeSettings.timeLimit;
		if( curModeSettings.timeLimit > 0 && m_pTimeLimitCheckBox )
			m_pTimeLimitCheckBox->SetCheckState( true, true );
		else if( m_pTimeLimitCheckBox )
			m_pTimeLimitCheckBox->SetCheckState( false, true );

		// Set the game difficulty
		SetGameDifficulty( curModeSettings.diffLevel );
	}
	// Else no one is logged in
	else
	{
		// Clear the profile settings object
		m_pSettingsProfile = 0;

		// Addition attack has a time limit by default, but no other game modes do
		if( (m_GameType & GameDefines::GT_Mask_Type) == GameDefines::GT_AdditionAttack )
		{
			m_TimeLimit = 5*60;
			m_pTimeLimitCheckBox->SetCheckState( true, true );
		}
		else
		{
			m_TimeLimit = 0;
			m_pTimeLimitCheckBox->SetCheckState( false, true );
		}

		SetGameDifficulty( m_GameDiff );
	}

	UpdateTimeLimit();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutModeOptions::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.  It goes through
///	the controls in the layout and finds the desired controls by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutModeOptions::HookupCtrls()
{
	// Allow the base class to hook up the selected controls
	GameGUILayout::HookupCtrls();

	// Link up the main list
	GUIControl* pPlayCtrl = GetCtrlByName( CTRLNAME_BackButton );
	if( pPlayCtrl )
	{
		GUICtrlButton* pBackButton = (GUICtrlButton*)pPlayCtrl;
		pBackButton->SetClickCallback( BackButtonCB );
	}

	pPlayCtrl = GetCtrlByName( CTRLNAME_PlayButton );
	if( pPlayCtrl )
	{
		GUICtrlButton* pPlayButton = (GUICtrlButton*)pPlayCtrl;
		pPlayButton->SetClickCallback( PlayButtonCB );
	}


	// The difficulty
	pPlayCtrl = GetCtrlByName( CTRLNAME_DiffScrollUpBtn );
	if( pPlayCtrl )
	{
		GUICtrlButton* pPlayButton = (GUICtrlButton*)pPlayCtrl;
		pPlayButton->SetClickCallback( DiffScrollUpCB );
	}

	pPlayCtrl = GetCtrlByName( CTRLNAME_DiffScrollDownBtn );
	if( pPlayCtrl )
	{
		GUICtrlButton* pPlayButton = (GUICtrlButton*)pPlayCtrl;
		pPlayButton->SetClickCallback( DiffScrollDownCB );
	}


	// The time limit
	if( m_pTimeScrollUpBtn )
		m_pTimeScrollUpBtn->SetClickCallback( TimeScrollUpCB );

	if( m_pTimeScrollDownBtn )
		m_pTimeScrollDownBtn->SetClickCallback( TimeScrollDownCB );

	if( m_pTimeLimitCheckBox )
		m_pTimeLimitCheckBox->SetCallback( TimeLimitCheckCB );

	UpdateTimeLimit();
}

/// Set the level
void GUILayoutInGame::SetScore( uint32 score )
{
	if( m_pLabels[L_Score] )
		m_pLabels[L_Score]->SetText( TCBase::EasyUIToA( score ) );
}

/// Set the score
void GUILayoutInGame::SetLevel( int32 level )
{
	if( m_pLabels[L_Level] )
		m_pLabels[L_Level]->SetText( TCBase::EasyIToA( level ) );
}

/// Set the count
void GUILayoutInGame::SetCount( int32 count )
{
	if( m_pLabels[L_Count] )
		m_pLabels[L_Count]->SetText( TCBase::EasyIToA( count ) );
}

/// Set the time played
void GUILayoutInGame::SetTimePlayed( uint32 secsElapsed )
{
	if( !m_pLabels[L_Time] )
		return;

	const uint32 TIME_BUF_SIZE = 16;
	static wchar_t* szTimeStr[ TIME_BUF_SIZE ];

	swprintf( (wchar_t*)szTimeStr, TIME_BUF_SIZE, L"%u:%02u", secsElapsed / 60, secsElapsed % 60 );
	
	m_pLabels[L_Time]->SetText( (wchar_t*)szTimeStr );
}

/// Set the score multiplier
void GUILayoutInGame::SetComboCount( uint32 comboCount )
{
	if( !m_pLabels[L_ComboCountVal] )
		return;

	if( comboCount == 0 )
	{
		if( m_pComboTextSpr )
			m_pComboTextSpr->SetVisible( false );
		if( m_pLabels[L_ComboCountVal] )
			m_pLabels[L_ComboCountVal]->SetVisible( false );
	}
	else
	{
		// Update the text and make it visible
		if( m_pComboTextSpr )
			m_pComboTextSpr->SetVisible( true );
		if( m_pLabels[L_ComboCountVal] )
		{
			// Get the count text
			const uint32 BUF_SIZE = 4;
			wchar_t szBuffer[BUF_SIZE] = {0};
			if( comboCount > 99 )
				swprintf( szBuffer, BUF_SIZE, L"??" );
			else
				swprintf( szBuffer, BUF_SIZE, L"%02d", comboCount );

			// Set the text
			m_pLabels[L_ComboCountVal]->SetText( szBuffer );
			m_pLabels[L_ComboCountVal]->SetVisible( true );
		}
	}
}

/// Update the layout based on the game type
void GUILayoutInGame::InitFromGameType( GameDefines::EGameType gameType )
{
	// Hide all of the labels
	for( uint32 labelIndex = 0; labelIndex < L_COUNT; ++labelIndex )
	{
		if( m_pLabels[labelIndex] )
			m_pLabels[labelIndex]->SetVisible( false );
	}
	if( m_pComboTextSpr )
		m_pComboTextSpr->SetVisible( false );

	bool shouldHideSpeedButton = false;

	// If the game is in tutorial mode
	bool isTutorial = (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial;
	isTutorial |= (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial2;
	if( isTutorial )
	{
		// If this is the combo tutorial
		if( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime
			&& (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial2 )
		{
			// Enable the combo block text
			if( m_pLabels[L_ComboBlock] )
				m_pLabels[L_ComboBlock]->SetVisible( true );
		}
		// Or if this is addition mode
		else if( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_AdditionAttack )
		{
			// Show the sum label
			if( m_pLabels[L_SumLabel] )
				m_pLabels[L_SumLabel]->SetVisible( true );
		}
		// Or if this is the basic gameplay tutorial
		else if( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime
			&& (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Tutorial )
		{
			// Hide speed up button
			shouldHideSpeedButton = true;
		}

		// Bail out
		return;
	}

	// Update the count text
	if( m_pLabels[L_Count] )
	{
		m_pLabels[L_Count]->SetVisible( true );
		m_pLabels[L_Count]->SetText( L"0" );
	}

	// Update the score text
	if( m_pLabels[L_Score] )
	{
		m_pLabels[L_Score]->SetVisible( true );
		m_pLabels[L_Score]->SetText( L"0" );
	}
	if( m_pLabels[L_ScoreName] )
		m_pLabels[L_ScoreName]->SetVisible( true );

	// Update the time text
	if( m_pLabels[L_Time] )
		m_pLabels[L_Time]->SetVisible( true );

	// If this is addition mode
	if( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_AdditionAttack )
	{
		//TODO localize
		// Show the equation count
		SetCountLabelText( L"Sums Cleared" );

		// Show the sum label
		if( m_pLabels[L_SumLabel] )
			m_pLabels[L_SumLabel]->SetVisible( true );

		// Hide speed up button
		shouldHideSpeedButton = true;
	}
	// Else if this is multiplication
	else if( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime
			|| (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_Ceiling )
	{
		// Show the combo block text if this is prime time mode
		if( m_pLabels[L_ComboBlock] )
			m_pLabels[L_ComboBlock]->SetVisible( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_PrimeTime );

		//TODO localize
		// Show the equation count
		SetCountLabelText( L"Products Cleared" );

		// Show the level number
		if( m_pLabels[L_Level] )
		{
			m_pLabels[L_Level]->SetVisible( true );
			m_pLabels[L_Level]->SetText( L"1" );
		}

		// Show the level label
		if( m_pLabels[L_LevelName] )
			m_pLabels[L_LevelName]->SetVisible( true );

		// If this is practice mode
		if( (gameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_Practice )
		{
			// Hide the level label
			if( m_pLabels[L_LevelName] )
				m_pLabels[L_LevelName]->SetVisible( false );

			// Hide the level number
			if( m_pLabels[L_Level] )
				m_pLabels[L_Level]->SetVisible( false );
		}
	}
	// Else if this is multiplication
	else if( (gameType & GameDefines::GT_Mask_Type) == GameDefines::GT_Fractions )
	{
		//TODO localize
		SetCountLabelText( L"Fractions Cleared" );

		// Show the level label
		if( m_pLabels[L_LevelName] )
			m_pLabels[L_LevelName]->SetVisible( true );
		SetLabelText( L_Level, L"1" );
	}

	// Hide speed up button
	if( m_pSpeedUpButton )
		m_pSpeedUpButton->SetVisible( !shouldHideSpeedButton );
}

/// Set the product
void GUILayoutInGame::SetProduct( int32 product )
{
	if( product == 0 )
	{
		// Hide current product labels
		if( m_pLabels[L_CurrentProd] )
			m_pLabels[L_CurrentProd]->SetVisible( false );
		if( m_pLabels[L_CurrentProdName] )
			m_pLabels[L_CurrentProdName]->SetVisible( false );
	}
	else
	{
		// Show the product labels and set the product
		if( m_pLabels[L_CurrentProd] )
		{
			m_pLabels[L_CurrentProd]->SetText( TCBase::EasyIToA( product ) );
			m_pLabels[L_CurrentProd]->SetVisible( true );
		}
		if( m_pLabels[L_CurrentProdName] )
			m_pLabels[L_CurrentProdName]->SetVisible( true );
	}
}

/// Set the tutorial note
void GUILayoutInGame::SetNoteText( const wchar_t* szNote )
{
	if( !m_pLabels[L_TutorialNote] || !m_pLabels[L_TutorialNote]->GetFont() )
		return;

	// Get the left position of the field
	int fieldLeft = 389;
	if( GameMgr::Get().GetLogic() )
	{
		if( GameMgr::Get().GetLogic()->GetGameField() )
			fieldLeft = GameMgr::Get().GetLogic()->GetGameField()->GetFieldRect().pos.x;
	}

	// Get the text bounding rectangle
	Box2i textBox;
	textBox.pos = m_pLabels[L_TutorialNote]->GetPos();
	textBox.size.x = (fieldLeft - textBox.pos.x) - 10;
	textBox.size.y = g_pGraphicsMgr->GetDisplayDims().y - textBox.pos.y;
	if( m_pLabels[L_FieldBoxPos] )
		textBox.size.x = m_pLabels[L_FieldBoxPos]->GetPos().x - textBox.pos.x;

	// Wrap the text
	std::wstring sWrapText = m_pLabels[L_TutorialNote]->GetFont()->GetWordWrapString( szNote, textBox );
	
	// Set the text
	m_pLabels[L_TutorialNote]->SetText( sWrapText.c_str() );
	
	// Set the visibility based on if there is text or not
	m_pLabels[L_TutorialNote]->SetVisible( szNote != 0 && wcslen( szNote ) > 0 );
}

/// Set the count label text
void GUILayoutInGame::SetCountLabelText( const wchar_t* szLabelText )
{
	SetLabelText( L_CountName, szLabelText );
}

/// Set the count text
void GUILayoutInGame::SetCountText( const wchar_t* szText )
{
	SetLabelText( L_Count, szText );
}

/// Set the text of a label
void GUILayoutInGame::SetLabelText( ELabels labelId, const wchar_t* szText )
{
	if( m_pLabels[labelId] )
	{
		m_pLabels[labelId]->SetVisible( szText != 0 && wcslen(szText) > 0 );
		
		if( m_pLabels[labelId]->IsVisible() )
			m_pLabels[labelId]->SetText( szText );
	}
}

/// Get the text of a label
std::wstring GUILayoutInGame::GetLabelText( ELabels labelId ) const
{
	if( m_pLabels[labelId] )
		return m_pLabels[labelId]->GetTextStr();

	return std::wstring();
}

/// Get the bound box for a label so we can justify other graphics off of a label position
Box2i GUILayoutInGame::GetLabelBoundBox( ELabels labelId )
{
	if( m_pLabels[labelId] )
		return m_pLabels[labelId]->GetBoundBox();

	return Box2i(0,0,0,0);
}

// Get the sum label bottom left position
Point2i GUILayoutInGame::GetSumBottomLeftPos() const
{
	if( !m_pLabels[L_SumLabel] )
		return Point2i();

	Point2i retPos = m_pLabels[L_SumLabel]->GetPos();
	retPos.y += m_pLabels[L_SumLabel]->GetBoundBox().size.y;

	// Return the bottom left
	return retPos;
}


static uint32 ModPixelCB( uint32 inColor )
{
	uint32 r = inColor & 0xFF;
	uint32 g = (inColor >> 8) & 0xFF;
	uint32 b = (inColor >> 16) & 0xFF;

	r /= 2;
	g /= 2;
	b /= 2;

	return (b << 16) | (g << 8) | r;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutInGame::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutInGame::Update( float32 frameTime )
{
	// Update the base class
	GameGUILayout::Update( frameTime );

	// If the field background is being darkened
    /*
	const int32 FIELD_MIN_Y = 61;
	const int32 FIELD_MAX_Y = FIELD_MIN_Y + GameField::FIELD_HEIGHT * 55;
	if( m_FieldDarkenYPos >= FIELD_MIN_Y && m_FieldDarkenYPos < FIELD_MAX_Y )
	{
		// Get the background image
		GUICtrlSprite* pSpr = (GUICtrlSprite*)GetCtrlByName( GUILayout::BG_CTRL_NAME );
		if( pSpr )
		{
			TCImage* pImg = ResourceMgr::Get().GetTCImage( pSpr->m_Sprite.GetSprite()->GetImgResID() ).GetObj();

			// Darken one row of pixels
			g_pGraphicsMgr->ApplyEffect( pImg, Box2i(389, m_FieldDarkenYPos, GameField::FIELD_WIDTH * 55, m_FieldDarkenYDelta), ModPixelCB );
			m_FieldDarkenYPos += m_FieldDarkenYDelta;
		}
	}
    */
	// If any notes are flashing
	UpdateFlashing( frameTime );
}


/// Update the label flashing
void GUILayoutInGame::UpdateFlashing( float32 frameTime )
{
	const float32 MAX_FLASH_TIME = 1.625f;
	const float32 FLASH_SPEED = 0.125f;

	// Update flashing
	std::list<int32> timersToRemove;
	for( std::map< int32, float32 >::iterator iterFlasher = _flashTimeMap.begin(); iterFlasher != _flashTimeMap.end(); ++iterFlasher )
	{
		// Update the timer
		iterFlasher->second += frameTime;

		// If this timer is expired then mark it for removal
		GUICtrlLabel* pLabel = m_pLabels[iterFlasher->first];
		ResourceID noteFontResID = GetFontResID();
		if( !pLabel || iterFlasher->second > MAX_FLASH_TIME )
			timersToRemove.push_back( iterFlasher->first );
		// Otherwise flash the timer
		else
		{
			noteFontResID = RESID_FONT_LETTEROMATIC;
			if(pLabel->GetFont())
			{
				// If the note should toggle fonts
				noteFontResID = pLabel->GetFont()->GetResID();
				if( (int32)(iterFlasher->second / FLASH_SPEED) != (int32)((iterFlasher->second - frameTime) / FLASH_SPEED) )
				{
					if( noteFontResID == RESID_FONT_LETTEROMATIC )
						noteFontResID = RESID_FONT_BLACK_LETTEROMATIC;
					else
						noteFontResID = RESID_FONT_LETTEROMATIC;				
				}
			}
		}

		// Set the font
		pLabel->SetFont( ResourceMgr::Get().GetTCFont( noteFontResID ) );
	}

	// Remove expired timers
	for( std::list< int32 >::iterator iterFlasher = timersToRemove.begin(); iterFlasher != timersToRemove.end(); ++iterFlasher )
		_flashTimeMap.erase( *iterFlasher );

	// If the note is flashing
	if( m_TutorialNoteFlashTime >= 0.0f && m_TutorialNoteFlashTime < MAX_FLASH_TIME )
	{
		ResourceID noteFontResID = RESID_FONT_LETTEROMATIC;
		if(m_pLabels[L_TutorialNote] && m_pLabels[L_TutorialNote]->GetFont())
		{
			const float32 FLASH_SPEED = 0.125f;

			// If the note should toggle fonts
			noteFontResID = m_pLabels[L_TutorialNote]->GetFont()->GetResID();
			if( (int32)(m_TutorialNoteFlashTime / FLASH_SPEED) != (int32)((m_TutorialNoteFlashTime + frameTime) / FLASH_SPEED) )
			{
				if( noteFontResID == RESID_FONT_LETTEROMATIC )
					noteFontResID = RESID_FONT_BLACK_LETTEROMATIC;
				else
					noteFontResID = RESID_FONT_LETTEROMATIC;				
			}
		}

		// Update the time
		m_TutorialNoteFlashTime += frameTime;

		// If the time has expired reset the font back to the normal layout font
		if( m_TutorialNoteFlashTime >= MAX_FLASH_TIME )
			noteFontResID = GetFontResID();

		// Set the font
		m_pLabels[L_TutorialNote]->SetFont( ResourceMgr::Get().GetTCFont( noteFontResID ) );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutInGame::OnGraphicsRefresh  Public
///
/// Reset the needed data if the graphics system has been reset
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutInGame::OnGraphicsRefresh()
{
	GameGUILayout::OnGraphicsRefresh();

	m_FieldDarkenYPos = 61;
	m_FieldDarkenYDelta = 2;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutInGame::GetFontResID  Private
///
/// Get the font used to render text based on the background image. This prevents white text on
///	white backgrounds.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
ResourceID GUILayoutInGame::GetFontResID() const
{
	GUICtrlSprite* pBGSprite = (GUICtrlSprite*)GetCtrlByName( GUILayout::BG_CTRL_NAME );
	if( pBGSprite && pBGSprite->m_Sprite.GetSprite() )
	{
		ResourceID bgImgResID = pBGSprite->m_Sprite.GetSprite()->GetResID();

		// Set the font so it can be read on a white background
		if( bgImgResID == RESID_SPRITE_SNOW_BG )
			return RESID_FONT_BLACK_LETTEROMATIC;
	}

	// Default to the normal white font
	return RESID_FONT_LETTEROMATIC;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  LayoutCB_BackToTitle  Static Global
///
///	\param UNNAMED The button that sent this message
///
///	A message handler called when the user presses the back button to go back to the title screen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void LayoutCB_InGameMenuButton( GUICtrlButton* pSenderButton )
{
	const uint32 KEYCODE_ESCAPE = 27;

	GUILayoutInGame* pGameLayout = (GUILayoutInGame*)pSenderButton->GetContainingLayout();
	GameMgrCtrl* pGameCtrl = (GameMgrCtrl*)pGameLayout->GetCtrlByName( GameMgrCtrl::CTRL_NAME );
	if( pGameCtrl )
		pGameCtrl->OnKeyDown(KEYCODE_ESCAPE);
}


void GUILayoutInGame::OnSpeedButtonDown(GUICtrlButton* pButton)
{
	GUILayoutInGame* pGameLayout = (GUILayoutInGame*)pButton->GetContainingLayout();

	GameMgrCtrl* pGameCtrl = (GameMgrCtrl*)pGameLayout->GetCtrlByName( GameMgrCtrl::CTRL_NAME );

	pGameCtrl->OnKeyDown( GameDefines::KEY_CODE_FASTPUSH );
}

void GUILayoutInGame::OnSpeedButtonUp(GUICtrlButton* pButton)
{
	GUILayoutInGame* pGameLayout = (GUILayoutInGame*)pButton->GetContainingLayout();

	GameMgrCtrl* pGameCtrl = (GameMgrCtrl*)pGameLayout->GetCtrlByName( GameMgrCtrl::CTRL_NAME );

	pGameCtrl->OnKeyUp( GameDefines::KEY_CODE_FASTPUSH );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutInGame::OnActivate  Public
///
///	A message handler called when the layout is activated.  This will populate the list of
///	profiles.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutInGame::OnActivate()
{
	GameGUILayout::OnActivate();

	// Get the background sprite
	GUICtrlSprite* pBGSprite = (GUICtrlSprite*)GetCtrlByName( GUILayout::BG_CTRL_NAME );
	if( pBGSprite )
	{
		// Set the background with a random image
		ResourceID bgImgResID = (rand() % 2 == 0) ? RESID_SPRITE_DESERT_BG_SPRITE : RESID_SPRITE_SNOW_BG;
		pBGSprite->m_Sprite = ResourceMgr::Get().GetRefSprite(bgImgResID);
		
		// Set the font so it can be read on the background (White on black, visa versa, etc.)
		TCFontHndl noteFont = ResourceMgr::Get().GetTCFont( GetFontResID() );

		// Update all of the label fonts
		for( ControlList::iterator iterCtrl = m_Controls.begin(); iterCtrl != m_Controls.end(); ++iterCtrl )
		{
			// Ignore the combo label
			if( (*iterCtrl)->GetName() == CTRLNAME_ComboCountVal )
				continue;

			if( (*iterCtrl)->GetCtrlType() == GUIDefines::CT_Label )
			{
				GUICtrlLabel* pNoteLabel = (GUICtrlLabel*)*iterCtrl;
				if( pNoteLabel )
					pNoteLabel->SetFont( noteFont );
			}
		}
	}

	if( m_pSpeedUpButton )
	{
		m_pSpeedUpButton->SetButtonDownCallback( OnSpeedButtonDown );
		m_pSpeedUpButton->SetButtonUpCallback( OnSpeedButtonUp );
	}

	// Start the darkening
	const int32 FIELD_MIN_Y = 61;
	//const int32 FIELD_MAX_Y = FIELD_MIN_Y + GameField::FIELD_HEIGHT * 55;
	m_FieldDarkenYPos = FIELD_MIN_Y;
	m_FieldDarkenYDelta = 10;

	// Create the game manager control and add it to the layout, on top of other controls
	GameMgrCtrl* pGameCtrl = new GameMgrCtrl();
	AddControl( pGameCtrl );

	// Bring all buttons to the top so they can be clicked on
	GUILayout::ControlList buttonControls = GetAllControlsOfType(GUIDefines::CT_Button);
	for( GUILayout::ControlList::iterator iterButton = buttonControls.begin(); iterButton != buttonControls.end(); ++iterButton )
		BringCtrlToFront(*iterButton);

	if( m_pMenuButton != NULL )
		m_pMenuButton->SetClickCallback(LayoutCB_InGameMenuButton);

	// Focus on the game
	SetFocusCtrl( pGameCtrl );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutInGame::OnDeactivate  Public
///
///	A message handler called when the layout is deactivated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutInGame::OnDeactivate()
{
	GUIControl* pGameCtrl = GetCtrlByName( GameMgrCtrl::CTRL_NAME );
	if( pGameCtrl )
	{
		RemoveControl( pGameCtrl );
		delete pGameCtrl;
	}

	// Reset the darken row index
	m_FieldDarkenYPos = -1;
}


void GUILayoutProfileSelection::OnProfileListItemSelect( GUICtrlScrollList* pScrollList )
{
	GUILayoutProfileSelection* pProfileLayout = static_cast<GUILayoutProfileSelection*>( pScrollList->GetContainingLayout() );
	if( !pProfileLayout )
		return;

	// Show the rename/delete buttons if a proifle is selected
	bool showProfileButtons = pScrollList->GetSelItemIndex() != -1;

	// Hide the buttons if the selected button is the logged-in user
	if( showProfileButtons && GameMgr::Get().GetActiveProfile() )
		showProfileButtons = GameMgr::Get().GetActiveProfile()->GetName() != pScrollList->GetItemString( pScrollList->GetSelItemIndex() );

	if( pProfileLayout->_pRenameProfileButton )
		pProfileLayout->_pRenameProfileButton->SetVisible( showProfileButtons );

	if( pProfileLayout->_pDeleteProfileButton )
		pProfileLayout->_pDeleteProfileButton->SetVisible( showProfileButtons );

	std::wstring sSelectedProfileName = pScrollList->GetItemString(pScrollList->GetSelItemIndex());
	if( pProfileLayout->_pKeepLoggedInCheckBox )
		pProfileLayout->_pKeepLoggedInCheckBox->SetCheckState( sSelectedProfileName == ApplicationBase::Get()->Settings.AutoLoginProfile );
}

/// An event handler invoked when the user presses the button to rename a profile
void GUILayoutProfileSelection::OnRenameProfileButtonClicked( GUICtrlButton* pButton )
{
	GUILayoutProfileSelection* pProfileLayout = static_cast<GUILayoutProfileSelection*>( pButton->GetContainingLayout() );
	if( !pProfileLayout )
		return;

	// Create the message box to warn the player of a bad name
	pProfileLayout->m_pRenameMsgBox = MsgBox::CreateMsgBox( MSG_EnterRenameProfileName, MsgBox::BL_TextEntry );
	GUIMgr::Get().PopupMessageBox( pProfileLayout->m_pRenameMsgBox );	
}

/// An event handler invoked when the user presses the button to delete a profile
void GUILayoutProfileSelection::OnDeleteProfileButtonClicked( GUICtrlButton* pButton )
{
	GUILayoutProfileSelection* pProfileLayout = static_cast<GUILayoutProfileSelection*>( pButton->GetContainingLayout() );
	if( !pProfileLayout )
		return;

	// Get the selected profile
	int32 selProfileIndex = pProfileLayout->m_pProfileList->GetSelItemIndex();
	if( selProfileIndex == -1 )
		return;
	std::wstring sProfileName = pProfileLayout->m_pProfileList->GetItemString(selProfileIndex);

	//TODO localize
	// Create the message box to warn the player of a bad name
	std::wstring sDeleteMessage(L"Are you sure you want to delete " + sProfileName + L"?");
	pProfileLayout->m_pDeleteProfileMsgBox = MsgBox::CreateMsgBox( sDeleteMessage.c_str(), MsgBox::BL_YesNo );
	GUIMgr::Get().PopupMessageBox( pProfileLayout->m_pDeleteProfileMsgBox );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutProfileSelection::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.  It goes through
///	the controls in the layout and finds the desired controls by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutProfileSelection::HookupCtrls()
{
	// Allow the base class to hook up the selected controls
	GameGUILayout::HookupCtrls();

	// Link up the back button
	GUIControl* pPlayCtrl = GetCtrlByName( CTRLNAME_BackButton );
	if( pPlayCtrl )
	{
		GUICtrlButton* pBackButton = (GUICtrlButton*)pPlayCtrl;
		pBackButton->SetClickCallback( LayoutCB_BackToTitle );
	}

	// Link up create profile button
	pPlayCtrl = GetCtrlByName( CTRLNAME_CreateProfButton );
	if( pPlayCtrl )
	{
		GUICtrlButton* pCreateProfButton = (GUICtrlButton*)pPlayCtrl;
		pCreateProfButton->SetClickCallback( CreateBtnCallback );
	}

	// Link up the select profile button
	pPlayCtrl = GetCtrlByName( CTRLNAME_SelectProfButton );
	if( pPlayCtrl )
	{
		GUICtrlButton* pSelProfButton = (GUICtrlButton*)pPlayCtrl;
		pSelProfButton->SetClickCallback( SelectProfBtnCallback );
	}

	if( m_pProfileList )
		m_pProfileList->SetItemSelectCallback( OnProfileListItemSelect );
	
	// Link up the scroll buttons
	GUICtrlButton* pScrollProfUpBtn = (GUICtrlButton*)GetCtrlByName( CTRLNAME_ScrollProfUp );
	GUICtrlButton* pScrollProfDownBtn = (GUICtrlButton*)GetCtrlByName( CTRLNAME_ScrollProfDown );
	if( m_pProfileList )
		m_pProfileList->SetScrollButtons( pScrollProfUpBtn, pScrollProfDownBtn );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutProfileSelection::OnActivate  Public
///
///	A message handler called when the layout is activated.  This will populate the list of
///	profiles.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutProfileSelection::OnActivate()
{
	GameGUILayout::OnActivate();

	PopulateProfileList();

	// If a player is logged-in then display the logout button
	GUICtrlButton* pLogoutBtn = (GUICtrlButton*)GetCtrlByName( CTRLNAME_LogoutButton );
	if( pLogoutBtn )
	{
		pLogoutBtn->SetVisible( GameMgr::Get().GetActiveProfile() != 0 );
		pLogoutBtn->SetClickCallback( SelectLogoutBtnCallback );
	}

	if( _pRenameProfileButton )
	{
		_pRenameProfileButton->SetClickCallback( OnRenameProfileButtonClicked );
		_pRenameProfileButton->SetVisible( false );
	}

	if( _pDeleteProfileButton )
	{
		_pDeleteProfileButton->SetClickCallback( OnDeleteProfileButtonClicked );
		_pDeleteProfileButton->SetVisible( false );
	}

	if( GameMgr::Get().GetActiveProfile() != 0 )
	{
		int32 profileIndex = m_pProfileList->IndexOfString( GameMgr::Get().GetActiveProfile()->GetName().c_str() );
		if( profileIndex >= 0 )
			m_pProfileList->SetSelItemIndex( profileIndex );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutProfileSelection::CreateBtnCallback  Static Private
///
///	A callback function for when the user presses the create button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutProfileSelection::CreateBtnCallback( GUICtrlButton* pBtn )
{
	if( !pBtn || !pBtn->GetContainingLayout() || pBtn->GetContainingLayout()->GetLayoutID() != GameDefines::MS_ProfileSelection )
		return;

	// Create a message box to get a new user name
	GUILayoutProfileSelection* pProfLayout = reinterpret_cast<GUILayoutProfileSelection*>( pBtn->GetContainingLayout() );
	pProfLayout->m_pCreateProfMsgBox = MsgBox::CreateMsgBox( GUILayoutProfileSelection::MSG_EnterNewProfileName, MsgBox::BL_TextEntry );
	GUICtrlTextBox* pTextBox = pProfLayout->m_pCreateProfMsgBox->GetTextBoxCtrl();
	if( pTextBox )
		pTextBox->SetMaxAllowedLength( 14 );

	// Display the message box
	GUIMgr::Get().PopupMessageBox( pProfLayout->m_pCreateProfMsgBox );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutProfileSelection::SelectProfBtnCallback  Static Private
///
///	The callback function for when the user selects a profile
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutProfileSelection::SelectProfBtnCallback( GUICtrlButton* pBtn )
{
	if( !pBtn || !pBtn->GetContainingLayout() || pBtn->GetContainingLayout()->GetLayoutID() != GameDefines::MS_ProfileSelection )
		return;
	GUILayoutProfileSelection* pProfLayout = reinterpret_cast<GUILayoutProfileSelection*>( pBtn->GetContainingLayout() );
	
	int32 selIndex = pProfLayout->m_pProfileList->GetSelItemIndex();
	if( selIndex < 0 )
		return;

	// Get the selected profile name
	std::wstring sProfName = pProfLayout->m_pProfileList->GetItemString( (uint32)selIndex );
	PlayerProfile* pProfile = PlayerProfile::OpenProfile( sProfName.c_str() );

	// Update the auto-login setting
	if( pProfLayout->_pKeepLoggedInCheckBox && pProfLayout->_pKeepLoggedInCheckBox->GetChecked() )
		ApplicationBase::Get()->SetAutoLoginProfile( sProfName.c_str() );
	else
		ApplicationBase::Get()->SetAutoLoginProfile( L"" );

	// Use the profile and go back to the title screen
	GameMgr::Get().SetActiveProfile( pProfile );
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_TitleScreen );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutProfileSelection::SelectLogoutBtnCallback  Static Private
///
/// The callback function for when the user presses the logout button
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutProfileSelection::SelectLogoutBtnCallback( GUICtrlButton* )
{
	// Clear the profile and go back to the title screen
	GameMgr::Get().SetActiveProfile( NULL );
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_TitleScreen );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutProfileSelection::OnDeactivate  Public
///
///	A message handler called when the layout is deactivated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutProfileSelection::OnDeactivate()
{
	// Free the message boxes
	const int32 NumMsgBoxes = 6;
	MsgBox** ppMsgBoxes[NumMsgBoxes] = { &m_pCreateProfMsgBox,
										&m_pBadNameMsgBox,
										&m_pRenameMsgBox,
										&m_pDeleteProfileMsgBox,
										&m_pTryTutorialsMsgBox,
										&m_pCantCompleteMsgBox };
	for( int32 curMsgBoxIndex = 0; curMsgBoxIndex < NumMsgBoxes; ++curMsgBoxIndex )
	{
		if( *ppMsgBoxes[curMsgBoxIndex] )
		{
			GUIMgr::Get().CloseMessageBox( *ppMsgBoxes[curMsgBoxIndex] );
			delete *ppMsgBoxes[curMsgBoxIndex];
			*ppMsgBoxes[curMsgBoxIndex] = 0;
		}
	}
}

//TODO localize
const wchar_t* GUILayoutProfileSelection::MSG_EnterNewProfileName = L"Enter Your Desired User Name";
const wchar_t* GUILayoutProfileSelection::MSG_EnterRenameProfileName = L"Enter The New Profile Name";

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutProfileSelection::Update  Public
///
///	\param UNNAMED The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutProfileSelection::Update( float32 frameTime )
{
	//TODO localize
	const wchar_t* INVALID_PROFILE_NAME_MESSAGE = L"The player name you entered is invalid.  It must be at least one character long, not start with a space, and be no longer than 63 characters.";

	// Update the base class
	GameGUILayout::Update( frameTime );

	// If there is a message box for receiving a player's name
	if( m_pCreateProfMsgBox )
	{
		// If the user hasn't closed the box yet
		if( m_pCreateProfMsgBox->GetRetVal() == MsgBox::MBR_Error )
			return;

		// Get the entered player name and return value
		std::wstring sEnteredName = m_pCreateProfMsgBox->GetEnteredText();
		MsgBox::EMsgBoxReturn msgBoxReturn = m_pCreateProfMsgBox->GetRetVal();

		// Free the message box
		GUIMgr::Get().CloseMessageBox( m_pCreateProfMsgBox );
		delete m_pCreateProfMsgBox;
		m_pCreateProfMsgBox = 0;

		// If the user pressed OK
		if( msgBoxReturn == MsgBox::MBR_OK )
		{
			// Ensure a valid player name
			if( !PlayerProfile::IsValidProfileName( sEnteredName.c_str() ) )
			{
				// Create the message box to warn the player of a bad name
				m_BadNameIsOpenForNew = true;
				m_pBadNameMsgBox = MsgBox::CreateMsgBox( INVALID_PROFILE_NAME_MESSAGE );
				GUIMgr::Get().PopupMessageBox( m_pBadNameMsgBox );
			}
			// Else create the new profile
			else
			{
				// Create a new profile
				PlayerProfile* pProfile = PlayerProfile::OpenProfile( sEnteredName.c_str() );
				GameMgr::Get().SetActiveProfile( pProfile );

				// Go back to the title screen
				GUIMgr::Get().SetActiveLayout( GameDefines::MS_TitleScreen );
			}
		}
	}
	// Else if there is a message box to tell the player that the name is invalid
	else if( m_pBadNameMsgBox )
	{
		// If the user hasn't closed the box yet
		if( m_pBadNameMsgBox->GetRetVal() == MsgBox::MBR_Error )
			return;

		// Free the message box
		GUIMgr::Get().CloseMessageBox( m_pBadNameMsgBox );
		delete m_pBadNameMsgBox;
		m_pBadNameMsgBox = 0;

		// Display the name message box again
		if( m_BadNameIsOpenForNew )
		{
			m_pCreateProfMsgBox = MsgBox::CreateMsgBox( MSG_EnterNewProfileName, MsgBox::BL_TextEntry );
			GUIMgr::Get().PopupMessageBox( m_pCreateProfMsgBox );
		}
		else
		{
			m_pRenameMsgBox = MsgBox::CreateMsgBox( MSG_EnterRenameProfileName, MsgBox::BL_TextEntry );
			GUIMgr::Get().PopupMessageBox( m_pRenameMsgBox );
		}
	}
	// Else if there is a message box asking the player to try the tutorials
	else if( m_pTryTutorialsMsgBox )
	{
		// If the user hasn't closed the box yet
		if( m_pTryTutorialsMsgBox->GetRetVal() == MsgBox::MBR_Error )
			return;

		// Get the return value
		MsgBox::EMsgBoxReturn retVal = m_pTryTutorialsMsgBox->GetRetVal();

		// Free the message box
		GUIMgr::Get().CloseMessageBox( m_pTryTutorialsMsgBox );
		delete m_pTryTutorialsMsgBox;
		m_pTryTutorialsMsgBox = 0;

		// If the player wants to try the tutorials
		if( retVal == MsgBox::MBR_Yes )
		{
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
			GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_PrimeTime | GameDefines::GT_Mod_Tutorial) );
		}
		// Else go back to the title screen
		else
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_TitleScreen );
	}
	// Or if the user is being asked to confirm a profile deletion
	else if( m_pDeleteProfileMsgBox )
	{
		// If the user hasn't closed the box yet
		if( m_pDeleteProfileMsgBox->GetRetVal() == MsgBox::MBR_Error )
			return;

		// Get the return value
		MsgBox::EMsgBoxReturn retVal = m_pDeleteProfileMsgBox->GetRetVal();

		// Free the message box
		GUIMgr::Get().CloseMessageBox( m_pDeleteProfileMsgBox );
		delete m_pDeleteProfileMsgBox;
		m_pDeleteProfileMsgBox = 0;

		// If the player wants to delete the profile
		if( retVal == MsgBox::MBR_Yes )
		{
			std::wstring sSelectedProfileName = m_pProfileList->GetItemString( m_pProfileList->GetSelItemIndex() );

			// If we are deleting the active user then log them out first
			if( GameMgr::Get().GetActiveProfile() && GameMgr::Get().GetActiveProfile()->GetName() == sSelectedProfileName )
			{
				GameMgr::Get().SetActiveProfile( NULL );
				GUICtrlButton* pLogOutButton = static_cast<GUICtrlButton*>( GetCtrlByName(CTRLNAME_LogoutButton) );
				if( pLogOutButton )
					pLogOutButton->SetVisible( false );
			}

			if( sSelectedProfileName.length() > 0 )
			{
				std::wstring sProfileFile = PlayerProfile::GetFilePathFromName( sSelectedProfileName.c_str() );

				// If delete failed
				if( !TCBase::TCDeleteFile( sProfileFile.c_str() ) )
				{
					//TODO localize
					// Display the error message box
					m_pCantCompleteMsgBox = MsgBox::CreateMsgBox( L"The profile could not be deleted. You will have to delete the profile from Windows instead. Please visit our website for help.", MsgBox::BL_OK );
					GUIMgr::Get().PopupMessageBox( m_pCantCompleteMsgBox );				
				}
				else
				{
					PopulateProfileList();

					// Clear the selection to ensure the rename/delete buttons are hidden
					m_pProfileList->SetSelItemIndex( -1 );
				}
			}
		}
	}
	// Or if the player is renaming a profile
	else if( m_pRenameMsgBox )
	{
		// If the user hasn't closed the box yet
		if( m_pRenameMsgBox->GetRetVal() == MsgBox::MBR_Error )
			return;

		// Get the return value
		MsgBox::EMsgBoxReturn retVal = m_pRenameMsgBox->GetRetVal();

		std::wstring sNewProfileName = m_pRenameMsgBox->GetEnteredText();

		// Free the message box
		GUIMgr::Get().CloseMessageBox( m_pRenameMsgBox );
		delete m_pRenameMsgBox;
		m_pRenameMsgBox = 0;

		// If the player wants to rename the profile
		if( retVal == MsgBox::MBR_OK )
		{
			// If the name is invalid
			if( !PlayerProfile::IsValidProfileName( sNewProfileName.c_str() ) )
			{
				// Create the message box to warn the player of a bad name
				m_BadNameIsOpenForNew = false;
				m_pBadNameMsgBox = MsgBox::CreateMsgBox( INVALID_PROFILE_NAME_MESSAGE );
				GUIMgr::Get().PopupMessageBox( m_pBadNameMsgBox );
			}
			// Otherwise attempt to rename
			else
			{
				std::wstring sSelectedProfileName = m_pProfileList->GetItemString( m_pProfileList->GetSelItemIndex() );

				if( sSelectedProfileName.length() > 0 )
				{
					std::wstring sOldProfileFile = PlayerProfile::GetFilePathFromName( sSelectedProfileName.c_str() );

					std::wstring sNewProfileFile = PlayerProfile::GetFilePathFromName( sNewProfileName.c_str() );

					// If the rename failed
					if( TCBase::RenameFile( sOldProfileFile.c_str(), sNewProfileFile.c_str() ) != 0 )
					{
						//TODO localize
						// Display the name message box again
						m_pCantCompleteMsgBox = MsgBox::CreateMsgBox( L"The profile could not be renamed. You will have to delete the profile from Windows instead. Please visit our website for help.", MsgBox::BL_OK );
						GUIMgr::Get().PopupMessageBox( m_pCantCompleteMsgBox );				
					}
					// Otherwise update the list
					else
					{
						PopulateProfileList();

						// Select the newly renamed profile
						m_pProfileList->SetSelItemIndex( m_pProfileList->IndexOfString( sNewProfileFile.c_str() ) );
					}
				}
			}
		}
	}
	// Or if we are displaying a simple message to the user
	else if( m_pCantCompleteMsgBox ) 
	{
		// If the user hasn't closed the box yet
		if( m_pCantCompleteMsgBox->GetRetVal() == MsgBox::MBR_Error )
			return;

		// Free the message box
		GUIMgr::Get().CloseMessageBox( m_pCantCompleteMsgBox );
		delete m_pCantCompleteMsgBox;
		m_pCantCompleteMsgBox = 0;		
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutProfileSelection::PopulateProfileList  Private
///
///	Populate the profile list with all known profiles.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutProfileSelection::PopulateProfileList()
{
	if( !m_pProfileList )
		return;

	// Get the profiles
	m_pProfileList->ClearStrings();
	std::list<std::wstring> profiles = PlayerProfile::GetAllProfileNames();
	for( std::list<std::wstring>::iterator iterProfile = profiles.begin(); iterProfile != profiles.end(); ++iterProfile )
		m_pProfileList->AddString( iterProfile->c_str() );

	m_pProfileList->ReformatControl();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutHelpMeLearn::MainListCB  Static Private
///
///	\param selIndex The selected item index
///	\param pList The list that invoked this function
///
///	The callback for the main list.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutHelpMeLearn::MainListCB( uint32 selIndex, GUICtrlList* pList )
{
	if( !pList || !pList->GetContainingLayout() )
		return;
	GUILayoutHelpMeLearn* pLayout = (GUILayoutHelpMeLearn*)pList->GetContainingLayout();

#ifndef NOPROFILES
	// If the player wants to improve skills
	if( selIndex == LISTINDEX_IMPROVESKILLS )
	{
		if( GameMgr::Get().GetActiveProfile() )
			GUIMgr::Get().SetActiveLayout( GameDefines::MS_LearningAssistance );
		else
		{
			//TODO localize
			pLayout->m_pMustLogInMsgBox = MsgBox::CreateMsgBox( L"You must be logged in to use the \"Improve My Skills Feature\"." );
			GUIMgr::Get().PopupMessageBox( pLayout->m_pMustLogInMsgBox );
		}
	}
	// Else if the player wants to practice
	else if( selIndex == LISTINDEX_PRACTICE )
	{
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_PracticeSetup );
	}
	// Else if the player wants run the tutorials
	else if( selIndex == LISTINDEX_TUTORIALS )
	{
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_Tutorials );
	}
#else
	if( selIndex == LISTINDEX_PRACTICE - 1 )
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_PracticeSetup );
	// Else if the player wants run the tutorials
	else if( selIndex == LISTINDEX_TUTORIALS )
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_Tutorials );
#endif
	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutHelpMeLearn::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutHelpMeLearn::OnActivate()
{
	GameGUILayout::OnActivate();
	
#ifdef NOPROFILES
	if( m_pMainList->GetNumListItems() == 3 )
	{
		m_pMainList->RemoveString( LISTINDEX_IMPROVESKILLS );
		m_pMainList->ReformatControl();
	}
#endif
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutHelpMeLearn::OnDeactivate  Public
///
///	A message handler called when layout is closed.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutHelpMeLearn::OnDeactivate()
{
	// If there is a message box then close it
	if( m_pMustLogInMsgBox )
	{
		// Close the message box
		GUIMgr::Get().CloseMessageBox( m_pMustLogInMsgBox );
		delete m_pMustLogInMsgBox;
		m_pMustLogInMsgBox = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutHelpMeLearn::Update  Public
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutHelpMeLearn::Update( float32 frameTime )
{
	GameGUILayout::Update( frameTime );

	// If there is a message box
	if( m_pMustLogInMsgBox )
	{
		// If it is not ready to close then do nothing
		if( m_pMustLogInMsgBox->GetRetVal() == MsgBox::MBR_Error )
			return;

		// Close the message box
		GUIMgr::Get().CloseMessageBox( m_pMustLogInMsgBox );
		delete m_pMustLogInMsgBox;
		m_pMustLogInMsgBox = 0;
	}

	// If the pointers to the mode list or help label are null then bail
	if( !m_pMainList || !m_pHelpLabel )
		return;

	// Get the current list item
	int32 curListItem = m_pMainList->GetMouseOverItem();

	// If the selection has not changed then bail
	if( curListItem == m_CurMainListIndex )
		return;
	m_CurMainListIndex = curListItem;

	// If the mouse is over nothing then clear the help lable
	if( m_CurMainListIndex == -1 )
		m_pHelpLabel->SetText( L"" );
	// Else update the text
	else
	{
		//TODO localize
		std::wstring sText;
#ifndef NOPROFILES
		if( m_CurMainListIndex == LISTINDEX_IMPROVESKILLS )
			sText = L"Based on your previous gameplay, you will be given a specially setup game field that focuses on your weakest points.";
		else if( m_CurMainListIndex == LISTINDEX_PRACTICE )
			sText = L"Play a game of Prime Time mode gameplay, but configure what numbers are generated. Also the product of your current selection is displayed as an aid.";
		else if( m_CurMainListIndex == LISTINDEX_TUTORIALS )
			sText = L"Quickly and effortlessly learn how to play Prime Time's various modes.";
#else
		if( m_CurMainListIndex == LISTINDEX_PRACTICE - 1 )
			sText = L"Play a game of Prime Time mode gameplay, but configure what numbers are generated. Also the product of your current selection is displayed as an aid.";
		else if( m_CurMainListIndex == LISTINDEX_TUTORIALS )
			sText = L"Quickly and effortlessly learn how to play Prime Time's various modes.";
		
#endif
		// Wrap the text and display it
		if( m_pHelpLabel->GetFont() )
			sText = m_pHelpLabel->GetFont()->GetWordWrapString( sText.c_str(), m_HelpLabelBox );
		m_pHelpLabel->SetText( sText.c_str() );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutHelpMeLearn::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutHelpMeLearn::HookupCtrls()
{
	GameGUILayout::HookupCtrls();

	// Link up the back button
	GUICtrlButton* pBackButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_BackButton );
	if( pBackButton )
		pBackButton->SetClickCallback( LayoutCB_BackToTitle );

	// Link up the main list
	if( m_pMainList )
		m_pMainList->SetCallback( MainListCB );

	// If there is a help label then set its bounding box
	if( m_pHelpLabel )
	{
		m_HelpLabelBox.pos = m_pHelpLabel->GetPos();
		int32 displayWidth = g_pGraphicsMgr->GetDisplayDims().x;
		m_HelpLabelBox.size.x = (displayWidth * 5) / 6;

		// Center the box
		m_HelpLabelBox.pos.x = (displayWidth - m_HelpLabelBox.size.x) / 2;
		m_pHelpLabel->SetPos( m_HelpLabelBox.pos );

		// Resize the box vertically
		if( pBackButton )
			m_HelpLabelBox.size.y = pBackButton->GetPos().y - m_HelpLabelBox.pos.y;
		else
			m_HelpLabelBox.size.y = 60;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::MinMaxValueChangeCB  Public
///
///	The callback for when the user changes the minimum value callback.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::MinMaxValueChangeCB( GUICtrlTextBox* pTextBox )
{
	// Get the layout
	GUILayoutPracticeSetup* pLayout = (GUILayoutPracticeSetup*)pTextBox->GetContainingLayout();
	if( !pLayout )
		return;

	// If this is the minimum value
	if( pTextBox->GetName() == pLayout->CTRLNAME_MinProdTextBox )
		pLayout->m_MinProductValue = TCBase::WToI( pTextBox->GetText().c_str() );
	// Else the maximum product value changed
	else
		pLayout->m_MaxProductValue = TCBase::WToI( pTextBox->GetText().c_str() );

	// Update the number generatable
	pLayout->UpdateNumberOfProducts();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  BackToLearnCB  Global
///
///	Go back to the Help Me Learn menu.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static void BackToLearnCB( GUICtrlButton* )
{
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_HelpMeLearn );
}


/// The default constructor
GUILayoutPracticeSetup::GUILayoutPracticeSetup() : GameGUILayout(),
							CTRLNAME_BackButton( L"back_button" ),
							CTRLNAME_PlayButton( L"play_button" ),
							CTRLNAME_MinProdTextBox( L"min_prod_textbox" ),
							CTRLNAME_MaxProdTextBox( L"max_prod_textbox" ),
							CTRLNAME_MaxNumGenProdLabel( L"num_products_label" ),
							
							CTRLNAME_MinNumFactorsLabel( L"min_num_factors_label"),
							CTRLNAME_MinNumFactorsUpBtn( L"min_num_factors_upbtn" ),
							CTRLNAME_MinNumFactorsDownBtn( L"min_num_factors_downbtn" ),
							
							CTRLNAME_PrimeChecksPreLabel( L"prime_check_" ),
							CTRLNAME_PrimeReqChecksPreLabel( L"prime_req_check_" ),
							
							m_pMinProductTextBox( 0 ),
							m_pMaxProductTextBox( 0 ),
							m_pNumProductsLabel( 0 ),
							m_pMinNumFactorsLabel( 0 ),
							m_IsFactorRequired( false ),
							m_ReqFactorIndex( 0 ),
							m_CurNumProds( 0 ),
							m_MinProductValue( 0 ),
							m_MaxProductValue( 100 ),
							m_MinNumFactors( 2 ),
							m_pMsgBox( 0 ),
							m_pSettingsProfile( 0 )
{
	// Add the prime check boxes
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		// Clear the variables
		m_pPrimeChecks[ primeIndex ] = 0;
		m_pReqPrimeChecks[ primeIndex ] = 0;

		// Add the expected controls
		std::wstring sName = CTRLNAME_PrimeChecksPreLabel;
		sName += TCBase::EasyIToA( GameDefines::PRIMES[ primeIndex ] );
		AddExpectedCtrl( sName.c_str(), GUIDefines::CT_CheckBox, &(m_pPrimeChecks[ primeIndex ]) );

		sName = CTRLNAME_PrimeReqChecksPreLabel;
		sName += TCBase::EasyIToA( GameDefines::PRIMES[ primeIndex ] );
		AddExpectedCtrl( sName.c_str(), GUIDefines::CT_CheckBox, &(m_pReqPrimeChecks[ primeIndex ]) );
	}

	m_LayoutID = GameDefines::MS_PracticeSetup;

	// Add expected controls
	AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
	AddExpectedCtrl( CTRLNAME_PlayButton, GUIDefines::CT_Button );
	AddExpectedCtrl( CTRLNAME_MinProdTextBox, GUIDefines::CT_TextBox, &m_pMinProductTextBox );
	AddExpectedCtrl( CTRLNAME_MaxProdTextBox, GUIDefines::CT_TextBox, &m_pMaxProductTextBox );
	AddExpectedCtrl( CTRLNAME_MaxNumGenProdLabel, GUIDefines::CT_Label, &m_pNumProductsLabel );

	AddExpectedCtrl( CTRLNAME_MinNumFactorsLabel, GUIDefines::CT_Label, &m_pMinNumFactorsLabel );
	AddExpectedCtrl( CTRLNAME_MinNumFactorsUpBtn, GUIDefines::CT_Button );
	AddExpectedCtrl( CTRLNAME_MinNumFactorsDownBtn, GUIDefines::CT_Button );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::HookupCtrls()
{
	GameGUILayout::HookupCtrls();

	// Link up the back button
	GUICtrlButton* pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_BackButton );
	if( pButton )
		pButton->SetClickCallback( BackToLearnCB );

	// Link up the play button
	pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_PlayButton );
	if( pButton )
		pButton->SetClickCallback( PlayButtonCB );

	// Assign the minimum number of factor callbacks
	pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_MinNumFactorsUpBtn );
	if( pButton )
		pButton->SetClickCallback( MinNumFactorsChangeCB );
	pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_MinNumFactorsDownBtn );
	if( pButton )
		pButton->SetClickCallback( MinNumFactorsChangeCB );

	// Hook up the minimum product value text box
	if( m_pMinProductTextBox )
	{
		m_pMinProductTextBox->SetCallback( MinMaxValueChangeCB );
		m_pMinProductTextBox->SetInputFilter( GUICtrlTextBox::IF_PositiveWholeNumbers );
	}

	// Hook up the maximum product value text box
	if( m_pMaxProductTextBox )
	{
		m_pMaxProductTextBox->SetCallback( MinMaxValueChangeCB );
		m_pMaxProductTextBox->SetInputFilter( GUICtrlTextBox::IF_PositiveWholeNumbers );
	}

	// Hook up the prime factor check boxes
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		if( m_pPrimeChecks[ primeIndex ] )
			m_pPrimeChecks[ primeIndex ]->SetCallback( AllowableFactorCB );

		if( m_pReqPrimeChecks[ primeIndex ] )
		{
			m_pReqPrimeChecks[ primeIndex ]->SetCallback( ReqFactorCB );
			m_pReqPrimeChecks[ primeIndex ]->SetCheckState( true );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::OnActivate()
{
	GameGUILayout::OnActivate();

	// If a player is logged in
	if( GameMgr::Get().GetActiveProfile() != 0 )
	{
		// Store the settings profile subset
		m_pSettingsProfile = (ProfileSubsetSettings*)GameMgr::Get().GetActiveProfile()->GetSubset( ProfileSubsetSettings::SUBSET_ID );

		// Initialize the UI from the saved settings
		SetUIFromFilter( m_pSettingsProfile->m_PracticeSettings );
	}
	// Else noone is logged in
	else
	{
		// There is no settings profile subset
		m_pSettingsProfile = 0;

		// Initialize the UI with defaults
		if( m_pMinProductTextBox )
			m_pMinProductTextBox->SetText( TCBase::EasyIToA( m_MinProductValue ) );
		if( m_pMaxProductTextBox )
			m_pMaxProductTextBox->SetText( TCBase::EasyIToA( m_MaxProductValue ) );
		UpdateMinNumFactorLabel();
		UpdateVisibleReqFactors();
		UpdateNumberOfProducts();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::AllowableFactorCB  Static Private
///
/// The callback for when the player selects an allowable prime factor check box
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::AllowableFactorCB( GUICtrlCheckBox* pCheckBox )
{
	// Get the layout
	GUILayoutPracticeSetup* pLayout = (GUILayoutPracticeSetup*)pCheckBox->GetContainingLayout();
	if( !pLayout )
		return;
	pLayout->UpdateVisibleReqFactors();
	pLayout->UpdateNumberOfProducts();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::ReqFactorCB  Static Private
///
/// The callback for when the player selects a required prime factor check box
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::ReqFactorCB( GUICtrlCheckBox* pCheckBox )
{
	// Get the layout
	GUILayoutPracticeSetup* pLayout = (GUILayoutPracticeSetup*)pCheckBox->GetContainingLayout();
	if( !pLayout )
		return;
	pLayout->UpdateNumberOfProducts();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::UpdateVisibleReqFactors  Static Private
///
/// Update which required prime factors are visible
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::UpdateVisibleReqFactors()
{
	// Hook up the prime factor check boxes
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		if( !m_pPrimeChecks[ primeIndex ] || !m_pReqPrimeChecks[ primeIndex ] )
			continue;

		// Set the visibility based on if the factors are allowed
		m_pReqPrimeChecks[ primeIndex ]->SetVisible( !m_pPrimeChecks[ primeIndex ]->GetChecked() );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::PlayButtonCB  Static Private
///
///	\param pButton The button that invoked this callback
///
///	The callback for when the user presses play.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::PlayButtonCB( GUICtrlButton* pButton )
{
	// Get the layout
	GUILayoutPracticeSetup* pLayout = reinterpret_cast<GUILayoutPracticeSetup*>( pButton->GetContainingLayout() );
	if( !pLayout )
		return;
	
	// Ensure there are enough products
	if( pLayout->m_CurNumProds == 0 )
	{
		//TODO localize
		// Pop-up a message box then bail
		pLayout->m_pMsgBox = MsgBox::CreateMsgBox( L"You must configure the parameters so you have at least 1 generatable product." );
		GUIMgr::Get().PopupMessageBox( pLayout->m_pMsgBox );
		return;
	}

	// Setup the product filter
	GameDefines::ProductSubsetFilter prodFilter = pLayout->GetFilterFromUI();

	// Start a game
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
	GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_PrimeTime | GameDefines::GT_Mod_Practice), prodFilter );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::MinNumFactorsChangeCB  Private
///
///	The callback for when the user presses the minimum number of factors scroll buttons.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::MinNumFactorsChangeCB( GUICtrlButton* pButton )
{
	// Get the layout
	GUILayoutPracticeSetup* pLayout = (GUILayoutPracticeSetup*)pButton->GetContainingLayout();
	if( !pLayout )
		return;

	// Determine if the up or down button was pressed
	bool isScrollUp = pButton->GetName() == pLayout->CTRLNAME_MinNumFactorsUpBtn;

	// If the user wants to scroll up
	if( isScrollUp )
	{
		// If the current max factor index is smaller than the maximum then increment it
		if( pLayout->m_MinNumFactors < GameDefines::MAX_PRIME_FACTORS_NEEDED )
			++pLayout->m_MinNumFactors;
	}
	// Else the user wants to scroll down
	else
	{
		// If the minimum number of factors is still valid
		if( pLayout->m_MinNumFactors > 2 )
			--pLayout->m_MinNumFactors;
	}

	// Update the required factor and number of products labels
	pLayout->UpdateMinNumFactorLabel();
	pLayout->UpdateNumberOfProducts();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::Update( float32 frameTime )
{
	GameGUILayout::Update( frameTime );

	// If there is a message box open
	if( m_pMsgBox )
	{
		if( m_pMsgBox->GetRetVal() != MsgBox::MBR_Error )
		{
			// Close the message box
			GUIMgr::Get().CloseMessageBox( m_pMsgBox );
			delete m_pMsgBox;
			m_pMsgBox = 0;
		}
		else
			return;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::OnDectivate  Public
///
///	A message handler called when the layout is deactivated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::OnDectivate()
{
	// Clear the message box if there is one
	if( m_pMsgBox )
	{
		// Close the message box
		GUIMgr::Get().CloseMessageBox( m_pMsgBox );
		delete m_pMsgBox;
		m_pMsgBox = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::UpdateMinNumFactorLabel  Private
///
///	Update the minimum number of factors label.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::UpdateMinNumFactorLabel()
{
	if( !CTRLNAME_MinNumFactorsLabel )
		return;

	// Set the text
	m_pMinNumFactorsLabel->SetText( TCBase::EasyUIToA(m_MinNumFactors) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::GetFilterFromUI  Private
///
/// Get the settings into a product filter
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameDefines::ProductSubsetFilter GUILayoutPracticeSetup::GetFilterFromUI() const
{
	// Setup the product filter
	GameDefines::ProductSubsetFilter prodFilter;
	prodFilter.minValue = m_MinProductValue;
	prodFilter.maxValue = m_MaxProductValue;
	prodFilter.minNumFactors = m_MinNumFactors;
	prodFilter.maxNumFactors = GameDefines::MAX_PRIME_FACTORS_NEEDED;
	
	// Go through the prime checks
	prodFilter.useablePrimes = 0;
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		if( !m_pPrimeChecks[primeIndex] )
			continue;

		// If the prime box is not checked then allow the prime
		if( !m_pPrimeChecks[primeIndex]->GetChecked() )
			prodFilter.useablePrimes |= (1 << primeIndex);

		// If the prime is required
		if( m_pReqPrimeChecks[primeIndex] && m_pReqPrimeChecks[primeIndex]->IsVisible() && !m_pReqPrimeChecks[primeIndex]->GetChecked() )
			prodFilter.requiredPrimes |= (1 << primeIndex);
	}

	// Return the filter
	return prodFilter;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::SetUIFromFilter  Private
///
/// Set the settings from a product filter
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::SetUIFromFilter( const GameDefines::ProductSubsetFilter& prodFilter )
{
	// Setup the product filter
	m_MinProductValue = prodFilter.minValue;
	m_pMinProductTextBox->SetText( TCBase::EasyIToA( m_MinProductValue ) );
	m_MaxProductValue = prodFilter.maxValue;
	m_pMaxProductTextBox->SetText( TCBase::EasyIToA( m_MaxProductValue ) );
	m_MinNumFactors = prodFilter.minNumFactors;
	UpdateMinNumFactorLabel();
	
	// Go through the prime checks
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		bool isUsable = (prodFilter.useablePrimes & (1 << primeIndex)) != 0;

		// Set if the prime is usable
		if( m_pPrimeChecks[primeIndex] != 0 )
			m_pPrimeChecks[primeIndex]->SetCheckState( !isUsable );

		// Set if the prime is required
		if( m_pReqPrimeChecks[primeIndex] != 0 )
			m_pReqPrimeChecks[primeIndex]->SetCheckState( (prodFilter.requiredPrimes & (1 << primeIndex) ) == 0 );
	}

	// Update the number of products
	UpdateVisibleReqFactors();
	UpdateNumberOfProducts();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutPracticeSetup::UpdateNumberOfProducts  Private
///
///	Update the number of generatable products.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutPracticeSetup::UpdateNumberOfProducts()
{
	// Setup the product filter
	GameDefines::ProductSubsetFilter prodFilter = GetFilterFromUI();

	// If there is a profile then store the settings
	if( m_pSettingsProfile )
		m_pSettingsProfile->m_PracticeSettings = prodFilter;

	// Get the number of products
	GameDefines::GetProductArraySubset( prodFilter, true, &m_CurNumProds );

	// Update the label
	if( m_pNumProductsLabel )
	{
		//TODO localize
		// Set the text
		std::wstring sText( L"Number Of Generatable Products:" );
		sText += TCBase::EasyUIToA( m_CurNumProds );
		m_pNumProductsLabel->SetText( sText.c_str() );

		// Center the label
		//int32 width = m_pNumProductsLabel->GetBoundBox().size.x;
		//int32 newX = (g_pGraphicsMgr->GetDisplayDims().x - width) / 2;
		//m_pNumProductsLabel->SetPos( Point2i( newX, m_pNumProductsLabel->GetPos().y ) );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTutorials::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTutorials::HookupCtrls()
{
	GameGUILayout::HookupCtrls();

	// Link up the back button
	GUICtrlButton* pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_BackButton );
	if( pButton )
	{
#ifdef _PT_DEMO
		pButton->SetClickCallback( LayoutCB_BackToTitle );
#else
		pButton->SetClickCallback( BackToLearnCB );
#endif
	}

	// Link up the list
	if( m_pTutorialList )
	{
		m_pTutorialList->Clear();
		//TODO localize
#ifdef _PT_DEMO
		m_pTutorialList->AddString( L"Basic Gameplay" );
		m_pTutorialList->AddString( L"Combos" );
#else
		m_pTutorialList->AddString( L"Basic Gameplay" );
		m_pTutorialList->AddString( L"Ceiling" );
		m_pTutorialList->AddString( L"Combos" );
		m_pTutorialList->AddString( GameDefines::GetGameTypeDesc(GameDefines::GT_AdditionAttack) );
		m_pTutorialList->AddString( GameDefines::GetGameTypeDesc(GameDefines::GT_Fractions) );
#endif
		m_pTutorialList->ReformatControl();
		m_pTutorialList->SetCallback( ListSelCB );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutTutorials::ListSelCB  Static Private
///
///	\param selIndex The list index that was selected
///	\param UNNAMED The list that invoked this callback
///
///	The callback for when a tutorial is selected from the list.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutTutorials::ListSelCB( uint32 selIndex, GUICtrlList* )
{
	// Track which tutorial they played so we don't ask again
	int32 noPromptGameType = -1;

#ifdef _PT_DEMO
	// If the user wants to run the Prime Time tutorial
	if( selIndex == 0 )
	{
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_PrimeTime | GameDefines::GT_Mod_Tutorial) );
	}
	// Else if the user wants to run the combos tutorial
	else if( selIndex == 1 )
	{
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_PrimeTime | GameDefines::GT_Mod_Tutorial2) );
	}
#else
	// If the user wants to run the Prime Time tutorial
	if( selIndex == GUILayoutTutorials::LISTIDX_PRIMETIME )
	{
		noPromptGameType = (int32)GameDefines::GT_PrimeTime;

		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_PrimeTime | GameDefines::GT_Mod_Tutorial) );
	}
	// Else if the user wants to run the ceiling tutorial
	else if( selIndex == GUILayoutTutorials::LISTIDX_CEILING )
	{
		noPromptGameType = (int32)GameDefines::GT_Ceiling;

		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_Ceiling | GameDefines::GT_Mod_Tutorial) );
	}
	// Else if the user wants to run the combos tutorial
	else if( selIndex == GUILayoutTutorials::LISTIDX_COMBOS )
	{
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_PrimeTime | GameDefines::GT_Mod_Tutorial2) );
	}
	// Else if the user wants to run the combos tutorial
	else if( selIndex == GUILayoutTutorials::LISTIDX_ADDITION )
	{
		noPromptGameType = (int32)GameDefines::GT_AdditionAttack;

		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_AdditionAttack | GameDefines::GT_Mod_Tutorial) );
	}
	// Else if the user wants to run the combos tutorial
	else if( selIndex == GUILayoutTutorials::LISTIDX_FRACTIONS )
	{
		noPromptGameType = (int32)GameDefines::GT_Fractions;

		GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
		GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_Fractions | GameDefines::GT_Mod_Tutorial) );
	}
#endif

	// The player is running a tutorial so don't ask if they want to try one
	if( noPromptGameType > 0 )
	{
		// Set the flag and remember it (Subtract 1, because the first game type has a value of 1,
		// not 0)
		ApplicationBase::Get()->Settings.PromptedForTutorial[noPromptGameType - 1] = true;
		ApplicationBase::Get()->Settings.SaveToFile();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLearningAssistance::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLearningAssistance::HookupCtrls()
{
	GameGUILayout::HookupCtrls();

	// Link up the back button
	GUICtrlButton* pButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_BackButton );
	if( pButton )
		pButton->SetClickCallback( BackToLearnCB );

	// Link up the play button
	if( m_pPlayButton )
		m_pPlayButton->SetClickCallback( OnPlayPress );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutLearningAssistance::SetInstText  Private
///
/// Set and word-wrap the instruction text
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLearningAssistance::SetInstText( const wchar_t* szText )
{
	if( !m_pInstLabel || !m_pInstLabel->GetFont() )
		return;

	// Word-wrap the text
	const TCFont* pFont = m_pInstLabel->GetFont();
	std::wstring sModStr = pFont->GetWordWrapString( szText, m_InstBoundBox );
	
	// Set the text
	m_pInstLabel->SetText( sModStr.c_str() );
}

/// The maximum amount of time to consider when processing sessions
const uint32 MAX_DAYS_OBSERVED = 14;
const time_t OLDEST_SESSION = 60 * 60 * 24 * MAX_DAYS_OBSERVED;


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutLearningAssistance::OnActivate  Public
///
///	A message handler called when the layout is activated.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutLearningAssistance::OnActivate()
{
	GameGUILayout::OnActivate();

	// Calculate the instruction text bounding box
	if( m_pInstLabel )
	{
		m_InstBoundBox.size.x = g_pGraphicsMgr->GetDisplayDims().x - (m_pInstLabel->GetPos().x * 2);
		m_InstBoundBox.size.y = g_pGraphicsMgr->GetDisplayDims().y - m_pInstLabel->GetPos().y;
	}

	GatherData( _playersAnalyzedData );	

	// If enough time has been played (10 minutes)
	const uint32 REQ_TIME_PLAYED = 60 * 10;
	if( _playersAnalyzedData.totalSecsPlayed < REQ_TIME_PLAYED )
	{
		//TODO localize
		std::wstring sMsg( L"You have " );
		if( _playersAnalyzedData.totalSecsPlayed == 0 )
			sMsg += L"not played within the last 2 weeks,";
		else
		{
			sMsg += L"played about ";
			int timePlayed = (uint32)(_playersAnalyzedData.totalSecsPlayed / 60.0f) + 1;
			if( timePlayed >= 10 )
				timePlayed = 9;
			sMsg += TCBase::EasyUIToA( timePlayed );
			if( timePlayed == 1 )
				sMsg += L" minute";
			else
				sMsg += L" minutes";
			sMsg += L" within the last 2 weeks,";
		}

		sMsg += L" but you need to have played at least 10 minutes for learning assistance to be accurate. Please play the multiplication modes more then try again.";
		
		// Hide the play button
		if( m_pPlayButton )
			m_pPlayButton->SetVisible( false );

		// Set the text and bail out
		SetInstText( sMsg.c_str() );
		return;
	}

	// Show the play button
	if( m_pPlayButton )
		m_pPlayButton->SetVisible( true );

	// If the player is making a lot of mistakes with a specific prime factor
	bool gameStarted = Setup_PrimeImpv( _playersAnalyzedData );

	// If the player does not make many combos
	if( !gameStarted )
		gameStarted = Setup_ComboSizeImpv( _playersAnalyzedData );

	// The player does not need help and is not on the hardest difficulty then tell them to try harder

	// See if the player needs help making combos
	if( !gameStarted )
		gameStarted = Setup_ComboSizeImpv( _playersAnalyzedData );

	// The player plays at the hardest so present a new challenge of super difficulty
	if( !gameStarted )
		gameStarted = Setup_IncreaseDiff( _playersAnalyzedData );
}

/// The callback for when the player presses the 'Play' button
void GUILayoutLearningAssistance::OnPlayPress( GUICtrlButton* pButton )
{
	GUILayoutLearningAssistance* pLayout = static_cast<GUILayoutLearningAssistance*>( pButton->GetContainingLayout() );

	// Start a game
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_InGame1Player );
	
	GameMgr::Get().StartGame( (GameDefines::EGameType)(GameDefines::GT_PrimeTime | GameDefines::GT_Mod_LearningAssitance), pLayout->_productFilter );
}

/// Analyze the play data
void GUILayoutLearningAssistance::GatherData( AnalyzedData& data )
{
	// Get the player profile
	PlayerProfile* pActiveProfile = GameMgr::Get().GetActiveProfile();
	if( !pActiveProfile || !m_pInstLabel )
	{
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_HelpMeLearn );
		return;
	}

	// Get the multiplication subset
	ProfileSubsetMultiplication* pMultSubset = (ProfileSubsetMultiplication*)pActiveProfile->GetSubset( ProfileSubsetMultiplication::SUBSET_ID );
	if( !pMultSubset )
	{
		GUIMgr::Get().SetActiveLayout( GameDefines::MS_HelpMeLearn );
		return;
	}

	// Go through the sessions and total the time played for the sessions played in the last 14 days
	const GameSessionStatsMult* pSession = pMultSubset->GetFirstSession();
	int32 avgDiff = 0;
	while( pSession )
	{
		// Store the last session to determine if learning assistance was tried just before this
		// session
		data.pLastAnalyzedSession = pSession;

		// If this is a "improve my skills" session
		if( (pSession->m_GameType & GameDefines::GT_Mod_LearningAssitance) != 0 )
		{
			// Store the session
			data.pLastImprovementSession = pSession;

			// If this session was a difficulty increasing session
			if( (pSession->m_ContextVal & ProfileSubsetMultiplication::PRO_SESS_TYPEMASK) == ProfileSubsetMultiplication::PRO_SESS_TYPE_INCDIFF )
			{
				int32 diffLevel = pSession->m_ContextVal & ProfileSubsetMultiplication::PRO_SESS_DIFFINC_VAL_MASK;
				diffLevel >>= ProfileSubsetMultiplication::PRO_SESS_DIFFINC_VAL_OFFSET;

				// If the level played is higher than currently stored then store the new value
				if( diffLevel > data.secretDiffPlayed )
					data.secretDiffPlayed = diffLevel;
			}
		}

		// If the session is not valid then skip it
		if( !IsSessionValid( pSession ) )
		{
			pSession = pMultSubset->GetNextSession();
			continue;
		}

		++data.numSessionsConsidered;

		// Add up the session duration
		data.totalSecsPlayed += pSession->m_Duration;

		// Add in the difficulty
		avgDiff += (int32)pSession->m_Difficulty;

		// Total up the prime usage counts
		for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
		{
			data.primeUsageCounts[ primeIndex ] += pSession->m_ValidCounts[ primeIndex ];
			data.primeUsageCounts[ primeIndex ] += pSession->m_ErrorCounts[ primeIndex ];
			data.primeErrorCounts[ primeIndex ] += pSession->m_ErrorCounts[ primeIndex ];
		}

		data.avgCombo += pSession->m_NumCombos;
		data.avgMaxCombo += pSession->m_MaxCombo;

		// Get the next session
		pSession = pMultSubset->GetNextSession();
	}

	// Average the necessary values
	if( data.numSessionsConsidered > 0 )
	{
		avgDiff /= data.numSessionsConsidered;
		data.avgCombo /= data.numSessionsConsidered;
		data.avgMaxCombo /= data.numSessionsConsidered;
	}

	// Convert the difficulty
	if( avgDiff < 0 )
		data.avgDiffLevel = GameDefines::GPDL_Pre3;
	else if( avgDiff >= (int32)GameDefines::GPDL_Post8 )
		data.avgDiffLevel = GameDefines::GPDL_Post8;
	else
		data.avgDiffLevel = (GameDefines::EGameplayDiffLevel)avgDiff;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutLearningAssistance::Setup_PrimeImpv  Private
///
///	\param data The data mined from analyzing the player's most recent sessions
///
///	Setup a learning session to improve the player's skills with a specfic prime factor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUILayoutLearningAssistance::Setup_PrimeImpv( const GUILayoutLearningAssistance::AnalyzedData& data )
{
	// See if any of the factors have a significant error count
	float32 curErrorPct = 0.0f;
	uint32 badPrimeIndex = GameDefines::NUM_PRIMES;
	float32 backupErrorPct = 0.0f;
	uint32 backupBadPrimeIndex = GameDefines::NUM_PRIMES;
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		// Ensure the number has been used a fair amount
		if( data.primeUsageCounts[ primeIndex ] < 20 )
			continue;

		// If the number has been used in mistakes too much
		float32 errorPct = (float32)data.primeErrorCounts[ primeIndex ] / (float32)data.primeUsageCounts[ primeIndex ];
		if( errorPct > 0.3f && errorPct > curErrorPct )
		{
			// Store the current error number
			backupErrorPct = curErrorPct;
			backupBadPrimeIndex = primeIndex;

			// Store the new error number
			curErrorPct = errorPct;
			badPrimeIndex = primeIndex;
		}
	}

	// If there is a valid candidate for prime improvement then check if it was already worked on
	// last session
	if( badPrimeIndex != GameDefines::NUM_PRIMES )
	{
		// If the previous session was learning assistance
		const GameSessionStatsMult* pLastSession = data.pLastAnalyzedSession;
		if( pLastSession != 0 && (pLastSession->m_GameType & GameDefines::GT_Mask_Mod) == GameDefines::GT_Mod_LearningAssitance )
		{
			// If the session was prime value improvement
			if( (pLastSession->m_ContextVal & ProfileSubsetMultiplication::PRO_SESS_TYPEMASK) == ProfileSubsetMultiplication::PRO_SESS_TYPE_PRMIMPV )
			{
				// Get the prime factor that was focussed on in that session
				uint32 prevImpvVal = (pLastSession->m_ContextVal & ProfileSubsetMultiplication::PRO_SESS_PRMIMPV_VAL_MASK) >> ProfileSubsetMultiplication::PRO_SESS_PRMIMPV_VAL_OFFSET;

				// If the prime factor is the same as the one last focussed on
				if( prevImpvVal == badPrimeIndex )
				{
					// Use the backup
					curErrorPct = backupErrorPct;
					badPrimeIndex = backupBadPrimeIndex;
				}
			}
		}
	}

	// If there is not still a valid candidate for prime factor improvement then bail
	if( badPrimeIndex == GameDefines::NUM_PRIMES )
		return false;

	//TODO localize
	// Create the message text
	int32 primeVal = GameDefines::PRIMES[ badPrimeIndex ];
	std::wstring sMsg = L"After analyzing your most recent gameplay it appears that you make the most mistakes when trying to use ";
	sMsg += TCBase::EasyIToA( primeVal );
	sMsg += L" as a factor. The following session will focus on using the number ";
	sMsg += TCBase::EasyIToA( primeVal );
	sMsg += L". Press 'Play' to continue.";
	SetInstText( sMsg.c_str() );

	// Setup the product filter and ensure it covers the prime to work on
	_productFilter = GameLogicProduct::GetFilterFromDiff( data.avgDiffLevel, 1 );
	_productFilter.useablePrimes |= (3 << badPrimeIndex);
//	_productFilter.reqFactorIndex = badPrimeIndex;

	return true;
}

/// Setup a session to improve combo length
bool GUILayoutLearningAssistance::Setup_ComboSizeImpv( const GUILayoutLearningAssistance::AnalyzedData& )
{
	return false;
}

/// Tell the user to try a harder level
bool GUILayoutLearningAssistance::Setup_IncreaseDiff( const GUILayoutLearningAssistance::AnalyzedData& data )
{
	// Total the usage and error counts
	uint32 errorCounts = 0;
	uint32 usageCounts = 0;
	for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
	{
		errorCounts += data.primeErrorCounts[primeIndex];
		usageCounts += data.primeUsageCounts[primeIndex];
	}

	// Get the error percent
	float32 errorPct = 1.0f;
	if( usageCounts > 0 )
		errorPct = (float32)errorCounts / (float32)usageCounts;

	// If the user makes frequent mistakes (12.5% in this case) then don't advance difficulty
	const float32 RARE_ERROR_PCT = 0.125f;
	if( errorPct > RARE_ERROR_PCT )
		return false;

	// If the player is already playing secret levels
	std::wstring sMsg;
	if( data.secretDiffPlayed > 0 )
	{
		GameDefines::EGameplayDiffLevel diffLevel = GameDefines::GPDL_Secret1;

		// If the last secret level played was the first one and there is a last improvement session
		if( data.secretDiffPlayed == 1 && data.pLastImprovementSession )
		{
			// If the player cleared equations and played long enough
			if( data.pLastImprovementSession->m_NumEqsCleared > 20 && data.pLastImprovementSession->m_Duration > (5 * 60) )
			{
				// Determine the last error percent
				float32 lastErrorPct = (float32)data.pLastImprovementSession->m_NumEqsFailed / (float32)(data.pLastImprovementSession->m_NumEqsCleared + data.pLastImprovementSession->m_NumEqsFailed);

				// If the player was very accurate (10% in this case)
				if( lastErrorPct < 0.1f )
				{
					//TODO localize
					// Advance to the second secret level
					sMsg = L"Most impressive, it appears that even the special difficulty level is not enough of a challenge for you. Well, we have one more secret challenge for you that is insanely hard. Good luck, you'll need it.";
					diffLevel = GameDefines::GPDL_Secret2;
				}
			}
		}
		
		// If the player is continuing the difficulty
		if( sMsg.size() == 0 )
		{
			//TODO localize
			if( diffLevel == GameDefines::GPDL_Secret2 )
				sMsg = L"Back for more? Go ahead and give the insane difficulty another try.";
			else
				sMsg = L"You're still doing very well so let's continue playing the special difficulty level. Good luck!";
		}
		
		// Get the product filter
		_productFilter = GameLogicProduct::GetFilterFromDiff( diffLevel, 1 );
	}
	// Else if the player needs to try the secret levels
	else if( data.avgDiffLevel >= GameDefines::GPDL_Post8 )
	{
		//TODO localize
		sMsg = L"After analyzing your recent gameplay, it looks like the normal difficulty levels don't provide enough of a challenge for you. You will now be presented with a super hard challenge that will push your math skills to the limits.";

		_productFilter = GameLogicProduct::GetFilterFromDiff( GameDefines::GPDL_Secret1, 1 );
	}
	else
	{
		//TODO localize
		sMsg = L"You are a really great player and it appears that you need more of a challenge. You will be presented with a difficulty level harder than what you usually play. Try it out and let's see how you do!";
		
		_productFilter = GameLogicProduct::GetFilterFromDiff( (GameDefines::EGameplayDiffLevel)(data.avgDiffLevel + 1), 1 );
	}

	// Display the new text
	SetInstText( sMsg.c_str() );

	// Return that a sessions was started
	return true;
}


/// Test if a gameplay session is valid for consideration
bool GUILayoutLearningAssistance::IsSessionValid( const GameSessionStatsMult* pSession )
{
	if( !pSession )
		return false;

	// If the session is practice or tutorial
	if( (pSession->m_GameType & GameDefines::GT_Mask_Mod) != 0 )
		return false;

	// Get the current system time
	time_t currentTime;
	time( &currentTime );

	// Get the session start date
	time_t startTime = mktime( (tm*)&pSession->m_SessionStartTime );
	
	// If this session started too long ago
	if( (currentTime - startTime) > OLDEST_SESSION )
		return false;

	// The session is valid
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutGameResults::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutGameResults::HookupCtrls()
{
	GameGUILayout::HookupCtrls();

	// Link up the back button
	GUICtrlButton* pDoneButton = (GUICtrlButton*)GetCtrlByName( CTRLNAME_DoneButton );
	if( pDoneButton )
		pDoneButton->SetClickCallback( OnDonePress );

	// Hide the default label (The one that is used as a position and font reference for the other
	// labels) if it is there
	if( m_pDefLabel )
		m_pDefLabel->SetVisible( false );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutGameResults::OnDonePress  Static Private
///
/// The callback invoked when the player presses the 'Done' button
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutGameResults::OnDonePress( GUICtrlButton* )
{
	GUIMgr::Get().SetActiveLayout( GameDefines::MS_TitleScreen );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutGameResults::CreateLabels  Private
///	\param stats The stats to display
///
/// Create the labels to display the stats
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutGameResults::CreateLabels( const StatList& stats, const wchar_t* szGameTypeName )
{
	// Clear any labels that may have been left from last time, although this could only happen
	// if the layout's deactivate handler wasn't called for some reason
	ClearAddedLabels();

	// If there is a title label then update it
	if( m_pTitleLabel )
	{
		std::wstring sTitle = szGameTypeName;
		//TODO localize
		sTitle += L" Results";
		m_pTitleLabel->SetText( sTitle.c_str() );

		// Center the control
		CenterCtrlHorz( m_pTitleLabel );
	}

	// Get the font with which labels are rendered
	TCFontHndl fontHndl;
	if( m_pDefLabel )
		fontHndl = ResourceMgr::Get().GetTCFont( m_pDefLabel->GetFont()->GetResID() );
	else
		fontHndl = GameDefines::GetDefaultGameFont();

	const int32 SPLIT_WIDTH = 12;
	const int32 SPLIT_X_POS = 600;
	
	// Get the starting Y position
	int32 curYPos = 100;
	if( m_pDefLabel )
		curYPos = m_pDefLabel->GetPos().y;

	// Go through all of the stats and generate labels
	for( StatList::const_iterator iterStat = stats.begin(); iterStat != stats.end(); ++iterStat )
	{
		// Create the name label
		GUICtrlLabel* pNewNameLabel = (GUICtrlLabel*)GUIControl::CreateControl( GUIDefines::CT_Label, this );
		pNewNameLabel->SetData( iterStat->first.c_str(), fontHndl );

		// Right align the name label
		Point2i pos;
		pos.x = (SPLIT_X_POS - (SPLIT_WIDTH / 2)) - pNewNameLabel->GetBoundBox().size.x;
		pos.y = curYPos;
		pNewNameLabel->SetPos( pos );

		// Create the value label
		GUICtrlLabel* pNewValueLabel = (GUICtrlLabel*)GUIControl::CreateControl( GUIDefines::CT_Label, this );
		pNewValueLabel->SetData( iterStat->second.c_str(), fontHndl );
		
		// Left align the value
		pos.x = SPLIT_X_POS + (SPLIT_WIDTH / 2);
		pos.y = curYPos;
		pNewValueLabel->SetPos( pos );

		// Add the controls
		AddControl( pNewNameLabel );
		AddControl( pNewValueLabel );

		_addedLabels.push_back( pNewNameLabel );
		_addedLabels.push_back( pNewValueLabel );

		// Step down to the next line
		curYPos += (int32)((float32)pNewNameLabel->GetBoundBox().size.y * 2.5f);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutGameResults::DisplayResults  Public
///	\param session The game session for which results are displayed
///
/// Display the results for a played game
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutGameResults::DisplayResults( const GameSessionStatsMult& session )
{
	//TODO localize
	StatList stats;
	stats.push_back( StringPair( std::wstring(L"Time Played:"), TCBase::EasyDurationToA( session.m_Duration, false ) ) );

	stats.push_back( StringPair( std::wstring(L"Score:"), TCBase::EasyIToA( session.m_Score ) ) );

	stats.push_back( StringPair( std::wstring(L"Difficulty:"), GameDefines::GetDifficultyDesc( session.m_Difficulty ) ) );

	stats.push_back( StringPair( std::wstring(L"Products Cleared:"), TCBase::EasyIToA( session.m_NumEqsCleared ) ) );

	stats.push_back( StringPair( std::wstring(L"Mistakes:"), TCBase::EasyIToA( session.m_NumEqsFailed ) ) );

	stats.push_back( StringPair( std::wstring(L"Highest Combo:"), TCBase::EasyIToA( session.m_MaxCombo ) ) );

	float32 blocksPerMin = 0;
	if( session.m_Duration > 0 )
		blocksPerMin = (float32)session.m_NumBlocksCleared / (session.m_Duration / 60.0f);
	stats.push_back( StringPair( std::wstring(L"Average Blocks Cleared Per Minute:"), TCBase::EasyFToA( blocksPerMin, 2 ) ) );

	stats.push_back( StringPair( std::wstring(L"Level Reached:"), TCBase::EasyIToA( session.m_LevelReached ) ) );

	// Display the values
	CreateLabels( stats, GameDefines::GetGameTypeDesc( session.m_GameType) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutGameResults::DisplayResults  Public
///	\param session The game session for which results are displayed
///
/// Display the results for a played game
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutGameResults::DisplayResults( const GameSessionStatsAdd& session )
{		
	//TODO localize
	StatList stats;
	stats.push_back( StringPair( std::wstring(L"Time Played:"), TCBase::EasyDurationToA( session.m_Duration, false ) ) );

	stats.push_back( StringPair( std::wstring(L"Score:"), TCBase::EasyIToA( session.m_Score ) ) );

	stats.push_back( StringPair( std::wstring(L"Difficulty:"), GameDefines::GetDifficultyDesc( session.m_Difficulty ) ) );

	stats.push_back( StringPair( std::wstring(L"Sums Cleared:"), TCBase::EasyIToA( session.m_NumEqsCleared ) ) );
	
	stats.push_back( StringPair( std::wstring(L"Mistakes:"), TCBase::EasyIToA( session.m_NumEqsFailed ) ) );

	// Display the values
	CreateLabels( stats, GameDefines::GetGameTypeDesc(GameDefines::GT_AdditionAttack) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutGameResults::DisplayResults  Public
///	\param session The game session for which results are displayed
///
/// Display the results for a fractions game
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutGameResults::DisplayResults( const GameSessionStatsFractions& session )
{
	//TODO localize
	StatList stats;
	stats.push_back( StringPair( std::wstring(L"Time Played:"), TCBase::EasyDurationToA( session.m_Duration, false ) ) );

	stats.push_back( StringPair( std::wstring(L"Score:"), TCBase::EasyIToA( session.m_Score ) ) );

	stats.push_back( StringPair( std::wstring(L"Difficulty:"), GameDefines::GetDifficultyDesc( session.m_Difficulty ) ) );

	stats.push_back( StringPair( std::wstring(L"Fractions Cleared:"), TCBase::EasyIToA( session.m_NumFractionsCleared ) ) );
	
	stats.push_back( StringPair( std::wstring(L"Mistakes:"), TCBase::EasyIToA( session.m_NumFractionsFailed ) ) );

	// Display the values
	CreateLabels( stats, GameDefines::GetGameTypeDesc(GameDefines::GT_Fractions) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutGameResults::ClearAddedLabels  Private
///
/// Clear any labels added to display stats
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutGameResults::ClearAddedLabels()
{
	// Remove the label controls
	for( ControlList::iterator iterLabel = _addedLabels.begin(); iterLabel != _addedLabels.end(); ++iterLabel )
	{
		RemoveControl( *iterLabel );
		delete *iterLabel;
	}

	// Clear the label list
	_addedLabels.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayoutGameResults::OnDeactivate  Public
///
///	A message handler called when the layout is deactivated.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutGameResults::OnDeactivate()
{
	GameGUILayout::OnDeactivate();

	ClearAddedLabels();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutDemoExit::OnActivate  Public
///
/// A message handler called when the layout is activated
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutDemoExit::OnActivate()
{
	GameGUILayout::OnActivate();

	// Hide the exit button
	if( m_pExitButton )
		m_pExitButton->SetVisible( false );

	// Start countdown
	m_IsCountingDown = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutDemoExit::Update  Public
///
///	\param frameTime The elapsed frame time
///
/// Update the layout
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutDemoExit::Update( float32 frameTime )
{
	GameGUILayout::Update( frameTime );

	// If the screen is counting down
	if( m_IsCountingDown )
	{
		// Update the label
		if( m_pTimeLeftLabel )
		{
			//TODO localize
			std::wstring sTimeLeft = L"This screen is frozen for ";
			sTimeLeft += TCBase::EasyIToA( 5 - (int32)m_TimeSinceActivate );
			sTimeLeft += L" more seconds...";

			// Set the text and center the label
			m_pTimeLeftLabel->SetText( sTimeLeft.c_str() );
			CenterCtrlHorz( m_pTimeLeftLabel );
		}

		// If the time has elapsed then exit
		if( m_TimeSinceActivate > 5.0f )
		{
			//TODO localize
			// Display the final message and stop the count down
			m_pTimeLeftLabel->SetText( L"Press the exit button to quit" );
			CenterCtrlHorz( m_pTimeLeftLabel );
			m_IsCountingDown = false;

			// If there is an exit button then display it
			if( m_pExitButton )
				m_pExitButton->SetVisible( true );
			// Else there is no exit button so just quit
			else
				ApplicationBase::Get()->SetAppToExit();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutDemoExit::OnExitPress  Static Private
///
/// The callback invoked when the player presses the 'Exit' button
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutDemoExit::OnExitPress( GUICtrlButton* )
{
	ApplicationBase::Get()->SetAppToExit();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayoutDemoExit::HookupCtrls  Public
///
/// Link the control objects to their corresponding object found in the layout
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayoutDemoExit::HookupCtrls()
{
	GameGUILayout::HookupCtrls();

	// Hook-up the exit button
	if( m_pExitButton )
		m_pExitButton->SetClickCallback( OnExitPress );
}