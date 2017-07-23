#include "..\PrivateInclude\DDraw1DrawList.h"


/// Test if the rectangle intersects any of the dirty rectangles
bool DDraw1DrawList::IntersectsDirtyRects( const Box2i& destRect ) const
{
	// Go through each dirty rectangle and test for intersection
	for( std::list<Box2i>::const_iterator iterRect = m_DirtyRects.begin(); iterRect != m_DirtyRects.end(); ++iterRect )
	{
		if( destRect.Intersects( *iterRect ) )
			return true;
	}

	// The destination rectangle does not intersect any of the dirty rectangles
	return false;
}

DDraw1DrawList::DrawingList DDraw1DrawList::DrawImage( const TCImage* pImage, const Box2i& destRect, const Box2i& srcRect, int32 fx )
{
	DrawingArray curDrawList = m_BackBufferList[m_CurBackBufferIndex];

	DrawingList retList;

	// Create the item for this draw
	DrawnItem newItem;
	newItem.destRect = destRect;
	newItem.fx = fx;
	newItem.pImage = pImage;
	newItem.srcRect = srcRect;

	// If this item has not been drawn yet then draw the entire thing
	if( m_DrawIndex >= curDrawList.size() )
	{
		curDrawList.resize( m_DrawIndex + 1 );
		curDrawList[ m_DrawIndex ] = newItem;

		retList.push_back( newItem );

		++m_DrawIndex;
		return retList;
	}

	// Test if this will intersect any dirty rectangles
	if( IntersectsDirtyRects( destRect ) )
	{
	}

	// If this item is the same as last frame then there is no need to draw it
	const DrawnItem& lastFrameDrawn = curDrawList[m_DrawIndex];
	if( newItem == lastFrameDrawn )
	{
		++m_DrawIndex;
		return retList;
	}

	// At this point in the method it is known that this draw needs to occur, but also that all
	// subsequent draws above this image need to be drawn

	return retList;
}

void DDraw1DrawList::DisplayScene()
{
	// Step to the next back buffer, looping around if necessary
	m_CurBackBufferIndex++;
	m_CurBackBufferIndex %= m_BackBufferList.size();

	// Clear the list of rectangles to which images were drawn
	m_DirtyRects.clear();

	// Reset the drawn image index
	m_DrawIndex = 0;
}