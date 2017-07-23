using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using TCUtil;
using System.IO;

namespace SpriteCreator
{
	public partial class FormMain : Form
	{
		public enum AnimPlayType : int
		{
			NoLoop = 0,
			Loop,
			PingPong
		}

		public class AnimFrame
		{
			/// <summary> The frame delay in milliseconds </summary>
			public int frameDelay = 10;

			/// <summary> The coordinates on the source image to find this frame's graphics data </summary>
			public Rectangle sourceRect = new Rectangle();

			internal void TransferData(ISimpleSerializer serializer)
			{
				int tempValue = sourceRect.X;
				serializer.AddData(ref tempValue);
				sourceRect.X = tempValue;

				tempValue = sourceRect.Y;
				serializer.AddData(ref tempValue);
				sourceRect.Y = tempValue;

				tempValue = sourceRect.Width;
				serializer.AddData(ref tempValue);
				sourceRect.Width = tempValue;

				tempValue = sourceRect.Height;
				serializer.AddData(ref tempValue);
				sourceRect.Height = tempValue;

				// Write out the frame delay
				serializer.AddData(ref frameDelay);
			}
		}

		/// <summary> The current version of the sprite data files </summary>
		private const int SPRITE_FILE_VER = 1;

		/// <summary> The currently loaded image </summary>
		public Image m_SrcImage = null;

		/// <summary> The start position of the cursor when dragging a frame's rectangle </summary>
		private Rectangle m_DragRect = new Rectangle();

		/// <summary> Whether or not we are dragging a rectangle </summary>
		private bool m_IsDraggingCreateRect = false;

		/// <summary> If we are dragging a frame around the image </summary>
		private bool m_IsDraggingFrameRect = false;

		/// <summary>
		/// If the user is dragging a frame then this is the offset from the top left of the
		/// frame, in image coordinates
		/// </summary>
		private Point m_DragFrameMouseOffset = new Point();

		/// <summary> The list of animation frames </summary>
		private List<AnimFrame> m_AnimFrames = new List<AnimFrame>();

		/// <summary> The currently loaded file, empty if no file name is associated with the file </summary>
		private string _curFileName = "";

		/// <summary> The title bar text when the application is first run </summary>
		private string m_sOrigTitle = "";

		/// <summary> The source image resource ID </summary>
		private uint m_SourceImageResID = 0;

