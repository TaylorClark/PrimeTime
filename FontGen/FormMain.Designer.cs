namespace FontGen
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
			this.menuStrip1 = new System.Windows.Forms.MenuStrip();
			this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.fontToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.fontColorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.fontDialog = new System.Windows.Forms.FontDialog();
			this.label1 = new System.Windows.Forms.Label();
			this.numericUpDownWidth = new System.Windows.Forms.NumericUpDown();
			this.numericUpDownHeight = new System.Windows.Forms.NumericUpDown();
			this.checkBoxCaps = new System.Windows.Forms.CheckBox();
			this.colorDialog = new System.Windows.Forms.ColorDialog();
			this.label2 = new System.Windows.Forms.Label();
			this.pictureBoxBGColor = new System.Windows.Forms.PictureBox();
			this.buttonSave = new System.Windows.Forms.Button();
			this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
			this.textBoxResID = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.menuStrip1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDownWidth)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDownHeight)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxBGColor)).BeginInit();
			this.SuspendLayout();
			// 
			// menuStrip1
			// 
			this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.fontToolStripMenuItem,
            this.fontColorToolStripMenuItem});
			this.menuStrip1.Location = new System.Drawing.Point(0, 0);
			this.menuStrip1.Name = "menuStrip1";
			this.menuStrip1.Size = new System.Drawing.Size(836, 24);
			this.menuStrip1.TabIndex = 0;
			this.menuStrip1.Text = "menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveToolStripMenuItem});
			this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
			this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
			this.fileToolStripMenuItem.Text = "File";
			// 
			// saveToolStripMenuItem
			// 
			this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
			this.saveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
			this.saveToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
			this.saveToolStripMenuItem.Text = "Save";
			// 
			// fontToolStripMenuItem
			// 
			this.fontToolStripMenuItem.Name = "fontToolStripMenuItem";
			this.fontToolStripMenuItem.Size = new System.Drawing.Size(41, 20);
			this.fontToolStripMenuItem.Text = "Font";
			this.fontToolStripMenuItem.Click += new System.EventHandler(this.fontToolStripMenuItem_Click);
			// 
			// fontColorToolStripMenuItem
			// 
			this.fontColorToolStripMenuItem.Name = "fontColorToolStripMenuItem";
			this.fontColorToolStripMenuItem.Size = new System.Drawing.Size(69, 20);
			this.fontColorToolStripMenuItem.Text = "Font Color";
			this.fontColorToolStripMenuItem.Click += new System.EventHandler(this.fontColorToolStripMenuItem_Click);
			// 
			// fontDialog
			// 
			this.fontDialog.FontMustExist = true;
			// 
			// label1
			// 
			this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(715, 44);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(12, 13);
			this.label1.TabIndex = 3;
			this.label1.Text = "x";
			// 
			// numericUpDownWidth
			// 
			this.numericUpDownWidth.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.numericUpDownWidth.Location = new System.Drawing.Point(621, 40);
			this.numericUpDownWidth.Maximum = new decimal(new int[] {
            4096,
            0,
            0,
            0});
			this.numericUpDownWidth.Minimum = new decimal(new int[] {
            32,
            0,
            0,
            0});
			this.numericUpDownWidth.Name = "numericUpDownWidth";
			this.numericUpDownWidth.ReadOnly = true;
			this.numericUpDownWidth.Size = new System.Drawing.Size(88, 20);
			this.numericUpDownWidth.TabIndex = 4;
			this.numericUpDownWidth.Value = new decimal(new int[] {
            256,
            0,
            0,
            0});
			this.numericUpDownWidth.ValueChanged += new System.EventHandler(this.numericUpDownWidth_ValueChanged);
			// 
			// numericUpDownHeight
			// 
			this.numericUpDownHeight.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.numericUpDownHeight.Location = new System.Drawing.Point(733, 40);
			this.numericUpDownHeight.Maximum = new decimal(new int[] {
            4096,
            0,
            0,
            0});
			this.numericUpDownHeight.Minimum = new decimal(new int[] {
            32,
            0,
            0,
            0});
			this.numericUpDownHeight.Name = "numericUpDownHeight";
			this.numericUpDownHeight.ReadOnly = true;
			this.numericUpDownHeight.Size = new System.Drawing.Size(88, 20);
			this.numericUpDownHeight.TabIndex = 5;
			this.numericUpDownHeight.Value = new decimal(new int[] {
            256,
            0,
            0,
            0});
			this.numericUpDownHeight.ValueChanged += new System.EventHandler(this.numericUpDownWidth_ValueChanged);
			// 
			// checkBoxCaps
			// 
			this.checkBoxCaps.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.checkBoxCaps.AutoSize = true;
			this.checkBoxCaps.Location = new System.Drawing.Point(685, 72);
			this.checkBoxCaps.Name = "checkBoxCaps";
			this.checkBoxCaps.Size = new System.Drawing.Size(82, 17);
			this.checkBoxCaps.TabIndex = 6;
			this.checkBoxCaps.Text = "Ignore caps";
			this.checkBoxCaps.UseVisualStyleBackColor = true;
			this.checkBoxCaps.CheckedChanged += new System.EventHandler(this.checkBoxCaps_CheckedChanged);
			// 
			// colorDialog
			// 
			this.colorDialog.AnyColor = true;
			this.colorDialog.FullOpen = true;
			// 
			// label2
			// 
			this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(669, 104);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(49, 13);
			this.label2.TabIndex = 7;
			this.label2.Text = "BG Color";
			// 
			// pictureBoxBGColor
			// 
			this.pictureBoxBGColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.pictureBoxBGColor.BackColor = System.Drawing.Color.White;
			this.pictureBoxBGColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBoxBGColor.Location = new System.Drawing.Point(733, 96);
			this.pictureBoxBGColor.Name = "pictureBoxBGColor";
			this.pictureBoxBGColor.Size = new System.Drawing.Size(72, 32);
			this.pictureBoxBGColor.TabIndex = 8;
			this.pictureBoxBGColor.TabStop = false;
			this.pictureBoxBGColor.Click += new System.EventHandler(this.pictureBoxBGColor_Click);
			// 
			// buttonSave
			// 
			this.buttonSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonSave.Location = new System.Drawing.Point(693, 136);
			this.buttonSave.Name = "buttonSave";
			this.buttonSave.Size = new System.Drawing.Size(104, 40);
			this.buttonSave.TabIndex = 9;
			this.buttonSave.Text = "Save...";
			this.buttonSave.UseVisualStyleBackColor = true;
			this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
			// 
			// textBoxResID
			// 
			this.textBoxResID.Location = new System.Drawing.Point(712, 184);
			this.textBoxResID.Name = "textBoxResID";
			this.textBoxResID.Size = new System.Drawing.Size(112, 20);
			this.textBoxResID.TabIndex = 10;
			this.textBoxResID.Text = "100";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(637, 188);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(75, 13);
			this.label3.TabIndex = 11;
			this.label3.Text = "Image Res ID:";
			// 
			// FormMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(836, 619);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.textBoxResID);
			this.Controls.Add(this.buttonSave);
			this.Controls.Add(this.pictureBoxBGColor);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.checkBoxCaps);
			this.Controls.Add(this.numericUpDownHeight);
			this.Controls.Add(this.numericUpDownWidth);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.menuStrip1);
			this.MainMenuStrip = this.menuStrip1;
			this.Name = "FormMain";
			this.Text = "Font Generator";
			this.Paint += new System.Windows.Forms.PaintEventHandler(this.FormMain_Paint);
			this.menuStrip1.ResumeLayout(false);
			this.menuStrip1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDownWidth)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDownHeight)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxBGColor)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.MenuStrip menuStrip1;
		private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
		private System.Windows.Forms.FontDialog fontDialog;
		private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem fontToolStripMenuItem;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.NumericUpDown numericUpDownWidth;
		private System.Windows.Forms.NumericUpDown numericUpDownHeight;
		private System.Windows.Forms.CheckBox checkBoxCaps;
		private System.Windows.Forms.ToolStripMenuItem fontColorToolStripMenuItem;
		private System.Windows.Forms.ColorDialog colorDialog;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.PictureBox pictureBoxBGColor;
		private System.Windows.Forms.Button buttonSave;
		private System.Windows.Forms.SaveFileDialog saveFileDialog;
		private System.Windows.Forms.TextBox textBoxResID;
		private System.Windows.Forms.Label label3;
	}
}

