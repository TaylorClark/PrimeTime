#include "..\PrivateInclude\DDraw1.h"
#include <windows.h>
#include <wingdi.h>
#include "Base/MsgLogger.h"
#include "Base/StringFuncs.h"
#include "Base/NumFuncs.h"
//#include <d3dx9math.h>
#include "..\PrivateInclude\ImageLoadingTypes.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  StoreJpegDataOnDIBCB  Global
///
///	\param pSrcPixel The decompressed jpeg image data
///	\param numRows The number of rows that were decompressed
///	\param pUserData The object passed by the user to the jpeg loading function
///
///	Store image data from a jpeg image as it is decompressed to a DIB. This is a callback function
///	invoked by the JPEG loader.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
//static void StoreJpegDataOnDIBCB( uint8* pSrcPixel, uint32 numRows, void* pUserData )
//{
//	JpegGDIParseData* pParseData = (JpegGDIParseData*)pUserData;
//	if( !pParseData )
//		return;	
//
//	uint8* pDestRowPixelData = (uint8*)pParseData->pDestData + (pParseData->curRow * pParseData->dibPitch);
//
//	const int32 redOffset = 2;
//	const int32 greenOffset = 1;
//	const int32 blueOffset = 0;
//	const int32 BYTES_PER_PIXEL = 3;
//	
//	for( uint32 rowIndex = 0; rowIndex < numRows; ++rowIndex )
//	{
//		for( uint32 colIndex = 0; colIndex < pParseData->jpegWidth; ++colIndex )
//		{
//			uint8* pCurPixel = pDestRowPixelData + (colIndex * BYTES_PER_PIXEL);
//
//			pCurPixel[redOffset] = *pSrcPixel++;
//			pCurPixel[greenOffset] = *pSrcPixel++;
//			pCurPixel[blueOffset] = *pSrcPixel++;
//		}
//
//		pDestRowPixelData += pParseData->dibPitch;
//		pParseData->curRow++;
//	}
//
//}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FillInImageData_GDI  Global
///
///	\param pSurface The DirectDraw surface that is to be loaded with data
///	\param imgDims The size of the image to be created
///	\param imageType The type of image that is being loaded such as bitmap or jpeg
///	\param pixelData The image data that is loaded to the surface
///
///	Load a DirectDraw surface with image data using Windows GDI to perform color transformation as
///	needed.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static void FillInImageData_GDI( IDirectDrawSurface* pSurface, const Vector2i& imgDims, EImageResourceType imageType, DataBlock* pPixelDataBlock )
{
	// Setup the DIB info
	BITMAPINFO bmInfo;
	memset( &bmInfo, 0, sizeof(bmInfo) );
	bmInfo.bmiHeader.biBitCount = 24;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biHeight = -imgDims.y;
	bmInfo.bmiHeader.biWidth = imgDims.x;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biSizeImage = imgDims.x * imgDims.y * 3;
	bmInfo.bmiHeader.biSize = sizeof(bmInfo.bmiHeader);

	// Create the DIB into which the 24-bit data will be copied
	uint8* pDestData = NULL;
	HBITMAP hSrcBitmap = CreateDIBSection( NULL, &bmInfo, DIB_RGB_COLORS, (void**)&pDestData, NULL, 0 );
	if( hSrcBitmap == NULL )
		return;

	// Get the pixel data
	uint32 pixelDataSize = pPixelDataBlock->GetRemainingBytes();
	uint8* pPixelData = (uint8*)pPixelDataBlock->ReadData( pixelDataSize );

	// Get the DIB pitch
	uint32 dibPitch = ((imgDims.x * 24 + 31) & ~31) >> 3;

	// If the image is a jpeg
	SrcImgData* pSrcImgData = 0;
	if( imageType == IRT_Bitmap )
		pSrcImgData = new BmpSrcData(pPixelData, imgDims.x * imgDims.y );
	// Else if the image is a bitmap
	else if( imageType == IRT_Jpeg )
		pSrcImgData = new JPEGSrcData(pPixelData, pixelDataSize, imgDims.x );
	else if( imageType == IRT_BitmapRLE )
		pSrcImgData = new BitmapRLESrcData( pPixelData, pixelDataSize );
	// Else there is a problem
	else
	{
		TCBREAKX( L"Unknown image type" );
	}

	// Create the DIB destination image object
	DestImageData destImg( pDestData );
	destImg.Init( 2, 1, 0, 3, dibPitch, imgDims );

	// Store the image data
	destImg.SetPixels( pSrcImgData );

	// Free the source data
	delete pSrcImgData;
	pSrcImgData = 0;

	// Get the device context for the DirectDraw surface
	HDC hDestSurfDC = NULL;
	if( FAILED( pSurface->GetDC( &hDestSurfDC ) ) )
	{
		DeleteObject( hSrcBitmap );
		return;
	}

	// Get the device contxt to select the DIB
	HDC hSrcDC = CreateCompatibleDC( hDestSurfDC );
	SelectObject( hSrcDC, hSrcBitmap );

	// Draw the DIB to the DDraw surface
	BitBlt( hDestSurfDC, 0, 0, imgDims.x, imgDims.y, hSrcDC, 0, 0, SRCCOPY );

	// Release the GDI objects
	DeleteDC( hSrcDC );
	pSurface->ReleaseDC( hDestSurfDC );
	DeleteObject( hSrcBitmap );
}


