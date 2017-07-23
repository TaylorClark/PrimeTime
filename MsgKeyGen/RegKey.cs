using System;

/// Modified April 17, 2006  Add root key enum and init using the enum.
/// fixed comments

namespace TCUtil
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A class used to interface with the windows registry
	/// </summary>
	//-------------------------------------------------------------------------
	class RegKey
	{
		/// <summary> The possible registry root keys </summary>
		public enum RootKeys
		{
			LocalMachine,
			CurrentUser,
			Users,
			CurrentConfig,
			ClassesRoot,
			RK_COUNT
		};

		/// <summary> The registry key that stores the values </summary>
		private Microsoft.Win32.RegistryKey m_RegKey = null;

		/// <summary>
		/// A member that contains a helpful string describing problems if
		/// there are any
		/// </summary>
		private string m_ErrorStr = ERROR_STR_NO_ERROR;

		/// <summary> Error strings </summary>
		private const string ERROR_STR_NO_ERROR = "No Error";
		private const string ERROR_STR_NOT_INIT = "The registry key object has not been initialized";
		private const string ERROR_STR_NO_PERMISSION = "The current user does not have permision to create or open registry keys and values";
		private const string ERROR_STR_NO_VALUE = "The value does not exist";
		private const string ERROR_STR_OP_FAILED = "The operation failed";
        

		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The destructor for the class closes the registry key if needed
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		~RegKey()
		{
			// If we are initialized, close the key
			if( m_RegKey != null )
			{
				m_RegKey.Close();
				m_RegKey = null;
			}
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the registry key class for use
		/// </summary>
		/// <param name="baseKeyName"> The name of the base key that values are
		/// stored under </param>
		/// <returns> True if this registry key is ready for use, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public bool Init( string baseKeyName )
		{
			// Initialize
			return InitInternal( RootKeys.LocalMachine, baseKeyName );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the registry key class for use
		/// </summary>
		/// <param name="rootKey"> The root key to use in the registry </param>
		/// <param name="baseKeyPath"> The name of the base key that values are
		/// stored under, all keys are opened under the SOFTWARE node </param>
		/// <returns> True if this registry key is ready for use, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public bool Init( RootKeys rootKey, string baseKeyPath )
		{
			// Initialize
			return InitInternal(rootKey, baseKeyPath);
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize this registry key by using the product name as the parent
		/// key that values are stored under
		/// </summary>
		/// <returns> True if this registry key is ready for use, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public bool InitProductName()
		{
			// Create the key for this application
			string appPath = System.Windows.Forms.Application.ProductName;

			// Initialize
			return InitInternal( RootKeys.LocalMachine, appPath );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize this registry key by using the path of the running
		/// application as the parent key that values are stored under
		/// </summary>
		/// <returns> True if this registry key is ready for use, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		//public bool InitPathDependent()
		//{
		//    // Create the key for this application
		//    string appPath = System.Windows.Forms.Application.ExecutablePath;

		//    // Initialize
		//    return InitInternal( RootKeys.LocalMachine, appPath);
		//}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the base registry keys
		/// </summary>
		/// <param name="keyPath"> The key to create to store values </param>
		/// <returns> True if this registry key is ready for use, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		private bool InitInternal(RootKeys rootKey, string keyPath)
		{
			// Get the base key
			Microsoft.Win32.RegistryKey baseKey = Microsoft.Win32.Registry.LocalMachine;
			switch (rootKey)
			{
				case RootKeys.ClassesRoot:
					baseKey = Microsoft.Win32.Registry.ClassesRoot;
					break;

				case RootKeys.CurrentConfig:
					baseKey = Microsoft.Win32.Registry.CurrentConfig;
					break;

				case RootKeys.CurrentUser:
					baseKey = Microsoft.Win32.Registry.CurrentUser;
					break;

				case RootKeys.Users:
					baseKey = Microsoft.Win32.Registry.Users;
					break;
			}

			// If we are reinitializing
			if( m_RegKey != null )
			{
				m_RegKey.Close();
				m_RegKey = null;
			}

			try
			{
				// Ensure backslashes
				keyPath = keyPath.Replace('/', '\\');
                keyPath = keyPath.Replace("SOFTWARE\\", String.Empty);

                baseKey = baseKey.OpenSubKey("SOFTWARE", true );
				
                // Create our registry key
				m_RegKey = baseKey.CreateSubKey(keyPath);
				if( m_RegKey == null )
				{
					m_ErrorStr = ERROR_STR_OP_FAILED;
					return false;
				}

				// Update the registry in case we created the key
				m_RegKey.Flush();

				// If we got here then we successfully created our key
				IsInitialized = true;
				return true;
			}
			// Handle security excpetions
			catch( System.Security.SecurityException )
			{
				m_ErrorStr = ERROR_STR_NO_PERMISSION;
				return false;
			}
			// Handle generic exceptions
			catch( System.Exception genExc )
			{
				m_ErrorStr = genExc.Message;
				return false;
			}
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>Test if a value exists for a specific key</summary>
		/// <param name="valName"> The name of the value to test </param>
		/// <returns> True if the value key exists, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public bool DoesValueExist( string valName )
		{
			object value = m_RegKey.GetValue( valName );
			if( value == null )
				return false;

			return !String.IsNullOrEmpty(value.ToString());
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>Test if a value exists for a specific key. This is the
		/// same as DoesValueExist, but with a better name in my opinion. I
		/// left DoesValueExist for backwards compatibilty.</summary>
		/// <param name="valName"> The name of the value to test </param>
		/// <returns> True if the value key exists, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public bool HasValue(string valName)
		{
			return DoesValueExist(valName);
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a value from the registry
		/// </summary>
		/// <param name="valName"> The name of the value </param>
		/// <param name="val"> The string to be filled in with the value </param>
		/// <returns> True if the value was received, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public bool GetValue( string valName, ref string val )
		{
			// If we are not initialized then bail
			if( m_RegKey == null )
			{
				m_ErrorStr = ERROR_STR_NOT_INIT;
				return false;
			}

			// Attempt to get the value
			System.String retVal = m_RegKey.GetValue( valName ) as System.String;
			if( retVal == null )
			{
				m_ErrorStr = ERROR_STR_NO_VALUE;
				return false;
			}

			// We retrieved the value
			val = (string)retVal;
			return true;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a value from the registry
		/// </summary>
		/// <param name="valName"> The name of the value </param>
		/// <returns> True if the value was received, false otherwise </returns>
		/// <exception cref=""></exception>
		///////////////////////////////////////////////////////////////////////
		public string GetValue(string valName )
		{
			// If we are not initialized then bail
			if (m_RegKey == null)
			{
				m_ErrorStr = ERROR_STR_NOT_INIT;
				throw new NullReferenceException();
			}

			// Attempt to get the value
			System.String retVal = m_RegKey.GetValue(valName) as System.String;
			if (retVal == null)
			{
				m_ErrorStr = ERROR_STR_NO_VALUE;
				throw new ArgumentException("The key name " + valName + " is unknown");
			}

			// Return the value
			return (string)retVal;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set a value in the registry.  Create the value if it does not
		/// exist.
		/// </summary>
		/// <param name="valName"> The name of the value </param>
		/// <param name="val"> The string containing the value </param>
		/// <returns> True if the value was set, false otherwise </returns>
		///////////////////////////////////////////////////////////////////////
		public bool SetValue( string valName, string val )
		{
			// If we are not initialized
			if( m_RegKey == null )
			{
				m_ErrorStr = ERROR_STR_NOT_INIT;
				return false;
			}

			try
			{
				// Set the value
				m_RegKey.SetValue( valName, val );

				// Update the registry
				m_RegKey.Flush();
			}
			catch( System.Exception e )
			{
				m_ErrorStr = e.Message;
				return false;
			}

			// We set the value
			return true;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get all of the values this key contains
		/// </summary>
		/// <returns> A sorted list with the name as the key and the value as
		/// the value </returns>
		///////////////////////////////////////////////////////////////////////
		public System.Collections.SortedList GetAllValues()
		{
			// Get the values of this sub key
			string[] values = m_RegKey.GetValueNames();

			// Go through and build the map of names to values
			System.Collections.SortedList retList = new System.Collections.SortedList( values.Length );
			foreach( string name in values )
			{
				// Get the value
				string val = "";
				if( !GetValue( name, ref val ) )
					continue;

				// Add the item
				retList.Add( name, val );
			}

			// Return the list
			return retList;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Clear all of the values under this registry key
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public void DeleteAllValues()
		{
			// Get the value names
			System.Collections.SortedList values = GetAllValues();

			// Remove each one
			foreach( string valueName in values.Keys )
				m_RegKey.DeleteValue( valueName, false );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Get a sub key of this registry key and fail if it does
		/// not exist </summary>
		/// <param name="subKeyName"> The name of the sub key </param>
		/// <returns> The registry key to use, null if the sub key does not
		/// exist </returns>
		///////////////////////////////////////////////////////////////////////
		public RegKey GetSubKey( string subKeyName )
		{
			// Attempt to open the sub key
			Microsoft.Win32.RegistryKey subKey = m_RegKey.OpenSubKey( subKeyName, true );
			if( subKey == null )
				return null;

			// Create a new registry key
			RegKey retSubKey = new RegKey();
			retSubKey.m_RegKey = subKey;

			// Return the sub key object
			return retSubKey;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Get a sub key of this registry key and create it if it
		/// does no exist </summary>
		/// <param name="subKeyName"> The name of the sub key </param>
		/// <param name="createIfDoesntExist"> True if the sub key should be
		/// created if it doesn't exist, false if the subkey will not be
		/// created </param>
		/// <returns> The registry key to use, null if the sub key does not
		/// exist or couldn't be created </returns>
		///////////////////////////////////////////////////////////////////////
		public RegKey CreateSubKey( string subKeyName )
		{
			// Attempt to get the registry key
			RegKey subKey = GetSubKey( subKeyName );

			// If we got the sub key
			if( subKey != null )
				return subKey;

			// Attempt to create the sub key
			Microsoft.Win32.RegistryKey newSubKey = m_RegKey.CreateSubKey( subKeyName );
			if( newSubKey == null )
				return null;

			// Create and return the new sub key object
			RegKey retSubKey = new RegKey();
			retSubKey.m_RegKey = newSubKey;
			return retSubKey;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Delete a sub key or sub key tree </summary>
		/// <param name="subKeyName"> The sub key to remove </param>
		/// <param name="removeTree"> True if we are deleting the entire tree
		/// under the sub key </param>
		/// <returns> Whether or not the delete action succeeded </returns>
		///////////////////////////////////////////////////////////////////////
		public bool DeleteSubKey( string subKeyName, bool removeTree )
		{
			bool wasSubKeyRemoved = false;

			// If we are removing the whole tree
			if( removeTree )
			{
				// Delete the sub key tree (Note: placed in a try-catch block
				// to test if the entire sub key tree was really removed )
				try
				{
					m_RegKey.DeleteSubKeyTree( subKeyName );
					wasSubKeyRemoved = true;
				}
				catch{}
			}
			// Else we should just remove a subkey
			else
			{
				// Delete the sub key (Note: placed in a try-catch block
				// to test if the sub key was really removed )
				try
				{
					m_RegKey.DeleteSubKey( subKeyName, true );
					wasSubKeyRemoved = true;
				}
				catch{}
			}

			// Return the result
			return wasSubKeyRemoved;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string describing the last error that occurred 
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public string LastErrorString
		{
			get{ return m_ErrorStr; }
		}

		public bool IsInitialized { get; set; }
	}
}