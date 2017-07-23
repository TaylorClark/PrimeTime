using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Threading;
using System.Runtime.InteropServices;

namespace FontGen
{
	public partial class FormMain : Form
	{
		private Font m_Font = new Font( "Arial", 36 );
		private Color m_FontColor = Color.Black;
		private Color m_BGColor = Color.Magenta;
		private static List<char> sm_RenderChars = new List<char>();

		/// <summary>The map of characters to their rectangular area on the source image</summary>
		private Dictionary<char, Rectangle> m_ActualCharBounds = new Dictionary<char, Rectangle>();

		/// <summary>The bitmap to which the font image is drawn</summary>
		private Bitmap m_BufferBitmap = new Bitmap(256, 256, PIX_FORMAT);

		private int m_MaxCharHeight = 0;

		private const PixelFormat PIX_FORMAT = PixelFormat.Format24bppRgb;

		public FormMain()
		{
			InitializeComponent();
			GenerateCharList(checkBoxCaps.Checked);

			pictureBoxBGColor.BackColor = m_BGColor;

			RedrawBitmap();
		}

		private static void GenerateCharList( bool ignoreCaps)
		{
			sm_RenderChars.Clear();

			for( char curChar = 'A'; curChar <= 'Z'; ++curChar )
				sm_RenderChars.Add(curChar);

			if (!ignoreCaps)
			{
				for( char curChar = 'a'; curChar <= 'z'; ++curChar )
					sm_RenderChars.Add( curChar );
			}

			for (char curChar = '0'; curChar <= '9'; ++curChar)
				sm_RenderChars.Add(curChar);

			sm_RenderChars.Add('-');
			sm_RenderChars.Add('!');
			sm_RenderChars.Add('?');
			sm_RenderChars.Add('\'');
			sm_RenderChars.Add('\"');
			sm_RenderChars.Add(',');
			sm_RenderChars.Add('.');
		}

		private static void SetGraphicsQuality(Graphics gfx, bool makeHigh)
		{
			if( makeHigh )
			{
				gfx.SmoothingMode = SmoothingMode.AntiAlias;
				gfx.TextRenderingHint = TextRenderingHint.AntiAlias;
				gfx.CompositingQuality = CompositingQuality.HighQuality;
				gfx.InterpolationMode = InterpolationMode.HighQualityBicubic;
			}
			else
			{
				gfx.SmoothingMode = SmoothingMode.None;
				gfx.TextRenderingHint = TextRenderingHint.SingleBitPerPixel;
				gfx.CompositingQuality = CompositingQuality.HighSpeed;
				gfx.InterpolationMode = InterpolationMode.NearestNeighbor;
			}
		}

		private void FormMain_Paint(object sender, PaintEventArgs e)
		{
			//e.Graphics.DrawString("ABCabc", m_Font, new SolidBrush(m_FontColor), new PointF(100.0f, 100.0f));

			//TextRenderer.DrawText( e.Graphics, "TESTtest", m_Font, new Point(100,100), m_FontColor, Color.Red );
			//e.Graphics.TextRenderingHint = TextRenderingHint.SingleBitPerPixel;
			//e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;

			SetGraphicsQuality( e.Graphics, true );

			lock (m_BufferBitmap)
			{
				e.Graphics.DrawImage( m_BufferBitmap, 25, 25 );
			}
		}

		private void RedrawBitmap()
		{
			lock(m_BufferBitmap)
			{
				Graphics gfx = Graphics.FromImage( m_BufferBitmap );

				SetGraphicsQuality( gfx, true );

				DrawText( gfx, m_Font, m_FontColor );
			}

			Invalidate();
		}


