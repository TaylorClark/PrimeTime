using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ResourceCS
{
	public partial class ResChooser : Form
	{
		private class TreeNodeResource : TreeNode
		{
			public TreeNodeResource(string sText, uint inResID)
				: base(sText)
			{
				resID = inResID;
			}

			/// <summary> The resource ID associated with this node </summary>
			public uint resID = -1;
		}


		/// <summary> The tools resource database </summary>
		private ResourceToolsDB m_ResDB = new ResourceToolsDB();

		/// <summary> The selected resource ID </summary>
		private uint m_ResID;

		/// <summary>
		/// The list of resource types to allow the user to select, if empty then all resource
		/// types are displayed
		/// </summary>
		private ResourceType[] m_Filters = new ResourceType[0];


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> The default constructor </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public ResChooser( ResourceToolsDB resDB )
		{
			// Initialize the controls
			InitializeComponent();

			// Place all resource types in the filter
			const int numResources = (int)ResourceType.COUNT;
			m_Filters = new ResourceType[numResources];
			for (int resTypeIndex = 0; resTypeIndex < numResources; ++resTypeIndex)
				m_Filters[resTypeIndex] = (ResourceType)resTypeIndex;
		
			// Store the resource database
			m_ResDB = resDB;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> The constructor used to filter the resources available to the
		/// user </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public ResChooser( ResourceToolsDB resDB, params ResourceType[] filters)
			: this(resDB)
		{
			// Store the filters
			m_Filters = filters;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the form is loaded.
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void ResChooser_Load(object sender, EventArgs e)
		{
			// Ensure an empty tree
			treeViewResources.BeginUpdate();
			treeViewResources.Nodes.Clear();

			// Go through all of the filter types
			foreach (ResourceType curResType in m_Filters)
			{
				// Create the base item for this type
				TreeNode typeRootNode = new TreeNode(curResType.ToString());
				treeViewResources.Nodes.Add(typeRootNode);

				// Get all of the resources of this type
				List<uint> resIDList = m_ResDB.GetResourcesOfType(curResType);
 
				// If there are no resources by this type then bail
				if (resIDList.Count == 0)
					continue;

				// Go through all of the resource IDs found
				foreach (uint curResID in resIDList)
				{
					// Get the current resource's info
					ResourceToolsDB.ResourceIndexItem curResInfo = m_ResDB.GetResourceInfo( curResID );

					// Create the node item with the parent set to the current type's root node
					TreeNodeResource newResNode = new TreeNodeResource(curResInfo.sFileName, curResID);
					typeRootNode.Nodes.Add(newResNode);
				}
			}

			// Display the changes
			treeViewResources.EndUpdate();
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Get or set the selected resource ID </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		public int ResID
		{
			get { return m_ResID; }
			set { m_ResID = value; }
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// A message handler called when the user selects a new tree node
		/// </summary>
		///////////////////////////////////////////////////////////////////////////////////////////
		private void treeViewResources_AfterSelect(object sender, TreeViewEventArgs e)
		{
			// If the user selected a resource
			if (e.Node is TreeNodeResource)
			{
				TreeNodeResource selNode = e.Node as TreeNodeResource;
				m_ResID = selNode.resID;
			}
			// Else store a bad value
			else
				m_ResID = -1;
		}

		private void treeViewResources_DoubleClick(object sender, EventArgs e)
		{
			// Close the dialog if the user selected a resource
			if( m_ResID != -1 )
				DialogResult = DialogResult.OK;
		}

	}
}