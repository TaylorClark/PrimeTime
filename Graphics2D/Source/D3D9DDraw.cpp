#include "..\PrivateInclude\D3D9DDraw.h"
#include <windows.h>
#include <wingdi.h>
#include <tchar.h>
#include "Base/MsgLogger.h"
#include "Base/StringFuncs.h"
#include "Base/NumFuncs.h"
#include "Jpeg/corona.h"

#include <d3d9.h>


//-------------------------------------------------------------------------------------------------
/*!
	\class D3D9DrawSurface
	\brief The DirectDraw surface class.

	This class defines the DirectDraw surface for the DDraw1 interface.
*/
//-------------------------------------------------------------------------------------------------
class D3D9DrawSurface : public DDrawSurface
{
public:

	// The surface
	IDirectDrawSurface* m_pSurface;

	/// The basic constructor
	D3D9DrawSurface( IDirectDrawSurface* pSurface ) : m_pSurface( pSurface )
	{
		TCASSERTX( pSurface != NULL, _T("NULL pointer not allowed when creating image.") );
	}

	/// Release the surface data stored
	virtual void Release()
	{
		if( m_pSurface )
			m_pSurface->Release();
		m_pSurface = NULL;
	}

	/// The destructor
	virtual ~D3D9DrawSurface()
	{
		Release();
	}
};


const int32 FULL_SCREEN_WIDTH = 1024;
const int32 FULL_SCREEN_HEIGHT = 768;


struct AdapterInfo
{
	GUID m_GUID;
	std::wstring sAdapterDesc;
	std::wstring sDriverName;
};
typedef std::list<AdapterInfo> AdapterList;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDrawEnumCallback  Static Global
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

static BOOL WINAPI DDrawEnumCallback(GUID FAR * pGUID, LPSTR szDriverDesc, LPSTR szDriverName, LPVOID pVoid )
{
	// Allow one function to handle the logic
	return DDrawEnumCallbackEx( pGUID, szDriverDesc, szDriverName, pVoid, NULL );
}


