//=================================================================================================
/*!
	\file DDraw1.h
	2D Graphics Engine
	DirectDraw Header
	\author Taylor Clark
	\date December 16, 2005

	This file contains the definition for the DirectDraw rendering class.
*/
//=================================================================================================

#pragma once
#ifndef __DDraw1_h
#define __DDraw1_h

#include <string>
#include "DrawInterface.h"
#include <ddraw.h>
#include "Base/TCAssert.h"
#include "../TCImage.h"
#include "../GraphicsDefines.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class DDrawSurface1
	\brief The DirectDraw surface class.

	This class defines the DirectDraw surface for the DDraw1 interface.
*/
//-------------------------------------------------------------------------------------------------
class DDrawSurface1 : public DDrawSurface
{
public:

	// The surface
	IDirectDrawSurface* m_pSurface;

	/// The basic constructor
	DDrawSurface1( IDirectDrawSurface* pSurface ) : m_pSurface( pSurface )
	{
		TCASSERTX( pSurface != NULL, L"NULL pointer not allowed when creating image." );
	}

	/// Release the surface data stored
	virtual void Release()
	{
		if( m_pSurface )
			m_pSurface->Release();
		m_pSurface = NULL;
	}

	/// The destructor
	virtual ~DDrawSurface1()
	{
		Release();
	}
};


//-------------------------------------------------------------------------------------------------
/*!
	\class DDraw1
	\brief The class for DirectDraw drawing objects.

	This class defines the interface for DirectDraw objects that are used to render images to the
	screen.
*/
//-------------------------------------------------------------------------------------------------
class DDraw1 : public DrawInterface
{
	/// The DirectDraw interface
	IDirectDraw2* m_pDDraw;
	
	/// The window to which graphics are rendered
	void* m_hWnd;

	/// The primary surface for displaying to
	IDirectDrawSurface* m_pPrimarySurface;

	/// The back buffer surface
	IDirectDrawSurface* m_pBufferSurface;

	/// The render target surface
	IDirectDrawSurface* m_pRenderSurface;

	/// The back buffer size
	Vector2i m_BufferSize;

	/// The transparent color key
	int m_TransColorKey;

	/// If the interface is in full screen mode
	bool m_InFullScreen;

	static const int32 NUM_BACKBUFFERS = 2;

public:

	/// The default constructor
	DDraw1() : m_pPrimarySurface( NULL ),
				m_hWnd( NULL ),
				m_pBufferSurface( NULL ),
				m_pRenderSurface( 0 ),
				m_TransColorKey( 0x00FF00FF ),
				m_InFullScreen( false ),
				m_pDDraw( 0 )
	{
	}

	/// Initialize DirectDraw
	virtual bool Init( void* hWnd, bool startFullscreen );

	/// Close DirectDraw
	virtual void Term();

	/// Begin drawing a scene
	virtual bool BeginScene( bool clear );

	/// Display the back buffer to the screen
	virtual bool DisplayScene();


	/// Draw an image
	virtual bool DrawImage( Point2i destPos, const TCImage* const pSrcImage, Box2i srcRect );
	
	/// Draw an image with effects
	virtual bool DrawImageEx( Box2i destRect, const TCImage* const pSrcImage, Box2i srcRect, int32 fx );

	/// Draw a rectangle
	virtual bool DrawRect( const Box2i& rect, int fillColor );

	/// Fill a rectangular area
	virtual bool FillRect( const Box2i& rect, int fillColor );


	/// Apply a pixel transformation
	virtual bool ApplyEffect( TCImage* pImage, const Box2i& rect, GraphicsDefines::ModPixelCB pixelCB );

	/// Set a temporary render target
	virtual bool SetTempRenderTarget( TCImage* pImage );

	/// Return to using the back buffer
	virtual void ClearTempRenderTarget(){ m_pRenderSurface = m_pBufferSurface; }


	/// Create an offscreen image surface and load image data onto it
	virtual DDrawSurface* LoadImageFromMemory( const Vector2i& imgDims, EImageResourceType imageType, DataBlock* pImageDataBlock );

	/// Reload the image data for a surface
	virtual bool ReloadImageFromMemory( TCImage* pImage, const Vector2i& size, EImageResourceType imageType, DataBlock* pImageDataBlock );

	/// Get the display dimensions
	virtual Vector2i GetDisplayDims() const { return m_BufferSize; }

	/// Get if the interface is in full-screen mode
	virtual bool IsFullScreen() const { return m_InFullScreen; }
};

#endif // __DDraw1_h