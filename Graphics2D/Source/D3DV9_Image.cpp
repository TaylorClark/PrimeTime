//=================================================================================================
/*!
	\file D3DV9_Image.cpp
	Graphics Library
	D3D Image Loading Functions
	\author Taylor Clark
	\date December 16, 2009

	This source file contains the functions used to load images for Direct3D.
*/
//=================================================================================================

#include "..\PrivateInclude\ImageLoadingTypes.h"
//#include <d3dx9math.h>
#include <d3dx9tex.h>
#include "..\PrivateInclude\D3DV9.h"


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
static void FillInImageData( IDirect3DSurface9* pSurface, const Vector2i& imgDims, EImageResourceType imageType, DataBlock* pImageDataBlock )
{
	// Lock the surface
	D3DSURFACE_DESC lockSurfDesc;
	pSurface->GetDesc( &lockSurfDesc );

	D3DLOCKED_RECT lockedRect;
	if( FAILED( pSurface->LockRect( &lockedRect, NULL, 0 ) ) )
	{
		return;
	}

	// Get the source pixel data
	uint32 srcPixelDataSize = pImageDataBlock->GetRemainingBytes();
	uint8* pSrcPixelData = (uint8*)pImageDataBlock->ReadData( srcPixelDataSize );

	// Get a pointer to the surface data
	uint8* pDestPixelData = (uint8*)lockedRect.pBits;

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
	destImg.Init( 2,
					1,
					0,
					4,
					lockedRect.Pitch,
					imgDims );

	// Store the image data
	destImg.SetPixelsWithAlpha( pSrcImgData );
	delete pSrcImgData;
	pSrcImgData = 0;
	
	// Unlock the surface
	pSurface->UnlockRect();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::LoadImageFromMemory  Public
///	\param imgDims The size of the image to load
///	\param imageType The type of information specified by the imageData parameter
///	\param imageData The actual image information
///	\returns A pointer to the loaded image, or NULL on failure
///
/// Create an offscreen image surface and load image data onto it
///////////////////////////////////////////////////////////////////////////////////////////////////
DDrawSurface* D3DV9::LoadImageFromMemory( const Vector2i& imgDims, EImageResourceType imageType, DataBlock* pImageDataBlock )
{
	IDirect3DTexture9* pNewTexture = NULL;
	D3DXCreateTexture( m_pD3DDevice, imgDims.x, imgDims.y, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pNewTexture );
	
	IDirect3DSurface9* pSurface = NULL;
	pNewTexture->GetSurfaceLevel( 0, &pSurface );
	FillInImageData( pSurface, imgDims, imageType, pImageDataBlock );

	D3DV9Texture* pNewSurface = new D3DV9Texture(pNewTexture);
	pNewSurface->m_Dims = imgDims;

	return pNewSurface;
}

/// Reload the image data for a surface
bool D3DV9::ReloadImageFromMemory( TCImage* pImage, const Vector2i& size, EImageResourceType imageType, DataBlock* pImageDataBlock )
{
	// Ensure valid data
	if( !pImage || size.x <= 0 || size.y <= 0 || pImageDataBlock->GetRemainingBytes() == 0 )
		return false;

	// Reload the data
	D3DV9Texture* pSurface = (D3DV9Texture*)pImage->GetImageData();
	IDirect3DSurface9* pD3DSurface = NULL;

	if( pSurface->m_pTexture != NULL )
		pSurface->m_pTexture->Release();

	D3DXCreateTexture( m_pD3DDevice, size.x, size.y, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pSurface->m_pTexture );

	pSurface->m_pTexture->GetSurfaceLevel( 0, &pD3DSurface );
	FillInImageData( pD3DSurface, size, imageType, pImageDataBlock );

	return true;
}