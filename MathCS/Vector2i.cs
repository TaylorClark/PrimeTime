using System;
using System.IO;

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A 2-dimensional vector.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct Vector2i
	{
		/// <summary> The x and y components of the vector </summary>
		public int x, y;


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector with an x and y component.
		/// </summary>
		/// <param name="inX"> The x component </param>
		/// <param name="inY"> The y compenent </param>
		///////////////////////////////////////////////////////////////////////
		public Vector2i( int inX, int inY ) 
		{
			x = inX; y = inY;
		}
		

		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from another vector.
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Vector2i( Vector2i rhs )
		{
			x = rhs.x; y = rhs.y;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from another floating point vector.
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Vector2i( Vector2 rhs )
		{
			x = (int)rhs.x; y = (int)rhs.y;
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set the x and y components of the vector.
		/// </summary>
		/// <param name="nX"> The x component </param>
		/// <param name="nY"> The y component </param>
		///////////////////////////////////////////////////////////////////////
		public void Set( int nX, int nY )
		{
			x = nX; y = nY;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Negate the vector
		/// </summary>
		/// <param name="inVect"> The vector to negate </param>
		/// <returns> The negated vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector2i operator -( Vector2i inVect )
		{
			return new Vector2i( -inVect.x, -inVect.y );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the dot product of two vectors.
		/// </summary>
		/// <param name="rhs"> The vector to dot with </param>
		/// <returns> The dot product </returns>
		///////////////////////////////////////////////////////////////////////
		public int Dot( Vector2i rhs )
		{
			return ((x * rhs.x) + (y * rhs.y));
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The cross product of 2 vectors.  Since we know this vector lies in 2-space, we know the
		/// cross product will give us a normal to that plane.  The return value is
		/// is the magnitude in that direction
		/// </summary>
		/// <param name="rhs"> The vector to cross with </param>
		/// <returns> The scalar of the z component of the (0,0,1) 3D vector </returns>
		///////////////////////////////////////////////////////////////////////
		public int Cross( Vector2i rhs )
		{
			return ((x * rhs.y) - (rhs.x * y));
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Access the components via an index
		/// </summary>
		/// <param name="index"> The index to access the components, 0 for x and any other index for y </param>
		/// <returns> The value of the component accessed </returns>
		///////////////////////////////////////////////////////////////////////
		public int this[ int index ]
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
		/// Rotate the vector -90 degress based on a cartesian coordinate system with +x to the "right" and +y "up"
		/// </summary>
		/// <returns> The rotated vector </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector2i GetOrthoN90()
		{
			return new Vector2i( y, -x );
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Rotate the vector 90 degress based on a cartesian coordinate system with +x to the "right" and +y "up"
		/// </summary>
		/// <returns> The rotated vector </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector2i GetOrtho90()
		{
			return new Vector2i( -y, x );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the magnitude of the vector
		/// </summary>
		/// <returns> The magnitude of the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public float GetMag()
		{
			return (float)Math.Sqrt( (double)((x * x) + (y * y)) );
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
			return ((x * x) + (y * y));
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a normalized copy of the vector while leaving this vector
		/// unchanged.
		/// </summary>
		/// <returns> This vector normalized </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector2i Normalized()
		{
			// Get the magnitude and ensure it is not 0
			Vector2i retV = new Vector2i( 0, 0 );
			float mag = GetMag();
			if( mag == 0.0f )
				return retV;

			// Get the reciprocal magnitude
			mag = 1.0f / mag;

			// Normalize the vector
			retV.x = (int)((float)x * mag);
			retV.y = (int)((float)y * mag);
			return retV;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Normalize this vector
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		void Normalize()
		{
			// Get the magnitude and ensure it is not 0
			float mag = GetMag();
			if( mag == 0.0f )
				return;

			// Get the reciprocal magnitude
			mag = 1.0f / mag;

			// Normalize the vector
			x = (int)((float)x * mag);
			y = (int)((float)y * mag);
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Project this vector onto another vector.
		/// </summary>
		/// <param name="proj"> The vector to project onto </param>
		/// <returns> The projected vector </returns>
		///////////////////////////////////////////////////////////////////////
		Vector2i ProjectOnto( Vector2i proj )
		{
			// If the magnitude is 0, bail
			if( proj.GetMagSq() == 0 )
				return new Vector2i( 0,0 );

			int scalar = Dot( proj ) / proj.GetMagSq();
			return (proj * scalar);
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the vector that represents this vector reflected about the
		/// passed in vector
		/// </summary>
		/// <param name="reflectVect"> The vector to reflect about </param>
		/// <returns> The reflected vector </returns>
		////////////////////////////////////////////////////////////////////////
		Vector2i ReflectAcrossVector( Vector2i reflectVect )
		{
			return ( (reflectVect * 2 * Dot(reflectVect)) - this );
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add two vectors
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="rhs"> The second vector </param>
		/// <returns> The vector representing the sum of the two input vectors </returns>
		////////////////////////////////////////////////////////////////////////
		public static Vector2i operator + ( Vector2i lhs, Vector2i rhs )
		{
			return new Vector2i( lhs.x + rhs.x, lhs.y + rhs.y );
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Subtract two vectors
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="rhs"> The second vector </param>
		/// <returns> The vector representing the subtraction of the vectors</returns>
		////////////////////////////////////////////////////////////////////////
		public static Vector2i operator - ( Vector2i lhs, Vector2i rhs )
		{
			return new Vector2i( lhs.x - rhs.x, lhs.y - rhs .y );
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Divide the components of a vector by a scalar
		/// </summary>
		/// <param name="lhs"> The vector to divide </param>
		/// <param name="den"> The scalar </param>
		/// <returns> The vector with scaled components </returns>
		////////////////////////////////////////////////////////////////////////
		public static Vector2i operator / ( Vector2i lhs, int den )
		{
			return new Vector2i( lhs.x / den, lhs.y / den );
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Divide the components of a vector by a scalar
		/// </summary>
		/// <param name="lhs"> The vector to divide </param>
		/// <param name="den"> The scalar </param>
		/// <returns> The vector with scaled components </returns>
		////////////////////////////////////////////////////////////////////////
		public static Vector2i operator / ( Vector2i lhs, float den )
		{
			return new Vector2i( (int)((float)lhs.x / den), (int)((float)lhs.y / den) );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Scale a vector by a scalar.
		/// </summary>
		/// <param name="lhs"> The vector to scale </param>
		/// <param name="scalar"> The amount to scale </param>
		/// <returns> The scaled vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector2i operator * ( Vector2i lhs, int scalar )
		{
			return new Vector2i( lhs.x * scalar, lhs.y * scalar );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Scale a vector by a scalar.
		/// </summary>
		/// <param name="lhs"> The vector to scale </param>
		/// <param name="scalar"> The amount to scale </param>
		/// <returns> The scaled vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector2i operator * ( Vector2i lhs, float scalar )
		{
			return new Vector2i( (int)((float)lhs.x * scalar), (int)((float)lhs.y * scalar) );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string representing the vector in the format (x,y)
		/// </summary>
		/// <returns> A string representing the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public override string ToString()
		{
			return String.Format( "({0},{1})", x, y );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Parse a vector from a string </summary>
		/// <param name="str"> The string to parse </param>
		/// <returns> The vector stored in the string </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector2i Parse( string str )
		{
			// Ensure a long enough string starting with an open parenthesis
			// (0,0) 5 char min 
			if( str.Length < 5 && str[0] == '(' )
				throw new FormatException( "Vector2i string not formatted correctly" );

			Vector2i retVect;

			// Copy the string to work with
			string subStr = str.Substring( 1 );

			// Get the x value
			int delimitPos = subStr.IndexOf( ',' );
			if( delimitPos < 0 )
				throw new FormatException( "Vector2i string not formatted correctly" );
			string valStr = subStr.Substring( 0, delimitPos );
			retVect.x = int.Parse( valStr );
			subStr = subStr.Substring( delimitPos + 1 );

			// Get the y value
			delimitPos = subStr.IndexOf( ')' );
			if( delimitPos < 0 )
				throw new FormatException( "Vector2i string not formatted correctly" );
			valStr = subStr.Substring( 0, delimitPos );
			retVect.y = int.Parse( valStr );
			subStr = subStr.Substring( delimitPos + 1 );

			// Return the vector
			return retVect;
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
		/// Read point in from a binary stream
		/// </summary>
		/// <param name="binReader"> The binary reader to retrieve the data
		/// from </param>
		///////////////////////////////////////////////////////////////////////
		public void FromBinary( BinaryReader binReader )
		{
			x = binReader.ReadInt32();
			y = binReader.ReadInt32();
		}
	}
}
