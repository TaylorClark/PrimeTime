//=================================================================================================
/*!
	\file D3D9DDraw.h
	2D Graphics Engine
	Direct3D 3 DirectDraw Interface Header
	\author Taylor Clark
	\date March 15, 2007

	This file contains the definition for the DirecDraw interface class.
*/
//=================================================================================================

#pragma once
#ifndef __D3D9DDraw_h
#define __D3D9DDraw_h

#include <string>
#include "DDrawInterface.h"
#include <ddraw.h>
#include "Base/TCAssert.h"
#include "../TCImage.h"
#include <tchar.h>
#include "../GraphicsDefines.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class D3D9DDraw
	\brief The class for a DirectDraw interface class that uses Direct3D.
*/
//-------------------------------------------------------------------------------------------------
class D3D9DDraw : public DDrawInterface
{
	/// The DirectDraw interface
	IDirectDraw2* m_pDDraw;
	
	/// The window we are blitting to
	void* m_hWnd;

	/// The primary surface for displaying to
	IDirectDrawSurface* m_pPrimarySurface;

	/// The buffer surface for drawing to
	IDirectDrawSurface* m_pBufferSurface;

	/// The back buffer size
	Vector2i m_BufferSize;

	/// The transparent color key
	int m_TransColorKey;

	/// If the interface is in full screen mode
	bool m_InFullScreen;

	static const int32 NUM_BACKBUFFERS = 2;

public:

	/// The default constructor
	D3D9DDraw() : m_pPrimarySurface( NULL ),
				m_hWnd( NULL ),
				m_pBufferSurface( NULL ),
				m_BufferSize( 0, 0 ),
				m_TransColorKey( 0x00FF00FF ),
				m_InFullScreen( false ),
				m_pDDraw( 0 )
	{
	}

	/// Initialize DirectDraw
	virtual bool Init( void* hWnd, bool startFullscreen );

	/// Close DirectDraw
	virtual void Term();

	/// Display the back buffer to the screen
	virtual bool DisplayScene();

	/// Draw an image
	virtual bool DrawImage( Point2i destPos, const TCImage* const pSrcImage, Box2i srcRect );
	
	/// Draw an image with effects
	virtual bool DrawImage( Box2i destRect, const TCImage* const pSrcImage, Box2i srcRect, int32 fx, float fRotAngle );

	/// Draw a rectangle
	virtual bool DrawRect( const Box2i& rect, int fillColor );

	/// Fill a rectangular area
	virtual bool FillRect( const Box2i& rect, int fillColor );

	/// Create an offscreen image surface and load image data onto it
	virtual DDrawSurface* LoadImageFromMemory( const Vector2i& imgDims, EImageResourceType imageType, DataBlock imageData );

	/// Reload the image data for a surface
	virtual bool ReloadImageFromMemory( TCImage* pImage, const Vector2i& size, EImageResourceType imageType, DataBlock imageData );

	/// Get the display dimensions
	virtual Vector2i GetDisplayDims() const { return m_BufferSize; }

	/// Get if the interface is in full-screen mode
	virtual bool IsFullScreen() const { return m_InFullScreen; }
};

#endif // __D3D9DDraw_h