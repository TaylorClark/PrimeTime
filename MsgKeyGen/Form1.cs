using System;
using System.Globalization;
using System.Windows.Forms;
using TCUtil;
using PTWebApp;
using System.Drawing;

namespace MsgKeyGen
{
    public partial class Form1 : Form
    {
        private TCUtil.RegKey m_RegKey = new RegKey();

        PTCustomizeUtil m_PTUtil = new PTCustomizeUtil();
        
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Random rand = new Random();
            // Get the text
            string sEntered = textBoxMsg.Text;

            short[] outData = new short[PTCustomizeUtil.MSG_TOTAL_LEN];
            int dataOffset = 0;

            // Get the length of the entered message
            int msgLen = (int) sEntered.Length;
            if( msgLen < 1 )
                return;
            if (msgLen > PTCustomizeUtil.MSG_NUM_MSG_LEN)
            {
                MessageBox.Show("The message must be less than 64 characters, the entered text is " +msgLen +" characters." );
                return;
            }

            // Generate the string
            string sOut = "const wchar_t CUST_MSG[";
            sOut += PTCustomizeUtil.MSG_TOTAL_LEN.ToString();
            sOut += "] = { ";

            // Add 4 constant numbers which are used to find the message entry within the EXE file
            // 0xFFFF, 0x000F, 0x9AB5, 0xD431
            sOut += "65535, 15, 39605, 54321, ";

            // Add some random prefix
            for (int charIndex = 4; charIndex < PTCustomizeUtil.MSG_NUM_FRONT_PAD; ++charIndex)
            {
                int val = (rand.Next() % 54321) + 5;
                sOut += val.ToString();
                sOut += ", ";

                outData[ dataOffset++ ] = (short)val;
            }

            // Add the string lengh
            sOut += msgLen.ToString();
            sOut += ", ";
            outData[ dataOffset++ ] = (short)msgLen;

            // Add the text
            for( int charIndex = 0; charIndex < msgLen; ++charIndex )
            {
                int val = (int)sEntered[charIndex] * 3;
                sOut += val.ToString();
                sOut += ", ";

                outData[ dataOffset++ ] = (short)val;
            }

            // Add some end characters
            int numRemainingChars = (PTCustomizeUtil.MSG_NUM_MSG_LEN - msgLen) + PTCustomizeUtil.MSG_NUM_END_PAD - 1;
            for (int charIndex = 0; charIndex < numRemainingChars; ++charIndex)
            {
                int val = (rand.Next() % 54321) + 5;
                sOut += val.ToString();

                outData[ dataOffset++ ] = (short)val;

                // If this is not the last characters
                if (charIndex < numRemainingChars - 1)
                    sOut += ", ";
            }

            sOut += " };";

            textBoxOutMsg.Text = sOut;

            //std::ofstream outFileBin( "c:\\test.bin", std::ios_base::binary | std::ios_base::out );
            //outFileBin.write( (const char*)outData, TOTAL_LEN * sizeof(wchar_t) );
            //outFileBin.close();
        }

