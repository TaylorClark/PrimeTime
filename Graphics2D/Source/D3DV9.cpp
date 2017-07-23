//=================================================================================================
/*!
	\file D3DV9.cpp
	2D Graphics Engine
	Direct3D Renderer Source
	\author Taylor Clark
	\date December 19, 2007

	This source file contains the implementation for the Direct3D version 9 rendering class.
*/
//=================================================================================================

#include "../PrivateInclude/D3DV9.h"
#include <d3dx9math.h>
#include "Math/Point2.h"
#include "Math/Box2.h"

// The fancy sprite vertex structure
struct SpriteVertexEx
{
	// The 3D coordinates
	D3DXVECTOR3 position;

	// The texture coordinates
	FLOAT       tu, tv;
};
#define D3DFVF_SpriteVertexExFlags (D3DFVF_XYZ | D3DFVF_TEX1)

// The fancy sprite vertex structure
struct ShapeVertexEx
{
	// The 3D coordinates
	D3DXVECTOR3 position;

	// The color mod
	DWORD diffuse;
};
#define D3DFVF_ShapeVertexFlags (D3DFVF_XYZ | D3DFVF_DIFFUSE )



///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::BeginScene  Public
///	\param clear Set to true to clear the buffer before starting the new scene
///	\returns Whether or not the scene was started successfully
///
///	Start a new scene.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3DV9::BeginScene( bool clear )
{
	// Ensure our device is valid
	if( m_pD3DDevice == NULL )
		return false;

	
	// Clear the back buffer
	if( clear )
	{
		m_pD3DDevice->Clear( 0,
							NULL,
							D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
							D3DCOLOR_XRGB( 128, 128, rand() % 255 ),
							1.0f,
							0 );
	}

	// Start the scene
	if( FAILED( m_pD3DDevice->BeginScene() ) )
		return false;

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::DisplayScene  Public
///	\returns True if the display was successful, otherwise false
///
///	Display the scene to the window
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3DV9::DisplayScene()
{
	// Ensure our device is valid
	if( m_pD3DDevice == NULL )
		return false;

	// End rendering
	m_pD3DDevice->EndScene();
	
	// Display the scene
	m_pD3DDevice->Present( NULL, NULL, NULL, NULL );

	return true;
}

static void MakeSpriteRect( SpriteVertexEx* pVertices, const Box2& destRect, const Box2& srcTextureCoords )
{
	int32 vertIndex = 0;
	
	// Top left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].tu = srcTextureCoords.pos.x; pVertices[vertIndex].tv = srcTextureCoords.pos.y;
	++vertIndex;

	// Top right
	pVertices[vertIndex].position.x = destRect.Right();
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].tu = srcTextureCoords.Right(); pVertices[vertIndex].tv = srcTextureCoords.pos.y;
	++vertIndex;

	// Bottom Left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.Bottom();
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].tu = srcTextureCoords.pos.x; pVertices[vertIndex].tv = srcTextureCoords.Bottom();
	++vertIndex;


	// Top Right
	pVertices[vertIndex].position.x = destRect.Right();
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].tu = srcTextureCoords.Right(); pVertices[vertIndex].tv = srcTextureCoords.pos.y;
	++vertIndex;

	// Bottom Right
	pVertices[vertIndex].position.x = destRect.Right();
	pVertices[vertIndex].position.y = destRect.Bottom();
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].tu = srcTextureCoords.Right(); pVertices[vertIndex].tv = srcTextureCoords.Bottom();
	++vertIndex;

	// Bottom Left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.Bottom();
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].tu = srcTextureCoords.pos.x; pVertices[vertIndex].tv = srcTextureCoords.Bottom();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::DrawImage  Public
///	\param destPos The viewport position to which the image will be drawn
///	\param pSrcImage The image to draw
///	\param srcRect The coordinates of the image that will be drawn
///	\returns True if the image was drawn successfully, otherwise false
///
///	Draw an image to the viewport
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3DV9::DrawImage( Point2i destPos, const TCImage* const pSrcImage, Box2i srcRect )
{
	// Get the proper image pointer
	D3DV9Texture* pD3DTexture = reinterpret_cast< D3DV9Texture* >( pSrcImage->GetImageData() );
	if( !pD3DTexture )
		return false;

	// Clip the image horizontally
	if( destPos.x < 0 )
	{
		srcRect.pos.x += -destPos.x;
		srcRect.size.x -= -destPos.x;
		destPos.x = 0;
	}
	int32 right = destPos.x + srcRect.size.x;
	if( right > m_ViewportSize.x )
		srcRect.size.x -= (right - m_ViewportSize.x);

	// Clip the image vertically
	if( destPos.y < 0 )
	{
		srcRect.pos.y += -destPos.y;
		srcRect.size.y -= -destPos.y;
		destPos.y = 0;
	}
	int32 bottom = destPos.y + srcRect.size.y;
	if( bottom > m_ViewportSize.y )
		srcRect.size.y -= (bottom - m_ViewportSize.y);

	// If the image is off the screen then bail
	if( srcRect.size.x <= 0 || srcRect.size.y <= 0 )
		return true;

	Box2 destRect( (float32)destPos.x, (float32)destPos.y, (float32)srcRect.size.x, (float32)srcRect.size.y );

	Box2 srcTextureCoords( srcRect.pos.x / (float32)pD3DTexture->m_Dims.x,
							srcRect.pos.y / (float32)pD3DTexture->m_Dims.y,
							srcRect.size.x / (float32)pD3DTexture->m_Dims.x,
							srcRect.size.y / (float32)pD3DTexture->m_Dims.y );

	// Fill in the vertex buffer with the destinate coordinates
	SpriteVertexEx* pVertices = NULL;
	if( FAILED(m_pSpriteVertexBuffer->Lock( 0,
										6 * sizeof(SpriteVertexEx),
										(void**)&pVertices,
										0 )) )
	{
		// Failed to lock the vertex buffer to fill in with data
		return false;
	}

	MakeSpriteRect( pVertices, destRect, srcTextureCoords );
	m_pSpriteVertexBuffer->Unlock();

	// Set the texture to use for drawing
	m_pD3DDevice->SetTexture( 0, pD3DTexture->m_pTexture );

	// Set stream 0 to use the class's vertex buffer
	m_pD3DDevice->SetStreamSource( 0,
									m_pSpriteVertexBuffer,
									0,
									sizeof(SpriteVertexEx) );

	// Set the flexible vertex format to expect the sprite vertex format
	m_pD3DDevice->SetFVF( D3DFVF_SpriteVertexExFlags );

	// Draw the quad
	if( FAILED( m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ) ) )
		return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::DrawImageEx  Public