struct JpegDDSurfParseData
{
	JpegDDSurfParseData() : curRow(0),
						pLockSufDesc(0)
	{}

	uint32 curRow;
	DDSURFACEDESC* pLockSufDesc;
};

int32 GetColorIndex( DWORD bitMask )
{
	return TCBase::GetRightMost1Index(bitMask) / 8;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FillInImageData  Global
///
///	\param pSurface The surface to which the image data will be copied
///	\param imgDims The size of the image to be created
///	\param imageType Whether the image is a bitmap, jpeg, etc.
///	\param imageData The pixel data for the image
///
///	Fill in a DirectDraw surface with image data
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static void FillInImageData( IDirectDrawSurface* pSurface, const Vector2i& imgDims, EImageResourceType imageType, DataBlock* pImageDataBlock )
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
		FillInImageData_GDI( pSurface, imgDims, imageType, pImageDataBlock );
		return;
	}

	// Get the source pixel data
	uint32 srcPixelDataSize = pImageDataBlock->GetRemainingBytes();
	uint8* pSrcPixelData = (uint8*)pImageDataBlock->ReadData( srcPixelDataSize );

	// Get a pointer to the surface data
	uint8* pDestPixelData = (uint8*)lockSurfDesc.lpSurface;

	// Initialize the source pixel data
	SrcImgData* pSrcImgData = 0;
	if( imageType == IRT_Bitmap )
		pSrcImgData = new BmpSrcData(pSrcPixelData, imgDims.x * imgDims.y );
	else if( imageType == IRT_Jpeg )
		pSrcImgData = new JPEGSrcData(pSrcPixelData, srcPixelDataSize, imgDims.x );
	else if( imageType == IRT_BitmapRLE )
		pSrcImgData = new BitmapRLESrcData( pSrcPixelData, srcPixelDataSize );
	// Else there is a problem
	else
	{
		TCBREAKX( L"Unknown image type" );
	}

	// Initialize the destination surface object
	DestImageData destImg( pDestPixelData );
	destImg.Init( GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwRBitMask ),
					GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwGBitMask ),
					GetColorIndex( lockSurfDesc.ddpfPixelFormat.dwBBitMask ),
					lockSurfDesc.ddpfPixelFormat.dwRGBBitCount / 8,
					lockSurfDesc.lPitch,
					imgDims );

	// Store the image data
	destImg.SetPixels( pSrcImgData );
	delete pSrcImgData;
	pSrcImgData = 0;

	// Unlock the surface
	pSurface->Unlock( NULL );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  DDraw1::LoadImageFromMemory  Public
///	\param imgDims The image dimensions
///	\param pData The bitmap data in a 24-bit RGB format.
///
///	Create an offscreen DirectDraw surface from image data.
///////////////////////////////////////////////////////////////////////////////////////////////////
DDrawSurface* DDraw1::LoadImageFromMemory( const Vector2i& imgDims, EImageResourceType imageType, DataBlock* pImageDataBlock )
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
		MsgLogger::Get().Output( MsgLogger::MI_Error, L"Failed to create DirectDraw surface with the dimensions %dx%d.", imgDims.x, imgDims.y );
		return NULL;
	}

	// Fill in the surface data
	FillInImageData( pRetSurface, imgDims, imageType, pImageDataBlock );

	// Set the color key for magenta
	DDCOLORKEY setKey;
	setKey.dwColorSpaceLowValue = m_TransColorKey;
	setKey.dwColorSpaceHighValue = m_TransColorKey;
	pRetSurface->SetColorKey( DDCKEY_SRCBLT, &setKey );

	// Return the surface
	DDrawSurface1 *pDDrawImage = new DDrawSurface1( pRetSurface );
	pDDrawImage->m_Dims = imgDims;
	pDDrawImage->m_BPP = primDesc.ddpfPixelFormat.dwRGBBitCount;

	//pDDrawImage->m_Dims.Set( width, height );
	return (DDrawSurface*)pDDrawImage;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DDraw1::ReloadImageFromMemory  Public
///
///	\param pImage The image on which to load data
///	\param size The size of the image data to load
///	\param pSrcPixelData The array or RGB values to load onto the surface
///	\returns True on success, false on failure
///
/// Reload the image data for an already created surface
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DDraw1::ReloadImageFromMemory( TCImage* pImage, const Vector2i& size, EImageResourceType imageType, DataBlock* pImageDataBlock )
{
	// Ensure valid data
	if( !pImage || size.x <= 0 || size.y <= 0 || pImageDataBlock->GetRemainingBytes() == 0 )
		return false;

	// Reload the data
	DDrawSurface1* pSurface = (DDrawSurface1*)pImage->GetImageData();
	pSurface->m_pSurface->Restore();
	FillInImageData( pSurface->m_pSurface, size, imageType, pImageDataBlock );
	return true;
}