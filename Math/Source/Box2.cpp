//=============================================================================
/*!
	\file Box2.cpp
	Math Library
	2D Box Class
	\author Taylor Clark

  This source file contains the 2D box object implementation.
*/
//=============================================================================

#include "../Box2.h"
#include "../Interval.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
//
//	Box2::IntelliSnapToBox( ) Public
///
///	\param snapBox The box to snap to
///	\param threshold The max distance where snapping is allowed, defaults to 10
///	\return Returns if the box was snapped to the snap box.
///
///	Snap the box to another box "smartly".  It is similar to the way windows
///	docks windows.
///
///////////////////////////////////////////////////////////////////////////////
bool Box2::IntelliSnapToBox( const Box2& snapBox, const float32 threshold )
{
	// Make the frame box larger for hit test
	const float32 threshSq = threshold * threshold;
	float32 curMinDistSq = threshSq;
	int32 closestSnapIdx = -1, closestBoxIdx = -1;
	for( int32 snapIdx = 0; snapIdx < 4; ++snapIdx )
	{
		for( int32 boxIdx = 0; boxIdx < 4; ++boxIdx )
		{
			// Get the distance between the verts
			float32 distSq = ((*this)[boxIdx] - snapBox[snapIdx]).GetMagSq();
			if( distSq > threshSq )
				continue;

			// If this is closer than the current, use it
			if( distSq < curMinDistSq )
			{
				// Store these vertices as the closest
				curMinDistSq = distSq;
				closestSnapIdx = snapIdx;
				closestBoxIdx = boxIdx;
			}
		}
	}

	// If we found close corners
	if( closestSnapIdx != -1 || closestBoxIdx != -1 )
	{
		// Get the offset from the corners
		pos += snapBox[closestSnapIdx] - (*this)[closestBoxIdx];
		return true;
	}

	// See if the boxes are lined up for snapping
	float32 minMoveDist = threshold;
	bool moveHorz = true;
	IntervalFloat ourVertInterval = IntervalFloat::MakeClosed( Top(), Bottom() );
	IntervalFloat ourHorzInterval = IntervalFloat::MakeClosed( Left(), Right() );
	IntervalFloat snapVertInterval = IntervalFloat::MakeClosed( snapBox.Top(), snapBox.Bottom() );
	IntervalFloat snapHorzInterval = IntervalFloat::MakeClosed( snapBox.Left(), snapBox.Right() );

	// If the boxes can snap horizontally
	if( ourVertInterval.IsIntervalIntersecting( snapVertInterval ) )
	{
		// Test our left to snap left
		float32 dist = snapBox.Left() - Left();
		if( fabs( dist ) < fabs( minMoveDist ) )
			minMoveDist = dist;

		// Test our left to snap right
		dist = snapBox.Right() - Left();
		if( fabs( dist ) < fabs( minMoveDist ) )
			minMoveDist = dist;

		// Test our right to snap left
		dist = snapBox.Left() - Right();
		if( fabs( dist ) < fabs( minMoveDist ) )
			minMoveDist = dist;

		// Test our right to snap right
		dist = snapBox.Right() - Right();
		if( fabs( dist ) < fabs( minMoveDist ) )
			minMoveDist = dist;
	}

	// If the boxes can snap vertically
	if( ourHorzInterval.IsIntervalIntersecting( snapHorzInterval ) )
	{
		float32 minDist = threshold;

		// Test our top to snap top
		float32 dist = snapBox.Top() - Top();
		if( fabs( dist ) < fabs( minDist ) )
			minDist = dist;

		// Test our top to snap bottom
		dist = snapBox.Bottom() - Top();
		if( fabs( dist ) < fabs( minDist ) )
			minDist = dist;

		// Test our bottom to snap top
		dist = snapBox.Top() - Bottom();
		if( fabs( dist ) < fabs( minDist ) )
			minDist = dist;

		// Test our bottom to snap bottom
		dist = snapBox.Bottom() - Bottom();
		if( fabs( dist ) < fabs( minDist ) )
			minDist = dist;

		// If the vertical translation is small than the horizontal one
		if( fabs( minDist ) < fabs( minMoveDist ))
		{
			minMoveDist = minDist;
			moveHorz = false;
		}
	}

	// If we should move
	if( minMoveDist < threshold )
	{
		// If we should move horizontally
		if( moveHorz )
			pos.x += minMoveDist;
		// Else we should move vertically
		else
			pos.y += minMoveDist;

		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Box2::DoesIntersectBox( ) Public
///
///	\param testBox The box to test for intersection with
///
///	Test for intersection with another box.
///
///////////////////////////////////////////////////////////////////////////////
bool Box2::DoesIntersectBox( const Box2& testBox ) const
{
	// Treat the box dimensions as intervals and see if the horizontal and
	// vertical interval intersect
	IntervalFloat ourVertInterval = IntervalFloat::MakeClosed( pos.y, pos.y + size.y );
	IntervalFloat ourHorzInterval = IntervalFloat::MakeClosed( pos.x, pos.x + size.x );
	IntervalFloat testVertInterval = IntervalFloat::MakeClosed( testBox.pos.y, testBox.pos.y + testBox.size.y );
	IntervalFloat testHorzInterval = IntervalFloat::MakeClosed( testBox.pos.x, testBox.pos.x + testBox.size.x );

	return ourVertInterval.IsIntervalIntersecting( testVertInterval )
			&& ourHorzInterval.IsIntervalIntersecting( testHorzInterval );
}