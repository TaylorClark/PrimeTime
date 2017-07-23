using System;
using System.IO;

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A 2-dimensional point.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct Point2
	{
		/// <summary> The x and y position of the point </summary>
		public float x, y;


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize a point with an x and y position
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public Point2( float inX, float inY )
		{
			x = inX; y = inY;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize a point from another point
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public Point2( Point2 copyPt )
		{
			x = copyPt.x; y = copyPt.y;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize a point from an integer point
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public Point2( Point2i copyPt )
		{
			x = (float)copyPt.x; y = (float)copyPt.y;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set the components of the point
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public void Set( float inX, float inY )
		{
			x = inX; y = inY;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Access the components via an index
		/// </summary>
		/// <param name="index"> The index to access the components, 0 for x and any other index for y </param>
		/// <returns> The value of the component accessed </returns>
		///////////////////////////////////////////////////////////////////////
		public float this[ int index ]
		{
			get
			{
				// On the 0 index return the x component
				if( index == 0 )
					return x;

				// On other indices return the y component
				return y;
			}

			set
			{
				// On the 0 index set the x component
				if( index == 0 )
					x = value;
				// On other indices set the y component
				else
					y = value;
			}
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a point and a vector to offset the point by the vector.
		/// </summary>
		/// <param name="pt"> The point to add to </param>
		/// <param name="rhs"> The vector to offset the point by </param>
		/// <returns> The point offset by the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Point2 operator +( Point2 pt, Vector2 rhs )
		{
			return new Point2( pt.x + rhs.x, pt.y + rhs.y );
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the difference between two points, represented by a vector.
		/// </summary>
		/// <param name="lhs"> The first point </param>
		/// <param name="rhs"> The second point </param>
		/// <returns> The offset between two points described by a vector from the second point to the first point </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector2 operator -( Point2 lhs, Point2 rhs )
		{
			return new Vector2( lhs.x - rhs.x, lhs.y - rhs.y );
		}
		

		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Multiply a point's components by a scalar.
		/// </summary>
		/// <param name="lhs"> The point to scale </param>
		/// <param name="scalar"> The scalar </param>
		/// <returns> The scaled point </returns>
		///////////////////////////////////////////////////////////////////////
		public static Point2 operator *( Point2 lhs, float scalar )
		{
			return new Point2( lhs.x * scalar, lhs.y * scalar );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string representing the point in the format (x,y)
		/// </summary>
		/// <returns> A string representing the point </returns>
		///////////////////////////////////////////////////////////////////////
		public override string ToString()
		{
			return String.Format( "({0:G},{1:G})", x, y );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Parse a point from a string </summary>
		/// <param name="str"> The string to parse </param>
		/// <returns> The point stored in the string </returns>
		///////////////////////////////////////////////////////////////////////
		public static Point2 Parse( string str )
		{
			try
			{
				// Simply use the Vector2's parse
				Vector2 retVect = Vector2.Parse( str );
				return new Point2( retVect.x, retVect.y );
			}
			catch( FormatException )
			{
				throw new FormatException( "Point2 string not formatted correctly" );
			}
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Write the point out to a binary stream
		/// </summary>
		/// <param name="binWriter"> The binary writer to use for outputting </param>
		///////////////////////////////////////////////////////////////////////
		public void ToBinary( BinaryWriter binWriter )
		{
			binWriter.Write( x );
			binWriter.Write( y );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Read a point in from a binary stream
		/// </summary>
		/// <param name="binReader"> The binary reader to retrieve the data
		/// from </param>
		///////////////////////////////////////////////////////////////////////
		public void FromBinary( BinaryReader binReader )
		{
			x = binReader.ReadSingle();
			y = binReader.ReadSingle();
		}
	}
}