		/// <summary> The animation preview window </summary>
		FormAnimPreview m_AnimPreviewForm = new FormAnimPreview();


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The default constructor
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public FormMain()
		{
			InitializeComponent();

			Padding frameListPadding = listViewFrames.Padding;
			
			// Set the image list size based on the list view size
			Size imageListImageSize = new Size();
			imageListImageSize.Width = listViewFrames.ClientRectangle.Width - 64;
			imageListImageSize.Height = 48;
			imageListFrames.ImageSize = imageListImageSize;

			// Hook up the image list
			listViewFrames.LargeImageList = imageListFrames;

			// Create the animation preview window
			m_AnimPreviewForm.Show(this);

			// Store the title bar text
			m_sOrigTitle = Text;

			// Cap the name length
			textBoxSpriteName.MaxLength = ResourceCS.ResourceToolsDB.REZ_NAME_LEN - 1;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add a frame to the animation
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void AddNewFrame()
		{
			// Create a new image
			Bitmap newFrameImage = new Bitmap(imageListFrames.ImageSize.Width, imageListFrames.ImageSize.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
			
			// Fill it in
			Rectangle imageRect = new Rectangle( 0, 0, imageListFrames.ImageSize.Width, imageListFrames.ImageSize.Height );
			Graphics bitmapGfx = Graphics.FromImage(newFrameImage);
			bitmapGfx.FillRectangle(new SolidBrush(Color.Black), imageRect);

			// Create the string formatting object to center the text
			StringFormat strFormat = new StringFormat();
			strFormat.LineAlignment = StringAlignment.Center;
			strFormat.Alignment = StringAlignment.Center;

			// Draw the text
			RectangleF imageFRect = new RectangleF(0, 0, imageListFrames.ImageSize.Width, imageListFrames.ImageSize.Height);
			bitmapGfx.DrawString("EMPTY", new Font("Arial", 12), new SolidBrush(Color.White), imageFRect, strFormat);
			bitmapGfx.Dispose();

			// Add the image to the list
			imageListFrames.Images.Add(newFrameImage);

			// Add the new list item
			ListViewItem newListItem = listViewFrames.Items.Add("Frame", imageListFrames.Images.Count - 1);

			// Create the animation frame object
			AnimFrame newFrame = new AnimFrame();
			if (checkBoxLinkDelays.Checked && m_AnimFrames.Count > 0)
				newFrame.frameDelay = m_AnimFrames[0].frameDelay;
			m_AnimFrames.Add(newFrame);

			// Update the preview window
			m_AnimPreviewForm.AnimList = m_AnimFrames;

			// If we have more than 1 frame
			if (m_AnimFrames.Count > 1)
			{
				// Ensure the frame delayed and animation play type can be editted
				textBoxFrameDelay.Enabled = true;
				radioButtonPlayOnce.Enabled = true;
				radioButtonLoop.Enabled = true;
				radioButtonPingPong.Enabled = true;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Remove a frame of animation from the sprite
		/// </summary>
		/// <param name="frameIndex"> The index of the frame to remove </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void RemoveFrame(int frameIndex)
		{
			// If we have an invalid frame index then bail
			if (frameIndex < 0 || frameIndex >= m_AnimFrames.Count)
				return;

			// Remove the frame
			listViewFrames.Items.RemoveAt(frameIndex);
			m_AnimFrames.RemoveAt(frameIndex);
			m_AnimPreviewForm.AnimList = m_AnimFrames;

			// If we now only have one frame
			if (m_AnimFrames.Count == 1)
			{
				// Disable the delay text box and animation play type radio buttons
				textBoxFrameDelay.Enabled = false;
				radioButtonPlayOnce.Enabled = false;
				radioButtonLoop.Enabled = false;
				radioButtonPingPong.Enabled = false;
			}

			// Redraw
			Refresh();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user presses the "Add Frame" button
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void buttonAddFrame_Click(object sender, EventArgs e)
		{
			AddNewFrame();

			// Select the new item
			if (listViewFrames.SelectedItems.Count > 0)
				listViewFrames.SelectedItems[0].Selected = false;
			listViewFrames.Items[listViewFrames.Items.Count - 1].Selected = true;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user presses the mouse mouse down over the image
		/// picture box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomImage_MouseDown(object sender, MouseEventArgs e)
		{
			// Get the mouse position in image coordinates
			Point cursorPos = pictureBoxZoomImage.ClientToImage(new Point(e.X, e.Y));
				
			// If the left mouse button was pressed
			if (e.Button == MouseButtons.Left)
			{
				// If a frame is selected
				if( SelectedFrameIndex != -1 )
				{
					AnimFrame curFrame = m_AnimFrames[ SelectedFrameIndex ];
					if (curFrame.sourceRect.Contains(cursorPos))
					{
						// We are moving a frame
						m_IsDraggingFrameRect = true;
						m_DragFrameMouseOffset = new Point(cursorPos.X - curFrame.sourceRect.X, cursorPos.Y - curFrame.sourceRect.Y);

						// We are moving a frame so we are done
						return;
					}

					// Get the image coordinates and start creating the rectangle
					m_DragRect = new Rectangle(cursorPos.X, cursorPos.Y, 0, 0);
					m_IsDraggingCreateRect = true;
				}				
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user releases the mouse button over the image
		/// picture box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomImage_MouseUp(object sender, MouseEventArgs e)
		{
			// If we are drag-creating a rectangle
			if (m_IsDraggingCreateRect && e.Button == MouseButtons.Left)
			{
				m_IsDraggingCreateRect = false;

				// Store the end position
				Point endImagePos = pictureBoxZoomImage.ClientToImage(new Point(e.X, e.Y));
				m_DragRect.Width = endImagePos.X - m_DragRect.X;
				m_DragRect.Height = endImagePos.Y - m_DragRect.Y;

				// Get the selected frame index
				if (SelectedFrameIndex != -1)
				{
					// Store the data
					m_AnimFrames[SelectedFrameIndex].sourceRect = m_DragRect;

					// Update the preview window
					m_AnimPreviewForm.AnimList = m_AnimFrames;

					// Update the frame's list box image
					UpdateFrameImageList(SelectedFrameIndex, m_DragRect);

					// Update the list view, text boxes, and picture
					listViewFrames.Refresh();
					UpdateFrameTextBoxes();
					pictureBoxZoomImage.Refresh();
				}
			}
			// Else if we are done moving a frame
			else if (m_IsDraggingFrameRect)
			{
				m_IsDraggingFrameRect = false;

				// Update the preview window
				m_AnimPreviewForm.AnimList = m_AnimFrames;

				// Update the frame's list box image
				UpdateFrameImageList(SelectedFrameIndex, m_AnimFrames[SelectedFrameIndex].sourceRect);
		
				// Update the list view, text boxes, and picture
				listViewFrames.Refresh();
				UpdateFrameTextBoxes();
				pictureBoxZoomImage.Refresh();
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Update a frame list view image list image from a new selection
		/// </summary>
		/// <param name="frameIndex"> The index of the frame to update </param>
		/// <param name="srcRect"> The source rectangle on the image </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void UpdateFrameImageList(int frameIndex, Rectangle srcRect)
		{
			// Create the new image
			Bitmap newFrameImage = new Bitmap(imageListFrames.ImageSize.Width, imageListFrames.ImageSize.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
			
			// If we have a source image
			if (m_SrcImage != null)
			{
				Graphics destGfx = Graphics.FromImage(newFrameImage);

				// If we are larger than the image list size then scale down
				Rectangle destRect = new Rectangle(0, 0, srcRect.Width, srcRect.Height);
				if (srcRect.Width < imageListFrames.ImageSize.Width)
					destRect.X = (imageListFrames.ImageSize.Width - srcRect.Width) / 2;
				if (srcRect.Height < imageListFrames.ImageSize.Height)
					destRect.Y = (imageListFrames.ImageSize.Height - srcRect.Height) / 2;

				// Copy the image data
				destGfx.DrawImage(m_SrcImage, destRect, srcRect, GraphicsUnit.Pixel);
				destGfx.Dispose();
			}

			// Store the image
			imageListFrames.Images[frameIndex] = newFrameImage;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user moves the mouse over the image
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomImage_MouseMove(object sender, MouseEventArgs e)
		{
			Point cursorPos = pictureBoxZoomImage.ClientToImage(new Point(e.X, e.Y));

			// If we are dragging a frame rectangle
			if (m_IsDraggingCreateRect)
			{
				// Store the current point
				m_DragRect.Width = cursorPos.X - m_DragRect.X;
				m_DragRect.Height = cursorPos.Y - m_DragRect.Y;

				// Redraw
				pictureBoxZoomImage.Refresh();
			}
			// Else if we are moving a frame
			else if (m_IsDraggingFrameRect)
			{
				AnimFrame curFrame = m_AnimFrames[SelectedFrameIndex];
				curFrame.sourceRect.X = cursorPos.X - m_DragFrameMouseOffset.X;
				curFrame.sourceRect.Y = cursorPos.Y - m_DragFrameMouseOffset.Y;

				// Clamp the values
				if (curFrame.sourceRect.X < 0)
					curFrame.sourceRect.X = 0;
				else if (curFrame.sourceRect.Right >= m_SrcImage.Width)
					curFrame.sourceRect.X = m_SrcImage.Width - curFrame.sourceRect.Width;

				if (curFrame.sourceRect.Y < 0)
					curFrame.sourceRect.Y = 0;
				else if (curFrame.sourceRect.Bottom >= m_SrcImage.Height)
					curFrame.sourceRect.Y = m_SrcImage.Height - curFrame.sourceRect.Height;

				// Redraw
				pictureBoxZoomImage.Refresh();
			}
			// Else if the user is over the selected frame
			else if (SelectedFrameIndex != -1)
			{
				if (m_AnimFrames[SelectedFrameIndex].sourceRect.Contains(cursorPos))
					Cursor.Current = Cursors.SizeAll;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the currently selected frame index.  Will return -1 if no valid frame is selected
		/// or will return the guaranteed-safe selected frame index
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private int SelectedFrameIndex
		{
			get
			{
				// If we don't have any frames selected then bail
				if (listViewFrames.SelectedIndices.Count < 1)
					return -1;
				
				// Get the last selected index
				int retIndex = listViewFrames.SelectedIndices[listViewFrames.SelectedIndices.Count - 1];

				// If the index is out of range then return -1
				if (retIndex >= m_AnimFrames.Count)
					return -1;

				// Return the selected frame index
				return retIndex;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get or set the source image for the animation frames
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private Image SourceImage
		{
			set
			{
				m_SrcImage = value;

				// Go through the image list images
				for (int frameIndex = 0; frameIndex < m_AnimFrames.Count; ++frameIndex)
					UpdateFrameImageList(frameIndex, m_AnimFrames[frameIndex].sourceRect);

				// Update the preview window
				m_AnimPreviewForm.SourceImage = m_SrcImage;

				// Update the picture box
				pictureBoxZoomImage.Image = m_SrcImage;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the image zoom paint box is repainted
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomImage_picturePaint(object sender, Graphics g)
		{
			// If we are creating a frame
			if (m_IsDraggingCreateRect)
			{
				Rectangle drawRect = pictureBoxZoomImage.ImageToPictureBoxClient(m_DragRect);
				g.DrawRectangle(new Pen(Color.Red), drawRect);
			}
			else
			{
				// Get the selection index
				if (SelectedFrameIndex != -1)
				{
					Rectangle drawRect = pictureBoxZoomImage.ImageToPictureBoxClient(m_AnimFrames[SelectedFrameIndex].sourceRect);
					g.DrawRectangle(new Pen(Color.Black), drawRect );
				}
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the selected index for the frame list control changes
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void listViewFrames_SelectedIndexChanged(object sender, EventArgs e)
		{
			// If we have a valid selected frame
			if( SelectedFrameIndex != -1 )
				UpdateFrameTextBoxes();

			// Redraw the picture with the selection rectangles
			pictureBoxZoomImage.Refresh();
		}


		private void UpdateFrameTextBoxes()
		{
			// If there is no frame selected
			if (SelectedFrameIndex == -1)
			{
				textBoxXPos.Text = "";
				textBoxYPos.Text = "";
				textBoxFrameWidth.Text = "";
				textBoxFrameHeight.Text = "";
				textBoxFrameDelay.Text = "";
				return;
			}

			// Prevent the text box text changing from raising events
			textBoxXPos.TextChanged -= textBoxXPos_TextChanged;
			textBoxYPos.TextChanged -= textBoxYPos_TextChanged;
			textBoxFrameWidth.TextChanged -= textBoxFrameWidth_TextChanged;
			textBoxFrameHeight.TextChanged -= textBoxFrameHeight_TextChanged;

			// Update the text boxes
			AnimFrame curFrame = m_AnimFrames[SelectedFrameIndex];
			textBoxXPos.Text = curFrame.sourceRect.X.ToString();
			textBoxYPos.Text = curFrame.sourceRect.Y.ToString();
			textBoxFrameWidth.Text = curFrame.sourceRect.Width.ToString();
			textBoxFrameHeight.Text = curFrame.sourceRect.Height.ToString();
			textBoxFrameDelay.Text = curFrame.frameDelay.ToString();

			// Allow updating again
			textBoxXPos.TextChanged += textBoxXPos_TextChanged;
			textBoxYPos.TextChanged += textBoxYPos_TextChanged;
			textBoxFrameWidth.TextChanged += textBoxFrameWidth_TextChanged;
			textBoxFrameHeight.TextChanged += textBoxFrameHeight_TextChanged;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if the current data can be saved to file
		/// </summary>
		/// <returns> True if the sprite is ready for saving, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private bool CanSaveSprite()
		{
			// If there is no data then bail
			if (m_AnimFrames.Count == 0)
			{
				MessageBox.Show("Cannot save sprite file that has no frames.", m_sOrigTitle);
				return false;
			}

			// Ensure the sprite has a name
			if (textBoxSpriteName.Text.Length == 0)
			{
				MessageBox.Show("Please name the sprite.", m_sOrigTitle);
				return false;
			}

			// If no source image is chosen
			if (m_SourceImageResID == 0)
			{
				MessageBox.Show("Select a source image for the sprite.", m_sOrigTitle);
				return false;
			}

			// The file can be saved
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks the 'Save' menu item under 'File'
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuFileSave_Click(object sender, EventArgs e)
		{
			if (!CanSaveSprite())
				return;

			// If we don't have a current file name then act like we clicked "Save As..."
			if ( String.IsNullOrEmpty(_curFileName) )
				menuFileSaveAs_Click(sender, e);
			// Else save the file
			else
				SaveSpriteFile(_curFileName);
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks the 'Save As' menu item under 'File'
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuFileSaveAs_Click(object sender, EventArgs e)
		{
			if (!CanSaveSprite())
				return;

			// Create the save file dialog
			SaveFileDialog saveDialog = new SaveFileDialog();
			saveDialog.Filter = "Sprite File (*.spr)|*.spr|All Files (*.*)|*.*";
			//saveDlg.InitialDirectory = sResPath;

			if (String.IsNullOrEmpty( _curFileName ))
			{
				ResourceCS.ResourceToolsDB resDB = new ResourceCS.ResourceToolsDB();
				if (resDB.LoadToolsIndex( textBoxResourceIndexPath.Text ))
					saveDialog.InitialDirectory = resDB.RootResourcePath;
			}

			// If the user failed to select a file name
			if (saveDialog.ShowDialog() != DialogResult.OK)
				return;

			// Attempt to save the file and, if successful, store the file name
			if (SaveSpriteFile( saveDialog.FileName ))
			{
				// Store the file name and update the window text
				_curFileName = saveDialog.FileName;
				Text = this.m_sOrigTitle + " - " + System.IO.Path.GetFileName(_curFileName);
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks the 'Open' menu item under 'File'
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuFileOpen_Click(object sender, EventArgs e)
		{
			// Create the open file dialog
			OpenFileDialog openDlg = new OpenFileDialog();
			openDlg.Filter = "Sprite File (*.spr)|*.spr|All Files (*.*)|*.*";

			// If the user did not pick a file then bail
			if (openDlg.ShowDialog() != DialogResult.OK)
				return;

			// Clear all of the data
			SetSpriteImage(0);
			listViewFrames.Clear();
			m_AnimFrames.Clear();
			textBoxSpriteName.Text = "";
			UpdateFrameTextBoxes();

			// Attempt to load the file and, if successful, store the file name
			if (LoadSpriteFile(openDlg.FileName))
			{
				// Store the file name and update the window text
				_curFileName = openDlg.FileName;
				Text = this.m_sOrigTitle + " - " + System.IO.Path.GetFileName(_curFileName);
			}

			// Update the preview window
			m_AnimPreviewForm.AnimList = m_AnimFrames;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks the 'New' menu item under 'File'
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuFileNew_Click(object sender, EventArgs e)
		{
			// Clear all of the data
			SetSpriteImage(0);
			listViewFrames.Clear();
			m_AnimFrames.Clear();
			textBoxSpriteName.Text = "";
			UpdateFrameTextBoxes();

			// Disable the multi-frame animation related data
			textBoxFrameDelay.Enabled = false;
			radioButtonPlayOnce.Enabled = false;
			radioButtonLoop.Enabled = false;
			radioButtonPingPong.Enabled = false;

			// Redraw the dialog
			Refresh();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks the 'Close' menu item under 'File'
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuFileExit_Click(object sender, EventArgs e)
		{
			// Close the dialog
			Close();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Save the current animation data to file
		/// </summary>
		/// <param name="sFileName"> The name of the file to which we are saving </param>
		/// <returns> Whether or not the sprite was saved successfully </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private bool SaveSpriteFile(string sFileName)
		{
			try
			{
				// Open/create the file
				System.IO.FileStream outFile = new System.IO.FileStream(sFileName, System.IO.FileMode.Create, System.IO.FileAccess.Write);

				ISimpleSerializer serializer = new NetSafeSimpleSerializer( new System.IO.BinaryWriter( outFile, Encoding.Unicode ) );

				// Write out the game resource file key
				int resFourCC = ResourceCS.ResourceToolsDB.REZ_FOURCC.ToInt32();
				serializer.AddData(ref resFourCC);

				// Write out the type of resource, sprite in this case
				int resType = (int)ResourceCS.ResourceType.Sprite;
				serializer.AddData(ref resType);

				// Write out the name of the sprite
				char[] spriteName = new char[ResourceCS.ResourceToolsDB.REZ_NAME_LEN];
				for (int charIndex = 0; charIndex < spriteName.Length; ++charIndex)
					spriteName[charIndex] = '\x0000';
				textBoxSpriteName.Text.CopyTo(0, spriteName, 0, textBoxSpriteName.Text.Length);
				//string spriteName = textBoxSpriteName.Text;
				serializer.AddData(ref spriteName);

				// Write out the file version
				int fileVersion = SPRITE_FILE_VER;
				serializer.AddData( ref fileVersion );

				TransferSpriteFileV1( serializer );

				// Close the file
				outFile.Close();
			}
			catch (Exception exc)
			{
				MessageBox.Show("Failed to save file: " + exc.Message, m_sOrigTitle );
				return false;
			}

			// Return success
			return true;
		}


		// Used to convert the sprites in PC byte-order to network byte-order
		//private void SaveLoadAllSprites()
		//{
		//    const string RootPath = @"C:\Taylor\Programming Projects\PrimeTimeCode\Final Resource\";
		//    DirectoryInfo rootDir = new DirectoryInfo(RootPath);

		//    FileInfo[] spriteFiles = rootDir.GetFiles( "*.spr", SearchOption.AllDirectories );

		//    foreach( FileInfo curSpriteFile in spriteFiles )
		//    {
		//        LoadSpriteFile( curSpriteFile.FullName );

		//        string subDir = curSpriteFile.DirectoryName.Substring( RootPath.Length );
		//        string fullDir = "C:\\NewSprites\\" + subDir + "\\";
		//        if( !Directory.Exists( fullDir ) )
		//            Directory.CreateDirectory( fullDir );

		//        SaveSpriteFile( fullDir + curSpriteFile.Name );
		//    }
		//}

		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Load a sprite file
		/// </summary>
		/// <param name="sFileName"> The name of the file to load </param>
		/// <returns> Whether or not the sprite was loaded successfully </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private bool LoadSpriteFile(string sFileName)
		{
			try
			{
				// Open the file
				System.IO.FileStream inFile = new System.IO.FileStream(sFileName, System.IO.FileMode.Open, System.IO.FileAccess.Read);
				
				ISimpleSerializer serializer = new NetSafeSimpleSerializer(new System.IO.BinaryReader(inFile, Encoding.Unicode));

				// Read in the FourCC key
				int fourCCVal = 0;
				serializer.AddData(ref fourCCVal);
				TCBase.FourCC fileFourCC = new TCBase.FourCC(fourCCVal);
				if (fileFourCC != ResourceCS.ResourceToolsDB.REZ_FOURCC)
				{
					inFile.Close();
					throw new Exception("File has invalid opening FourCC key: " + fileFourCC.ToString());
				}

				// Read in resource type, it better be sprite
				int tempResType = 0;
				serializer.AddData(ref tempResType);
				ResourceCS.ResourceType resType = (ResourceCS.ResourceType)tempResType;
				if (resType != ResourceCS.ResourceType.Sprite)
				{
					inFile.Close();
					throw new Exception("File has not a sprite, read in as: " + resType.ToString());
				}

				// Read in the name of the sprite
				char[] spriteName = new char[ ResourceCS.ResourceToolsDB.REZ_NAME_LEN ];
				serializer.AddData(ref spriteName);
				textBoxSpriteName.Text = new string( spriteName );

				// Read in the sprite file version
				int fileVer = -1;
				serializer.AddData(ref fileVer);
				
				if( fileVer == 1 )
					TransferSpriteFileV1( serializer );

				// Close the file
				inFile.Close();
			}
			catch (Exception exc)
			{
				MessageBox.Show("Failed to load file: " + exc.Message, m_sOrigTitle);
				return false;
			}

			// Resize the image list and determine if we should link the frame delays
			imageListFrames.Images.Clear();
			int testFrameDelay = -1;
			bool linkFrameDelays = true;
			for( int curFrameIndex = 0; curFrameIndex < m_AnimFrames.Count; ++curFrameIndex )
			{
				// Test if this frame's delay matches the other frames
				if( testFrameDelay != -1 )
					linkFrameDelays &= ( m_AnimFrames[curFrameIndex].frameDelay == testFrameDelay );
				else
					testFrameDelay = m_AnimFrames[curFrameIndex].frameDelay;

				imageListFrames.Images.Add( new Bitmap( imageListFrames.ImageSize.Width, imageListFrames.ImageSize.Height ) );
			}

			// If we should link the frame delays because they're all the same
			if( linkFrameDelays )
			{
				checkBoxLinkDelays.CheckedChanged -= checkBoxLinkDelays_CheckedChanged;
				checkBoxLinkDelays.Checked = true;
				checkBoxLinkDelays.CheckedChanged += checkBoxLinkDelays_CheckedChanged;
			}

			// Load the image
			SetSpriteImage(m_SourceImageResID);
			
			// Update the list box with our animation data
			{
				int curFrameIndex = 0;
				foreach (AnimFrame curFrame in m_AnimFrames)
				{
					listViewFrames.Items.Add("Frame " + curFrameIndex.ToString(), curFrameIndex);
					UpdateFrameImageList(curFrameIndex++, curFrame.sourceRect);
				}
			}

			// Return success
			return true;
		}

		private void TransferSpriteFileV1(ISimpleSerializer serializer)
		{
			// Read in the source image resource ID
			serializer.AddData( ref m_SourceImageResID );

			// Read in the number of frames
			int numFrames = m_AnimFrames.Count;
			serializer.AddData(ref numFrames);

			// Clear our current frames if we are loading
			if( serializer.InReadMode )
				m_AnimFrames.Clear();

			// If we have one frame
			if( numFrames == 1 )
			{
				// Read in the source rectangle and store the data
				AnimFrame singleFrame;
				if( serializer.InReadMode )
					singleFrame = new AnimFrame();
				else
					singleFrame = m_AnimFrames[0];

				int tempValue = singleFrame.sourceRect.X;
				serializer.AddData(ref tempValue);
				singleFrame.sourceRect.X = tempValue;

				tempValue = singleFrame.sourceRect.Y;
				serializer.AddData(ref tempValue);
				singleFrame.sourceRect.Y = tempValue;

				tempValue = singleFrame.sourceRect.Width;
				serializer.AddData(ref tempValue);
				singleFrame.sourceRect.Width = tempValue;

				tempValue = singleFrame.sourceRect.Height;
				serializer.AddData(ref tempValue);
				singleFrame.sourceRect.Height = tempValue;

				if( serializer.InReadMode )
				{
					m_AnimFrames.Add( singleFrame );

					// Ensure the frame delayed and animation play type can not be modified
					textBoxFrameDelay.Enabled = false;
					radioButtonPlayOnce.Enabled = false;
					radioButtonLoop.Enabled = false;
					radioButtonPingPong.Enabled = false;
				}
			}
			// Else it is an animated sprite
			else
			{
				// Read in the animation type
				int tempPlayType = (int)m_AnimPreviewForm.AnimPlayType;
				serializer.AddData(ref tempPlayType);
				AnimPlayType playType = (AnimPlayType)tempPlayType;

				if( serializer.InReadMode )
				{
					// Ensure the frame delayed and animation play type can be modified
					textBoxFrameDelay.Enabled = true;
					radioButtonPlayOnce.Enabled = true;
					radioButtonLoop.Enabled = true;
					radioButtonPingPong.Enabled = true;

					// Update the radio buttons
					if( playType == AnimPlayType.Loop )
						radioButtonLoop.Checked = true;
					else if( playType == AnimPlayType.PingPong )
						radioButtonPingPong.Checked = true;
					else
						radioButtonPlayOnce.Checked = true;
				}

				// Transfer the frames				
				for( int curFrameIndex = 0; curFrameIndex < numFrames; ++curFrameIndex )
				{
					// Read in the source rectangle and store the data
					AnimFrame currentFrame;
					if( serializer.InReadMode )
						currentFrame = new AnimFrame();
					else
						currentFrame = m_AnimFrames[curFrameIndex];

					currentFrame.TransferData( serializer );

					// Store the frame
					if( serializer.InReadMode )
						m_AnimFrames.Add( currentFrame );
				}
			}
		}

		[System.Runtime.InteropServices.DllImport( "ResPathDlg_dbg.dll" )]
		public static extern bool RunResChooserDlg_Filter( IntPtr hWnd, [MarshalAs( UnmanagedType.LPWStr)] string szRootResPath, ref uint resID, int typeFilter);


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user chooses to load an image for use by the sprite
		/// from the Animation menu
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuAnimLoadImage_Click(object sender, EventArgs e)
		{
			// Allow the user to choose a resource
			uint selResID = 0;
			if (!RunResChooserDlg_Filter( Handle, textBoxResourceIndexPath.Text, ref selResID, (int)ResourceCS.ResourceType.Image ))
				return;

			// Set the sprite image
			SetSpriteImage(selResID);
		}


		private void SetSpriteImage(uint resID)
		{
			textBoxImageRef.Text = String.Empty;

			// If the ID is 0, then clear the image
			if (resID == 0)
			{
				// Clear the data
				if (m_SrcImage != null )
					m_SrcImage.Dispose();
				m_SrcImage = null;
				m_SourceImageResID = 0;
				m_AnimPreviewForm.SourceImage = null;
				pictureBoxZoomImage.Image = null;	

				// Bail
				return;
			}

			// Load the resource database
			ResourceCS.ResourceToolsDB resDB = new ResourceCS.ResourceToolsDB();
			if (!resDB.LoadToolsIndex( textBoxResourceIndexPath.Text ))
			{
				string sMsg = "Failed to load tools resource index file (";
				sMsg += textBoxResourceIndexPath.Text;
				sMsg += ")";
				MessageBox.Show(sMsg, "Resource Chooser");
				return;
			}

			// Get the resource info
			ResourceCS.ResourceToolsDB.ResourceIndexItem resInfo = new ResourceCS.ResourceToolsDB.ResourceIndexItem();
			try
			{
				resInfo = resDB.GetResourceInfo(resID);
			}
			catch (Exception exc)
			{
				MessageBox.Show(exc.Message, Text);
			}

			// If the file is not an image then bail
			if (resInfo.type != ResourceCS.ResourceType.Image)
			{
				MessageBox.Show("The selected resource is not an image, please re-choose.", Text, MessageBoxButtons.OK, MessageBoxIcon.Hand);
				return;
			}

			// Store the resource ID
			m_SourceImageResID = resID;

			textBoxImageRef.Text = resID.ToString() + " - " + resInfo.sFileName;

			try
			{
				// Get the file name
				string sResImgFile = resDB.RootResourcePath;
				sResImgFile += resInfo.sFileName;

				// Load the image
				if (m_SrcImage != null)
					m_SrcImage.Dispose();
				m_SrcImage = Image.FromFile(sResImgFile);

				// Go through the image list images
				for (int frameIndex = 0; frameIndex < m_AnimFrames.Count; ++frameIndex)
					UpdateFrameImageList(frameIndex, m_AnimFrames[frameIndex].sourceRect);

				// Update the preview window
				m_AnimPreviewForm.SourceImage = m_SrcImage;

				// Update the picture box
				pictureBoxZoomImage.Image = m_SrcImage;				
			}
			catch (Exception exc)
			{
				MessageBox.Show("Failed to load image file. (" + exc.Message + ")", Text);
			}			
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user presses a key down with the list box focuessed
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void listViewFrames_KeyDown(object sender, KeyEventArgs e)
		{
			// If the delete key is pressed
			if (e.KeyCode == Keys.Delete )
				RemoveFrame( SelectedFrameIndex );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user changes the X position of the frame via the
		/// text box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxXPos_TextChanged(object sender, EventArgs e)
		{
			// If we don't have a valid frame selected then bail
			if (SelectedFrameIndex == -1)
				return;

			try
			{
				// Get the animation frame data
				AnimFrame curFrame = m_AnimFrames[SelectedFrameIndex];
				curFrame.sourceRect.X = int.Parse(textBoxXPos.Text);

				// Redraw
				pictureBoxZoomImage.Refresh();

				// Update the frame's list box image
				UpdateFrameImageList(SelectedFrameIndex, m_DragRect);
			}
			catch (Exception) { }
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user changes the Y position of the frame via the
		/// text box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxYPos_TextChanged(object sender, EventArgs e)
		{
			// If we don't have a valid frame selected then bail
			if (SelectedFrameIndex == -1)
				return;

			try
			{
				// Get the animation frame data
				AnimFrame curFrame = m_AnimFrames[SelectedFrameIndex];
				curFrame.sourceRect.Y = int.Parse(textBoxYPos.Text);

				// Redraw
				pictureBoxZoomImage.Refresh();

				// Update the frame's list box image
				UpdateFrameImageList(SelectedFrameIndex, m_DragRect);
			}
			catch (Exception) { }
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user changes the width of the frame via the text
		/// box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxFrameWidth_TextChanged(object sender, EventArgs e)
		{
			// If we don't have a valid frame selected then bail
			if (SelectedFrameIndex == -1)
				return;

			try
			{
				// Get the animation frame data
				AnimFrame curFrame = m_AnimFrames[SelectedFrameIndex];
				curFrame.sourceRect.Width = int.Parse(textBoxFrameWidth.Text);

				// Redraw
				pictureBoxZoomImage.Refresh();

				// Update the frame's list box image
				UpdateFrameImageList(SelectedFrameIndex, m_DragRect);
			}
			catch (Exception) { }
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user changes the height of the frame via the text
		/// box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxFrameHeight_TextChanged(object sender, EventArgs e)
		{
			// If we don't have a valid frame selected then bail
			if (SelectedFrameIndex == -1)
				return;

			try
			{
				// Get the animation frame data
				AnimFrame curFrame = m_AnimFrames[SelectedFrameIndex];
				curFrame.sourceRect.Height = int.Parse(textBoxFrameHeight.Text);

				// Redraw
				pictureBoxZoomImage.Refresh();

				// Update the frame's list box image
				UpdateFrameImageList(SelectedFrameIndex, m_DragRect);
			}
			catch (Exception) { }
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user changes the frame delay of the current frame
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxFrameDelay_TextChanged(object sender, EventArgs e)
		{
			// If we don't have a valid frame selected then bail
			if (SelectedFrameIndex == -1)
				return;

			try
			{
				int frameDelay = int.Parse(textBoxFrameDelay.Text);

				// If we are using the same delay for all frames
				if (checkBoxLinkDelays.Checked)
				{
					// Set the frame delay for all frames
					foreach (AnimFrame curFrame in m_AnimFrames)
						curFrame.frameDelay = frameDelay;
				}
				// Else set the current frame
				else
				{
					// Get the animation frame data
					AnimFrame curFrame = m_AnimFrames[SelectedFrameIndex];
					curFrame.frameDelay = frameDelay;
				}
			}
			catch (Exception) { }
		}

		private void radioButtonLoop_CheckedChanged(object sender, EventArgs e)
		{
			// Update the animation window
			m_AnimPreviewForm.AnimPlayType = AnimPlayType.Loop;
		}

		private void radioButtonPingPong_CheckedChanged(object sender, EventArgs e)
		{
			// Update the animation window
			m_AnimPreviewForm.AnimPlayType = AnimPlayType.PingPong;
		}

		private void radioButtonPlayOnce_CheckedChanged(object sender, EventArgs e)
		{
			// Update the animation window
			m_AnimPreviewForm.AnimPlayType = AnimPlayType.NoLoop;
		}

		private void FormMain_Shown(object sender, EventArgs e)
		{
			// Reposition the animation preview window
			m_AnimPreviewForm.Left = DesktopLocation.X + Width;
			m_AnimPreviewForm.Top = DesktopLocation.Y;

			textBoxResourceIndexPath.Text = Main.Default.ToolsResourceIndex;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks on the menu item to display the
		/// animation preview window
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuAnimShowPrevWin_Click(object sender, EventArgs e)
		{
			// If we need to create the window then do so
			if (m_AnimPreviewForm.IsDisposed)
				m_AnimPreviewForm = new FormAnimPreview();

			// Position it next to the parent
			m_AnimPreviewForm.Left = DesktopLocation.X + Width;
			m_AnimPreviewForm.Top = DesktopLocation.Y;

			// If the window is off the screen the bring it on
			if (m_AnimPreviewForm.Left + m_AnimPreviewForm.Width > Screen.PrimaryScreen.Bounds.Width)
				m_AnimPreviewForm.Left = Screen.PrimaryScreen.Bounds.Width - m_AnimPreviewForm.Width;

			// Display the window
			m_AnimPreviewForm.Show();
		}

		private void checkBoxLinkDelays_CheckedChanged(object sender, EventArgs e)
		{
			// If the check box is checked
			if (checkBoxLinkDelays.Checked && SelectedFrameIndex != -1 )
			{
				DialogResult msgBoxRes = MessageBox.Show("Selecting this will set all frames to use the same frame delay.  Any frame delays currently set will be lost.  Are you sure you want to continue?", Text, MessageBoxButtons.YesNo, MessageBoxIcon.Question);
				if (msgBoxRes == DialogResult.No)
					return;

				// Get the frame time
				int frameDelay = m_AnimFrames[SelectedFrameIndex].frameDelay;
				foreach (AnimFrame curFrame in m_AnimFrames)
					curFrame.frameDelay = frameDelay;
			}
		}

		private void textBoxResourceIndexPath_TextChanged( object sender, EventArgs e )
		{
			Main.Default.ToolsResourceIndex = textBoxResourceIndexPath.Text;

			Main.Default.Save();
		}

		private void buttonPickResourceIndex_Click( object sender, EventArgs e )
		{
			if (openToolsResourceFileDialog.ShowDialog() == DialogResult.OK)
				textBoxResourceIndexPath.Text = openToolsResourceFileDialog.FileName;
		}		
	}
}