static void EnumerateDevices( AdapterList* pRetList )
{
	if( !pRetList )
		return;

	HINSTANCE h = LoadLibrary( L"ddraw.dll" );
 
    // If ddraw.dll doesn't exist in the search path,
    // then DirectX probably isn't installed, so fail.
    if (!h)
        return;
 
    // Note that you must know which version of the
    // function to retrieve (see the following text).
    // For this example, we use the ANSI version.
    LPDIRECTDRAWENUMERATEEXA lpDDEnumEx;
    lpDDEnumEx = (LPDIRECTDRAWENUMERATEEXA)GetProcAddress( h, "DirectDrawEnumerateExA" );
 
    // If the function is there, call it to enumerate all display 
    // devices attached to the desktop, and any non-display DirectDraw
    // devices.
    if (lpDDEnumEx)
	{
		HRESULT hResult;
        if( FAILED( hResult = lpDDEnumEx( DDrawEnumCallbackEx, pRetList, DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES ) ) )
		{
			hResult = 0;
		}
	}
    else
    {
        /*
         * We must be running on an old version of DirectDraw.
         * Therefore MultiMon isn't supported. Fall back on
         * DirectDrawEnumerate to enumerate standard devices on a 
         * single-monitor system.
         */
        DirectDrawEnumerateA( DDrawEnumCallback, pRetList );
 
        /* Note that it could be handy to let the OldCallback function
         * be a wrapper for a DDEnumCallbackEx. 
         * 
         * Such a function would look like:
         *    BOOL FAR PASCAL OldCallback(GUID FAR *lpGUID,
         *                                LPSTR pDesc,
         *                                LPSTR pName,
         *                                LPVOID pContext)
         *    {
         *         return Callback(lpGUID,pDesc,pName,pContext,NULL);
         *    }
         */
    }
 
    // If the library was loaded by calling LoadLibrary(),
    // then you must use FreeLibrary() to let go of it.
    FreeLibrary(h);
}


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
//  D3D9DDraw::Init  Public
///
///	\param hWnd The window to draw to
///
///	Initialize DirectDraw for use.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3D9DDraw::Init( void* hWnd, bool startFullscreen )
{
	HRESULT hResult;

	IDirect3D9* pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( pD3D9 )
	{
		D3DDISPLAYMODE d3ddm;
		if( FAILED( pD3D9->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
			return false;

		D3DPRESENT_PARAMETERS d3dpp; 
		ZeroMemory( &d3dpp, sizeof(d3dpp) );
		d3dpp.Windowed   = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = d3ddm.Format;

		IDirect3DDevice9* pD3D9Interface = 0;
		pD3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)hWnd,
									  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
									  &d3dpp, &pD3D9Interface );

		pD3D9->Release();
		pD3D9 = 0;
	}

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
	if( startFullscreen )
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
	}
	else
	{
		m_pDDraw->SetCooperativeLevel( (HWND)m_hWnd, DDSCL_NORMAL );
		m_InFullScreen = false;
	}
	
	// If the program is going to full screen mode
	if( m_InFullScreen )
	{
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
	// Else the program is in windowed mode
	else
	{
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

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgr::Term  Public
///
///	Clean up DirectDraw
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void D3D9DDraw::Term()
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
}


static void FillInImageData_GDI( IDirectDrawSurface* pSurface, const Vector2i& imgDims, uint8* pSrcPixelData )
{
	BITMAPINFO bmInfo;
	memset( &bmInfo, 0, sizeof(bmInfo) );
	bmInfo.bmiHeader.biBitCount = 24;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biHeight = -imgDims.y;
	bmInfo.bmiHeader.biWidth = imgDims.x;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biSizeImage = imgDims.x * imgDims.y * 3;
	bmInfo.bmiHeader.biSize = sizeof(bmInfo.bmiHeader);

	uint8* pDestData = NULL;
	HBITMAP hSrcBitmap = CreateDIBSection( NULL, &bmInfo, DIB_RGB_COLORS, (void**)&pDestData, NULL, 0 );
	if( hSrcBitmap == NULL )
		return;

	// Fill in the data
	int pitch = imgDims.x * 3;
	pitch += pitch % 4;
	for( int row = 0; row < imgDims.y; ++row )
	{
		memcpy_s( pDestData, bmInfo.bmiHeader.biSizeImage, pSrcPixelData, imgDims.x * 3 );
		pSrcPixelData += imgDims.x * 3;
		pDestData += pitch;
	}

	// Copy the data over
	HDC hDestSurfDC = NULL;
	if( FAILED( pSurface->GetDC( &hDestSurfDC ) ) )
	{
		DeleteObject( hSrcBitmap );
		return;
	}

	HDC hSrcDC = CreateCompatibleDC( hDestSurfDC );
	SelectObject( hSrcDC, hSrcBitmap );

	BitBlt( hDestSurfDC, 0, 0, imgDims.x, imgDims.y, hSrcDC, 0, 0, SRCCOPY );

	DeleteDC( hSrcDC );
	pSurface->ReleaseDC( hDestSurfDC );

	DeleteObject( hSrcBitmap );
}

struct JpegParseData
{
	JpegParseData() : curRow(0),
						pLockSufDesc(0)
	{}

	uint32 curRow;
	DDSURFACEDESC* pLockSufDesc;
};

static int32 GetColorIndex( DWORD bitMask )
{
	return TCBase::GetRightMost1Index(bitMask) / 8;
}

static void StoreJpegData( uint8* pSrcPixel, uint32 numRows, void* pUserData )
{
	JpegParseData* pParseData = (JpegParseData*)pUserData;
	if( !pParseData || !pParseData->pLockSufDesc )
		return;	

	uint8* pDestRowPixelData = (uint8*)pParseData->pLockSufDesc->lpSurface + (pParseData->curRow * pParseData->pLockSufDesc->lPitch);

	int32 redOffset = GetColorIndex( pParseData->pLockSufDesc->ddpfPixelFormat.dwRBitMask );
	int32 greenOffset = GetColorIndex( pParseData->pLockSufDesc->ddpfPixelFormat.dwGBitMask );
	int32 blueOffset = GetColorIndex( pParseData->pLockSufDesc->ddpfPixelFormat.dwBBitMask );
	

	uint32 offset = pParseData->pLockSufDesc->ddpfPixelFormat.dwRGBBitCount / 8;
	for( uint32 rowIndex = 0; rowIndex < numRows; ++rowIndex )
	{
		for( uint32 colIndex = 0; colIndex < pParseData->pLockSufDesc->dwWidth; ++colIndex )
		{
			uint8* pCurPixel = pDestRowPixelData + (colIndex * offset);

			pCurPixel[redOffset] = *pSrcPixel++;
			pCurPixel[greenOffset] = *pSrcPixel++;
			pCurPixel[blueOffset] = *pSrcPixel++;
		}

		pDestRowPixelData += pParseData->pLockSufDesc->lPitch;
		pParseData->curRow++;
	}

}

void FillInImageData( IDirectDrawSurface* pSurface, const Vector2i& imgDims, EImageResourceType imageType, DataBlock imageData )
{
	// Lock the surface
	DDSURFACEDESC lockSurfDesc;
	memset( &lockSurfDesc, 0, sizeof(lockSurfDesc) );
	lockSurfDesc.dwSize = sizeof(lockSurfDesc);
	if( FAILED( pSurface->Lock( NULL, &lockSurfDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL ) ) )
	{
		return;
	}

	// If the surface isn't a normal 24-bit or 32-bit RGB format then use GDI to convert
	if( lockSurfDesc.ddpfPixelFormat.dwRGBBitCount != 24
		&& lockSurfDesc.ddpfPixelFormat.dwRGBBitCount != 32 )
	{
		// Unlock the surface
		pSurface->Unlock( NULL );

		// Use GDI to load the image for color conversion
		FillInImageData_GDI( pSurface, imgDims, (uint8*)imageData.ReadData(imageData.GetRemainingBytes()) );
		return;
	}

	// Get a pointer to the surface data
	uint8* pDestPixelData = (uint8*)lockSurfDesc.lpSurface;

	// If the image is a bitmap
	uint32 imageDataSize = imageData.GetRemainingBytes();
	if( imageType == IRT_Bitmap )
	{
		uint8* pSrcPixelData = (uint8*)imageData.ReadData( imageDataSize );

		// Fill in the data
		int32 pixelStep = lockSurfDesc.ddpfPixelFormat.dwRGBBitCount / 8;
		int32 redOffset = GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwRBitMask );
		int32 greenOffset = GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwGBitMask );
		int32 blueOffset = GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwBBitMask );
		for( int rowIndex = 0; rowIndex < imgDims.y; ++rowIndex )
		{
			for( int xIndex = 0; xIndex < imgDims.x; ++xIndex )
			{
				uint8* pCurPixel = pDestPixelData + (xIndex * pixelStep);
				pCurPixel[redOffset] = *pSrcPixelData++;
				pCurPixel[greenOffset] = *pSrcPixelData++;
				pCurPixel[blueOffset] = *pSrcPixelData++;
			}

			// Step a row
			pDestPixelData += lockSurfDesc.lPitch;
		}
	}
	// Else if the image is a JPEG
	else if( imageType == IRT_Jpeg )
	{
		corona::File* pFile = corona::CreateMemoryFile( imageData.ReadData(imageDataSize), imageDataSize, false );
		if( pFile )
		{
			JpegParseData parseData;
			parseData.pLockSufDesc = &lockSurfDesc;

			// Open the Jpeg and read the data onto our surface
			corona::Image* pImage = corona::OpenJPEG( pFile, StoreJpegData, &parseData );
			if( pImage )
				delete pImage;			

			// Free the file
			delete pFile;
		}
	}
	// Else there is a problem
	else
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Unknown image type: %d", (int)imageType );
	
	// Unlock the surface
	pSurface->Unlock( NULL );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  D3D9DDraw::LoadImageFromMemory  Public
