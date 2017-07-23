using System;
using System.IO;
using System.Text;
using System.Drawing;


namespace PTWebApp
{
	public class PTCustomizeUtil
	{
		// Message lengths in characters for both the title screen message and custom picture
		// message
		public const int MSG_NUM_MSG_LEN = 64;
		public const int MSG_NUM_FRONT_PAD = 7;
		public const int MSG_NUM_END_PAD = 5;
		public const int MSG_TOTAL_LEN = MSG_NUM_MSG_LEN + MSG_NUM_FRONT_PAD + MSG_NUM_END_PAD;

		const int NUM_KEY_INDICES = 13;

		public long m_MsgOffset = 0;
		public FullKeyOffset m_PCKeyOffset = new FullKeyOffset();
		public long m_ExpirationDateOffset = 0;		
		public long m_CustomPicDataOffset = 0;
		public long m_CustomPicMsgOffset = 0;

		// Mac offsets
		public MacOffset m_MacMsgOffset = new MacOffset();		
		public FullKeyOffset m_MacPPCKey = new FullKeyOffset();
		public FullKeyOffset m_Macx86Key = new FullKeyOffset();
		public FullKeyOffset m_Macx64Key = new FullKeyOffset();
		public MacOffset m_MacPicDataOffset = new MacOffset();
		public MacOffset m_MacPicMsgOffset = new MacOffset();
		public MacOffset m_MacEvalExpirationDateOffset = new MacOffset();

		public int WcharSize = 2;

		public class FullKeyOffset
		{
			/// <summary>The byte offset within the EXE file of the key</summary>
			public long KeyOffset;

			/// <summary>The padding between 64-bit integers</summary>
			public int Inter64bitPadding = -1;

			/// <summary>The padding between the two DWORDs of the 64-bit integer</summary>
			public int Intra64bitPadding = -1;

			public override string ToString()
			{
				return KeyOffset.ToString() + "," + Inter64bitPadding + "," + Intra64bitPadding;
			}

			public void FromString(string inString)
			{
				string[] parts = inString.Split( ',' );
				if( parts.Length != 3 )
					throw new ApplicationException( "Invalid Mac key offset string: " + inString );

				KeyOffset = long.Parse( parts[0] );
				Inter64bitPadding = int.Parse( parts[1] );
				Intra64bitPadding = int.Parse( parts[2] );
			}
		}

		public class MacOffset
		{
			public long PPC;

			public long x86;

			public long x64;

			public override string ToString()
			{
				return PPC.ToString() + "," + x86 + "," + x64;
			}
			
			public void FromString(string inString)
			{
				string[] parts = inString.Split( ',' );
				if( parts.Length != 3 )
					throw new ApplicationException( "Invalid Mac offset string: " + inString );

				PPC = long.Parse( parts[0] );
				x86 = long.Parse( parts[1] );
				x64 = long.Parse( parts[2] );
			}
		}

		// wchar_t characters are written differently in the Mac and PC executables
		public enum CharacterByteOrder
		{
			/// <summary>
			/// PC
			/// </summary>
			BigEndian,

			/// <summary>
			/// Mac PPC
			/// </summary>
			LittleEndian
		}

		public CharacterByteOrder CharByteOrder = CharacterByteOrder.BigEndian;
		

		public const string Ini_KeyOffset = "Key offset";
		public const string Ini_MessageOffset = "Message offset";
		public const string Ini_ExpirationDate = "Expiration date offset";
		public const string Ini_CustomPicData = "Custom pic data offset";
		public const string Ini_CustomPicMsg = "Custom pic msg offset";

