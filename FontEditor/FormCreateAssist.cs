using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace FontEditor
{
	/// <summary>
	/// Summary description for FormCreateAssist.
	/// </summary>
	public class FormCreateAssist : System.Windows.Forms.Form
	{
		private enum AssistSteps
		{
			CreateStrips,
			SetCharRects
		}

		/// <summary> The font image </summary>
		private Image m_FontImage = null;

		/// <summary> Required designer variable </summary>
		private System.ComponentModel.Container components = null;

		/// <summary> The current step of assisting we are on </summary>
		private AssistSteps m_CurStep = AssistSteps.CreateStrips;

		/// <summary> The current character we are setting </summary>
		private int m_CurUnicodeChar = PTFont.MIN_UNICODE_READABLE;

		/// <summary> The list of row/column coordinates </summary>
		private System.Collections.ArrayList m_GlyphStrips = new System.Collections.ArrayList();

		/// <summary> The index of the glyph strip we are dragging, -1 if no strip is
		/// moving </summary>
		private int m_DraggingStripIndex = -1;

		/// <summary> The last image Y-position while we were dragging a strip </summary>
		private int m_DraggingLastY = 0;

		/// <summary> Whether or not the user is dragging a rectangle for a glyph </summary>
		private bool m_IsDraggingGlyph = false;

		/// <summary> The rectangle of a glyph being created </summary>
		private Rectangle m_DraggingGlyphRect = new Rectangle();

		/// <summary> The list of added characters </summary>
		public System.Collections.SortedList m_AddedChars = new System.Collections.SortedList();

		const string SKIP_BUTTON_TEXT = "Skip Character: ";

		private System.Windows.Forms.GroupBox groupBoxFontImage;
		private System.Windows.Forms.GroupBox groupBoxTask;
		private System.Windows.Forms.Label labelTask;
		private System.Windows.Forms.Button buttonDone;
		private System.Windows.Forms.Button buttonCancel;
		private System.Windows.Forms.GroupBox groupBoxLetters;
		private System.Windows.Forms.Label labelFontChars;
		private System.Windows.Forms.TextBox textBoxCharList;
		private PictureBoxZoom.PictureBoxZoom pictureBoxZoomFontImage;
		private Button buttonPrevChar;
		private System.Windows.Forms.Button buttonNextSkip;


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> The default constructor </summary>
		/// <param name="fontImage"> The font image to use, must not be null </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public FormCreateAssist( Image fontImage )
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			// Store the font image
			m_FontImage = fontImage;
			if( m_FontImage == null )
				throw( new Exception( "Create assistant needs a vaild font image." ) );

			// Draw the image
			pictureBoxZoomFontImage.Image = m_FontImage;
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.groupBoxFontImage = new System.Windows.Forms.GroupBox();
			this.pictureBoxZoomFontImage = new PictureBoxZoom.PictureBoxZoom();
			this.groupBoxTask = new System.Windows.Forms.GroupBox();
			this.labelTask = new System.Windows.Forms.Label();
			this.buttonDone = new System.Windows.Forms.Button();
			this.buttonCancel = new System.Windows.Forms.Button();
			this.groupBoxLetters = new System.Windows.Forms.GroupBox();
			this.labelFontChars = new System.Windows.Forms.Label();
			this.buttonNextSkip = new System.Windows.Forms.Button();
			this.textBoxCharList = new System.Windows.Forms.TextBox();
			this.buttonPrevChar = new System.Windows.Forms.Button();
			this.groupBoxFontImage.SuspendLayout();
			this.groupBoxTask.SuspendLayout();
			this.groupBoxLetters.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBoxFontImage
			// 
			this.groupBoxFontImage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBoxFontImage.Controls.Add(this.pictureBoxZoomFontImage);
			this.groupBoxFontImage.Location = new System.Drawing.Point(232, 8);
			this.groupBoxFontImage.Name = "groupBoxFontImage";
			this.groupBoxFontImage.Size = new System.Drawing.Size(312, 344);
			this.groupBoxFontImage.TabIndex = 0;
			this.groupBoxFontImage.TabStop = false;
			this.groupBoxFontImage.Text = "Font Image";
			// 
			// pictureBoxZoomFontImage
			// 
			this.pictureBoxZoomFontImage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.pictureBoxZoomFontImage.Image = null;
			this.pictureBoxZoomFontImage.Location = new System.Drawing.Point(8, 16);
			this.pictureBoxZoomFontImage.Name = "pictureBoxZoomFontImage";
			this.pictureBoxZoomFontImage.Size = new System.Drawing.Size(296, 320);
			this.pictureBoxZoomFontImage.TabIndex = 1;
			this.pictureBoxZoomFontImage.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBoxZoomFontImage_CreateStrips_MouseDown);
			this.pictureBoxZoomFontImage.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBoxZoomFontImage_CreateStrips_MouseMove);
			this.pictureBoxZoomFontImage.picturePaint += new PictureBoxZoom.PictureBoxZoom.PicturePaintHandler(this.pictureBoxZoomFontImage_picturePaint);
			this.pictureBoxZoomFontImage.MouseLeave += new System.EventHandler(this.pictureBoxZoomFontImage_MouseLeave);
			this.pictureBoxZoomFontImage.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBoxZoomFontImage_MouseUp);
			// 
			// groupBoxTask
			// 
			this.groupBoxTask.Controls.Add(this.labelTask);
			this.groupBoxTask.Location = new System.Drawing.Point(8, 8);
			this.groupBoxTask.Name = "groupBoxTask";
			this.groupBoxTask.Size = new System.Drawing.Size(216, 104);
			this.groupBoxTask.TabIndex = 1;
			this.groupBoxTask.TabStop = false;
			this.groupBoxTask.Text = "Do This (Step 1 of 2)";
			// 
			// labelTask
			// 
			this.labelTask.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.labelTask.Location = new System.Drawing.Point(8, 24);
			this.labelTask.Name = "labelTask";
			this.labelTask.Size = new System.Drawing.Size(200, 72);
			this.labelTask.TabIndex = 0;
			this.labelTask.Text = "Click on the image to generate a new glyph strip or click+drag to move a glyph st" +
				"rip.  Do remove a strip simply drag it off the image.";
			// 
			// buttonDone
			// 
			this.buttonDone.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonDone.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.buttonDone.Enabled = false;
			this.buttonDone.Location = new System.Drawing.Point(8, 328);
			this.buttonDone.Name = "buttonDone";
			this.buttonDone.Size = new System.Drawing.Size(104, 24);
			this.buttonDone.TabIndex = 2;
			this.buttonDone.Text = "OK";
			// 
			// buttonCancel
			// 
			this.buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.buttonCancel.Location = new System.Drawing.Point(120, 328);
			this.buttonCancel.Name = "buttonCancel";
			this.buttonCancel.Size = new System.Drawing.Size(104, 24);
			this.buttonCancel.TabIndex = 3;
			this.buttonCancel.Text = "Cancel";
			// 
			// groupBoxLetters
			// 
			this.groupBoxLetters.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)));
			this.groupBoxLetters.Controls.Add(this.labelFontChars);
			this.groupBoxLetters.Location = new System.Drawing.Point(8, 220);
			this.groupBoxLetters.Name = "groupBoxLetters";
			this.groupBoxLetters.Size = new System.Drawing.Size(216, 100);
			this.groupBoxLetters.TabIndex = 4;
			this.groupBoxLetters.TabStop = false;
			this.groupBoxLetters.Text = "Characters To Add";
			// 
			// labelFontChars
			// 
			this.labelFontChars.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.labelFontChars.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelFontChars.Location = new System.Drawing.Point(8, 16);
			this.labelFontChars.Name = "labelFontChars";
			this.labelFontChars.Size = new System.Drawing.Size(200, 76);
			this.labelFontChars.TabIndex = 0;
			// 
			// buttonNextSkip
			// 
			this.buttonNextSkip.Enabled = false;
			this.buttonNextSkip.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonNextSkip.Location = new System.Drawing.Point(100, 120);
			this.buttonNextSkip.Name = "buttonNextSkip";
			this.buttonNextSkip.Size = new System.Drawing.Size(124, 50);
			this.buttonNextSkip.TabIndex = 5;
			this.buttonNextSkip.Text = "Next Step";
			this.buttonNextSkip.Click += new System.EventHandler(this.buttonNextSkip_Click);
			// 
			// textBoxCharList
			// 
			this.textBoxCharList.BackColor = System.Drawing.SystemColors.Window;
			this.textBoxCharList.Font = new System.Drawing.Font("Arial", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.textBoxCharList.ForeColor = System.Drawing.Color.Black;
			this.textBoxCharList.HideSelection = false;
			this.textBoxCharList.Location = new System.Drawing.Point(10, 180);
			this.textBoxCharList.Name = "textBoxCharList";
			this.textBoxCharList.ReadOnly = true;
			this.textBoxCharList.Size = new System.Drawing.Size(216, 29);
			this.textBoxCharList.TabIndex = 6;
			this.textBoxCharList.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.textBoxCharList.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBoxCharList_KeyPress);
			// 
			// buttonPrevChar
			// 
			this.buttonPrevChar.Enabled = false;
			this.buttonPrevChar.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonPrevChar.Location = new System.Drawing.Point(10, 120);
			this.buttonPrevChar.Name = "buttonPrevChar";
			this.buttonPrevChar.Size = new System.Drawing.Size(80, 50);
			this.buttonPrevChar.TabIndex = 7;
			this.buttonPrevChar.Text = "Back";
			this.buttonPrevChar.Click += new System.EventHandler(this.buttonPrevChar_Click);
			// 
			// FormCreateAssist
			// 
			this.AcceptButton = this.buttonDone;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.buttonCancel;
			this.ClientSize = new System.Drawing.Size(552, 358);
			this.Controls.Add(this.buttonPrevChar);
			this.Controls.Add(this.textBoxCharList);
			this.Controls.Add(this.buttonNextSkip);
			this.Controls.Add(this.groupBoxLetters);
			this.Controls.Add(this.buttonCancel);
			this.Controls.Add(this.buttonDone);
			this.Controls.Add(this.groupBoxTask);
			this.Controls.Add(this.groupBoxFontImage);
			this.KeyPreview = true;
			this.Name = "FormCreateAssist";
			this.ShowInTaskbar = false;
			this.Text = "Font Creation Assitant";
			this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.FormCreateAssist_KeyPress);
			this.groupBoxFontImage.ResumeLayout(false);
			this.groupBoxTask.ResumeLayout(false);
			this.groupBoxLetters.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}
		#endregion

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user presses the next or skip button
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void buttonNextSkip_Click(object sender, System.EventArgs e)
		{
			if( m_CurStep == AssistSteps.CreateStrips )
			{
				// Allow the user to finish
				buttonDone.Enabled = true;
				buttonPrevChar.Enabled = true;

				// Update the step text
				groupBoxTask.Text = "Do This (Step 2 of 2)";
				labelTask.Text = "Click and drag on a glyph row to set the glyph rectangle for the current character or press the skip button below to move to the next character.";
				textBoxCharList.BackColor = Color.FromArgb( 255, 192, 64 );

				// Use a different mouse movement and mouse down handler
				pictureBoxZoomFontImage.MouseMove -= new MouseEventHandler( pictureBoxZoomFontImage_CreateStrips_MouseMove );
				pictureBoxZoomFontImage.MouseMove += new MouseEventHandler( pictureBoxZoomFontImage_SetGlyphs_MouseMove );

				pictureBoxZoomFontImage.MouseDown -= new MouseEventHandler( pictureBoxZoomFontImage_CreateStrips_MouseDown );
				pictureBoxZoomFontImage.MouseDown += new MouseEventHandler( pictureBoxZoomFontImage_SetGlyphs_MouseDown );

				// Update the button text
				m_CurUnicodeChar = PTFont.MIN_UNICODE_READABLE - 1;
				IncrementCurCharacter();

				// Go to the next step
				m_CurStep = AssistSteps.SetCharRects;
			}
			// Else if we are setting character glyph rectangles
			else if( m_CurStep == AssistSteps.SetCharRects )
				IncrementCurCharacter();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user presses the back button
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void buttonPrevChar_Click(object sender, EventArgs e)
		{
			--m_CurUnicodeChar;

			// Update the button text
			buttonNextSkip.Text = SKIP_BUTTON_TEXT + (char)m_CurUnicodeChar;

			// Update the character list
			UpdateTextCharList();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Update the list of characters displaying what's coming up
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void UpdateTextCharList()
		{
			// Create the string for the edit box
			string charListString = "";
			const string SPACE_STR = "  ";
			int selCharIndex = 1 + SPACE_STR.Length;
			int startIndex = (int)m_CurUnicodeChar - 1;
			if (startIndex < PTFont.MIN_UNICODE_READABLE)
			{
				startIndex = PTFont.MIN_UNICODE_READABLE;
				selCharIndex = 0;
			}
			int endIndex = m_CurUnicodeChar + 8;
			if (endIndex > PTFont.MAX_UNICODE_READABLE)
				endIndex = PTFont.MAX_UNICODE_READABLE;
			for (int curCharIndex = startIndex; curCharIndex < endIndex; ++curCharIndex)
			{
				charListString += (char)curCharIndex;

				// If this is not the last character
				if (curCharIndex < (endIndex - 1))
					charListString += SPACE_STR;
			}

			// Update the text
			textBoxCharList.Text = charListString;

			// Select the current letter
			textBoxCharList.SelectionStart = selCharIndex;
			textBoxCharList.SelectionLength = 1;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the font image box is redrawn
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_picturePaint(object sender, System.Drawing.Graphics gfx )
		{
			// Create the strip brush
			int stripAlpha = 90;
			if( m_CurStep == AssistSteps.SetCharRects )
				stripAlpha = 40;
			Brush stripBrush = new SolidBrush( Color.FromArgb( stripAlpha, 0, 250, 0 ) );

			// Go through the glyph strips
			Rectangle stripRect = new Rectangle();
			stripRect.Width = m_FontImage.Width;
			stripRect.Height = FormMain.sm_Font.CharHeight;
			stripRect.X = 0;
			foreach( int glyphStripY in m_GlyphStrips )
			{
				// Set the top
				stripRect.Y = glyphStripY;

				// Draw the row
				gfx.FillRectangle( stripBrush, pictureBoxZoomFontImage.ImageToPictureBoxClient(stripRect) );
			}

			// If we are creating a glyph rectangle
			if( m_IsDraggingGlyph )
			{
				Brush glyphBrush = new SolidBrush( Color.FromArgb( 150, 0, 0, 250 ) );
				Rectangle fillRect = pictureBoxZoomFontImage.ImageToPictureBoxClient( m_DraggingGlyphRect );
				if( fillRect.Width < 0 )
				{
					fillRect.X = fillRect.X + fillRect.Width;
					fillRect.Width = (0 - fillRect.Width) + 1;
				}
				gfx.FillRectangle( glyphBrush, fillRect );
			}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user presses the mouse button down on the font image
		/// while they are setting the glyph strips
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_CreateStrips_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// We only handle left clicks
			if( e.Button != MouseButtons.Left )
				return;

			// Get the position of the cursor in image coordinates
			int mouseImgY = pictureBoxZoomFontImage.ClientToImage( new Point(e.X,e.Y) ).Y;

			// See if we are over an existing strip
			int glyphStripIndex = 0;
			for( glyphStripIndex = 0; glyphStripIndex < m_GlyphStrips.Count; ++glyphStripIndex )
			{
				// Get the Y value of this strip
				int glyphStripY = (int)m_GlyphStrips[ glyphStripIndex ];

				// If we are over this strip
				if( (mouseImgY > glyphStripY) && (mouseImgY < glyphStripY + FormMain.sm_Font.CharHeight) )
					break;
			}

			// If we clicked on a strip
			if( glyphStripIndex != m_GlyphStrips.Count )
			{
				// Store the index and mouse position and mark that we are dragging
				m_DraggingStripIndex = glyphStripIndex;
				m_DraggingLastY = mouseImgY;
			}
				// Else add the strip
			else
			{
				// Add a strip at the Y position
				int newGlyphStripIndex = m_GlyphStrips.Add( mouseImgY );

				// We created a strip so we can move on
				buttonNextSkip.Enabled = true;

				// Store the index and mouse position and mark that we are dragging
				m_DraggingStripIndex = newGlyphStripIndex;
				m_DraggingLastY = mouseImgY;

				// Redraw
				pictureBoxZoomFontImage.Refresh();
			}
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user presses the mouse button down on the font image
		/// while they are setting a glyph rectangle
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_SetGlyphs_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// If the left mouse button was pressed down
			if (e.Button == MouseButtons.Left)
			{
				// Get the mouse position in image coordinates
				Point mousePosImg = pictureBoxZoomFontImage.ClientToImage(new Point(e.X, e.Y));

				// If our cursor is not over a strip then bail
				int glyphStripIndex = IsOverGlyphStrip(mousePosImg);
				if (glyphStripIndex == -1)
					return;

				// Start creating a glyph
				m_IsDraggingGlyph = true;
				m_DraggingGlyphRect.X = mousePosImg.X;
				m_DraggingGlyphRect.Y = (int)m_GlyphStrips[glyphStripIndex];
				m_DraggingGlyphRect.Height = FormMain.sm_Font.CharHeight;
				m_DraggingGlyphRect.Width = 0;
			}
			// Else if the right mouse button was pressed
			else if (e.Button == MouseButtons.Right)
			{
				// If we are creating a glpyh then cancel it
				m_IsDraggingGlyph = false;
				
				// Redraw
				pictureBoxZoomFontImage.Refresh();
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user moves the mouse over the font image while we are
		/// creating the glyph strips
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_CreateStrips_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// Get the mouse position in image coordinates
			Point mousePosImg = pictureBoxZoomFontImage.ClientToImage( new Point(e.X,e.Y) );

			// If we are dragging a strip
			if( m_DraggingStripIndex != -1 )
			{
				// If we are leaving the paint box
				if( e.Y < 0 || e.Y > pictureBoxZoomFontImage.Height )
				{
					// Remove this drag strip and stop dragging
					m_GlyphStrips.RemoveAt( m_DraggingStripIndex );
					m_DraggingStripIndex = -1;
				}
					// Else we are still in the box
				else
				{
					// Move the strip
					int stripY = (int)m_GlyphStrips[ m_DraggingStripIndex ];
					stripY += mousePosImg.Y - m_DraggingLastY;
					m_GlyphStrips[ m_DraggingStripIndex ] = stripY;

					// Store the mouse position
					m_DraggingLastY = mousePosImg.Y;
				}

				// Redraw
				pictureBoxZoomFontImage.Refresh();
			}
				// Else see if we are over an existing strip
			else
			{
				// If we are over a strip
				if( IsOverGlyphStrip( mousePosImg ) != -1 )
					Cursor = Cursors.SizeNS;
					// Else we aren't
				else
					Cursor = Cursors.Default;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user moves the mouse over the font image when we are
		/// setting the rectangle for a glyph
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_SetGlyphs_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			Point mousePosImg = pictureBoxZoomFontImage.ClientToImage( new Point(e.X,e.Y) );
			
			// If we are creating a glyph
			if( m_IsDraggingGlyph )
			{				
				// Set the width of the rectangle
				m_DraggingGlyphRect.Width = mousePosImg.X - m_DraggingGlyphRect.X;

				// Redraw
				pictureBoxZoomFontImage.Refresh();
			}
				// Else if we are over a glyph strip
			else
			{
				// If we are over a strip
				if( IsOverGlyphStrip( mousePosImg ) != -1 )
					Cursor = Cursors.IBeam;
				else
					Cursor = Cursors.Default;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Get the index of the glyph strip at the passed in coordinates </summary>
		/// <param name="fontImgPos"> The image coordinates to test </param>
		/// <returns> If the mouse is over a glyph strip then the return value is the index in the
		/// glyph strip array, otherwise -1 is returned </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private int IsOverGlyphStrip( Point fontImgPos )
		{
			// Go through each glyph strip and see if we are over one
			for( int glyphStripIndex = 0; glyphStripIndex < m_GlyphStrips.Count; ++glyphStripIndex )
			{
				// Get the strip Y value
				int glyphStripY = (int)m_GlyphStrips[ glyphStripIndex ];

				// If we are over this strip
				if( (fontImgPos.Y > glyphStripY)
					&& (fontImgPos.Y < glyphStripY + FormMain.sm_Font.CharHeight) )
					return glyphStripIndex;
			}

			// If we got here then we are not over a glyph strip
			return -1;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Step to the next character to set the glyph for
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void IncrementCurCharacter()
		{
			++m_CurUnicodeChar;

			// Update the button text
			buttonNextSkip.Text = SKIP_BUTTON_TEXT + (char)m_CurUnicodeChar;

			// Update the character list
			UpdateTextCharList();
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user releases the mouse button over the font image
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// We are only handling left click right now
			if( e.Button != MouseButtons.Left )
				return;
			
			// If we are setting a glyph
			if( m_IsDraggingGlyph )
			{
				// Make sure the rectangle has positive values to be safe
				if( m_DraggingGlyphRect.Width < 0 )
				{
					m_DraggingGlyphRect.X = m_DraggingGlyphRect.X + m_DraggingGlyphRect.Width;
					m_DraggingGlyphRect.Width = (0 - m_DraggingGlyphRect.Width) + 1;
				}

				// Store the rectangle
				char curChar = (char)m_CurUnicodeChar;
				if (m_AddedChars.Contains(curChar))
					m_AddedChars[curChar] = m_DraggingGlyphRect;
				else
				{
					m_AddedChars.Add(curChar, m_DraggingGlyphRect);
					labelFontChars.Text += curChar;
				}
				m_IsDraggingGlyph = false;

				// Step to the next character
				IncrementCurCharacter();				
			}

			// We can't be dragging anymore
			m_DraggingStripIndex = -1;
			m_IsDraggingGlyph = false;

			// Redraw
			pictureBoxZoomFontImage.Refresh();
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user moves the mouse cursor off the paint box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_MouseLeave(object sender, System.EventArgs e)
		{
			// If we are dragging
			if( m_DraggingStripIndex != -1 )
			{
				// Remove this drag strip and stop dragging
				m_GlyphStrips.RemoveAt( m_DraggingStripIndex );
				m_DraggingStripIndex = -1;

				// Redraw
				pictureBoxZoomFontImage.Refresh();
			}

			// Reset the mouse cursor
			Cursor = Cursors.Default;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user presses a key on the character preview text box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxCharList_KeyPress(object sender, KeyPressEventArgs e)
		{
			//m_CurUnicodeChar = e.KeyChar;

			//UpdateTextCharList();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user presses a key
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void FormCreateAssist_KeyPress(object sender, KeyPressEventArgs e)
		{
			// Only do this if we are setting character rectangles
			if (m_CurStep == AssistSteps.SetCharRects)
			{
				m_CurUnicodeChar = e.KeyChar;
				UpdateTextCharList();
			}
		}
	}
}
