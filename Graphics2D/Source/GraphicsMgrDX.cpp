//=================================================================================================
/*!
	\file GraphicsMgrDX.cpp
	2D Graphics Engine
	DirectX Graphics Manager Source
	\author Taylor Clark

	This source file contains the implementation for the DirectX graphics manager class.
*/
//=================================================================================================

#include "../GraphicsMgr.h"
#include "../PrivateInclude/DrawInterface.h"
#include <ddraw.h>
#include <list>
#include "../PrivateInclude/DDraw1.h"
#include "../PrivateInclude/D3DV9.h"
//#include "../PrivateInclude/D3D9DDraw.h"
#include "../../Math\Vector2i.h"
#include "../RefSprite.h"
#include "Base/StringFuncs.h"
#include "Base/MsgLogger.h"
#include "../CachedFontDraw.h"
#include "../PrivateInclude/TCFontImpl.h"
#include "../PrivateInclude/RefSpriteImpl.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GraphicsMgrDX
	\brief The singleton class for graphics related functionality.

	This class defines the manager for all graphics related functionality.  It uses the singleton
	pattern so that there is only one instance of the class.
*/
//-------------------------------------------------------------------------------------------------
class GraphicsMgrDX : public GraphicsMgrBase
{
	friend class GraphicsMgrBase;
protected:
	
	/// The object that actually does the drawing
	DrawInterface* m_pDDrawObj;

	/// The default constructor, private since this is a singleton class
	GraphicsMgrDX() : m_pDDrawObj( 0 )	
	{
	}

public:
	
	/// The destructor
	virtual ~GraphicsMgrDX()
	{
		Term();
	}

	/// Initialize the graphics manager for use
	virtual bool Init( void* hWnd, bool startFullScreen );

	/// Draw text
	virtual void DrawCachedFontText( const CachedFontDraw& cachedText, uint32 colorTint );

	/// Draw an image
	virtual void DrawImage( const Point2i& destPos, const TCImage* pImage, const Box2i& srcRect, uint32 colorTint );

	/// Draw an image
	virtual void DrawImageEx( const Box2i& destRect, const TCImage* pImage, const Box2i& srcRect, int32 fxFlags, uint32 colorTint );

	/// Draw a rectangle
	virtual bool DrawRect( const Box2i& rect, uint32 lineColor );

	/// Fill a rectangular area
	virtual bool FillRect( const Box2i& rect, uint32 fillColor );

    /// Set the display mode
    virtual void GraphicsMgrDX::SetDisplayMode( GraphicsMgrBase::EDisplayMode displayMode ){UNREFERENCED_PARAMETER( displayMode );}

	/// Apply an effect to an image
	virtual bool ApplyEffect( TCImage* pImage, const Box2i& rect, GraphicsDefines::ModPixelCB pixelCB );
	
	/// Set a temporary render target
	virtual bool SetTempRenderTarget( TCImage* pImage );

	/// Clear any temporary render targets and return to using the back buffer
	virtual void ClearTempRenderTarget();


	/// Load an image from memory
	virtual TCImage* LoadImageFromMemory( uint32 resID, DataBlock* pImageDataBlock );


	/// Reload the image data for a resource
	virtual bool ReloadImageData( TCImage* pImage, DataBlock* pImageDataBlock );

	/// Recreate the DirectDraw surface and load the image data onto it
	virtual bool RecreateImageData( TCImage* pImage, DataBlock* pImageDataBlock );

	/// Free the surface data for a TC Image, used for reloading purposes
	virtual void FreeTCImageSurfaces( std::list< TCImage* > imageList );


	/// Begin drawing a new scene
	virtual bool BeginScene( bool clear = false );

	/// Display the back buffer to the screen
	virtual bool DisplayScene();

	/// Get the display dimensions
	virtual Vector2i GetDisplayDims() const;

	/// Get if the graphics manager is full-screen
	virtual bool IsFullScreen() const;

	/// Close the graphics manager and free any used resources
	virtual void Term();

	/// Check if the surfaces have been lost and images need to be reloaded
	virtual bool ImagesNeedReload() const;

