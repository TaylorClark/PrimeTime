//=========4========================================================================================
/*!
	\file GameGUILayout.h
	Game Library
	Base Game GUI Layout
	\author Taylor Clark
	\date July 10, 2006

	This file contains the definition for base class of GUI layouts for Prime Time.
*/
//=================================================================================================

#pragma once
#ifndef __GameGUILayout_h
#define __GameGUILayout_h

#include "GUI/GUILayout.h"
#include "GUI/GUIDefines.h"
#include "GameDefines.h"
#include "Base/MsgLogger.h"
#include <list>

#ifdef _DEBUG
#include "GUI/GUIControl.h"
#endif

class GUIControl;


//-------------------------------------------------------------------------------------------------
/*!
	\class GameGUILayout
	\brief The base class for Prime Time GUI layouts
*/
//-------------------------------------------------------------------------------------------------
class GameGUILayout : public GUILayout
{
private:

	/// The channel number sprite
	TCFontHndl m_ChannelNumFont;

	#ifdef _DEBUG
	/// A structure containing the data for an expected control
	virtual void OnShowCtrlsActivate()
	{
		std::wstring sMsg( GameDefines::GetMenuScreenDesc( (GameDefines::EMenuScreens)m_LayoutID) );
		sMsg += L" expcts the following controls:";
		MsgLogger::Get().Output( MsgLogger::MI_Note, sMsg.c_str() );
		
		// Go through the expected controls
		for( ExpecCtrlList::iterator iterCtrl = m_ExpectedCtrls.begin(); iterCtrl != m_ExpectedCtrls.end(); ++iterCtrl )
		{
			sMsg = L"A ";
			sMsg += GUIControl::GetCtrlTypeName( iterCtrl->type );
			sMsg += L" named ";
			sMsg += iterCtrl->sCtrlName;
			MsgLogger::Get().Output( MsgLogger::MI_Note, sMsg.c_str() );
		}
	}
#endif

protected:

	/// The time since the layout was activated
	float m_TimeSinceActivate;

	/// The default constructor, protected so you must use the create function
	GameGUILayout() : m_TimeSinceActivate( 0.0f )
	{}

public:

	/// The destructor
	virtual ~GameGUILayout(){}

public:

	/// Load the GUI from file
	static bool LoadLayouts( const std::wstring& sFilePath );

	/// Update the layout
	virtual void Update( float32 frameTime );

	/// Handle a GUI message
	virtual bool HandleMessage( GUIMessage* pMsg );

	/// Draw the layout
	virtual void Draw() const;

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// Reset the needed data if the graphics system has been reset
	virtual void OnGraphicsRefresh();

	/// Load or save a GUI layout
	static bool TransferGUIResourceData( Serializer& serializer );

	/// Create a layout
	static GameGUILayout* CreateGameLayout( GameDefines::EMenuScreens layoutID, bool onlyLoadKnown = false );
};

#endif // __GameGUILayout_h