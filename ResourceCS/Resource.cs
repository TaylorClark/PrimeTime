using System;
using System.Collections.Generic;
using System.Text;

namespace ResourceCS
{
	// The resource types
	// *** MUST MATCH Resource.h
	public enum ResourceType : int
	{
		Error = -1,
		Image = 0,
		Sound,
		Music,
		Font,
		Sprite,
		COUNT
	}

	public class ResourceToolsDB
	{
		/// <summary>
		/// An entry into the tools resource index
		/// </summary>
		public struct ResourceIndexItem
		{
			/// <summary> The name of the resource </summary>
			public string sName;

			/// <summary> The file where the resource resides </summary>
			public string sFileName;

			/// <summary> The type of resource this is </summary>
			public ResourceType type;

			/// <summary> The resource define identifier </summary>
			public string sDefineName;
 
			/// <summary> The resource ID </summary>
			public uint ID;
		}

		/// <summary> The maximum name length of a resource including the NULL terminating
		/// zero. NOTE: Changing this will invalidate all resources. </summary>
		public const int REZ_NAME_LEN = 24;

		/// <summary> The FourCC for all game resource files </summary>
		public static readonly TCBase.FourCC REZ_FOURCC = new TCBase.FourCC("GRES");

		public static readonly string REGKEYPATH_BASEPATH = "SOFTWARE/PrimeTime Tools";
		public static readonly string REGKEYPATH_RESPATH = "ResPath";
		public static readonly string FILENAME_ToolsResIndex = "ToolsResIndex.dat";

		/// <summary> The list of known resources found in the tools resource index </summary>
		private SortedList< uint, ResourceIndexItem> m_ResList = new SortedList< uint, ResourceIndexItem>();

		public string RootResourcePath { get; set; }


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Load a tool resource index file.
		/// </summary>
		/// <param name="sFileName"> The file name to load </param>
		/// <returns></returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public bool LoadToolsIndex(string sFileName)
		{
			try
			{
				// Open the file
				System.IO.FileStream inFile = new System.IO.FileStream(sFileName, System.IO.FileMode.Open, System.IO.FileAccess.Read);
				System.IO.BinaryReader binReader = new System.IO.BinaryReader(inFile, Encoding.Unicode );

				// Read in the file key
				TCBase.FourCC FOURCC_TOOLSDB = new TCBase.FourCC("TRDB");
				int fourCCVal = binReader.ReadInt32();
				TCBase.FourCC fileFourCC = new TCBase.FourCC(fourCCVal);

				// Ensure a proper key
				if (fourCCVal != FOURCC_TOOLSDB.ToInt32())
				{
					inFile.Close();
					return false;
				}

				// Read in the version
				int fileVersion = binReader.ReadInt32();
				if (fileVersion == 1)
				{
				}
				else if (fileVersion == 2)
				{
					int numChars = (int)binReader.ReadUInt32();

					byte[] stringBytes = binReader.ReadBytes( numChars * 2 );

					// Read in the resource base path
					RootResourcePath = Encoding.Unicode.GetString( stringBytes );
				}
				else
					throw new ApplicationException( "Bad file version" );

				// Read in the number of resources
				int numResources = binReader.ReadInt32();

				// Size the list based on the number of resources
				m_ResList = new SortedList<uint, ResourceIndexItem>(numResources);

				// Read in the resource index items
				for (int itemIndex = 0; itemIndex < numResources; ++itemIndex)
				{
					ResourceIndexItem newItem = new ResourceIndexItem();

					// Read in the length of the name
					int nameLen = binReader.ReadInt32();

					// Read in the name
					char[] resName = binReader.ReadChars(nameLen);
					newItem.sName = new string(resName);

					// Read in the length of the file name
					int fileNameLen = binReader.ReadInt32();

					// Read in the file name
					char[] fileName = binReader.ReadChars(fileNameLen);
					newItem.sFileName = new string(fileName);

					// Read in the type
					int resType = binReader.ReadInt32();
					newItem.type = (ResourceType)resType;

					// Read in the length of the resource define identifier
					int defineNameLen = binReader.ReadInt32();

					// Read in the resource define
					char[] defineName = binReader.ReadChars(defineNameLen);
					newItem.sDefineName = new string(defineName);

					// Read in the ID
					newItem.ID = binReader.ReadUInt32();

					// Read in the image type
					int imageType = binReader.ReadInt32();

					// Store the item
					m_ResList.Add( newItem.ID, newItem );
				}

				// Close the file
				inFile.Close();
			}
			catch (Exception exc)
			{
				System.Windows.Forms.MessageBox.Show(exc.Message, "Failed To Load Tools Resource Index");
				return false;
			}

			// Return success
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get resource information about a resource ID
		/// </summary>
		/// <param name="resID"> The resource ID for which data is being retrieved </param>
		/// <returns> The resource index info or null if the ID is unknown </returns>
		/// <exception cref="System.Exception"> Thrown if the resource ID is unknown </exception>
		///////////////////////////////////////////////////////////////////////////////////////////
		public ResourceIndexItem GetResourceInfo(uint resID)
		{
			// If there is no known resource with this ID
			if (!m_ResList.ContainsKey(resID))
				throw (new Exception("Unknown reource ID."));

			return m_ResList[resID];
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a list of all resource IDs
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public List<uint> GetAllResIDs()
		{
			return new List<uint>( m_ResList.Keys );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the list of resource IDs of a specific type from the list of known resources
		/// </summary>
		/// <param name="type"> The type of resources to find </param>
		/// <returns> The list of all resources of the passed-in type </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public List<uint> GetResourcesOfType(ResourceType type)
		{
			List<uint> retList = new List<uint>();

			// Go through the resource list and add all that match the passed in type
			foreach (KeyValuePair<uint, ResourceIndexItem> resItem in m_ResList)
			{
				// If this item is the correct type then store it
				if (resItem.Value.type == type)
					retList.Add(resItem.Key);
			}

			// Return the list
			return retList;
		}
	}
}
