using System;
using System.IO;

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A 3-dimensional point.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct Point3
	{
		/// <summary> The x, y, and z position of the point </summary>
		public float x, y, z;


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize a point with an x, y, and z position
		/// </summary>
		///	<param name="inX"> The x component </param>
		///	<param name="inY"> The y component </param>
		///	<param name="inZ"> The z component </param>
		///////////////////////////////////////////////////////////////////////
		public Point3( float inX, float inY, float inZ )
		{
			x = inX; y = inY; z = inZ;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize a point from another point
		/// </summary>
		/// <param name="copyPt"> The point to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Point3( Point3 copyPt )
		{
			x = copyPt.x; y = copyPt.y; z = copyPt.z;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize a point from a 2D point and a Z value
		/// </summary>
		/// <param name="copyPt"> The point to copy </param>
		/// <param name="zVal"> The Z value of the point </param>
		///////////////////////////////////////////////////////////////////////
		public Point3( Point2 copyPt, float zVal )
		{
			x = copyPt.x; y = copyPt.y; z = zVal;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set the components of the point
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public void Set( float inX, float inY, float inZ )
		{
			x = inX; y = inY; z = inZ;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Access the components via an index
		/// </summary>
		/// <param name="index"> The index to access the components, 0 for x, 1 for y, and anything else for z </param>
		/// <returns> The value of the component accessed </returns>
		///////////////////////////////////////////////////////////////////////
		public float this[ int index ]
		{
			get
			{
				// On the 0 index return the x component
				if( index == 0 )
					return x;
				// On 1 return the y component
				else if( index == 1 )
					return y;

				// On other indices return the z component
				return z;
			}

			set
			{
				// On the 0 index set the x component
				if( index == 0 )
					x = value;
				// On 1 set the y component
				else if( index == 1 )
					y = value;
				// Store z otherwise
				else
					z = value;
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
		public static Point3 operator +( Point3 pt, Vector3 rhs )
		{
			return new Point3( pt.x + rhs.x, pt.y + rhs.y, pt.z + rhs.z );
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the difference between two points, represented by a vector.
		/// </summary>
		/// <param name="lhs"> The first point </param>
		/// <param name="rhs"> The second point </param>
		/// <returns> The offset between two points described by a vector from the second point to the first point </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3 operator -( Point3 lhs, Point3 rhs )
		{
			return new Vector3( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Move a point by a vector </summary>
		/// <param name="lhs"> The point </param>
		/// <param name="rhs"> The vector to subtract from the point </param>
		/// <returns> The point offset by the negative vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Point3 operator -( Point3 lhs, Vector3 rhs )
		{
			return new Point3( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Compare two points </summary>
		/// <param name="lhs"> The point on the left hand side of the equals </param>
		/// <param name="rhs"> The point on the right hand side of the equals </param>
		/// <returns> True if the points are equal false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public static bool operator ==( Point3 lhs, Point3 rhs )
		{
			return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Compare two points for inequality </summary>
		/// <param name="lhs"> The point on the left hand side of the equals </param>
		/// <param name="rhs"> The point on the right hand side of the equals </param>
		/// <returns> True if the points are not equal false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public static bool operator !=( Point3 lhs, Point3 rhs )
		{
			return (lhs.x != rhs.x) || (lhs.y != rhs.y) || (lhs.z != rhs.z);
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Determine if two points are equal </summary>
		/// <param name="obj"> The object to test for equality with </param>
		/// <returns> Whether or not the objects are equal </returns>
		///////////////////////////////////////////////////////////////////////
		public override bool Equals(object obj)
		{
			if( obj is Point3 )
				return this == (Point3)obj;

			// The other object is not a point so bail
			return false;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the hash code for the point.  The value returned is the
		/// components multiplied by each other.
		/// </summary>
		/// <returns> A hash code that can be used in hashing algorithms </returns>
		///////////////////////////////////////////////////////////////////////
		public override int GetHashCode()
		{
			return (int)Math.Floor( (double)(x * y * z) );
		}



		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string representing the point in the format (x,y,z)
		/// </summary>
		/// <returns> A string representing the point </returns>
		///////////////////////////////////////////////////////////////////////
		public override string ToString()
		{
			return String.Format( "({0:G},{1:G},{2:G})", x, y, z );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Parse a point from a string </summary>
		/// <param name="str"> The string to parse </param>
		/// <returns> The point stored in the string </returns>
		///////////////////////////////////////////////////////////////////////
		public static Point3 Parse( string str )
		{
			try
			{
				// Simply use the Vector3's parse
				Vector3 retVect = Vector3.Parse( str );
				return new Point3( retVect.x, retVect.y, retVect.z );
			}
			catch( FormatException )
			{
				throw new FormatException( "Point3 string not formatted correctly" );
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
			binWriter.Write( z );
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
			z = binReader.ReadSingle();
		}
	}
}
