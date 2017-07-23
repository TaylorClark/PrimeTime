//=================================================================================================
/*!
	\file ImageLoadingTypes.h
	Graphics Library
	Image Loading Types Header
	\author Taylor Clark
	\date December 16, 2009

	This file contains the definition for classes used to assist in image loading.
*/
//=================================================================================================

#pragma once
#ifndef __ImageLoadingTypes_h
#define __ImageLoadingTypes_h

#include "Base/Types.h"
#ifndef SFML
#include "Jpeg/corona.h"
#endif
#include "Base/NetSafeDataBlock.h"
#include "Math/Vector2i.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class SrcImgData
	\brief The base class for an image data source
*/
//-------------------------------------------------------------------------------------------------
class SrcImgData
{
public:
	virtual uint8 GetR() = 0;
	virtual uint8 GetG() = 0;
	virtual uint8 GetB() = 0;

	virtual bool IsTransparent() const = 0;

	virtual bool AdvancePixel() = 0;

	virtual ~SrcImgData(){}
};

//-------------------------------------------------------------------------------------------------
/*!
	\class BmpSrcData
	\brief The class for an uncompressed bitmap image data source
*/
//-------------------------------------------------------------------------------------------------
class BmpSrcData : public SrcImgData
{
	uint8* pCurPixelData;

	uint32 numPixels;

	uint32 curPixel;

public:

	BmpSrcData( uint8* pSrcData, uint32 inNumPixels ) : numPixels( inNumPixels ),
														curPixel(0),
														pCurPixelData( pSrcData )
	{}

	virtual uint8 GetR(){ return pCurPixelData[0]; }
	virtual uint8 GetG(){ return pCurPixelData[1]; }
	virtual uint8 GetB(){ return pCurPixelData[2]; }

	virtual bool IsTransparent() const
	{
		return (pCurPixelData[0] == 0xFF) && (pCurPixelData[1] == 0) && (pCurPixelData[2] == 0xFF);
	}

	bool AdvancePixel()
	{
		curPixel++;

		if( curPixel >= numPixels )
			return false;

		pCurPixelData += 3;

		return true;
	}
};


#ifndef SFML
//-------------------------------------------------------------------------------------------------
/*!
	\class JPEGSrcData
	\brief The class for a JPEG image data source
*/
//-------------------------------------------------------------------------------------------------
class JPEGSrcData : public SrcImgData
{
	corona::File* pJPEGFile;
	corona::JPEGReader* pJPEGReader;

	uint8* pCurPixelData;

	int32 imgWidth;

	int32 curPixelX;

public:

	JPEGSrcData( uint8* pSrcData, uint32 pixelDataSize, int32 inImgWidth ) : imgWidth( inImgWidth ),
																			curPixelX(0)
	{
		// Create the JPEG reader
		pJPEGFile = corona::CreateMemoryFile( pSrcData, pixelDataSize, false );
		pJPEGReader = corona::OpenJPEGReader( pJPEGFile );

		// Start at the first line
		pCurPixelData = pJPEGReader->ReadLine();
	}

	~JPEGSrcData()
	{
		// Free the JPEG related data
		if( pJPEGReader )
		{
			delete pJPEGReader;
			pJPEGReader = 0;
		}
		if( pJPEGFile )
		{
			delete pJPEGFile;
			pJPEGFile = 0;
		}
	}

	virtual uint8 GetR(){ return pCurPixelData[0]; }
	virtual uint8 GetG(){ return pCurPixelData[1]; }
	virtual uint8 GetB(){ return pCurPixelData[2]; }

	virtual bool IsTransparent() const
	{
		return (pCurPixelData[0] == 0xFF) && (pCurPixelData[1] == 0) && (pCurPixelData[2] == 0xFF);
	}
	
	bool AdvancePixel()
	{
		curPixelX++;
		pCurPixelData += 3;

		if( curPixelX >= imgWidth )
		{
			// Read the next JPEG line
			curPixelX = 0;
			pCurPixelData = pJPEGReader->ReadLine();

			// If there is no data left
			if( !pCurPixelData )
				return false;
		}

		return true;
	}
};
#endif

//-------------------------------------------------------------------------------------------------
/*!
	\class BitmapRLESrcData
	\brief The class for a run length encoded bitmap image data source
*/
//-------------------------------------------------------------------------------------------------
class BitmapRLESrcData : public SrcImgData
{
	NetSafeDataBlock m_ImageDataReader;

	uint32 m_CurRunCount;

	uint32 m_CurRunIndex;

	bool m_IsNonMatchingRun;

	uint32 m_CurColor;
public:

	BitmapRLESrcData( uint8* pPixelData, uint32 pixelDataLen ) : m_ImageDataReader( pPixelData, pixelDataLen ),
												m_CurRunCount(0),
												m_CurRunIndex(0),
												m_IsNonMatchingRun( true ),
												m_CurColor(0)
	{
		// Initialize the data
		AdvancePixel();
	}