        private void InitValuesFromReg( Control parentCtrl )
        {
            if (m_RegKey == null)
                return;

            // Go through the controls and restore values from the registry
            foreach (Control curCtrl in parentCtrl.Controls)
            {
                // Filter everything except text boxes
                TextBox curTextBox = curCtrl as TextBox;
                if (curTextBox == null)
                    InitValuesFromReg(curCtrl);
                // Else this is a text box
                else
                {
                    if( m_RegKey.DoesValueExist( curTextBox.Name ) )
                        curTextBox.Text = m_RegKey.GetValue( curTextBox.Name );
                }
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            m_RegKey.Init(RegKey.RootKeys.CurrentUser, "TaylorClarkSoftware\\MsgKeyGen");

            // Go through the controls and restore values from the registry
            InitValuesFromReg( this );
        }

        private void buttonFindOffset_Click(object sender, EventArgs e)
        {
            try
            {
                PTCustomizeUtil.FullKeyOffset keyOffset = PTExeDiscovery.FindKeyOffset( textBoxFindOffsetFile.Text );
                textBoxKeyOffset.Text = keyOffset.KeyOffset.ToString();

                textBoxInt64Pad.Text = keyOffset.Inter64bitPadding.ToString() + "," + keyOffset.Intra64bitPadding;
            }
            catch( Exception exc )
            {
                MessageBox.Show( "Failed: " + exc.Message );
            }
        }

        private void buttonFindMacPPCKeyOffset_Click(object sender, EventArgs e)
        {
            try
            {
                PTCustomizeUtil.FullKeyOffset keyOffset = PTExeDiscovery.FindMacPPCKeyOffset( textBoxFindOffsetFile.Text );

                textBoxKeyOffset.Text = keyOffset.KeyOffset.ToString();

                textBoxInt64Pad.Text = keyOffset.Inter64bitPadding.ToString() + "," + keyOffset.Intra64bitPadding;
            }
            catch( Exception exc )
            {
                MessageBox.Show( "Failed: " + exc.Message );
            }
        }

        private void buttonFindMacx86KeyOffset_Click(object sender, EventArgs e)
        {
            buttonFindOffset_Click( null, e );
            
        }

        private void buttonFindMacx64KeyOffset_Click(object sender, EventArgs e)
        {
            try
            {
                PTCustomizeUtil.FullKeyOffset keyOffset = PTExeDiscovery.FindMacx64KeyOffset( textBoxFindOffsetFile.Text );

                textBoxKeyOffset.Text = keyOffset.KeyOffset.ToString();

                textBoxInt64Pad.Text = keyOffset.Inter64bitPadding.ToString() + "," + keyOffset.Intra64bitPadding;
            }
            catch( Exception exc )
            {
                MessageBox.Show( "Failed: " + exc.Message );
            }
        }

        private void buttonFindOffsetBrowse_Click(object sender, EventArgs e)
        {
            if( openFileDialogFindOffset.ShowDialog() != DialogResult.OK )
                return;

            // Store the path
            textBoxFindOffsetFile.Text = openFileDialogFindOffset.FileName;
        }

        private void textBox_RegVal_TextChanged(object sender, EventArgs e)
        {
            TextBox senderTextBox = sender as TextBox;
            if (senderTextBox == null )
                return;

            m_RegKey.SetValue(senderTextBox.Name, senderTextBox.Text);
        }

        private void buttonFindMsgOffset_Click(object sender, EventArgs e)
        {
            textBoxMsgOffset.Text = PTExeDiscovery.FindMsgOffset( textBoxFindOffsetFile.Text ).ToString();
        }

        private void buttonFindMacPPCMsgOffset_Click(object sender, EventArgs e)
        {
            try
            {
                PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.LittleEndian;
                PTExeDiscovery.WcharSize = 4;

                textBoxMsgOffset.Text = PTExeDiscovery.FindMsgOffset( textBoxFindOffsetFile.Text ).ToString();
            }
            finally
            {
                // Put it back to PC values
                PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;
                PTExeDiscovery.WcharSize = 2;
            }
        }

        private void buttonFindMacx86MsgOffset_Click(object sender, EventArgs e)
        {
            try
            {
                PTExeDiscovery.WcharSize = 4;

                textBoxMsgOffset.Text = PTExeDiscovery.FindMsgOffset( textBoxFindOffsetFile.Text ).ToString();
            }
            finally
            {
                // Put it back to PC values
                PTExeDiscovery.WcharSize = 2;
            }
        }

        private void buttonFindMacx64MsgOffset_Click(object sender, EventArgs e)
        {
            try
            {
                PTExeDiscovery.WcharSize = 4;
                int offset = (int)PTExeDiscovery.FindMsgOffset( textBoxFindOffsetFile.Text );
                if( offset > 0 )
                    textBoxMsgOffset.Text = PTExeDiscovery.FindMsgOffset( textBoxFindOffsetFile.Text, offset + 10 ).ToString();
                else
                    textBoxMsgOffset.Text = "-1";
            }
            finally
            {
                // Put it back to PC values
                PTExeDiscovery.WcharSize = 2;
            }
        }

        private void buttonOverwriteMsg_Click(object sender, EventArgs e)
        {
            // Ensure the offset box contains a valid value
            long offset = 0;
            if( !long.TryParse( textBoxMsgOffset.Text, out offset ) )
            {
                MessageBox.Show( "Ensure a valid value is enteted into the message offset text box." );
                return;
            }

            // Overwrite the message
            m_PTUtil.OverwriteMsg(textBoxFindOffsetFile.Text, offset, textBoxOverwriteMsg.Text);
        }

        private void buttonOverwiteMacMsg_Click(object sender, EventArgs e)
        {
            try
            {
                PTExeDiscovery.WcharSize = 4;
                PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.LittleEndian;
                
                long ppcOffset = PTExeDiscovery.FindMsgOffset( textBoxFindOffsetFile.Text );

                PTExeDiscovery.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;

                long x86Offset = PTExeDiscovery.FindMsgOffset( textBoxFindOffsetFile.Text );

                long x64Offset = PTExeDiscovery.FindMsgOffset( textBoxFindOffsetFile.Text, x86Offset + 10 );

                // Ensure the offset box contains a valid value
                if( ppcOffset == 0 || x86Offset == 0 || x64Offset == 0 )
                {
                    MessageBox.Show( "Couldn't find one of the three offsets." );
                    return;
                }

                // Overwrite the message
                m_PTUtil.OverwriteMacMsg( textBoxFindOffsetFile.Text, ppcOffset, x86Offset, x64Offset, textBoxOverwriteMsg.Text );
            }
            finally
            {
                m_PTUtil.CharByteOrder = PTCustomizeUtil.CharacterByteOrder.BigEndian;
                m_PTUtil.WcharSize = 2;
            }
        }


        private void buttonOverwriteKey_Click(object sender, EventArgs e)
        {
            // Get the key value
            UInt64 keyValue = 0;
            if (textBoxOverwriteKeyValue.Text.StartsWith("0x"))
            {
                string val = textBoxOverwriteKeyValue.Text.Substring( 2 );
                if( !UInt64.TryParse( val, NumberStyles.AllowHexSpecifier, NumberFormatInfo.CurrentInfo, out keyValue ) )
                {
                    MessageBox.Show("Invalid key value entered");
                    return;
                }	
            }
            else if( !UInt64.TryParse( textBoxOverwriteKeyValue.Text, out keyValue ) )
            {
                MessageBox.Show("Invalid key value entered");
                return;
            }	
            

            // Get the value offset
            long keyOffsetx = 0;
            if (!long.TryParse(textBoxKeyOffset.Text, out keyOffsetx))
            {
                MessageBox.Show("Invalid key offset entered");
                return;
            }

            PTCustomizeUtil.FullKeyOffset keyOffset = new PTCustomizeUtil.FullKeyOffset();
            keyOffset.KeyOffset = keyOffsetx;

            string[] int64KeyOffsets = textBoxInt64Pad.Text.Split(',');
            keyOffset.Inter64bitPadding = int.Parse(int64KeyOffsets[0]);
            keyOffset.Intra64bitPadding = int.Parse(int64KeyOffsets[1]);

            // Overwrite the key
            m_PTUtil.OverwriteValueInEXE( textBoxFindOffsetFile.Text, keyValue, keyOffset );
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// A message handler called when the user changes the contents of the message text box
        /// </summary>
        ///////////////////////////////////////////////////////////////////////////////////////////
        private void textBoxMsg_TextChanged(object sender, EventArgs e)
        {
            labelGenMsgLen.Text = "Length: " + textBoxMsg.Text.Length.ToString();

            textBox_RegVal_TextChanged( sender, e );
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// A message handler called when the user changes the contents of the overwrite message
        /// text box
        /// </summary>
        ///////////////////////////////////////////////////////////////////////////////////////////
        private void textBoxOverwriteMsg_TextChanged(object sender, EventArgs e)
        {
            labelOverwriteMsgLen.Text = "Length: " + textBoxOverwriteMsg.Text.Length.ToString();

            textBox_RegVal_TextChanged(sender, e);
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// A message handler called when the user changes the contents of the Int64 padding
        /// message text box
        /// </summary>
        ///////////////////////////////////////////////////////////////////////////////////////////
        private void textBoxInt64Pad_TextChanged(object sender, EventArgs e)
        {
            try
            {
                textBox_RegVal_TextChanged( sender, e );
            }
            catch( FormatException )
            {
            }
        }

        private void buttonMakeKey_Click(object sender, EventArgs e)
        {
            uint[] keyVals = new uint[7] { 4, 13, 22, 18, 15, 2, 1 };
            textBoxGenKeyVal.Text = m_PTUtil.MakeKey( keyVals ).ToString();
        }

        private void textBoxReadKey_DragEnter(object sender, DragEventArgs e)
        {
            // By default, do not allow dropping
            e.Effect = DragDropEffects.None;

            // If there is file data
            if( e.Data.GetDataPresent( DataFormats.FileDrop ) )
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
                if( files.Length == 1 )
                    e.Effect = DragDropEffects.Copy;
            }
        }

        private void textBoxReadKey_DragDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData( DataFormats.FileDrop );

            ReadKey( files[ 0 ] );
        }

        private void buttonReadKey_Click(object sender, EventArgs e)
        {
            ReadKey( textBoxFindOffsetFile.Text );
        }

        private void ReadKey( string exeFile )
        {
            // Get the value offset
            long keyOffsetVal = 0;
            if( !long.TryParse( textBoxKeyOffset.Text, out keyOffsetVal ) )
            {
                MessageBox.Show("Invalid key offset entered");
                return;
            }

            PTCustomizeUtil.FullKeyOffset keyOffset = new PTCustomizeUtil.FullKeyOffset();
            keyOffset.KeyOffset = keyOffsetVal;

            string[] int64KeyOffsets = textBoxInt64Pad.Text.Split( ',' );
            keyOffset.Inter64bitPadding = int.Parse( int64KeyOffsets[0] );
            keyOffset.Intra64bitPadding = int.Parse( int64KeyOffsets[1] );

            UInt64 keyVal = m_PTUtil.GetKeyFromExe( exeFile, keyOffset );

            textBoxReadKey.Text = keyVal.ToString() + " ( 0x" + keyVal.ToString("X") + " )";
        }

        private void buttonPickCustomPic_Click(object sender, EventArgs e)
        {
            if( openFileDialogCustomPic.ShowDialog() != DialogResult.OK )
                return;

            // Store the path
            textBoxCustomPic.Text = openFileDialogCustomPic.FileName;

            try
            {
                pictureBoxCustomPic.Image = LoadImageForCustomPic( textBoxCustomPic.Text );
            }
            catch( Exception )
            {
                pictureBoxCustomPic.Image = null;
            }
        }

        private Bitmap LoadImageForCustomPic(string imageFilePath)
        {
            Image sourceImage = Image.FromFile( textBoxCustomPic.Text );

            Bitmap destImage = new Bitmap( 128, 128, System.Drawing.Imaging.PixelFormat.Format24bppRgb );

            float scaleAmount = sourceImage.Width < sourceImage.Height ? ( 128.0f / sourceImage.Height ) : ( 128.0f / sourceImage.Width );

            // Get the offset so the scaled image is centered
            Rectangle destRect = new Rectangle();
            destRect.Width = (int)(sourceImage.Width * scaleAmount);
            destRect.Height = (int)(sourceImage.Height * scaleAmount);
            destRect.X = (128 - destRect.Width) / 2;
            destRect.Y = (128 - destRect.Height) / 2;
            
            Graphics destGfx = Graphics.FromImage( destImage );

            destGfx.Clear( Color.Magenta );

            destGfx.DrawImage( sourceImage, destRect );

            destGfx.Dispose();

            return destImage;
        }

        private void buttonEmbedCustomPic_Click(object sender, EventArgs e)
        {
            if( !( pictureBoxCustomPic.Image is Bitmap )
                || pictureBoxCustomPic.Image.Width != 128
                || pictureBoxCustomPic.Image.Height != 128 )
            {
                MessageBox.Show( "Please select a valid image" );
                return;
            }

            long offset = long.Parse( textBoxCustomPicOffset.Text );
            m_PTUtil.EmbedCustomPic( textBoxFindOffsetFile.Text, offset, (Bitmap)pictureBoxCustomPic.Image );
        }

        private void buttonFindCustomPicOffset_Click(object sender, EventArgs e)
        {
            textBoxCustomPicOffset.Text = PTExeDiscovery.FindCustomPicOffset( textBoxFindOffsetFile.Text ).ToString();
            textBoxCustomPicMsgOffset.Text = PTExeDiscovery.FindCustomPicMsgOffset( textBoxFindOffsetFile.Text ).ToString();
        }

        private void buttonFindMacPicOffsets_Click(object sender, EventArgs e)
        {
            string offset = String.Empty;

            PTCustomizeUtil.MacOffset macPicDataOffset = PTExeDiscovery.FindMacCustomPicDataOffsets( textBoxFindOffsetFile.Text );
            textBoxCustomPicOffset.Text = macPicDataOffset.ToString();

            PTCustomizeUtil.MacOffset macPicMsgOffset = PTExeDiscovery.FindMacCustomPicMsgOffset( textBoxFindOffsetFile.Text );
            textBoxCustomPicMsgOffset.Text = macPicMsgOffset.ToString();			
        }

        private void textBoxCustomPicOffset_TextChanged(object sender, EventArgs e)
        {
            textBox_RegVal_TextChanged( sender, e );
        }

        private void textBoxCustomPicMsgOffset_TextChanged(object sender, EventArgs e)
        {
            textBox_RegVal_TextChanged( sender, e );
        }

        private void buttonFindAllOffsets_Click(object sender, EventArgs e)
        {
            UseWaitCursor = true;

            textBoxKeyOffset.Text = String.Empty;
            textBoxInt64Pad.Text = String.Empty;
            textBoxMsgOffset.Text = String.Empty;
            textBoxCustomPicOffset.Text = String.Empty;
            textBoxCustomPicMsgOffset.Text = String.Empty;

            Update();

            buttonFindOffset_Click( sender, e );
            buttonFindMsgOffset_Click( sender, e );
            buttonFindCustomPicOffset_Click( sender, e );

            buttonFindExpirationDateOffset_Click( sender, e );

            UseWaitCursor = false;
        }

        private void buttonOverwriteCustomPicMsg_Click(object sender, EventArgs e)
        {
            // Ensure the offset box contains a valid value
            long offset = 0;
            if( !long.TryParse( textBoxCustomPicMsgOffset.Text, out offset ) )
            {
                MessageBox.Show( "Ensure a valid value is enteted into the message offset text box." );
                return;
            }

            // Overwrite the message
            m_PTUtil.OverwriteMsg( textBoxFindOffsetFile.Text, offset, textBoxCustomPicMsg.Text );
        }

        private void buttonFindExpirationDateOffset_Click(object sender, EventArgs e)
        {
            long offset = PTExeDiscovery.FindEvalExpirationDateOffset( textBoxFindOffsetFile.Text );

            textBoxExpirationDateOffset.Text = offset.ToString();
        }

        private void textBoxExpirationDateOffset_TextChanged(object sender, EventArgs e)
        {
            textBox_RegVal_TextChanged( sender, e );
        }

        private void buttonSetExpirationDate_Click(object sender, EventArgs e)
        {
            long offset = long.Parse( textBoxExpirationDateOffset.Text );
            m_PTUtil.m_ExpirationDateOffset = offset;

            m_PTUtil.SetExpirationDate( textBoxFindOffsetFile.Text, dateTimePickerExpiration.Value );
        }

        private void buttonGenerateExeInfo_Click(object sender, EventArgs e)
        {
            if( saveFileDialogInfo.ShowDialog() == System.Windows.Forms.DialogResult.OK )
                PTExeDiscovery.GenerateExeInfoFile( textBoxFindOffsetFile.Text, saveFileDialogInfo.FileName );
        }

        private void buttonGenerateMacExeInfo_Click(object sender, EventArgs e)
        {
            if( saveFileDialogInfo.ShowDialog() == System.Windows.Forms.DialogResult.OK )
                PTExeDiscovery.GenerateMacExeInfoFile( textBoxFindOffsetFile.Text, saveFileDialogInfo.FileName );
        }
    }
}