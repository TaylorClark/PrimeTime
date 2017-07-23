namespace SpriteCreator
{
	partial class FormAnimPreview
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.groupBoxAnimation = new System.Windows.Forms.GroupBox();
			this.pictureBoxAnim = new System.Windows.Forms.PictureBox();
			this.hScrollBarAnimFrames = new System.Windows.Forms.HScrollBar();
			this.buttonPlay = new System.Windows.Forms.Button();
			this.timerPlay = new System.Windows.Forms.Timer(this.components);
			this.groupBoxAnimation.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxAnim)).BeginInit();
			this.SuspendLayout();
			// 
			// groupBoxAnimation
			// 
			this.groupBoxAnimation.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBoxAnimation.Controls.Add(this.pictureBoxAnim);
			this.groupBoxAnimation.Location = new System.Drawing.Point(10, 10);
			this.groupBoxAnimation.Name = "groupBoxAnimation";
			this.groupBoxAnimation.Size = new System.Drawing.Size(310, 193);
			this.groupBoxAnimation.TabIndex = 0;
			this.groupBoxAnimation.TabStop = false;
			this.groupBoxAnimation.Text = "Animation";
			// 
			// pictureBoxAnim
			// 
			this.pictureBoxAnim.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.pictureBoxAnim.Location = new System.Drawing.Point(10, 20);
			this.pictureBoxAnim.Name = "pictureBoxAnim";
			this.pictureBoxAnim.Size = new System.Drawing.Size(290, 160);
			this.pictureBoxAnim.TabIndex = 0;
			this.pictureBoxAnim.TabStop = false;
			this.pictureBoxAnim.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBoxAnim_Paint);
			// 
			// hScrollBarAnimFrames
			// 
			this.hScrollBarAnimFrames.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.hScrollBarAnimFrames.LargeChange = 1;
			this.hScrollBarAnimFrames.Location = new System.Drawing.Point(10, 213);
			this.hScrollBarAnimFrames.Maximum = 0;
			this.hScrollBarAnimFrames.Name = "hScrollBarAnimFrames";
			this.hScrollBarAnimFrames.Size = new System.Drawing.Size(310, 20);
			this.hScrollBarAnimFrames.TabIndex = 1;
			this.hScrollBarAnimFrames.ValueChanged += new System.EventHandler(this.hScrollBarAnimFrames_ValueChanged);
			// 
			// buttonPlay
			// 
			this.buttonPlay.Location = new System.Drawing.Point(130, 240);
			this.buttonPlay.Name = "buttonPlay";
			this.buttonPlay.Size = new System.Drawing.Size(60, 30);
			this.buttonPlay.TabIndex = 2;
			this.buttonPlay.Text = "Play";
			this.buttonPlay.UseVisualStyleBackColor = true;
			this.buttonPlay.Click += new System.EventHandler(this.buttonPlay_Click);
			// 
			// timerPlay
			// 
			this.timerPlay.Tick += new System.EventHandler(this.timerPlay_Tick);
			// 
			// FormAnimPreview
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(335, 285);
			this.Controls.Add(this.buttonPlay);
			this.Controls.Add(this.hScrollBarAnimFrames);
			this.Controls.Add(this.groupBoxAnimation);
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FormAnimPreview";
			this.ShowInTaskbar = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "Animation Preview";
			this.groupBoxAnimation.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxAnim)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBoxAnimation;
		private System.Windows.Forms.HScrollBar hScrollBarAnimFrames;
		private System.Windows.Forms.PictureBox pictureBoxAnim;
		private System.Windows.Forms.Button buttonPlay;
		private System.Windows.Forms.Timer timerPlay;
	}
}