using System.Diagnostics;
using System.Collections;
using System.ComponentModel;
using System.Configuration.Install;
using System.Reflection;
using System.IO;

namespace RunAfterInstall
{
	// Taken from:http://msdn2.microsoft.com/en-us/library/
	// system.configuration.configurationmanager.aspx
	// Set 'RunInstaller' attribute to true.

	[RunInstaller( true )]
	public class InstallerClass : System.Configuration.Install.Installer
	{
		public InstallerClass()
			: base()
		{
			// Attach the 'Committed' event.
			this.Committed += new InstallEventHandler( MyInstaller_Committed );
			// Attach the 'Committing' event.
			this.Committing += new InstallEventHandler( MyInstaller_Committing );
		}

		// Event handler for 'Committing' event.
		private void MyInstaller_Committing(object sender, InstallEventArgs e)
		{
			//Console.WriteLine("");
			//Console.WriteLine("Committing Event occurred.");
			//Console.WriteLine("");
		}

		// Event handler for 'Committed' event.
		private void MyInstaller_Committed(object sender, InstallEventArgs e)
		{
			try
			{
				Directory.SetCurrentDirectory( Path.GetDirectoryName( Assembly.GetExecutingAssembly().Location ) );
				Process.Start( Path.GetDirectoryName( Assembly.GetExecutingAssembly().Location ) + "\\InstallSample.exe" );
			}
			catch
			{
				// Do nothing... 
			}
		}
	}
}