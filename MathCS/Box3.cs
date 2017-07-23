using System;
using System.IO;

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// This class encapsulates the functionality of a axis aligned 3-D box.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct Box3
	{
		/// <summary> The position of the box </summary>
		public Point3 pos;

		/// <summary> The dimensions of the box </summary>
		public Vector3 dims;

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> A copy constructor </summary>
		/// <param name="copy"> The box to copy </param>
		////////////////////////////////////////////////////////////////////////
		public Box3( Box3 copy )
		{
			pos = copy.pos; dims = copy.dims;
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Construct a pox from a position and size
		/// </summary>
		/// <param name="inPos"> The position of the box </param>
		/// <param name="inSize"> The size of the box </param>
		////////////////////////////////////////////////////////////////////////
		public Box3( Point3 inPos, Vector3 inSize )
		{
			pos = inPos;
			dims = inSize;
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Construct a pox from 2 points
		/// </summary>
		/// <param name="pt1"> One point on the box </param>
		/// <param name="pt2"> The opposite corner on the box from pt1 </param>
		////////////////////////////////////////////////////////////////////////
		public Box3( Point3 pt1, Point3 pt2 )
		{
			pos = pt1;
			dims = pt2 - pt1;
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> A box constructor that initializes the data </summary>
		/// <param name="x"> The x position of the box </param>
		/// <param name="y"> The y position of the box </param>
		/// <param name="z"> The z position of the box </param>
		/// <param name="width"> The width of the box </param>
		/// <param name="height"> The height of the box </param>
		/// <param name="depth"> The depth of the box </param>
		////////////////////////////////////////////////////////////////////////
		public Box3( float x, float y, float z, float width, float height, float depth )
		{
			// The fields must be fully initialized in C#, so we can't call Set
			pos = new Point3( x, y, z );
			dims = new Vector3( width, height, depth );
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Does this box intersect another box
		/// </summary>
		/// <param name="testBox"> The box to test against </param>
		/// <returns> Whether or not the boxes intersect </returns>
		////////////////////////////////////////////////////////////////////////
		public bool DoesIntersectBox( Box3 testBox )
		{
			// Treat the box dimensions as intervals and see if the horizontal and
			// vertical interval intersect
			Interval ourVertInterval = Interval.MakeHalfOpen( pos.y, pos.y + dims.y );
			Interval ourHorzInterval = Interval.MakeHalfOpen( pos.x, pos.x + dims.x );
			Interval ourDepthInterval = Interval.MakeHalfOpen( pos.z, pos.z + dims.z );
			
			Interval testVertInterval = Interval.MakeHalfOpen( testBox.pos.y, testBox.pos.y + testBox.dims.y );
			Interval testHorzInterval = Interval.MakeHalfOpen( testBox.pos.x, testBox.pos.x + testBox.dims.x );
			Interval testDepthInterval = Interval.MakeHalfOpen( testBox.pos.z, testBox.pos.z + testBox.dims.z );

			return ourVertInterval.IsIntervalIntersecting( testVertInterval )
				&& ourHorzInterval.IsIntervalIntersecting( testHorzInterval )
				&& ourDepthInterval.IsIntervalIntersecting( testDepthInterval );
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> Is a point contained within this box.  The left, top, and
		/// front sides are inclusive but if the point is on the right, bottom,
		/// or back  then it is not contained </summary>
		/// <param name="pt"> The point to test for containment </param>
		/// <returns> Whether or not the point is contained </returns>
		////////////////////////////////////////////////////////////////////////
		public bool ContainsPoint( Point3 pt )
		{
			// If we have a negative dimension, check differently
			for( int compIndex = 0; compIndex < 3; ++compIndex )
			{
				// Get the far extent for this component
				float farVal = pos[compIndex] + dims[compIndex];

				// If the dimensions are negative
				if( dims[compIndex] < 0.0f )
				{
					if( (pt[compIndex] >= pos[compIndex]) || (pt[compIndex] < farVal) )
						return false;
				}
					// Else check normally
				else if( (pt[compIndex] < pos[compIndex]) || (pt[compIndex] >= farVal) )
					return false;
			}

			// The point is inside
			return true;
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Scale the box about its center
		/// </summary>
		/// <param name="factor"> The amount to grow (factor > 1) or shrink
		/// (factor &lt 1) the box </param>
		////////////////////////////////////////////////////////////////////////
		public void Scale( float factor )
		{
			// Get the scaled dimensions
			Vector3 scaledDims = dims * factor;

			// Offset the position by the dimensions
			Vector3 dimChange = scaledDims - dims;
			pos += dimChange * 0.5f;

			// Store the dimensions
			dims = scaledDims;
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the minimum point (The vertex on the box with lowest X,Y,Z
		/// values) of the box
		/// </summary>
		////////////////////////////////////////////////////////////////////////
		public Point3 MinPoint
		{
			get
			{
				Point3 retMinPt = new Point3();

				// Get the minimum point by going through the components of the
				// dimensions
				for( int compIndex = 0; compIndex < 3; ++compIndex )
				{
					// If this dimension is negative then use it
					if( dims[compIndex] < 0.0f )
						retMinPt[compIndex] = pos[compIndex] + dims[compIndex];
						// Otherwise the position component is the minimum
					else
						retMinPt[compIndex] = pos[compIndex];
				}

				// Return the point
				return retMinPt;
			}
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the maximum point (The vertex on the box with greatest X,Y,Z
		/// values) of the box
		/// </summary>
		////////////////////////////////////////////////////////////////////////
		public Point3 MaxPoint
		{
			get
			{
				Point3 retMaxPt = new Point3();

				// Get the maximum point by going through the components of the
				// dimensions
				for( int compIndex = 0; compIndex < 3; ++compIndex )
				{
					// If this dimension is position then use it
					if( dims[compIndex] > 0.0f )
						retMaxPt[compIndex] = pos[compIndex] + dims[compIndex];
					// Otherwise the position component is the maximum
					else
						retMaxPt[compIndex] = pos[compIndex];
				}

				// Return the point
				return retMaxPt;
			}
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the center point of the box
		/// </summary>
		////////////////////////////////////////////////////////////////////////
		public Point3 CenterPoint
		{
			get{ return new Point3( pos + (dims * 0.5f) ); }
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the area of the box
		/// </summary>
		////////////////////////////////////////////////////////////////////////
		public float Area
		{
			get{ return dims.x * dims.y * dims.z; }
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string representing the box in the format (x,y,z,dx,dy,dz)
		/// </summary>
		/// <returns> A string representing the box </returns>
		///////////////////////////////////////////////////////////////////////
		public override string ToString()
		{
			return String.Format( "({0:G},{1:G},{2:G},{3:G},{4:G},{5:G)", pos.x, pos.y, pos.z, dims.x, dims.y, dims.z );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Parse a box from a string </summary>
		/// <param name="str"> The string to parse </param>
		/// <returns> The box represented by the string </returns>
		///////////////////////////////////////////////////////////////////////
		public static Box3 Parse( string str )
		{
			const string ERROR_STR = "Box3 string not formatted correctly";

			// Ensure a long enough string starting with an open parenthesis
			// (0,0,0,0) 9 char min 
			if( str.Length < 9 && str[0] == '(' )
				throw new FormatException( ERROR_STR );

			Box3 retBox = new Box3();

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
			binWriter.Write( pos.z );
			binWriter.Write( dims.x );
			binWriter.Write( dims.y );
			binWriter.Write( dims.z );
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
			pos.z = binReader.ReadSingle();
			dims.x = binReader.ReadSingle();
			dims.y = binReader.ReadSingle();
			dims.z = binReader.ReadSingle();
		}
	}
}
