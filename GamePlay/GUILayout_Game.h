//=================================================================================================
/*!
	\file GUILayout_Game.h
	2D Game Engine
	Graphical User Interface Game Screen Layout Header
	\author Taylor Clark
	\date February 18, 2006

	This file contains the definition for the main menu layout class.
*/
//=================================================================================================

#pragma once
#ifndef __GUILayout_Game_h
#define __GUILayout_Game_h

#include "GameGUILayout.h"
#include "GameDefines.h"
#include <stdio.h>
#include "PlayerProfile.h"
//#include "Network/IPAddress.h"
//#include "GameNetMsgs.h"
#include "ProfileSubsetMultiplication.h"
#include "ProfileSubsetAddition.h"
#include "GameSessionStatsFractions.h"

class GUICtrlList;
class GUICtrlLabel;
class GUICtrlSprite;
class GUICtrlCheckBox;
class GUICtrlTextBox;
class MsgBox;
class NetConnLAN;
class NetConnTCP;
class GameField;
class ProfileSubsetSettings;


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutTitleScreen
	\brief The class for the title screen GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutTitleScreen : public GameGUILayout
{
private:

	/// The main menu list
	GUICtrlList* m_pMainList;

	/// The label displaying who is logged in
	GUICtrlLabel* m_pLoggedInLabel;

	/// The label displaying the custom text
	GUICtrlLabel* m_pCustomTextLabel;

	/// The label displaying the game version
	GUICtrlLabel* m_pGameVersionLabel;

	/// The sprite that displays the custom picture, if there is one
	GUICtrlSprite* m_pCustomPicSprite;

	/// The label that displays the custom picture message, if there is one
	GUICtrlLabel* m_pCustomPicMsgLabel;

    /// The button that lets the user adjust the audio volume
	GUICtrlButton* m_pAdjustVolumeButton;

    /// The button that lets the user adjust the game resolution
	GUICtrlButton* m_pAdjustVideoButton;

	/// The message box alerting the user to log-in
	MsgBox* m_pMustLogInMsgBox;

	/// The message box asking if the user wants to try a tutorial
	MsgBox* m_pAskTryTutorialMsgBox;

	/// Control names
	const wchar_t* CTRLNAME_MainList;
	const wchar_t* CTRLNAME_LoggedInLabel;
	const wchar_t* CTRLNAME_CustomTextLabel;
	const wchar_t* CTRLNAME_GameVersionLabel;
	const wchar_t* CTRLNAME_CustomPicSprite;
	const wchar_t* CTRLNAME_CustomPicMsgLabel;
    const wchar_t* CTRLNAME_AdjustVolumeButton;
    const wchar_t* CTRLNAME_AdjustVideoButton;
				
	static const uint32 LISTITEMINDEX_1PLAYER = 0;
	//static const uint32 LISTITEMINDEX_2PLAYER = 1;
	static const uint32 LISTITEMINDEX_LogIn = 1;
	static const uint32 LISTITEMINDEX_HELPMELEARN = 2;
	//static const uint32 LISTITEMINDEX_MYSTATS = 3;

	static void TitleScreenCB_MainListSel( uint32 selIndex, GUICtrlList* );
	static void TitleScreenCB_MainListSel_Demo( uint32 selIndex, GUICtrlList* );
    static void TitleScreenCB_AdjustVolumeClick( GUICtrlButton* );
    static void TitleScreenCB_AdjustVideoClick( GUICtrlButton* );

public:

	/// The default constructor
	GUILayoutTitleScreen() : GameGUILayout(),
								CTRLNAME_MainList( L"main_list" ),
								CTRLNAME_LoggedInLabel( L"logged_in_label" ),
								CTRLNAME_CustomTextLabel( L"custom_text_label" ),
								CTRLNAME_GameVersionLabel( L"version_label" ),
								CTRLNAME_CustomPicSprite( L"CustomPicSprite" ),
								CTRLNAME_CustomPicMsgLabel( L"CustomPicMsgLabel" ),
								CTRLNAME_AdjustVolumeButton( L"AdjustVolumeButton" ),
                                CTRLNAME_AdjustVideoButton( L"AdjustVideoButton" ),
								m_pMainList( 0 ),
								m_pLoggedInLabel( 0 ),
								m_pMustLogInMsgBox( 0 ),
								m_pAskTryTutorialMsgBox( 0 ),
								m_pCustomTextLabel( 0 ),
								m_pGameVersionLabel( 0 ),
								m_pCustomPicSprite( 0 ),
								m_pCustomPicMsgLabel( 0 ),
                                m_pAdjustVolumeButton( 0 ),
                                m_pAdjustVideoButton( 0 )
	{
		m_LayoutID = GameDefines::MS_TitleScreen;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_MainList, GUIDefines::CT_List, &m_pMainList );
		AddExpectedCtrl( CTRLNAME_LoggedInLabel, GUIDefines::CT_Label, &m_pLoggedInLabel );
		AddExpectedCtrl( CTRLNAME_CustomTextLabel, GUIDefines::CT_Label, &m_pCustomTextLabel );
		AddExpectedCtrl( CTRLNAME_GameVersionLabel, GUIDefines::CT_Label, &m_pGameVersionLabel );
		AddExpectedCtrl( CTRLNAME_CustomPicSprite, GUIDefines::CT_Sprite, &m_pCustomPicSprite );
		AddExpectedCtrl( CTRLNAME_CustomPicMsgLabel, GUIDefines::CT_Label, &m_pCustomPicMsgLabel );
        AddExpectedCtrl( CTRLNAME_AdjustVolumeButton, GUIDefines::CT_Button, &m_pAdjustVolumeButton );
        AddExpectedCtrl( CTRLNAME_AdjustVideoButton, GUIDefines::CT_Button, &m_pAdjustVideoButton );
	}

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// A message handler called when the layout is activated
	virtual void OnActivate();

    bool HandleMessage( GUIMessage* pMsg );

	/// Update the layout
	virtual void Update( float32 frameTime );

	/// Set the custom text
	void SetCustomText( const wchar_t* szText );

	/// Set the custom picture
	void SetCustomPic( TCImage* pCustomPicImage, const wchar_t* szText );

	/// Set the game version label
	void SetGameVersionText( int gameVersion );
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutPlayMode
	\brief The class for the game type GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutPlayMode : public GameGUILayout
{
private:

	/// Control names
	const wchar_t* CTRLNAME_TypeList;
	const wchar_t* CTRLNAME_BackButton;
	const wchar_t* CTRLNAME_HelpLabel;
	const wchar_t* CTRLNAME_DifficultyLabel;
	const wchar_t* CTRLNAME_DifficultyUpButton;
	const wchar_t* CTRLNAME_DifficultyDownButton;

	/// The mode list
	GUICtrlList* m_pModeList;

	/// The help label
	GUICtrlLabel* m_pHelpLabel;

	/// The game difficulty label
	GUICtrlLabel* m_pDiffLabel;

	/// The difficulty adjustment buttons
	GUICtrlButton* m_pDifficultyUpButton;
	GUICtrlButton* m_pDifficultyDownButton;

	/// The help label bounding box
	Box2i m_HelpLabelBox;

	/// The index of the list item the mouse is currently over
	int32 m_CurListItem;

	/// The current game difficulty
	GameDefines::EGameplayDiffLevel m_GameDiff;

	GameDefines::EGameType _selectedGameType;


	/// The message box asking if the user wants to try a tutorial
	MsgBox* m_pAskTryTutorialMsgBox;

	/// The callback function for when the user selects a list item
	static void ModeSelCallback( uint32 selIndex, GUICtrlList* );

	static void OnDiffChange( GUICtrlButton* pButton );

	void AdjustDifficulty( int32 difficultyChange );

	static const uint32 LISTITEMINDEX_PRIMETIME = 0;
	static const uint32 LISTITEMINDEX_CEILING = 1;
	static const uint32 LISTITEMINDEX_ADDITIONATTACK = 2;
	static const uint32 LISTITEMINDEX_FRACTIONS = 3;
	static const uint32 LISTITEMINDEX_CUSTOM = 4;


public:

	/// The default constructor
	GUILayoutPlayMode() : GameGUILayout(),
								CTRLNAME_TypeList( L"mode_list" ),
								CTRLNAME_BackButton( L"back_button" ),
								CTRLNAME_HelpLabel( L"help_label" ),
								CTRLNAME_DifficultyLabel( L"difficulty_label" ),
								CTRLNAME_DifficultyUpButton( L"difficulty_up_button" ),
								CTRLNAME_DifficultyDownButton( L"difficulty_down_button" ),
								m_pModeList( 0 ),
								m_pHelpLabel( 0 ),
								m_pDiffLabel( 0 ),
								m_pDifficultyUpButton( 0 ),
								m_pDifficultyDownButton( 0 ),
								m_CurListItem( -1 ),
								m_GameDiff( GameDefines::GPDL_5 ),
								m_pAskTryTutorialMsgBox( 0 ),
								_selectedGameType( GameDefines::GT_PrimeTime )
	{
		m_LayoutID = GameDefines::MS_ModeSelect;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_TypeList, GUIDefines::CT_List, &m_pModeList );
		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_HelpLabel, GUIDefines::CT_Label, &m_pHelpLabel );
		AddExpectedCtrl( CTRLNAME_DifficultyLabel, GUIDefines::CT_Label, &m_pDiffLabel );
		AddExpectedCtrl( CTRLNAME_DifficultyUpButton, GUIDefines::CT_Button, &m_pDifficultyUpButton );
		AddExpectedCtrl( CTRLNAME_DifficultyDownButton, GUIDefines::CT_Button, &m_pDifficultyDownButton );
	}

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// Update the layout
	virtual void Update( float32 frameTime );
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutModeOptions
	\brief The class for the game mode options GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutModeOptions : public GameGUILayout
{
private:

	/// Control names
	const wchar_t* CTRLNAME_BackButton;
	const wchar_t* CTRLNAME_PlayButton;
	const wchar_t* CTRLNAME_DiffScrollUpBtn;
	const wchar_t* CTRLNAME_DiffScrollDownBtn;
	const wchar_t* CTRLNAME_DiffLabel;
	const wchar_t* CTRLNAME_TitleLabel;
	const wchar_t* CTRLNAME_TimeScrollUpBtn;
	const wchar_t* CTRLNAME_TimeScrollDownBtn;
	const wchar_t* CTRLNAME_TimeLabel;
	const wchar_t* CTRLNAME_TimeLimitCheckBox;
	
	/// The callback for when the user presses back
	static void BackButtonCB( GUICtrlButton* );

	/// The callback for when the user presses play
	static void PlayButtonCB( GUICtrlButton* );

	/// The scroll button callbacks
	static void DiffScrollUpCB( GUICtrlButton* pButton );
	static void DiffScrollDownCB( GUICtrlButton* pButton );
	static void TimeScrollUpCB( GUICtrlButton* pButton );
	static void TimeScrollDownCB( GUICtrlButton* pButton );

	// The check box callback
	static void TimeLimitCheckCB( GUICtrlCheckBox* pCheckBox );

	/// The current game type
	GameDefines::EGameType m_GameType;

	/// The current game difficulty
	GameDefines::EGameplayDiffLevel m_GameDiff;

	/// The game difficulty label
	GUICtrlLabel* m_pDiffLabel;

	/// The time limit label
	GUICtrlLabel* m_pTimeLimitLabel;

	/// The time limit, in seconds
	uint32 m_TimeLimit;

	/// The time limit check box
	GUICtrlCheckBox* m_pTimeLimitCheckBox;

	/// The time limit scroll buttons
	GUICtrlButton* m_pTimeScrollUpBtn;
	GUICtrlButton* m_pTimeScrollDownBtn;
	
	/// The player profile subset for game settings
	ProfileSubsetSettings* m_pSettingsProfile;


	/// Update the time limit
	void UpdateTimeLimit();

public:

	/// The default constructor
	GUILayoutModeOptions() : GameGUILayout(),
								CTRLNAME_BackButton( L"back_button" ),
								CTRLNAME_PlayButton( L"play_button" ),
								CTRLNAME_DiffScrollUpBtn( L"diff_scroll_up" ),
								CTRLNAME_DiffScrollDownBtn( L"diff_scroll_down" ),
								CTRLNAME_DiffLabel( L"diff_label" ),
								CTRLNAME_TitleLabel( L"title_label" ),
								CTRLNAME_TimeScrollUpBtn( L"time_scroll_up" ),
								CTRLNAME_TimeScrollDownBtn( L"time_scroll_down" ),
								CTRLNAME_TimeLabel( L"time_label" ),
								CTRLNAME_TimeLimitCheckBox( L"time_limit_check" ),
								m_GameType( GameDefines::GT_PrimeTime ),
								m_GameDiff( GameDefines::GPDL_4 ),
								m_pDiffLabel( 0 ),
								m_pTimeLimitLabel( 0 ),
								m_TimeLimit( 180 ),
								m_pTimeLimitCheckBox( 0 ),
								m_pTimeScrollUpBtn( 0 ),
								m_pTimeScrollDownBtn( 0 ),
								m_pSettingsProfile( 0 )
	{
		m_LayoutID = GameDefines::MS_ModeOptions;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_PlayButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_DiffScrollUpBtn, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_DiffScrollDownBtn, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_DiffLabel, GUIDefines::CT_Label, &m_pDiffLabel );
		AddExpectedCtrl( CTRLNAME_TimeScrollUpBtn, GUIDefines::CT_Button, &m_pTimeScrollUpBtn );
		AddExpectedCtrl( CTRLNAME_TimeScrollDownBtn, GUIDefines::CT_Button, &m_pTimeScrollDownBtn );
		AddExpectedCtrl( CTRLNAME_TimeLabel, GUIDefines::CT_Label, &m_pTimeLimitLabel );
		AddExpectedCtrl( CTRLNAME_TimeLimitCheckBox, GUIDefines::CT_CheckBox, &m_pTimeLimitCheckBox );
	}

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// Set the game type
	void SetGameType( GameDefines::EGameType gameType );

	/// Set the game difficulty
	void SetGameDifficulty( GameDefines::EGameplayDiffLevel gameDiff );
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutMultiplayType
	\brief The class for the multiplayer type selection GUI layout