		private void DrawText( Graphics graphics, Font font, Color color )
		{
			// Clear the entire bitmap with the background color
			SetGraphicsQuality( graphics, false );
			graphics.FillRectangle(new SolidBrush(m_BGColor), graphics.VisibleClipBounds);
			
			SolidBrush brush = new SolidBrush(color);
			TextFormatFlags useFlags = TextFormatFlags.Default; //TextFormatFlags.NoPadding | TextFormatFlags.Right;
			Size initialSize = new Size(int.MaxValue, int.MaxValue);

			// Clear our map of character glyph information
			m_ActualCharBounds.Clear();

			// Go through each character, render it to the background, and then measure and store
			// its dimensions
			Dictionary<char,Rectangle> tempCharBounds = new Dictionary<char, Rectangle>();
			m_MaxCharHeight = 0;
			foreach (char curChar in sm_RenderChars)
			{
				// Get the estimated size of the current character. Double it so we can be sure it
				// covers the whole character
				Size size = TextRenderer.MeasureText( graphics, curChar.ToString(), font, initialSize, useFlags);
				size.Width *= 2;
				size.Height *= 2;

				// This is just a buffer, in pixels, from the edge to ensure text isn't rendered
				// off the edge
				const int SafeOffset = 10;

				// Draw the character (With high quality rendering)
				SetGraphicsQuality( graphics, true );
				TextRenderer.DrawText( graphics, curChar.ToString(), font, new Point( SafeOffset, SafeOffset ), color, useFlags );

				// Get the raw pixel data so we can step through it and find the exact edges of
				// the current character
				BitmapData lockData = m_BufferBitmap.LockBits( new Rectangle( Point.Empty, size ), ImageLockMode.ReadOnly, PIX_FORMAT );
				Rectangle actualRect = GetActualCharBounds( lockData );
				actualRect.X -= SafeOffset;
				actualRect.Y -= SafeOffset;
				if (actualRect.Height > m_MaxCharHeight)
					m_MaxCharHeight = actualRect.Height;
				m_BufferBitmap.UnlockBits(lockData);

				// Clear where the character was drawn. We clear the entire image just to ensure no
				// stray pixels remain. We use low quality rendering here to ensure there is no
				// attempt to blend the background color in any way
				SetGraphicsQuality( graphics, false );
				graphics.FillRectangle( new SolidBrush( m_BGColor ), graphics.VisibleClipBounds );

				// Store the character
				tempCharBounds[curChar] = actualRect;
			}

			// Again, clear the entire bitmap with the background color since we're done measuring
			// text and it's time to generate the image
			SetGraphicsQuality( graphics, false );
			graphics.FillRectangle( new SolidBrush( m_BGColor ), graphics.VisibleClipBounds );
			SetGraphicsQuality( graphics, true );				

			// Go through each character and fit them together
			Point curPos = new Point( 1, 1 );
			foreach (char curChar in tempCharBounds.Keys)
			{
				Rectangle curRect = tempCharBounds[curChar];
				Point textPos = curPos;
				textPos.X -= curRect.X;
				textPos.Y -= curRect.Y;
				textPos.Y += m_MaxCharHeight - curRect.Height;

				if (curPos.X + curRect.Width > graphics.VisibleClipBounds.Right)
				{
					curPos.X = 1;
					curPos.Y += m_MaxCharHeight + 1;

					if (curPos.Y + m_MaxCharHeight > graphics.VisibleClipBounds.Bottom)
						break;

					textPos = curPos;
					textPos.X -= curRect.X;
					textPos.Y -= curRect.Y;
					textPos.Y += m_MaxCharHeight - curRect.Height;
				}

				TextRenderer.DrawText(graphics, curChar.ToString(), font, textPos, color, useFlags);

				curRect.X = curPos.X;
				curRect.Y = curPos.Y;
				m_ActualCharBounds[curChar] = curRect;

				curPos.X += curRect.Width + 1;
			}
		}

		enum BoundDir
		{
			Left,
			Right,
			Top,
			Bottom
		}

