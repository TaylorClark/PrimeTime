//=================================================================================================
/*!
	\file ApplicationSFML.h
	Application Project
	SFML Application Class Header
	\author Taylor Clark
	\Date August 19, 2007

	This header contains the definition for the application object that is used to encapsulate
	the activity of the executable.
*/
//=================================================================================================

#pragma once
#ifndef __ApplicationSFML_h
#define __ApplicationSFML_h

#include "ApplicationBase.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "../Graphics2D/GraphicsMgr.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class ApplicationSFML
	\brief Represents the life cycle of the application when using SFML as the grapics and window
	API.
*/
//-------------------------------------------------------------------------------------------------
class ApplicationSFML : public ApplicationBase
{
private:

	/// The window handle
	sf::RenderWindow* _pRenderWindow;

	sf::Clock _clock;

    GraphicsMgrBase::EDisplayMode _activeDisplayMode;
    GraphicsMgrBase::EDisplayMode _targetDisplayMode;

    /// The amount of time, in seconds, to display what display mode we're using
    float _timeToShowDisplayMode;

	/// Toggle between windowed and full-screen
	void ToggleFullScreen() {}

	/// Draw the scene
	void Draw();

	/// Handle a SFML app event
	void HandleEvent( const sf::Event& curEvent );

	/// Close the game
	virtual void Term();

    void GoToTargetDisplayMode();

	/// The default constructor
	ApplicationSFML() : _pRenderWindow( 0 ),
                        _activeDisplayMode( GraphicsMgrBase::DM_NormalFill ),
                        _targetDisplayMode( GraphicsMgrBase::DM_NormalFill ),
                        _timeToShowDisplayMode( 0.0f )
	{}

public:

	/// Initialize the game
	bool Init( const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg );

	/// Initialize the game
	static bool InitApp( const TCBase::ParamList& cmdLineParams, const wchar_t* szCustomText, const uint8* pCustomPicData, const wchar_t* szCustomPicMsg );

    /// Change the display mode
    void SetDisplayMode( GraphicsMgrBase::EDisplayMode displayMode );

    /// Cycle through the available display modes
    virtual void CycleToNextDisplayMode();

	/// Run the game
	virtual void Run();

	const sf::RenderWindow* GetRenderWindow() const { return _pRenderWindow; }
#ifdef _DEBUG
	/// Run the game
	void Run_Dbg() {}
#endif
};

#endif // __ApplicationSFML_h