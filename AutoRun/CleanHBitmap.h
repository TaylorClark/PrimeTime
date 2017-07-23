#pragma once
#ifndef __CleanHBitmap_h
#define __CleanHBitmap_h

#include <windows.h>

class CleanHBitmap
{
private:

	HDC m_hBitmapDC;

	HBITMAP m_hBitmap;

	HBITMAP m_hOrigBitmap;

public:

	CleanHBitmap( HDC hBitmapDC, HBITMAP hBitmap ) : m_hBitmapDC(hBitmapDC), m_hBitmap( hBitmap )
	{
		m_hOrigBitmap = (HBITMAP)SelectObject( hBitmapDC, hBitmap );
	}

	CleanHBitmap( HINSTANCE hInst, UINT resID )
	{
		m_hBitmapDC = CreateCompatibleDC( NULL );
		m_hBitmap = (HBITMAP)LoadImage( hInst, MAKEINTRESOURCE(resID), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR );
		m_hOrigBitmap = (HBITMAP)SelectObject( m_hBitmapDC, m_hBitmap );
	}

	CleanHBitmap( HDC hWindowDC, int width, int height )
	{
		m_hBitmapDC = CreateCompatibleDC( hWindowDC );
		m_hBitmap = (HBITMAP)CreateCompatibleBitmap( hWindowDC, width, height );
		m_hOrigBitmap = (HBITMAP)SelectObject( m_hBitmapDC, m_hBitmap );
	}

	~CleanHBitmap()
	{
		Clear();
	}

	HDC GetDC()
	{
		return m_hBitmapDC;
	}

	void Clear()
	{
		SelectObject( m_hBitmapDC, m_hOrigBitmap );
		m_hOrigBitmap = 0;

		DeleteObject( m_hBitmap );
		m_hBitmap = 0;

		DeleteDC( m_hBitmapDC );
		m_hBitmapDC = 0;
	}
};

#endif