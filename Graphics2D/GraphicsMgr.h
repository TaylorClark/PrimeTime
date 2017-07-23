//=================================================================================================
/*!
	\file GraphicsMgr.h
	2D Graphics Engine
	Graphics Manager Header
	\author Taylor Clark

	This file contains the definition for the graphics manager class.
*/
//=================================================================================================

#pragma once
#ifndef __GraphicsMgr_h
#define __GraphicsMgr_h

#include "Base/Types.h"
#include <list>
#include "GraphicsDefines.h"
#include "Math/Vector2i.h"

class TCImage;
class TCFont;
class RefSprite;
class DataBlock;
class Box2i;
class Point2i;
struct CachedFontDraw;


//-------------------------------------------------------------------------------------------------
/*!
	\class GraphicsMgrBase
	\brief The singleton class for graphics related functionality.

	This class defines the manager for all graphics related functionality.  It follows the
	singleton pattern so that there is only one instance of the class. At one time this class was
	extracted into a graphics DLL so that is why GraphicsMgrBase is a shell of the class and
	GraphicsMgr is the implemented class.
*/
//-------------------------------------------------------------------------------------------------
class GraphicsMgrBase
{
public:

    enum EDisplayMode
    {
        DM_NormalFill,
        DM_WideScreen,
        DM_CenteredDesktop,
        DM_Windowed
    };

protected:


    EDisplayMode _activeDisplayMode;

public:
	
    static Vector2i DesktopDims;

    static GraphicsMgrBase& Get();
	
    /// The default constructor to initialize values
    GraphicsMgrBase() : _activeDisplayMode( DM_NormalFill )
    {
    }

	/// The destructor, virtual to ensure derived classes are freed properly
	virtual ~GraphicsMgrBase(){}

	/// Initialize the graphics manager for use
	virtual bool Init( void* hWnd, bool startFullScreen ) = 0;


	/// Draw text
	void DrawFontText( const TCFont* pFont, const wchar_t* szText, const Point2i& destPos, uint32 colorTint = 0xFFFFFFFF );

	/// Draw text
	void DrawFontTextClipped( const TCFont* pFont, const wchar_t* szText, const Box2i& destRect );

	/// Draw text
	void DrawFontTextClipped( const TCFont* pFont, const wchar_t* szText, const Point2i& destPos, const Box2i& clipRect );

	/// Cache font text that is to be drawn
	CachedFontDraw CacheFontText( TCFont* pFont, const wchar_t* szText, const Box2i& destRect, float32 fontScale = 1.0f );

	/// Draw text
	virtual void DrawCachedFontText( const CachedFontDraw& cachedText, uint32 colorTint = 0xFFFFFFFF ) = 0;

	/// Draw an image
	virtual void DrawImage( const Point2i& destPos, const TCImage* pImage, const Box2i& srcRect, uint32 colorTint = 0xFFFFFFFF ) = 0;

	/// Draw an image
	virtual void DrawImageEx( const Box2i& destRect, const TCImage* pImage, const Box2i& srcRect, int32 fxFlags, uint32 colorTint = 0xFFFFFFFF ) = 0;

	/// Draw a sprite
	void DrawSprite( const Point2i& destPos, const RefSprite* const pSprite );

	/// Draw a sprite with effects
	void DrawSpriteScaled( Box2i destRect, const RefSprite* const pSprite, int32 fxFlags = 0 );

	/// Draw a sprite
	void DrawSpriteUnscaled( Box2i destRect, const RefSprite* const pSprite );

	/// Draw a sprite with effects
	void DrawSprite( const Point2i& destPos, const RefSprite* const pSprite, int32 fxFlags );

	/// Draw a rectangle
	virtual bool DrawRect( const Box2i& rect, uint32 lineColor ) = 0;

	/// Fill a rectangular area
	virtual bool FillRect( const Box2i& rect, uint32 fillColor ) = 0;


    ///////////////////////////////////////////////////////////////////////////////////////////////
    //  GraphicsMgrSFML::GetNextDisplayMode  Static Public
    ///
    /// Get the next display mode
    ///////////////////////////////////////////////////////////////////////////////////////////////
    EDisplayMode GetNextDisplayMode( EDisplayMode displayMode )
    {
        // Cheat a bit and increment an enum
        if( displayMode == DM_Windowed )
            displayMode = DM_NormalFill;
        else
            displayMode = (EDisplayMode)((int)displayMode + 1);

        // Only allow wide screen if the desktop is actually wide
        if( displayMode == DM_WideScreen )
        {
            if( DesktopDims.x <= DesktopDims.y )
                displayMode = DM_CenteredDesktop;
        }

        return displayMode;
    }

    /// Set the display mode
    virtual void SetDisplayMode( EDisplayMode displayMode ) = 0;


	/// Apply an effect to an image
	virtual bool ApplyEffect( TCImage* pImage, const Box2i& rect, GraphicsDefines::ModPixelCB pixelCB ) = 0;

	/// Set a temporary render target
	virtual bool SetTempRenderTarget( TCImage* pImage ) = 0;

	/// Clear any temporary render targets and return to using the back buffer
	virtual void ClearTempRenderTarget() = 0;


	/// Load an image from memory
	virtual TCImage* LoadImageFromMemory( uint32 resID, DataBlock* pImageDataBlock ) = 0;

	/// Load an image from memory
	TCFont* LoadFontFromMemory( uint32 resID, DataBlock* pImageDataBlock );

	/// Load an image from memory
	RefSprite* LoadSpriteFromMemory( uint32 resID, DataBlock* pImageDataBlock );

	/// A helper method to create a single-frame sprite for an image
	RefSprite* CreateSpriteForImage( TCImage* pImage );


	/// Reload the image data for a resource
	virtual bool ReloadImageData( TCImage* pImage, DataBlock* pImageDataBlock ) = 0;

	/// Recreate the DirectDraw surface and load the image data onto it
	virtual bool RecreateImageData( TCImage* pImage, DataBlock* pImageDataBlock ) = 0;

	/// Free the surface data for a TC Image, this method is used for reloading purposes
	virtual void FreeTCImageSurfaces( std::list< TCImage* > imageList ) = 0;


	/// Begin drawing a new scene
	virtual bool BeginScene( bool clear = false ) = 0;

	/// Display the back buffer to the screen
	virtual bool DisplayScene() = 0;

	/// Get the display dimensions
	virtual Vector2i GetDisplayDims() const = 0;

	/// Get if the graphics manager is full-screen
	virtual bool IsFullScreen() const = 0;

	/// Close the graphics manager and free any used resources
	virtual void Term() = 0;

	/// Check if the surfaces have been lost and images need to be reloaded
	virtual bool ImagesNeedReload() const = 0;

	/// Clear the flag indicating that surfaces need to be reloaded
	virtual void ClearImageReloadFlag() = 0;
};


extern GraphicsMgrBase* g_pGraphicsMgr;

#endif // __GraphicsMgr_h