		public const string Ini_MacMsgOffsets = "Mac message offsets";
		public const string Ini_MacPPCKeyOffset = "Mac PPC key offset";
		public const string Ini_Macx86KeyOffset = "Mac x86 key offset";
		public const string Ini_Macx64KeyOffset = "Mac x64 key offset";
		public const string Ini_MacPicDataOffset = "Mac custom pic data offset";
		public const string Ini_MacPicMsgOffset = "Mac custom pic msg offset";
		public const string Ini_MacEvalExpirationDateOffset = "Mac eval expiration date offset";


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Initialize the internal offsets from an INI file</summary>
		/// <param name="infoFilePath">The INI file path</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void InitOffsetsFromFile(string infoFilePath)
		{
			FileStream exeFileStream = File.Open(infoFilePath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
			TextReader reader = new StreamReader( exeFileStream );

			// Get the first line of the file
			string curLine = reader.ReadLine();

			// Loop as long as there is text to read
			char[] splitChars = new char[]{':'};
			while (curLine != null)
			{
				// Split at the colon
				string[] lineVals = curLine.Split( splitChars, StringSplitOptions.RemoveEmptyEntries );
				if (lineVals.Length == 2 )
				{
					switch( lineVals[0])
					{
						case Ini_KeyOffset:
							m_PCKeyOffset.FromString(lineVals[1]);
							break;

						case Ini_MessageOffset:
							m_MsgOffset = long.Parse(lineVals[1]);
							break;

						case Ini_ExpirationDate:
							m_ExpirationDateOffset = long.Parse( lineVals[1] );
							break;

						case Ini_CustomPicData:
							m_CustomPicDataOffset = long.Parse( lineVals[1] );
							break;

						case Ini_CustomPicMsg:
							m_CustomPicMsgOffset = long.Parse( lineVals[1] );
							break;

						case Ini_MacMsgOffsets:
							m_MacMsgOffset.FromString( lineVals[1] );
							break;

						case Ini_MacPPCKeyOffset:
							m_MacPPCKey.FromString( lineVals[1] );
							break;

						case Ini_Macx86KeyOffset:
							m_Macx86Key.FromString( lineVals[1] );
							break;

						case Ini_Macx64KeyOffset:
							m_Macx64Key.FromString( lineVals[1] );
							break;

						case Ini_MacPicDataOffset:
							m_MacPicDataOffset.FromString( lineVals[1] );
							break;

						case Ini_MacPicMsgOffset:
							m_MacPicMsgOffset.FromString( lineVals[1] );
							break;

						case Ini_MacEvalExpirationDateOffset:
							m_MacEvalExpirationDateOffset.FromString( lineVals[1] );
							break;
					}
				}

				// Get the next line
				curLine = reader.ReadLine();	
			}

			exeFileStream.Close();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Get the key embedded within an executable</summary>
		/// <param name="exeFile">The executable file path</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public UInt64 GetKeyFromExe(string exeFile)
		{
			return GetKeyFromExe( exeFile, m_PCKeyOffset );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Get the key embedded within an executable</summary>
		/// <param name="exeFile">The executable file path</param>
		/// <param name="keyOffset">The offset within the EXE file of the game key</param>
		/// <param name="keyPadding">The number of bytes between the two double words that make up
		/// the key</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public UInt64 GetKeyFromExe(string exeFile, FullKeyOffset keyOffset )
		{
			FileStream exeFileStream = File.OpenRead(exeFile);
			BinaryReader reader = new BinaryReader(exeFileStream);

			// Get to the offset
			exeFileStream.Seek( keyOffset.KeyOffset, SeekOrigin.Begin );

			// Write the first 4 bytes
			byte[] valBytes = new byte[ 8 ];
			for (int byteIndex = 0; byteIndex < 4; ++byteIndex)
				valBytes[byteIndex] = reader.ReadByte();

			// Seek past the 4-byte block seperating a 64-bit value within a file
			exeFileStream.Seek( keyOffset.Intra64bitPadding, SeekOrigin.Current );

			// Write the last 4 bytes
			for (int byteIndex = 4; byteIndex < 8; ++byteIndex)
				valBytes[byteIndex] = reader.ReadByte();

			// Close the file stream
			exeFileStream.Close();

			// Return the 64-bit key formed from its components
			return ArrayToUInt64( valBytes );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Overwrite the key value in the exe file using the offset and padding value loaded from
		/// file
		/// </summary>
		/// <param name="exePath">The executable path</param>
		/// <param name="value">The key value</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void OverwriteKeyInEXE(string exePath, UInt64 value)
		{
			OverwriteValueInEXE( exePath, value, m_PCKeyOffset );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Overwrite the key value in the Mac application file using the offset and
		/// padding value loaded from file</summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void OverwriteMacKey(string exePath, UInt64 keyValue)
		{
			try
			{
				CharByteOrder = CharacterByteOrder.LittleEndian;
				OverwriteValueInEXE( exePath, keyValue, m_MacPPCKey );

				CharByteOrder = CharacterByteOrder.BigEndian;
				OverwriteValueInEXE( exePath, keyValue, m_Macx86Key );
				OverwriteValueInEXE( exePath, keyValue, m_Macx64Key );
			}
			finally
			{
				CharByteOrder = CharacterByteOrder.BigEndian;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Overwrite the key value stored within the Prime Time executable file
		/// </summary>
		/// <param name="exePath">The path to the executable</param>
		/// <param name="value">The key value to use</param>
		/// <param name="keyOffset">The offset within the file to the key value and the offset
		/// between the two DWORDs that make up the 64-bit integer</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void OverwriteValueInEXE(string exePath, UInt64 value, FullKeyOffset keyOffset )
		{
			FileStream exeFileStream = File.OpenWrite( exePath );
			BinaryWriter writer = new BinaryWriter( exeFileStream );

			// Get to the offset
			exeFileStream.Seek( keyOffset.KeyOffset, SeekOrigin.Begin );

			// Break the value into its components
			byte[] valBytes = UInt64ToArray( value, CharByteOrder );

			// Write the first 4 bytes
			for( int byteIndex = 0; byteIndex < 4; ++byteIndex )
				writer.Write( valBytes[byteIndex]);

			// Seek past the first 4-byte block seperating a 64-bit integer
			exeFileStream.Seek( keyOffset.Intra64bitPadding, SeekOrigin.Current );

			// Write the last 4 bytes
			for (int byteIndex = 4; byteIndex < 8; ++byteIndex)
				writer.Write(valBytes[byteIndex]);

			exeFileStream.Close();
		}


		public static byte[] UInt64ToArray(UInt64 val)
		{
			return UInt64ToArray( val, CharacterByteOrder.BigEndian );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Break a 64-bit integer into an 8 byte array. Array index 0 of the returned array is the
		/// LSB.
		/// </summary>
		/// <param name="val">The 64-bit integer to break into its component bytes</param>
		/// <returns>An 8 byte array representing the 64-bit value with the LSB at index 0</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static byte[] UInt64ToArray(UInt64 val, CharacterByteOrder byteOrder)
		{
			byte[] retVals = new byte[8];

			// Store the values
			for( int byteIndex = 0; byteIndex < 8; ++byteIndex )
			{
				if( byteOrder == CharacterByteOrder.BigEndian )
					retVals[byteIndex] = (byte)( ( val >> ( byteIndex * 8 ) ) & 0xFF );
				else
					retVals[7 - byteIndex] = (byte)( ( val >> ( byteIndex * 8 ) ) & 0xFF );
			}

			return retVals;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Combine 8-bytes into a 64-bit integer</summary>
		/// <param name="vals">The 8 byte array representing the 64-bit value with the LSB at
		/// index 0</param>
		/// <returns>A 64-bit integer formed from the 8-bytes passed in</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private static UInt64 ArrayToUInt64(byte[] vals)
		{
			UInt64 retVal = 0;

			// Store the values
			for (int byteIndex = 0; byteIndex < 8; ++byteIndex)
				retVal |= (UInt64)vals[byteIndex] << ( byteIndex * 8 );

			return retVal;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Convert the key components into one 64-bit value</summary>
		/// <param name="vals">The key components</param>
		/// <returns>The created key</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private static UInt64 MakeKeyVal(uint[] vals)
		{
			if (vals.Length != NUM_KEY_INDICES)
				throw new ArgumentException( "Invalid number of values passed to MakeKeyVal" );

			UInt64 retVal = 0;

			int bitIndex = 0;
			for( int valIndex = 0; valIndex < NUM_KEY_INDICES; ++valIndex )
			{
				// If this is the last value, then clamp the range to 16
				if( valIndex == NUM_KEY_INDICES - 1 )
				{
					retVal |= (UInt64)( vals[valIndex] % 16 ) << bitIndex;
				}
				else
				{
					// OR in the value
					retVal |= (UInt64)vals[valIndex] << bitIndex;
				}

				// There are 5 bits per key value
				bitIndex += 5;
			}

			return retVal;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Create a key value from the 7 seed components</summary>
		/// <param name="fixedVals">The 7 seed components. If this array does not contain 7
		/// compents then an exception is raised.</param>
		/// <returns>The generated 64-bit key</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public UInt64 MakeKey(uint[] fixedVals)
		{
			const int NUM_FIXED = 7;
			if (fixedVals.Length != NUM_FIXED)
				throw new ArgumentException( "7 values must be passed to MakeKey" );

			// The fixed value indices
			int[] FIXED_IDX = new int[NUM_FIXED] { 0, 2, 3, 7, 10, 11, 12 };

			// Store the fixed indices in the return array
			uint[] vals = new uint[NUM_KEY_INDICES]; 
			for (int fixedIndex = 0; fixedIndex < NUM_FIXED; ++fixedIndex)
			{
				int valIndex = FIXED_IDX[fixedIndex];
				vals[ valIndex ] = fixedVals[ fixedIndex ];
				if( fixedIndex < NUM_FIXED - 1 )
				{
					if (vals[valIndex] > 31)
						throw new ArgumentException( "Fixed value cannot be greater than 31" );
				}
				else
				{
					if (vals[valIndex] > 15)
						throw new ArgumentException("Fixed value cannot be greater than 15");
				}
			}

			// Value 1
			if (vals[FIXED_IDX[5]] > 15)
				vals[1] = vals[FIXED_IDX[3]] * 2;
			else
				vals[1] = vals[FIXED_IDX[6]] * 2;

			// Value 4
			vals[4] = vals[FIXED_IDX[0]] + vals[FIXED_IDX[3]];

			// Value 5
			vals[5] = (vals[FIXED_IDX[6]] % 8) + vals[FIXED_IDX[1]];

			// Value 6
			vals[6] = ~vals[FIXED_IDX[2]];

			// Value 8
			if (vals[FIXED_IDX[5]] % 4 == 0)
				vals[8] = vals[FIXED_IDX[2]] + vals[FIXED_IDX[6]];
			else if (vals[FIXED_IDX[5]] % 3 == 0)
				vals[8] = vals[FIXED_IDX[1]] + vals[FIXED_IDX[0]];
			else
				vals[8] = vals[FIXED_IDX[3]] | 0x15;

			// Value 9
			vals[9] = (uint)Math.Pow((vals[FIXED_IDX[0]] % 5), ((vals[FIXED_IDX[1]] + vals[FIXED_IDX[2]]) % 8));

			// Clamp values
			for (int valIndex = 0; valIndex < NUM_KEY_INDICES; ++valIndex)
				vals[valIndex] = vals[valIndex] % 32;

			return MakeKeyVal( vals );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Overwrite the title screen message using the message offset loaded from
		/// file</summary>
		/// <param name="exePath">The path to the EXE file to modify</param>
		/// <param name="msg">The new message to display</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void OverwriteMsg(string exePath, string msg)
		{
			OverwriteMsg( exePath, m_MsgOffset, msg );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Overwrite the small message under the custom picture using the message offset
		/// loaded from file</summary>
		/// <param name="exePath">The path to the EXE file to modify</param>
		/// <param name="msg">The new message to display</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void OverwriteCustomPicMsg(string exePath, string msg)
		{
			OverwriteMsg( exePath, m_CustomPicMsgOffset, msg );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Overwrite the small message under the custom picture using the message offset
		/// loaded from file</summary>
		/// <param name="exePath">The path to the EXE file to modify</param>
		/// <param name="msg">The new message to display</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void OverwriteMacCustomPicMsg(string exePath, string msg)
		{
			try
			{
				WcharSize = 4;
				CharByteOrder = CharacterByteOrder.LittleEndian;

				OverwriteMsg( exePath, m_MacPicMsgOffset.PPC, msg );

				CharByteOrder = CharacterByteOrder.BigEndian;

				OverwriteMsg( exePath, m_MacPicMsgOffset.x86, msg );

				OverwriteMsg( exePath, m_MacPicMsgOffset.x64, msg );
			}
			finally
			{
				WcharSize = 2;
				CharByteOrder = CharacterByteOrder.BigEndian;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Overwrite the expiration date for evaluation copies</summary>
		/// <param name="exePath">The path to the EXE file to modify</param>
		/// <param name="expirationDate">The date the game stops working</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void SetExpirationDate(string exePath, DateTime expirationDate)
		{
			FileStream exeFileStream = File.OpenWrite( exePath );
			BinaryWriter writer = new BinaryWriter( exeFileStream );

			// Seek to the character data past the 7 prefix charcters
			exeFileStream.Seek( m_ExpirationDateOffset, SeekOrigin.Begin );

			SetExpirationDate( writer, expirationDate );

			exeFileStream.Close();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Overwrite the expiration date for evaluation copies</summary>
		/// <param name="exePath">The path to the EXE file to modify</param>
		/// <param name="expirationDate">The date the game stops working</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void SetMacExpirationDate(string exePath, DateTime expirationDate)
		{
			FileStream exeFileStream = File.OpenWrite( exePath );
			BinaryWriter writer = new BinaryWriter( exeFileStream );

			try
			{
				CharByteOrder = CharacterByteOrder.LittleEndian;
				exeFileStream.Seek( m_MacEvalExpirationDateOffset.PPC, SeekOrigin.Begin );
				SetExpirationDate( writer, expirationDate );

				CharByteOrder = CharacterByteOrder.BigEndian;
				exeFileStream.Seek( m_MacEvalExpirationDateOffset.x86, SeekOrigin.Begin );
				SetExpirationDate( writer, expirationDate );

				exeFileStream.Seek( m_MacEvalExpirationDateOffset.x64, SeekOrigin.Begin );
				SetExpirationDate( writer, expirationDate );
			}
			finally
			{
				CharByteOrder = CharacterByteOrder.BigEndian;
			}

			exeFileStream.Close();
		}


		private void SetExpirationDate(BinaryWriter writer, DateTime expirationDate)
		{
			if( CharByteOrder == CharacterByteOrder.BigEndian )
			{
				writer.Write( (ushort)( expirationDate.Year - 2010 ) );
				writer.Write( (ushort)expirationDate.Month );
				writer.Write( (ushort)expirationDate.Day );
			}
			else
			{
				byte[] dateData = new byte[sizeof( ushort ) * 3];
				int dataDataByteIndex = 0;

				ushort curValue = (ushort)(expirationDate.Year - 2010);
				dateData[dataDataByteIndex++] = (byte)( ( curValue >> 8 ) & 0xFF );
				dateData[dataDataByteIndex++] = (byte)( curValue & 0xFF );

				curValue = (ushort)expirationDate.Month;
				dateData[dataDataByteIndex++] = (byte)( ( curValue >> 8 ) & 0xFF );
				dateData[dataDataByteIndex++] = (byte)( curValue & 0xFF );

				curValue = (ushort)expirationDate.Day;
				dateData[dataDataByteIndex++] = (byte)( ( curValue >> 8 ) & 0xFF );
				dateData[dataDataByteIndex++] = (byte)( curValue & 0xFF );

				writer.Write( dateData );
			}
		}


		public void OverwriteMacMsg(string exePath, string msg)
		{
			OverwriteMacMsg( exePath, m_MacMsgOffset.PPC, m_MacMsgOffset.x86, m_MacMsgOffset.x64, msg );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Overwrite the title screen or custom picture message for a Mac application</summary>
		/// <param name="exePath">The path to the EXE file to modify</param>
		/// <param name="msgDataOffset">The offset within the EXE file of the message data </param>
		/// <param name="msg">The new message to display</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void OverwriteMacMsg(string exePath, long ppcOffset, long x86Offset, long x64Offset, string msg)
		{
			WcharSize = 4;
			CharByteOrder = CharacterByteOrder.LittleEndian;
			OverwriteMsg( exePath, ppcOffset, msg );

			CharByteOrder = CharacterByteOrder.BigEndian;
			OverwriteMsg( exePath, x86Offset, msg );
			OverwriteMsg( exePath, x64Offset, msg );

			WcharSize = 2;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Overwrite the title screen or custom picture message</summary>
		/// <param name="exePath">The path to the EXE file to modify</param>
		/// <param name="msgDataOffset">The offset within the EXE file of the message data </param>
		/// <param name="msg">The new message to display</param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void OverwriteMsg(string exePath, long msgDataOffset, string msg)
		{
			// If the message is too big
			if (msg.Length > MSG_NUM_MSG_LEN)
				throw new ArgumentOutOfRangeException( "Message is too long" );

			FileStream exeFileStream = File.OpenWrite(exePath);
			BinaryWriter writer = new BinaryWriter(exeFileStream);

			// Break the value into its components
			byte[] charData = new byte[msg.Length * WcharSize];
			int curCharDataIndex = 0;
			for( int charIndex = 0; charIndex < msg.Length; ++charIndex )
			{
				// Times 3 is for encryption
				int curMsgCharValue = (int)msg[charIndex] * 3;

				if( WcharSize == 4 )
				{
					charData[curCharDataIndex++] = 0;
					charData[curCharDataIndex++] = 0;
				}

				if( CharByteOrder == PTCustomizeUtil.CharacterByteOrder.BigEndian )
				{
					charData[curCharDataIndex++] = (byte)( curMsgCharValue & 0xFF );
					charData[curCharDataIndex++] = (byte)( ( curMsgCharValue >> 8 ) & 0xFF );
				}
				else
				{
					charData[curCharDataIndex++] = (byte)( ( curMsgCharValue >> 8 ) & 0xFF );
					charData[curCharDataIndex++] = (byte)( curMsgCharValue & 0xFF );
				}
			}

			// Seek to the character data past the 7 prefix charcters
			exeFileStream.Seek( msgDataOffset + ( MSG_NUM_FRONT_PAD * WcharSize ), SeekOrigin.Begin );

			// Write the size
			if( WcharSize == 4 )
			{
				writer.Write( (byte)0 );
				writer.Write( (byte)0 );
			}
			if( CharByteOrder == PTCustomizeUtil.CharacterByteOrder.BigEndian )
			{
				writer.Write( (byte)msg.Length );
				writer.Write( (byte)0 );				
			}
			else
			{
				writer.Write( (byte)0 );
				writer.Write( (byte)msg.Length );
			}
			

			// Write the message
			writer.Write( charData );

			exeFileStream.Close();
		}


		/// <summary>Embed an image into a Prime Time exe</summary>
		/// <param name="ptExePath"></param>
		/// <param name="image"></param>
		/// <returns></returns>
		public void EmbedCustomPic(string ptExePath, Bitmap image)
		{
			EmbedCustomPic( ptExePath, m_CustomPicDataOffset, image );
		}


		/// <summary>Embed an image into a Prime Time exe</summary>
		/// <param name="ptExePath"></param>
		/// <param name="image"></param>
		/// <returns></returns>
		public void EmbedCustomPic(string ptExePath, long imageDataOffset, Bitmap image)
		{
			if( image.Width != 128 || image.Height != 128 )
				throw new ApplicationException( "Embedded image must be 128x128" );

			FileStream exeFileStream = File.OpenWrite( ptExePath );
			BinaryWriter writer = new BinaryWriter( exeFileStream );

			// Seek to the image data
			writer.Seek( (int)imageDataOffset, SeekOrigin.Begin );

			// Write the inital data

			// Enable the image
			writer.Write( (byte)1 );

			// Enable the width and height
			writer.Write( (int)128 );
			writer.Write( (int)128 );

			// Write that it's a bitmap (1 = JPEG, 2 = RLE Bitmap)
			writer.Write( (int)0 );

			WritePixelData( writer, image );

			exeFileStream.Close();
		}

		public void EmbedMacCustomPic(string ptExePath, Bitmap image)
		{
			EmbedCustomPic( ptExePath, m_MacPicDataOffset.PPC, image );
			EmbedCustomPic( ptExePath, m_MacPicDataOffset.x86, image );
			EmbedCustomPic( ptExePath, m_MacPicDataOffset.x64, image );
		}


		private static void WritePixelData(BinaryWriter writer, Bitmap image)
		{
			for( int y = 0; y < image.Height; ++y )
			{
				for( int x = 0; x < image.Width; ++x )
				{
					Color curPixelColor = image.GetPixel( x, y );
					writer.Write( (byte)curPixelColor.R );
					writer.Write( (byte)curPixelColor.G );
					writer.Write( (byte)curPixelColor.B );
				}
			}
		}
	}
}
