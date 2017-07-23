namespace MsgKeyGen
{
	partial class Form1
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
			this.openFileDialogFindOffset = new System.Windows.Forms.OpenFileDialog();
			this.textBoxFindOffsetFile = new System.Windows.Forms.TextBox();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.button2 = new System.Windows.Forms.Button();
			this.buttonFindMacx86MsgOffset = new System.Windows.Forms.Button();
			this.buttonOverwiteMacMsg = new System.Windows.Forms.Button();
			this.buttonFindMacMsgOffset = new System.Windows.Forms.Button();
			this.labelOverwriteMsgLen = new System.Windows.Forms.Label();
			this.buttonOverwriteMsg = new System.Windows.Forms.Button();
			this.textBoxOverwriteMsg = new System.Windows.Forms.TextBox();
			this.buttonFindMsgOffset = new System.Windows.Forms.Button();
			this.textBoxMsgOffset = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.groupBox4 = new System.Windows.Forms.GroupBox();
			this.buttonFindMacx64KeyOffset = new System.Windows.Forms.Button();
			this.buttonFindMacx86KeyOffset = new System.Windows.Forms.Button();
			this.buttonFindMacPPCKeyOffset = new System.Windows.Forms.Button();
			this.label5 = new System.Windows.Forms.Label();
			this.textBoxInt64Pad = new System.Windows.Forms.TextBox();
			this.buttonFindOffset = new System.Windows.Forms.Button();
			this.label3 = new System.Windows.Forms.Label();
			this.textBoxKeyOffset = new System.Windows.Forms.TextBox();
			this.textBoxOverwriteKeyValue = new System.Windows.Forms.TextBox();
			this.buttonOverwriteKey = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.buttonFindOffsetBrowse = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.buttonFindAllOffsets = new System.Windows.Forms.Button();
			this.textBoxMsg = new System.Windows.Forms.TextBox();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.labelGenMsgLen = new System.Windows.Forms.Label();
			this.textBoxOutMsg = new System.Windows.Forms.TextBox();
			this.button1 = new System.Windows.Forms.Button();
			this.groupBox5 = new System.Windows.Forms.GroupBox();
			this.buttonReadKey = new System.Windows.Forms.Button();
			this.label6 = new System.Windows.Forms.Label();
			this.textBoxReadKey = new System.Windows.Forms.TextBox();
			this.textBoxGenKeyVal = new System.Windows.Forms.TextBox();
			this.buttonMakeKey = new System.Windows.Forms.Button();
			this.groupBox6 = new System.Windows.Forms.GroupBox();
			this.buttonFindMacPicOffsets = new System.Windows.Forms.Button();
			this.buttonOverwriteCustomPicMsg = new System.Windows.Forms.Button();
			this.textBoxCustomPicMsg = new System.Windows.Forms.TextBox();
			this.textBoxCustomPicMsgOffset = new System.Windows.Forms.TextBox();
			this.label8 = new System.Windows.Forms.Label();
			this.buttonFindCustomPicOffset = new System.Windows.Forms.Button();
			this.textBoxCustomPicOffset = new System.Windows.Forms.TextBox();
			this.labelCustomPicOffset = new System.Windows.Forms.Label();
			this.buttonEmbedCustomPic = new System.Windows.Forms.Button();
			this.label7 = new System.Windows.Forms.Label();
			this.buttonPickCustomPic = new System.Windows.Forms.Button();
			this.textBoxCustomPic = new System.Windows.Forms.TextBox();
			this.pictureBoxCustomPic = new System.Windows.Forms.PictureBox();
			this.openFileDialogCustomPic = new System.Windows.Forms.OpenFileDialog();
			this.groupBox7 = new System.Windows.Forms.GroupBox();
			this.dateTimePickerExpiration = new System.Windows.Forms.DateTimePicker();
			this.buttonSetExpirationDate = new System.Windows.Forms.Button();
			this.buttonFindExpirationDateOffset = new System.Windows.Forms.Button();
			this.textBoxExpirationDateOffset = new System.Windows.Forms.TextBox();
			this.label9 = new System.Windows.Forms.Label();
			this.buttonGenerateExeInfo = new System.Windows.Forms.Button();
			this.buttonGenerateMacExeInfo = new System.Windows.Forms.Button();
			this.saveFileDialogInfo = new System.Windows.Forms.SaveFileDialog();
			this.groupBox3.SuspendLayout();
			this.groupBox4.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.groupBox5.SuspendLayout();
			this.groupBox6.SuspendLayout();
			( (System.ComponentModel.ISupportInitialize)( this.pictureBoxCustomPic ) ).BeginInit();
			this.groupBox7.SuspendLayout();
			this.SuspendLayout();
			// 
			// openFileDialogFindOffset
			// 
			this.openFileDialogFindOffset.DefaultExt = "exe";
			this.openFileDialogFindOffset.FileName = "openFileDialog1";
			this.openFileDialogFindOffset.Filter = "Executables (*.exe)|*.exe|All Files|*.*";
			// 
			// textBoxFindOffsetFile
			// 
			this.textBoxFindOffsetFile.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.textBoxFindOffsetFile.Location = new System.Drawing.Point( 80, 24 );
			this.textBoxFindOffsetFile.Name = "textBoxFindOffsetFile";
			this.textBoxFindOffsetFile.Size = new System.Drawing.Size( 462, 20 );
			this.textBoxFindOffsetFile.TabIndex = 1;
			this.textBoxFindOffsetFile.TextChanged += new System.EventHandler( this.textBox_RegVal_TextChanged );
			// 
			// groupBox3
			// 
			this.groupBox3.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.groupBox3.Controls.Add( this.button2 );
			this.groupBox3.Controls.Add( this.buttonFindMacx86MsgOffset );
			this.groupBox3.Controls.Add( this.buttonOverwiteMacMsg );
			this.groupBox3.Controls.Add( this.buttonFindMacMsgOffset );
			this.groupBox3.Controls.Add( this.labelOverwriteMsgLen );
			this.groupBox3.Controls.Add( this.buttonOverwriteMsg );
			this.groupBox3.Controls.Add( this.textBoxOverwriteMsg );
			this.groupBox3.Controls.Add( this.buttonFindMsgOffset );
			this.groupBox3.Controls.Add( this.textBoxMsgOffset );
			this.groupBox3.Controls.Add( this.label4 );
			this.groupBox3.Location = new System.Drawing.Point( 16, 152 );
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size( 638, 104 );
			this.groupBox3.TabIndex = 5;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Message";
			// 
			// button2
			// 
			this.button2.Location = new System.Drawing.Point( 211, 45 );
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size( 133, 23 );
			this.button2.TabIndex = 9;
			this.button2.Text = "Find Offset for x64 Mac";
			this.button2.UseVisualStyleBackColor = true;
			this.button2.Click += new System.EventHandler( this.buttonFindMacx64MsgOffset_Click );
			// 
			// buttonFindMacx86MsgOffset
			// 
			this.buttonFindMacx86MsgOffset.Location = new System.Drawing.Point( 392, 17 );
			this.buttonFindMacx86MsgOffset.Name = "buttonFindMacx86MsgOffset";
			this.buttonFindMacx86MsgOffset.Size = new System.Drawing.Size( 129, 23 );
			this.buttonFindMacx86MsgOffset.TabIndex = 8;
			this.buttonFindMacx86MsgOffset.Text = "Find Offset for x86 Mac";
			this.buttonFindMacx86MsgOffset.UseVisualStyleBackColor = true;
			this.buttonFindMacx86MsgOffset.Click += new System.EventHandler( this.buttonFindMacx86MsgOffset_Click );
			// 
			// buttonOverwiteMacMsg
			// 
			this.buttonOverwiteMacMsg.Location = new System.Drawing.Point( 480, 80 );
			this.buttonOverwiteMacMsg.Name = "buttonOverwiteMacMsg";
			this.buttonOverwiteMacMsg.Size = new System.Drawing.Size( 147, 20 );
			this.buttonOverwiteMacMsg.TabIndex = 7;
			this.buttonOverwiteMacMsg.Text = "Overwrite Message Mac";
			this.buttonOverwiteMacMsg.UseVisualStyleBackColor = true;
			this.buttonOverwiteMacMsg.Click += new System.EventHandler( this.buttonOverwiteMacMsg_Click );
			// 
			// buttonFindMacMsgOffset
			// 
			this.buttonFindMacMsgOffset.Location = new System.Drawing.Point( 260, 17 );
			this.buttonFindMacMsgOffset.Name = "buttonFindMacMsgOffset";
			this.buttonFindMacMsgOffset.Size = new System.Drawing.Size( 132, 23 );
			this.buttonFindMacMsgOffset.TabIndex = 6;
			this.buttonFindMacMsgOffset.Text = "Find Offset for PPC Mac";
			this.buttonFindMacMsgOffset.UseVisualStyleBackColor = true;
			this.buttonFindMacMsgOffset.Click += new System.EventHandler( this.buttonFindMacPPCMsgOffset_Click );
			// 
			// labelOverwriteMsgLen
			// 
			this.labelOverwriteMsgLen.AutoSize = true;
			this.labelOverwriteMsgLen.Location = new System.Drawing.Point( 434, 69 );
			this.labelOverwriteMsgLen.Name = "labelOverwriteMsgLen";
			this.labelOverwriteMsgLen.Size = new System.Drawing.Size( 52, 13 );
			this.labelOverwriteMsgLen.TabIndex = 5;
			this.labelOverwriteMsgLen.Text = "Length: 0";
			// 
			// buttonOverwriteMsg
			// 
			this.buttonOverwriteMsg.Location = new System.Drawing.Point( 499, 48 );
			this.buttonOverwriteMsg.Name = "buttonOverwriteMsg";
			this.buttonOverwriteMsg.Size = new System.Drawing.Size( 128, 24 );
			this.buttonOverwriteMsg.TabIndex = 4;
			this.buttonOverwriteMsg.Text = "Overwrite Message";
			this.buttonOverwriteMsg.UseVisualStyleBackColor = true;
			this.buttonOverwriteMsg.Click += new System.EventHandler( this.buttonOverwriteMsg_Click );
			// 
			// textBoxOverwriteMsg
			// 
			this.textBoxOverwriteMsg.Location = new System.Drawing.Point( 8, 74 );
			this.textBoxOverwriteMsg.MaxLength = 64;
			this.textBoxOverwriteMsg.Name = "textBoxOverwriteMsg";
			this.textBoxOverwriteMsg.Size = new System.Drawing.Size( 424, 20 );
			this.textBoxOverwriteMsg.TabIndex = 3;
			this.textBoxOverwriteMsg.TextChanged += new System.EventHandler( this.textBoxOverwriteMsg_TextChanged );
			// 
			// buttonFindMsgOffset
			// 
			this.buttonFindMsgOffset.Location = new System.Drawing.Point( 185, 16 );
			this.buttonFindMsgOffset.Name = "buttonFindMsgOffset";
			this.buttonFindMsgOffset.Size = new System.Drawing.Size( 69, 24 );
			this.buttonFindMsgOffset.TabIndex = 2;
			this.buttonFindMsgOffset.Text = "Find Offset";
			this.buttonFindMsgOffset.UseVisualStyleBackColor = true;
			this.buttonFindMsgOffset.Click += new System.EventHandler( this.buttonFindMsgOffset_Click );
			// 
			// textBoxMsgOffset
			// 
			this.textBoxMsgOffset.Location = new System.Drawing.Point( 92, 18 );
			this.textBoxMsgOffset.Name = "textBoxMsgOffset";
			this.textBoxMsgOffset.Size = new System.Drawing.Size( 88, 20 );
			this.textBoxMsgOffset.TabIndex = 1;
			this.textBoxMsgOffset.TextChanged += new System.EventHandler( this.textBox_RegVal_TextChanged );
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point( 12, 21 );
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size( 81, 13 );
			this.label4.TabIndex = 0;
			this.label4.Text = "Message Offset";
			// 
			// groupBox4
			// 
			this.groupBox4.Controls.Add( this.buttonFindMacx64KeyOffset );
			this.groupBox4.Controls.Add( this.buttonFindMacx86KeyOffset );
			this.groupBox4.Controls.Add( this.buttonFindMacPPCKeyOffset );
			this.groupBox4.Controls.Add( this.label5 );
			this.groupBox4.Controls.Add( this.textBoxInt64Pad );
			this.groupBox4.Controls.Add( this.buttonFindOffset );
			this.groupBox4.Controls.Add( this.label3 );
			this.groupBox4.Controls.Add( this.textBoxKeyOffset );
			this.groupBox4.Controls.Add( this.textBoxOverwriteKeyValue );
			this.groupBox4.Controls.Add( this.buttonOverwriteKey );
			this.groupBox4.Controls.Add( this.label1 );
			this.groupBox4.Location = new System.Drawing.Point( 16, 56 );
			this.groupBox4.Name = "groupBox4";
			this.groupBox4.Size = new System.Drawing.Size( 640, 80 );
			this.groupBox4.TabIndex = 9;
			this.groupBox4.TabStop = false;
			this.groupBox4.Text = "Key";
			// 
			// buttonFindMacx64KeyOffset
			// 
			this.buttonFindMacx64KeyOffset.Location = new System.Drawing.Point( 484, 16 );
			this.buttonFindMacx64KeyOffset.Name = "buttonFindMacx64KeyOffset";
			this.buttonFindMacx64KeyOffset.Size = new System.Drawing.Size( 114, 24 );
			this.buttonFindMacx64KeyOffset.TabIndex = 13;
			this.buttonFindMacx64KeyOffset.Text = "Find Mac x64 Offset";
			this.buttonFindMacx64KeyOffset.UseVisualStyleBackColor = true;
			this.buttonFindMacx64KeyOffset.Click += new System.EventHandler( this.buttonFindMacx64KeyOffset_Click );
			// 
			// buttonFindMacx86KeyOffset
			// 
			this.buttonFindMacx86KeyOffset.Location = new System.Drawing.Point( 364, 16 );
			this.buttonFindMacx86KeyOffset.Name = "buttonFindMacx86KeyOffset";
			this.buttonFindMacx86KeyOffset.Size = new System.Drawing.Size( 114, 24 );
			this.buttonFindMacx86KeyOffset.TabIndex = 12;
			this.buttonFindMacx86KeyOffset.Text = "Find Mac x86 Offset";
			this.buttonFindMacx86KeyOffset.UseVisualStyleBackColor = true;
			this.buttonFindMacx86KeyOffset.Click += new System.EventHandler( this.buttonFindMacx86KeyOffset_Click );
			// 
			// buttonFindMacPPCKeyOffset
			// 
			this.buttonFindMacPPCKeyOffset.Location = new System.Drawing.Point( 248, 16 );
			this.buttonFindMacPPCKeyOffset.Name = "buttonFindMacPPCKeyOffset";
			this.buttonFindMacPPCKeyOffset.Size = new System.Drawing.Size( 114, 24 );
			this.buttonFindMacPPCKeyOffset.TabIndex = 11;
			this.buttonFindMacPPCKeyOffset.Text = "Find Mac PPC Offset";
			this.buttonFindMacPPCKeyOffset.UseVisualStyleBackColor = true;
			this.buttonFindMacPPCKeyOffset.Click += new System.EventHandler( this.buttonFindMacPPCKeyOffset_Click );
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point( 393, 55 );
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size( 168, 13 );
			this.label5.TabIndex = 10;
			this.label5.Text = "64-bit Integer Padding (Inter, Intra)";
			// 
			// textBoxInt64Pad
			// 
			this.textBoxInt64Pad.Location = new System.Drawing.Point( 562, 52 );
			this.textBoxInt64Pad.Name = "textBoxInt64Pad";
			this.textBoxInt64Pad.ReadOnly = true;
			this.textBoxInt64Pad.Size = new System.Drawing.Size( 72, 20 );
			this.textBoxInt64Pad.TabIndex = 9;
			this.textBoxInt64Pad.TextChanged += new System.EventHandler( this.textBoxInt64Pad_TextChanged );
			// 
			// buttonFindOffset
			// 
			this.buttonFindOffset.Location = new System.Drawing.Point( 156, 16 );
			this.buttonFindOffset.Name = "buttonFindOffset";
			this.buttonFindOffset.Size = new System.Drawing.Size( 88, 24 );
			this.buttonFindOffset.TabIndex = 0;
			this.buttonFindOffset.Text = "Find Offset";
			this.buttonFindOffset.UseVisualStyleBackColor = true;
			this.buttonFindOffset.Click += new System.EventHandler( this.buttonFindOffset_Click );
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point( 16, 22 );
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size( 56, 13 );
			this.label3.TabIndex = 8;
			this.label3.Text = "Key Offset";
			// 
			// textBoxKeyOffset
			// 
			this.textBoxKeyOffset.Location = new System.Drawing.Point( 78, 19 );
			this.textBoxKeyOffset.Name = "textBoxKeyOffset";
			this.textBoxKeyOffset.Size = new System.Drawing.Size( 72, 20 );
			this.textBoxKeyOffset.TabIndex = 7;
			this.textBoxKeyOffset.TextChanged += new System.EventHandler( this.textBox_RegVal_TextChanged );
			// 
			// textBoxOverwriteKeyValue
			// 
			this.textBoxOverwriteKeyValue.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.textBoxOverwriteKeyValue.Location = new System.Drawing.Point( 72, 48 );
			this.textBoxOverwriteKeyValue.MaxLength = 21;
			this.textBoxOverwriteKeyValue.Name = "textBoxOverwriteKeyValue";
			this.textBoxOverwriteKeyValue.Size = new System.Drawing.Size( 212, 20 );
			this.textBoxOverwriteKeyValue.TabIndex = 3;
			this.textBoxOverwriteKeyValue.TextChanged += new System.EventHandler( this.textBox_RegVal_TextChanged );
			// 
			// buttonOverwriteKey
			// 
			this.buttonOverwriteKey.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.buttonOverwriteKey.Location = new System.Drawing.Point( 290, 47 );
			this.buttonOverwriteKey.Name = "buttonOverwriteKey";
			this.buttonOverwriteKey.Size = new System.Drawing.Size( 96, 23 );
			this.buttonOverwriteKey.TabIndex = 5;
			this.buttonOverwriteKey.Text = "Set Key Value";
			this.buttonOverwriteKey.UseVisualStyleBackColor = true;
			this.buttonOverwriteKey.Click += new System.EventHandler( this.buttonOverwriteKey_Click );
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point( 16, 52 );
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size( 55, 13 );
			this.label1.TabIndex = 4;
			this.label1.Text = "Key Value";
			// 
			// buttonFindOffsetBrowse
			// 
			this.buttonFindOffsetBrowse.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.buttonFindOffsetBrowse.Location = new System.Drawing.Point( 548, 24 );
			this.buttonFindOffsetBrowse.Name = "buttonFindOffsetBrowse";
			this.buttonFindOffsetBrowse.Size = new System.Drawing.Size( 25, 24 );
			this.buttonFindOffsetBrowse.TabIndex = 2;
			this.buttonFindOffsetBrowse.Text = "...";
			this.buttonFindOffsetBrowse.UseVisualStyleBackColor = true;
			this.buttonFindOffsetBrowse.Click += new System.EventHandler( this.buttonFindOffsetBrowse_Click );
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point( 5, 27 );
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size( 72, 13 );
			this.label2.TabIndex = 6;
			this.label2.Text = "EXE File Path";
			// 
			// groupBox2
			// 
			this.groupBox2.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.groupBox2.Controls.Add( this.buttonFindAllOffsets );
			this.groupBox2.Controls.Add( this.label2 );
			this.groupBox2.Controls.Add( this.groupBox4 );
			this.groupBox2.Controls.Add( this.groupBox3 );
			this.groupBox2.Controls.Add( this.buttonFindOffsetBrowse );
			this.groupBox2.Controls.Add( this.textBoxFindOffsetFile );
			this.groupBox2.Location = new System.Drawing.Point( 8, 136 );
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size( 672, 264 );
			this.groupBox2.TabIndex = 4;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "In-EXE";
			// 
			// buttonFindAllOffsets
			// 
			this.buttonFindAllOffsets.Location = new System.Drawing.Point( 579, 24 );
			this.buttonFindAllOffsets.Name = "buttonFindAllOffsets";
			this.buttonFindAllOffsets.Size = new System.Drawing.Size( 85, 24 );
			this.buttonFindAllOffsets.TabIndex = 10;
			this.buttonFindAllOffsets.Text = "Find All Offsets";
			this.buttonFindAllOffsets.UseVisualStyleBackColor = true;
			this.buttonFindAllOffsets.Click += new System.EventHandler( this.buttonFindAllOffsets_Click );
			// 
			// textBoxMsg
			// 
			this.textBoxMsg.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.textBoxMsg.Location = new System.Drawing.Point( 8, 16 );
			this.textBoxMsg.MaxLength = 64;
			this.textBoxMsg.Name = "textBoxMsg";
			this.textBoxMsg.Size = new System.Drawing.Size( 486, 20 );
			this.textBoxMsg.TabIndex = 0;
			this.textBoxMsg.TextChanged += new System.EventHandler( this.textBoxMsg_TextChanged );
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.groupBox1.Controls.Add( this.labelGenMsgLen );
			this.groupBox1.Controls.Add( this.textBoxOutMsg );
			this.groupBox1.Controls.Add( this.textBoxMsg );
			this.groupBox1.Controls.Add( this.button1 );
			this.groupBox1.Location = new System.Drawing.Point( 8, 8 );
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size( 678, 120 );
			this.groupBox1.TabIndex = 3;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Generate Message";
			// 
			// labelGenMsgLen
			// 
			this.labelGenMsgLen.AutoSize = true;
			this.labelGenMsgLen.Location = new System.Drawing.Point( 512, 16 );
			this.labelGenMsgLen.Name = "labelGenMsgLen";
			this.labelGenMsgLen.Size = new System.Drawing.Size( 52, 13 );
			this.labelGenMsgLen.TabIndex = 3;
			this.labelGenMsgLen.Text = "Length: 0";
			// 
			// textBoxOutMsg
			// 
			this.textBoxOutMsg.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
						| System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.textBoxOutMsg.Location = new System.Drawing.Point( 8, 40 );
			this.textBoxOutMsg.Multiline = true;
			this.textBoxOutMsg.Name = "textBoxOutMsg";
			this.textBoxOutMsg.ReadOnly = true;
			this.textBoxOutMsg.Size = new System.Drawing.Size( 662, 72 );
			this.textBoxOutMsg.TabIndex = 2;
			// 
			// button1
			// 
			this.button1.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.button1.Location = new System.Drawing.Point( 576, 16 );
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size( 96, 24 );
			this.button1.TabIndex = 1;
			this.button1.Text = "Generate";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler( this.button1_Click );
			// 
			// groupBox5
			// 
			this.groupBox5.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.groupBox5.Controls.Add( this.buttonReadKey );
			this.groupBox5.Controls.Add( this.label6 );
			this.groupBox5.Controls.Add( this.textBoxReadKey );
			this.groupBox5.Controls.Add( this.textBoxGenKeyVal );
			this.groupBox5.Controls.Add( this.buttonMakeKey );
			this.groupBox5.Location = new System.Drawing.Point( 8, 408 );
			this.groupBox5.Name = "groupBox5";
			this.groupBox5.Size = new System.Drawing.Size( 672, 96 );
			this.groupBox5.TabIndex = 5;
			this.groupBox5.TabStop = false;
			this.groupBox5.Text = "Key";
			// 
			// buttonReadKey
			// 
			this.buttonReadKey.Location = new System.Drawing.Point( 616, 64 );
			this.buttonReadKey.Name = "buttonReadKey";
			this.buttonReadKey.Size = new System.Drawing.Size( 48, 24 );
			this.buttonReadKey.TabIndex = 4;
			this.buttonReadKey.Text = "Read";
			this.buttonReadKey.UseVisualStyleBackColor = true;
			this.buttonReadKey.Click += new System.EventHandler( this.buttonReadKey_Click );
			// 
			// label6
			// 
			this.label6.Location = new System.Drawing.Point( 56, 56 );
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size( 136, 32 );
			this.label6.TabIndex = 3;
			this.label6.Text = "Read key from file (Drop file in box or press button) :";
			// 
			// textBoxReadKey
			// 
			this.textBoxReadKey.AllowDrop = true;
			this.textBoxReadKey.Location = new System.Drawing.Point( 200, 64 );
			this.textBoxReadKey.Name = "textBoxReadKey";
			this.textBoxReadKey.ReadOnly = true;
			this.textBoxReadKey.Size = new System.Drawing.Size( 408, 20 );
			this.textBoxReadKey.TabIndex = 2;
			this.textBoxReadKey.DragDrop += new System.Windows.Forms.DragEventHandler( this.textBoxReadKey_DragDrop );
			this.textBoxReadKey.DragEnter += new System.Windows.Forms.DragEventHandler( this.textBoxReadKey_DragEnter );
			// 
			// textBoxGenKeyVal
			// 
			this.textBoxGenKeyVal.Location = new System.Drawing.Point( 232, 32 );
			this.textBoxGenKeyVal.Name = "textBoxGenKeyVal";
			this.textBoxGenKeyVal.Size = new System.Drawing.Size( 176, 20 );
			this.textBoxGenKeyVal.TabIndex = 1;
			// 
			// buttonMakeKey
			// 
			this.buttonMakeKey.Location = new System.Drawing.Point( 416, 32 );
			this.buttonMakeKey.Name = "buttonMakeKey";
			this.buttonMakeKey.Size = new System.Drawing.Size( 144, 24 );
			this.buttonMakeKey.TabIndex = 0;
			this.buttonMakeKey.Text = "Make Key";
			this.buttonMakeKey.UseVisualStyleBackColor = true;
			this.buttonMakeKey.Click += new System.EventHandler( this.buttonMakeKey_Click );
			// 
			// groupBox6
			// 
			this.groupBox6.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.groupBox6.Controls.Add( this.buttonFindMacPicOffsets );
			this.groupBox6.Controls.Add( this.buttonOverwriteCustomPicMsg );
			this.groupBox6.Controls.Add( this.textBoxCustomPicMsg );
			this.groupBox6.Controls.Add( this.textBoxCustomPicMsgOffset );
			this.groupBox6.Controls.Add( this.label8 );
			this.groupBox6.Controls.Add( this.buttonFindCustomPicOffset );
			this.groupBox6.Controls.Add( this.textBoxCustomPicOffset );
			this.groupBox6.Controls.Add( this.labelCustomPicOffset );
			this.groupBox6.Controls.Add( this.buttonEmbedCustomPic );
			this.groupBox6.Controls.Add( this.label7 );
			this.groupBox6.Controls.Add( this.buttonPickCustomPic );
			this.groupBox6.Controls.Add( this.textBoxCustomPic );
			this.groupBox6.Controls.Add( this.pictureBoxCustomPic );
			this.groupBox6.Location = new System.Drawing.Point( 8, 510 );
			this.groupBox6.Name = "groupBox6";
			this.groupBox6.Size = new System.Drawing.Size( 672, 186 );
			this.groupBox6.TabIndex = 6;
			this.groupBox6.TabStop = false;
			this.groupBox6.Text = "Custom Picture";
			// 
			// buttonFindMacPicOffsets
			// 
			this.buttonFindMacPicOffsets.Location = new System.Drawing.Point( 294, 93 );
			this.buttonFindMacPicOffsets.Name = "buttonFindMacPicOffsets";
			this.buttonFindMacPicOffsets.Size = new System.Drawing.Size( 108, 24 );
			this.buttonFindMacPicOffsets.TabIndex = 18;
			this.buttonFindMacPicOffsets.Text = "Find Mac Offsets";
			this.buttonFindMacPicOffsets.UseVisualStyleBackColor = true;
			this.buttonFindMacPicOffsets.Click += new System.EventHandler( this.buttonFindMacPicOffsets_Click );
			// 
			// buttonOverwriteCustomPicMsg
			// 
			this.buttonOverwriteCustomPicMsg.Location = new System.Drawing.Point( 294, 148 );
			this.buttonOverwriteCustomPicMsg.Name = "buttonOverwriteCustomPicMsg";
			this.buttonOverwriteCustomPicMsg.Size = new System.Drawing.Size( 128, 24 );
			this.buttonOverwriteCustomPicMsg.TabIndex = 17;
			this.buttonOverwriteCustomPicMsg.Text = "Overwrite Message";
			this.buttonOverwriteCustomPicMsg.UseVisualStyleBackColor = true;
			this.buttonOverwriteCustomPicMsg.Click += new System.EventHandler( this.buttonOverwriteCustomPicMsg_Click );
			// 
			// textBoxCustomPicMsg
			// 
			this.textBoxCustomPicMsg.Location = new System.Drawing.Point( 11, 151 );
			this.textBoxCustomPicMsg.MaxLength = 32;
			this.textBoxCustomPicMsg.Name = "textBoxCustomPicMsg";
			this.textBoxCustomPicMsg.Size = new System.Drawing.Size( 277, 20 );
			this.textBoxCustomPicMsg.TabIndex = 16;
			// 
			// textBoxCustomPicMsgOffset
			// 
			this.textBoxCustomPicMsgOffset.Location = new System.Drawing.Point( 105, 115 );
			this.textBoxCustomPicMsgOffset.Name = "textBoxCustomPicMsgOffset";
			this.textBoxCustomPicMsgOffset.Size = new System.Drawing.Size( 88, 20 );
			this.textBoxCustomPicMsgOffset.TabIndex = 15;
			this.textBoxCustomPicMsgOffset.TextChanged += new System.EventHandler( this.textBoxCustomPicMsgOffset_TextChanged );
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point( 25, 118 );
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size( 81, 13 );
			this.label8.TabIndex = 14;
			this.label8.Text = "Message Offset";
			// 
			// buttonFindCustomPicOffset
			// 
			this.buttonFindCustomPicOffset.Location = new System.Drawing.Point( 200, 93 );
			this.buttonFindCustomPicOffset.Name = "buttonFindCustomPicOffset";
			this.buttonFindCustomPicOffset.Size = new System.Drawing.Size( 88, 24 );
			this.buttonFindCustomPicOffset.TabIndex = 13;
			this.buttonFindCustomPicOffset.Text = "Find Offsets";
			this.buttonFindCustomPicOffset.UseVisualStyleBackColor = true;
			this.buttonFindCustomPicOffset.Click += new System.EventHandler( this.buttonFindCustomPicOffset_Click );
			// 
			// textBoxCustomPicOffset
			// 
			this.textBoxCustomPicOffset.Location = new System.Drawing.Point( 105, 79 );
			this.textBoxCustomPicOffset.Name = "textBoxCustomPicOffset";
			this.textBoxCustomPicOffset.Size = new System.Drawing.Size( 88, 20 );
			this.textBoxCustomPicOffset.TabIndex = 12;
			this.textBoxCustomPicOffset.TextChanged += new System.EventHandler( this.textBoxCustomPicOffset_TextChanged );
			// 
			// labelCustomPicOffset
			// 
			this.labelCustomPicOffset.AutoSize = true;
			this.labelCustomPicOffset.Location = new System.Drawing.Point( 8, 82 );
			this.labelCustomPicOffset.Name = "labelCustomPicOffset";
			this.labelCustomPicOffset.Size = new System.Drawing.Size( 91, 13 );
			this.labelCustomPicOffset.TabIndex = 11;
			this.labelCustomPicOffset.Text = "Custom Pic Offset";
			// 
			// buttonEmbedCustomPic
			// 
			this.buttonEmbedCustomPic.Location = new System.Drawing.Point( 433, 55 );
			this.buttonEmbedCustomPic.Name = "buttonEmbedCustomPic";
			this.buttonEmbedCustomPic.Size = new System.Drawing.Size( 75, 23 );
			this.buttonEmbedCustomPic.TabIndex = 10;
			this.buttonEmbedCustomPic.Text = "Embed";
			this.buttonEmbedCustomPic.UseVisualStyleBackColor = true;
			this.buttonEmbedCustomPic.Click += new System.EventHandler( this.buttonEmbedCustomPic_Click );
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point( 13, 32 );
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size( 61, 13 );
			this.label7.TabIndex = 9;
			this.label7.Text = "Image Path";
			// 
			// buttonPickCustomPic
			// 
			this.buttonPickCustomPic.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.buttonPickCustomPic.Location = new System.Drawing.Point( 461, 25 );
			this.buttonPickCustomPic.Name = "buttonPickCustomPic";
			this.buttonPickCustomPic.Size = new System.Drawing.Size( 37, 24 );
			this.buttonPickCustomPic.TabIndex = 8;
			this.buttonPickCustomPic.Text = "...";
			this.buttonPickCustomPic.UseVisualStyleBackColor = true;
			this.buttonPickCustomPic.Click += new System.EventHandler( this.buttonPickCustomPic_Click );
			// 
			// textBoxCustomPic
			// 
			this.textBoxCustomPic.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.textBoxCustomPic.Location = new System.Drawing.Point( 88, 29 );
			this.textBoxCustomPic.Name = "textBoxCustomPic";
			this.textBoxCustomPic.Size = new System.Drawing.Size( 364, 20 );
			this.textBoxCustomPic.TabIndex = 7;
			// 
			// pictureBoxCustomPic
			// 
			this.pictureBoxCustomPic.Location = new System.Drawing.Point( 528, 29 );
			this.pictureBoxCustomPic.Name = "pictureBoxCustomPic";
			this.pictureBoxCustomPic.Size = new System.Drawing.Size( 128, 128 );
			this.pictureBoxCustomPic.TabIndex = 0;
			this.pictureBoxCustomPic.TabStop = false;
			// 
			// openFileDialogCustomPic
			// 
			this.openFileDialogCustomPic.FileName = "openFileDialog1";
			this.openFileDialogCustomPic.Filter = "Image Files|*.jpg;*.png;*.bmp|All Files|*.*";
			// 
			// groupBox7
			// 
			this.groupBox7.Controls.Add( this.dateTimePickerExpiration );
			this.groupBox7.Controls.Add( this.buttonSetExpirationDate );
			this.groupBox7.Controls.Add( this.buttonFindExpirationDateOffset );
			this.groupBox7.Controls.Add( this.textBoxExpirationDateOffset );
			this.groupBox7.Controls.Add( this.label9 );
			this.groupBox7.Location = new System.Drawing.Point( 8, 702 );
			this.groupBox7.Name = "groupBox7";
			this.groupBox7.Size = new System.Drawing.Size( 672, 60 );
			this.groupBox7.TabIndex = 7;
			this.groupBox7.TabStop = false;
			this.groupBox7.Text = "Evaluation Expiration Date";
			// 
			// dateTimePickerExpiration
			// 
			this.dateTimePickerExpiration.Format = System.Windows.Forms.DateTimePickerFormat.Short;
			this.dateTimePickerExpiration.Location = new System.Drawing.Point( 308, 24 );
			this.dateTimePickerExpiration.MaxDate = new System.DateTime( 2200, 12, 31, 0, 0, 0, 0 );
			this.dateTimePickerExpiration.MinDate = new System.DateTime( 2010, 1, 1, 0, 0, 0, 0 );
			this.dateTimePickerExpiration.Name = "dateTimePickerExpiration";
			this.dateTimePickerExpiration.Size = new System.Drawing.Size( 114, 20 );
			this.dateTimePickerExpiration.TabIndex = 19;
			// 
			// buttonSetExpirationDate
			// 
			this.buttonSetExpirationDate.Location = new System.Drawing.Point( 435, 20 );
			this.buttonSetExpirationDate.Name = "buttonSetExpirationDate";
			this.buttonSetExpirationDate.Size = new System.Drawing.Size( 138, 24 );
			this.buttonSetExpirationDate.TabIndex = 18;
			this.buttonSetExpirationDate.Text = "Set Expiration Date";
			this.buttonSetExpirationDate.UseVisualStyleBackColor = true;
			this.buttonSetExpirationDate.Click += new System.EventHandler( this.buttonSetExpirationDate_Click );
			// 
			// buttonFindExpirationDateOffset
			// 
			this.buttonFindExpirationDateOffset.Location = new System.Drawing.Point( 172, 20 );
			this.buttonFindExpirationDateOffset.Name = "buttonFindExpirationDateOffset";
			this.buttonFindExpirationDateOffset.Size = new System.Drawing.Size( 88, 24 );
			this.buttonFindExpirationDateOffset.TabIndex = 16;
			this.buttonFindExpirationDateOffset.Text = "Find Offset";
			this.buttonFindExpirationDateOffset.UseVisualStyleBackColor = true;
			this.buttonFindExpirationDateOffset.Click += new System.EventHandler( this.buttonFindExpirationDateOffset_Click );
			// 
			// textBoxExpirationDateOffset
			// 
			this.textBoxExpirationDateOffset.Location = new System.Drawing.Point( 78, 23 );
			this.textBoxExpirationDateOffset.Name = "textBoxExpirationDateOffset";
			this.textBoxExpirationDateOffset.Size = new System.Drawing.Size( 88, 20 );
			this.textBoxExpirationDateOffset.TabIndex = 15;
			this.textBoxExpirationDateOffset.TextChanged += new System.EventHandler( this.textBoxExpirationDateOffset_TextChanged );
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point( 8, 26 );
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size( 64, 13 );
			this.label9.TabIndex = 14;
			this.label9.Text = "Date Offset:";
			// 
			// buttonGenerateExeInfo
			// 
			this.buttonGenerateExeInfo.Location = new System.Drawing.Point( 137, 769 );
			this.buttonGenerateExeInfo.Name = "buttonGenerateExeInfo";
			this.buttonGenerateExeInfo.Size = new System.Drawing.Size( 131, 23 );
			this.buttonGenerateExeInfo.TabIndex = 8;
			this.buttonGenerateExeInfo.Text = "Generate EXE Info";
			this.buttonGenerateExeInfo.UseVisualStyleBackColor = true;
			this.buttonGenerateExeInfo.Click += new System.EventHandler( this.buttonGenerateExeInfo_Click );
			// 
			// buttonGenerateMacExeInfo
			// 
			this.buttonGenerateMacExeInfo.Location = new System.Drawing.Point( 285, 769 );
			this.buttonGenerateMacExeInfo.Name = "buttonGenerateMacExeInfo";
			this.buttonGenerateMacExeInfo.Size = new System.Drawing.Size( 131, 23 );
			this.buttonGenerateMacExeInfo.TabIndex = 9;
			this.buttonGenerateMacExeInfo.Text = "Generate Mac EXE Info";
			this.buttonGenerateMacExeInfo.UseVisualStyleBackColor = true;
			this.buttonGenerateMacExeInfo.Click += new System.EventHandler( this.buttonGenerateMacExeInfo_Click );
			// 
			// saveFileDialogInfo
			// 
			this.saveFileDialogInfo.DefaultExt = "txt";
			this.saveFileDialogInfo.Filter = "Text Files|*.txt|All Files|*.*";
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size( 694, 796 );
			this.Controls.Add( this.buttonGenerateMacExeInfo );
			this.Controls.Add( this.buttonGenerateExeInfo );
			this.Controls.Add( this.groupBox7 );
			this.Controls.Add( this.groupBox6 );
			this.Controls.Add( this.groupBox5 );
			this.Controls.Add( this.groupBox2 );
			this.Controls.Add( this.groupBox1 );
			this.Name = "Form1";
			this.Text = "Prime Time Message Generator";
			this.Load += new System.EventHandler( this.Form1_Load );
			this.groupBox3.ResumeLayout( false );
			this.groupBox3.PerformLayout();
			this.groupBox4.ResumeLayout( false );
			this.groupBox4.PerformLayout();
			this.groupBox2.ResumeLayout( false );
			this.groupBox2.PerformLayout();
			this.groupBox1.ResumeLayout( false );
			this.groupBox1.PerformLayout();
			this.groupBox5.ResumeLayout( false );
			this.groupBox5.PerformLayout();
			this.groupBox6.ResumeLayout( false );
			this.groupBox6.PerformLayout();
			( (System.ComponentModel.ISupportInitialize)( this.pictureBoxCustomPic ) ).EndInit();
			this.groupBox7.ResumeLayout( false );
			this.groupBox7.PerformLayout();
			this.ResumeLayout( false );

		}

		#endregion

		private System.Windows.Forms.OpenFileDialog openFileDialogFindOffset;
		private System.Windows.Forms.TextBox textBoxFindOffsetFile;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.GroupBox groupBox4;
		private System.Windows.Forms.Button buttonFindOffset;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox textBoxKeyOffset;
		private System.Windows.Forms.Button buttonFindOffsetBrowse;
		private System.Windows.Forms.TextBox textBoxOverwriteKeyValue;
		private System.Windows.Forms.Button buttonOverwriteKey;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.TextBox textBoxMsg;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox textBoxOutMsg;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Button buttonFindMsgOffset;
		private System.Windows.Forms.TextBox textBoxMsgOffset;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Button buttonOverwriteMsg;
		private System.Windows.Forms.TextBox textBoxOverwriteMsg;
		private System.Windows.Forms.Label labelGenMsgLen;
		private System.Windows.Forms.Label labelOverwriteMsgLen;
		private System.Windows.Forms.TextBox textBoxInt64Pad;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.GroupBox groupBox5;
		private System.Windows.Forms.Button buttonMakeKey;
		private System.Windows.Forms.TextBox textBoxGenKeyVal;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.TextBox textBoxReadKey;
		private System.Windows.Forms.Button buttonReadKey;
		private System.Windows.Forms.GroupBox groupBox6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Button buttonPickCustomPic;
		private System.Windows.Forms.TextBox textBoxCustomPic;
		private System.Windows.Forms.PictureBox pictureBoxCustomPic;
		private System.Windows.Forms.OpenFileDialog openFileDialogCustomPic;
		private System.Windows.Forms.Button buttonEmbedCustomPic;
		private System.Windows.Forms.Button buttonFindCustomPicOffset;
		private System.Windows.Forms.TextBox textBoxCustomPicOffset;
		private System.Windows.Forms.Label labelCustomPicOffset;
		private System.Windows.Forms.TextBox textBoxCustomPicMsgOffset;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Button buttonOverwriteCustomPicMsg;
		private System.Windows.Forms.TextBox textBoxCustomPicMsg;
		private System.Windows.Forms.Button buttonFindAllOffsets;
		private System.Windows.Forms.GroupBox groupBox7;
		private System.Windows.Forms.DateTimePicker dateTimePickerExpiration;
		private System.Windows.Forms.Button buttonSetExpirationDate;
		private System.Windows.Forms.Button buttonFindExpirationDateOffset;
		private System.Windows.Forms.TextBox textBoxExpirationDateOffset;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Button buttonFindMacMsgOffset;
		private System.Windows.Forms.Button buttonOverwiteMacMsg;
		private System.Windows.Forms.Button buttonFindMacx86MsgOffset;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.Button buttonFindMacx64KeyOffset;
		private System.Windows.Forms.Button buttonFindMacx86KeyOffset;
		private System.Windows.Forms.Button buttonFindMacPPCKeyOffset;
		private System.Windows.Forms.Button buttonFindMacPicOffsets;
		private System.Windows.Forms.Button buttonGenerateExeInfo;
		private System.Windows.Forms.Button buttonGenerateMacExeInfo;
		private System.Windows.Forms.SaveFileDialog saveFileDialogInfo;

	}
}

