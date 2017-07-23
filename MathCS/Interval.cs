using System;
using System.IO;

// NOTE: This should be turned into a generic class when VS 2005 comes out.
// Further NOTE:  So should most of the classes in this assebly

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// This class encapsulates the functionality of a mathematical interval.
	/// An interval is a subset of the real line and contains all real numbers
	/// lying between its boundary points, or endpoints.  An interval is said
	/// to be closed if it includes its boundary points, half-open if it
	/// includes one boundary point, and open if it does not include either
	/// boundary point.
	/// </summary>
	//-------------------------------------------------------------------------
	public class Interval
	{
		/// <summary> The range of the interval </summary>
		internal float m_Min = 0.0f, m_Max = 1.0f;

		/// <summary> If the boundary points are closed </summary>
		private bool m_IsMinClosed = false, m_IsMaxClosed = false;

		/// <summary> If the min or max is infinite </summary>
		private bool m_IsMinNegInfinite = false, m_IsMaxInfinite = false;


		///////////////////////////////////////////////////////////////////////
		/// <summary> The default constructor </summary>
		///////////////////////////////////////////////////////////////////////
		public Interval()
		{
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> The copy constructor </summary>
		/// <param name="copyObj"> The object to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Interval( Interval copyObj )
		{
			// Copy all of the data
			m_Min = copyObj.m_Min;
			m_Max = copyObj.m_Max;
			m_IsMinClosed = copyObj.m_IsMinClosed;
			m_IsMaxClosed = copyObj.m_IsMaxClosed;
			m_IsMinNegInfinite = copyObj.m_IsMinNegInfinite;
			m_IsMaxInfinite = copyObj.m_IsMaxInfinite;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Create an interval with both boundary points closed.  The min and
		/// max will be sorted automatically.
		/// </summary>
		/// <param name="val1"> One of the boundary points </param>
		/// <param name="val2"> The other boundary point </param>
		/// <returns> The created closed interval </returns>
		///////////////////////////////////////////////////////////////////////
		static public Interval MakeClosed( float val1, float val2 )
		{
			Interval retInterval = new Interval();

			// Set the closed values
			retInterval.m_IsMinClosed = true;
			retInterval.m_IsMaxClosed = true;

			// Store the values
			retInterval.m_Min = val1;
			retInterval.m_Max = val2;

			// Ensure the values are in the right order
			if( retInterval.m_Min > retInterval.m_Max )
				swap( ref retInterval.m_Min, ref retInterval.m_Max );

			return retInterval;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Create an interval that has one closed boundary point and one open boundary
		/// point.  It will automatically sort the min and max.
		/// </summary>
		/// <param name="closedEndpoint"> The value of the closed endpoint </param>
		/// <param name="openEndpoint"> The value of the open endpoint </param>
		/// <returns> The created half open interval </returns>
		///////////////////////////////////////////////////////////////////////
		static public Interval MakeHalfOpen( float closedEndpoint, float openEndpoint )
		{
			Interval retInterval = new Interval();

			// If the closed endpoint is the min
			if( closedEndpoint < openEndpoint )
			{
				// Set the min to be closed
				retInterval.m_IsMinClosed = true;
					
				// Store the values
				retInterval.m_Min = closedEndpoint;
				retInterval.m_Max = openEndpoint;
			}
			// Else the closed endpoint is the max
			else
			{
				// Set the max to be closed
				retInterval.m_IsMaxClosed = true;
						
				// Store the values
				retInterval.m_Min = openEndpoint;
				retInterval.m_Max = closedEndpoint;
			}

			return retInterval;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Create an interval that includes neither boundary point.  The order
		/// of the boundary points does not matter since they will be auto
		/// sorted.
		/// </summary>
		/// <param name="val1"> A value of one of the boundary points </param>
		/// <param name="val2"> A value of one of the boundary points </param>
		/// <returns> The created open interval </returns>
		///////////////////////////////////////////////////////////////////////
		static public Interval MakeOpen( float val1, float val2 )
		{
			Interval retInterval = new Interval();

			// Store the values
			retInterval.m_Min = val1;
			retInterval.m_Max = val2;

			// Ensure the values are in the right order
			if( retInterval.m_Min > retInterval.m_Max )
				swap( ref retInterval.m_Min, ref retInterval.m_Max );

			return retInterval;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Create an interval with an open enpoint and infinite endpoint.
		/// </summary>
		/// <param name="openVal"> The non-infinite open boundary point </param>
		/// <param name="isNegInfinite"> True if the interval includes infinitely less than the boundary point, false to include infinitely greater than the boundary point </param>
		/// <returns> The created half open infinite interval </returns>
		///////////////////////////////////////////////////////////////////////
		static public Interval MakeOpenInfinite( float openVal, bool isNegInfinite )
		{
			Interval retInterval = new Interval();

			// Store the values, we can store the value in both since setting an
			// endpoint to infinite will ignore the value
			retInterval.m_Min = openVal;
			retInterval.m_Max = openVal;
				
			// Set the infinite flag
			if( isNegInfinite )
				retInterval.m_IsMinNegInfinite = true;
			else
				retInterval.m_IsMaxInfinite = true;

			return retInterval;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Create an interval with a closed enpoint and infinite endpoint.
		/// </summary>
		/// <param name="closedVal"> The value of the boundary point </param>
		/// <param name="isNegInfinite"> True if the interval includes infinitely less than the boundary point, false to include infinitely greater than the boundary point </param>
		/// <returns> The created half open infinite interval </returns>
		///////////////////////////////////////////////////////////////////////
		static public Interval MakeClosedInfinite( float closedVal, bool isNegInfinite )
		{
			Interval retInterval = new Interval();

			// Store the values, we can store the value in both since setting an
			// endpoint to infinite will ignore the value
			retInterval.m_Min = closedVal;
			retInterval.m_Max = closedVal;
				
			// Set the infinite flag
			if( isNegInfinite )
			{
				retInterval.m_IsMinNegInfinite = true;
				retInterval.m_IsMaxClosed = true;
			}
			else
			{
				retInterval.m_IsMaxInfinite = true;
				retInterval.m_IsMinClosed = true;
			}

			return retInterval;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if a value is contained within the interval
		/// </summary>
		/// <param name="val"> The value to test for containment in the interval </param>
		/// <returns> Whether or not the value is contained </returns>
		///////////////////////////////////////////////////////////////////////
		public bool IsValueContained( float val )
		{
			bool satisfiesMin = false, satisfiesMax = false;

			// If the min is infinite then we are greater than the min
			if( m_IsMinNegInfinite )
				satisfiesMin = true;
				// Else if we are greater than the min
			else if( val > m_Min )
				satisfiesMin = true;
				// Else if we include the min value
			else if( m_IsMinClosed && val >= m_Min )
				satisfiesMin = true;

			// If the max is infinite then we are less than the max
			if( m_IsMaxInfinite )
				satisfiesMax = true;
				// Else if we are less than the max
			else if( val < m_Max )
				satisfiesMax = true;
				// Else if we include the max value
			else if( m_IsMaxClosed && val <= m_Max )
				satisfiesMax = true;

			// Return if we are contained
			return satisfiesMin && satisfiesMax;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if another interval intersects this interval
		/// </summary>
		/// <param name="interval"> The interval to test for intersection </param>
		/// <returns> Whether or not the interval is intersecting </returns>
		///////////////////////////////////////////////////////////////////////
		public bool IsIntervalIntersecting( Interval interval )
		{
			// Run the internal versions
			return IsIntervalIntersectingInternal( interval ) || interval.IsIntervalIntersectingInternal( this );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test in an interval's boundary points are included in this interval
		/// </summary>
		/// <param name="interval"> The interval to test for containment </param>
		/// <returns> Whether or not either endpoint of the passed-in interval is contained in this interval </returns>
		///////////////////////////////////////////////////////////////////////
		private bool IsIntervalIntersectingInternal( Interval interval )
		{
			return IsValueContained( interval.m_Min ) || IsValueContained( interval.m_Max );
		}

		
		// A simple swap function to make things simpler
		private static void swap( ref float rVal, ref float lVal )
		{
			float temp = rVal;
			rVal = lVal;
			lVal = temp;
		}		


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Write the interval out to a binary stream
		/// </summary>
		/// <param name="binWriter"> The binary writer to use for outputting </param>
		///////////////////////////////////////////////////////////////////////
		public void ToBinary( BinaryWriter binWriter )
		{
			binWriter.Write( m_Min );
			binWriter.Write( m_Max );
			binWriter.Write( m_IsMinClosed );
			binWriter.Write( m_IsMaxClosed );
			binWriter.Write( m_IsMinNegInfinite );
			binWriter.Write( m_IsMaxInfinite );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initalize the interval in from a binary stream
		/// </summary>
		/// <param name="binReader"> The binary reader to retrieve the data
		/// from </param>
		///////////////////////////////////////////////////////////////////////
		public void FromBinary( BinaryReader binReader )
		{
			m_Min = binReader.ReadSingle();
			m_Max = binReader.ReadSingle();
			m_IsMinClosed = binReader.ReadBoolean();
			m_IsMaxClosed = binReader.ReadBoolean();
			m_IsMinNegInfinite = binReader.ReadBoolean();
			m_IsMaxInfinite = binReader.ReadBoolean();
		}
	}


	//-------------------------------------------------------------------------
	/// <summary>
	/// This class is used for closed intervals.  It optimizes the functions
	/// for closed intervals since a most of the time that is all the
	/// functionality desired.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct IntervalClosed
	{
		/// <summary> The range of the interval </summary>
		private float m_Min, m_Max;

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A constructor for the closed interval class that sets the value.
		/// </summary>
		/// <param name="val1"> One of the boundary points </param>
		/// <param name="val2"> The other boundary point </param>
		///////////////////////////////////////////////////////////////////////
		public IntervalClosed( float val1, float val2 )
		{
			// Store the values
			m_Min = val1;
			m_Max = val2;
			Set( val1, val2 );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Create the interval with both boundary points closed.  The min and max will be sorted automatically.
		/// </summary>
		/// <param name="val1"> One of the boundary points </param>
		/// <param name="val2"> The other boundary point </param>
		/// <returns> The created closed interval </returns>
		///////////////////////////////////////////////////////////////////////
		public void Set( float val1, float val2 )
		{
			// Store the values
			m_Min = val1;
			m_Max = val2;

			// Ensure the values are in the right order
			if( m_Min > m_Max )
			{
				float temp = m_Min;
				m_Min = m_Max;
				m_Max = temp;
			}
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if a value is contained within the interval
		/// </summary>
		/// <param name="val"> The value to test for containment in the interval </param>
		/// <returns> Whether or not the value is contained </returns>
		///////////////////////////////////////////////////////////////////////
		public bool IsValueContained( float val )
		{
			// Test for containment
			if( val >= m_Min && val <= m_Max )
				return true;

			// The value is not contained if we got here
			return false;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if another interval intersects this interval
		/// </summary>
		/// <param name="interval"> The closed interval to test for intersection </param>
		/// <returns> Whether or not the interval is intersecting </returns>
		///////////////////////////////////////////////////////////////////////
		public bool IsIntervalIntersecting( IntervalClosed interval )
		{
			// Run the internal versions
			return IsIntervalIntersectingInternal( interval ) || interval.IsIntervalIntersectingInternal( this );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if another interval intersects this interval
		/// </summary>
		/// <param name="interval"> The interval to test for intersection </param>
		/// <returns> Whether or not the interval is intersecting </returns>
		///////////////////////////////////////////////////////////////////////
		public bool IsIntervalIntersecting( Interval interval )
		{
			// Test if either of our boundary points are contained
			if( interval.IsValueContained( m_Min )
				|| interval.IsValueContained( m_Max ) )
			{
				return true;
			}

			// Test if either of their endpoints are contained
			if( IsValueContained( interval.m_Min )
				|| IsValueContained( interval.m_Max ) )
				return true;

			// If we got here we are not contained
			return false;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test in an interval's boundary points are included in this interval
		/// </summary>
		/// <param name="interval"> The interval to test for containment </param>
		/// <returns> Whether or not either endpoint of the passed-in interval is contained in this interval </returns>
		///////////////////////////////////////////////////////////////////////
		private bool IsIntervalIntersectingInternal( IntervalClosed interval )
		{
			return IsValueContained( interval.m_Min ) || IsValueContained( interval.m_Max );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Write the interval out to a binary stream
		/// </summary>
		/// <param name="binWriter"> The binary writer to use for outputting </param>
		///////////////////////////////////////////////////////////////////////
		public void ToBinary( BinaryWriter binWriter )
		{
			binWriter.Write( m_Min );
			binWriter.Write( m_Max );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initalize the interval in from a binary stream
		/// </summary>
		/// <param name="binReader"> The binary reader to retrieve the data
		/// from </param>
		///////////////////////////////////////////////////////////////////////
		public void FromBinary( BinaryReader binReader )
		{
			m_Min = binReader.ReadSingle();
			m_Max = binReader.ReadSingle();
		}
	}
}
