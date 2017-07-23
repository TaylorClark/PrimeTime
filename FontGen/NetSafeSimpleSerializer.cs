using System.IO;
using System.Net;
using System;

namespace TCUtil
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A class to abstract the specifics of reading and writing binary data
	/// to/from a stream
	/// </summary>
	//-------------------------------------------------------------------------
	public class NetSafeSimpleSerializer : ISimpleSerializer
	{
		/// <summary> The stream we are using to write out </summary>
		private readonly BinaryWriter m_BinWriter = null;

		/// <summary> The stream we are using to read data in </summary>
		private readonly BinaryReader m_BinReader = null;


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The constructor to create a serializer for writing data out
		/// </summary>
		/// <param name="binWriter"> The binary writing stream to use for
		/// writing data out </param>
		///////////////////////////////////////////////////////////////////////
		public NetSafeSimpleSerializer(BinaryWriter binWriter)
		{
			// Store the writer
			m_BinWriter = binWriter;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The constructor to create a serializer for reading data in
		/// </summary>
		/// <param name="binReader"> The binary stream for reading data in </param>
		///////////////////////////////////////////////////////////////////////
		public NetSafeSimpleSerializer(BinaryReader binReader)
		{
			// Store the reader
			m_BinReader = binReader;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a 32-bit integer for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The integer to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData(ref int val)
		{
			// If we are writing
			if( m_BinWriter != null )
				m_BinWriter.Write( IPAddress.HostToNetworkOrder( val ) );
			// Else if we are reading
			else if( m_BinReader != null )
				val = IPAddress.NetworkToHostOrder( m_BinReader.ReadInt32() );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Add a 32-bit integer for reading from or writing to the stream</summary>
		/// <param name="val"> The integer to use </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void AddData(ref short val)
		{
			// If we are writing
			if( m_BinWriter != null )
				m_BinWriter.Write( IPAddress.HostToNetworkOrder( val ) );
			// Else if we are reading
			else if( m_BinReader != null )
				val = IPAddress.NetworkToHostOrder( m_BinReader.ReadInt16() );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a 32-bit unsigned integer for reading from or writing to the
		/// stream
		/// </summary>
		/// <param name="val"> The unsigned integer to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData(ref uint val)
		{
			// If we are writing
			if( m_BinWriter != null )
			{
				byte[] intBytes = BitConverter.GetBytes( val );

				m_BinWriter.Write( IPAddress.HostToNetworkOrder( BitConverter.ToInt32( intBytes, 0 ) ) );
			}
			// Else if we are reading
			else if( m_BinReader != null )
			{
				int tempVal = IPAddress.NetworkToHostOrder( m_BinReader.ReadInt32() );
				byte[] intBytes = BitConverter.GetBytes( tempVal );
				val = BitConverter.ToUInt32( intBytes, 0 );
			}
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a floating point for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The float to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData(ref float val)
		{
			// If we are writing
			if( m_BinWriter != null )
			{
				byte[] floatBytes = BitConverter.GetBytes( val );
				object tempVal = val;
				m_BinWriter.Write( IPAddress.HostToNetworkOrder( (int)tempVal ) );
			}
			// Else if we are reading
			else if( m_BinReader != null )
			{
				int tempInt = IPAddress.NetworkToHostOrder( m_BinReader.ReadInt32() );
				object tempVal = tempInt;
				val = (float)tempVal;
			}
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a boolean for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The boolean to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData(ref bool val)
		{
			// If we are writing
			if( m_BinWriter != null )
				m_BinWriter.Write( val );
			// Else if we are reading
			else if( m_BinReader != null )
				val = m_BinReader.ReadBoolean();
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a string for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The string to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData(ref string val)
		{
			int numChars = val.Length;
			AddData( ref numChars );

			// If we are writing
			if( m_BinWriter != null )
			{
				char[] stringChars = val.ToCharArray();
				AddData( ref stringChars );
			}
			// Else if we are reading
			else if( m_BinReader != null )
			{
				char[] stringChars = new char[val.Length];
				AddData( ref stringChars );
				val = new string( stringChars );
			}
		}


		private static short CharToShort(char val)
		{
			byte[] charBytes = BitConverter.GetBytes( val );

			return BitConverter.ToInt16( charBytes, 0 );
		}

		private static short[] CharToShort(char[] val)
		{
			short[] retShorts = new short[val.Length];

			for( int charIndex = 0; charIndex < val.Length; ++charIndex )
				retShorts[charIndex] = CharToShort( val[charIndex] );

			return retShorts;
		}

		private static char ShortToChar(short val)
		{
			byte[] shortBytes = BitConverter.GetBytes( val );

			return BitConverter.ToChar( shortBytes, 0 );
		}

		private static char[] ShortToChar(short[] val)
		{
			char[] retChars = new char[val.Length];

			for( int charIndex = 0; charIndex < val.Length; ++charIndex )
				retChars[charIndex] = ShortToChar( val[charIndex] );

			return retChars;
		}

		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a character for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The character to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData(ref char val)
		{
			// If we are writing
			if( m_BinWriter != null )
				m_BinWriter.Write( IPAddress.HostToNetworkOrder( (short)CharToShort( val ) ) );
			// Else if we are reading
			else if( m_BinReader != null )
				val = ShortToChar( IPAddress.NetworkToHostOrder( m_BinReader.ReadInt16() ) );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Add characters for reading from or writing to the stream</summary>
		/// <param name="val"> The character to use </param>
		/// <param name="len"> The number of characters to write </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void AddData(ref char[] val)
		{
			short[] shortVals = new short[val.Length];
			for( int valueIndex = 0; valueIndex < val.Length; ++valueIndex )
				shortVals[valueIndex] = CharToShort( val[valueIndex] );

			AddData( ref shortVals );

			for( int valueIndex = 0; valueIndex < val.Length; ++valueIndex )
				val[valueIndex] = ShortToChar( shortVals[valueIndex] );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Add characters for reading from or writing to the stream</summary>
		/// <param name="val"> The character to use </param>
		/// <param name="len"> The number of characters to write </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void AddData(ref short[] val)
		{
			for( int valueIndex = 0; valueIndex < val.Length; ++valueIndex )
				AddData( ref val[valueIndex] );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get if the serializer is being used for writing data out
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public bool InWriteMode
		{
			get { return m_BinWriter != null; }
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get if the serializer is being used for reading data in
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public bool InReadMode
		{
			get { return m_BinReader != null; }
		}
	}
}