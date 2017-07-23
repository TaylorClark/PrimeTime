using System;
using System.IO;

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// This class encapsulates the functionality of a axis aligned 2-D box.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct Box2
	{
		/// <summary> The position of the box </summary>
		public Point2 pos;

		/// <summary> The dimensions of the box </summary>
		public Vector2 dims;

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> A copy constructor </summary>
		/// <param name="copy"> The box to copy </param>
		////////////////////////////////////////////////////////////////////////
		public Box2( Box2 copy )
		{
			pos = copy.pos; dims = copy.dims;
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary> A copy constructor for integer boxes </summary>
		/// <param name="copy"> The box to copy </param>
		////////////////////////////////////////////////////////////////////////
		public Box2( Box2i copy )
		{
			pos = new Point2(copy.pos);
			dims = new Vector2(copy.dims);
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Construct a pox from a position and size
		/// </summary>
		/// <param name="inPos"> The position of the box </param>
		/// <param name="inSize"> The size of the box </param>
		////////////////////////////////////////////////////////////////////////
		public Box2( Point2 inPos, Vector2 inSize )
		{
			pos = inPos;
			dims = inSize;
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> A box constructor that initializes the data </summary>
		/// <param name="x"> The x position of the box </param>
		/// <param name="y"> The y position of the box </param>
		/// <param name="width"> The width of the box </param>
		/// <param name="height"> The height of the box </param>
		////////////////////////////////////////////////////////////////////////
		public Box2( float x, float y, float width, float height)
		{
			// The fields must be fully initialized in C#, so we can't call Set
			pos.x = x; pos.y = y;
			dims.x = width; dims.y = height;
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> Set the box data </summary>
		/// <param name="x"> The x position of the box </param>
		/// <param name="y"> The y position of the box </param>
		/// <param name="width"> The width of the box </param>
		/// <param name="height"> The height of the box </param>
		////////////////////////////////////////////////////////////////////////
		public void Set( float x, float y, float width, float height )
		{
			pos.Set( x, y );
			dims.Set( width, height );
		}
	
		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Does this box intersect another box
		/// </summary>
		/// <param name="testBox"> The box to test against </param>
		/// <returns> Whether or not the boxes intersect </returns>
		////////////////////////////////////////////////////////////////////////
		public bool DoesIntersectBox( Box2 testBox )
		{
			// Treat the box dimensions as intervals and see if the horizontal and
			// vertical interval intersect
			Interval ourVertInterval = Interval.MakeHalfOpen( pos.y, pos.y + dims.y );
			Interval ourHorzInterval = Interval.MakeHalfOpen( pos.x, pos.x + dims.x );
			Interval testVertInterval = Interval.MakeHalfOpen( testBox.pos.y, testBox.pos.y + testBox.dims.y );
			Interval testHorzInterval = Interval.MakeHalfOpen( testBox.pos.x, testBox.pos.x + testBox.dims.x );

			return ourVertInterval.IsIntervalIntersecting( testVertInterval )
					&& ourHorzInterval.IsIntervalIntersecting( testHorzInterval );
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> Is a point contained within this box.  The left and top
		/// sides are inclusive but if the point is on the right and bottom
		/// then it is not contained </summary>
		/// <param name="pt"> The point to test for containment </param>
		/// <returns> Whether or not the point is contained </returns>
		////////////////////////////////////////////////////////////////////////
		public bool ContainsPoint( Point2 pt )
		{
			float right = pos.x + dims.x;

			// If we have a negative dimension, check differently
			if( dims.x < 0.0f )
			{
				if( (pt.x >= pos.x) || (pt.x < right) )
					return false;
			}
			// Else check normally
			else
			{
				if( (pt.x < pos.x) || (pt.x >= right) )
					return false;
			}

			// Test the y component
			float bottom = pos.y + dims.y;
			if( dims.y < 0.0f )
			{
				if( (pt.y >= pos.y) || (pt.y < bottom) )
					return false;
			}
			// Else check normally
			else
			{
				if( (pt.y < pos.y) || (pt.y >= bottom) )
					return false;
			}

			// The point is inside
			return true;
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> Snap this box to another box with a threshold of 10 </summary>
		/// <param name="snapBox"> The box to snap to </param>
		/// <returns> Whether or not this box was within 10 units and snapped to it </returns>
		////////////////////////////////////////////////////////////////////////
		public bool IntelliSnapToBox( Box2 snapBox )
		{
			return IntelliSnapToBox( snapBox, 10.0f );
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> Snap this box to another box </summary>
		/// <param name="snapBox"> The box to snap to </param>
		/// <param name="threshold"> The threshold within this box will snap </param>
		/// <returns> Whether or not this box was within the threshold and snapped to it </returns>
		////////////////////////////////////////////////////////////////////////
		public bool IntelliSnapToBox( Box2 snapBox, float threshold )
		{
			// Test if we snap
			Vector2 moveVect = new Vector2(0,0);
			if( !GetIntelliSnapOffset( snapBox, threshold, ref moveVect ) )
				return false;

			// Move the box
			pos += moveVect;

			// We snapped the box
			return true;
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> Get the offset that would be needed to intelli-snap this
		/// box to another box </summary>
		/// <param name="snapBox"> The box to snap to </param>
		/// <param name="threshold"> The threshold within this box will snap </param>
		/// <param name="snapVect"> The output vector that represents the offset
		/// that would snap this box to the passed in box </param>
		/// <returns> Whether or not this box is in range to be snapped </returns>
		////////////////////////////////////////////////////////////////////////
		public bool GetIntelliSnapOffset( Box2 snapBox, float threshold, ref Vector2 snapVect )
		{
			// Figure out the edge values
			float left = pos.x, right = pos.x + dims.x;
			if( dims.x < 0 )
			{
				left = pos.x + dims.x;
				right = pos.x;
			}
			float top = pos.y, bottom = pos.y + dims.y;
			if( dims.y < 0 )
			{
				top = pos.y + dims.y;
				bottom = pos.y;
			}
			
			// First test if the corners would snap to each other
			float threshSq = threshold * threshold;
			float curMinDistSq = threshSq;
			int closestSnapIdx = -1, closestBoxIdx = -1;
			for( int snapIdx = 0; snapIdx < 4; ++snapIdx )
			{
				for( int boxIdx = 0; boxIdx < 4; ++boxIdx )
				{
					// Get the distance between the verts
					float distSq = ( this[boxIdx] - snapBox[snapIdx] ).GetMagSq();
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
				snapVect = snapBox[closestSnapIdx] - this[closestBoxIdx];
				return true;
			}

			// See if the boxes are lined up for snapping
			float minMoveDist = threshold;
			bool moveHorz = true;
			Interval ourVertInterval = Interval.MakeClosed( pos.y, pos.y + dims.y );
			Interval ourHorzInterval = Interval.MakeClosed( pos.x, pos.x + dims.x );
			Interval snapVertInterval = Interval.MakeClosed( snapBox.pos.y, snapBox.pos.y + snapBox.dims.y );
			Interval snapHorzInterval = Interval.MakeClosed( snapBox.pos.x, snapBox.pos.x + snapBox.dims.x );

			// If the boxes can snap horizontally
			if( ourVertInterval.IsIntervalIntersecting( snapVertInterval ) )
			{
				// Test our left to snap left
				float dist = snapBox.pos.x - pos.x;
				if( Math.Abs( dist ) < Math.Abs( minMoveDist ) )
					minMoveDist = dist;

				// Test our left to snap right
				dist = (snapBox.pos.x + snapBox.dims.y) - pos.x;
				if( Math.Abs( dist ) < Math.Abs( minMoveDist ) )
					minMoveDist = dist;

				// Test our right to snap left
				dist = snapBox.pos.x - (pos.x + dims.x);
				if( Math.Abs( dist ) < Math.Abs( minMoveDist ) )
					minMoveDist = dist;

				// Test our right to snap right
				dist = (snapBox.pos.x + snapBox.dims.x) - (pos.x + dims.x);
				if( Math.Abs( dist ) < Math.Abs( minMoveDist ) )
					minMoveDist = dist;
			}

			// If the boxes can snap vertically
			if( ourHorzInterval.IsIntervalIntersecting( snapHorzInterval ) )
			{
				float minDist = threshold;

				// Test our top to snap top
				float dist = snapBox.pos.y - pos.y;
				if( Math.Abs( dist ) < Math.Abs( minDist ) )
					minDist = dist;

				// Test our top to snap bottom
				dist = (snapBox.pos.y + snapBox.dims.y) - pos.y;
				if( Math.Abs( dist ) < Math.Abs( minDist ) )
					minDist = dist;

				// Test our bottom to snap top
				dist = snapBox.pos.y - (pos.y + dims.y);
				if( Math.Abs( dist ) < Math.Abs( minDist ) )
					minDist = dist;

				// Test our bottom to snap bottom
				dist = (snapBox.pos.y + snapBox.dims.y) - pos.y + dims.y;
				if( Math.Abs( dist ) < Math.Abs( minDist ) )
					minDist = dist;

				// If the vertical translation is small than the horizontal one
				if( Math.Abs( minDist ) < Math.Abs( minMoveDist ))
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
					snapVect.Set( minMoveDist, 0 );
					// Else we should move vertically
				else
					snapVect.Set( 0, minMoveDist );

				return true;
			}

			return false;
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Access a vertex bases on quadrants (0 is top right, 1 is top left,
		/// 2 is bottom left, and 3 is bottom right)
		/// </summary>
		////////////////////////////////////////////////////////////////////////
		public Point2 this [ int index ]
		{
			get
			{
				if( index == 0 )
					return new Point2( pos.x + dims.x, pos.y );
				else if( index == 1 )
					return pos;
				else if( index == 2 )
					return new Point2( pos.x, pos.y + dims.y );

				return new Point2( pos.x + dims.x, pos.y + dims.y );
			}
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the area of the box
		/// </summary>
		////////////////////////////////////////////////////////////////////////
		public float Area
		{
			get{ return dims.x * dims.y; }
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string representing the box in the format (x,y,dx,dy)
		/// </summary>
		/// <returns> A string representing the box </returns>
		///////////////////////////////////////////////////////////////////////
		public override string ToString()
		{
			return String.Format( "({0:G},{1:G},{2:G},{3:G})", pos.x, pos.y, dims.x, dims.y );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Parse a box from a string </summary>
		/// <param name="str"> The string to parse </param>
		/// <returns> The box represented by the string </returns>
		///////////////////////////////////////////////////////////////////////
		public static Box2 Parse( string str )
		{
			const string ERROR_STR = "Box2 string not formatted correctly";

			// Ensure a long enough string starting with an open parenthesis
			// (0,0,0,0) 9 char min 
			if( str.Length < 9 && str[0] == '(' )
				throw new FormatException( ERROR_STR );

			Box2 retBox = new Box2();

			// Copy the string to work with
			string subStr = str.Substring( 1 );

			// Read in the parameters
			char[] delims = { ',', ',', ',', ')' };
			for( int paramIndex = 0; paramIndex < delims.Length; ++paramIndex )
			{
				// Get the value string
				int delimitPos = subStr.IndexOf( delims[paramIndex] );
				if( delimitPos < 0 )
					throw new FormatException( ERROR_STR );
				string valStr = subStr.Substring( 0, delimitPos );

				// Get the value from the string
				float paramValue = float.Parse( valStr );

				// Store the value
				if( paramIndex == 0 )
					retBox.pos.x = paramValue;
				else if( paramIndex == 1 )
					retBox.pos.y = paramValue;
				else if( paramIndex == 2 )
					retBox.dims.x = paramValue;
				else if( paramIndex == 3 )
					retBox.dims.y = paramValue;

				// Remove the parameter we just read in from the string and continue
				subStr = subStr.Substring( delimitPos + 1 );
			}

			// Return the box
			return retBox;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Write the box out to a binary stream
		/// </summary>
		/// <param name="binWriter"> The binary writer to use for outputting </param>
		///////////////////////////////////////////////////////////////////////
		public void ToBinary( BinaryWriter binWriter )
		{
			binWriter.Write( pos.x );
			binWriter.Write( pos.y );
			binWriter.Write( dims.x );
			binWriter.Write( dims.y );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initalize the box from a binary stream
		/// </summary>
		/// <param name="binReader"> The binary reader to retrieve the data
		/// from </param>
		///////////////////////////////////////////////////////////////////////
		public void FromBinary( BinaryReader binReader )
		{
			pos.x = binReader.ReadSingle();
			pos.y = binReader.ReadSingle();
			dims.x = binReader.ReadSingle();
			dims.y = binReader.ReadSingle();
		}
	}
}
