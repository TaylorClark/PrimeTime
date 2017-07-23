#include "..\PrivateInclude\DDraw1.h"
#include <windows.h>
#include <wingdi.h>
#include "Base/MsgLogger.h"
#include "Base/StringFuncs.h"
#include "Base/NumFuncs.h"
#include "Jpeg/corona.h"

const int32 FULL_SCREEN_WIDTH = 1024;
const int32 FULL_SCREEN_HEIGHT = 768;


struct AdapterInfo
{
	GUID m_GUID;
	std::wstring sAdapterDesc;
	std::wstring sDriverName;
};
typedef std::list<AdapterInfo> AdapterList;

// Get the byte index of a color (R, G, or B) from a mask
extern int32 GetColorIndex( DWORD bitMask );


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDrawEnumCallbackEx  Static Global
///
///	A callback function for the DirectDraw function to enumerate the devices.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL WINAPI DDrawEnumCallbackEx(GUID FAR * pGUID, LPSTR szDriverDesc, LPSTR szDriverName, LPVOID pVoid, HMONITOR )
{
	AdapterList* pAdapterList = (AdapterList*)pVoid;

	// Store the driver description
	AdapterInfo newInfo;
	if( !pGUID )
		memset( &newInfo.m_GUID, 0, sizeof(newInfo.m_GUID) );
	else
		newInfo.m_GUID = *pGUID;
	newInfo.sAdapterDesc = TCBase::Widen( szDriverDesc );
	newInfo.sDriverName = TCBase::Widen( szDriverName );
	pAdapterList->push_back( newInfo );

	// Continue enumeration
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDrawEnumCallback  Static Global
///
///	A callback function for the DirectDraw function to enumerate the devices.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL WINAPI DDrawEnumCallback(GUID FAR * pGUID, LPSTR szDriverDesc, LPSTR szDriverName, LPVOID pVoid )
{
	// Allow one function to handle the logic
	return DDrawEnumCallbackEx( pGUID, szDriverDesc, szDriverName, pVoid, NULL );
}


//static void EnumerateDevices( AdapterList* pRetList )
//{
//	if( !pRetList )
//		return;
//
//	HINSTANCE h = LoadLibrary( L"ddraw.dll" );
// 
//    // If ddraw.dll doesn't exist in the search path,
//    // then DirectX probably isn't installed, so fail.
//    if (!h)
//        return;
// 
//    // Note that you must know which version of the
//    // function to retrieve (see the following text).
//    // For this example, we use the ANSI version.
//    LPDIRECTDRAWENUMERATEEXA lpDDEnumEx;
//    lpDDEnumEx = (LPDIRECTDRAWENUMERATEEXA)GetProcAddress( h, "DirectDrawEnumerateExA" );
// 
//    // If the function is there, call it to enumerate all display 
//    // devices attached to the desktop, and any non-display DirectDraw
//    // devices.
//    if (lpDDEnumEx)
//	{
//		HRESULT hResult;
//        if( FAILED( hResult = lpDDEnumEx( DDrawEnumCallbackEx, pRetList, DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES ) ) )
//		{
//			hResult = 0;
//		}
//	}
//    else
//    {
//        /*
//         * We must be running on an old version of DirectDraw.
//         * Therefore MultiMon isn't supported. Fall back on
//         * DirectDrawEnumerate to enumerate standard devices on a 
//         * single-monitor system.
//         */
//        DirectDrawEnumerateA( DDrawEnumCallback, pRetList );
// 
//        /* Note that it could be handy to let the OldCallback function
//         * be a wrapper for a DDEnumCallbackEx. 
//         * 
//         * Such a function would look like:
//         *    BOOL FAR PASCAL OldCallback(GUID FAR *lpGUID,
//         *                                LPSTR pDesc,
//         *                                LPSTR pName,
//         *                                LPVOID pContext)
//         *    {
//         *         return Callback(lpGUID,pDesc,pName,pContext,NULL);
//         *    }
//         */
//    }
// 
//    // If the library was loaded by calling LoadLibrary(),
//    // then you must use FreeLibrary() to let go of it.
//    FreeLibrary(h);
//}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDrawModesCB  Global
///
///	\param pSurfDesc The surface desribing the current draw mode
///	\param pContextData The user variable passed to the enumeration methoed
///
///	The callback to handle the enumeration of DirectDraw display modes.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static HRESULT WINAPI DDrawModesCB( DDSURFACEDESC* pSurfDesc, void* pContextData )
{
	if( pSurfDesc->dwWidth == FULL_SCREEN_WIDTH && pSurfDesc->dwHeight == FULL_SCREEN_HEIGHT )
	{
		if( pSurfDesc->ddpfPixelFormat.dwRGBBitCount >= 16 && pSurfDesc->ddpfPixelFormat.dwRGBBitCount <= 32 )
		{
			// Prefer 32-bit then 24-bit
			DDSURFACEDESC* pBestSurfDesc = (DDSURFACEDESC*)pContextData;
			if( pSurfDesc->ddpfPixelFormat.dwRGBBitCount > pBestSurfDesc->ddpfPixelFormat.dwRGBBitCount )
				*pBestSurfDesc = *pSurfDesc;
		}
	}

	// Continue enumeration
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDraw1::Init  Public
///
///	\param hWnd The window to draw to
///
///	Initialize DirectDraw for use.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::Init( void* hWnd, bool isFullscreen )
{
	HRESULT hResult;

	//typedef BOOL (FAR PASCAL * LPDDENUMCALLBACKA)(GUID FAR *, LPSTR, LPSTR, LPVOID);
	AdapterList adapterList;
	DirectDrawEnumerateA( DDrawEnumCallback, &adapterList );
	//EnumerateDevices( &adapterList );

	// Create the DirectDraw interface
	IDirectDraw* pDDraw = 0;
	if( FAILED( hResult = DirectDrawCreate( NULL, &pDDraw, NULL)) )
	{
		return false;
	}

	// Get the newer version of the interface
	HRESULT hNewInterfaceRes = pDDraw->QueryInterface(IID_IDirectDraw2, (void **)&m_pDDraw);
	
	// Free the old interface
	pDDraw->Release();
	pDDraw = 0;

	// If the newer version failed to be acquired then bail out
	if( FAILED(hNewInterfaceRes) )
		return false;

	// Store the window and set the cooperative level
	m_hWnd = hWnd;
	if( isFullscreen )
	{
		// Go through the display modes
		DDSURFACEDESC bestSurfDesc;
		memset( &bestSurfDesc, 0, sizeof(bestSurfDesc) );
		bestSurfDesc.dwSize = sizeof(bestSurfDesc);
		m_pDDraw->EnumDisplayModes( 0, NULL, (void*)&bestSurfDesc, DDrawModesCB );
		
		// Become the exclusive user of the screen
		m_pDDraw->SetCooperativeLevel( (HWND)m_hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE );
		m_pDDraw->SetDisplayMode( FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT, bestSurfDesc.ddpfPixelFormat.dwRGBBitCount, 0, 0 );

		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Graphics manager going to full screen (%d x %d : %dbpp)", FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT, bestSurfDesc.ddpfPixelFormat.dwRGBBitCount );
		m_InFullScreen = true;

		// Set up the primary surface descriptor data
		DDSURFACEDESC ddrawSurfDesc;
		memset( &ddrawSurfDesc, 0, sizeof(ddrawSurfDesc) );
		ddrawSurfDesc.dwSize = sizeof(ddrawSurfDesc);
		ddrawSurfDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddrawSurfDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddrawSurfDesc.dwBackBufferCount = NUM_BACKBUFFERS;

		// Create the primary surface
		HRESULT hResult;
		if( FAILED( hResult = m_pDDraw->CreateSurface( &ddrawSurfDesc, (IDirectDrawSurface**)&m_pPrimarySurface, NULL ) ) )
		{
			return false;
		}

		// Get the backbuffer
		DDSCAPS ddsCaps;
		memset( &ddsCaps, 0, sizeof(ddsCaps) );
		ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
		if( FAILED( hResult = m_pPrimarySurface->GetAttachedSurface( &ddsCaps, &m_pBufferSurface ) ) )
		{
			return false;
		}
		
		// Get the back buffer size
		m_BufferSize.Set( FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT );

		// Get the key colors
		DDPIXELFORMAT pixelFormat;
		memset( &pixelFormat, 0, sizeof(pixelFormat) );
		pixelFormat.dwSize = sizeof(pixelFormat);
		m_pPrimarySurface->GetPixelFormat( &pixelFormat );
		m_TransColorKey = pixelFormat.dwRBitMask | pixelFormat.dwBBitMask;
	}
	else
	{
		m_pDDraw->SetCooperativeLevel( (HWND)m_hWnd, DDSCL_NORMAL );
		m_InFullScreen = false;

		// Set up the primary surface descriptor data
		DDSURFACEDESC ddrawSurfDesc;
		memset( &ddrawSurfDesc, 0, sizeof(ddrawSurfDesc) );

		ddrawSurfDesc.dwSize = sizeof(ddrawSurfDesc);
		ddrawSurfDesc.dwFlags = DDSD_CAPS;
		ddrawSurfDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		HRESULT hResult;
		if( FAILED( hResult = m_pDDraw->CreateSurface( &ddrawSurfDesc, (IDirectDrawSurface**)&m_pPrimarySurface, NULL ) ) )
		{
			return false;
		}

		// Get the client rectangle to use when creating the back buffer
		RECT clientRect;
		GetClientRect( (HWND)m_hWnd, &clientRect );

		// Get the back buffer size
		m_BufferSize.x = clientRect.right - clientRect.left;
		m_BufferSize.y = clientRect.bottom - clientRect.top;

		// Setup the buffer info
		ddrawSurfDesc.dwFlags |= DDSD_WIDTH | DDSD_HEIGHT;
		ddrawSurfDesc.dwWidth = m_BufferSize.x;
		ddrawSurfDesc.dwHeight = m_BufferSize.y;
		ddrawSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

		// Get the key colors
		DDPIXELFORMAT pixelFormat;
		memset( &pixelFormat, 0, sizeof(pixelFormat) );
		pixelFormat.dwSize = sizeof(pixelFormat);
		m_pPrimarySurface->GetPixelFormat( &pixelFormat );
		m_TransColorKey = pixelFormat.dwRBitMask | pixelFormat.dwBBitMask;
		//m_DestColorKey = pixelFormat.dwGBitMask | pixelFormat.dwBBitMask;

		// Create the surface
		if( FAILED( hResult = m_pDDraw->CreateSurface( &ddrawSurfDesc, (IDirectDrawSurface**)&m_pBufferSurface, NULL ) ) )
		{
			return false;
		}

		// Create the clipper
		IDirectDrawClipper* pClipper = NULL;
		if( FAILED( hResult = m_pDDraw->CreateClipper( 0, (IDirectDrawClipper**)&pClipper, NULL ) ) )
		{
			return false;
		}

		// Set the window
		pClipper->SetHWnd( 0, (HWND)hWnd );

		// Set the clipper
		m_pPrimarySurface->SetClipper( pClipper );
	}

	// Set the render surface to the back buffer
	m_pRenderSurface = m_pBufferSurface;

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDraw1::Term  Public
///
///	Clean up DirectDraw
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void DDraw1::Term()
{
	if( !m_pDDraw )
		return;

	// Free the primary surface
	if( m_pPrimarySurface )
	{
		m_pPrimarySurface->SetClipper( NULL );
		m_pPrimarySurface->Release();
		m_pPrimarySurface = NULL;
	}

	// Free the drawing buffer surface only if this is windowed mode
	if( m_pBufferSurface )
	{
		if( !m_InFullScreen )
			m_pBufferSurface->Release();
		m_pBufferSurface = NULL;
	}

	m_pDDraw->Release();
	m_pDDraw = NULL;
	m_pRenderSurface = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDraw1::ApplyEffect  Public
///
///	\param pImage The image to affect
///	\param rect The rectangle on the surface to affect
///	\param pixelCB The callback function the modifies the pixel values
///	\returns True if the transformation was applied, otherwise false
///
///	Apply a pixel transformation
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::ApplyEffect( TCImage* pImage, const Box2i& rect, GraphicsDefines::ModPixelCB pixelCB )
{
	// Get the proper image pointer
	DDrawSurface1* pDDrawImage1 = reinterpret_cast< DDrawSurface1* >( pImage->GetImageData() );
	if( !pDDrawImage1 || !pDDrawImage1->m_pSurface )
		return false;

	// If the image is 24 or 32 bit then use the fast version
	if( pDDrawImage1->m_BPP == 24 || pDDrawImage1->m_BPP == 32 )
	{
		// Lock the surface
		DDSURFACEDESC lockSurfDesc;
		memset( &lockSurfDesc, 0, sizeof(lockSurfDesc) );
		lockSurfDesc.dwSize = sizeof(lockSurfDesc);
		RECT lockRect;
		lockRect.left = rect.pos.x;
		lockRect.top = rect.pos.y;
		lockRect.right = rect.Right();
		lockRect.bottom = rect.Bottom();

		// Lock the surface so we can get access to the pixels
		if( FAILED( pDDrawImage1->m_pSurface->Lock( &lockRect, &lockSurfDesc, DDLOCK_WAIT, NULL ) ) )
			return false;

		// Get the color offsets
		int rIndex = GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwRBitMask );
		int gIndex = GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwGBitMask );
		int bIndex = GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwBBitMask );

		//const COLORREF trans = RGB(255,0,255);
		const int32 pixelStride = lockSurfDesc.ddpfPixelFormat.dwRGBBitCount / 8;
		for( int row = 0; row < rect.size.y; ++row )
		{
			// Get a pointer to the current row of pixels
			uint8* pCurRowData = (uint8*)lockSurfDesc.lpSurface + (row * lockSurfDesc.lPitch);

			// Go through each column in the row
			for( int col = 0; col < rect.size.x; ++col )
			{
				// Skip transparent pixels
				uint8* pCurPixel = pCurRowData + (col * pixelStride);
				if( pCurPixel[rIndex] == 255 && pCurPixel[gIndex] == 0 && pCurPixel[bIndex] == 255 )
					continue;

				// Darken the pixel
				pCurPixel[rIndex] = pCurPixel[rIndex] / 2;
				pCurPixel[gIndex] = pCurPixel[gIndex] / 2;
				pCurPixel[bIndex] = pCurPixel[bIndex] / 2;

				//// Get the current pixel's color
				//uint8* pCurPixelData = pCurRowData + (col * pixelStride);
				//COLORREF color = RGB( pCurPixelData[rIndex], pCurPixelData[gIndex], pCurPixelData[bIndex] );

				//// If the color is not transparent
				//if( color != trans )
				//{
				//	// Transform the color
				//	color = pixelCB( color );

				//	// Store the transformed pixel
				//	pCurPixelData[rIndex] = GetRValue( color );
				//	pCurPixelData[gIndex] = GetGValue( color );
				//	pCurPixelData[bIndex] = GetBValue( color );
				//}
			}
		}

		// Unlock the surface
		pDDrawImage1->m_pSurface->Unlock( NULL );
	}
	// Else use GDI for the color conversion
	else
	{
		HDC hDC = 0;
		if( FAILED( pDDrawImage1->m_pSurface->GetDC( &hDC ) ) )
			return false;

		const COLORREF trans = RGB(255,0,255);

		for( int row = 0; row < rect.size.y; ++row )
		{
			for( int col = 0; col < rect.size.x; ++col )
			{
				COLORREF color = GetPixel( hDC, rect.pos.x + col, rect.pos.y + row );
				if( color != trans )
				{
					color = pixelCB( color );
					SetPixel( hDC, rect.pos.x + col, rect.pos.y + row, color );
				}
			}
		}
		pDDrawImage1->m_pSurface->ReleaseDC( hDC );
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDraw1::SetTempRenderTarget  Public
///
///	\param pImage The image to which subsequent draw calls will affect
///	\returns Whether or not the image was set as the temporary render target
///
/// Set a temporary render target.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::SetTempRenderTarget( TCImage* pImage )
{
	if( !pImage )
		return false;

	// Get the proper image pointer
	DDrawSurface1* pDDrawImage1 = reinterpret_cast< DDrawSurface1* >( pImage->GetImageData() );
	if( !pDDrawImage1 || !pDDrawImage1->m_pSurface )
		return false;

	// Store the render surface
	m_pRenderSurface = pDDrawImage1->m_pSurface;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  DDraw1::BeginScene  Public
///	\param clear Set to true to clear the buffer when starting the scene
///
/// Begin drawing a scene
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::BeginScene( bool clear )
{
	if( clear )
		FillRect( Box2i( Point2i(), GetDisplayDims() ), 0x000000FF );

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  DDraw1::DisplayScene  Public
///	\returns Whether or not the scene was displayed without error
///
///	Display the back buffer to the primary surface.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::DisplayScene()
{
	// If the interface is in full-screen mode then use flip
	if( m_InFullScreen )
	{
		HRESULT hRes;
		if( FAILED( hRes = m_pPrimarySurface->Flip( NULL, DDFLIP_WAIT )) )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to flip: %d", (int)hRes );
			if( hRes == DDERR_SURFACELOST )
				m_SurfacesNeedReload = true;
			m_pPrimarySurface->Restore();
			return false;
		}
	}
	// Else blit to the window
	else
	{
		// Get the X,Y to blit to
		POINT clientPt;
		clientPt.x = 0;
		clientPt.y = 0;
		ClientToScreen( (HWND)m_hWnd, &clientPt );

		// Store the display rectangles
		RECT destRect = { clientPt.x, clientPt.y, clientPt.x + m_BufferSize.x, clientPt.y + m_BufferSize.y };
		RECT srcRect = { 0, 0, m_BufferSize.x, m_BufferSize.y };
		HRESULT hRes;
		if( FAILED( hRes = m_pPrimarySurface->Blt( &destRect, m_pBufferSurface, &srcRect, DDBLT_WAIT, NULL ) ) )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to flip: %d", (int)hRes );
			if( hRes == DDERR_SURFACELOST )
				m_SurfacesNeedReload = true;
			m_pPrimarySurface->Restore();
			m_pBufferSurface->Restore();
			return false;
		}
	}

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDraw1::DrawImage  Public
///
///	\param destPos The destination position to draw to
///	\param pSrcImage The image to use for drawing
///	\param srcRect The rectangle on the source image to draw
///	\returns True if there were no problems, false otherwise
///
///	Draw an image to the back buffer
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::DrawImage( Point2i destPos, const TCImage* const pSrcImage, Box2i srcRect )
{
	// Get the proper image pointer
	DDrawSurface1* pDDrawImage1 = reinterpret_cast< DDrawSurface1* >( pSrcImage->GetImageData() );
	if( !pDDrawImage1 )
		return false;

	// Clip the image horizontally
	if( destPos.x < 0 )
	{
		srcRect.pos.x += -destPos.x;
		srcRect.size.x -= -destPos.x;
		destPos.x = 0;
	}
	
	if( destPos.x + srcRect.size.x > m_BufferSize.x )
		srcRect.size.x -= (destPos.x + srcRect.size.x - m_BufferSize.x);

	// Clip the image vertically
	if( destPos.y < 0 )
	{
		srcRect.pos.y += -destPos.y;
		srcRect.size.y -= -destPos.y;
		destPos.y = 0;
	}
	
	if( destPos.y + srcRect.size.y > m_BufferSize.y )
		srcRect.size.y -= (destPos.y + srcRect.size.y - m_BufferSize.y);

	// If the image is off the screen then bail
	if( srcRect.size.x <= 0 || srcRect.size.y <= 0 )
		return true;

	// Get the rectangles for displaying
	RECT srcDXRect = { srcRect.pos.x, srcRect.pos.y, srcRect.pos.x + srcRect.size.x, srcRect.pos.y + srcRect.size.y };

	// Blit the data to the buffer surface
    if( FAILED( m_pRenderSurface->BltFast( destPos.x, destPos.y, pDDrawImage1->m_pSurface, &srcDXRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY ) ) )
		return false;

	return true;
}

	
/// Draw an image with effects
bool DDraw1::DrawImageEx( Box2i destRect, const TCImage* const pSrcImage, Box2i srcRect, int32 fx )
{
	// Get the proper image pointer
	DDrawSurface1* pDDrawImage1 = reinterpret_cast< DDrawSurface1* >( pSrcImage->GetImageData() );
	if( !pDDrawImage1 )
		return false;

	if( srcRect.size.x == 0 || srcRect.size.y == 0 || destRect.size.x == 0 || destRect.size.y == 0 )
		return false;

	float horzScale = (float32)destRect.size.x / (float32)srcRect.size.x;
	float vertScale = (float32)destRect.size.y / (float32)srcRect.size.y;

	// Clip the image horizontally
	if( destRect.pos.x < 0 )
	{
		srcRect.pos.x += (int32)( (float32)-destRect.pos.x / horzScale );
		srcRect.size.x -= (int32)( (float32)-destRect.pos.x / horzScale );
		destRect.pos.x = 0;
	}
	if( destRect.Right() > m_BufferSize.x )
	{
		int32 amountOff = destRect.Right() - m_BufferSize.x;
		srcRect.size.x -= (int32)( (float32)amountOff / horzScale );
		destRect.size.x -= amountOff;
	}

	// Clip the image vertically
	if( destRect.pos.y < 0 )
	{
		srcRect.pos.y += (int32)( (float32)-destRect.pos.y / vertScale );
		srcRect.size.y -= (int32)( (float32)-destRect.pos.y / vertScale );
		destRect.pos.y = 0;
	}
	if( destRect.Bottom() > m_BufferSize.y )
	{
		int32 amountOff = destRect.Bottom() - m_BufferSize.y;
		srcRect.size.y -= (int32)( (float32)amountOff / vertScale );
		destRect.size.y -= amountOff;
	}

	// If the image is off the screen then there is no need to draw
	if( srcRect.size.x <= 0 || srcRect.size.y <= 0 )
		return true;

	// Get the rectangles for displaying
	RECT srcDXRect = { srcRect.pos.x, srcRect.pos.y, srcRect.pos.x + srcRect.size.x, srcRect.pos.y + srcRect.size.y };
	RECT destDXRect = { destRect.pos.x, destRect.pos.y, destRect.pos.x + destRect.size.x, destRect.pos.y + destRect.size.y };

	// Fill in the effect structure
	DDBLTFX blitFX;
	memset( &blitFX, 0, sizeof( blitFX ) );
	blitFX.dwSize = sizeof(blitFX);
	
	if( fx & GraphicsDefines::DEF_FlipHoriz )
		blitFX.dwDDFX |= DDBLTFX_MIRRORLEFTRIGHT;
	if( fx & GraphicsDefines::DEF_FlipVert )
		blitFX.dwDDFX |= DDBLTFX_MIRRORUPDOWN;

	if( fx & GraphicsDefines::DEF_Rotate_90 )
		blitFX.dwDDFX |= DDBLTFX_ROTATE90;
	else if( fx & GraphicsDefines::DEF_Rotate_180 )
		blitFX.dwDDFX |= DDBLTFX_ROTATE180;
	else if( fx & GraphicsDefines::DEF_Rotate_270 )
		blitFX.dwDDFX |= DDBLTFX_ROTATE270;

	// If there are no effects and there is no scaling then use a fast blit
	if( blitFX.dwDDFX == 0 && horzScale == 1.0f && vertScale == 1.0f )
	{
		// Blit the data to the buffer surface
		if( FAILED( m_pRenderSurface->BltFast( destDXRect.left, destDXRect.top, pDDrawImage1->m_pSurface, &srcDXRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY ) ) )
			return false;
	}
	// Otherwise we need to use the normal blit
	else
	{
		// Get the blit flags
		DWORD blitFlags = DDBLT_WAIT | DDBLT_KEYSRC;

		// If there are effects then add in the FX flag
		if( blitFX.dwDDFX != 0 )
			blitFlags |= DDBLT_DDFX;	

		// Blit the data to the buffer surface
		DWORD err = 0;
		if( FAILED( err = m_pRenderSurface->Blt( &destDXRect, pDDrawImage1->m_pSurface, &srcDXRect, blitFlags, &blitFX ) ) )
			return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDraw1::DrawRect  Public
///
///	\param rect The rectangle to draw
///	\param fillColor The color to use for the lines
///	\returns True if there were no problems, false otherwise
///
///	Draw a rectangle
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::DrawRect( const Box2i& rect, int fillColor )
{
	// Set up the blit FX for filling color
	DDBLTFX bltFX;
	memset( &bltFX, 0, sizeof(bltFX) );
	bltFX.dwSize = sizeof(bltFX);
	bltFX.dwFillColor = fillColor;
	
	// Top
	RECT topRect = { rect.pos.x, rect.pos.y, rect.pos.x + rect.size.x, rect.pos.y + 1 };
	if( FAILED( m_pRenderSurface->Blt( &topRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;
	
	// Right
	RECT rightRect = { rect.pos.x + rect.size.x, rect.pos.y, rect.pos.x + rect.size.x + 1, rect.pos.y + rect.size.y };
	if( FAILED( m_pRenderSurface->Blt( &rightRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;
	
	// Bottom
	RECT bottomRect = { rect.pos.x, rect.pos.y + rect.size.y, rect.pos.x + rect.size.x, rect.pos.y + rect.size.y + 1 };
	if( FAILED( m_pRenderSurface->Blt( &bottomRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;
	
	// Left
	RECT leftRect = { rect.pos.x, rect.pos.y, rect.pos.x + 1, rect.pos.y + rect.size.y };
	if( FAILED( m_pRenderSurface->Blt( &leftRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDraw1::FillRect  Public
///
///	\param rect The rectangle to draw
///	\param fillColor The color to use for the fill
///	\returns True if there were no problems, false otherwise
///
///	Fill a rectangular area.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::FillRect( const Box2i& rect, int fillColor )
{
	// Set up the blit FX for filling color
	DDBLTFX bltFX;
	memset( &bltFX, 0, sizeof(bltFX) );
	bltFX.dwSize = sizeof(bltFX);
	bltFX.dwFillColor = fillColor;
	
	// Fill the rectangle
	RECT fillRect = { rect.pos.x, rect.pos.y, rect.pos.x + rect.size.x, rect.pos.y + rect.size.y };
	if( FAILED( m_pRenderSurface->Blt( &fillRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;

	// Return success
	return true;
}