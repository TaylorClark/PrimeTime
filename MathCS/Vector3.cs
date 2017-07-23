using System;
using System.IO;

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A 3-dimensional vector.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct Vector3
	{
		/// <summary> The x, y, and z components of the vector </summary>
		public float x, y, z;


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector with an x, y, and z component
		/// </summary>
		/// <param name="inX"> The x component </param>
		/// <param name="inY"> The y compenent </param>
		/// <param name="inZ"> The z compenent </param>
		///////////////////////////////////////////////////////////////////////
		public Vector3( float inX, float inY, float inZ ) 
		{
			x = inX; y = inY; z = inZ;
		}
		

		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from another vector
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Vector3( Vector3 rhs )
		{
			x = rhs.x; y = rhs.y; z = rhs.z;
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from a 2D vector and a Z value
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Vector3( Vector2 rhs, float zVal )
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
		public void Set( float nX, float nY, float nZ )
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
		public static Vector3 operator -( Vector3 inVect )
		{
			return new Vector3( -inVect.x, -inVect.y, -inVect.z );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if another vector "nearly equals" vector.  This function tries
		/// to compensate for floating point precision by testing if the components
		/// are "close enough" to each other.
		/// </summary>
		/// <param name="rhs"> The vector to test against</param>
		/// <returns> Whether or not the vectors are equal to each other, within a treshold </returns>
		///////////////////////////////////////////////////////////////////////
		public bool NearlyEquals( Vector3 rhs )
		{
			const float V3_Epsilon = 0.00048828125f;
			return NearlyEquals( rhs, V3_Epsilon );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if another vector "nearly equals" vector.  This function tries
		/// to compensate for floating point precision by testing if the components
		/// are "close enough" to each other.
		/// </summary>
		/// <param name="rhs"> The vector to test against</param>
		/// <param name="threshold"> The threshold to test against </param>
		/// <returns> Whether or not the vectors are equal to each other, within a treshold </returns>
		///////////////////////////////////////////////////////////////////////
		public bool NearlyEquals( Vector3 rhs, float threshold )
		{
			// Get the differences
			float dX = x - rhs.x;
			float dY = y - rhs.y;
			float dZ = z - rhs.z;

			// See if the vectors are close enough
			if( dX < threshold && dX > -threshold
				&& dY < threshold && dY > -threshold
				&& dZ < threshold && dZ > -threshold )
				return true;

			return false;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the dot product of two vectors.
		/// </summary>
		/// <param name="rhs"> The vector to dot with </param>
		/// <returns> The dot product </returns>
		///////////////////////////////////////////////////////////////////////
		public float Dot( Vector3 rhs )
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
		public Vector3 Cross( Vector3 rhs )
		{
			Vector3 returnVector = new Vector3();
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
		/// Get the magnitude of the vector
		/// </summary>
		/// <returns> The magnitude of the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public float GetMag()
		{
			return (float)Math.Sqrt( (double)((x * x) + (y * y) + (z * z)) );
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the squared magnitude of the vector.  This function does not
		/// contain a call to the square root function and can be used for
		/// optimizations.
		/// </summary>
		/// <returns> The squared magnitude of the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public float GetMagSq()
		{
			return ((x * x) + (y * y) + (z * z));
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a normalized copy of the vector while leaving this vector
		/// unchanged.
		/// </summary>
		/// <returns> This vector normalized </returns>
		/// <seealso cref="Normalize" />
		///////////////////////////////////////////////////////////////////////
		public Vector3 Normalized()
		{
			// Get the magnitude and ensure it is not 0
			Vector3 retV = new Vector3( 0, 0, 0 );
			float mag = GetMag();
			if( mag == 0.0f )
				return retV;

			// Get the reciprocal magnitude
			mag = 1.0f / mag;

			// Normalize the vector
			retV.x = x * mag;
			retV.y = y * mag;
			retV.z = z * mag;
			return retV;
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Normalize this vector
		/// </summary>
		/// <seealso cref="Normalized" />
		///////////////////////////////////////////////////////////////////////
		public void Normalize()
		{
			// Get the magnitude and ensure it is not 0
			float mag = GetMag();
			if( mag == 0.0f )
				return;

			// Get the reciprocal magnitude
			mag = 1.0f / mag;

			// Normalize the vector
			x *= mag;
			y *= mag;
			z *= mag;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Reflect this vector across another </summary>
		/// <param name="reflectVect"> The vector to reflect across </param>
		/// <returns> The reflected vector </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector3 ReflectAcrossVector( Vector3 reflectVect )
		{
			return ( (reflectVect * 2.0f * Dot(reflectVect)) - this );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if this is the zero vector
		/// </summary>
		/// <returns> Whether or not this is the zero vector </returns>
		///////////////////////////////////////////////////////////////////////
		public bool IsZeroVector()
		{
			return (x == 0.0f) && (y == 0.0f) && (z == 0.0f);
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a vector that is orthogonal to this vector
		/// </summary>
		/// <returns> The orthogonal vector or the zero vector if no orthogonal
		/// vector could be found </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector3 GetRandomOrthoVector()
		{
			// If we have a 0 vector
			if( IsZeroVector() )
				return new Vector3( 0, 0, 0 );

			// Get a random vector from this vector
			Vector3 randVect = new Vector3( z, x, y );

			// Cross the random vector with this vector
			Vector3 retVect = new Vector3( Cross(randVect) );

			// Ensure it is orthogonal
			float dotVal = Dot( retVect );
			if( (dotVal != 0.0f) || retVect.IsZeroVector() )
			{
				// Try crossing with another vector
				randVect.Set( -x, -y, z );
				retVect = Cross(randVect);
				dotVal = Dot( retVect );

				if( (dotVal != 0.0f) || retVect.IsZeroVector() )
				{
					// Try crossing with another vector
					randVect.Set( x, y, -z );
					retVect = Cross(randVect);
					// Just use the vector at this point
				}
			}

			// Return the vector
			return retVect;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Project this vector onto another
		/// </summary>
		/// <param name="proj"> The vector to project onto </param>
		/// <returns> The projected vector </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector3 ProjectOnto( Vector3 proj )
		{
			// If the magnitude is 0, bail
			if( proj.GetMagSq() == 0.0f )
				return new Vector3();

			float scalar = Dot( proj ) / proj.GetMagSq();
			return (proj * scalar);
		}

	
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add two vectors
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="rhs"> The second vector </param>
		/// <returns> The vector that is the sum of the two vectors </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3 operator + ( Vector3 lhs, Vector3 rhs )
		{
			return new Vector3( lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Subtract two vectors
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="rhs"> The second vector </param>
		/// <returns> The vector that is the difference of the two vectors </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3 operator - ( Vector3 lhs, Vector3 rhs )
		{
			return new Vector3( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Divide each component of the vector by a scalar
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="den"> The scalar to divide each component by </param>
		/// <returns> The vector with scaled compoents </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3 operator / ( Vector3 lhs, float den )
		{
			return new Vector3( lhs.x / den, lhs.y / den, lhs.z / den );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Scale a vector by a scalar.
		/// </summary>
		/// <param name="lhs"> The vector to scale </param>
		/// <param name="scalar"> The amount to scale </param>
		/// <returns> The scaled vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3 operator * ( Vector3 lhs, float scalar )
		{
			return new Vector3( lhs.x * scalar, lhs.y * scalar, lhs.z * scalar );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Compare two vectors </summary>
		/// <param name="lhs"> The vector on the left hand side of the equals </param>
		/// <param name="rhs"> The vector on the right hand side of the equals </param>
		/// <returns> True if the vectors are equal false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public static bool operator ==( Vector3 lhs, Vector3 rhs )
		{
			return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Compare two vectors for inequality </summary>
		/// <param name="lhs"> The vector on the left hand side of the equals </param>
		/// <param name="rhs"> The vector on the right hand side of the equals </param>
		/// <returns> True if the vectors are not equal false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public static bool operator !=( Vector3 lhs, Vector3 rhs )
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
			if( obj is Vector3 )
				return this == (Vector3)obj;

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
			return (int)Math.Floor( (double)(x * y * z) );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string representing the vector in the format (x,y,z)
		/// </summary>
		/// <returns> A string representing the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public override string ToString()
		{
			return String.Format( "({0:G},{1:G},{2:G})", x, y, z );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Parse a vector from a string </summary>
		/// <param name="str"> The string to parse </param>
		/// <returns> The vector stored in the string </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector3 Parse( string str )
		{
			// Ensure a long enough string starting with an open parenthesis
			// (0,0,0) 7 char min 
			if( str.Length < 7 && str[0] == '(' )
				throw new FormatException( "Vector3 string not formatted correctly" );

			Vector3 retVect;

			// Copy the string to work with
			string subStr = str.Substring( 1 );

			// Get the x value
			int delimitPos = subStr.IndexOf( ',' );
			if( delimitPos < 0 )
				throw new FormatException( "Vector3 string not formatted correctly" );
			string valStr = subStr.Substring( 0, delimitPos );
			retVect.x = float.Parse( valStr );
			subStr = subStr.Substring( delimitPos + 1 );

			// Get the y value
			delimitPos = subStr.IndexOf( ',' );
			if( delimitPos < 0 )
				throw new FormatException( "Vector3 string not formatted correctly" );
			valStr = subStr.Substring( 0, delimitPos );
			retVect.y = float.Parse( valStr );
			subStr = subStr.Substring( delimitPos + 1 );

			// Get the z value
			delimitPos = subStr.IndexOf( ')' );
			if( delimitPos < 0 )
				throw new FormatException( "Vector3 string not formatted correctly" );
			valStr = subStr.Substring( 0, delimitPos );
			retVect.z = float.Parse( valStr );

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
			x = binReader.ReadSingle();
			y = binReader.ReadSingle();
			z = binReader.ReadSingle();
		}
	}
}