		private int GetBound( byte[,,] pixels, BoundDir dir )
		{
			int width = pixels.GetLength( 0 );
			int height = pixels.GetLength(1);

			byte[] bgComps = new byte[] { m_BGColor.B, m_BGColor.G, m_BGColor.R };
			byte[] boundComps = new byte[] { Color.LightGreen.B, Color.LightGreen.G, Color.LightGreen.R };

			int bound = 0;

			switch( dir )
			{
				case BoundDir.Left:
				{
					for (int curX = 0; curX < width; )
					{
						bool foundPixel = false;
						for (int curY = 0; curY < height; ++curY)
						{
							bool isPixelBG = pixels[curX, curY, 0] == bgComps[0]
											 && pixels[curX, curY, 1] == bgComps[1]
											 && pixels[curX, curY, 2] == bgComps[2];

							isPixelBG |= pixels[curX, curY, 0] == boundComps[0]
											 && pixels[curX, curY, 1] == boundComps[1]
											 && pixels[curX, curY, 2] == boundComps[2];
							if (!isPixelBG)
							{
								foundPixel = true;
								bound = curX;
								break;
							}
						}

						if (foundPixel)
							break;

						++curX;
					}
				}
					break;

				case BoundDir.Right:
				{
					for (int curX = width - 1; curX >= 0; --curX)
					{
						bool foundPixel = false;
						for (int curY = 0; curY < height; ++curY)
						{
							bool isPixelBG = pixels[curX, curY, 0] == bgComps[0]
											 && pixels[curX, curY, 1] == bgComps[1]
											 && pixels[curX, curY, 2] == bgComps[2];

							isPixelBG |= pixels[curX, curY, 0] == boundComps[0]
											 && pixels[curX, curY, 1] == boundComps[1]
											 && pixels[curX, curY, 2] == boundComps[2];

							if (!isPixelBG)
							{
								foundPixel = true;
								bound = curX;
								break;
							}
						}

						if (foundPixel)
							break;
					}
				}
					break;

				case BoundDir.Top:
					{
						for (int curY = 0; curY < height; ++curY)
						{
							bool foundPixel = false;
							for (int curX = 0; curX < width; ++curX)
							{
								bool isPixelBG = pixels[curX, curY, 0] == bgComps[0]
												 && pixels[curX, curY, 1] == bgComps[1]
												 && pixels[curX, curY, 2] == bgComps[2];

								isPixelBG |= pixels[curX, curY, 0] == boundComps[0]
												 && pixels[curX, curY, 1] == boundComps[1]
												 && pixels[curX, curY, 2] == boundComps[2];
								if (!isPixelBG)
								{
									foundPixel = true;
									bound = curY;
									break;
								}
							}

							if (foundPixel)
								break;
						}
					}
					break;

				case BoundDir.Bottom:
				{
					for (int curY = height - 1; curY >= 0; --curY)
					{
						bool foundPixel = false;
						for (int curX = 0; curX < width; ++curX)
						{
							bool isPixelBG = pixels[ curX, curY, 0 ] == bgComps[ 0 ]
							                 && pixels[ curX, curY, 1 ] == bgComps[ 1 ]
							                 && pixels[ curX, curY, 2 ] == bgComps[ 2 ];

							isPixelBG |= pixels[ curX, curY, 0 ] == boundComps[ 0 ]
							             && pixels[ curX, curY, 1 ] == boundComps[ 1 ]
							             && pixels[ curX, curY, 2 ] == boundComps[ 2 ];

							if( !isPixelBG )
							{
								foundPixel = true;
								bound = curY;
								break;
							}
						}

						if( foundPixel )
							break;
					}
				}
					break;
			}

			return bound;
		}

		private Rectangle GetActualCharBounds( BitmapData lockedData )
		{
			Rectangle retRect = new Rectangle();

			// Convert the data into an array that can be randomly accessed
			byte[,,] rgbValues = new byte[lockedData.Width, lockedData.Height, 3];
			int curX = 0;
			int curY = 0;
			byte[] rgbRawValues = new byte[lockedData.Width * 3];
			for (curY = 0; curY < lockedData.Height; ++curY)
			{
				// Copy the RGB values into the array
				IntPtr pixelPtr = new IntPtr( lockedData.Scan0.ToInt64() + (long) ( ( curY ) * lockedData.Stride ) );// + (long)(imgPos.X * 3) );
				System.Runtime.InteropServices.Marshal.Copy(pixelPtr, rgbRawValues, 0, rgbRawValues.Length);

				for (curX = 0; curX < lockedData.Width; curX++)
				{
					int srcX = curX * 3;
					rgbValues[curX, curY, 0] = rgbRawValues[srcX];
					rgbValues[curX, curY, 1] = rgbRawValues[srcX + 1];
					rgbValues[curX, curY, 2] = rgbRawValues[srcX + 2];
				}
			}


			retRect.X = GetBound( rgbValues, BoundDir.Left );

			retRect.Width = (GetBound(rgbValues, BoundDir.Right) - retRect.X) + 1;

			retRect.Y = GetBound( rgbValues, BoundDir.Top );

			retRect.Height = (GetBound(rgbValues, BoundDir.Bottom) - retRect.Y) + 1;

			return retRect;
		}