///	\param destPos The viewport position to which the image will be drawn
///	\param pSrcImage The image to draw
///	\param srcRect The coordinates of the image that will be drawn
///	\param fx Special effects to apply to the rendering
///	\returns True if the image was drawn successfully, otherwise false
///
///	Draw an image to the viewport with effects applied
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3DV9::DrawImageEx( Box2i destRect, const TCImage* const pSrcImage, Box2i srcRect, int32 fx )
{
	// Get the proper image pointer
	D3DV9Texture* pD3DTexture = reinterpret_cast< D3DV9Texture* >( pSrcImage->GetImageData() );
	if( !pD3DTexture )
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
	if( destRect.Right() > m_ViewportSize.x )
	{
		int32 amountOff = destRect.Right() - m_ViewportSize.x;
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
	if( destRect.Bottom() > m_ViewportSize.y )
	{
		int32 amountOff = destRect.Bottom() - m_ViewportSize.y;
		srcRect.size.y -= (int32)( (float32)amountOff / vertScale );
		destRect.size.y -= amountOff;
	}

	// If the image is off the screen then there is no need to draw
	if( srcRect.size.x <= 0 || srcRect.size.y <= 0 )
		return true;

	// Handle the effects
	if( fx & GraphicsDefines::DEF_FlipHoriz )
	{
		srcRect.pos.x += srcRect.size.x;
		srcRect.size.x = -srcRect.size.x;
	}
	if( fx & GraphicsDefines::DEF_FlipVert )
	{
		srcRect.pos.y += srcRect.size.y;
		srcRect.size.y = -srcRect.size.y;
	}

	/*if( fx & GraphicsDefines::DEF_Rotate_90 )
	{
		
	}
	else if( fx & GraphicsDefines::DEF_Rotate_180 )
		blitFX.dwDDFX |= DDBLTFX_ROTATE180;
	else if( fx & GraphicsDefines::DEF_Rotate_270 )
		blitFX.dwDDFX |= DDBLTFX_ROTATE270;*/

	Box2 destRectFloat( (float32)destRect.pos.x, (float32)destRect.pos.y, (float32)destRect.size.x, (float32)destRect.size.y );

	Box2 srcTextureCoords( srcRect.pos.x / (float32)pD3DTexture->m_Dims.x,
							srcRect.pos.y / (float32)pD3DTexture->m_Dims.y,
							srcRect.size.x / (float32)pD3DTexture->m_Dims.x,
							srcRect.size.y / (float32)pD3DTexture->m_Dims.y );

	// Fill in the vertex buffer with the destinate coordinates
	SpriteVertexEx* pVertices = NULL;
	if( FAILED(m_pSpriteVertexBuffer->Lock( 0,
										6 * sizeof(SpriteVertexEx),
										(void**)&pVertices,
										0 )) )
	{
		// Failed to lock the vertex buffer to fill in with data
		return false;
	}
	MakeSpriteRect( pVertices, destRectFloat, srcTextureCoords );
	m_pSpriteVertexBuffer->Unlock();

	// Set the texture to use for drawing
	m_pD3DDevice->SetTexture( 0, pD3DTexture->m_pTexture );

	// Set stream 0 to use the class's vertex buffer
	m_pD3DDevice->SetStreamSource( 0,
									m_pSpriteVertexBuffer,
									0,
									sizeof(SpriteVertexEx) );

	// Set the flexible vertex format to expect the sprite vertex format
	m_pD3DDevice->SetFVF( D3DFVF_SpriteVertexExFlags );

	// Draw the quad
	if( FAILED( m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ) ) )
		return false;

	return true;
}

