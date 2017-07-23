//=================================================================================================
/*!
	\file DDraw1DrawList.h
	2D Graphics Engine
	DirectDraw Draw List Header
	\author Taylor Clark
	\date March 16, 2005

	This file contains the definition for the DirecDraw draw list class.
*/
//=================================================================================================

#pragma once
#ifndef __DDraw1DrawList_h
#define __DDraw1DrawList_h

#include "Base/Types.h"
#include "Math/Box2i.h"
#include <vector>
#include <list>

class TCImage;


//-------------------------------------------------------------------------------------------------
/*!
	\class DDraw1DrawList
	\brief Manages the drawing of images so that unnecessary rendering can be avoided
*/
//-------------------------------------------------------------------------------------------------
class DDraw1DrawList
{
public:

	struct DrawnItem
	{
		DrawnItem() : pImage(0), fx(0)
		{}

		const TCImage* pImage;
		Box2i destRect;
		Box2i srcRect;
		int32 fx;

		bool operator == ( const DrawnItem& rhs )
		{
			return pImage == rhs.pImage
					&& destRect == rhs.destRect
					&& srcRect == rhs.srcRect
					&& fx == rhs.fx;
		}
	};
	typedef std::list< DrawnItem > DrawingList;

private:
	typedef std::vector< DrawnItem > DrawingArray;

	/// The current index of what is being drawn in the current frame
	uint32 m_DrawIndex;

	/// The index of the current back buffer to which images are being drawn
	uint32 m_CurBackBufferIndex;

	/// The maps, one for each back bufffer
	std::vector<DrawingArray> m_BackBufferList;

	/// The list of areas that have been drawn to in the current frame
	std::list<Box2i> m_DirtyRects;


	/// Test if the rectangle intersects any of the dirty rectangles
	bool IntersectsDirtyRects( const Box2i& destRect ) const;

public:

	/// The default constructor
	DDraw1DrawList( uint32 numBackBuffers ) : m_DrawIndex(0), m_CurBackBufferIndex(0)
	{
		m_BackBufferList.resize( numBackBuffers );
	}

	DrawingList DrawImage( const TCImage* pImage, const Box2i& destRect, const Box2i& srcRect, int32 fx );

	void DisplayScene();
};

#endif // __DDraw1DrawList_h