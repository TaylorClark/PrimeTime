using System;
using System.Drawing;
using System.Collections.Generic;

namespace FontEditor
{
	//---------------------------------------------------------------------------------------------
	/// <summary>
	/// The font object that stores a font's character data
	/// </summary>
	//---------------------------------------------------------------------------------------------
	public class PTFont
	{
		/// <summary> The list of characters for the font </summary>
		private Dictionary<char, Rectangle> m_CharList = new Dictionary<char, Rectangle>();
		
		/// <summary> The resource ID of the image to use for the font </summary>
		public uint m_ImageResID = 0;

		/// <summary> The height of the characters in pixels </summary>
		private int m_CharHeight = 16;

		/// <summary> The spacing, in pixels, between letters </summary>
		public int m_LetterSpacing = 2;

		/// <summary> The name of the font </summary>
		public string m_Name = "";


		/// <summary> The current font file version </summary>
		private const int FONT_FILE_VER = 1;

		/// <summary> The minimum width, in pixels, a character is allowed </summary>
		public const int MIN_CHAR_WIDTH = 2;

		/// <summary> The minimum height, in pixels, a character is allowed </summary>
		public const int MIN_CHAR_HEIGHT = 2;

		/// <summary> The first normal readable unicode character supported </summary>
		public const int MIN_UNICODE_READABLE = 33;

