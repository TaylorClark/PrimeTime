using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using PTWebApp;

namespace MsgKeyGen
{
	static class Program
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main( string[] args )
		{
			if (args.Length > 0 ) 
			{
				HandleCmdLine( args );
				return;
			}
			
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			Application.Run(new Form1());
		}

		private static void HandleCmdLine( string[] args )
		{
			// If the application is being run from the command line
			if ( args[0] == "/geninfofile")
			{
				if( args.Length != 3 )
				{
					MessageBox.Show( "Pass the EXE path as the first parameter and the output file as the second." );
					return;
				}

				// Get the EXE path
				string exePath = args[1];
				string outFile = args[2];
				FileInfo exeInfo = new FileInfo(exePath);
				if (!exeInfo.Exists)
					Console.WriteLine("Cannot find EXE file (" + exePath + ")");
				else
					PTExeDiscovery.GenerateExeInfoFile( exePath, outFile );

				return;
			}
			else if( args[0] == "/genmacinfofile" )
			{
				if( args.Length != 3 )
				{
					MessageBox.Show( "Pass the EXE path as the first parameter and the output file as the second." );
					return;
				}

				// Get the EXE path
				string exePath = args[1];
				string outFile = args[2];
				FileInfo exeInfo = new FileInfo( exePath );
				if( !exeInfo.Exists )
					Console.WriteLine( "Cannot find EXE file (" + exePath + ")" );
				else
					PTExeDiscovery.GenerateMacExeInfoFile( exePath, outFile );

				return;
			}
			else if ( args[0] == "/setkey")
			{
				if (args.Length != 5)
				{
					MessageBox.Show("USAGE: MsgKeyGen /setkey keyoffset intra64Padding keyvalue exepath");
					return;
				}

				// Get the values
				long keyOffsetVal = long.Parse( args[ 1 ] );
				UInt64 keyValue = UInt64.Parse( args[2] );
				int intra64Padding = int.Parse( args[ 3 ] );
				string exePath = args[ 4 ];

				FileInfo exeInfo = new FileInfo(exePath);
				if (!exeInfo.Exists)
					Console.WriteLine("Cannot find EXE file (" + exePath + ")");
				else
				{
					PTWebApp.PTCustomizeUtil util = new PTWebApp.PTCustomizeUtil();

					PTCustomizeUtil.FullKeyOffset keyOffset = new PTCustomizeUtil.FullKeyOffset();
					keyOffset.KeyOffset = keyOffsetVal;
					keyOffset.Intra64bitPadding = intra64Padding;

					// Modify the EXE
					util.OverwriteValueInEXE(exePath, keyValue, keyOffset);
					Console.WriteLine("EXE key updated");
				}

				return;
			}
			else if (args[0] == "/findandsetkey")
			{
				if (args.Length != 3)
				{
					MessageBox.Show("USAGE: MsgKeyGen /findandsetkey keyvalue exepath");
					return;
				}

				// Get the values
				UInt64 keyValue = UInt64.Parse( args[1] );
				string exePath = args[2];

				FileInfo exeInfo = new FileInfo(exePath);
				if (!exeInfo.Exists)
					Console.WriteLine("Cannot find EXE file (" + exePath + ")");
				else
				{
					PTCustomizeUtil util = new PTCustomizeUtil();

					// Modify the EXE
					PTCustomizeUtil.FullKeyOffset keyOffset = PTExeDiscovery.FindKeyOffset( exePath );
					if( keyOffset.KeyOffset != 0 )
					{
						util.OverwriteValueInEXE( exePath, keyValue, keyOffset );
						Console.WriteLine("EXE key updated (Key found at:" + keyOffset + ")");
					}
					else
						Console.WriteLine("Failed to find key offsset");
				}

				return;
			}
			else if (args[0] == "/setmsg")
			{
				if (args.Length != 4)
				{
					MessageBox.Show("USAGE: MsgKeyGen /setmsg msgoffset msg exepath");
					return;
				}

				// Get the values
				long msgOffset = long.Parse(args[1]);
				string msg = args[2];
				string exePath = args[3];

				FileInfo exeInfo = new FileInfo(exePath);
				if (!exeInfo.Exists)
					Console.WriteLine("Cannot find EXE file (" + exePath + ")");
				else
				{
					PTCustomizeUtil util = new PTCustomizeUtil();

					// Modify the EXE
					util.OverwriteMsg(exePath, msgOffset, msg);
					Console.WriteLine("EXE message updated");
				}

				return;
			}
			else if (args[0] == "/findandsetmsg")
			{
				if (args.Length != 3)
				{
					MessageBox.Show("USAGE: MsgKeyGen /findandsetmsg msg exepath");
					return;
				}

				// Get the values
				string msg = args[1];
				string exePath = args[2];

				FileInfo exeInfo = new FileInfo(exePath);
				if (!exeInfo.Exists)
					Console.WriteLine("Cannot find EXE file (" + exePath + ")");
				else
				{
					long msgOffset = PTExeDiscovery.FindMsgOffset( exePath );
				
					// Modify the EXE
					if (msgOffset != 0)
					{
						new PTCustomizeUtil().OverwriteMsg( exePath, msgOffset, msg );
						Console.WriteLine( "EXE message updated (message found at: " + msgOffset + ")" );
					}
					else
						Console.WriteLine("Failed to find message offset");
				}

				return;
			}
		}
	}
}