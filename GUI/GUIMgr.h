//=================================================================================================
/*!
	\file GUIMgr.h
	GUI Library
	Graphical User Interface Manager Header
	\author Taylor Clark
	\date January 31, 2006

	This file contains the definition for the GUI manager class.
*/
//=================================================================================================

#pragma once
#ifndef __GUIMgr_h
#define __GUIMgr_h

#include "Math/Point2i.h"
#include "GUIDefines.h"
#include "Base/IUpdatable.h"
#include "Base/FourCC.h"
#include <list>
#include <map>

class GUILayout;
class GUIMessage;
class GUIControl;
class Serializer;
class MsgBox;


//-------------------------------------------------------------------------------------------------
/*!
	\class GUIMgr
	\brief The singleton class to manage user interface actions

	This class defines the manager for handling user interface loading, messages, and transitions.
	It uses the singleton pattern so that there is one and only instance of the class.
*/
//-------------------------------------------------------------------------------------------------
class GUIMgr : public IUpdatable
{
public:
	struct CompressedLayout
	{
		/// The layout to populate when needed
		GUILayout* pLayout;

		/// The size of the data
		uint32 dataSize;

		/// The layout data
		uint8* pLayoutData;
	};
private:

	/// Whether or not the GUI is active
	bool m_IsActive;

	/// The current layout, NULL if there is none
	GUILayout* m_pCurLayout;

	/// The current message box, NULL if there is none
	MsgBox* m_pCurMsgBox;

	/// The layout to transition to
	GUILayout* m_pPendingActiveLayout;

	/// The mutex for the GUI message list
	void* m_hMsgListMutex;

	/// The cursor position
	Point2i m_CursorPos;

	/// The list of messages that happened last frame
	std::list< GUIMessage* > m_GUIMessageList;

	/// The map of layouts
	typedef std::map< uint32, GUILayout* > LayoutMap;
	LayoutMap m_LayoutMap;

	/// The map of compressed layouts
	typedef std::map< uint32, CompressedLayout > CompressedLayoutMap;
	CompressedLayoutMap m_CompLayoutMap;

	/// The default constructor, private since the class is a singleton
	GUIMgr() : m_pCurLayout( 0 ),
				m_IsActive( true ),
				m_hMsgListMutex( 0 ),
				m_pCurMsgBox( 0 ),
				m_pPendingActiveLayout( 0 )
	{
	}

	/// Add a message to the list
	void AddNewGUIMsg( GUIMessage* pMsg );

public:

	/// The file FourCC key for the GUI resource file
	static const FourCC FOURCCKEY_GUI;

	/// Get the one and only GUI manager instance
	static GUIMgr& Get()
	{
		static GUIMgr s_Mgr;
		return s_Mgr;
	}

	/// The destructor
	~GUIMgr(){ Term(); }

	/// Clear the data in the GUI manager
	void ClearLayouts();

	/// Initialize the GUI manager
	bool Init();

	/// Free the resources need by the GUI manager
	void Term();

	/// Draw the GUI
	virtual void Draw() const;

	/// Update the GUI
	virtual void Update( float32 frameTime );

	/// Activate the GUI manager
	//void Activate(){ m_IsActive = true; }

	/// Turn off the GUI
	void Deactivate();

	/// Get the active layout
	GUILayout* GetCurrentLayout() { return m_pCurLayout; }

	/// Test if a key is down
	//bool IsKeyDown( unsigned int windowsKeyCode );
	bool IsShiftDown();

	/// Handle a key being pressed down
	void OnKeyDown( unsigned int windowsKeyCode );
	
	/// Handle a key being released
	void OnKeyUp( unsigned int windowsKeyCode );

	/// Handle a keyboard character pressed and released
	void OnKeyChar( wchar_t charKey );

	/// Handle a mouse button being pressed
	void OnMouseDown( GUIDefines::EMouseButtons mouseButton );

	/// Handle a mouse button being released
	void OnMouseUp( GUIDefines::EMouseButtons mouseButton );

	/// Get if a mouse button is currently pressed down
	bool IsMouseDown( GUIDefines::EMouseButtons mouseButton );

	/// Set the active layout
	void SetActiveLayout( uint32 layoutID );

	/// Display a message box
	bool PopupMessageBox( MsgBox* pMsgBox );

	/// Close the specified message box
	void CloseMessageBox( MsgBox* pMsgBox );

	/// Add a layout
	void AddLayout( GUILayout* pNewLayout );

	/// Add a compressed layout
	void AddCompressedLayout( const CompressedLayout& layout ); 

	/// Get the number of layouts
	uint32 GetNumLayouts() const { return (uint32)m_LayoutMap.size() + (uint32)m_CompLayoutMap.size(); }

	/// Get the position of the cursor in client coordinates
	Point2i GetCursorPos(){ return m_CursorPos; }

	/// Set the mouse position
	void SetCursorPos( Point2i cursorPos ){ m_CursorPos = cursorPos; }

	/// Get a layout from ID
	GUILayout* GetLayoutByID( uint32 layoutID );

#ifdef _DEBUG

	// The handle to the menu that is used to add controls to a layout
	void* m_ToolsPopupMenu;

	void PopupToolsContextMenu( const Point2i& mousePos );

	bool HandleGUIMessage( GUIMessage* pMsg );

	void SaveLayouts( Serializer& writer );

#endif	
};

#endif // __GUIMgr_h