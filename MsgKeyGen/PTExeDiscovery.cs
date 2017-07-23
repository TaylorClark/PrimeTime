using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace PTWebApp
{
	public static class PTExeDiscovery
	{
		private static readonly UInt64[] GAME_FIXED_KEYS = new UInt64[] { 0xffff33a45bcc8009, 0x0000000100000002, 0xaadc0a081dffb05e, 0x0101020103010002 };
		private static readonly byte[] GAME_CUSTOM_PIC_KEYS = new byte[] { 0x00, 0xBB, 0xEE, 0x66, 0xBB, 0xCC, 0x01, 0x45, 0xFB, 0x55, 0x55, 0x55, 0x55, 0x55 };
		private static readonly byte[] GAME_CUSTOM_PIC_END_KEYS = new byte[] { 0xEE, 0x00, 0xFF, 0x55 };

		private static readonly ushort[] MSG_PREFIX = new ushort[] { 0xFFFF, 0x000F, 0x9AB5, 0xD431 };
		private static readonly ushort[] CUSTOM_PIC_MSG_PREFIX = new ushort[] { 0xFFFF, 0x0F0F, 0x88BF, 0xD431 };
		private static readonly ushort[] EVAL_EXPIRATION_DATE_PREFIX = new ushort[] { 0x6543, 0x5432, 0x5321, 0x3210 };

		/// <summary>The number of bytes in the custom pic data. Image Data( 4 + 4 + 4 + (128 * 128 * 3) ),
		/// start key (14), and end key (4).</summary>
		private const int CUSTOM_PIC_DATA_LEN = 49183;

		// The index in GAME_FIXED_KEYS that is the key
		private const int ACTUAL_KEY_INDEX = 2;
		
		public static PTCustomizeUtil.CharacterByteOrder CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;
		public static int WcharSize = 2;


		public static void GenerateExeInfoFile(string exePath, string outFile)
		{
			PTCustomizeUtil.FullKeyOffset keyOffset = PTExeDiscovery.FindKeyOffset( exePath );
			long msgOffset = PTExeDiscovery.FindMsgOffset( exePath );
			long expirationDateOffset = PTExeDiscovery.FindEvalExpirationDateOffset( exePath );
			long customPicDataOffset = PTExeDiscovery.FindCustomPicOffset( exePath );
			long customPicMsgOffset = PTExeDiscovery.FindCustomPicMsgOffset( exePath );

			// Output the data
			StreamWriter sw = new StreamWriter( outFile );
			sw.WriteLine( PTCustomizeUtil.Ini_KeyOffset + ":" + keyOffset.ToString() );
			sw.WriteLine( PTCustomizeUtil.Ini_MessageOffset + ":" + msgOffset.ToString() );

			if( expirationDateOffset > 0 )
				sw.WriteLine( PTCustomizeUtil.Ini_ExpirationDate + ":" + expirationDateOffset.ToString() );

			sw.WriteLine( PTCustomizeUtil.Ini_CustomPicData + ":" + customPicDataOffset.ToString() );
			sw.WriteLine( PTCustomizeUtil.Ini_CustomPicMsg + ":" + customPicMsgOffset.ToString() );

			sw.Close();
		}


		public static void GenerateMacExeInfoFile(string exePath, string outFile)
		{
			PTCustomizeUtil.FullKeyOffset ppcOffset = PTExeDiscovery.FindMacPPCKeyOffset( exePath );
			PTCustomizeUtil.FullKeyOffset x86Offset = PTExeDiscovery.FindMacx86KeyOffset( exePath );
			PTCustomizeUtil.FullKeyOffset x64Offset = PTExeDiscovery.FindMacx64KeyOffset( exePath );

			PTCustomizeUtil.MacOffset msgOffsets = PTExeDiscovery.FindMacMsgOffsets( exePath );

			PTCustomizeUtil.MacOffset expirationDateOffset = PTExeDiscovery.FindMacEvalExpirationDateOffset( exePath );
			PTCustomizeUtil.MacOffset customPicDataOffset = PTExeDiscovery.FindMacCustomPicDataOffsets( exePath );
			PTCustomizeUtil.MacOffset customPicMsgOffset = PTExeDiscovery.FindMacCustomPicMsgOffset( exePath );

			// Output the data
			StreamWriter sw = new StreamWriter( outFile );
			sw.WriteLine( PTCustomizeUtil.Ini_MacPPCKeyOffset + ":" + ppcOffset.ToString() );
			sw.WriteLine( PTCustomizeUtil.Ini_Macx86KeyOffset + ":" + x86Offset.ToString() );
			sw.WriteLine( PTCustomizeUtil.Ini_Macx64KeyOffset + ":" + x64Offset.ToString() );

			sw.WriteLine( PTCustomizeUtil.Ini_MacMsgOffsets + ":" + msgOffsets.ToString() );

			sw.WriteLine( PTCustomizeUtil.Ini_MacEvalExpirationDateOffset + ":" + expirationDateOffset.ToString() );

			sw.WriteLine( PTCustomizeUtil.Ini_MacPicDataOffset + ":" + customPicDataOffset.ToString() );
			sw.WriteLine( PTCustomizeUtil.Ini_MacPicMsgOffset + ":" + customPicMsgOffset.ToString() );

			sw.Close();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the message prefix when generating the message for the CPP files
		/// </summary>
		/// <returns>A string containing the prefix shorts seperated with commas</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static string GetHeadMsgPrefix()
		{
			StringBuilder retStr = new StringBuilder();
			for( int charIndex = 0; charIndex < MSG_PREFIX.Length; ++charIndex )
			{
				ushort curChar = MSG_PREFIX[charIndex];
				retStr.Append( curChar.ToString() );
				retStr.Append( ", " );
			}

			return retStr.ToString();
		}


		//public string EncodeOrderString( ulong key, string message )
		//{
		//    StringBuilder retStr = new StringBuilder();	

		//    // Add the key
		//    retStr.Append( key.ToString() );

		//    // Add the message
		//    retStr.Append( '\x1' );
		//    if( !String.IsNullOrEmpty(message))
		//        retStr.Append( message );

		//    // Add the current time in binary format
		//    retStr.Append( '\x1' );
		//    retStr.Append( DateTime.Now.ToUniversalTime().ToBinary().ToString() );

		//    // Return the string
		//    return retStr.ToString();
		//}

		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Decode an order string into the order information
		/// </summary>
		/// <param name="orderString">The string sent from the Prime Time store</param>
		/// <returns>The order info object or null on an invalid string</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		//public CustomOrderInfo DecodeOrderString(string orderString)
		//{
		//    // Break the string into its components
		//    string[] orderStrComps = orderString.Split( new char[] {'\x1'} );
		//    if (orderStrComps.Length != 3)
		//        return null;

		//    // Get the key
		//    ulong key = 0;
		//    if( !ulong.TryParse( orderStrComps[0], out key ) )
		//        return null;

		//    // Get the title screen message
		//    string titleMsg = orderStrComps[ 1 ];

		//    // Get the time of the code generation
		//    long fileTimeVal = 0;
		//    if (!long.TryParse(orderStrComps[2], out fileTimeVal))
		//        return null;
		//    if( fileTimeVal < 0 || fileTimeVal > DateTime.MaxValue.ToUniversalTime().ToBinary() )
		//        return null;
		//    DateTime codeGenTime = DateTime.FromBinary(fileTimeVal);

		//    // Create and return the order information
		//    CustomOrderInfo retVal = new CustomOrderInfo();
		//    retVal.key = key;
		//    retVal.orderTime = codeGenTime;
		//    retVal.titleMsg = titleMsg;
		//    return retVal;
		//}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find a the offset within the EXE file of the message characters</summary>
		/// <param name="exePath">The path of the Prime Time executable</param>
		/// <returns>The offset in bytes of the message data or 0 if the offset could not be
		/// found </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static long FindMsgOffset(string exePath)
		{
			return FindMsgDataOffset( exePath, MSG_PREFIX, 0 );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find a the offset within the EXE file of the message characters</summary>
		/// <param name="exePath">The path of the Prime Time executable</param>
		/// <returns>The offset in bytes of the message data or 0 if the offset could not be
		/// found </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.MacOffset FindMacMsgOffsets(string exePath)
		{
			PTCustomizeUtil.MacOffset retOffsets = new PTCustomizeUtil.MacOffset();

			try
			{
				PTExeDiscovery.WcharSize = 4;
				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.LittleEndian;

				retOffsets.PPC = FindMsgDataOffset( exePath, MSG_PREFIX, 0 );

				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;

				retOffsets.x86 = FindMsgDataOffset( exePath, MSG_PREFIX, 0 );

				retOffsets.x64 = FindMsgDataOffset( exePath, MSG_PREFIX, retOffsets.x86 + 10 );
			}
			finally
			{
				// Back to PC values
				PTExeDiscovery.WcharSize = 2;
				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;
			}

			return retOffsets;
		}


		public static long FindMsgOffset(string exePath, long startOffset)
		{
			return FindMsgDataOffset( exePath, MSG_PREFIX, startOffset );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find a the offset within the EXE file of the message characters</summary>
		/// <param name="exePath">The path of the Prime Time executable</param>
		/// <returns>The offset in bytes of the message data or 0 if the offset could not be
		/// found </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static long FindCustomPicMsgOffset(string exePath)
		{
			return FindMsgDataOffset( exePath, CUSTOM_PIC_MSG_PREFIX, 0 );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the custom pic
		/// data.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.MacOffset FindMacCustomPicMsgOffset(string exePath)
		{
			PTCustomizeUtil.MacOffset retOffset = new PTCustomizeUtil.MacOffset();

			try
			{
				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.LittleEndian;
				PTExeDiscovery.WcharSize = 4;

				retOffset.PPC = FindMsgDataOffset( exePath, CUSTOM_PIC_MSG_PREFIX, 0 );

				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;
				retOffset.x86 = FindMsgDataOffset( exePath, CUSTOM_PIC_MSG_PREFIX, 0 );

				retOffset.x64 = FindMsgDataOffset( exePath, CUSTOM_PIC_MSG_PREFIX, retOffset.x86 + 10 );
			}
			finally
			{
				// Go back to PC values
				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;
				PTExeDiscovery.WcharSize = 2;
			}

			return retOffset;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find a the offset within the evaluation EXE file of the expiration date year.
		/// The year is 2 bytes, followed by the month, then day, all 2 bytes.</summary>
		/// <param name="exePath">The path of the Prime Time executable</param>
		/// <returns>The offset in bytes of the message data or 0 if the offset could not be
		/// found </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static long FindEvalExpirationDateOffset(string exePath)
		{
			long expirationDateOffset = FindMsgDataOffset( exePath, EVAL_EXPIRATION_DATE_PREFIX, 0 );

			// If we found the offset, then add 8 (4 wchars) to step passed the data prefix
			if( expirationDateOffset > 0 )
				expirationDateOffset += 8;

			return expirationDateOffset;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find a the offset within the evaluation EXE file of the expiration date year.
		/// The year is 2 bytes, followed by the month, then day, all 2 bytes.</summary>
		/// <param name="exePath">The path of the Prime Time executable</param>
		/// <returns>The offset in bytes of the message data or 0 if the offset could not be
		/// found </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.MacOffset FindMacEvalExpirationDateOffset(string exePath)
		{
			PTCustomizeUtil.MacOffset retOffsets = new PTCustomizeUtil.MacOffset();

			try
			{
				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.LittleEndian;
				
				retOffsets.PPC = FindMsgDataOffset( exePath, EVAL_EXPIRATION_DATE_PREFIX, 0 );

				// If we found the offset, then step passed the data prefix
				if( retOffsets.PPC > 0 )
					retOffsets.PPC += 4 * WcharSize;

				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;

				retOffsets.x86 = FindMsgDataOffset( exePath, EVAL_EXPIRATION_DATE_PREFIX, 0 );
				if( retOffsets.x86 > 0 )
					retOffsets.x86 += 4 * WcharSize;

				retOffsets.x64 = FindMsgDataOffset( exePath, EVAL_EXPIRATION_DATE_PREFIX, retOffsets.x86 + 10 );
				if( retOffsets.x64 > 0 )
					retOffsets.x64 += 4 * WcharSize;
			}
			finally
			{
				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;
				PTExeDiscovery.WcharSize = 2;
			}

			return retOffsets;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find a offset within the EXE file of the message characters</summary>
		/// <param name="exePath">The path of the Prime Time executable</param>
		/// <param name="keys">The keys that identify the message data</param>
		/// <param name="startOffset">The offset, in bytes from the start of the file, to start
		/// looking for the message data</param>
		/// <returns>The offset in bytes of the message data or 0 if the offset could not be
		/// found </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private static long FindMsgDataOffset(string exePath, ushort[] keys, long startOffset)
		{
			long retOffset = 0;
			
			try
			{
				FileStream exeFileStream = File.OpenRead( exePath );
				BinaryReader reader = new BinaryReader( exeFileStream );

				byte[] exeFileData = reader.ReadBytes( (int)exeFileStream.Length );

				exeFileStream.Close();

				// Create the array of bytes to find
				byte[] searchBytes = new byte[keys.Length * 2];
				int byteMatchIndex = 0;
				foreach( ushort curChar in keys )
				{
					if( CharByteOrder == PTCustomizeUtil.CharacterByteOrder.BigEndian )
					{
						searchBytes[byteMatchIndex++] = (byte)( curChar & 0xFF );
						searchBytes[byteMatchIndex++] = (byte)( ( curChar >> 8 ) & 0xFF );
					}
					else
					{
						searchBytes[byteMatchIndex++] = (byte)( ( curChar >> 8 ) & 0xFF );
						searchBytes[byteMatchIndex++] = (byte)( curChar & 0xFF );						
					}
				}

				// Go through the file
				byteMatchIndex = 0;
				long charMatchStartPosition = 0;
				long curExeDataByteIndex = startOffset;
				while( curExeDataByteIndex < exeFileData.Length )
				{
					// Read a byte
					byte curByte = exeFileData[ curExeDataByteIndex++ ];

					// If the byte matches the value currently being tested
					if( curByte == searchBytes[byteMatchIndex] )
					{
						// If this is the first match
						if( byteMatchIndex == 0 )
							charMatchStartPosition = curExeDataByteIndex - 1;

						// Increment the search
						++byteMatchIndex;

						// If we've matched the second byte of a wchar_t
						if( byteMatchIndex % 2 == 0 )
						{
							// If we're dealing with wchar_t larger than 2 bytes
							curExeDataByteIndex += WcharSize - 2;
						}
					}
					// Else restart the search
					else
					{
						byteMatchIndex = 0;

						// If this byte matches the first byte to find
						if( curByte == searchBytes[byteMatchIndex] )
						{
							++byteMatchIndex;

							// Store the offset
							charMatchStartPosition = curExeDataByteIndex - 1;
						}
					}

					// If the characters were matched
					if( byteMatchIndex == searchBytes.Length )
					{
						// We look for two bytes at a time, so offset it if we're dealing with a larger char
						charMatchStartPosition -= WcharSize - 2;

						retOffset = charMatchStartPosition;
						break;
					}
				}

				
			}
			catch( Exception )
			{
				throw;
			}

			return retOffset;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the key value.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.FullKeyOffset FindMacPPCKeyOffset(string exePath)
		{
			try
			{
				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.LittleEndian;
				return FindKeyOffset( exePath, 0 );
			}
			catch
			{
				throw;
			}
			finally
			{
				PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the key value.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.FullKeyOffset FindMacx86KeyOffset(string exePath)
		{
			return FindKeyOffset( exePath, 0 );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the key value.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.FullKeyOffset FindMacx64KeyOffset(string exePath)
		{
			PTCustomizeUtil.FullKeyOffset keyOffset = PTExeDiscovery.FindKeyOffset( exePath );
			return PTExeDiscovery.FindKeyOffset( exePath, keyOffset.KeyOffset + 10 );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the key value.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.FullKeyOffset FindKeyOffset(string exePath)
		{
			return FindKeyOffset( exePath, 0 );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the key value.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.FullKeyOffset FindKeyOffset(string exePath, long startOffset)
		{
			PTCustomizeUtil.FullKeyOffset retOffset = new PTCustomizeUtil.FullKeyOffset();

			// Open the file
			try
			{
				FileStream exeFileStream = File.OpenRead( exePath );
				BinaryReader reader = new BinaryReader( exeFileStream );
				byte[] exeFileData = reader.ReadBytes( (int)exeFileStream.Length );
				exeFileStream.Close();

				byte[] searchBytes = PTCustomizeUtil.UInt64ToArray( GAME_FIXED_KEYS[0], CharByteOrder );

				// Go through the file
				int keyMatchIndex = 0;
				int byteMatchIndex = 0;
				long keyMatchStartPosition = 0;
				long curExeDataByteIndex = startOffset;
				while( curExeDataByteIndex < exeFileData.Length )
				{
					byte curByte = exeFileData[curExeDataByteIndex++];

					// If the byte matches the value currently being tested
					if( curByte == searchBytes[byteMatchIndex] )
					{
						// If this is the first match
						if( byteMatchIndex == 0 && keyMatchIndex == 0 )
							keyMatchStartPosition = curExeDataByteIndex - 1;

						// Increment the search
						++byteMatchIndex;
					}
					// Else restart the search if there are matches
					else if( keyMatchIndex > 0 || byteMatchIndex > 0 )
					{
						// If there are matches then restart
						keyMatchIndex = 0;
						searchBytes = PTCustomizeUtil.UInt64ToArray( GAME_FIXED_KEYS[keyMatchIndex], CharByteOrder );

						byteMatchIndex = 0;

						// If this byte matches the first byte to find
						if( curByte == searchBytes[byteMatchIndex] )
						{
							++byteMatchIndex;

							// Store the offset
							keyMatchStartPosition = curExeDataByteIndex - 1;
						}

						continue;
					}

					// If the match has found the first four bytes
					if( byteMatchIndex == 4 )
					{
						int oldPadding = retOffset.Intra64bitPadding;

						// Get passed the padding between the two DWORDs that make up the 64-bit integer
						retOffset.Intra64bitPadding = StepPassed64Padding( searchBytes[byteMatchIndex], exeFileData, curExeDataByteIndex );
						curExeDataByteIndex += retOffset.Intra64bitPadding;

						// If the padding changed then FREAK OUT
						if( oldPadding != -1 && oldPadding != retOffset.Intra64bitPadding )
						{
							// The padding is incorrect so ignore this
							keyMatchIndex = 0;
							searchBytes = PTCustomizeUtil.UInt64ToArray( GAME_FIXED_KEYS[keyMatchIndex], CharByteOrder );

							byteMatchIndex = 0;

							//throw new ApplicationException( "Intra-64-bit integer offset (Between DWORDs) not standard!" );
						}

						continue;
					}
					// Else if the key was matched
					else if( byteMatchIndex == 8 )
					{
						++keyMatchIndex;

						// If all of the keys have matched
						if( keyMatchIndex == GAME_FIXED_KEYS.Length )
							break;
						// The 3 item in the key list is the one that gets overwritten so skip it
						else if( keyMatchIndex == 2 )
						{
							int amountToSkip = 8 + retOffset.Inter64bitPadding + retOffset.Intra64bitPadding;
							curExeDataByteIndex += amountToSkip;
							keyMatchIndex = 3;
						}

						// Get the new bytes to find
						searchBytes = PTCustomizeUtil.UInt64ToArray( GAME_FIXED_KEYS[keyMatchIndex], CharByteOrder );
						byteMatchIndex = 0;

						int oldPadding = retOffset.Inter64bitPadding;

						// Get past the padding between 64-bit integers
						retOffset.Inter64bitPadding = StepPassed64Padding( searchBytes[byteMatchIndex], exeFileData, curExeDataByteIndex );
						curExeDataByteIndex += retOffset.Inter64bitPadding;

						// If the padding changed then FREAK OUT
						if( oldPadding != -1 && oldPadding != retOffset.Inter64bitPadding )
							throw new ApplicationException( "64-bit integer offset not standard!" );

						continue;
					}
				}

				// If the keys were found
				if( keyMatchIndex == GAME_FIXED_KEYS.Length )
				{
					long PACKED_KEY_LEN = 8 + retOffset.Inter64bitPadding + retOffset.Intra64bitPadding;
					long actualKeyOffsetFromStart = PACKED_KEY_LEN * ACTUAL_KEY_INDEX;

					// Store the offset
					retOffset.KeyOffset = keyMatchStartPosition + actualKeyOffsetFromStart;
				}
			}
			catch( Exception )
			{
				// This is just here for convenience in case I want to handle the exception or not
				throw;
			}

			return retOffset;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the custom pic
		/// data.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static long FindCustomPicOffset(string exePath)
		{
			return FindCustomPicOffset( exePath, 0 );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the custom pic
		/// data.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static PTCustomizeUtil.MacOffset FindMacCustomPicDataOffsets(string exePath)
		{
			PTCustomizeUtil.MacOffset retOffsets = new PTCustomizeUtil.MacOffset();
			retOffsets.PPC = FindCustomPicOffset( exePath, 0 );
			retOffsets.x86 = FindCustomPicOffset( exePath, retOffsets.PPC + 10 );
			retOffsets.x64 = FindCustomPicOffset( exePath, retOffsets.x86 + 10 );

			return retOffsets;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>Find the offset with in the Prime Time executable of the custom pic
		/// data.</summary>
		/// <param name="exePath">The full path to the Prime Time executable</param>
		/// <returns>The offset to the key or 0 on error</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public static long FindCustomPicOffset(string exePath, long startOffset)
		{
			// Open the file
			long keyMatchStartPosition = 0;
			try
			{
				FileStream exeFileStream = File.OpenRead( exePath );
				BinaryReader reader = new BinaryReader( exeFileStream );

				byte[] exeFileData = reader.ReadBytes( (int)exeFileStream.Length );

				exeFileStream.Close();

				// Go through the file
				//byte[] prevBytes = new byte[GAME_CUSTOM_PIC_END_KEYS.Length];
				int byteMatchIndex = 0;
				long curExeDataByteIndex = startOffset;
				while( curExeDataByteIndex < exeFileData.LongLength )
				{
					byte curByte = exeFileData[curExeDataByteIndex++];

					// If the byte matches the value currently being tested
					if( curByte == GAME_CUSTOM_PIC_KEYS[byteMatchIndex] )
					{
						// If this is the first match
						if( byteMatchIndex == 0 )
							keyMatchStartPosition = curExeDataByteIndex - 1;

						// Increment the search
						++byteMatchIndex;
					}
					// Else restart the search if there are matches
					else if( byteMatchIndex > 0 )
					{
						byteMatchIndex = 0;

						// If this byte matches the first byte to find
						if( curByte == GAME_CUSTOM_PIC_KEYS[byteMatchIndex] )
						{
							++byteMatchIndex;

							// Store the offset
							keyMatchStartPosition = curExeDataByteIndex - 1;
						}

						continue;
					}

					// If the key was matched
					if( byteMatchIndex == GAME_CUSTOM_PIC_KEYS.Length )
					{
						// Ensure this block has the right end data
						int skipBytes = CUSTOM_PIC_DATA_LEN - GAME_CUSTOM_PIC_KEYS.Length - GAME_CUSTOM_PIC_END_KEYS.Length;
						curExeDataByteIndex += skipBytes;
						
						for( int endByteIndex = 0; endByteIndex < GAME_CUSTOM_PIC_END_KEYS.Length; ++endByteIndex )
						{
							if( GAME_CUSTOM_PIC_END_KEYS[endByteIndex] != exeFileData[curExeDataByteIndex + endByteIndex] )
							{
								byteMatchIndex = 0;
								break;
							}
						}

						if( byteMatchIndex > 0 )
						{
							// Step passed the start keys
							keyMatchStartPosition += 14;

							return keyMatchStartPosition;
						}
					}

					// Update the previous bytes
					//for( int prevByteIndex = 1; prevByteIndex < prevBytes.Length; ++prevByteIndex )
					//	prevBytes[prevByteIndex] = prevBytes[prevByteIndex - 1];
					//prevBytes[prevBytes.Length - 1] = curByte;
				}
			}
			catch( Exception )
			{
				// This is just here for convenience in case I want to handle the exception or not
				throw;
			}

			// If the key was found then it would have been returned before this point
			return 0;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 64-bit integers are stored in the compiled executable differently between debug and
		/// release versions. This method steps one byte at a time in an attempt to compensate
		/// for this issue.
		/// </summary>
		/// <param name="nextByte">The next expected byte to find of the 8-byte integer</param>
		/// <param name="reader">The file reader</param>
		/// <returns>The last discovered byte. This can be compared to the nextByte parameter to
		/// determine if the value was found. If so, the Int64Padding member variable stores the
		/// number of padding bytes.</returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private static int StepPassed64Padding(byte nextByte, byte[] exeData, long curDataOffset)
		{
			const int MaxSearch = 32;

			// Read bytes and stop if the next byte in the 64-bit integer is found
			byte curByte;
			long originalOffset = curDataOffset;			
			do
			{
				curByte = exeData[curDataOffset++];
			}
			while( curByte != nextByte && (curDataOffset - originalOffset) < MaxSearch );

			if( curByte == nextByte )
				return (int)(curDataOffset - originalOffset) - 1;
			else
				return -2;
		}
	}
}