	/// Clear the flag indicating that surfaces need to be reloaded
	virtual void ClearImageReloadFlag();
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrBase::Get  Public
///
///	The graphics manager follows the singleton pattern and this method retrieves the one and only
///	instantiation of the class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsMgrBase& GraphicsMgrBase::Get()
{
	static GraphicsMgrDX s_GfxMgr;
	return s_GfxMgr;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrDX::Init  Public
///
///	Initialize the graphics manager for use
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsMgrDX::Init( void* hWnd, bool startFullScreen )
{
	if( m_pDDrawObj )
	{
		delete m_pDDrawObj;
		m_pDDrawObj = 0;
	}

	m_pDDrawObj = new DDraw1();
	//m_pDDrawObj = new D3DV9();

	// Get the initial screen resolution to help when adjusting video
	//Vector2i initialScreenRes;
	//initialScreenRes.x = inGetSystemMetrics(SM_CXSCREEN);
	//initialScreenRes.y = inGetSystemMetrics(SM_CYSCREEN);

	return m_pDDrawObj->Init( hWnd, startFullScreen );
}


/// Draw text
void GraphicsMgrDX::DrawCachedFontText( const CachedFontDraw& cachedText, uint32 )
{
	// Verify the data
	TCFontImpl* pFontImpl = (TCFontImpl*)cachedText.m_Font.GetObj();
	if( !m_pDDrawObj || !pFontImpl || !pFontImpl->m_Image.GetObj() )
		return;
	const TCImage* pFontImage = pFontImpl->m_Image.GetObj();

	// Draw the characters
	for( uint32 charIndex = 0; charIndex < cachedText.Chars.size(); ++charIndex )
		m_pDDrawObj->DrawImage( cachedText.Chars[charIndex].screenPos, pFontImage, cachedText.Chars[charIndex].srcRect );

	//TODO Enable scaling here and the caching code
		//m_pDDrawObj->DrawImage( cachedText.Chars[charIndex].screenRect, pFontImage, cachedText.Chars[charIndex].srcRect, 0 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrDX::DrawImage  Public
///
///	\param destPos The position of the top left of the iamge in screen coordinates
///	\param pImage The image to draw
///	\param srcRect The rectangle for the source pixels
///
///	Draw an image to the screen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrDX::DrawImage( const Point2i& destPos, const TCImage* pImage, const Box2i& srcRect, uint32 colorTint )
{
    UNREFERENCED_PARAMETER( colorTint );

	if( !m_pDDrawObj || !pImage )
		return;

	m_pDDrawObj->DrawImage( destPos, pImage, srcRect );
}

/// Draw an image with effects
void GraphicsMgrDX::DrawImageEx( const Box2i& destRect, const TCImage* pImage, const Box2i& srcRect, int32 fxFlags, uint32 colorTint )
{
    UNREFERENCED_PARAMETER( colorTint );

	if( !m_pDDrawObj || !pImage )
		return;

	m_pDDrawObj->DrawImageEx( destRect, pImage, srcRect, fxFlags );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrDX::DrawRect  Public
///
///	\param rect The rectangle to draw
///	\param lineColor The color to use for the lines
///	\returns True if there were no problems, false otherwise
///
///	Draw a rectangle
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsMgrDX::DrawRect( const Box2i& rect, uint32 lineColor )
{
	if( !m_pDDrawObj )
		return false;

	return m_pDDrawObj->DrawRect( rect, lineColor );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrDX::FillRect  Public
///
///	\param rect The coordinates to fill in screen pixels
///	\param fillColor The color to use for the fill
///
///	Fill a rectangular area with a color.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsMgrDX::FillRect( const Box2i& rect, uint32 fillColor )
{
	if( !m_pDDrawObj )
		return false;

	return m_pDDrawObj->FillRect( rect, fillColor );
}

bool GraphicsMgrDX::ApplyEffect( TCImage* pImage, const Box2i& rect, GraphicsDefines::ModPixelCB pixelCB )
{
	if( !m_pDDrawObj )
		return false;

	return m_pDDrawObj->ApplyEffect( pImage, rect, pixelCB );
}

/// Set a temporary render target
bool GraphicsMgrDX::SetTempRenderTarget( TCImage* pImage )
{
	if( !m_pDDrawObj )
		return false;

	return m_pDDrawObj->SetTempRenderTarget(pImage);
}

/// Clear any temporary render targets and return to using the back buffer
void GraphicsMgrDX::ClearTempRenderTarget()
{
	if( !m_pDDrawObj )
		return;

	return m_pDDrawObj->ClearTempRenderTarget();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrDX::LoadImageFromMemory  Public
///
///	\param resID The resource ID of the image being loaded
///	\param dataBlock The block of memory containing the image data
///	\returns A pointer to the image, NULL on failure
///
///	Load an image resource
///
///////////////////////////////////////////////////////////////////////////////////////////////////
TCImage* GraphicsMgrDX::LoadImageFromMemory( uint32 resID, DataBlock* pImageDataBlock )
{
	// Read in the dimensions
	Vector2i imgSize;
	imgSize.x = pImageDataBlock->ReadInt32();
	imgSize.y = pImageDataBlock->ReadInt32();

	// Read the image data type
	EImageResourceType imageType = (EImageResourceType)pImageDataBlock->ReadInt32();

	// Get to the pixel data
	uint32 dataLen = pImageDataBlock->GetRemainingBytes();
	DataBlock pixelData( pImageDataBlock->ReadData(dataLen), dataLen );
	
	// Load the image data to surface and return the pointer
	TCImage* pImg = TCImage::Create( resID );
	pImg->SetImageData( m_pDDrawObj->LoadImageFromMemory( imgSize, imageType, &pixelData ) );

	return pImg;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrDX::ReloadImageData  Public
///	\param pImage The image to reload to
///
///	Reload the image data for a resource.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsMgrDX::ReloadImageData( TCImage* pImage, DataBlock* pImageDataBlock )
{
	if( !pImage )
		return false;

	// Read in the dimensions
	Vector2i imgSize;
	imgSize.x = pImageDataBlock->ReadInt32();
	imgSize.y = pImageDataBlock->ReadInt32();

	// Read the image data type
	EImageResourceType imageType = (EImageResourceType)pImageDataBlock->ReadInt32();

	// Get to the pixel data
	uint32 dataLen = pImageDataBlock->GetRemainingBytes();
	DataBlock pixelData( pImageDataBlock->ReadData(dataLen), dataLen );
	
	// Reload the image data
	m_pDDrawObj->ReloadImageFromMemory( pImage, imgSize, imageType, &pixelData );
	return true;
}

/// Recreate the DirectDraw surface and load the image data onto it
bool GraphicsMgrDX::RecreateImageData( TCImage* pImage, DataBlock* pImageDataBlock )
{
	if( !pImage )
		return false;

	if( pImageDataBlock->GetRemainingBytes() < sizeof(int32) * 2 )
		return false;

	// Read in the dimensions
	Vector2i imgSize;
	imgSize.x = pImageDataBlock->ReadInt32();
	imgSize.y = pImageDataBlock->ReadInt32();

	// Read the image data type
	EImageResourceType imageType = (EImageResourceType)pImageDataBlock->ReadInt32();

	// Get the pixel data
	uint32 dataLen = pImageDataBlock->GetRemainingBytes();
	DataBlock pixelData( pImageDataBlock->ReadData(dataLen), dataLen );

	// Free the current data
	if( pImage->GetImageData() )
	{
		delete pImage->GetImageData();
		pImage->SetImageData( 0 );
	}

	// Set the image data
	pImage->SetImageData( m_pDDrawObj->LoadImageFromMemory( imgSize, imageType, &pixelData ) );
	return true;
}


/// Free the surface data for a TC Image, used for reloading purposes
void GraphicsMgrDX::FreeTCImageSurfaces( std::list< TCImage* > imageList )
{
	for( std::list< TCImage* >::iterator iterImage = imageList.begin(); iterImage != imageList.end(); ++iterImage )
	{
		TCImage* pCurImage = *iterImage;
		DDrawSurface* pSurface = (DDrawSurface*)pCurImage->GetImageData();
		if( pSurface )
			pSurface->Release();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrDX::IsFullScreen  Public
///	\returns If the game is being played in full-screen mode
///
///	Get if the graphics manager is full-screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsMgrDX::IsFullScreen() const
{
	if( !m_pDDrawObj )
		return false;
	return m_pDDrawObj->IsFullScreen();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrDX::BeginScene  Public
///	\param clear Whether or not to clear the back buffer after displaying the scene
///	\returns If the scene was successfully started.  On failure, check if the image data was lost
///			since that is the leading cause of this function failing.
///
///	Begin a new scene, clearing the buffer if needed
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsMgrDX::BeginScene(  bool clear )
{
	// If we don't have a DirectDraw object then bail
	if( !m_pDDrawObj )
		return true;

	// Display the back buffer
	if( !m_pDDrawObj->BeginScene( clear ) )
		return false;

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrDX::DisplayScene  Public
///	\returns If the scene was successfully displayed.  On failure, check if the image data was lost
///			since that is the leading cause of this function failing.
///
///	Display the back buffer to the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsMgrDX::DisplayScene()
{
	// If we don't have a DirectDraw object then bail
	if( !m_pDDrawObj )
		return true;

	// Display the back buffer
	if( !m_pDDrawObj->DisplayScene() )
		return false;

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrDX::ImagesNeedReload  Public
///
///	\returns True if the surface data needs to be reloaded, false otherwise
///
///	Check if the surfaces have been lost and images need to be reloaded.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsMgrDX::ImagesNeedReload() const
{
	if( !m_pDDrawObj )
		return false;
	return m_pDDrawObj->IsSurfaceDataLost();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrDX::ClearImageReloadFlag  Public
///
///	Clear the flag indicating that surfaces need to be reloaded.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrDX::ClearImageReloadFlag()
{
	if( m_pDDrawObj )
		m_pDDrawObj->ClearSurfaceDataLostFlag();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrDX::Term  Public
///
///	Clean up the graphics manager
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrDX::Term()
{
	if( !m_pDDrawObj )
		return;

	m_pDDrawObj->Term();

	// Free the DirectDraw object
	delete m_pDDrawObj;
	m_pDDrawObj = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GraphicsMgrDX::GetDisplayDims  Public
///
///	\returns The dimensions of the scene
///
///	Get the display dimensions.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Vector2i GraphicsMgrDX::GetDisplayDims() const 
{
	if( !m_pDDrawObj )
		return Vector2i();
	return m_pDDrawObj->GetDisplayDims();
}