	virtual uint8 GetR(){ return (uint8)( (m_CurColor >> 16) & 0xFF ); }
	virtual uint8 GetG(){ return (uint8)( (m_CurColor >> 8) & 0xFF ); }
	virtual uint8 GetB(){ return (uint8)( m_CurColor & 0xFF ); }

	virtual bool IsTransparent() const{ return ((m_CurColor & 0x00FFFFFF) == 0xFF00FF); }

	bool AdvancePixel()
	{
		const uint32 IS_NON_MATCH_RUN_FLAG = 0x80000000;

		// Advance the run index and check if the current run has ended
		++m_CurRunIndex;
		if( m_CurRunIndex >= m_CurRunCount )
		{
			// If the data is completed then bail
			if( m_ImageDataReader.GetRemainingBytes() == 0 )
				return false;

			// Read the next count
			m_CurRunCount = m_ImageDataReader.ReadUint32();

			// If this is a non-matching run
			if( (m_CurRunCount & IS_NON_MATCH_RUN_FLAG) == IS_NON_MATCH_RUN_FLAG )
				m_IsNonMatchingRun = true;
			// Else it is a matching run so get the color
			else
			{
				m_IsNonMatchingRun = false;
				m_CurColor = m_ImageDataReader.ReadUint32();
			}

			// Clear the flag and store the count
			m_CurRunCount = m_CurRunCount & ~IS_NON_MATCH_RUN_FLAG;
			m_CurRunIndex = 0;
		}

		// If this is a non-matching run then get the new color
		if( m_IsNonMatchingRun )
			m_CurColor = m_ImageDataReader.ReadUint32();

		return true;
	}
};

class DestImageData
{
protected:
	int32 redOffset, greenOffset, blueOffset;

	int32 bytesPerPixel;

	uint32 stride;

	Vector2i m_CurPixel;
	Vector2i m_DestImgSize;

	uint8* m_pDestPixelData;

	uint8* m_pCurPixelData;

public:

	DestImageData( uint8* pDestData ) : redOffset(0),
				greenOffset(1),
				blueOffset(2),
				bytesPerPixel(3),
				stride(0),
				m_pDestPixelData( pDestData )
	{
		m_pCurPixelData = m_pDestPixelData;
	}
	
	virtual ~DestImageData(){}

	void Init( int32 redOff, int32 greenOff, int32 blueOff, int32 bytesPP, uint32 inStride, Vector2i imgSize )
	{
		redOffset = redOff;
		greenOffset = greenOff;
		blueOffset = blueOff;
		bytesPerPixel = bytesPP;
		stride = inStride;
		m_DestImgSize = imgSize;
		m_CurPixel.Set(0,0);
	}

	virtual void SetPixels( SrcImgData* pSrcData )
	{
		do
		{
			// Store the pixel data
			m_pCurPixelData[redOffset] = pSrcData->GetR();
			m_pCurPixelData[greenOffset] = pSrcData->GetG();
			m_pCurPixelData[blueOffset] = pSrcData->GetB();

			// Advance a pixel on the destination DIB
			m_CurPixel.x++;
			m_pCurPixelData += bytesPerPixel;

			// If it is time to move down a line
			if( m_CurPixel.x >= m_DestImgSize.x )
			{
				m_CurPixel.x = 0;
				m_CurPixel.y++;

				if( m_CurPixel.y >= m_DestImgSize.y )
					return;

				m_pCurPixelData = m_pDestPixelData + (m_CurPixel.y * stride);
			}

		}while( pSrcData->AdvancePixel() );
	}

	virtual void SetPixelsWithAlpha( SrcImgData* pSrcData )
	{
		do
		{
			// The alpha offset could be 0 through 3. The red, green, and blue components are also
			// in the range 0 through 3. This simple calculation finds the index that is not being
			// used by the RGB components.
			int alphaOffset = 6 - redOffset - greenOffset - blueOffset;

			// If this pixel is transparent
			if( pSrcData->IsTransparent() )
				m_pCurPixelData[alphaOffset] = 0;
			else
				m_pCurPixelData[alphaOffset] = 255;
            
			// Store the pixel data
			m_pCurPixelData[redOffset] = pSrcData->GetR();
			m_pCurPixelData[greenOffset] = pSrcData->GetG();
			m_pCurPixelData[blueOffset] = pSrcData->GetB();

			// Advance a pixel on the destination DIB
			m_CurPixel.x++;
			m_pCurPixelData += bytesPerPixel;

			// If it is time to move down a line
			if( m_CurPixel.x >= m_DestImgSize.x )
			{
				m_CurPixel.x = 0;
				m_CurPixel.y++;

				if( m_CurPixel.y >= m_DestImgSize.y )
					return;

				m_pCurPixelData = m_pDestPixelData + (m_CurPixel.y * stride);
			}

		}while( pSrcData->AdvancePixel() );
	}
};

//struct JpegGDIParseData
//{
//	JpegGDIParseData() : curRow(0),
//						pDestData(0),
//						dibPitch( 0 ),
//						jpegWidth(1)
//	{}
//
//	uint32 curRow;
//	uint8* pDestData;
//	uint32 jpegWidth;
//	uint32 dibPitch;
//};

#endif // __ImageLoadingTypes_h