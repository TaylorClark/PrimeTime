//=================================================================================================
/*!
	\file ApplicationBase.h
	Application Project
	Windows Application Class Header
	\author Taylor Clark
	\Date March 8, 2010

	This header contains the base class definition for the application object that is used to
	encapsulate	the activity of the executable.
*/
//=================================================================================================

#pragma once
#ifndef __ApplicationBase_h
#define __ApplicationBase_h

#include "Base/Types.h"
#include <string>
#include "Base/FileFuncs.h"
#include "GameSettings.h"


#ifdef WINDOWS
class ApplicationWin;
#elif defined SFML
class ApplicationSFML;
#endif

//-------------------------------------------------------------------------------------------------
/*!
	\class ApplicationBase
	\brief Represents the base class for the life cycle of the application
*/
//-------------------------------------------------------------------------------------------------
class ApplicationBase
{
protected:

	/// The application exit flag
	bool m_IsAppDone;

	/// A flag indicating when the game is run from a read-only source, like a CD
	bool m_IsRunningOffCD;

	/// The flag that is set to indicate a full-screen/windowed toggle
	bool m_ToggleFullscreen;

	/// If the window is minimized
	bool m_IsWinMinimized;

	static ApplicationBase* _pAppInstance;

	
	/// Convert a title screen message into its displayable form
	static std::wstring DecodeCustomText( const wchar_t* szText );

	/// Close the game
	virtual void Term();

	static bool InitGameMgrs( const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg );

	void InitFromIni();

	static std::wstring s_resourcePath;

public:

	ApplicationBase() : m_IsAppDone( false ),
						m_IsRunningOffCD( false ),
						m_ToggleFullscreen( false ),
						m_IsWinMinimized( false )
	{
	}

	/// Get the one and only application instance
	static ApplicationBase* Get()
	{
		return _pAppInstance;
	}

	/// The descructor, virtual so derived classes destructors are called
	virtual ~ApplicationBase() {}

	/// Run the game
	virtual void Run() = 0;

	/// Set the application to exit
	void SetAppToExit() { m_IsAppDone = true; }

	static void TermApp();

    virtual void CycleToNextDisplayMode() = 0;

	static void SendAppConnectMessage(const wchar_t* szMsg);

	/// Get if the game is being run from the CD
	bool IsRunningOffCD() const { return m_IsRunningOffCD; }

	void SetAutoLoginProfile( const wchar_t* szProfileName );

	static std::wstring GetResourcePath();

	static uint64 GameKey;

	/// The settings that are persisted between each time the game is run
	GameSettings Settings;
};

#endif //__ApplicationBase_h
