//=================================================================================================
/*!
	\file GraphicsMgrSFML.cpp
	2D Graphics Engine
	SFML Graphics Manager Source
	\author Taylor Clark
	\date March 10, 2010

	This source file contains the implementation for the Simple Fast Media Layer (SFML) graphics
	manager class.
*/
//=================================================================================================
 
#include "../GraphicsMgr.h"
#include "../PrivateInclude/DrawInterface.h"
#include <list>
#include "Math/Vector2i.h"
#include "Math/Vector2.h"
#include "../../Math/Point2i.h"
#include "../../Math/Box2i.h"
#include "../RefSprite.h"
#include "Base/StringFuncs.h"
#include "Base/MsgLogger.h"
#include "../CachedFontDraw.h"
#include "../PrivateInclude/TCFontImpl.h"
#include "../PrivateInclude/RefSpriteImpl.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include "../PrivateInclude/ImageLoadingTypes.h"
#include "Base/TCAssert.h"


Vector2i GraphicsMgrBase::DesktopDims;


//-------------------------------------------------------------------------------------------------
/*!
	\class GraphicsMgrSFML
	\brief The singleton class for graphics related functionality.

	This class defines the manager for all SFML graphics related functionality.  It uses the
	singleton pattern so that there is only one instance of the class.
*/
//-------------------------------------------------------------------------------------------------
class GraphicsMgrSFML : public GraphicsMgrBase
{
	friend class GraphicsMgrBase;
private:

	sf::RenderWindow* _pRenderWindow;

	/// The temporary render target, if any
	sf::Image* _pTempTargetImage;

    /// The current texture image
    sf::Image* _pCurrentImage;

    /// The current texture, cached to avoid re-loading it to video memory
    sf::Texture _currentTexture;

	/// The default constructor, private to enforce a singleton
	GraphicsMgrSFML() : _pRenderWindow( 0 ),
						_pTempTargetImage( 0 ),
                        _pCurrentImage( NULL )
	{
	}

	void FillInImageData( sf::Image* pDestImage, const Vector2i& imgDims, EImageResourceType imageType, DataBlock* pImageDataBlock )
	{
		// Get the source pixel data
		uint32 srcPixelDataSize = pImageDataBlock->GetRemainingBytes();
		uint8* pSrcPixelData = (uint8*)pImageDataBlock->ReadData( srcPixelDataSize );

		// Get a pointer to the surface data
		uint32 destPixelDataSize = imgDims.x * imgDims.y * 4;
		uint8* pDestPixelData = (uint8*)new uint8[destPixelDataSize];

		if( imageType == IRT_Jpeg )
		{
			pDestImage->loadFromMemory( reinterpret_cast<const char*>(pSrcPixelData), srcPixelDataSize);
		}
		else
		{
			// Initialize the source pixel data
			SrcImgData* pSrcImgData = 0;
			if( imageType == IRT_Bitmap )
				pSrcImgData = new BmpSrcData(pSrcPixelData, imgDims.x * imgDims.y );
			else if( imageType == IRT_BitmapRLE )
				pSrcImgData = new BitmapRLESrcData( pSrcPixelData, srcPixelDataSize );
			// Else there is a problem
			else
				TCBREAKX( L"Unknown image type" );

			// Initialize the destination surface object
			DestImageData destImg( pDestPixelData );
			destImg.Init( 0, 1, 2, 4, imgDims.x * 4, imgDims );

			// Store the image data
			destImg.SetPixelsWithAlpha( pSrcImgData );
			delete pSrcImgData;
			pSrcImgData = 0;

			pDestImage->create( imgDims.x, imgDims.y, pDestPixelData );
		}

		delete [] pDestPixelData;
	}

public:
	
	/// Initialize the graphics manager for use
	virtual bool Init( void* pWnd, bool )
	{
        _pRenderWindow = static_cast<sf::RenderWindow*>( pWnd );

        return true;
	}

