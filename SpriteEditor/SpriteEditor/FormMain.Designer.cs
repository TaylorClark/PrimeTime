namespace SpriteCreator
{
	partial class FormMain
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager( typeof( FormMain ) );
			this.groupBoxImage = new System.Windows.Forms.GroupBox();
			this.pictureBoxZoomImage = new PictureBoxZoom.PictureBoxZoom();
			this.groupBoxFrameData = new System.Windows.Forms.GroupBox();
			this.checkBoxLinkDelays = new System.Windows.Forms.CheckBox();
			this.textBoxFrameHeight = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.textBoxYPos = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.textBoxFrameDelay = new System.Windows.Forms.TextBox();
			this.labelFrameDelay = new System.Windows.Forms.Label();
			this.textBoxFrameWidth = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.textBoxXPos = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBoxAnimation = new System.Windows.Forms.GroupBox();
			this.textBoxImageRef = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.radioButtonPingPong = new System.Windows.Forms.RadioButton();
			this.radioButtonPlayOnce = new System.Windows.Forms.RadioButton();
			this.radioButtonLoop = new System.Windows.Forms.RadioButton();
			this.textBoxSpriteName = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.buttonAddFrame = new System.Windows.Forms.Button();
			this.groupBoxFrames = new System.Windows.Forms.GroupBox();
			this.listViewFrames = new System.Windows.Forms.ListView();
			this.imageListFrames = new System.Windows.Forms.ImageList( this.components );
			this.openFileDialogImage = new System.Windows.Forms.OpenFileDialog();
			this.menuStripMain = new System.Windows.Forms.MenuStrip();
			this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.menuFileNew = new System.Windows.Forms.ToolStripMenuItem();
			this.menuFileOpen = new System.Windows.Forms.ToolStripMenuItem();
			this.menuFileSave = new System.Windows.Forms.ToolStripMenuItem();
			this.menuFileSaveAs = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
			this.menuFileExit = new System.Windows.Forms.ToolStripMenuItem();
			this.animationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.menuAnimLoadImage = new System.Windows.Forms.ToolStripMenuItem();
			this.menuAnimShowPrevWin = new System.Windows.Forms.ToolStripMenuItem();
			this.label7 = new System.Windows.Forms.Label();
			this.textBoxResourceIndexPath = new System.Windows.Forms.TextBox();
			this.buttonPickResourceIndex = new System.Windows.Forms.Button();
			this.openToolsResourceFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.groupBoxImage.SuspendLayout();
			this.groupBoxFrameData.SuspendLayout();
			this.groupBoxAnimation.SuspendLayout();
			this.groupBoxFrames.SuspendLayout();
			this.menuStripMain.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBoxImage
			// 
			this.groupBoxImage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBoxImage.Controls.Add( this.pictureBoxZoomImage );
			this.groupBoxImage.Location = new System.Drawing.Point( 200, 120 );
			this.groupBoxImage.Name = "groupBoxImage";
			this.groupBoxImage.Size = new System.Drawing.Size( 412, 297 );
			this.groupBoxImage.TabIndex = 0;
			this.groupBoxImage.TabStop = false;
			this.groupBoxImage.Text = "Source Image";
			// 
			// pictureBoxZoomImage
			// 
			this.pictureBoxZoomImage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.pictureBoxZoomImage.BackColor = System.Drawing.Color.Transparent;
			this.pictureBoxZoomImage.Image = null;
			this.pictureBoxZoomImage.Location = new System.Drawing.Point( 10, 20 );
			this.pictureBoxZoomImage.Name = "pictureBoxZoomImage";
			this.pictureBoxZoomImage.ShowCursorPosition = true;
			this.pictureBoxZoomImage.Size = new System.Drawing.Size( 392, 267 );
			this.pictureBoxZoomImage.TabIndex = 0;
			this.pictureBoxZoomImage.MouseMove += new System.Windows.Forms.MouseEventHandler( this.pictureBoxZoomImage_MouseMove );
			this.pictureBoxZoomImage.picturePaint += new PictureBoxZoom.PictureBoxZoom.PicturePaintHandler( this.pictureBoxZoomImage_picturePaint );
			this.pictureBoxZoomImage.MouseDown += new System.Windows.Forms.MouseEventHandler( this.pictureBoxZoomImage_MouseDown );
			this.pictureBoxZoomImage.MouseUp += new System.Windows.Forms.MouseEventHandler( this.pictureBoxZoomImage_MouseUp );
			// 
			// groupBoxFrameData
			// 
			this.groupBoxFrameData.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBoxFrameData.Controls.Add( this.checkBoxLinkDelays );
			this.groupBoxFrameData.Controls.Add( this.textBoxFrameHeight );
			this.groupBoxFrameData.Controls.Add( this.label4 );
			this.groupBoxFrameData.Controls.Add( this.textBoxYPos );
			this.groupBoxFrameData.Controls.Add( this.label5 );
			this.groupBoxFrameData.Controls.Add( this.textBoxFrameDelay );
			this.groupBoxFrameData.Controls.Add( this.labelFrameDelay );
			this.groupBoxFrameData.Controls.Add( this.textBoxFrameWidth );
			this.groupBoxFrameData.Controls.Add( this.label2 );
			this.groupBoxFrameData.Controls.Add( this.textBoxXPos );
			this.groupBoxFrameData.Controls.Add( this.label1 );
			this.groupBoxFrameData.Location = new System.Drawing.Point( 250, 30 );
			this.groupBoxFrameData.Name = "groupBoxFrameData";
			this.groupBoxFrameData.Size = new System.Drawing.Size( 362, 90 );
			this.groupBoxFrameData.TabIndex = 1;
			this.groupBoxFrameData.TabStop = false;
			this.groupBoxFrameData.Text = "Frame Data";
			// 
			// checkBoxLinkDelays
			// 
			this.checkBoxLinkDelays.AutoSize = true;
			this.checkBoxLinkDelays.Location = new System.Drawing.Point( 192, 48 );
			this.checkBoxLinkDelays.Name = "checkBoxLinkDelays";
			this.checkBoxLinkDelays.Size = new System.Drawing.Size( 135, 17 );
			this.checkBoxLinkDelays.TabIndex = 9;
			this.checkBoxLinkDelays.Text = "Use delay for all frames";
			this.checkBoxLinkDelays.UseVisualStyleBackColor = true;
			this.checkBoxLinkDelays.CheckedChanged += new System.EventHandler( this.checkBoxLinkDelays_CheckedChanged );
			// 
			// textBoxFrameHeight
			// 
			this.textBoxFrameHeight.Location = new System.Drawing.Point( 130, 50 );
			this.textBoxFrameHeight.Name = "textBoxFrameHeight";
			this.textBoxFrameHeight.Size = new System.Drawing.Size( 50, 20 );
			this.textBoxFrameHeight.TabIndex = 3;
			this.textBoxFrameHeight.TextChanged += new System.EventHandler( this.textBoxFrameHeight_TextChanged );
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point( 90, 50 );
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size( 44, 13 );
			this.label4.TabIndex = 8;
			this.label4.Text = "Height :";
			// 
			// textBoxYPos
			// 
			this.textBoxYPos.Location = new System.Drawing.Point( 30, 50 );
			this.textBoxYPos.Name = "textBoxYPos";
			this.textBoxYPos.Size = new System.Drawing.Size( 50, 20 );
			this.textBoxYPos.TabIndex = 1;
			this.textBoxYPos.TextChanged += new System.EventHandler( this.textBoxYPos_TextChanged );
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point( 10, 50 );
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size( 20, 13 );
			this.label5.TabIndex = 6;
			this.label5.Text = "Y :";
			// 
			// textBoxFrameDelay
			// 
			this.textBoxFrameDelay.Enabled = false;
			this.textBoxFrameDelay.Location = new System.Drawing.Point( 260, 20 );
			this.textBoxFrameDelay.Name = "textBoxFrameDelay";
			this.textBoxFrameDelay.Size = new System.Drawing.Size( 60, 20 );
			this.textBoxFrameDelay.TabIndex = 4;
			this.textBoxFrameDelay.TextChanged += new System.EventHandler( this.textBoxFrameDelay_TextChanged );
			// 
			// labelFrameDelay
			// 
			this.labelFrameDelay.AutoSize = true;
			this.labelFrameDelay.Location = new System.Drawing.Point( 190, 20 );
			this.labelFrameDelay.Name = "labelFrameDelay";
			this.labelFrameDelay.Size = new System.Drawing.Size( 73, 13 );
			this.labelFrameDelay.TabIndex = 4;
			this.labelFrameDelay.Text = "Delay (in ms) :";
			// 
			// textBoxFrameWidth
			// 
			this.textBoxFrameWidth.Location = new System.Drawing.Point( 130, 20 );
			this.textBoxFrameWidth.Name = "textBoxFrameWidth";
			this.textBoxFrameWidth.Size = new System.Drawing.Size( 50, 20 );
			this.textBoxFrameWidth.TabIndex = 2;
			this.textBoxFrameWidth.TextChanged += new System.EventHandler( this.textBoxFrameWidth_TextChanged );
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point( 90, 20 );
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size( 41, 13 );
			this.label2.TabIndex = 2;
			this.label2.Text = "Width :";
			// 
			// textBoxXPos
			// 
			this.textBoxXPos.Location = new System.Drawing.Point( 30, 20 );
			this.textBoxXPos.Name = "textBoxXPos";
			this.textBoxXPos.Size = new System.Drawing.Size( 50, 20 );
			this.textBoxXPos.TabIndex = 0;
			this.textBoxXPos.TextChanged += new System.EventHandler( this.textBoxXPos_TextChanged );
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point( 10, 20 );
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size( 20, 13 );
			this.label1.TabIndex = 0;
			this.label1.Text = "X :";
			// 
			// groupBoxAnimation
			// 
			this.groupBoxAnimation.Controls.Add( this.textBoxImageRef );
			this.groupBoxAnimation.Controls.Add( this.label6 );
			this.groupBoxAnimation.Controls.Add( this.radioButtonPingPong );
			this.groupBoxAnimation.Controls.Add( this.radioButtonPlayOnce );
			this.groupBoxAnimation.Controls.Add( this.radioButtonLoop );
			this.groupBoxAnimation.Controls.Add( this.textBoxSpriteName );
			this.groupBoxAnimation.Controls.Add( this.label3 );
			this.groupBoxAnimation.Location = new System.Drawing.Point( 10, 30 );
			this.groupBoxAnimation.Name = "groupBoxAnimation";
			this.groupBoxAnimation.Size = new System.Drawing.Size( 230, 90 );
			this.groupBoxAnimation.TabIndex = 2;
			this.groupBoxAnimation.TabStop = false;
			this.groupBoxAnimation.Text = "Animation Data";
			// 
			// textBoxImageRef
			// 
			this.textBoxImageRef.Location = new System.Drawing.Point( 64, 44 );
			this.textBoxImageRef.MaxLength = 23;
			this.textBoxImageRef.Name = "textBoxImageRef";
			this.textBoxImageRef.ReadOnly = true;
			this.textBoxImageRef.Size = new System.Drawing.Size( 160, 20 );
			this.textBoxImageRef.TabIndex = 4;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point( 4, 47 );
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size( 62, 13 );
			this.label6.TabIndex = 5;
			this.label6.Text = "Image Ref :";
			// 
			// radioButtonPingPong
			// 
			this.radioButtonPingPong.AutoSize = true;
			this.radioButtonPingPong.Enabled = false;
			this.radioButtonPingPong.Location = new System.Drawing.Point( 150, 67 );
			this.radioButtonPingPong.Name = "radioButtonPingPong";
			this.radioButtonPingPong.Size = new System.Drawing.Size( 74, 17 );
			this.radioButtonPingPong.TabIndex = 3;
			this.radioButtonPingPong.Text = "Ping-Pong";
			this.radioButtonPingPong.UseVisualStyleBackColor = true;
			this.radioButtonPingPong.CheckedChanged += new System.EventHandler( this.radioButtonPingPong_CheckedChanged );
			// 
			// radioButtonPlayOnce
			// 
			this.radioButtonPlayOnce.AutoSize = true;
			this.radioButtonPlayOnce.Checked = true;
			this.radioButtonPlayOnce.Enabled = false;
			this.radioButtonPlayOnce.Location = new System.Drawing.Point( 10, 67 );
			this.radioButtonPlayOnce.Name = "radioButtonPlayOnce";
			this.radioButtonPlayOnce.Size = new System.Drawing.Size( 74, 17 );
			this.radioButtonPlayOnce.TabIndex = 1;
			this.radioButtonPlayOnce.TabStop = true;
			this.radioButtonPlayOnce.Text = "Play Once";
			this.radioButtonPlayOnce.UseVisualStyleBackColor = true;
			this.radioButtonPlayOnce.CheckedChanged += new System.EventHandler( this.radioButtonPlayOnce_CheckedChanged );
			// 
			// radioButtonLoop
			// 
			this.radioButtonLoop.AutoSize = true;
			this.radioButtonLoop.Enabled = false;
			this.radioButtonLoop.Location = new System.Drawing.Point( 90, 67 );
			this.radioButtonLoop.Name = "radioButtonLoop";
			this.radioButtonLoop.Size = new System.Drawing.Size( 49, 17 );
			this.radioButtonLoop.TabIndex = 2;
			this.radioButtonLoop.Text = "Loop";
			this.radioButtonLoop.UseVisualStyleBackColor = true;
			this.radioButtonLoop.CheckedChanged += new System.EventHandler( this.radioButtonLoop_CheckedChanged );
			// 
			// textBoxSpriteName
			// 
			this.textBoxSpriteName.Location = new System.Drawing.Point( 64, 20 );
			this.textBoxSpriteName.MaxLength = 23;
			this.textBoxSpriteName.Name = "textBoxSpriteName";
			this.textBoxSpriteName.Size = new System.Drawing.Size( 160, 20 );
			this.textBoxSpriteName.TabIndex = 0;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point( 17, 23 );
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size( 41, 13 );
			this.label3.TabIndex = 2;
			this.label3.Text = "Name :";
			// 
			// buttonAddFrame
			// 
			this.buttonAddFrame.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonAddFrame.Location = new System.Drawing.Point( 10, 257 );
			this.buttonAddFrame.Name = "buttonAddFrame";
			this.buttonAddFrame.Size = new System.Drawing.Size( 160, 30 );
			this.buttonAddFrame.TabIndex = 1;
			this.buttonAddFrame.Text = "Add Frame";
			this.buttonAddFrame.UseVisualStyleBackColor = true;
			this.buttonAddFrame.Click += new System.EventHandler( this.buttonAddFrame_Click );
			// 
			// groupBoxFrames
			// 
			this.groupBoxFrames.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)));
			this.groupBoxFrames.Controls.Add( this.listViewFrames );
			this.groupBoxFrames.Controls.Add( this.buttonAddFrame );
			this.groupBoxFrames.Location = new System.Drawing.Point( 10, 120 );
			this.groupBoxFrames.Name = "groupBoxFrames";
			this.groupBoxFrames.Size = new System.Drawing.Size( 180, 297 );
			this.groupBoxFrames.TabIndex = 3;
			this.groupBoxFrames.TabStop = false;
			this.groupBoxFrames.Text = "Frames";
			// 
			// listViewFrames
			// 
			this.listViewFrames.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)));
			this.listViewFrames.BackColor = System.Drawing.SystemColors.Window;
			this.listViewFrames.FullRowSelect = true;
			this.listViewFrames.GridLines = true;
			this.listViewFrames.HideSelection = false;
			this.listViewFrames.Location = new System.Drawing.Point( 10, 20 );
			this.listViewFrames.MultiSelect = false;
			this.listViewFrames.Name = "listViewFrames";
			this.listViewFrames.ShowGroups = false;
			this.listViewFrames.Size = new System.Drawing.Size( 160, 227 );
			this.listViewFrames.TabIndex = 0;
			this.listViewFrames.UseCompatibleStateImageBehavior = false;
			this.listViewFrames.SelectedIndexChanged += new System.EventHandler( this.listViewFrames_SelectedIndexChanged );
			this.listViewFrames.KeyDown += new System.Windows.Forms.KeyEventHandler( this.listViewFrames_KeyDown );
			// 
			// imageListFrames
			// 
			this.imageListFrames.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
			this.imageListFrames.ImageSize = new System.Drawing.Size( 16, 16 );
			this.imageListFrames.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// openFileDialogImage
			// 
			this.openFileDialogImage.Filter = "All Images (*.bmp,*.jpg,*.png)|*.bmp;*.jpg;*.png|All Files (*.*)|*.*";
			// 
			// menuStripMain
			// 
			this.menuStripMain.BackColor = System.Drawing.SystemColors.ControlDark;
			this.menuStripMain.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.animationToolStripMenuItem} );
			this.menuStripMain.Location = new System.Drawing.Point( 0, 0 );
			this.menuStripMain.Name = "menuStripMain";
			this.menuStripMain.Size = new System.Drawing.Size( 624, 24 );
			this.menuStripMain.TabIndex = 4;
			this.menuStripMain.Text = "menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this.fileToolStripMenuItem.DropDownItems.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.menuFileNew,
            this.menuFileOpen,
            this.menuFileSave,
            this.menuFileSaveAs,
            this.toolStripMenuItem1,
            this.menuFileExit} );
			this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
			this.fileToolStripMenuItem.Size = new System.Drawing.Size( 35, 20 );
			this.fileToolStripMenuItem.Text = "&File";
			// 
			// menuFileNew
			// 
			this.menuFileNew.Name = "menuFileNew";
			this.menuFileNew.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
			this.menuFileNew.Size = new System.Drawing.Size( 204, 22 );
			this.menuFileNew.Text = "&New";
			this.menuFileNew.Click += new System.EventHandler( this.menuFileNew_Click );
			// 
			// menuFileOpen
			// 
			this.menuFileOpen.Name = "menuFileOpen";
			this.menuFileOpen.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
			this.menuFileOpen.Size = new System.Drawing.Size( 204, 22 );
			this.menuFileOpen.Text = "&Open...";
			this.menuFileOpen.Click += new System.EventHandler( this.menuFileOpen_Click );
			// 
			// menuFileSave
			// 
			this.menuFileSave.Name = "menuFileSave";
			this.menuFileSave.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
			this.menuFileSave.Size = new System.Drawing.Size( 204, 22 );
			this.menuFileSave.Text = "&Save";
			this.menuFileSave.Click += new System.EventHandler( this.menuFileSave_Click );
			// 
			// menuFileSaveAs
			// 
			this.menuFileSaveAs.Name = "menuFileSaveAs";
			this.menuFileSaveAs.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
						| System.Windows.Forms.Keys.S)));
			this.menuFileSaveAs.Size = new System.Drawing.Size( 204, 22 );
			this.menuFileSaveAs.Text = "Save &As...";
			this.menuFileSaveAs.Click += new System.EventHandler( this.menuFileSaveAs_Click );
			// 
			// toolStripMenuItem1
			// 
			this.toolStripMenuItem1.Name = "toolStripMenuItem1";
			this.toolStripMenuItem1.Size = new System.Drawing.Size( 201, 6 );
			// 
			// menuFileExit
			// 
			this.menuFileExit.Name = "menuFileExit";
			this.menuFileExit.Size = new System.Drawing.Size( 204, 22 );
			this.menuFileExit.Text = "E&xit";
			this.menuFileExit.Click += new System.EventHandler( this.menuFileExit_Click );
			// 
			// animationToolStripMenuItem
			// 
			this.animationToolStripMenuItem.DropDownItems.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.menuAnimLoadImage,
            this.menuAnimShowPrevWin} );
			this.animationToolStripMenuItem.Name = "animationToolStripMenuItem";
			this.animationToolStripMenuItem.Size = new System.Drawing.Size( 66, 20 );
			this.animationToolStripMenuItem.Text = "&Animation";
			// 
			// menuAnimLoadImage
			// 
			this.menuAnimLoadImage.Name = "menuAnimLoadImage";
			this.menuAnimLoadImage.Size = new System.Drawing.Size( 243, 22 );
			this.menuAnimLoadImage.Text = "Load Source Image...";
			this.menuAnimLoadImage.Click += new System.EventHandler( this.menuAnimLoadImage_Click );
			// 
			// menuAnimShowPrevWin
			// 
			this.menuAnimShowPrevWin.Name = "menuAnimShowPrevWin";
			this.menuAnimShowPrevWin.Size = new System.Drawing.Size( 243, 22 );
			this.menuAnimShowPrevWin.Text = "Show Animation Preview Window";
			this.menuAnimShowPrevWin.Click += new System.EventHandler( this.menuAnimShowPrevWin_Click );
			// 
			// label7
			// 
			this.label7.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point( 17, 429 );
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size( 114, 13 );
			this.label7.TabIndex = 5;
			this.label7.Text = "Tools Resource Index:";
			// 
			// textBoxResourceIndexPath
			// 
			this.textBoxResourceIndexPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxResourceIndexPath.Location = new System.Drawing.Point( 134, 426 );
			this.textBoxResourceIndexPath.Name = "textBoxResourceIndexPath";
			this.textBoxResourceIndexPath.Size = new System.Drawing.Size( 444, 20 );
			this.textBoxResourceIndexPath.TabIndex = 6;
			this.textBoxResourceIndexPath.TextChanged += new System.EventHandler( this.textBoxResourceIndexPath_TextChanged );
			// 
			// buttonPickResourceIndex
			// 
			this.buttonPickResourceIndex.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonPickResourceIndex.Location = new System.Drawing.Point( 584, 425 );
			this.buttonPickResourceIndex.Name = "buttonPickResourceIndex";
			this.buttonPickResourceIndex.Size = new System.Drawing.Size( 28, 22 );
			this.buttonPickResourceIndex.TabIndex = 7;
			this.buttonPickResourceIndex.Text = "...";
			this.buttonPickResourceIndex.UseVisualStyleBackColor = true;
			this.buttonPickResourceIndex.Click += new System.EventHandler( this.buttonPickResourceIndex_Click );
			// 
			// openToolsResourceFileDialog
			// 
			this.openToolsResourceFileDialog.DefaultExt = "dat";
			this.openToolsResourceFileDialog.Filter = "Resource Index Files (*.dat)|*.dat|All Files|*.*";
			// 
			// FormMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size( 624, 453 );
			this.Controls.Add( this.buttonPickResourceIndex );
			this.Controls.Add( this.textBoxResourceIndexPath );
			this.Controls.Add( this.label7 );
			this.Controls.Add( this.groupBoxFrames );
			this.Controls.Add( this.groupBoxAnimation );
			this.Controls.Add( this.groupBoxFrameData );
			this.Controls.Add( this.groupBoxImage );
			this.Controls.Add( this.menuStripMain );
			this.Icon = ((System.Drawing.Icon)(resources.GetObject( "$this.Icon" )));
			this.MinimumSize = new System.Drawing.Size( 460, 300 );
			this.Name = "FormMain";
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
			this.Text = "Prime Time Sprite Editor";
			this.Shown += new System.EventHandler( this.FormMain_Shown );
			this.groupBoxImage.ResumeLayout( false );
			this.groupBoxFrameData.ResumeLayout( false );
			this.groupBoxFrameData.PerformLayout();
			this.groupBoxAnimation.ResumeLayout( false );
			this.groupBoxAnimation.PerformLayout();
			this.groupBoxFrames.ResumeLayout( false );
			this.menuStripMain.ResumeLayout( false );
			this.menuStripMain.PerformLayout();
			this.ResumeLayout( false );
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBoxImage;
		private System.Windows.Forms.GroupBox groupBoxFrameData;
		private System.Windows.Forms.GroupBox groupBoxAnimation;
		private PictureBoxZoom.PictureBoxZoom pictureBoxZoomImage;
		private System.Windows.Forms.GroupBox groupBoxFrames;
		private System.Windows.Forms.ListView listViewFrames;
		private System.Windows.Forms.ImageList imageListFrames;
		private System.Windows.Forms.OpenFileDialog openFileDialogImage;
		private System.Windows.Forms.Button buttonAddFrame;
		private System.Windows.Forms.MenuStrip menuStripMain;
		private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem menuFileNew;
		private System.Windows.Forms.ToolStripMenuItem menuFileOpen;
		private System.Windows.Forms.ToolStripMenuItem menuFileSave;
		private System.Windows.Forms.ToolStripMenuItem menuFileSaveAs;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
		private System.Windows.Forms.ToolStripMenuItem menuFileExit;
		private System.Windows.Forms.TextBox textBoxXPos;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox textBoxFrameWidth;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox textBoxFrameDelay;
		private System.Windows.Forms.Label labelFrameDelay;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox textBoxSpriteName;
		private System.Windows.Forms.ToolStripMenuItem animationToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem menuAnimLoadImage;
		private System.Windows.Forms.RadioButton radioButtonPingPong;
		private System.Windows.Forms.RadioButton radioButtonPlayOnce;
		private System.Windows.Forms.RadioButton radioButtonLoop;
		private System.Windows.Forms.TextBox textBoxFrameHeight;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox textBoxYPos;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.ToolStripMenuItem menuAnimShowPrevWin;
		private System.Windows.Forms.CheckBox checkBoxLinkDelays;
		private System.Windows.Forms.TextBox textBoxImageRef;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.TextBox textBoxResourceIndexPath;
		private System.Windows.Forms.Button buttonPickResourceIndex;
		private System.Windows.Forms.OpenFileDialog openToolsResourceFileDialog;
	}
}