*/
//-------------------------------------------------------------------------------------------------
//class GUILayoutMultiplayType : public GameGUILayout
//{
//private:
//	
//	/// Control names
//	const wchar_t* CTRLNAME_TypeList;
//	const wchar_t* CTRLNAME_BackButton;
//	const wchar_t* CTRLNAME_HelpLabel;
//
//	/// The mode list
//	GUICtrlList* m_pTypeList;
//
//	/// The help label
//	GUICtrlLabel* m_pHelpLabel;
//
//	/// The help label bounding box
//	Box2i m_HelpLabelBox;
//
//	/// The index of the list item the mouse is currently over
//	int32 m_CurListItem;
//
//	/// The callback function for when the user selects a list item
//	static void TypeSelCallback( uint32 selIndex, GUICtrlList* );
//
//	static const uint32 LISTITEMINDEX_LAN = 0;
//	static const uint32 LISTITEMINDEX_IP = 1;
//
//public:
//
//	/// The default constructor
//	GUILayoutMultiplayType() : GameGUILayout(),
//								CTRLNAME_TypeList( L"type_list" ),
//								CTRLNAME_BackButton( L"back_button" ),
//								CTRLNAME_HelpLabel( L"help_label" ),
//								m_pTypeList( 0 ),
//								m_pHelpLabel( 0 ),
//								m_CurListItem( -1 )
//	{
//		m_LayoutID = GameDefines::MS_MultiplayerType;
//
//		// Add expected controls
//		AddExpectedCtrl( CTRLNAME_TypeList, GUIDefines::CT_List, &m_pTypeList );
//		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_HelpLabel, GUIDefines::CT_Label, &m_pHelpLabel );
//	}
//
//	/// A message handler called when the layout is activated
//	virtual void OnActivate();
//
//	/// Link the control objects to their corresponding object found in the layout
//	virtual void HookupCtrls();
//
//	/// Update the layout
//	virtual void Update( float32 frameTime );
//};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutInGame
	\brief The class for the in-game GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutInGame : public GameGUILayout
{
public:

	enum ELabels
	{
		L_Score = 0,
		L_ScoreName,
		L_Level,
		L_LevelName,
		L_Count,
		L_CountName,
		L_Time,
		L_ComboBlock,
		L_ComboCountVal,
		L_SumLabel,
		L_CurrentProd,
		L_CurrentProdName,
		L_TutorialNote,
		L_FieldBoxPos,
		L_COUNT
	};

private:

	/// Control names
	const wchar_t* CTRLNAME_ScoreLabel;
	const wchar_t* CTRLNAME_ScoreNameLabel;
	const wchar_t* CTRLNAME_LevelLabel;
	const wchar_t* CTRLNAME_LevelNameLabel;
	const wchar_t* CTRLNAME_CountLabel;
	const wchar_t* CTRLNAME_CountNameLabel;
	const wchar_t* CTRLNAME_TimeElapsed;
	const wchar_t* CTRLNAME_ComboBlock;
	const wchar_t* CTRLNAME_ComboCountText;
	const wchar_t* CTRLNAME_ComboCountVal;
	const wchar_t* CTRLNAME_SumNameLabel;
	const wchar_t* CTRLNAME_CurrentProdNameLabel;
	const wchar_t* CTRLNAME_CurrentProdLabel;
	const wchar_t* CTRLNAME_TutorialNoteLabel;
	const wchar_t* CTRLNAME_FieldPosLabel;
	const wchar_t* CTRLNAME_MenuButton;
	const wchar_t* CTRLNAME_SpeedUpButton;

	/// The text labels
	GUICtrlLabel* m_pLabels[L_COUNT];

	/// The combo text sprite
	GUICtrlSprite* m_pComboTextSpr;

	/// The menu button
	GUICtrlButton* m_pMenuButton;

	/// The speed up button
	GUICtrlButton* m_pSpeedUpButton;

	/// The current Y position of the row of the game field background that is being darkened
	int32 m_FieldDarkenYPos;

	/// The direction of the field background darkening
	int32 m_FieldDarkenYDelta;

	/// The time used to flash the tutorial note
	float32 m_TutorialNoteFlashTime;

	std::map< int32, float32 > _flashTimeMap;

	/// Get the font used to render text based on the background
	ResourceID GetFontResID() const;

	/// Update the label flashing
	void UpdateFlashing( float32 frameTime );

	static void OnSpeedButtonDown(GUICtrlButton* pButton);

	static void OnSpeedButtonUp(GUICtrlButton* pButton);

public:

	/// The default constructor
	GUILayoutInGame() : GameGUILayout(),
							CTRLNAME_ScoreLabel( L"score_label" ),
							CTRLNAME_ScoreNameLabel( L"score_name_label" ),
							CTRLNAME_LevelLabel( L"level_label" ),
							CTRLNAME_LevelNameLabel( L"level_name_label" ),
							CTRLNAME_CountLabel( L"count_label" ),
							CTRLNAME_CountNameLabel( L"count_name_label" ),
							CTRLNAME_TimeElapsed( L"time_elapsed_label" ),
							CTRLNAME_ComboBlock( L"combo_block_label" ),
							CTRLNAME_ComboCountText( L"combo_text" ),
							CTRLNAME_ComboCountVal( L"combo_count_label" ),
							CTRLNAME_SumNameLabel( L"sum_label" ),
							CTRLNAME_CurrentProdLabel( L"currentprod_label" ),
							CTRLNAME_CurrentProdNameLabel( L"currentprod_name_label" ),
							CTRLNAME_TutorialNoteLabel( L"tutorial_note_label" ),
							CTRLNAME_FieldPosLabel( L"field_pos_label" ),
							CTRLNAME_MenuButton( L"menu_button" ),
							CTRLNAME_SpeedUpButton( L"SpeedUpButton" ),
							m_pComboTextSpr( 0 ),
							m_FieldDarkenYPos( -1 ),
							m_FieldDarkenYDelta( 1 ),
							m_TutorialNoteFlashTime( -1.0f )
	{
		m_LayoutID = GameDefines::MS_InGame1Player;

		for( int32 labelIndex = 0; labelIndex < L_COUNT; ++labelIndex )
			m_pLabels[ labelIndex ] = 0;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_ScoreLabel, GUIDefines::CT_Label, &m_pLabels[L_Score] );
		AddExpectedCtrl( CTRLNAME_ScoreNameLabel, GUIDefines::CT_Label, &m_pLabels[L_ScoreName] );
		AddExpectedCtrl( CTRLNAME_LevelLabel, GUIDefines::CT_Label, &m_pLabels[L_Level] );
		AddExpectedCtrl( CTRLNAME_LevelNameLabel, GUIDefines::CT_Label, &m_pLabels[L_LevelName] );
		AddExpectedCtrl( CTRLNAME_CountLabel, GUIDefines::CT_Label, &m_pLabels[L_Count] );
		AddExpectedCtrl( CTRLNAME_CountNameLabel, GUIDefines::CT_Label, &m_pLabels[L_CountName] );
		AddExpectedCtrl( CTRLNAME_TimeElapsed, GUIDefines::CT_Label, &m_pLabels[L_Time] );
		AddExpectedCtrl( CTRLNAME_ComboBlock, GUIDefines::CT_Label, &m_pLabels[L_ComboBlock] );
		AddExpectedCtrl( CTRLNAME_ComboCountText, GUIDefines::CT_Sprite, &m_pComboTextSpr );
		AddExpectedCtrl( CTRLNAME_ComboCountVal, GUIDefines::CT_Label, &m_pLabels[L_ComboCountVal] );
		AddExpectedCtrl( CTRLNAME_SumNameLabel, GUIDefines::CT_Label, &m_pLabels[L_SumLabel] );
		AddExpectedCtrl( CTRLNAME_CurrentProdLabel, GUIDefines::CT_Label, &m_pLabels[L_CurrentProd] );
		AddExpectedCtrl( CTRLNAME_CurrentProdNameLabel, GUIDefines::CT_Label, &m_pLabels[L_CurrentProdName] );
		AddExpectedCtrl( CTRLNAME_TutorialNoteLabel, GUIDefines::CT_Label, &m_pLabels[L_TutorialNote] );
		AddExpectedCtrl( CTRLNAME_FieldPosLabel, GUIDefines::CT_Label, &m_pLabels[L_FieldBoxPos] );
		AddExpectedCtrl( CTRLNAME_MenuButton, GUIDefines::CT_Button, &m_pMenuButton );
		AddExpectedCtrl( CTRLNAME_SpeedUpButton, GUIDefines::CT_Button, &m_pSpeedUpButton );
	}

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// A message handler called when the layout is deactivated
	virtual void OnDeactivate();

	/// Update the layout
	virtual void Update( float32 frameTime );

	/// Reset the needed data if the graphics system has been reset
	virtual void OnGraphicsRefresh();

	/// Set the level
	void SetScore( uint32 score );

	/// Set the score
	void SetLevel( int32 level );

	/// Set the count
	void SetCount( int32 count );

	/// Set the combo count
	void SetComboCount( uint32 comboCount );

	/// Set the product
	void SetProduct( int32 product );

	/// Set the game field
	void SetGameField( GameField* pField );

	/// Set the tutorial note
	void SetNoteText( const wchar_t* szNote );

	/// Set the count label text
	void SetCountLabelText( const wchar_t* szLabelText );

	/// Set the text of a label
	void SetLabelText( ELabels labelId, const wchar_t* szText );

	/// Set the text of a label
	std::wstring GetLabelText( ELabels labelId ) const;

	/// Set the count text
	void SetCountText( const wchar_t* szText );

	/// Get the bound box for a label so we can justify other graphics off of a label position
	Box2i GetLabelBoundBox( ELabels labelId );

	/// Flash the tutorial note
	void FlashTutorialNote(){ m_TutorialNoteFlashTime = 0.0f; }

	/// Flash the tutorial note
	void FlashLabel(ELabels labelId){ _flashTimeMap[ (int32)labelId ] = 0.0f; }

	// Get the sum label bottom left position
	Point2i GetSumBottomLeftPos() const;

	/// Set the time played
	void SetTimePlayed( uint32 secsElapsed );

	/// Update the layout based on the game type
	void InitFromGameType( GameDefines::EGameType gameType );
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutProfileSelection
	\brief The class for the in-game GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutProfileSelection : public GameGUILayout
{
private:

	/// Control names
	const wchar_t* CTRLNAME_ProfilesList;
	const wchar_t* CTRLNAME_BackButton;
	const wchar_t* CTRLNAME_CreateProfButton;
	const wchar_t* CTRLNAME_SelectProfButton;
	const wchar_t* CTRLNAME_ScrollProfUp;
	const wchar_t* CTRLNAME_ScrollProfDown;
	const wchar_t* CTRLNAME_LogoutButton;
	const wchar_t* CTRLNAME_RenameProfileButton;
	const wchar_t* CTRLNAME_DeleteProfileButton;
	const wchar_t* CTRLNAME_KeepLoggedInCheckBox;
	const wchar_t* CTRLNAME_KeepLoggedInLabel;

	/// Messages
	static const wchar_t* MSG_EnterNewProfileName;
	static const wchar_t* MSG_EnterRenameProfileName;
	
	GUICtrlScrollList* m_pProfileList;

	/// The message box to receive a player name
	MsgBox* m_pCreateProfMsgBox;

	/// The message box to warn the player of a bad player name
	MsgBox* m_pBadNameMsgBox;

	/// The message box to allow the player to enter a new name
	MsgBox* m_pRenameMsgBox;

	/// The message box to confirm the player wants to remove a profile
	MsgBox* m_pDeleteProfileMsgBox;

	/// The message box asking a player if he/she would like to play through the tutorials
	MsgBox* m_pTryTutorialsMsgBox;

	/// The message box alerting the player to a failed profile rename or delete
	MsgBox* m_pCantCompleteMsgBox;

	/// A flag indicating if the bad name message box is open due to a new profiel being created or
	/// false if a profile is being renamed
	bool m_BadNameIsOpenForNew;

	GUICtrlButton* _pRenameProfileButton;

	GUICtrlButton* _pDeleteProfileButton;

	GUICtrlCheckBox* _pKeepLoggedInCheckBox;
	GUICtrlLabel* _pKeepLoggedInLabel;

	/// A callback function for when the user presses the create button
	static void CreateBtnCallback( GUICtrlButton* pBtn );

	/// The callback function for when the user selects a profile
	static void SelectProfBtnCallback( GUICtrlButton* pBtn );

	/// The callback function for when the user presses the logout button
	static void SelectLogoutBtnCallback( GUICtrlButton* pBtn );

	/// Populate the profile list with all known profiles
	void PopulateProfileList();

	/// An event handler invoked when the user selects a profile in the list
	static void OnProfileListItemSelect( GUICtrlScrollList* pScrollList );

	/// An event handler invoked when the user selects a profile in the list
	static void OnRenameProfileButtonClicked( GUICtrlButton* pButton );

	/// An event handler invoked when the user selects a profile in the list
	static void OnDeleteProfileButtonClicked( GUICtrlButton* pButton );

public:

	/// The default constructor
	GUILayoutProfileSelection() : GameGUILayout(),
							CTRLNAME_ProfilesList( L"profile_list" ),
							CTRLNAME_BackButton( L"back_button" ),
							CTRLNAME_CreateProfButton( L"create_button" ),
							CTRLNAME_SelectProfButton( L"select_prof_button" ),
							CTRLNAME_ScrollProfUp( L"profile_list_scroll_up" ),
							CTRLNAME_ScrollProfDown( L"profile_list_scroll_down" ),
							CTRLNAME_LogoutButton( L"logout_button" ),
							CTRLNAME_RenameProfileButton( L"RenameProfileButton" ),
							CTRLNAME_DeleteProfileButton( L"DeleteProfileButton" ),
							CTRLNAME_KeepLoggedInCheckBox( L"keep_logged_in_check" ),
							CTRLNAME_KeepLoggedInLabel( L"keep_logged_in_label" ),
							_pRenameProfileButton( 0 ),
							_pDeleteProfileButton( 0 ),
							m_pProfileList( 0 ),
							m_pCreateProfMsgBox( 0 ),
							m_pBadNameMsgBox( 0 ),
							m_pTryTutorialsMsgBox( 0 ),
							m_pRenameMsgBox( 0 ),
							m_pDeleteProfileMsgBox( 0 ),
							m_pCantCompleteMsgBox( 0 )
	{
		m_LayoutID = GameDefines::MS_ProfileSelection;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_ProfilesList, GUIDefines::CT_ScrollList, &m_pProfileList );
		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_CreateProfButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_SelectProfButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_LogoutButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_RenameProfileButton, GUIDefines::CT_Button, &_pRenameProfileButton );
		AddExpectedCtrl( CTRLNAME_DeleteProfileButton, GUIDefines::CT_Button, &_pDeleteProfileButton );
		AddExpectedCtrl( CTRLNAME_KeepLoggedInCheckBox, GUIDefines::CT_CheckBox, &_pKeepLoggedInCheckBox );
		AddExpectedCtrl( CTRLNAME_KeepLoggedInLabel, GUIDefines::CT_Label, &_pKeepLoggedInLabel );
	}

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// A message handler called when the layout is deactivated
	virtual void OnDeactivate();

	/// Update the layout
	virtual void Update( float32 frameTime );
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutViewStats
	\brief The class for the GUI layout to view profile stats
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutViewStats : public GameGUILayout
{
private:

	static const uint32 NUM_STAT_LABELS = 6;

	/// Control names
	const wchar_t* CTRLNAME_BackButton;
	const wchar_t* CTRLNAME_StatLabelNamePrefix;
	const wchar_t* CTRLNAME_StatLabelValuePrefix;
	const wchar_t* CTRLNAME_TitleLabel;
	const wchar_t* CTRLNAME_StatBtn_Main;
	const wchar_t* CTRLNAME_StatBtn_Game;
	const wchar_t* CTRLNAME_StatBtn_Mult;
	const wchar_t* CTRLNAME_StatBtn_Add;
	
	enum EStatBtn
	{
		SB_Main = 0,
		SB_Mult,
		SB_Add,
		SB_Game,
		SB_COUNT
	};

	/// The labels used for the statistic names
	GUICtrlLabel* m_pStatNameLabels[ NUM_STAT_LABELS ];

	/// The labels used for the statistic values
	GUICtrlLabel* m_pStatValueLabels[ NUM_STAT_LABELS ];

	/// The title label
	GUICtrlLabel* m_pTitleLabel;
	
	/// The current visible category
	EStatBtn m_CurCat;


	/// Clear the text
	void ClearLabels();

	/// A callback function for when the user presses a stat category button 
	static void StatCatBtnCallback( GUICtrlButton* pBtn );

	/// Get the button based on the selected type
	GUICtrlButton* GetCatButton( EStatBtn stat );

	/// Get the category for a button
	EStatBtn GetCatVal( GUICtrlButton* pBtn );

	/// Right align the stat name labels
	void RightAlignNames();

	void UpdateStats_Main();
	void UpdateStats_Game();
	void UpdateStats_Mult();
	void UpdateStats_Add();

public:

	/// The default constructor
	GUILayoutViewStats() : GameGUILayout(),
							CTRLNAME_BackButton( L"back_button" ),
							CTRLNAME_StatLabelNamePrefix( L"stat_namelabel_" ),
							CTRLNAME_StatLabelValuePrefix( L"stat_valuelabel_" ),
							CTRLNAME_TitleLabel( L"title_label" ),
							CTRLNAME_StatBtn_Main( L"stat_btn_main" ),
							CTRLNAME_StatBtn_Mult( L"stat_btn_mult" ),
							CTRLNAME_StatBtn_Add( L"stat_btn_add" ),
							CTRLNAME_StatBtn_Game( L"stat_btn_game" ),
							m_pTitleLabel( 0 ),
							m_CurCat( SB_Main )
	{
		m_LayoutID = GameDefines::MS_ViewStats;

		// Clear the label pointers
		for( uint32 labelIndex = 0; labelIndex < NUM_STAT_LABELS; ++labelIndex )
		{
			m_pStatNameLabels[ labelIndex ] = 0;
			m_pStatValueLabels[ labelIndex ] = 0;
		}

		// Add expected controls
		const uint32 STATNAME_BUFSIZE = 32;
		wchar_t szNameBuffer[STATNAME_BUFSIZE] = {0};
		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
		for( uint32 labelIndex = 0; labelIndex < NUM_STAT_LABELS; ++labelIndex )
		{
			swprintf( szNameBuffer, STATNAME_BUFSIZE, L"%s%u", CTRLNAME_StatLabelNamePrefix, labelIndex );
			AddExpectedCtrl( szNameBuffer, GUIDefines::CT_Label, &m_pStatNameLabels[labelIndex] );
			
			swprintf( szNameBuffer, STATNAME_BUFSIZE, L"%s%u", CTRLNAME_StatLabelValuePrefix, labelIndex );
			AddExpectedCtrl( szNameBuffer, GUIDefines::CT_Label, &m_pStatValueLabels[labelIndex] );
		}
		AddExpectedCtrl( CTRLNAME_TitleLabel, GUIDefines::CT_Label, &m_pTitleLabel );
		AddExpectedCtrl( CTRLNAME_StatBtn_Main, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_StatBtn_Mult, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_StatBtn_Add, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_StatBtn_Game, GUIDefines::CT_Button );
	}

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// Update the displayed stats based on the arithmetic type
	void UpdateStats( EStatBtn statType );
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutHelpMeLearn
	\brief The class for the help me learn GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutHelpMeLearn : public GameGUILayout
{
private:

	/// Control names
	const wchar_t* CTRLNAME_BackButton;
	const wchar_t* CTRLNAME_MainList;
	const wchar_t* CTRLNAME_HelpLabel;

	/// The list item indices
	static const uint32 LISTINDEX_IMPROVESKILLS = 1;
	static const uint32 LISTINDEX_PRACTICE = 2;
	static const uint32 LISTINDEX_TUTORIALS = 0;

	/// The main list
	GUICtrlList* m_pMainList;

	/// The help label
	GUICtrlLabel* m_pHelpLabel;

	/// The label rectangle
	Box2i m_HelpLabelBox;

	/// The current mouse over item index
	int32 m_CurMainListIndex;
	
	/// The message box alerting the user to log-in
	MsgBox* m_pMustLogInMsgBox;


	/// The callback for the main list
	static void MainListCB( uint32 selIndex, GUICtrlList* pList );

public:

	/// The default constructor
	GUILayoutHelpMeLearn() : GameGUILayout(),
							CTRLNAME_BackButton( L"back_button" ),
							CTRLNAME_MainList( L"main_list" ),
							CTRLNAME_HelpLabel( L"help_label" ),
							m_pMainList( 0 ),
							m_pHelpLabel( 0 ),
							m_CurMainListIndex( -1 ),
							m_pMustLogInMsgBox( 0 )
	{
		m_LayoutID = GameDefines::MS_HelpMeLearn;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_MainList, GUIDefines::CT_List, &m_pMainList );
		AddExpectedCtrl( CTRLNAME_HelpLabel, GUIDefines::CT_Label, &m_pHelpLabel );
	}

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// Update the layout
	virtual void Update( float32 frameTime );

	/// A message handler called when the layout is activated
	virtual void OnActivate();
	
	/// A message handler called when layout is closed
	virtual void OnDeactivate();
};

