using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;

namespace FontEditor
{
	//---------------------------------------------------------------------------------------------
	/// <summary>
	/// The main form for the font editor
	/// </summary>
	//---------------------------------------------------------------------------------------------
	public class FormMain : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.GroupBox groupBoxExampleText;
		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.MenuItem menuItem8;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.StatusBar statusBarMain;
		private System.Windows.Forms.MenuItem menuItemFontImage;
		private System.Windows.Forms.MenuItem menuItem9;
		private System.Windows.Forms.MenuItem menuItemEditUndo;
		private System.Windows.Forms.MenuItem menuItemEditRedo;
		private System.Windows.Forms.ListBox listBoxCharacters;
		private System.Windows.Forms.TextBox textBoxExampleText;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.PictureBox pictureBoxExampleTextBG;
		private System.Windows.Forms.PictureBox pictureBoxExampleText;
		private System.Windows.Forms.TextBox textBoxCharHeight;
		private System.Windows.Forms.MenuItem menuItem11;
		private System.Windows.Forms.MenuItem menuItemFileNew;
		private System.Windows.Forms.MenuItem menuItemFileOpen;
		private System.Windows.Forms.MenuItem menuItemFileSave;
		private System.Windows.Forms.MenuItem menuItemFileSaveAs;
		private System.Windows.Forms.MenuItem menuItemFileExit;
		private PictureBoxZoom.PictureBoxZoom pictureBoxZoomFontImage;
		private System.Windows.Forms.GroupBox groupBoxFontImage;
		private System.Windows.Forms.StatusBarPanel statusBarPanelInfo;
		private System.Windows.Forms.StatusBarPanel statusBarPanelMousePos;
		private Label label3;
		
		public const string MESSAGE_BOX_TITLE = "Prime Time Font Editor";

		private Color TRANS_COLOR = Color.FromArgb(255, 0, 255);
		private IContainer components;

		/// <summary>
		/// The font we are working on
		/// </summary>
		public static PTFont sm_Font = new PTFont();		

		/// <summary> The edit command manager to keep track of undo and redo </summary>
		private CSBase.EditCmdMgr m_EditCmdMgr = null;

		/// <summary> The array of active glpyh rectangles to draw </summary>
		private System.Collections.ArrayList m_GlyphRects = new System.Collections.ArrayList();

		/// <summary> Whether or not we are dragging a glyph rectangle(s) around </summary>
		private bool m_IsDraggingGlyph = false;
		
		/// <summary> The position of the cursor, in image coordinates, when the dragging of
		/// glyphs started </summary>
		private Point m_GlyphDragLastPos = new Point();

		/// <summary> True if there are changes that need to be saved </summary>
		private bool m_FontHasChanges = false;

		/// <summary> The full path to the font file </summary>
		private string m_sFontFilePath = "";
		private TextBox textBoxLetterSpace;
		private Label label5;
		private TextBox textBoxFontName;
		private MenuItem menuItem2;
		private MenuItem menuItem3;
		private MenuItem menuItem4;
		private GroupBox groupBox1;
		private TextBox textBoxResourceIndexPath;
		private Button buttonBrowseIndexPath;
		private OpenFileDialog openToolsResourceFileDialog;

		/// <summary>
		/// The original window text
		/// </summary>
		private readonly string m_sOrigTitle = "";


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The default constructor
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public FormMain()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			// Initialize the edit command manager
			m_EditCmdMgr = new CSBase.EditCmdMgr( new CSBase.EditCmdMgr.ExecuteCmdFunc( ExecCmd ),
				new CSBase.EditCmdMgr.ExecuteCmdFunc( UndoCmd ) );

			// Store the window title
			m_sOrigTitle = Text;

			// Update the menu items to start with
			UpdateEditCmdMenuItems();

