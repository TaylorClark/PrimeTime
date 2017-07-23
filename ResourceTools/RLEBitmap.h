#pragma once
#ifndef __BitmapRLE_h
#define __BitmapRLE_h

#include <windows.h>
#include "Base/Types.h"
#include <list>

struct ColorRun
{
	uint32 length;
	std::list<uint32> colors;

	ColorRun():length(0){}
};

typedef std::list<ColorRun> RunList;

struct RLEEncodeInfo
{
	RunList colorRuns;

	uint32 dataSize;

	RLEEncodeInfo() : dataSize(0)
	{}
};

RLEEncodeInfo RLEEncodeBitmap( HDC hImageDC, int32 width, int32 height );

#endif // __BitmapRLE_h