		private void fontToolStripMenuItem_Click(object sender, EventArgs e)
		{
			fontDialog.ShowDialog();

			m_Font = fontDialog.Font;
			
			RedrawBitmap();
			Invalidate();
		}

		private static bool IsPowerOf2( int val )
		{
			return ( val & ( val - 1 ) ) == 0;
		}

		private void HandleUpDownValChange( NumericUpDown ctrl )
		{
			ctrl.ValueChanged -= numericUpDownWidth_ValueChanged;

			// If the user scrolled up
			if (IsPowerOf2((int)ctrl.Value - 1))
			{
				int newVal = ((int)ctrl.Value - 1) * 2;
				if (newVal <= ctrl.Maximum)
					ctrl.Value = newVal;
				else
					ctrl.Value--;
			}
			// Else the user scrolled down
			else
			{
				int newVal = ((int)ctrl.Value + 1) / 2;
				if (newVal >= ctrl.Minimum)
					ctrl.Value = newVal;
				else
					ctrl.Value++;
			}

			// Recreate the bitmap
			lock( m_BufferBitmap )
			{
				m_BufferBitmap = new Bitmap((int)numericUpDownWidth.Value, (int)numericUpDownHeight.Value, PIX_FORMAT);
			}
			Thread t = new Thread(RedrawBitmap);
			t.Start();

			ctrl.ValueChanged += numericUpDownWidth_ValueChanged;
		}

		private void numericUpDownWidth_ValueChanged(object sender, EventArgs e)
		{
			HandleUpDownValChange((NumericUpDown)sender);
		}

		private void checkBoxCaps_CheckedChanged(object sender, EventArgs e)
		{
			GenerateCharList(checkBoxCaps.Checked);

			RedrawBitmap();
			Invalidate();
		}

		private void fontColorToolStripMenuItem_Click(object sender, EventArgs e)
		{
			colorDialog.Color = m_FontColor;
			if( colorDialog.ShowDialog() != DialogResult.OK )
				return;

			m_FontColor = colorDialog.Color;

			RedrawBitmap();
			Invalidate();
		}

		private void pictureBoxBGColor_Click(object sender, EventArgs e)
		{
			colorDialog.Color = m_BGColor;
			if (colorDialog.ShowDialog() != DialogResult.OK)
				return;

			pictureBoxBGColor.BackColor = colorDialog.Color;
			m_BGColor = colorDialog.Color;

			RedrawBitmap();
			Invalidate();
		}

		private void buttonSave_Click(object sender, EventArgs e)
		{
			saveFileDialog.Title = "Save Font...";
			saveFileDialog.Filter = "Font Files (*.fnt)|*.fnt|All Files (*.*)|*.*";
			if( saveFileDialog.ShowDialog() != DialogResult.OK )
				return;

			FontEditor.PTFont saveFont = new FontEditor.PTFont();
			foreach (char curChar in m_ActualCharBounds.Keys)
				saveFont.AddChar( curChar, m_ActualCharBounds[ curChar ] );
			if (checkBoxCaps.Checked)
			{
				int offset = 'A' - 'a';
				for (char curChar = 'a'; curChar <= 'z'; ++curChar)
					saveFont.AddChar(curChar, m_ActualCharBounds[(char)(curChar + offset)]);
			}
			
			
			saveFont.m_Name = System.IO.Path.GetFileNameWithoutExtension( saveFileDialog.FileName );
			saveFont.CharHeight = m_MaxCharHeight;
			uint resID = uint.Parse( textBoxResID.Text );
			saveFont.m_ImageResID = resID;

			saveFont.SaveFont(saveFileDialog.FileName);


			saveFileDialog.Title = "Save Bitmap...";
			saveFileDialog.Filter = "Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*";
			if (saveFileDialog.ShowDialog() != DialogResult.OK)
				return;

			Bitmap copyBitmap = new Bitmap( m_BufferBitmap.Width, m_BufferBitmap.Height, PixelFormat.Format32bppArgb);
			Graphics gfxDest = Graphics.FromImage( copyBitmap );

			SetGraphicsQuality( gfxDest, true );

			gfxDest.DrawImage( m_BufferBitmap, new Point());
			gfxDest.Dispose();
			copyBitmap.Save(saveFileDialog.FileName, ImageFormat.Bmp);
		}
	}
}