// This predeclaration is necessary for the friend tag in the the LAN lobby layout
class GUILayoutPreNetGame;

//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutLANLobby
	\brief The class for the LAN lobby GUI layout
*/
//-------------------------------------------------------------------------------------------------
//class GUILayoutLANLobby : public GameGUILayout
//{
//private:
//
//	friend class GUILayoutPreNetGame;
//
//	/// An active game in the list
//	struct ActiveGame
//	{
//		/// The host's name
//		std::wstring sCreatorName;
//
//		/// The game type
//		GameDefines::EGameType m_GameType;
//
//		/// The host's play level
//		GameDefines::EGameplayDiffLevel m_HostDiff;
//
//		/// The port and IP to connect to
//		uint16 m_ServerPort;
//		IPAddress m_ServerIP;
//
//		uint32 m_ListItemIndex;
//
//		/// The time since the last message received from this game
//		float m_LastMsgHeard;
//
//		/// The default constructor
//		ActiveGame() : m_ServerPort( 0 ),
//						m_LastMsgHeard( 0.0f ),
//						m_ListItemIndex( 12345 ),
//						m_GameType( GameDefines::GT_Error ),
//						m_HostDiff( GameDefines::GPDL_5 )
//		{}
//	};
//	
//	/// The active games
//	typedef std::list< ActiveGame > ActiveGameList;
//	ActiveGameList m_ActiveGames;
//
//	/// Control names
//	const wchar_t* CTRLNAME_BackButton;
//	const wchar_t* CTRLNAME_CreateGame;
//	const wchar_t* CTRLNAME_JoinButton;
//	const wchar_t* CTRLNAME_GamesList;
//	const wchar_t* CTRLNAME_GamesList_ScrollUp;
//	const wchar_t* CTRLNAME_GamesList_ScrollDown;
//	
//	/// The network connection for finding network games
//	NetConnLAN* m_pGameListener;
//
//	/// The list of games
//	GUICtrlScrollList* m_pGamesList;
//
//	/// The message box for an error
//	MsgBox* m_pMsgBox;
//
//	
//	/// Close the LAN connection
//	void CloseConnection();
//
//	/// The callback to create a game
//	static void CreateGameCB( GUICtrlButton* );
//
//	/// The callback for the join game button
//	static void JoinGameCB( GUICtrlButton* pButton );
//
//	/// Add a game to the known list
//	void AddActiveGame( ActiveGame newGame );
//
//	/// Find an active game by IP address
//	ActiveGameList::iterator FindActiveGame( IPAddress ip, uint16 port );
//
//	/// Remove an active game
//	void RemoveActiveGame( IPAddress ip, uint16 port );
//
//	/// Clear the active games
//	void ClearActiveGames();
//
//public:
//
//	/// The default constructor
//	GUILayoutLANLobby() : GameGUILayout(),
//								CTRLNAME_BackButton( L"back_button" ),
//								CTRLNAME_CreateGame( L"create_button" ),
//								CTRLNAME_JoinButton( L"join_button" ),
//								CTRLNAME_GamesList( L"games_list" ),
//								CTRLNAME_GamesList_ScrollUp( L"games_list_scrollup" ),
//								CTRLNAME_GamesList_ScrollDown( L"games_list_scrolldown" ),
//								m_pGameListener( 0 ),
//								m_pGamesList( 0 ),
//								m_pMsgBox( 0 )
//	{
//		m_LayoutID = GameDefines::MS_LANLobby;
//
//		// Add expected controls
//		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_CreateGame, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_JoinButton, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_GamesList, GUIDefines::CT_ScrollList, &m_pGamesList );
//		AddExpectedCtrl( CTRLNAME_GamesList_ScrollUp, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_GamesList_ScrollDown, GUIDefines::CT_Button );
//	}
//
//	/// The destructor
//	~GUILayoutLANLobby(){ CloseConnection(); }
//
//	/// A message handler called when the layout is activated
//	virtual void OnActivate();
//
//	/// A message handler called when the layout is deactivated
//	virtual void OnDeactivate();
//
//	/// Link the control objects to their corresponding object found in the layout
//	virtual void HookupCtrls();
//
//	/// Update the layout
//	virtual void Update( float32 frameTime );
//
//	/// Add a game to the list
//	void AddGame( ActiveGame& newGame );
//};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutPreNetGame
	\brief The class for the pre-network-game GUI layout