			textBoxResourceIndexPath.Text = Settings.Default.ToolsIndexPath;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Execute an edit command
		/// </summary>
		/// <param name="editCmd"> The edit command to execute </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void ExecCmd(CSBase.EditCmdMgr.IEditCmdBase editCmd)
		{
			//editCmd.Execute();

			// If the edit command changes the font's available characters
			if( editCmd is EditCmdModFontChars )
			{
				EditCmdModFontChars modCmd = editCmd as EditCmdModFontChars;
				
				// Update the character list box
				m_GlyphRects.Clear();
				listBoxCharacters.BeginUpdate();
				listBoxCharacters.Items.Clear();
				foreach( char strChar in sm_Font.CharString )
					listBoxCharacters.Items.Add( strChar );
				listBoxCharacters.EndUpdate();

				// If we added characters with this command
				if( modCmd.IsAddCmd )
					SelectChars( modCmd.ModChars );
			}
			// If the edit command changes the font's available characters
			else if (editCmd is EditCmdModFontCharRects)
			{
				EditCmdModFontCharRects modCmd = editCmd as EditCmdModFontCharRects;

				// Update the character list box
				m_GlyphRects.Clear();
				listBoxCharacters.BeginUpdate();
				listBoxCharacters.Items.Clear();
				foreach (char strChar in sm_Font.CharString)
					listBoxCharacters.Items.Add(strChar);
				listBoxCharacters.EndUpdate();

				// Redraw the font image and example text
				pictureBoxZoomFontImage.Refresh();
				pictureBoxExampleText.Refresh();
			}
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Undo an edit command
		/// </summary>
		/// <param name="editCmd"> The edit command to undo </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void UndoCmd(CSBase.EditCmdMgr.IEditCmdBase editCmd)
		{
			//editCmd.Undo();

			// If the edit command changes the font's available characters
			if( editCmd is EditCmdModFontChars )
			{
				EditCmdModFontChars modCmd = editCmd as EditCmdModFontChars;
				
				// Update the character list box
				m_GlyphRects.Clear();
				listBoxCharacters.BeginUpdate();
				listBoxCharacters.Items.Clear();
				foreach( char strChar in sm_Font.CharString )
					listBoxCharacters.Items.Add( strChar );
				listBoxCharacters.EndUpdate();

				// If we are undoing a remove command then update selection
				if( !modCmd.IsAddCmd )
					SelectChars( modCmd.ModChars );
			}
			// If the edit command changes the font's available characters
			else if (editCmd is EditCmdModFontCharRects)
			{
				EditCmdModFontCharRects modCmd = editCmd as EditCmdModFontCharRects;

				// Update the character list box
				m_GlyphRects.Clear();
				listBoxCharacters.BeginUpdate();
				listBoxCharacters.Items.Clear();
				foreach (char strChar in sm_Font.CharString)
					listBoxCharacters.Items.Add(strChar);
				listBoxCharacters.EndUpdate();

				// Redraw the font image and example text
				pictureBoxZoomFontImage.Refresh();
				pictureBoxExampleText.Refresh();
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Update the edit command menu items to reflect the current state of edit commands.
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void UpdateEditCmdMenuItems()
		{
			// Update the menu enabled status
			menuItemEditUndo.Enabled = m_EditCmdMgr.CanUndoCommand();
			menuItemEditRedo.Enabled = m_EditCmdMgr.CanRedoCommand();

			// Update the text
			menuItemEditUndo.Text = m_EditCmdMgr.GetUndoMenuText();
			menuItemEditRedo.Text = m_EditCmdMgr.GetRedoMenuText();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Select characters in the character list box
		/// </summary>
		/// <param name="chars"> The characters to select </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void SelectChars( string chars )
		{
			// Unselect everything
			listBoxCharacters.SelectedIndex = -1;
			
			// Go through the list box items and select the characters
			for( int curItemIndex = 0; curItemIndex < listBoxCharacters.Items.Count; ++curItemIndex )
			{
				// Get this item's character
				char curChar = listBoxCharacters.Items[curItemIndex].ToString()[0];

				// Set the selection for this item
				listBoxCharacters.SetSelected( curItemIndex, chars.IndexOf(curChar) != -1 );
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Update the drawn glyph rectangles based on the selection
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void UpdateGlyphsFromSel()
		{
			// Clear the current rectangles
			m_GlyphRects.Clear();

			// Get the selected characters
			string selChars = GetSelChars();

			// Go through the characters and update the glyphs
			for( int curCharIndex = 0; curCharIndex < selChars.Length; ++curCharIndex )
			{
				// Get this item's character
				char curChar = selChars[curCharIndex];

				// Update the glyph rectangle list
				m_GlyphRects.Add( new GlyphRect( curChar, sm_Font[curChar] ) );
			}

			// If there are no glyphs selected
			if( selChars.Length == 0 )
				statusBarPanelInfo.Text = "";
			// Else add some help text
			else
				statusBarPanelInfo.Text = "Move box around with W,A,S,D or hold SHIFT and use A,D to resize";
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
				if (components != null) 
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager( typeof( FormMain ) );
			this.label1 = new System.Windows.Forms.Label();
			this.groupBoxExampleText = new System.Windows.Forms.GroupBox();
			this.pictureBoxExampleTextBG = new System.Windows.Forms.PictureBox();
			this.label4 = new System.Windows.Forms.Label();
			this.textBoxExampleText = new System.Windows.Forms.TextBox();
			this.pictureBoxExampleText = new System.Windows.Forms.PictureBox();
			this.mainMenu1 = new System.Windows.Forms.MainMenu( this.components );
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.menuItemFileNew = new System.Windows.Forms.MenuItem();
			this.menuItemFileOpen = new System.Windows.Forms.MenuItem();
			this.menuItemFileSave = new System.Windows.Forms.MenuItem();
			this.menuItemFileSaveAs = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.menuItemFileExit = new System.Windows.Forms.MenuItem();
			this.menuItem9 = new System.Windows.Forms.MenuItem();
			this.menuItemEditUndo = new System.Windows.Forms.MenuItem();
			this.menuItemEditRedo = new System.Windows.Forms.MenuItem();
			this.menuItem8 = new System.Windows.Forms.MenuItem();
			this.menuItemFontImage = new System.Windows.Forms.MenuItem();
			this.menuItem11 = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.label2 = new System.Windows.Forms.Label();
			this.textBoxCharHeight = new System.Windows.Forms.TextBox();
			this.statusBarMain = new System.Windows.Forms.StatusBar();
			this.statusBarPanelInfo = new System.Windows.Forms.StatusBarPanel();
			this.statusBarPanelMousePos = new System.Windows.Forms.StatusBarPanel();
			this.listBoxCharacters = new System.Windows.Forms.ListBox();
			this.pictureBoxZoomFontImage = new PictureBoxZoom.PictureBoxZoom();
			this.groupBoxFontImage = new System.Windows.Forms.GroupBox();
			this.label3 = new System.Windows.Forms.Label();
			this.textBoxLetterSpace = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.textBoxFontName = new System.Windows.Forms.TextBox();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.textBoxResourceIndexPath = new System.Windows.Forms.TextBox();
			this.buttonBrowseIndexPath = new System.Windows.Forms.Button();
			this.openToolsResourceFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.groupBoxExampleText.SuspendLayout();
			( (System.ComponentModel.ISupportInitialize)( this.pictureBoxExampleTextBG ) ).BeginInit();
			( (System.ComponentModel.ISupportInitialize)( this.pictureBoxExampleText ) ).BeginInit();
			( (System.ComponentModel.ISupportInitialize)( this.statusBarPanelInfo ) ).BeginInit();
			( (System.ComponentModel.ISupportInitialize)( this.statusBarPanelMousePos ) ).BeginInit();
			this.groupBoxFontImage.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point( 8, 40 );
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size( 72, 40 );
			this.label1.TabIndex = 2;
			this.label1.Text = "Characters :  (Type to add characters)";
			// 
			// groupBoxExampleText
			// 
			this.groupBoxExampleText.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.groupBoxExampleText.Controls.Add( this.pictureBoxExampleTextBG );
			this.groupBoxExampleText.Controls.Add( this.label4 );
			this.groupBoxExampleText.Controls.Add( this.textBoxExampleText );
			this.groupBoxExampleText.Controls.Add( this.pictureBoxExampleText );
			this.groupBoxExampleText.Location = new System.Drawing.Point( 8, 448 );
			this.groupBoxExampleText.Name = "groupBoxExampleText";
			this.groupBoxExampleText.Size = new System.Drawing.Size( 760, 96 );
			this.groupBoxExampleText.TabIndex = 4;
			this.groupBoxExampleText.TabStop = false;
			this.groupBoxExampleText.Text = "Example Text";
			// 
			// pictureBoxExampleTextBG
			// 
			this.pictureBoxExampleTextBG.BackColor = System.Drawing.Color.FromArgb( ( (int)( ( (byte)( 255 ) ) ) ), ( (int)( ( (byte)( 128 ) ) ) ), ( (int)( ( (byte)( 0 ) ) ) ) );
			this.pictureBoxExampleTextBG.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBoxExampleTextBG.Location = new System.Drawing.Point( 696, 16 );
			this.pictureBoxExampleTextBG.Name = "pictureBoxExampleTextBG";
			this.pictureBoxExampleTextBG.Size = new System.Drawing.Size( 56, 16 );
			this.pictureBoxExampleTextBG.TabIndex = 3;
			this.pictureBoxExampleTextBG.TabStop = false;
			this.pictureBoxExampleTextBG.Click += new System.EventHandler( this.pictureBoxExampleTextBG_Click );
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point( 640, 16 );
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size( 64, 16 );
			this.label4.TabIndex = 2;
			this.label4.Text = "BG Color :";
			// 
			// textBoxExampleText
			// 
			this.textBoxExampleText.Location = new System.Drawing.Point( 8, 16 );
			this.textBoxExampleText.Name = "textBoxExampleText";
			this.textBoxExampleText.Size = new System.Drawing.Size( 624, 20 );
			this.textBoxExampleText.TabIndex = 0;
			this.textBoxExampleText.Text = "The fat brown FOX jumped ABC abc DEF def";
			this.textBoxExampleText.TextChanged += new System.EventHandler( this.textBoxExampleText_TextChanged );
			// 
			// pictureBoxExampleText
			// 
			this.pictureBoxExampleText.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
						| System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.pictureBoxExampleText.BackColor = System.Drawing.Color.FromArgb( ( (int)( ( (byte)( 255 ) ) ) ), ( (int)( ( (byte)( 128 ) ) ) ), ( (int)( ( (byte)( 0 ) ) ) ) );
			this.pictureBoxExampleText.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBoxExampleText.Location = new System.Drawing.Point( 8, 40 );
			this.pictureBoxExampleText.Name = "pictureBoxExampleText";
			this.pictureBoxExampleText.Size = new System.Drawing.Size( 744, 48 );
			this.pictureBoxExampleText.TabIndex = 0;
			this.pictureBoxExampleText.TabStop = false;
			this.pictureBoxExampleText.Paint += new System.Windows.Forms.PaintEventHandler( this.pictureBoxExampleText_Paint );
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange( new System.Windows.Forms.MenuItem[] {
            this.menuItem1,
            this.menuItem9,
            this.menuItem8} );
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange( new System.Windows.Forms.MenuItem[] {
            this.menuItemFileNew,
            this.menuItemFileOpen,
            this.menuItemFileSave,
            this.menuItemFileSaveAs,
            this.menuItem6,
            this.menuItemFileExit} );
			this.menuItem1.Text = "&File";
			// 
			// menuItemFileNew
			// 
			this.menuItemFileNew.Index = 0;
			this.menuItemFileNew.Shortcut = System.Windows.Forms.Shortcut.CtrlN;
			this.menuItemFileNew.Text = "&New";
			this.menuItemFileNew.Click += new System.EventHandler( this.menuItemFileNew_Click );
			// 
			// menuItemFileOpen
			// 
			this.menuItemFileOpen.Index = 1;
			this.menuItemFileOpen.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
			this.menuItemFileOpen.Text = "&Open...";
			this.menuItemFileOpen.Click += new System.EventHandler( this.menuItemFileOpen_Click );
			// 
			// menuItemFileSave
			// 
			this.menuItemFileSave.Index = 2;
			this.menuItemFileSave.Shortcut = System.Windows.Forms.Shortcut.CtrlS;
			this.menuItemFileSave.Text = "&Save";
			this.menuItemFileSave.Click += new System.EventHandler( this.menuItemFileSave_Click );
			// 
			// menuItemFileSaveAs
			// 
			this.menuItemFileSaveAs.Index = 3;
			this.menuItemFileSaveAs.Text = "Save &As...";
			this.menuItemFileSaveAs.Click += new System.EventHandler( this.menuItemFileSaveAs_Click );
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 4;
			this.menuItem6.Text = "-";
			// 
			// menuItemFileExit
			// 
			this.menuItemFileExit.Index = 5;
			this.menuItemFileExit.Text = "E&xit";
			this.menuItemFileExit.Click += new System.EventHandler( this.menuItemFileExit_Click );
			// 
			// menuItem9
			// 
			this.menuItem9.Index = 1;
			this.menuItem9.MenuItems.AddRange( new System.Windows.Forms.MenuItem[] {
            this.menuItemEditUndo,
            this.menuItemEditRedo} );
			this.menuItem9.Text = "&Edit";
			this.menuItem9.Popup += new System.EventHandler( this.menuItem9_Popup );
			// 
			// menuItemEditUndo
			// 
			this.menuItemEditUndo.Index = 0;
			this.menuItemEditUndo.Shortcut = System.Windows.Forms.Shortcut.CtrlZ;
			this.menuItemEditUndo.Text = "&Undo";
			this.menuItemEditUndo.Click += new System.EventHandler( this.menuItemEditUndo_Click );
			// 
			// menuItemEditRedo
			// 
			this.menuItemEditRedo.Index = 1;
			this.menuItemEditRedo.Shortcut = System.Windows.Forms.Shortcut.CtrlY;
			this.menuItemEditRedo.Text = "&Redo";
			this.menuItemEditRedo.Click += new System.EventHandler( this.menuItemEditRedo_Click );
			// 
			// menuItem8
			// 
			this.menuItem8.Index = 2;
			this.menuItem8.MenuItems.AddRange( new System.Windows.Forms.MenuItem[] {
            this.menuItemFontImage,
            this.menuItem11,
            this.menuItem2,
            this.menuItem3,
            this.menuItem4} );
			this.menuItem8.Text = "F&ont";
			// 
			// menuItemFontImage
			// 
			this.menuItemFontImage.Index = 0;
			this.menuItemFontImage.Text = "&Load Image...";
			this.menuItemFontImage.Click += new System.EventHandler( this.menuItemFontImage_Click );
			// 
			// menuItem11
			// 
			this.menuItem11.Index = 1;
			this.menuItem11.Text = "&Use Creation Assistant...";
			this.menuItem11.Click += new System.EventHandler( this.menuItem11_Click );
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 2;
			this.menuItem2.Text = "-";
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 3;
			this.menuItem3.Text = "Copy Upper Case to Lower Case Coords";
			this.menuItem3.Click += new System.EventHandler( this.menuItemCopyUpperToLower_Click );
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 4;
			this.menuItem4.Text = "Copy Lower Case to Upper Case Coordinates";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point( 642, 8 );
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size( 96, 16 );
			this.label2.TabIndex = 6;
			this.label2.Text = "Character Height :";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// textBoxCharHeight
			// 
			this.textBoxCharHeight.Location = new System.Drawing.Point( 740, 8 );
			this.textBoxCharHeight.Name = "textBoxCharHeight";
			this.textBoxCharHeight.Size = new System.Drawing.Size( 32, 20 );
			this.textBoxCharHeight.TabIndex = 1;
			this.textBoxCharHeight.Text = "16";
			this.textBoxCharHeight.TextChanged += new System.EventHandler( this.textBoxCharHeight_TextChanged );
			// 
			// statusBarMain
			// 
			this.statusBarMain.Location = new System.Drawing.Point( 0, 547 );
			this.statusBarMain.Name = "statusBarMain";
			this.statusBarMain.Panels.AddRange( new System.Windows.Forms.StatusBarPanel[] {
            this.statusBarPanelInfo,
            this.statusBarPanelMousePos} );
			this.statusBarMain.ShowPanels = true;
			this.statusBarMain.Size = new System.Drawing.Size( 776, 22 );
			this.statusBarMain.TabIndex = 8;
			// 
			// statusBarPanelInfo
			// 
			this.statusBarPanelInfo.AutoSize = System.Windows.Forms.StatusBarPanelAutoSize.Spring;
			this.statusBarPanelInfo.Name = "statusBarPanelInfo";
			this.statusBarPanelInfo.Text = "Help text comes up here";
			this.statusBarPanelInfo.Width = 679;
			// 
			// statusBarPanelMousePos
			// 
			this.statusBarPanelMousePos.Alignment = System.Windows.Forms.HorizontalAlignment.Right;
			this.statusBarPanelMousePos.Name = "statusBarPanelMousePos";
			this.statusBarPanelMousePos.Text = "x: 0 y: 0";
			this.statusBarPanelMousePos.Width = 80;
			// 
			// listBoxCharacters
			// 
			this.listBoxCharacters.ColumnWidth = 14;
			this.listBoxCharacters.Font = new System.Drawing.Font( "Arial", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( (byte)( 0 ) ) );
			this.listBoxCharacters.HorizontalScrollbar = true;
			this.listBoxCharacters.ItemHeight = 22;
			this.listBoxCharacters.Location = new System.Drawing.Point( 80, 40 );
			this.listBoxCharacters.MultiColumn = true;
			this.listBoxCharacters.Name = "listBoxCharacters";
			this.listBoxCharacters.ScrollAlwaysVisible = true;
			this.listBoxCharacters.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.listBoxCharacters.Size = new System.Drawing.Size( 520, 48 );
			this.listBoxCharacters.TabIndex = 0;
			this.listBoxCharacters.SelectedIndexChanged += new System.EventHandler( this.listBoxCharacters_SelectedIndexChanged );
			this.listBoxCharacters.KeyPress += new System.Windows.Forms.KeyPressEventHandler( this.listBoxCharacters_KeyPress );
			this.listBoxCharacters.KeyDown += new System.Windows.Forms.KeyEventHandler( this.listBoxCharacters_KeyDown );
			// 
			// pictureBoxZoomFontImage
			// 
			this.pictureBoxZoomFontImage.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
						| System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.pictureBoxZoomFontImage.Image = null;
			this.pictureBoxZoomFontImage.Location = new System.Drawing.Point( 8, 16 );
			this.pictureBoxZoomFontImage.Name = "pictureBoxZoomFontImage";
			this.pictureBoxZoomFontImage.Size = new System.Drawing.Size( 744, 270 );
			this.pictureBoxZoomFontImage.TabIndex = 3;
			this.pictureBoxZoomFontImage.MouseLeave += new System.EventHandler( this.pictureBoxZoomFontImage_MouseLeave );
			this.pictureBoxZoomFontImage.MouseMove += new System.Windows.Forms.MouseEventHandler( this.pictureBoxZoomFontImage_MouseMove );
			this.pictureBoxZoomFontImage.picturePaint += new PictureBoxZoom.PictureBoxZoom.PicturePaintHandler( this.pictureBoxZoomFontImage_picturePaint );
			this.pictureBoxZoomFontImage.MouseDown += new System.Windows.Forms.MouseEventHandler( this.pictureBoxZoomFontImage_MouseDown );
			this.pictureBoxZoomFontImage.KeyPress += new System.Windows.Forms.KeyPressEventHandler( this.pictureBoxZoomFontImage_KeyPress );
			this.pictureBoxZoomFontImage.MouseUp += new System.Windows.Forms.MouseEventHandler( this.pictureBoxZoomFontImage_MouseUp );
			// 
			// groupBoxFontImage
			// 
			this.groupBoxFontImage.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
						| System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.groupBoxFontImage.Controls.Add( this.pictureBoxZoomFontImage );
			this.groupBoxFontImage.Location = new System.Drawing.Point( 8, 100 );
			this.groupBoxFontImage.Name = "groupBoxFontImage";
			this.groupBoxFontImage.Size = new System.Drawing.Size( 760, 292 );
			this.groupBoxFontImage.TabIndex = 3;
			this.groupBoxFontImage.TabStop = false;
			this.groupBoxFontImage.Text = "Font Image";
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point( 598, 34 );
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size( 140, 16 );
			this.label3.TabIndex = 9;
			this.label3.Text = "Inter-Character Spacing :";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// textBoxLetterSpace
			// 
			this.textBoxLetterSpace.Location = new System.Drawing.Point( 740, 34 );
			this.textBoxLetterSpace.Name = "textBoxLetterSpace";
			this.textBoxLetterSpace.Size = new System.Drawing.Size( 32, 20 );
			this.textBoxLetterSpace.TabIndex = 10;
			this.textBoxLetterSpace.Text = "2";
			this.textBoxLetterSpace.TextChanged += new System.EventHandler( this.textBoxLetterSpace_TextChanged );
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point( 10, 10 );
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size( 70, 20 );
			this.label5.TabIndex = 11;
			this.label5.Text = "Name :";
			this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// textBoxFontName
			// 
			this.textBoxFontName.Location = new System.Drawing.Point( 80, 10 );
			this.textBoxFontName.MaxLength = 23;
			this.textBoxFontName.Name = "textBoxFontName";
			this.textBoxFontName.Size = new System.Drawing.Size( 230, 20 );
			this.textBoxFontName.TabIndex = 12;
			this.textBoxFontName.TextChanged += new System.EventHandler( this.textBoxFontName_TextChanged );
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.groupBox1.Controls.Add( this.buttonBrowseIndexPath );
			this.groupBox1.Controls.Add( this.textBoxResourceIndexPath );
			this.groupBox1.Location = new System.Drawing.Point( 8, 398 );
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size( 760, 44 );
			this.groupBox1.TabIndex = 13;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Tools Index Path";
			// 
			// textBoxResourceIndexPath
			// 
			this.textBoxResourceIndexPath.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.textBoxResourceIndexPath.Location = new System.Drawing.Point( 8, 19 );
			this.textBoxResourceIndexPath.Name = "textBoxResourceIndexPath";
			this.textBoxResourceIndexPath.Size = new System.Drawing.Size( 705, 20 );
			this.textBoxResourceIndexPath.TabIndex = 0;
			this.textBoxResourceIndexPath.TextChanged += new System.EventHandler( this.textBoxResourceIndexPath_TextChanged );
			// 
			// buttonBrowseIndexPath
			// 
			this.buttonBrowseIndexPath.Anchor = ( (System.Windows.Forms.AnchorStyles)( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.buttonBrowseIndexPath.Location = new System.Drawing.Point( 719, 17 );
			this.buttonBrowseIndexPath.Name = "buttonBrowseIndexPath";
			this.buttonBrowseIndexPath.Size = new System.Drawing.Size( 35, 23 );
			this.buttonBrowseIndexPath.TabIndex = 1;
			this.buttonBrowseIndexPath.Text = ". . .";
			this.buttonBrowseIndexPath.UseVisualStyleBackColor = true;
			this.buttonBrowseIndexPath.Click += new System.EventHandler( this.buttonBrowseIndexPath_Click );
			// 
			// openToolsResourceFileDialog
			// 
			this.openToolsResourceFileDialog.DefaultExt = "dat";
			this.openToolsResourceFileDialog.Filter = "Resource Index Files (*.dat)|*.dat|All Files|*.*";
			// 
			// FormMain
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size( 5, 13 );
			this.ClientSize = new System.Drawing.Size( 776, 569 );
			this.Controls.Add( this.groupBox1 );
			this.Controls.Add( this.textBoxFontName );
			this.Controls.Add( this.label5 );
			this.Controls.Add( this.textBoxLetterSpace );
			this.Controls.Add( this.listBoxCharacters );
			this.Controls.Add( this.statusBarMain );
			this.Controls.Add( this.textBoxCharHeight );
			this.Controls.Add( this.label2 );
			this.Controls.Add( this.groupBoxFontImage );
			this.Controls.Add( this.groupBoxExampleText );
			this.Controls.Add( this.label1 );
			this.Controls.Add( this.label3 );
			this.Icon = ( (System.Drawing.Icon)( resources.GetObject( "$this.Icon" ) ) );
			this.Menu = this.mainMenu1;
			this.Name = "FormMain";
			this.Text = "Prime Time Font Editor";
			this.groupBoxExampleText.ResumeLayout( false );
			this.groupBoxExampleText.PerformLayout();
			( (System.ComponentModel.ISupportInitialize)( this.pictureBoxExampleTextBG ) ).EndInit();
			( (System.ComponentModel.ISupportInitialize)( this.pictureBoxExampleText ) ).EndInit();
			( (System.ComponentModel.ISupportInitialize)( this.statusBarPanelInfo ) ).EndInit();
			( (System.ComponentModel.ISupportInitialize)( this.statusBarPanelMousePos ) ).EndInit();
			this.groupBoxFontImage.ResumeLayout( false );
			this.groupBox1.ResumeLayout( false );
			this.groupBox1.PerformLayout();
			this.ResumeLayout( false );
			this.PerformLayout();

		}
		#endregion

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		[STAThread]
		static void Main() 
		{
			Application.Run(new FormMain());
		}


//		private void textBoxCharacters_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
//		{
//			// If the user pressed backspace or delete and there is a selection
//			if( (e.KeyCode == Keys.Back || e.KeyCode == Keys.Delete) && listBoxCharacters.SelectedItems.Count > 0 )
//			{
//				string charsToRemove;
//				foreach( object selItem in listBoxCharacters.SelectedItems )
//				{
//					// Get the current selection
//					string objStr = selItem.ToString();
//					if( objStr.Length > 0 )
//						charsToRemove += objStr[0];
//				}
//
//				// Remove the characters
//				EditCmdModFontChars editCmd = new EditCmdModFontChars( charToRemove, false );
//				m_EditCmdMgr.ExecuteEditCommand( editCmd );
//			}
//		}
//
//		private void textBoxCharacters_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
//		{
//			// If the font already has this character then bail
//			if( sm_Font.ContainsChar( e.KeyChar ) )
//				return;
//
//			// Otherwise add the character
//			EditCmdModFontChars editCmd = new EditCmdModFontChars( e.KeyChar );
//			m_EditCmdMgr.ExecuteEditCommand( editCmd );
//			// If the text does not have this character already
//			int charIndex = textBoxCharacters.Text.IndexOf( e.KeyChar );
//			if( charIndex == -1 )
//			{
//				// Go through the letters and see if we can insert the character
//				for( int curCharIndex = 0; curCharIndex < textBoxCharacters.Text.Length; ++curCharIndex )
//				{
//					// If this character's value is less than the curent character then instert it
//					if( (int)e.KeyChar < textBoxCharacters.Text[ curCharIndex ] )
//					{
//						string charStr = "";
//						charStr += e.KeyChar;
//						textBoxCharacters.Text = textBoxCharacters.Text.Insert( curCharIndex, charStr );
//
//						// Store the index of the inserted character
//						charIndex = curCharIndex;
//						break;
//					}
//				}
//			
//				// If we need to append the character
//				if( charIndex == -1 )
//				{
//					textBoxCharacters.Text += e.KeyChar;
//					charIndex = textBoxCharacters.Text.Length - 1;
//				}
//			}			
//
//			// Select the entered character
//			textBoxCharacters.SelectionStart = charIndex;
//			textBoxCharacters.SelectionLength = 1;
//		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set the image that is used by the font
		/// </summary>
		/// <param name="resID"> The resource ID of the image to use </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void SetFontImage(uint resID)
		{
			// Load the resource database
			ResourceCS.ResourceToolsDB resDB = new ResourceCS.ResourceToolsDB();
			if( !resDB.LoadToolsIndex( textBoxResourceIndexPath.Text ) )
			{
				string sMsg = "Failed to load tools resource index file (";
				sMsg += textBoxResourceIndexPath.Text;
				sMsg += ")";
				MessageBox.Show(sMsg, "Resource Chooser");
				return;
			}

			// Get the resource info
			ResourceCS.ResourceToolsDB.ResourceIndexItem resInfo = resDB.GetResourceInfo(resID);

			// If the resource is not an image then bail
			if (resInfo.type != ResourceCS.ResourceType.Image)
			{
				MessageBox.Show("The selected resource is not an image, please re-choose.", Text, MessageBoxButtons.OK, MessageBoxIcon.Hand);
				return;
			}

			// Get the file name
			string sResImgFile = resDB.RootResourcePath + resInfo.sFileName;

			// Store the resource ID
			sm_Font.m_ImageResID = resID;

			// Unload the current image
			if (pictureBoxZoomFontImage.Image != null)
			{
				pictureBoxZoomFontImage.Image.Dispose();
				pictureBoxZoomFontImage.Image = null;
			}

			try
			{
				// Load the image with transparency
				Bitmap loadedBitmap = (Bitmap)Image.FromFile(sResImgFile);
				loadedBitmap.MakeTransparent(TRANS_COLOR);
				pictureBoxZoomFontImage.Image = loadedBitmap;
			}
			catch (Exception exc)
			{
				MessageBox.Show("Failed to load image file. (" + exc.Message + ")", Text);
			}
		}


		//[System.Runtime.InteropServices.DllImport("testmfc.dll")]
		[System.Runtime.InteropServices.DllImport( "ResPathDlg_dbg.dll" )]
		public static extern bool RunResChooserDlg_Filter( IntPtr hWnd, ref uint resID, int typeFilter );


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks on the menu item to load a font image
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItemFontImage_Click(object sender, System.EventArgs e)
		{
			// Load the tools resource file
			uint selResID = 0;
			if (!RunResChooserDlg_Filter( Handle, ref selResID, (int)ResourceCS.ResourceType.Image))
				return;

			try
			{
				// Set the font image
				SetFontImage(selResID);

				// Redraw
				pictureBoxZoomFontImage.Refresh();
			}
			catch( Exception exc )
			{
				MessageBox.Show( exc.Message, "Failed to Access Resource" );
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user chooses undo from the edit menu
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItemEditUndo_Click(object sender, System.EventArgs e)
		{
			m_EditCmdMgr.UndoEditCommand();
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user chooses undo from the edit menu
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItemEditRedo_Click(object sender, System.EventArgs e)
		{
			m_EditCmdMgr.RedoEditCommand();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user presses a character key on the character list box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void listBoxCharacters_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			// Ensure a valid readable character
			int unicodeCharCode = (int)e.KeyChar;
			if( unicodeCharCode < PTFont.MIN_UNICODE_READABLE || unicodeCharCode > PTFont.MAX_UNICODE_READABLE )
				return;

			// If the font already has this character then bail
			if( sm_Font.ContainsChar( e.KeyChar ) )
				return;

			// Otherwise add the character
			EditCmdModFontChars editCmd = new EditCmdModFontChars( e.KeyChar );
			m_EditCmdMgr.ExecuteEditCommand( editCmd );
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user presses a keyboard key on the character list box
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void listBoxCharacters_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			// If the user pressed backspace or delete and there is a selection
			if( (e.KeyCode == Keys.Back || e.KeyCode == Keys.Delete) && listBoxCharacters.SelectedItems.Count > 0 )
			{
				// Get the characters to remove
				string charsToRemove = "";
				foreach( object selItem in listBoxCharacters.SelectedItems )
				{
					// Get the current selection
					string objStr = selItem.ToString();
					if( objStr.Length > 0 )
						charsToRemove += objStr[0];
				}

				// Remove the characters
				EditCmdModFontChars editCmd = new EditCmdModFontChars( charsToRemove );
				m_EditCmdMgr.ExecuteEditCommand( editCmd );
			}
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the selected index of the character list box changes
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void listBoxCharacters_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			UpdateGlyphsFromSel();
			pictureBoxZoomFontImage.Refresh();
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string of the characters selected in the list box
		/// </summary>
		/// <returns> A string containing the characters currently selected </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private string GetSelChars()
		{
			string sRetStr = "";

			// Go through each selected list box item and add it to the return string
			foreach( object listItem in listBoxCharacters.SelectedItems )
				sRetStr += listItem.ToString()[0];

			return sRetStr;
		}

		private void pictureBoxExampleTextBG_Click(object sender, System.EventArgs e)
		{
			// Create a color picker
			ColorDialog colorDlg = new ColorDialog();
			colorDlg.Color = pictureBoxExampleTextBG.BackColor;
			colorDlg.AllowFullOpen = true;

			// Allow the user to pick a color
			if( colorDlg.ShowDialog() == DialogResult.OK )
			{
				// Set the color
				pictureBoxExampleTextBG.BackColor = colorDlg.Color;
				pictureBoxExampleText.BackColor = colorDlg.Color;
			}		
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the example text box needs to be redrawn
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxExampleText_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			const int LEFT_MARGIN = 4;

			// If there is no font image then do nothing
			if( pictureBoxZoomFontImage.Image == null )
				return;

			// Get the starting destination rectangle
			Rectangle destRect = new Rectangle();
			destRect.X = LEFT_MARGIN;
			destRect.Y = (pictureBoxExampleText.Height - sm_Font.CharHeight) / 2;
			destRect.Height = sm_Font.CharHeight;

			// Go through the letters of the example text
			foreach( char curChar in textBoxExampleText.Text )
			{
				// If this is a space, move over
				if( curChar == ' ' )
				{
					destRect.X += sm_Font.SpaceWidth;
					continue;
				}
				
				// Does the font supply a glyph for this character
				if( !sm_Font.ContainsChar( curChar ) )
					continue;

				// Get the source rectangle
				Rectangle srcRect = sm_Font[ curChar ];
				destRect.Width = srcRect.Width;

				// Draw the character
				e.Graphics.DrawImage( pictureBoxZoomFontImage.Image, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, GraphicsUnit.Pixel ); 

				// Move on
				destRect.X += srcRect.Width + sm_Font.m_LetterSpacing;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the edit menu is displayed
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItem9_Popup(object sender, System.EventArgs e)
		{
			// Update the edit menu item text
			UpdateEditCmdMenuItems();
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user chooses to use the font create assitant
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItem11_Click(object sender, System.EventArgs e)
		{
			// If no image is loaded
			if( pictureBoxZoomFontImage.Image == null )
			{
				MessageBox.Show( "An image must be loaded before using the create assistant.", MESSAGE_BOX_TITLE );
				return;
			}
			
			// Create the assistant window
			FormCreateAssist formAssitant = new FormCreateAssist( pictureBoxZoomFontImage.Image );
			if( formAssitant.ShowDialog() == DialogResult.OK )
			{
				// Add the characters we created
				string addChars = "";
				Rectangle[] charRects = new Rectangle[ formAssitant.m_AddedChars.Count ];
				for( int addedCharIndex = 0; addedCharIndex < formAssitant.m_AddedChars.Count; ++addedCharIndex )
				{
					addChars += (char)formAssitant.m_AddedChars.GetKey(addedCharIndex);
					charRects[addedCharIndex] = (Rectangle)formAssitant.m_AddedChars.GetByIndex(addedCharIndex);
				}

				// Otherwise add the character
				EditCmdModFontChars editCmd = EditCmdModFontChars.AddCharacters( addChars, charRects );
				m_EditCmdMgr.ExecuteEditCommand( editCmd );
			}
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user changes the character height text
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxCharHeight_TextChanged(object sender, System.EventArgs e)
		{
			// Store the height
			try
			{
				sm_Font.CharHeight = int.Parse(textBoxCharHeight.Text);
			}
			catch (Exception) { }

			// Update the glyph heights
			foreach (GlyphRect glyphRect in m_GlyphRects)
				glyphRect.rect.Height = sm_Font.CharHeight;

			// Redraw everything
			Invalidate( true );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user changes the letter spacing text
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxLetterSpace_TextChanged(object sender, EventArgs e)
		{
			// Store the letter spacing
			try
			{
				sm_Font.m_LetterSpacing = int.Parse(textBoxLetterSpace.Text);
			}
			catch (Exception) { }

			// Update the preview text
			pictureBoxExampleText.Invalidate();
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user changes the example text
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void textBoxExampleText_TextChanged(object sender, System.EventArgs e)
		{
			// Redraw the text
			pictureBoxExampleText.Refresh();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks on the menu item to create a new font
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItemFileNew_Click(object sender, System.EventArgs e)
		{
			// Close the font or bail if we don't want to close the file
			if( !CloseFont() )
				return;

			// Clear the data
			m_FontHasChanges = false;
			listBoxCharacters.Items.Clear();
			pictureBoxZoomFontImage.Image = null;
			m_sFontFilePath = "";
			sm_Font.Reset();
			textBoxFontName.Text = "";

			// Update the interface
			textBoxCharHeight.Text = sm_Font.CharHeight.ToString();
			textBoxLetterSpace.Text = sm_Font.m_LetterSpacing.ToString();
			UpdateWindowText();
			Invalidate( true );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks on the menu item to open a new font
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItemFileOpen_Click(object sender, System.EventArgs e)
		{
			// Close the font or bail if we don't want to close the file
			if( !CloseFont() )
				return;

			// Pop up the file dialog
			OpenFileDialog fileDlg = new OpenFileDialog();
			fileDlg.Filter = "Font Files (*.fnt)|*.fnt|All Files (*.*)|*.*";
			
			// If the user did not pick a file
			if( fileDlg.ShowDialog() != DialogResult.OK )
				return;
			
			// Open the font
			m_sFontFilePath = fileDlg.FileName;
			try
			{
				sm_Font.LoadFont( m_sFontFilePath );

				// Load the image
				SetFontImage(sm_Font.m_ImageResID);
			}
			catch( Exception exc )
			{
				MessageBox.Show( "Failed to load the font file :\n" + exc.Message, MESSAGE_BOX_TITLE );
			}

			// Update the character list box
			listBoxCharacters.BeginUpdate();
			listBoxCharacters.Items.Clear();
			foreach( char strChar in sm_Font.CharString )
				listBoxCharacters.Items.Add( strChar );
			listBoxCharacters.EndUpdate();

			// Update the character height and name
			textBoxCharHeight.Text = sm_Font.CharHeight.ToString();
			textBoxFontName.Text = sm_Font.m_Name;

			// Update the window text
			UpdateWindowText();
			
			// Redraw
			Invalidate( true );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks on the menu item to save a new font
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItemFileSave_Click(object sender, System.EventArgs e)
		{
			// Ensure a name is set
			if (textBoxFontName.Text.Length == 0)
			{
				MessageBox.Show("Please name the font.", Text, MessageBoxButtons.OK, MessageBoxIcon.Hand);
				return;
			}

			// If there is no file name then save as
			if( m_sFontFilePath == "" )
				menuItemFileSaveAs_Click( sender, e );
			// Else just save the font
			else
			{
				// Save the font
				try
				{
					sm_Font.SaveFont( m_sFontFilePath );
				}
				catch( Exception exc )
				{
					MessageBox.Show( "Failed to save the font file :" + exc.Message, MESSAGE_BOX_TITLE );
				}
			}

			// Update the window text
			UpdateWindowText();
		}


		// Used to convert the sprites in PC byte-order to network byte-order
		//private void SaveLoadAllFont()
		//{
		//    const string RootPath = @"C:\Taylor\Programming Projects\PrimeTimeCode\Final Resource";
		//    DirectoryInfo rootDir = new DirectoryInfo( RootPath );

		//    FileInfo[] fontFiles = rootDir.GetFiles( "*.fnt", SearchOption.AllDirectories );

		//    foreach( FileInfo curFontFile in fontFiles )
		//    {
		//        PTFont font = new PTFont();
		//        font.LoadFont( curFontFile.FullName );

		//        string subDir = curFontFile.DirectoryName.Substring( RootPath.Length );
		//        string fullDir = "C:\\NewFonts\\" + subDir + "\\";
		//        if( !Directory.Exists( fullDir ) )
		//            Directory.CreateDirectory( fullDir );

		//        font.SaveFont( fullDir + curFontFile.Name );
		//    }
		//}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks on the menu item to save a new font as a
		/// new name
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItemFileSaveAs_Click(object sender, System.EventArgs e)
		{
			// Ensure a name is set
			if (textBoxFontName.Text.Length == 0)
			{
				MessageBox.Show("Please name the font.", Text, MessageBoxButtons.OK, MessageBoxIcon.Hand);
				return;
			}

			// Pop up the file dialog
			SaveFileDialog fileDlg = new SaveFileDialog();
			fileDlg.Filter = "Font Files (*.fnt)|*.fnt|All Files (*.*)|*.*";
			
			// If the user picks a file
			if( fileDlg.ShowDialog() == DialogResult.OK )
			{
				// Save the font
				m_sFontFilePath = fileDlg.FileName;
				menuItemFileSave_Click( null, null );
			}

			// Update the window text
			UpdateWindowText();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user clicks on the menu item to create a new font
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void menuItemFileExit_Click(object sender, System.EventArgs e)
		{
			// Close the font or bail if we don't want to close the file
			if( !CloseFont() )
				return;

			// Close the window
			Close();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Close the font, but ask for confirmation if it has changes
		/// </summary>
		/// <returns> True if the font was closed, font otherwise </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		private bool CloseFont()
		{
			// If we don't have changes to save
			if( !m_FontHasChanges )
				return true;

			// Ask the user if he/she would like to save
			DialogResult msgBoxRes = MessageBox.Show( "Would you like to save your changes to this file?", "Prime Time Font Editor", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question );

			// If the user cancelled then don't continue
			if( msgBoxRes == DialogResult.Cancel )
				return false;

			// If the user wants to save
			if( msgBoxRes == DialogResult.Yes )
				menuItemFileSave_Click( null, null );

			// Return that we can continue
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the font image picture box is to be redrawn
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_picturePaint(object sender, System.Drawing.Graphics g)
		{
			try
			{
				// Go through the glyph rectangles and draw the boxes
				foreach( GlyphRect glyphRect in m_GlyphRects )
				{
					Rectangle fillRect = pictureBoxZoomFontImage.ImageToPictureBoxClient( glyphRect.rect );
					g.FillRectangle( new SolidBrush( Color.FromArgb(120,50,255,50) ), fillRect );
				}
			}
			catch( Exception exc )
			{
				MessageBox.Show( exc.Message, MESSAGE_BOX_TITLE );
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user presses the mouse button down over the font
		/// image
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// We only handle the left mouse button
			if( e.Button != MouseButtons.Left )
				return;

			// Get the mouse position in image coordinates
			Point mouseImgPos = pictureBoxZoomFontImage.ClientToImage( new Point( e.X, e.Y ) );

			// Get if the mouse is over a glyph
			bool isMouseOverGlyph = false;
			foreach( GlyphRect glyphRect in m_GlyphRects )
			{
				// If this glyph contains the mouse point
				if( glyphRect.rect.Contains( mouseImgPos ) )
				{
					isMouseOverGlyph = true;
					break;
				}
			}

			// If we are not over a glyph then do nothing
			if( !isMouseOverGlyph )
				return;

			// We are starting to drag a glyph
			Cursor.Current = Cursors.SizeAll;
			m_IsDraggingGlyph = true;
			m_GlyphDragLastPos = mouseImgPos;
		}

		
		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the mouse leaves the picture box zoom control
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_MouseLeave(object sender, System.EventArgs e)
		{
			// Clear the mouse position text and dragging
			statusBarMain.Text = "";
			m_IsDraggingGlyph = false;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user moves the mouse over the font image
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			Point mouseImgPos = pictureBoxZoomFontImage.ClientToImage( new Point( e.X, e.Y ) );

			// If we are not at a valid image position then bail
			if( mouseImgPos.X == -1 )
				statusBarPanelMousePos.Text = "";

			// Display the coordinates
			statusBarPanelMousePos.Text = string.Format( "x:{0} y:{1}", mouseImgPos.X, mouseImgPos.Y );

			// If we are moving glyphs around
			if( m_IsDraggingGlyph )
			{
				// Get the amount moved
				Point amtMoved = new Point();
				amtMoved.X = mouseImgPos.X - m_GlyphDragLastPos.X;
				amtMoved.Y = mouseImgPos.Y - m_GlyphDragLastPos.Y;

				// If we have moved
				if( amtMoved.X != 0 || amtMoved.Y != 0 )
				{
					// Move each selected glyph
					foreach( GlyphRect glyphRect in m_GlyphRects )
					{
						glyphRect.rect.X += amtMoved.X;
						glyphRect.rect.Y += amtMoved.Y;

						// Update the actual font data
						sm_Font[glyphRect.fontChar] = glyphRect.rect;
					}

					// Store the new position
					m_GlyphDragLastPos = mouseImgPos;

					// Redraw the font image and example text
					pictureBoxZoomFontImage.Refresh();
					pictureBoxExampleText.Refresh();
				}
			}
				// Else see if we should update the mouse cursor
			else
			{
				// If the mouse is over a glyph
				foreach( GlyphRect glyphRect in m_GlyphRects )
				{
					if( glyphRect.rect.Contains( mouseImgPos ) )
					{
						Cursor.Current = Cursors.SizeAll;
						break;
					}
				}
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user releases the mouse button over the font image
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// We can't be dragging anymore
			m_IsDraggingGlyph = false;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler for when the user presses a character key with glyphs selected
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void pictureBoxZoomFontImage_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			int xMoveAmt = 0;
			int yMoveAmt = 0;
			int xSizeAmt = 0;

			// Handle movement with wasd and sizing if shift is down
			if( e.KeyChar == 'w' )
				yMoveAmt = -1;
			else if( e.KeyChar == 'a' )
				xMoveAmt = -1;
			else if( e.KeyChar == 's' )
				yMoveAmt = 1;
			else if( e.KeyChar == 'd' )
				xMoveAmt = 1;
			else if( e.KeyChar == 'A' )
				xSizeAmt = -1;
			else if( e.KeyChar == 'D' )
				xSizeAmt = 1;

			// If we aren't changing anything then bail
			if( xMoveAmt == 0 && yMoveAmt == 0 && xSizeAmt == 0 )
				return;

			// Move each selected glyph
			foreach( GlyphRect glyphRect in m_GlyphRects )
			{
				glyphRect.rect.X += xMoveAmt;
				glyphRect.rect.Y += yMoveAmt;
				glyphRect.rect.Width += xSizeAmt;
				
				// Update the actual font data
				sm_Font[glyphRect.fontChar] = glyphRect.rect;
			}

			// Redraw
			pictureBoxZoomFontImage.Refresh();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Update the window text based on the loaded file
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void UpdateWindowText()
		{
			// Build the window title
			string sWinTitle = m_sOrigTitle;

			// Add in the file name
			if (m_sFontFilePath.Length > 0)
			{
				sWinTitle += " - ";
				sWinTitle += System.IO.Path.GetFileName(m_sFontFilePath);

				// Mark if there are changes
				if (this.m_FontHasChanges)
					sWinTitle += "*";
			}

			// Set the window text
			Text = sWinTitle;
		}


		//---------------------------------------------------------------------------------------------
		/// <summary>
		/// An edit command for adding or removing characters from the font's character list
		/// </summary>
		//---------------------------------------------------------------------------------------------
		class EditCmdModFontChars : CSBase.EditCmdMgr.IEditCmdBase
		{
			/// <summary> If this command is adding a character </summary>
			private bool m_AddChar = true;

			/// <summary> The character to add or remove </summary>
			private string m_Chars = "";

			/// <summary> The character's glyph on the font image </summary>
			private Rectangle[] m_CharImgRects = null;


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// The constructor to add a character to the font
			/// </summary>
			/// <param name="modChar"> The character to add </param>
			/// <param name="isAdd"> True if we are adding the character, false otherwise </param>
			///////////////////////////////////////////////////////////////////////////////////////
			public EditCmdModFontChars( char modChar )
			{
				// We are adding a character to the font
				m_AddChar = true;
				
				// Store the character
				m_Chars = "";
				m_Chars += modChar;

				// Create the glyph rectangle
				m_CharImgRects = new Rectangle[1];
				m_CharImgRects[0] = new Rectangle( 0, 0, PTFont.MIN_CHAR_WIDTH, sm_Font.CharHeight );
			}

			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// The constructor to remove a character to the font
			/// </summary>
			/// <param name="modChar"> The character to remove </param>
			/// <param name="isAdd"> True if we are adding the character, false otherwise </param>
			///////////////////////////////////////////////////////////////////////////////////////
			public EditCmdModFontChars( char modChar, Rectangle curRect )
			{
				// We are removing a character from the font
				m_AddChar = false;
				
				// Store the character
				m_Chars = "";
				m_Chars += modChar;

				// Create the glyph rectangle
				m_CharImgRects = new Rectangle[1];
				m_CharImgRects[0] = curRect;
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// The constructor to remove a group of characters from the font
			/// </summary>
			/// <param name="modChars"> The string of characters to add </param>
			/// <param name="glyphRects"> The list of rectangles representing the glyphs for each
			/// character in the string.  The order of the glyph rectangles must match the order
			/// of characters. </param>
			/// <param name="isAdd"> True if we are adding the character, false otherwise </param>
			///////////////////////////////////////////////////////////////////////////////////////
			public EditCmdModFontChars( string modChars )
			{
				// Ensure a valid string
				if( modChars.Length == 0 )
					throw( new Exception("Can not create edit command with no characters.") );

				// We are removing characters
				m_AddChar = false;

				// Store the data
				m_Chars = modChars;
				
				// Store the glyph rectangles
				m_CharImgRects = new Rectangle[ m_Chars.Length ];
				for( int rectIndex = 0; rectIndex < m_Chars.Length; ++rectIndex )
					m_CharImgRects[ rectIndex ] = sm_Font[ m_Chars[rectIndex] ];
			}

			
			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// The default constructor
			/// </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			private EditCmdModFontChars()
			{
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// The constructor to remove a group of characters from the font
			/// </summary>
			/// <param name="modChars"> The string of characters to add </param>
			/// <param name="glyphRects"> The list of rectangles representing the glyphs for each
			/// character in the string.  The order of the glyph rectangles must match the order
			/// of characters. </param>
			/// <param name="isAdd"> True if we are adding the character, false otherwise </param>
			///////////////////////////////////////////////////////////////////////////////////////
			public static EditCmdModFontChars AddCharacters( string modChars, Rectangle[] charRects )
			{
				// Ensure a valid string
				if( modChars.Length != charRects.Length )
					throw( new Exception("Can not create edit command with different number of characters and rectangles.") );

				EditCmdModFontChars retVal = new EditCmdModFontChars();

				// We are adding characters
				retVal.m_AddChar = true;

				// Store the data
				retVal.m_Chars = modChars;
				
				// Store the glyph rectangles
				retVal.m_CharImgRects = new Rectangle[ retVal.m_Chars.Length ];
				for( int rectIndex = 0; rectIndex < retVal.m_Chars.Length; ++rectIndex )
					retVal.m_CharImgRects[ rectIndex ] = charRects[ rectIndex ];

				return retVal;
			}

			
			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Modify the font's character list </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			public void Execute()
			{
				InternelExec( false );
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Undo the modification of the font's character list </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			public void Undo()
			{
				InternelExec( true );
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// Actual execute the modification of the character list
			/// </summary>
			/// <param name="isUndo"> True if we are undoing this command, false to execute </param>
			///////////////////////////////////////////////////////////////////////////////////////
			private void InternelExec( bool isUndo )
			{
				bool shouldAddChar = m_AddChar ^ isUndo;

				// If we are adding characters
				if( shouldAddChar )
				{
					for( int charIndex = 0; charIndex < m_Chars.Length; ++charIndex )
						sm_Font.AddChar( m_Chars[charIndex], m_CharImgRects[charIndex] );
				}
				// Else we are removing characters
				else
				{
					for( int charIndex = 0; charIndex < m_Chars.Length; ++charIndex )
					{
						// Store the image rectangle
						m_CharImgRects[charIndex] = sm_Font[ m_Chars[charIndex] ];

						// Remove the character
						sm_Font.RemoveChar( m_Chars[charIndex] );
					}
				}
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Get a descriptive name for this edit command </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			public string Name
			{
				get
				{
					// Return a name based on what the command is doing
					if( m_AddChar && m_Chars.Length == 1 )
						return "Add Character";
					else if( m_AddChar )
						return "Add Characters";
					else if( !m_AddChar && m_Chars.Length == 1 )
						return "Remove Character";
					return "Remove Characters";
				}
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// Get the characters to add/remove
			/// </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			public string ModChars
			{
				get{ return m_Chars; }
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// Get if we are adding the character, false if we are removing
			/// </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			public bool IsAddCmd
			{
				get{ return m_AddChar; }
			}
		}


		//---------------------------------------------------------------------------------------------
		/// <summary>
		/// An edit command for modifying characters rectangles.
		/// </summary>
		//---------------------------------------------------------------------------------------------
		class EditCmdModFontCharRects : CSBase.EditCmdMgr.IEditCmdBase
		{
			/// <summary>
			/// A structure containing the data needed for modifying a character's glyph rectangle
			/// </summary>
			private struct ModCharRect
			{
				public char modChar;
				public Rectangle oldRect;
				public Rectangle newRect;
				public bool isAddChar;
			}
			
			/// <summary> The list of modified characters </summary>
			private System.Collections.Generic.List<ModCharRect> m_ModCharList = new System.Collections.Generic.List<ModCharRect>();


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// Add a character to modify.
			/// </summary>
			/// <param name="modChar"> The character to modify </param>
			/// <param name="currentRect"> The current glyph rectangle for the character </param>
			/// <param name="newRect"> The new glyph rectangle for the character </param>
			///////////////////////////////////////////////////////////////////////////////////////
			public void AddModChar(char modChar, Rectangle currentRect, Rectangle newRect)
			{
				// Create the list item
				ModCharRect newCharMod;
				newCharMod.modChar = modChar;
				newCharMod.oldRect = currentRect;
				newCharMod.newRect = newRect;
				if (sm_Font.ContainsChar(newCharMod.modChar))
					newCharMod.isAddChar = false;
				else
					newCharMod.isAddChar = true;

				// Add the item
				m_ModCharList.Add(newCharMod);
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Modify the font's character list </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			public void Execute()
			{
				// Go through entry in the character modification list
				foreach( ModCharRect curRect in m_ModCharList )
				{
					// See if we are updating or adding
					if (curRect.isAddChar)
						sm_Font.AddChar(curRect.modChar, curRect.newRect);
					else
						sm_Font[curRect.modChar] = curRect.newRect;
				}
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Undo the modification of the font's character list </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			public void Undo()
			{
				// Go through entry in the character modification list
				foreach (ModCharRect curRect in m_ModCharList)
				{
					// See if we are updating or adding
					if (curRect.isAddChar)
						sm_Font.RemoveChar(curRect.modChar);
					else
						sm_Font[curRect.modChar] = curRect.oldRect;
				}
			}


			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Get a descriptive name for this edit command </summary>
			///////////////////////////////////////////////////////////////////////////////////////
			public string Name
			{
				get
				{
					return "Copy Character Rectangles";
				}
			}
		}

		
		//---------------------------------------------------------------------------------------------
		/// <summary>
		/// A class for storing a character's glyph rectangle on the paint box
		/// </summary>
		//---------------------------------------------------------------------------------------------
		private class GlyphRect
		{
			///////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// The basic constructor
			/// </summary>
			/// <param name="c"> The character this glyph represents </param>
			/// <param name="r"> The rectangle for the character </param>
			///////////////////////////////////////////////////////////////////////////////////////
			public GlyphRect( char c, Rectangle r )
			{
				fontChar = c;
				rect = r;
			}

			public char fontChar;

			public Rectangle rect;
		}

		private void textBoxFontName_TextChanged(object sender, EventArgs e)
		{
			// Store the name
			sm_Font.m_Name = textBoxFontName.Text;
		}

		private void menuItemCopyUpperToLower_Click(object sender, EventArgs e)
		{
			// Get the offset between upper and lower case
			const int offset = (int)('a' - 'A');

			// Go through each character we have
			EditCmdModFontCharRects newCmd = new EditCmdModFontCharRects();
			for (char srcChar = 'A'; srcChar <= 'Z'; ++srcChar)
			{
				// If there is no source character in the font
				if (!sm_Font.ContainsChar(srcChar))
					continue;

				// Get the dest character
				char destChar = (char)(srcChar + offset);

				// Get the source rectangle
				Rectangle srcRect = sm_Font[srcChar];

				// Set the destination rectangle
				if (sm_Font.ContainsChar(destChar))
					newCmd.AddModChar(destChar, sm_Font[destChar], srcRect);
				else
					newCmd.AddModChar(destChar, new Rectangle(), srcRect);	
			}

			// Execute the modify command
			m_EditCmdMgr.ExecuteEditCommand(newCmd);
		}

		private void buttonBrowseIndexPath_Click(object sender, EventArgs e)
		{
			if( openToolsResourceFileDialog.ShowDialog() == DialogResult.OK )
				textBoxResourceIndexPath.Text = openToolsResourceFileDialog.FileName;
		}

		private void textBoxResourceIndexPath_TextChanged(object sender, EventArgs e)
		{
			Settings.Default.ToolsIndexPath = textBoxResourceIndexPath.Text;
			Settings.Default.Save();
		}
	}
}
