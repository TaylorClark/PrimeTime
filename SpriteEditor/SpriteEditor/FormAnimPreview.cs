using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SpriteCreator
{
	public partial class FormAnimPreview : Form
	{
		/// <summary> The list of animation frames </summary>
		private List<FormMain.AnimFrame> m_AnimFrames = new List<FormMain.AnimFrame>();

		/// <summary> The image to reference for frame data </summary>
		private Image m_SourceImage = null;

		/// <summary> If we are playing the animation in reverse due to it being a ping-pong play
		/// type </summary>
		bool isPlayingReverse = false;

		/// <summary> How the animation handles coming to the last frame </summary>
		private FormMain.AnimPlayType m_AnimPlayType = FormMain.AnimPlayType.NoLoop;


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set the animation play type
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public FormMain.AnimPlayType AnimPlayType
		{
			get
			{
				return m_AnimPlayType;
			}
			set
			{
				// Store the value
				m_AnimPlayType = value;

				// If we aren't in ping-pong 
				if (m_AnimPlayType != FormMain.AnimPlayType.PingPong)
					isPlayingReverse = false;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Set the list of animation data </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public List<FormMain.AnimFrame> AnimList
		{
			set
			{
				lock (m_AnimFrames)
				{
					m_AnimFrames = new List<FormMain.AnimFrame>(value);

					// Update the scroll bar
					if (m_AnimFrames.Count == 0)
						hScrollBarAnimFrames.Enabled = false;
					else
					{
						hScrollBarAnimFrames.Enabled = true;
						if (hScrollBarAnimFrames.Value >= m_AnimFrames.Count)
							hScrollBarAnimFrames.Value = m_AnimFrames.Count - 1;
						hScrollBarAnimFrames.Maximum = m_AnimFrames.Count - 1;
					}
				}

				// Redraw
				pictureBoxAnim.Refresh();
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set the image to reference frame data from
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public Image SourceImage
		{
			set
			{
				m_SourceImage = value;
				
				// Redraw
				pictureBoxAnim.Refresh();
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The default constructor
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public FormAnimPreview()
		{
			InitializeComponent();			
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the animation picture box is being redrawn
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxAnim_Paint(object sender, PaintEventArgs e)
		{
			FormMain.AnimFrame curFrame;
			lock (m_AnimFrames)
			{
				// Get the current frame
				if (hScrollBarAnimFrames.Value >= m_AnimFrames.Count || m_SourceImage == null)
					return;

				// Get the current frame
				curFrame = m_AnimFrames[hScrollBarAnimFrames.Value];
			}

			// Calculate the destination rectangle
			Rectangle destRect = new Rectangle( 0, 0, curFrame.sourceRect.Width, curFrame.sourceRect.Height );
			destRect.X = (pictureBoxAnim.ClientRectangle.Width - destRect.Width) / 2;
			destRect.Y = (pictureBoxAnim.ClientRectangle.Height - destRect.Height) / 2;

			// Draw the frame
			e.Graphics.DrawImage(m_SourceImage, destRect, curFrame.sourceRect, GraphicsUnit.Pixel);
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks the play/stop button
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void buttonPlay_Click(object sender, EventArgs e)
		{
			// If we currently playing
			if (timerPlay.Enabled)
			{
				// Stop animating
				timerPlay.Stop();

				// Change the text
				buttonPlay.Text = "Play";
			}
			// Else start playing
			else
			{
				lock (m_AnimFrames)
				{
					if (m_AnimFrames.Count == 0)
						return;

					// If we are on the last frame then go to the first
					if (hScrollBarAnimFrames.Value == m_AnimFrames.Count - 1)
						hScrollBarAnimFrames.Value = 0;

					// Get the current frame delay
					timerPlay.Interval = m_AnimFrames[hScrollBarAnimFrames.Value].frameDelay;
				}

				// Change the text
				buttonPlay.Text = "Stop";

				// Play
				timerPlay.Start();				
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the timer ticks
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void timerPlay_Tick(object sender, EventArgs e)
		{
			int numFrames = 0;
			lock (m_AnimFrames)
				numFrames = m_AnimFrames.Count;

			// Step a frame
			int newFrameIndex = 0;

			// If we are playing reverse then we must have a ping-pong play type
			if( isPlayingReverse )
			{
				newFrameIndex = hScrollBarAnimFrames.Value - 1;

				// If we have reached the beginning of the animation
				if( newFrameIndex < 0 )
				{
					newFrameIndex = 1;
					isPlayingReverse = false;
				}
			}
			// Else we are playing in the normal forward direction
			else
			{
				newFrameIndex = hScrollBarAnimFrames.Value + 1;

				// If we have reached the end
				if (newFrameIndex >= numFrames)
				{
					// If we are in ping-pong play mode
					if( m_AnimPlayType == FormMain.AnimPlayType.PingPong )
					{
						isPlayingReverse = true;
						newFrameIndex = hScrollBarAnimFrames.Value - 1;
					}
					// Else if we are looping
					else if( m_AnimPlayType == FormMain.AnimPlayType.Loop )
					{
						newFrameIndex = 0;
					}
					// Else if we are done
					else if (m_AnimPlayType == FormMain.AnimPlayType.NoLoop)
					{
						timerPlay.Enabled = false;
						buttonPlay.Text = "Play";
						newFrameIndex = hScrollBarAnimFrames.Value;
					}
				}
			}

			// See if we should stop playing
			if (newFrameIndex < 0 || newFrameIndex >= numFrames)
			{
				timerPlay.Enabled = false;
				buttonPlay.Text = "Play";
				newFrameIndex = 0;
			}

			// Set the scroll value
			hScrollBarAnimFrames.Value = newFrameIndex;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the value of the horizontal scroll bar changes
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void hScrollBarAnimFrames_ValueChanged(object sender, EventArgs e)
		{
			// Redraw
			pictureBoxAnim.Refresh();

			// Set timer duration
			lock (m_AnimFrames)
			{
				if (m_AnimFrames.Count == 0)
					return;

				// Get the current frame delay
				timerPlay.Interval = m_AnimFrames[hScrollBarAnimFrames.Value].frameDelay;
			}
		}

		private void FormAnimPreview_Load(object sender, EventArgs e)
		{
			
		}
	}
}