	/// Draw text
	virtual void DrawCachedFontText( const CachedFontDraw& cachedText, uint32 colorTint )
	{
		// Verify the data
		TCFontImpl* pFontImpl = (TCFontImpl*)cachedText.m_Font.GetObj();
		if( !_pRenderWindow || !pFontImpl || !pFontImpl->m_Image.GetObj() )
			return;
		const TCImage* pFontImage = pFontImpl->m_Image.GetObj();

        if( !pFontImage )
			return;

		// If we are using a temporary render target then render to that target
		if( _pTempTargetImage )
		{
			for( uint32 charIndex = 0; charIndex < cachedText.Chars.size(); ++charIndex )
			    DrawImage( cachedText.Chars[charIndex].screenPos, pFontImage, cachedText.Chars[charIndex].srcRect, colorTint );

			return;
		}

		sf::Image* pSFMLImage = static_cast<sf::Image*>( pFontImage->GetImageData() );

        // If the active texture isn't using the correct image
        if( pSFMLImage != _pCurrentImage )
        {
            _pCurrentImage = pSFMLImage;
            _currentTexture.loadFromImage( *pSFMLImage );
            _currentTexture.setSmooth(false);            
        }

		sf::Sprite sprite( _currentTexture );
		sprite.setColor( IntToColor(colorTint) );

		// Draw the characters
		for( uint32 charIndex = 0; charIndex < cachedText.Chars.size(); ++charIndex )
        {
            const CachedFontDraw::Char &curChar = cachedText.Chars[charIndex];

            sprite.setPosition((float)curChar.screenPos.x, (float)curChar.screenPos.y);
		    sprite.setTextureRect( sf::IntRect(curChar.srcRect.pos.x, curChar.srcRect.pos.y, curChar.srcRect.size.x, curChar.srcRect.size.y ) );            

		    _pRenderWindow->draw(sprite);
        }

		//TODO Enable scaling here and the caching code
		//DrawImage( cachedText.Chars[charIndex].screenRect, pFontImage, cachedText.Chars[charIndex].srcRect, 0 );
	}

	/// Draw an image
	virtual void DrawImage( const Point2i& destPos, const TCImage* pImage, const Box2i& srcRect, uint32 colorTint )
	{
		if( !pImage )
			return;

		sf::Image* pSFMLImage = static_cast<sf::Image*>( pImage->GetImageData() );

		// If we are using a temporary render target then render to that target
		if( _pTempTargetImage )
		{
			if( srcRect.size.x == 0 || srcRect.size.y == 0 )
				return;

			_pTempTargetImage->copy( *pSFMLImage, destPos.x, destPos.y, sf::IntRect(srcRect.pos.x, srcRect.pos.y, srcRect.size.x, srcRect.size.y), true);
			return;
		}

        // If the active texture isn't using the correct image
        if( pSFMLImage != _pCurrentImage )
        {
            _pCurrentImage = pSFMLImage;
            _currentTexture.loadFromImage( *_pCurrentImage );
            _currentTexture.setSmooth(false);            
        }
        
		sf::Sprite sprite( _currentTexture );
		sprite.setPosition((float)destPos.x, (float)destPos.y);
		sprite.setTextureRect( sf::IntRect(srcRect.pos.x, srcRect.pos.y, srcRect.size.x, srcRect.size.y ) );
        sprite.setColor( IntToColor(colorTint) );

		_pRenderWindow->draw(sprite);
	}

    sf::Color IntToColor( uint32 argbColor )
	{
		return sf::Color( (sf::Uint8)((argbColor & 0xFF0000) >> 16), (sf::Uint8)((argbColor & 0x00FF00) >> 8), (sf::Uint8)(argbColor & 0x0000FF), (sf::Uint8)((argbColor & 0xFF000000) >> 24) );
	}