static void MakeLineRect( ShapeVertexEx* pVertices, const Box2& destRect, DWORD color )
{
	int32 vertIndex = 0;
	
	// Top left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;

	// Top right
	pVertices[vertIndex].position.x = destRect.Right();
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;

	// Bottom Right
	pVertices[vertIndex].position.x = destRect.Right();
	pVertices[vertIndex].position.y = destRect.Bottom();
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;

	// Bottom Left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.Bottom();
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;

	// Top left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::DrawRect  Public
///	\param rect The coordinates of the rectangle to draw
///	\param fillColor The color of the lines
///	\returns True if the rectangle was drawn successfully, otherwise false
///
///	Draw the outline of a rectangle
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3DV9::DrawRect( const Box2i& rect, int fillColor )
{
	Box2 destRect( (float32)rect.pos.x, (float32)rect.pos.y, (float32)rect.size.x, (float32)rect.size.y );

	// Fill in the vertex buffer with the destinate coordinates
	ShapeVertexEx* pVertices = NULL;
	if( FAILED(m_pShapeVertexBuffer->Lock( 0,
										5 * sizeof(ShapeVertexEx),
										(void**)&pVertices,
										0 )) )
	{
		// Failed to lock the vertex buffer to fill in data
		return false;
	}

	MakeLineRect( pVertices, destRect, fillColor );
	m_pShapeVertexBuffer->Unlock();

	// Set stream 0 to use the class's vertex buffer
	m_pD3DDevice->SetStreamSource( 0,
									m_pShapeVertexBuffer,
									0,
									sizeof(ShapeVertexEx) );

	// Set the flexible vertex format to expect the sprite vertex format
	m_pD3DDevice->SetFVF( D3DFVF_ShapeVertexFlags );

	// Draw the quad
	m_pD3DDevice->SetTexture( 0, NULL );
	if( FAILED( m_pD3DDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, 4 ) ) )
		return false;

	return true;
}