		/// <summary> The last normal readable unicode character supported </summary>
		public const int MAX_UNICODE_READABLE = 126;


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Reset the font back to its default values
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void Reset()
		{
			m_CharHeight = 16;
			m_LetterSpacing = 2;
			m_ImageResID = 0;
			m_CharList.Clear();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a character to the font
		/// </summary>
		/// <param name="newChar"> The character to add </param>
		/// <returns> True if the character was added, false if it was already in the 
		/// list </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public bool AddChar( char newChar )
		{
			return AddChar( newChar, new Rectangle(0,0,MIN_CHAR_WIDTH,m_CharHeight) );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a character to the font
		/// </summary>
		/// <param name="newChar"> The character to add </param>
		/// <param name="imgRect"> The pixel coordinates to find the character in the image </param>
		/// <returns> True if the character was added, false if it was already in the 
		/// list </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public bool AddChar( char newChar, Rectangle imgRect )
		{
			// If the character is already in the list then bail
			if( m_CharList.ContainsKey( newChar ) )
				return false;

			// Ensure the height is correct
			imgRect.Height = m_CharHeight;
			if( imgRect.Width < MIN_CHAR_WIDTH )
				imgRect.Width = MIN_CHAR_WIDTH;

			// Add the character
			m_CharList.Add( newChar, imgRect );

			// Return true that we added the character
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Remove a character from the font
		/// </summary>
		/// <param name="charToRemove"> The character to remove </param>
		/// <returns> True if the character was removed, false if it wasn't in the list </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public bool RemoveChar( char charToRemove )
		{
			// If the character is not provided by the font then do nothing
			if( !m_CharList.ContainsKey( charToRemove ) )
				return false;

			// Remove the character
			m_CharList.Remove( charToRemove );
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Update or add a character and its glyph rectangle
		/// </summary>
		/// <param name="modChar"> The character whose glyph rectangle we are updating </param>
		/// <param name="imgRect"> The new image rectangle </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void UpdateChar( char modChar, Rectangle imgRect )
		{
			// Ensure proper dimensions
			imgRect.Height = m_CharHeight;
			if( imgRect.Width < MIN_CHAR_WIDTH )
				imgRect.Width = MIN_CHAR_WIDTH;

			// Update or add the character the rectangle
			if( m_CharList.ContainsKey( modChar ) )
				m_CharList[ modChar ] = imgRect;
			else
				m_CharList.Add( modChar, imgRect );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Save a font to file </summary>
		/// <param name="sFilePath"> The complete file path to the file to write to </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void SaveFont( string sFilePath )
		{
			System.IO.FileStream fileStream = null;
			try
			{
				// Open the file
				fileStream = new System.IO.FileStream(sFilePath, System.IO.FileMode.Create, System.IO.FileAccess.Write);

				// Create the binary writer to write data
				System.IO.BinaryWriter binWriter = new System.IO.BinaryWriter(fileStream,System.Text.Encoding.Unicode);

				// Store the file
				TransferFontFile( new TCUtil.NetSafeSimpleSerializer( binWriter ) );
			}
			catch (Exception exc)
			{
				System.Windows.Forms.MessageBox.Show(exc.Message, "Error Saving Font", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
			}

			// Close the file
			if (fileStream != null)
				fileStream.Close();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Load a font from file </summary>
		/// <param name="sFilePath"> The complete file path of the file to read from </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public void LoadFont( string sFilePath )
		{
			System.IO.FileStream fileStream = null;
			try
			{
				// Open the file
				fileStream = new System.IO.FileStream(sFilePath, System.IO.FileMode.Open, System.IO.FileAccess.Read);

				// Create the binary writer to write data
				System.IO.BinaryReader binReader = new System.IO.BinaryReader(fileStream, System.Text.Encoding.Unicode);

				// Clear our data
				m_CharList.Clear();

				// Store the file
				TransferFontFile( new TCUtil.NetSafeSimpleSerializer( binReader ) );
			}
			catch (Exception exc)
			{
				System.Windows.Forms.MessageBox.Show(exc.Message, "Error Loading Font", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
			}

			// Close the file
			if( fileStream != null )
				fileStream.Close();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Read or write font data to/from file
		/// </summary>
		/// <param name="serializer"> The serializer to handle the data </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void TransferFontFile( TCUtil.ISimpleSerializer serializer )
		{
			// The game resource key
			int fourCCVal = ResourceCS.ResourceToolsDB.REZ_FOURCC.ToInt32();
			serializer.AddData(ref fourCCVal);
			TCBase.FourCC fourCC = new TCBase.FourCC(fourCCVal);
			if (fourCC != ResourceCS.ResourceToolsDB.REZ_FOURCC)
			{
			}

			// The resource type
			int resTypeVal = (int)ResourceCS.ResourceType.Font;
			serializer.AddData(ref resTypeVal);
			ResourceCS.ResourceType resType = (ResourceCS.ResourceType)resTypeVal;
			if (resType != ResourceCS.ResourceType.Font)
			{
			}

			// Create a buffer for the name and clear its data
			char[] fontName = new char[ResourceCS.ResourceToolsDB.REZ_NAME_LEN];
			for (int charIndex = 0; charIndex < fontName.Length; ++charIndex)
				fontName[charIndex] = '\x0000';

			// The name of the font
			if (serializer.InReadMode)
			{
				serializer.AddData(ref fontName);
				m_Name = new String( fontName );
			}
			// Otherwise we are saving the font
			else
			{
				m_Name.CopyTo(0, fontName, 0, m_Name.Length);
				serializer.AddData(ref fontName);
			}

			// The font file version
			int fileVer = FONT_FILE_VER;
			serializer.AddData(ref fileVer);

			// The character height
			serializer.AddData( ref m_CharHeight );

			// The letter spacing
			serializer.AddData(ref m_LetterSpacing);

			// The image file resource ID
			serializer.AddData( ref m_ImageResID );

			// The number of characters
			int charCount = 0;
			if( serializer.InWriteMode )
				charCount = m_CharList.Count;
			else
				m_CharList.Clear();
			serializer.AddData( ref charCount );

			if (serializer.InWriteMode)
			{
				foreach (char curChar in m_CharList.Keys )
				{
					// The character
					char curCharCopy = curChar;
					serializer.AddData(ref curCharCopy);

					// The glyph rectangle
					Rectangle glyphRect = m_CharList[curChar];
					int rectVal = glyphRect.X;
					serializer.AddData(ref rectVal);
					rectVal = glyphRect.Y;
					serializer.AddData(ref rectVal);
					rectVal = glyphRect.Width;
					serializer.AddData(ref rectVal);
					rectVal = glyphRect.Height;
					serializer.AddData(ref rectVal);
				}
			}
			else
			{
				// Go through each character
				for (int charIndex = 0; charIndex < charCount; ++charIndex)
				{
					// The character
					char curChar = '\0';
					serializer.AddData(ref curChar);

					// The glyph rectangle
					Rectangle glyphRect = new Rectangle();
					int rectVal = 0;
					serializer.AddData(ref rectVal);
					glyphRect.X = rectVal;
					serializer.AddData(ref rectVal);
					glyphRect.Y = rectVal;
					serializer.AddData(ref rectVal);
					glyphRect.Width = rectVal;
					serializer.AddData(ref rectVal);
					glyphRect.Height = rectVal;

					// Store the data if we are reading in
					m_CharList.Add(curChar, glyphRect);
				}
			}
			
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string of all the characters the font offers
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public string CharString
		{
			get
			{
				// Go through each character and add it to a string
				string retStr = "";
				foreach( char curChar in m_CharList.Keys )
				{
					retStr += curChar;
				}

				// Return the string of characters
				return retStr;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get or set the height of each character in the font
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public int CharHeight
		{
			get{ return m_CharHeight; }
			set
			{
				// Store the height
				m_CharHeight = value;
				if( m_CharHeight < MIN_CHAR_HEIGHT )
					m_CharHeight = MIN_CHAR_HEIGHT;

				// Go through each character and set the height
				List<char> chars = new List<char>(m_CharList.Keys);
				foreach( char curChar in chars )
				{
					Rectangle curRect = m_CharList[curChar];
					m_CharList[curChar] = new Rectangle( curRect.X, curRect.Y, curRect.Width, m_CharHeight );
				}
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Determine if this font contains a certain character. </summary>
		/// <param name="testChar"> The character to test for existance in the font. </param>
		/// <returns> True if the font contains the character, false otherwise. </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public bool ContainsChar(char testChar)
		{
			return m_CharList.ContainsKey(testChar);
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get or set the image rectangle for a character from the font
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public Rectangle this[ char curChar ]
		{
			get
			{
				// If the character is not in the list then return an empty rectangle
				if( !m_CharList.ContainsKey( curChar ) )
					return new Rectangle( 0,0,0,0 );
				return m_CharList[curChar];
			}

			set
			{
				if( !m_CharList.ContainsKey( curChar ) )
					return;
				m_CharList[curChar] = value;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the width, in pixels, of the space character
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public int SpaceWidth
		{
			get{ return (m_CharHeight * 3) / 4; }
		}
	}
}