	uint32 ColorToInt( sf::Color color )
	{
		return (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
	}

	/// Draw an image
	virtual void DrawImageEx( const Box2i& destRect, const TCImage* pImage, const Box2i& srcRect, int32 fxFlags, uint32 colorTint )
	{
		if( !pImage )
			return;

		// If we are using a temporary render target then bail because SFML can't render to texture
		// with effects
		if( _pTempTargetImage )
			return;

		sf::Image* pSFMLImage = static_cast<sf::Image*>( pImage->GetImageData() );
        
        // If the active texture isn't using the correct image
        if( pSFMLImage != _pCurrentImage )
        {
            _pCurrentImage = pSFMLImage;
            _currentTexture.loadFromImage( *_pCurrentImage );
            _currentTexture.setSmooth(false);            
        }

		sf::Sprite sprite( _currentTexture );

		sprite.setPosition((float)destRect.pos.x, (float)destRect.pos.y); 
		sprite.setTextureRect( sf::IntRect(srcRect.pos.x, srcRect.pos.y, srcRect.size.x, srcRect.size.y ) );

		// Set the rotation based on the flags
		float rotation = 0.0f;
		if( fxFlags & GraphicsDefines::DEF_Rotate_90 )
			rotation = 90.0f;
		else if( fxFlags & GraphicsDefines::DEF_Rotate_180 )
			rotation = 180.0f;
		else if( fxFlags & GraphicsDefines::DEF_Rotate_270 )
			rotation = 270.0f;
		sprite.setRotation( rotation );

		sf::Vector2f flipScalar(1.0f,1.0f);
		if( fxFlags & GraphicsDefines::DEF_FlipHoriz )
        {
			flipScalar.x = -1.0f;
            sprite.setPosition( (float)destRect.pos.x + srcRect.size.x, (float)destRect.pos.y );
        }
		else if( fxFlags & GraphicsDefines::DEF_FlipVert )
			flipScalar.y = -1.0f;

		sprite.setScale( flipScalar.x * (float)destRect.size.x / (float)srcRect.size.x, flipScalar.y * (float)destRect.size.y / (float)srcRect.size.y );

        sprite.setColor( IntToColor(colorTint) );

		_pRenderWindow->draw(sprite);
	}

	
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //  GraphicsMgrSFML::NextDisplayMode  Public
    ///
    /// Set the display mode
    ///////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetDisplayMode( EDisplayMode displayMode )
    {
        _activeDisplayMode = displayMode;

        switch( _activeDisplayMode )
        {
        case DM_NormalFill:
        case DM_Windowed:
            _pRenderWindow->setView( _pRenderWindow->getDefaultView() );
            break;

        case DM_WideScreen:
            {
                sf::View view( sf::FloatRect(0,0,1024,768) );

                // Maximize the height
                float heightScalar = (float)DesktopDims.y / 768.0f;

                // Center the view inside the screen
                float xSize = heightScalar * 1024.0f;
                xSize /= (float)DesktopDims.x;

                view.setViewport( sf::FloatRect( (1.0f - xSize) / 2.0f, 0.0f, xSize, 1.0f ) );
                _pRenderWindow->setView( view );
            }
            break;

        case DM_CenteredDesktop:
            {
                sf::View view( sf::FloatRect(0,0,1024,768) );

                // Center the view inside the screen
                Vector2 size( 1024.0f / (float)DesktopDims.x, 768.0f / (float)DesktopDims.y );
                Vector2 pos( 1.0f - size.x, 1.0f - size.y );
                pos *= 0.5f;

                view.setViewport( sf::FloatRect( pos.x, pos.y, size.x, size.y ) );
                _pRenderWindow->setView( view );
            }
            break;
        }
    }


	///////////////////////////////////////////////////////////////////////////////////////////////
    //  GraphicsMgrSFML::DrawRect  Public
    ///
    /// Draw the outline of a rectangle.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool DrawRect( const Box2i& rect, uint32 lineColor )
	{
		sf::RectangleShape sfmlRect = sf::RectangleShape( sf::Vector2f( (float)rect.size.x, (float)rect.size.y ) );
		sfmlRect.setPosition( (float)rect.pos.x, (float)rect.pos.y );
		
		sfmlRect.setOutlineColor( IntToColor(lineColor) );
		sfmlRect.setOutlineThickness( 1.0f );
		sfmlRect.setFillColor( sf::Color::Transparent );

		_pRenderWindow->draw( sfmlRect );

		return true;
	}

	/// Fill a rectangular area
	virtual bool FillRect( const Box2i& rect, uint32 fillColor )
	{
		sf::RectangleShape sfmlRect = sf::RectangleShape( sf::Vector2f( (float)rect.size.x, (float)rect.size.y ) );
		
		sfmlRect.setPosition( (float)rect.pos.x, (float)rect.pos.y );
		sfmlRect.setFillColor( IntToColor(fillColor) );

		_pRenderWindow->draw( sfmlRect );

		return true;
	}


	/// Apply an effect to an image
	virtual bool ApplyEffect( TCImage* pImage, const Box2i& rect, GraphicsDefines::ModPixelCB pixelCB )
	{
		sf::Image* pSFMLImage = static_cast<sf::Image*>( pImage->GetImageData() );

		const sf::Color trans(255,0,255);

		for( int32 row = 0; row < rect.size.y; ++row )
		{
			for( int32 col = 0; col < rect.size.x; ++col )
			{
				sf::Color curColor = pSFMLImage->getPixel( rect.pos.x + col, rect.pos.y + row );
				if( curColor != trans )
				{
					curColor = IntToColor( pixelCB( ColorToInt(curColor) ) );

					pSFMLImage->setPixel( rect.pos.x + col, rect.pos.y + row, curColor );
				}
			}
		}

		return true;
	}

	/// Set a temporary render target
	virtual bool SetTempRenderTarget( TCImage* pImage )
	{
		_pTempTargetImage = static_cast<sf::Image*>( pImage->GetImageData() );

		return true;
	}

	/// Clear any temporary render targets and return to using the back buffer
	virtual void ClearTempRenderTarget()
	{
		_pTempTargetImage = NULL;
	}


	/// Load an image from memory
	virtual TCImage* LoadImageFromMemory( uint32 resID, DataBlock* pImageDataBlock )
	{
		// Read in the dimensions
		Vector2i imgDims;
		imgDims.x = pImageDataBlock->ReadInt32();
		imgDims.y = pImageDataBlock->ReadInt32();

		// Read the image data type
		EImageResourceType imageType = (EImageResourceType)pImageDataBlock->ReadInt32();

		// Get to the pixel data
		uint32 dataLen = pImageDataBlock->GetRemainingBytes();
		DataBlock pixelData( pImageDataBlock->ReadData(dataLen), dataLen );
		
		// Load the image data to surface and return the pointer
		TCImage* pImg = TCImage::Create( resID );
		sf::Image* pNewImage = new sf::Image();
		pNewImage->create( imgDims.x, imgDims.y );
		FillInImageData( pNewImage, imgDims, imageType, &pixelData );

		pImg->SetImageData( pNewImage );

		return pImg;
	}

	
	/// Reload the image data for a resource
	virtual bool ReloadImageData( TCImage* pImage, DataBlock* pImageDataBlock )
	{
		sf::Image* pDestImage = static_cast<sf::Image*>( pImage->GetImageData() );

		// Read in the dimensions
		Vector2i imgDims;
		imgDims.x = pImageDataBlock->ReadInt32();
		imgDims.y = pImageDataBlock->ReadInt32();

		// Read the image data type
		EImageResourceType imageType = (EImageResourceType)pImageDataBlock->ReadInt32();

		// Get to the pixel data
		uint32 dataLen = pImageDataBlock->GetRemainingBytes();
		DataBlock pixelData( pImageDataBlock->ReadData(dataLen), dataLen );
		
		// Load the image data to surface and return the pointer
		FillInImageData( pDestImage, imgDims, imageType, &pixelData );

		// No need to do this with SFML, as far as I know
		return true;
	}

	/// Recreate the DirectDraw surface and load the image data onto it
	virtual bool RecreateImageData( TCImage*, DataBlock* )
	{
		// No need to do this with SFML, as far as I know
		return false;
	}

	/// Free the surface data for a TC Image, this method is used for reloading purposes
	virtual void FreeTCImageSurfaces( std::list< TCImage* > )
	{
		// This shouldn't be needed for SFML
	}


	/// Begin drawing a new scene
	virtual bool BeginScene( bool clear = false )
	{
		if( clear )
			_pRenderWindow->clear();

		return true;
	}

	/// Display the back buffer to the screen
	virtual bool DisplayScene()
	{
		_pRenderWindow->display();

		return true;
	}

	/// Get the display dimensions
	virtual Vector2i GetDisplayDims() const
	{
		//return Vector2i( _pRenderWindow->getSize().x, _pRenderWindow->getSize().y );
        return Vector2i( 1024, 768 );
	}

	/// Get if the graphics manager is full-screen
	virtual bool IsFullScreen() const
	{
		return false;
	}

	/// Close the graphics manager and free any used resources
	virtual void Term()
	{
	}

	/// Check if the surfaces have been lost and images need to be reloaded
	virtual bool ImagesNeedReload() const
	{
		return false;
	}

	/// Clear the flag indicating that surfaces need to be reloaded
	virtual void ClearImageReloadFlag()
	{
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::Get  Public
///
///	The graphics manager follows the singleton pattern and this method retrieves the one and only
///	instantiation of the class.
///////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsMgrBase& GraphicsMgrBase::Get()
{
	static GraphicsMgrSFML s_GfxMgr;
	return s_GfxMgr;
}