static void MakeFilledRect( ShapeVertexEx* pVertices, const Box2& destRect, DWORD color )
{
	int32 vertIndex = 0;
	
	// Top left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;

	// Top right
	pVertices[vertIndex].position.x = destRect.Right();
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;

	// Bottom Left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.Bottom();
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;


	// Top Right
	pVertices[vertIndex].position.x = destRect.Right();
	pVertices[vertIndex].position.y = destRect.pos.y;
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;

	// Bottom Right
	pVertices[vertIndex].position.x = destRect.Right();
	pVertices[vertIndex].position.y = destRect.Bottom();
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
	++vertIndex;

	// Bottom Left
	pVertices[vertIndex].position.x = destRect.pos.x;
	pVertices[vertIndex].position.y = destRect.Bottom();
	pVertices[vertIndex].position.z = 1;
	pVertices[vertIndex].diffuse = color;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::FillRect  Public
///	\param rect The coordinates of the rectangle to draw
///	\param fillColor The color of the lines
///	\returns True if the rectangle was drawn successfully, otherwise false
///
///	Draw a filled rectangle
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3DV9::FillRect( const Box2i& rect, int fillColor )
{
	Box2 destRect( (float32)rect.pos.x, (float32)rect.pos.y, (float32)rect.size.x, (float32)rect.size.y );

	// Fill in the vertex buffer with the destinate coordinates
	ShapeVertexEx* pVertices = NULL;
	if( FAILED(m_pShapeVertexBuffer->Lock( 0,
										6 * sizeof(ShapeVertexEx),
										(void**)&pVertices,
										0 )) )
	{
		// Failed to lock the vertex buffer to fill in data
		return false;
	}

	MakeFilledRect( pVertices, destRect, fillColor );
	m_pShapeVertexBuffer->Unlock();

	// Set stream 0 to use the class's vertex buffer
	m_pD3DDevice->SetStreamSource( 0,
									m_pShapeVertexBuffer,
									0,
									sizeof(ShapeVertexEx) );

	// Set the flexible vertex format to expect the shape vertex format
	m_pD3DDevice->SetFVF( D3DFVF_ShapeVertexFlags );

	// Draw the quad
	m_pD3DDevice->SetTexture( 0, NULL );
	if( FAILED( m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ) ) )
		return false;

	return true;
}


/// Apply a pixel transformation
bool D3DV9::ApplyEffect( TCImage*, const Box2i&, GraphicsDefines::ModPixelCB )
{
	return false;
}

/// Set a temporary render target
bool D3DV9::SetTempRenderTarget( TCImage* )
{
	return false;
}

// Define a function that matches the prototype of LPD3DXFILL3D
VOID WINAPI ColorFill (D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
const D3DXVECTOR2* , LPVOID )
{
    *pOut = D3DXVECTOR4(pTexCoord->x, pTexCoord->y, 0.0f, 0.0f);
}


