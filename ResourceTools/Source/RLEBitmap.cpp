#include <windows.h>
#include "Base/Types.h"
#include <list>
#include "../RLEBitmap.h"

struct PixelTracker
{
	uint32 width, height;
	uint32 curX, curY;
	HDC hDC;
	bool isEOP;

	PixelTracker() : width( 0 ),
						height(0),
						curX( 0 ),
						curY(0),
						hDC( 0 ),
						isEOP( false )
	{}

	PixelTracker( const PixelTracker& copyObj ) : width( copyObj.width ),
						height(copyObj.height),
						curX( copyObj.curX ),
						curY(copyObj.curY),
						hDC( copyObj.hDC ),
						isEOP( copyObj.isEOP )
	{}

	uint32 GetNextPixel()
	{
		++curX;
		if( curX >= width )
		{
			curX = 0;
			++curY;
			if( curY >= height )
				isEOP = true;
		}

		return GetCurPixel();
	}

	uint32 GetCurPixel()
	{
		COLORREF curColor = GetPixel( hDC, curX, curY );
		uint32 retColor = GetRValue( curColor ) << 16 | GetGValue( curColor ) << 8 | GetBValue( curColor );
		return retColor;
	}
};

bool DoNextPixelsMatch(PixelTracker tempTracker)
{
	int32 numToMatch = 3;
	while(--numToMatch )
	{
		if( tempTracker.GetCurPixel() != tempTracker.GetNextPixel() )
			return false;
		if(tempTracker.isEOP)
			return false;
	}

	return true;
}

uint32 GetMatchingRun(PixelTracker& pixelTracker)
{
	uint32 curColor = 0;
	uint32 runCount = 0;
	do
	{
		curColor = pixelTracker.GetCurPixel();
		++runCount;
		
	}while( curColor == pixelTracker.GetNextPixel() && !pixelTracker.isEOP );

	return runCount;
}

uint32 GetRLEImageSize( const RunList& imageRuns )
{
	uint32 retSize = 0;
	const uint32 UINT32_SIZE = (uint32)sizeof(uint32);
	for( RunList::const_iterator iterRun = imageRuns.begin(); iterRun != imageRuns.end(); ++iterRun )
	{
		// Add the run count
		retSize += UINT32_SIZE;

		retSize += UINT32_SIZE * iterRun->colors.size();
	}

	return retSize;
}

ColorRun GetNonMatchingRun(PixelTracker& pixelTracker)
{
	ColorRun retRun;
	uint32 runCount = 0;
	do
	{
		retRun.colors.push_back( pixelTracker.GetCurPixel() );
		pixelTracker.GetNextPixel();
		++retRun.length;
		
	}while( !DoNextPixelsMatch( pixelTracker ) && !pixelTracker.isEOP );

	return retRun;
}

RLEEncodeInfo RLEEncodeBitmap( HDC hImageDC, int32 width, int32 height )
{
	// Initialize the tracker
	PixelTracker pixelTracker;
	pixelTracker.hDC = hImageDC;
	pixelTracker.height = height;
	pixelTracker.width = width;

	// Start encoding
	RLEEncodeInfo retData;
	bool isMatching = DoNextPixelsMatch(pixelTracker);
	while( !pixelTracker.isEOP )
	{
		ColorRun run;
		if( isMatching )
		{
			run.colors.push_back( pixelTracker.GetCurPixel() );
			run.length = GetMatchingRun(pixelTracker);

			// Add 2 uint32's since the matching runs simply store the length and color
			retData.dataSize += 2 * sizeof(uint32);

			// Determine the next run
			isMatching = DoNextPixelsMatch(pixelTracker);
		}
		// Else it is a non-matching run
		else
		{
			run = GetNonMatchingRun(pixelTracker);

			retData.dataSize += (run.length + 1) * sizeof(uint32);

			// If a non-matching run stops then there must be a matching run coming up (or the
			// image has been completely processed)
			isMatching = true;
		}

		// Store the color run
		retData.colorRuns.push_back( run );
	}

	// Compare sizes
	uint32 origSize = width * height * 3;
	float32 ratio = (float32)retData.dataSize / (float32)origSize;

	return retData;
}