///
///	\param imgDims The image dimensions
///	\param pData The bitmap data in a 24-bit RGB format.
///
///	Create an offscreen DirectDraw surface from image data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
DDrawSurface* D3D9DDraw::LoadImageFromMemory( const Vector2i& imgDims, EImageResourceType imageType, DataBlock imageData )
{
	// Get the primary surface descriptor
	DDSURFACEDESC primDesc;
	memset( &primDesc, 0, sizeof(primDesc) );
	primDesc.dwSize = sizeof(primDesc);
	m_pPrimarySurface->GetSurfaceDesc( &primDesc );

	// Fill in the image surface descriptor data
	DDSURFACEDESC ddrawSurfDesc;
	memset( &ddrawSurfDesc, 0, sizeof(ddrawSurfDesc) );
	ddrawSurfDesc.dwSize = sizeof(ddrawSurfDesc);
	ddrawSurfDesc.dwFlags = DDSD_PIXELFORMAT | DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddrawSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddrawSurfDesc.dwWidth = imgDims.x;
	ddrawSurfDesc.dwHeight = imgDims.y;
	ddrawSurfDesc.ddpfPixelFormat = primDesc.ddpfPixelFormat;
	
	// Create the image surface
	IDirectDrawSurface* pRetSurface = NULL;
	HRESULT hResult;
	if( FAILED( hResult = m_pDDraw->CreateSurface( &ddrawSurfDesc, (IDirectDrawSurface**)&pRetSurface, NULL ) ) )
	{
		MsgLogger::Get().Output( MsgLogger::MI_Error, _T("Failed to create DirectDraw surface with the dimensions %dx%d."), imgDims.x, imgDims.y );
		return NULL;
	}

	// Fill in the surface data
	FillInImageData( pRetSurface, imgDims, imageType, imageData );

	// Set the color key for magenta
	DDCOLORKEY setKey;
	setKey.dwColorSpaceLowValue = m_TransColorKey;
	setKey.dwColorSpaceHighValue = m_TransColorKey;
	pRetSurface->SetColorKey( DDCKEY_SRCBLT, &setKey );

	// Return the surface
	D3D9DrawSurface *pDDrawImage = new D3D9DrawSurface( pRetSurface );
	pDDrawImage->m_Dims = imgDims;

	//pDDrawImage->m_Dims.Set( width, height );
	return (DDrawSurface*)pDDrawImage;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  D3D9DDraw::ReloadImageFromMemory  Public
///
///	\param pImage The image on which to load data
///	\param size The size of the image data to load
///	\param pSrcPixelData The array or RGB values to load onto the surface
///	\returns True on success, false on failure
///
/// Reload the image data for an already created surface
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3D9DDraw::ReloadImageFromMemory( TCImage* pImage, const Vector2i& size, EImageResourceType imageType, DataBlock imageData )
{
	// Ensure valid data
	if( !pImage || size.x <= 0 || size.y <= 0 || imageData.GetRemainingBytes() == 0 )
		return false;

	// Reload the data
	D3D9DrawSurface* pSurface = (D3D9DrawSurface*)pImage->GetImageData();
	pSurface->m_pSurface->Restore();
	FillInImageData( pSurface->m_pSurface, size, imageType, imageData );
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  D3D9DDraw::DisplayScene  Public
///
///	\returns Whether or not the scene was displayed without error
///
///	Display the back buffer to the primary surface.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3D9DDraw::DisplayScene()
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
//  D3D9DDraw::DrawImage  Public
///
///	\param destPos The destination position to draw to
///	\param pSrcImage The image to use for drawing
///	\param srcRect The rectangle on the source image to draw
///	\returns True if there were no problems, false otherwise
///
///	Draw an image to the back buffer
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3D9DDraw::DrawImage( Point2i destPos, const TCImage* const pSrcImage, Box2i srcRect )
{
	// Get the proper image pointer
	D3D9DrawSurface* pDDrawImage1 = reinterpret_cast< D3D9DrawSurface* >( pSrcImage->GetImageData() );
	if( !pDDrawImage1 )
		return false;

	// Clip the image horizontally
	if( destPos.x < 0 )
	{
		srcRect.pos.x += -destPos.x;
		srcRect.size.x -= -destPos.x;
		destPos.x = 0;
	}
	int right = destPos.x + srcRect.size.x;
	if( right > m_BufferSize.x )
		srcRect.size.x -= (right - m_BufferSize.x);

	// Clip the image vertically
	if( destPos.y < 0 )
	{
		srcRect.pos.y += -destPos.y;
		srcRect.size.y -= -destPos.y;
		destPos.y = 0;
	}
	int bottom = destPos.y + srcRect.size.y;
	if( bottom > m_BufferSize.y )
		srcRect.size.y -= (bottom - m_BufferSize.y);

	// If the image is off the screen then bail
	if( srcRect.size.x <= 0 || srcRect.size.y <= 0 )
		return true;

	// Get the rectangles for displaying
	RECT srcDXRect = { srcRect.pos.x, srcRect.pos.y, srcRect.pos.x + srcRect.size.x, srcRect.pos.y + srcRect.size.y };

	// Blit the data to the buffer surface
	if( FAILED( m_pBufferSurface->BltFast( destPos.x, destPos.y, pDDrawImage1->m_pSurface, &srcDXRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY ) ) )
		return false;

	return true;
}

/// Draw an image
bool D3D9DDraw::DrawImage( Box2i destRect, const TCImage* const pSrcImage, Box2i srcRect, int32 fx, float fRotAngle )
{
	// Get the proper image pointer
	D3D9DrawSurface* pDDrawImage1 = reinterpret_cast< D3D9DrawSurface* >( pSrcImage->GetImageData() );
	if( !pDDrawImage1 )
		return false;

	// If the image is off the screen then bail
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
	if( fRotAngle == 90.0f )
		blitFX.dwDDFX |= DDBLTFX_ROTATE90;
	else if( fRotAngle == 180.0f )
		blitFX.dwDDFX |= DDBLTFX_ROTATE180;
	else if( fRotAngle == 270.0f )
		blitFX.dwDDFX |= DDBLTFX_ROTATE270;
	blitFX.dwRotationAngle = (DWORD)( fRotAngle * 100.0f );

	// Blit the data to the buffer surface
	if( FAILED( m_pBufferSurface->Blt( &destDXRect, pDDrawImage1->m_pSurface, &srcDXRect, DDBLT_WAIT | DDBLT_KEYSRC | DDBLT_DDFX, &blitFX ) ) )
		return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  D3D9DDraw::DrawRect  Public
///
///	\param rect The rectangle to draw
///	\param fillColor The color to use for the lines
///	\returns True if there were no problems, false otherwise
///
///	Draw a rectangle
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3D9DDraw::DrawRect( const Box2i& rect, int fillColor )
{
	// Set up the blit FX for filling color
	DDBLTFX bltFX;
	memset( &bltFX, 0, sizeof(bltFX) );
	bltFX.dwSize = sizeof(bltFX);
	bltFX.dwFillColor = fillColor;
	
	// Top
	RECT topRect = { rect.pos.x, rect.pos.y, rect.pos.x + rect.size.x, rect.pos.y + 1 };
	if( FAILED( m_pBufferSurface->Blt( &topRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;
	
	// Right
	RECT rightRect = { rect.pos.x + rect.size.x, rect.pos.y, rect.pos.x + rect.size.x + 1, rect.pos.y + rect.size.y };
	if( FAILED( m_pBufferSurface->Blt( &rightRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;
	
	// Bottom
	RECT bottomRect = { rect.pos.x, rect.pos.y + rect.size.y, rect.pos.x + rect.size.x, rect.pos.y + rect.size.y + 1 };
	if( FAILED( m_pBufferSurface->Blt( &bottomRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;
	
	// Left
	RECT leftRect = { rect.pos.x, rect.pos.y, rect.pos.x + 1, rect.pos.y + rect.size.y };
	if( FAILED( m_pBufferSurface->Blt( &leftRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  D3D9DDraw::FillRect  Public
///
///	\param rect The rectangle to draw
///	\param fillColor The color to use for the fill
///	\returns True if there were no problems, false otherwise
///
///	Fill a rectangular area.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3D9DDraw::FillRect( const Box2i& rect, int fillColor )
{
	// Set up the blit FX for filling color
	DDBLTFX bltFX;
	memset( &bltFX, 0, sizeof(bltFX) );
	bltFX.dwSize = sizeof(bltFX);
	bltFX.dwFillColor = fillColor;
	
	// Fill the rectangle
	RECT fillRect = { rect.pos.x, rect.pos.y, rect.pos.x + rect.size.x, rect.pos.y + rect.size.y };
	if( FAILED( m_pBufferSurface->Blt( &fillRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFX ) ) )
		return false;

	// Return success
	return true;
}