void D3DV9::RecalcSceneTransforms()
{
	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH( &matProj, 
						(FLOAT)m_ViewportSize.x,
						(FLOAT)m_ViewportSize.y,
						-10.0f,
						10.0f );

	D3DXMATRIX matMirror;
	D3DXPLANE mirrorPlane;
	mirrorPlane.a = 0;
	mirrorPlane.b = -1;
	mirrorPlane.c = 0;
	mirrorPlane.d = 0;
	D3DXMatrixReflect( &matMirror, 
						&mirrorPlane );

	D3DXMATRIX matTrans;
	D3DXMatrixTranslation( &matTrans, -m_ViewportSize.x * 0.5f, -m_ViewportSize.y * 0.5f, 0.0f );
	
	D3DXMATRIX transformMat;
	D3DXMatrixMultiply( &transformMat, &matTrans, &matMirror );
	//D3DXMatrixMultiply( &transformMat, &matTrans, &transformMat );

	m_pD3DDevice->SetTransform( D3DTS_VIEW, &transformMat );
	m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );

}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::Init  Public
///	\param hWnd The window to which scenes are rendered
///	\param startFullscreen True to have the scene take up the entire screen, false for windowed
///	\returns Whether or not initialization was successful
///
///	Initialize Direct3D.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool D3DV9::Init( void* hWnd, bool  )
{
	m_hWnd = hWnd;

	// If we don't already have a interface, get it
	if( !m_pD3D9 )
	{
		// Create a IDirect3D9 instance
		m_pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
		
		// Ensure it was created
		if( !m_pD3D9 )
			return false;
	}

	// If we have an interface, clear it
	if( m_pD3DDevice )
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Initialize the presentation paremeters
	D3DPRESENT_PARAMETERS d3dPresentParams;
	ZeroMemory( &d3dPresentParams, sizeof(d3dPresentParams) );
	d3dPresentParams.Windowed = true;
	d3dPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dPresentParams.EnableAutoDepthStencil = TRUE;
    d3dPresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dPresentParams.AutoDepthStencilFormat = D3DFMT_D16;
	d3dPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the Direct3D device
	if( FAILED(m_pD3D9->CreateDevice( D3DADAPTER_DEFAULT,
										D3DDEVTYPE_HAL,
										(HWND)m_hWnd,
										D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										&d3dPresentParams,
										&m_pD3DDevice )) )
	{
		// We failed
		Term();
		return false;
	}

	// Create the sprite vertex buffer
	if( FAILED(m_pD3DDevice->CreateVertexBuffer( 6 * sizeof(SpriteVertexEx),
													0,
													D3DFVF_SpriteVertexExFlags,
													D3DPOOL_DEFAULT,
													&m_pSpriteVertexBuffer,
													NULL)) )
	{
		// Failed to create the vertex buffer
		Term();
		return false;
	}

	// Create the shape vertex buffer
	if( FAILED(m_pD3DDevice->CreateVertexBuffer( 6 * sizeof(ShapeVertexEx),
													0,
													D3DFVF_ShapeVertexFlags,
													D3DPOOL_DEFAULT,
													&m_pShapeVertexBuffer,
													NULL)) )
	{
		// Failed to create the vertex buffer
		Term();
		return false;
	}

	// Disable lighting and backface culling
	m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	// Enable alhpa blending
    m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// Store the width and height of the window
	RECT clientRect;
	GetClientRect( (HWND)m_hWnd, &clientRect );
	m_ViewportSize.x = clientRect.right;
	m_ViewportSize.y = clientRect.bottom;

	RecalcSceneTransforms();

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  D3DV9::Term  Public
///
///	Close and clean up Direct3D.
///////////////////////////////////////////////////////////////////////////////////////////////////
void D3DV9::Term()
{
	// Free the vertex buffers
	if( m_pShapeVertexBuffer )
	{
		m_pShapeVertexBuffer->Release();
		m_pShapeVertexBuffer = NULL;
	}
	if( m_pSpriteVertexBuffer )
	{
		m_pSpriteVertexBuffer->Release();
		m_pSpriteVertexBuffer = NULL;
	}

	// Free the Direct3D 9 device interface
	if( m_pD3DDevice )
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Release the Direct3D 9 Interface
	if( m_pD3D9 )
	{
		m_pD3D9->Release();
		m_pD3D9 = NULL;
	}
}