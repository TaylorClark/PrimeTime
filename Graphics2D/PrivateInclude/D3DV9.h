//=================================================================================================
/*!
	\file D3DV9.h
	2D Graphics Engine
	Direct3D Header
	\author Taylor Clark
	\date December 19, 2007

	This file contains the definition for the Direct3D rendering class.
*/
//=================================================================================================

#pragma once
#ifndef __D3DV9_h
#define __D3DV9_h

#include <string>
#include "DrawInterface.h"
#include <d3d9.h>
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
class D3DV9Texture : public DDrawSurface
{
public:

	// The texture
	IDirect3DTexture9* m_pTexture;

	/// The basic constructor
	D3DV9Texture( IDirect3DTexture9* pTexture ) : m_pTexture( pTexture )
	{
		TCASSERTX( pTexture != NULL, L"NULL pointer not allowed when creating a texture." );
	}

	/// Release the stored texture data
	virtual void Release()
	{
		if( m_pTexture )
			m_pTexture->Release();
		m_pTexture = NULL;
	}

	/// The destructor
	virtual ~D3DV9Texture()
	{
		Release();
	}
};


//-------------------------------------------------------------------------------------------------
/*!
	\class D3DV9
	\brief The class for rendering using Direct3D version 9.
*/
//-------------------------------------------------------------------------------------------------
class D3DV9 : public DrawInterface
{
	/// The Direct3D 9 interface
	IDirect3D9* m_pD3D9;

	/// The Direct3D 9 graphics device
	IDirect3DDevice9* m_pD3DDevice;
	
	/// The window to which graphics are rendered
	void* m_hWnd;

	/// The vertex buffer for drawing sprites
	IDirect3DVertexBuffer9* m_pSpriteVertexBuffer;

	/// The vertex buffer for drawing shapes and lines
	IDirect3DVertexBuffer9* m_pShapeVertexBuffer;

	/// The render target size
	Vector2i m_ViewportSize;

	/// The transparent color key
	int32 m_TransColorKey;

	/// If the interface is in full screen mode
	bool m_InFullScreen;

	static const int32 NUM_BACKBUFFERS = 2;


	/// Recalculate the scene transformations
	void RecalcSceneTransforms();

public:

	/// The default constructor
	D3DV9() : m_pD3D9( NULL ),
				m_pD3DDevice( NULL ),
				m_hWnd( NULL ),
				m_pSpriteVertexBuffer( 0 ),
				m_pShapeVertexBuffer( 0 ),
				m_TransColorKey( 0x00FF00FF ),
				m_InFullScreen( false )
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
	virtual void ClearTempRenderTarget(){ TCBREAKX( L"Not implemented" ); }


	/// Create an offscreen image surface and load image data onto it
	virtual DDrawSurface* LoadImageFromMemory( const Vector2i& imgDims, EImageResourceType imageType, DataBlock* pImageDataBlock );

	/// Reload the image data for a surface
	virtual bool ReloadImageFromMemory( TCImage* pImage, const Vector2i& size, EImageResourceType imageType, DataBlock* pImageDataBlock );

	/// Get the display dimensions
	virtual Vector2i GetDisplayDims() const { return m_ViewportSize; }

	/// Get if the interface is in full-screen mode
	virtual bool IsFullScreen() const { return m_InFullScreen; }
};

#endif // __D3DV9_h