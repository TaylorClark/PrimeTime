using System.IO;

namespace TCUtil
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A class to abstract the specifics of reading and writing binary data
	/// to/from a stream
	/// </summary>
	//-------------------------------------------------------------------------
	public class SimpleSerializer : ISimpleSerializer
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
		public SimpleSerializer( BinaryWriter binWriter )
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
		public SimpleSerializer( BinaryReader binReader )
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
		public void AddData( ref int val )
		{
			// If we are writing
			if( m_BinWriter != null )
				m_BinWriter.Write( val );
			// Else if we are reading
			else if( m_BinReader != null )
				val = m_BinReader.ReadInt32();
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
			if (m_BinWriter != null)
				m_BinWriter.Write(val);
			// Else if we are reading
			else if (m_BinReader != null)
				val = m_BinReader.ReadUInt32();
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a floating point for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The float to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData( ref float val )
		{
			// If we are writing
			if( m_BinWriter != null )
				m_BinWriter.Write( val );
			// Else if we are reading
			else if( m_BinReader != null )
				val = m_BinReader.ReadSingle();
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a boolean for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The boolean to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData( ref bool val )
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
		public void AddData( ref string val )
		{
			// If we are writing
			if( m_BinWriter != null )
				m_BinWriter.Write( val );
				// Else if we are reading
			else if( m_BinReader != null )
				val = m_BinReader.ReadString();
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a character for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The character to use </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData( ref char val )
		{
			// If we are writing
			if( m_BinWriter != null )
				m_BinWriter.Write( val );
			// Else if we are reading
			else if( m_BinReader != null )
				val = m_BinReader.ReadChar();
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add characters for reading from or writing to the stream
		/// </summary>
		/// <param name="val"> The character to use </param>
		/// <param name="len"> The number of characters to write </param>
		///////////////////////////////////////////////////////////////////////
		public void AddData( ref char[] val )
		{
			// If we are writing
			if (m_BinWriter != null)
				m_BinWriter.Write( val, 0, val.Length );
			// Else if we are reading
			else if (m_BinReader != null)
				val = m_BinReader.ReadChars( val.Length );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get if the serializer is being used for writing data out
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public bool InWriteMode
		{
			get{ return m_BinWriter != null; }
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get if the serializer is being used for reading data in
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public bool InReadMode
		{
			get{ return m_BinReader != null; }
		}
	}
}