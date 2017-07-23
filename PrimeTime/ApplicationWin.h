//=================================================================================================
/*!
	\file ApplicationWin.h
	Application Project
	Windows Application Class Header
	\author Taylor Clark
	\Date August 19, 2007

	This header contains the definition for the application object that is used to encapsulate
	the activity of the executable.
*/
//=================================================================================================

#pragma once
#ifndef __ApplicationWin_h
#define __ApplicationWin_h

#include "ApplicationBase.h"
#include <windows.h>
#include "../Graphics2D/GraphicsMgr.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class ApplicationWin
	\brief Represents the life cycle of the application
*/
//-------------------------------------------------------------------------------------------------
class ApplicationWin : ApplicationBase
{
private:

	/// The window handle
	HWND m_hWnd;

	/// The application instance
	HINSTANCE m_hInstance;

	
	/// Toggle between windowed and full-screen
	void ToggleFullScreen();

	/// Draw the scene
	void Draw();

	/// Register a window class
	bool RegisterWindowClass();

	/// The Windows message handler function
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/// Close the game
	virtual void Term();

	/// The default constructor
	ApplicationWin() : m_hWnd( 0 ),
					m_hInstance( 0 )
	{}

public:

	/// Initialize the game
	bool Init( HINSTANCE hInstance, const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg );

	/// Initialize the game
	static bool InitApp( HINSTANCE hInstance, const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg );

	/// Run the game
	virtual void Run();

    /// Change the display mode
    void SetDisplayMode( GraphicsMgrBase::EDisplayMode displayMode ){UNREFERENCED_PARAMETER(displayMode);}

    /// Cycle through the available display modes
    virtual void CycleToNextDisplayMode(){}

#ifdef _DEBUG
	/// Run the game
	void Run_Dbg();
#endif

	/// Get the window
	HWND GetHWnd() const { return m_hWnd; }
};

#endif // __ApplicationWin_h