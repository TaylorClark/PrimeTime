//=================================================================================================
/*!
	\file DDrawBase.h
	2D Graphics Engine
	DirectDraw Interface Header
	\author Taylor Clark
	\date December 16, 2005

	This file contains the definition for the DirecDraw interface class.
*/
//=================================================================================================

#pragma once
#ifndef __DrawInterface_h
#define __DrawInterface_h

#include <string>
#include "Math/Vector2i.h"
#include "Math/Box2i.h"
#include "Math/Point2i.h"
#include "Base/Types.h"
#include "Base/DataBlock.h"
#include "Resource/ResTypes.h"
#include "../GraphicsDefines.h"


class TCImage;


class DDrawSurface
{
public:

	/// The size of the surface
	Vector2i m_Dims;

	/// The bits per pixel of the surface
	int32 m_BPP;

	/// The default constructor
	DDrawSurface() : m_BPP( 0 )
	{}

	/// Release the surface data stored
	virtual void Release() = 0;

	/// The destructor, virtual to ensure derived classes are freed properly
	virtual ~DDrawSurface() {}
};


//-------------------------------------------------------------------------------------------------
/*!
	\class DrawInterface
	\brief The abstract base class for drawing objects.

	This class defines the interface for objects that are used to render images to the screen.
*/
//-------------------------------------------------------------------------------------------------
class DrawInterface
{
protected:

	/// A flag set when the surfaces have been lost and need reloading
	bool m_SurfacesNeedReload;

public:

	/// The default constructor
	DrawInterface() : m_SurfacesNeedReload( false )
	{}

	/// The destructor, virtual to ensure derived classes are freed properly
	virtual ~DrawInterface(){}

	/// Initialize DirectDraw
	virtual bool Init( void* hWnd, bool startFullscreen ) = 0;

	/// Close DirectDraw
	virtual void Term() = 0;

	/// Begin drawing a scene
	virtual bool BeginScene( bool clear ) = 0;

	/// Display the back buffer to the screen
	virtual bool DisplayScene() = 0;

	/// Check if the surface data was lost
	virtual bool IsSurfaceDataLost() { return m_SurfacesNeedReload; }

	/// Clear the surface lost flag
	virtual void ClearSurfaceDataLostFlag() { m_SurfacesNeedReload = false; }


	/// Draw an image
	virtual bool DrawImage( Point2i destPos, const TCImage* const pSrcImage, Box2i srcRect ) = 0;

	/// Draw an image with effects
	virtual bool DrawImageEx( Box2i destRect, const TCImage* const pSrcImage, Box2i srcRect, int32 fx ) = 0;

	/// Draw a rectangle
	virtual bool DrawRect( const Box2i& rect, int fillColor ) = 0;

	/// Fill a rectangular area
	virtual bool FillRect( const Box2i& rect, int fillColor ) = 0;


	/// Apply a pixel transformation
	virtual bool ApplyEffect( TCImage* pImage, const Box2i& rect, GraphicsDefines::ModPixelCB pixelCB ) = 0;

	/// Set a temporary render target
	virtual bool SetTempRenderTarget( TCImage* pImage ) = 0;

	/// Return to using the back buffer
	virtual void ClearTempRenderTarget() = 0;


	/// Create an offscreen image surface and load image data onto it
	virtual DDrawSurface* LoadImageFromMemory( const Vector2i& size, EImageResourceType imageType, DataBlock* pImageDataBlock ) = 0;
	
	/// Reload the image data for a surface
	virtual bool ReloadImageFromMemory( TCImage* pImage, const Vector2i& size, EImageResourceType imageType, DataBlock* pImageDataBlock ) = 0;

	/// Get the display dimensions
	virtual Vector2i GetDisplayDims() const = 0;

	/// Get if the interface is in full-screen mode
	virtual bool IsFullScreen() const = 0;
};

#endif // __DrawInterface_h