*/
//-------------------------------------------------------------------------------------------------
//class GUILayoutPreNetGame : public GameGUILayout
//{
//private:
//
//	friend class GUILayoutLANLobby;
//
//	/// The data used by the player when he/she is the host
//	struct ServerData
//	{
//		// The client name
//		std::wstring m_sClientName;
//
//		/// The LAN connection to alert clients of this game
//		NetConnLAN* m_pLANConn;
//
//		/// The game running message
//		NetMsgGameRunning m_GameRunningMsg;
//
//		/// The default constructor
//		ServerData() : m_pLANConn( 0 )
//		{
//			Clear();
//		}
//
//		/// Clear the data
//		void Clear();
//	};
//
//	/// The data used by the player when he/she is the client
//	struct ClientData
//	{
//		ClientData() : m_IsReady( false )
//		{}
//
//		bool m_IsReady;
//
//		void Clear()
//		{
//			m_IsReady = false;
//		}
//	};
//
//	/// The data about the other player
//	struct OtherPlayerInfo
//	{
//		OtherPlayerInfo() : isReady( false ),
//			m_Diff( GameDefines::GPDL_4 )
//		{}
//
//		std::wstring sName;
//		bool isReady;
//		GameDefines::EGameplayDiffLevel m_Diff;
//	};
//
//
//	/// Control names
//	const wchar_t* CTRLNAME_BackButton;
//	const wchar_t* CTRLNAME_ChatBox;
//	const wchar_t* CTRLNAME_ChatEntryField;
//	const wchar_t* CTRLNAME_LocalPlayerNameLable;
//	const wchar_t* CTRLNAME_ClientNameLabel;
//	const wchar_t* CTRLNAME_ReadyHelpText;
//	const wchar_t* CTRLNAME_ReadyHelpText2;
//	const wchar_t* CTRLNAME_SendTextButton;
//	const wchar_t* CTRLNAME_ReadyPlayButton;
//	const wchar_t* CTRLNAME_VersusLabel;
//	const wchar_t* CTRLNAME_HostDiffLabel;
//	const wchar_t* CTRLNAME_ClientDiffLabel;
//	const wchar_t* CTRLNAME_DiffUpBtn;
//	const wchar_t* CTRLNAME_DiffDownBtn;
//
//	/// The network connection for connection to a game
//	NetConnTCP* m_pGameConn;
//
//	/// The message send timer
//	float32 m_TimeSinceSend;
//
//	/// The server data
//	ServerData m_ServerData;
//
//	/// The client data
//	ClientData m_ClientData;
//
//	/// The game type
//	GameDefines::EGameType m_GameType;
//
//	/// This player's difficulty level
//	GameDefines::EGameplayDiffLevel m_LocalDiff;
//
//	OtherPlayerInfo m_OpponentInfo;
//
//	// The name labels
//	GUICtrlLabel* m_pHostNameLabel;
//	GUICtrlLabel* m_pClientNameLabel;
//	GUICtrlLabel* m_pVersusLabel;
//	GUICtrlLabel* m_pHostDiffLabel;
//	GUICtrlLabel* m_pClientDiffLabel;
//
//
//	/// The difficulty change callback
//	static void OnDiffChangeCB( GUICtrlButton* pButton );
//
//	/// Go back to the LAN lobby
//	static void BackToLobbyCB( GUICtrlButton* );
//
//	/// The play/ready button callback
//	static void PlayButtonCB( GUICtrlButton* pButton );
//
//	/// Update the difficulty level labels
//	void SetDiffLabels();
//
//public:
//
//	bool m_IsLANGame;
//
//	/// If this computer is the host or client
//	bool m_IsHost;
//
//	/// The default constructor
//	GUILayoutPreNetGame() : GameGUILayout(),
//								CTRLNAME_BackButton( L"back_button" ),
//								CTRLNAME_ChatBox( L"chat_box" ),
//								CTRLNAME_ChatEntryField( L"chat_entry_field" ),
//								CTRLNAME_LocalPlayerNameLable( L"player_name_label" ),
//								CTRLNAME_ClientNameLabel( L"opponent_name_label" ),
//								CTRLNAME_ReadyPlayButton( L"ready_play_button" ),
//								CTRLNAME_SendTextButton( L"send_chat_button" ),
//								CTRLNAME_ReadyHelpText( L"ready_help_label" ),
//								CTRLNAME_ReadyHelpText2( L"ready_help_label2" ),
//								CTRLNAME_VersusLabel( L"versus_label" ),
//								CTRLNAME_HostDiffLabel( L"host_diff_label" ),
//								CTRLNAME_ClientDiffLabel( L"client_diff_label" ),
//								CTRLNAME_DiffUpBtn( L"diff_up_btn" ),
//								CTRLNAME_DiffDownBtn( L"diff_down_btn" ),
//								m_pGameConn( 0 ),
//								m_IsHost( false ),
//								m_TimeSinceSend( 0.0f ),
//								m_GameType( GameDefines::GT_PrimeTime ),
//								m_LocalDiff( GameDefines::GPDL_4 ),
//								m_pClientNameLabel( 0 ),
//								m_pHostNameLabel( 0 ),
//								m_pVersusLabel( 0 ),
//								m_pHostDiffLabel( 0 ),
//								m_pClientDiffLabel( 0 )
//	{
//		m_LayoutID = GameDefines::MS_PreNetGame;
//
//		// Add expected controls
//		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_ChatBox, GUIDefines::CT_Label );
//		AddExpectedCtrl( CTRLNAME_ChatEntryField, GUIDefines::CT_TextBox );
//		AddExpectedCtrl( CTRLNAME_LocalPlayerNameLable, GUIDefines::CT_Label, &m_pHostNameLabel );
//		AddExpectedCtrl( CTRLNAME_ClientNameLabel, GUIDefines::CT_Label, &m_pClientNameLabel );
//		AddExpectedCtrl( CTRLNAME_ReadyPlayButton, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_ReadyHelpText, GUIDefines::CT_Label );
//		AddExpectedCtrl( CTRLNAME_ReadyHelpText2, GUIDefines::CT_Label );
//		AddExpectedCtrl( CTRLNAME_SendTextButton, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_VersusLabel, GUIDefines::CT_Label, &m_pVersusLabel );
//		AddExpectedCtrl( CTRLNAME_HostDiffLabel, GUIDefines::CT_Label, &m_pHostDiffLabel );
//		AddExpectedCtrl( CTRLNAME_ClientDiffLabel, GUIDefines::CT_Label, &m_pClientDiffLabel );
//		AddExpectedCtrl( CTRLNAME_DiffUpBtn, GUIDefines::CT_Button );
//		AddExpectedCtrl( CTRLNAME_DiffDownBtn, GUIDefines::CT_Button );
//	}
//
//	/// The destructor
//	virtual ~GUILayoutPreNetGame();
//
//	/// Link the control objects to their corresponding object found in the layout
//	virtual void HookupCtrls();
//
//	/// Update the layout
//	virtual void Update( float32 frameTime );
//
//	/// A message handler called when the layout is activated
//	virtual void OnActivate();
//
//	/// A message handler called when the layout is deactivated
//	virtual void OnDeactivate();
//
//	/// Retrieve the TCP connection and clear the variable
//	NetConnTCP* TakeTCPConn()
//	{
//		NetConnTCP* pRetConn = m_pGameConn;
//		m_pGameConn = 0;
//		return pRetConn;
//	}
//
//	/// Set the host name
//	void SetHostName( const wchar_t* szName );
//};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutPracticeSetup
	\brief The class for the practice mode options GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutPracticeSetup : public GameGUILayout
{
private:

	/// Control names
	const wchar_t* CTRLNAME_BackButton;
	const wchar_t* CTRLNAME_PlayButton;
	const wchar_t* CTRLNAME_MinProdTextBox;
	const wchar_t* CTRLNAME_MaxProdTextBox;
	const wchar_t* CTRLNAME_ForceFactorCheckBox;
	const wchar_t* CTRLNAME_MaxNumGenProdLabel;
	const wchar_t* CTRLNAME_MinNumFactorsLabel;
	const wchar_t* CTRLNAME_MinNumFactorsUpBtn;
	const wchar_t* CTRLNAME_MinNumFactorsDownBtn;
	const wchar_t* CTRLNAME_PrimeChecksPreLabel;
	const wchar_t* CTRLNAME_PrimeReqChecksPreLabel;
	
	/// The minimum product text box
	GUICtrlTextBox* m_pMinProductTextBox;

	/// The maximum product text box
	GUICtrlTextBox* m_pMaxProductTextBox;

	/// The number of generatable products label
	GUICtrlLabel* m_pNumProductsLabel;

	/// The minimum number of factors label
	GUICtrlLabel* m_pMinNumFactorsLabel;

	/// The prime factor selection blocks
	GUICtrlCheckBox* m_pPrimeChecks[ GameDefines::NUM_PRIMES ];

	/// The required prime factor selection blocks
	GUICtrlCheckBox* m_pReqPrimeChecks[ GameDefines::NUM_PRIMES ];

	/// Whether or not the player wants a common required factor in all of the products
	bool m_IsFactorRequired;

	/// The required prime number index
	uint32 m_ReqFactorIndex;

	/// The current number of products availabe that meet the current settings
	uint32 m_CurNumProds;

	/// The minimum product value
	int32 m_MinProductValue;

	/// The maximum product value
	int32 m_MaxProductValue;

	/// The minimum number of factors
	uint32 m_MinNumFactors;

	/// The message box warning of invlaid parameters
	MsgBox* m_pMsgBox;

	/// The player profile subset for game settings
	ProfileSubsetSettings* m_pSettingsProfile;


	/// The callback for when the player presses play
	static void PlayButtonCB( GUICtrlButton* pButton );

	/// The callback for when the player presses the minimum number of factors scroll buttons
	static void MinNumFactorsChangeCB( GUICtrlButton* pButton );

	/// The callback for when the player changes the minimum value callback
	static void MinMaxValueChangeCB( GUICtrlTextBox* pTextBox );

	/// The callback for when the player selects an allowable prime factor check box
	static void AllowableFactorCB( GUICtrlCheckBox* pCheckBox );

	/// The callback for when the player selects a required prime factor check box
	static void ReqFactorCB( GUICtrlCheckBox* pCheckBox );

	/// Update the minimum number of factors label
	void UpdateMinNumFactorLabel();

	/// Update the number of generatable products
	void UpdateNumberOfProducts();

	/// Get the settings into a product filter
	GameDefines::ProductSubsetFilter GetFilterFromUI() const;

	/// Set the settings from a product filter
	void SetUIFromFilter( const GameDefines::ProductSubsetFilter& prodFilter );

	/// Update which required prime factors are visible
	void UpdateVisibleReqFactors();

public:

	/// The default constructor
	GUILayoutPracticeSetup();

	/// Update the layout
	virtual void Update( float32 frameTime );

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// A message handler called when the layout is deactivated
	virtual void OnDectivate();

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutTutorials
	\brief The class for the tutorial selection game GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutTutorials : public GameGUILayout
{
private:
	static const uint32 LISTIDX_PRIMETIME = 0;
	static const uint32 LISTIDX_CEILING = 1;
	static const uint32 LISTIDX_COMBOS = 2;
	static const uint32 LISTIDX_ADDITION = 3;
	static const uint32 LISTIDX_FRACTIONS = 4;

	/// Control names
	const wchar_t* CTRLNAME_BackButton;
	const wchar_t* CTRLNAME_TutorialList;

	/// The tutorial list
	GUICtrlList* m_pTutorialList;

	
	/// The callback for when a tutorial is selected from the list
	static void ListSelCB( uint32 selIndex, GUICtrlList* );

public:

	/// The default constructor
	GUILayoutTutorials() : GameGUILayout(),
								CTRLNAME_BackButton( L"back_button" ),
								CTRLNAME_TutorialList( L"tutorials_list" ),
								m_pTutorialList( 0 )
	{
		m_LayoutID = GameDefines::MS_Tutorials;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_TutorialList, GUIDefines::CT_List, &m_pTutorialList );
	}

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutLearningAssistance
	\brief The class for the learning assistance game GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutLearningAssistance : public GameGUILayout
{
private:

	struct AnalyzedData
	{
		AnalyzedData()
		{
			totalSecsPlayed = 0;
			avgDiffLevel = GameDefines::GPDL_Pre3;
			numSessionsConsidered = 0;

			for( uint32 primeIndex = 0; primeIndex < GameDefines::NUM_PRIMES; ++primeIndex )
			{
				primeUsageCounts[ primeIndex ] = 0;
				primeErrorCounts[ primeIndex ] = 0;
			}

			pLastAnalyzedSession = 0;
			pLastImprovementSession = 0;
			avgCombo = 0;
			avgMaxCombo = 0;
			secretDiffPlayed = 0;
		}

		float32 totalSecsPlayed;
		GameDefines::EGameplayDiffLevel avgDiffLevel;
		uint32 numSessionsConsidered;
		uint32 primeUsageCounts[ GameDefines::NUM_PRIMES ];
		uint32 primeErrorCounts[ GameDefines::NUM_PRIMES ];

		/// The last played session
		const GameSessionStatsMult* pLastAnalyzedSession;

		/// The last played "improve my skills" session
		const GameSessionStatsMult* pLastImprovementSession;

		/// The average combo size
		uint32 avgCombo;

		/// The average max combo size
		uint32 avgMaxCombo;

		/// The secret difficulty level played
		int32 secretDiffPlayed;
	};
	
	/// Control names
	const wchar_t* CTRLNAME_BackButton;
	const wchar_t* CTRLNAME_InstructionText;
	const wchar_t* CTRLNAME_PlayButton;

	/// The tutorial list
	GUICtrlLabel* m_pInstLabel;

	/// The play button
	GUICtrlButton* m_pPlayButton;

	/// The bounding box for the instruction text
	Box2i m_InstBoundBox;

	AnalyzedData _playersAnalyzedData;

	GameDefines::ProductSubsetFilter _productFilter;
	
	/// Analyze the play data
	void GatherData( AnalyzedData& data );

	/// Test if a session is valid for consideration
	bool IsSessionValid( const GameSessionStatsMult* pSession );

	/// Set and word-wrap the instruction text
	void SetInstText( const wchar_t* szText );

	/// Setup a session to work on improvement of a prime
	bool Setup_PrimeImpv( const AnalyzedData& data );

	/// Setup a session to improve combo length
	bool Setup_ComboSizeImpv( const AnalyzedData& data );

	/// Tell the user to try a harder level
	bool Setup_IncreaseDiff( const AnalyzedData& data );

	/// The callback for when the player presses the 'Play' button
	static void OnPlayPress( GUICtrlButton* pButton );

public:

	/// The default constructor
	GUILayoutLearningAssistance() : GameGUILayout(),
								CTRLNAME_BackButton( L"back_button" ),
								CTRLNAME_InstructionText( L"inst_label" ),
								CTRLNAME_PlayButton( L"play_button" ),
								m_pInstLabel( 0 ),
								m_pPlayButton( 0 )
	{
		m_LayoutID = GameDefines::MS_LearningAssistance;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_BackButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_InstructionText, GUIDefines::CT_Label, &m_pInstLabel );
		AddExpectedCtrl( CTRLNAME_PlayButton, GUIDefines::CT_Button, &m_pPlayButton );
	}

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutGameResults
	\brief The class for the post-game results GUI layout
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutGameResults : public GameGUILayout
{
private:
	
	/// Control names
	const wchar_t* CTRLNAME_DoneButton;
	const wchar_t* CTRLNAME_DefaultLabel;
	const wchar_t* CTRLNAME_TitleLabel;
	
	/// The default label that is used as reference for position and font
	GUICtrlLabel* m_pDefLabel;

	/// The default label
	GUICtrlLabel* m_pTitleLabel;

	typedef std::pair<std::wstring,std::wstring> StringPair;
	typedef std::list< StringPair > StatList;

	/// The list of labels added to the layout
	ControlList _addedLabels;

	/// Clear any labels added to display stats
	void ClearAddedLabels();

	/// The callback invoked when the player presses the 'Done' button
	static void OnDonePress( GUICtrlButton* pButton );

	/// Create the labels to display the stats
	void CreateLabels( const StatList& stats, const wchar_t* szGameTypeName );

public:

	/// The default constructor
	GUILayoutGameResults() : GameGUILayout(),
								CTRLNAME_DoneButton( L"done_button" ),
								CTRLNAME_DefaultLabel( L"default_label" ),
								CTRLNAME_TitleLabel( L"title_label" ),
								m_pDefLabel( 0 ),
								m_pTitleLabel( 0 )
	{
		m_LayoutID = GameDefines::MS_GameResults;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_DoneButton, GUIDefines::CT_Button );
		AddExpectedCtrl( CTRLNAME_DefaultLabel, GUIDefines::CT_Label, &m_pDefLabel );
		AddExpectedCtrl( CTRLNAME_TitleLabel, GUIDefines::CT_Label, &m_pTitleLabel );
	}

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// Display the results for a multiplication game
	void DisplayResults( const GameSessionStatsMult& session );

	/// Display the results for a addition game
	void DisplayResults( const GameSessionStatsAdd& session );

	/// Display the results for a fractions game
	void DisplayResults( const GameSessionStatsFractions& session );

	/// A message handler called when the layout is deactivated
	virtual void OnDeactivate();
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayoutDemoExit
	\brief The class for the layout that is displayed when the player tries to exit the game
*/
//-------------------------------------------------------------------------------------------------
class GUILayoutDemoExit : public GameGUILayout
{
private:

	/// Control names
	const wchar_t* CTRLNAME_TimeLeftLabel;
	const wchar_t* CTRLNAME_ExitButton;

	/// The time left label
	GUICtrlLabel* m_pTimeLeftLabel;

	/// The button to exit the application
	GUICtrlButton* m_pExitButton;

	/// The flag to determine if the screen is counting down
	bool m_IsCountingDown;


	/// The callback invoked when the player presses the 'Exit' button
	static void OnExitPress( GUICtrlButton* );

public:

	/// The default constructor
	GUILayoutDemoExit() : GameGUILayout(),
							CTRLNAME_TimeLeftLabel( L"time_left_label" ),
							CTRLNAME_ExitButton( L"exit_button" ),
							m_pTimeLeftLabel( 0 ),
							m_pExitButton( 0 ),
							m_IsCountingDown( true )
	{
		m_LayoutID = GameDefines::MS_DemoExit;

		// Add expected controls
		AddExpectedCtrl( CTRLNAME_TimeLeftLabel, GUIDefines::CT_Label, &m_pTimeLeftLabel );
		AddExpectedCtrl( CTRLNAME_ExitButton, GUIDefines::CT_Button, &m_pExitButton );
	}

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// Update the layout
	virtual void Update( float32 frameTime );
};


#endif // __GUILayout_Game_h