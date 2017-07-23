using System;
using System.IO;

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A 3-dimensional vector with integer components.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct Vector3i
	{
		/// <summary> The x, y, and z components of the vector </summary>
		public int x, y, z;


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector with an x, y, and z component
		/// </summary>
		/// <param name="inX"> The x component </param>
		/// <param name="inY"> The y compenent </param>
		/// <param name="inZ"> The z compenent </param>
		///////////////////////////////////////////////////////////////////////
		public Vector3i( int inX, int inY, int inZ ) 
		{
			x = inX; y = inY; z = inZ;
		}
		

		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from another vector
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Vector3i( Vector3i rhs )
		{
			x = rhs.x; y = rhs.y; z = rhs.z;
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from a 2D vector and a Z value
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		/// <param name="zVal"> The value of the Z component </param>
		///////////////////////////////////////////////////////////////////////
		public Vector3i( Vector2i rhs, int zVal )
		{
			x = rhs.x; y = rhs.y; z = zVal;
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set the x and y components of the vector.
		/// </summary>
		/// <param name="nX"> The x component </param>
		/// <param name="nY"> The y component </param>
		/// <param name="nZ"> The z component </param>
		///////////////////////////////////////////////////////////////////////
		public void Set( int nX, int nY, int nZ )
		{
			x = nX; y = nY; z = nZ;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Negate the vector
		/// </summary>
		/// <param name="inVect"> The vector to negate </param>
		/// <returns> The negated vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3i operator -( Vector3i inVect )
		{
			return new Vector3i( -inVect.x, -inVect.y, -inVect.z );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the dot product of two vectors.
		/// </summary>
		/// <param name="rhs"> The vector to dot with </param>
		/// <returns> The dot product </returns>
		///////////////////////////////////////////////////////////////////////
		public int Dot( Vector3i rhs )
		{
			return ((x * rhs.x) + (y * rhs.y) + (z * rhs.z));
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The cross product of 2 vectors
		/// </summary>
		/// <param name="rhs"> The vector to cross with </param>
		/// <returns> The resultant vector </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector3i Cross( Vector3i rhs )
		{
			Vector3i returnVector = new Vector3i();
			returnVector.x = (y * rhs.z) - (z * rhs.y);
			returnVector.y = (z * rhs.x) - (x * rhs.z);
			returnVector.z = (x * rhs.y) - (y * rhs.x);
			return returnVector;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Access the components via an index
		/// </summary>
		/// <param name="index"> The index to access the components, 0 for x, 1 for y, and anything else for z </param>
		/// <returns> The value of the component accessed </returns>
		///////////////////////////////////////////////////////////////////////
		public int this[ int index ]
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
		/// Get the magnitude of the vector
		/// </summary>
		/// <returns> The magnitude of the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public int GetMag()
		{
			return (int)Math.Sqrt( (double)((x * x) + (y * y) + (z * z)) );
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the squared magnitude of the vector.  This function does not
		/// contain a call to the square root function and can be used for
		/// optimizations.
		/// </summary>
		/// <returns> The squared magnitude of the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public int GetMagSq()
		{
			return ((x * x) + (y * y) + (z * z));
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if this is the zero vector
		/// </summary>
		/// <returns> Whether or not this is the zero vector </returns>
		///////////////////////////////////////////////////////////////////////
		public bool IsZeroVector()
		{
			return (x == 0) && (y == 0) && (z == 0);
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add two vectors
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="rhs"> The second vector </param>
		/// <returns> The vector that is the sum of the two vectors </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3i operator + ( Vector3i lhs, Vector3i rhs )
		{
			return new Vector3i( lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Subtract two vectors
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="rhs"> The second vector </param>
		/// <returns> The vector that is the difference of the two vectors </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3i operator - ( Vector3i lhs, Vector3i rhs )
		{
			return new Vector3i( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Divide each component of the vector by a scalar
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="den"> The scalar to divide each component by </param>
		/// <returns> The vector with scaled compoents </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3i operator / ( Vector3i lhs, int den )
		{
			return new Vector3i( lhs.x / den, lhs.y / den, lhs.z / den );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Scale a vector by a scalar.
		/// </summary>
		/// <param name="lhs"> The vector to scale </param>
		/// <param name="scalar"> The amount to scale </param>
		/// <returns> The scaled vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3i operator * ( Vector3i lhs, int scalar )
		{
			return new Vector3i( lhs.x * scalar, lhs.y * scalar, lhs.z * scalar );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Compare two vectors </summary>
		/// <param name="lhs"> The vector on the left hand side of the equals </param>
		/// <param name="rhs"> The vector on the right hand side of the equals </param>
		/// <returns> True if the vectors are equal false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public static bool operator ==( Vector3i lhs, Vector3i rhs )
		{
			return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Compare two vectors for inequality </summary>
		/// <param name="lhs"> The vector on the left hand side of the equals </param>
		/// <param name="rhs"> The vector on the right hand side of the equals </param>
		/// <returns> True if the vectors are not equal false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public static bool operator !=( Vector3i lhs, Vector3i rhs )
		{
			return (lhs.x != rhs.x) || (lhs.y != rhs.y) || (lhs.z != rhs.z);
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Determine if two vectors are equal </summary>
		/// <param name="obj"> The object to test for equality with </param>
		/// <returns> Whether or not the objects are equal </returns>
		///////////////////////////////////////////////////////////////////////
		public override bool Equals(object obj)
		{
			if( obj is Vector3i )
				return this == (Vector3i)obj;

			// The other object is not a point so bail
			return false;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the hash code for the vector.  The value returned is the
		/// components multiplied by each other.
		/// </summary>
		/// <returns> A hash code that can be used in hashing algorithms </returns>
		///////////////////////////////////////////////////////////////////////
		public override int GetHashCode()
		{
			return (x * y * z);
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string representing the vector in the format (x,y,z)
		/// </summary>
		/// <returns> A string representing the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public override string ToString()
		{
			return String.Format( "({0},{1},{2})", x, y, z );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Parse a vector from a string </summary>
		/// <param name="str"> The string to parse </param>
		/// <returns> The vector stored in the string </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3i Parse( string str )
		{
			// Ensure a long enough string starting with an open parenthesis
			// (0,0,0) 7 char min 
			if( str.Length < 7 && str[0] == '(' )
				throw new FormatException( "Vector3i string not formatted correctly" );

			Vector3i retVect;

			// Copy the string to work with
			string subStr = str.Substring( 1 );

			// Get the x value
			int delimitPos = subStr.IndexOf( ',' );
			if( delimitPos < 0 )
				throw new FormatException( "Vector3i string not formatted correctly" );
			string valStr = subStr.Substring( 0, delimitPos );
			retVect.x = int.Parse( valStr );
			subStr = subStr.Substring( delimitPos + 1 );

			// Get the y value
			delimitPos = subStr.IndexOf( ',' );
			if( delimitPos < 0 )
				throw new FormatException( "Vector3i string not formatted correctly" );
			valStr = subStr.Substring( 0, delimitPos );
			retVect.y = int.Parse( valStr );
			subStr = subStr.Substring( delimitPos + 1 );

			// Get the z value
			delimitPos = subStr.IndexOf( ')' );
			if( delimitPos < 0 )
				throw new FormatException( "Vector3i string not formatted correctly" );
			valStr = subStr.Substring( 0, delimitPos );
			retVect.z = int.Parse( valStr );

			// Return the vector
			return retVect;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Write the vector out to a binary stream
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
		/// Read the vector in from a binary stream
		/// </summary>
		/// <param name="binReader"> The binary reader to retrieve the data
		/// from </param>
		///////////////////////////////////////////////////////////////////////
		public void FromBinary( BinaryReader binReader )
		{
			x = binReader.ReadInt32();
			y = binReader.ReadInt32();
			z = binReader.ReadInt32();
		}
	}
}
