//=================================================================================================
/*!
	\file GUILayout.h
	2D Game Engine
	Graphical User Interface Layout Header
	\author Taylor Clark
	\date January 31, 2006

	This file contains the definition for the GUI layout class.
*/
//=================================================================================================

#pragma once
#ifndef __GUILayout_h
#define __GUILayout_h

#include "Math/Point2i.h"
#include <list>
#include "Base/IUpdatable.h"
#include "GUIDefines.h"
#include <string>

#ifdef _DEBUG
#include <fstream>
#endif

class GUIControl;
class GUIMessage;
class DataBlock;
class Serializer;
class GUICtrlSprite;


//-------------------------------------------------------------------------------------------------
/*!
	\class GUILayout
	\brief The class that encapsulates all of a GUI layout's data

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class GUILayout : public IUpdatable
{
public:

	static const wchar_t* BG_CTRL_NAME;

protected:

	/// A control that is expected to exist in a layout
	struct ExpectedCtrl
	{
		/// The default constructor to initialize the data
		ExpectedCtrl() : ppCtrl( 0 ),
			type( GUIDefines::CT_COUNT )
		{}

		GUIControl** ppCtrl;
		std::wstring sCtrlName;
		GUIDefines::EControlType type;
	};

	/// The list of expected controls for this layout
	typedef std::list<ExpectedCtrl> ExpecCtrlList;
	ExpecCtrlList m_ExpectedCtrls;

	/// Add a control to the list of expected controls
	void AddExpectedCtrl( const wchar_t* szName,  GUIDefines::EControlType type )
	{
		// Create a new expected control object
		ExpectedCtrl newExpected;
		newExpected.sCtrlName = szName;
		newExpected.type = type;

		// Store the control
		m_ExpectedCtrls.push_back( newExpected );
	}

	/// Add an expected control and pass the object that is to be hooked up to this control
	template< class T >
	void AddExpectedCtrl( const wchar_t* szName,  GUIDefines::EControlType type, T** ppCtrl )
	{
		// Create a new expected control object
		ExpectedCtrl newExpected;
		newExpected.sCtrlName = szName;
		newExpected.type = type;
		newExpected.ppCtrl = (GUIControl**)ppCtrl;

		// Store the control
		m_ExpectedCtrls.push_back( newExpected );
	}

	/// The control the mouse is currently over
	GUIControl* m_pMouseOverControl;

	/// The control the mouse was over when the mouse was pressed a mouse button down
	GUIControl* m_pMouseDownControl[GUIDefines::MB_COUNT];

	/// The control with focus
	GUIControl* m_pFocusCtrl;

	/// The list of controls in this layout sorted by Z depth, with the back-most control at the
	/// front of the list
	typedef std::list< GUIControl* > ControlList;
	ControlList m_FullControlList;

	/// The list of non-static controls that need updating and rendering
	ControlList m_Controls;

	/// A helper function to get a control itereator
	ControlList::iterator GetCtrlIter( GUIControl* pCtrl, bool useFullList = false );

	/// The unique layout ID
	uint32 m_LayoutID;

	/// The background sprite, NULL if there is no special background control
	GUICtrlSprite* m_pBGSprite;


	/// Get the background sprite if it is a valid target for drawing
	GUICtrlSprite* GetDrawableBG();

public:
	/// The default constructor
	GUILayout() : m_pFocusCtrl( 0 ),
					m_pMouseOverControl( 0 ),
					m_LayoutID( 0 ),
					m_pBGSprite( 0 )
	{
		for( int mouseButtonIndex = 0; mouseButtonIndex < GUIDefines::MB_COUNT; ++mouseButtonIndex )
			m_pMouseDownControl[mouseButtonIndex] = 0;
	}

	/// The destructor
	virtual ~GUILayout()
	{
		Clear();
	}

	/// Link the control objects to their corresponding object found in the layout
	virtual void HookupCtrls();

	/// Handle a GUI message
	virtual bool HandleMessage( GUIMessage* pMsg );

	/// Get the control at a position
	GUIControl* GetControlAtPoint( const Point2i& testPt );

	/// Update the mouse enter/leave messages
	void UpdateMouseEnterLeaveMsgs();

	/// Clear the data held by the layout
	void Clear();

	/// Update the layout
	virtual void Update( float32 frameTime );

	/// Draw the layout
	virtual void Draw() const;

	/// Draw the static items to the background
	void DrawStaticsToBackground();

	/// Undo any background changes necessary
	void UndoBackgroundMods();

	/// Determine if a windows key code is an arrow key
	static bool IsArrowKey( unsigned int winKey );

	/// Save/load a layour
	void TransferLayout( Serializer& serializer );

	/// Get a control by name
	GUIControl* GetCtrlByName( const wchar_t* szCtrlName );

	/// Get a control by name (constant)
	const GUIControl* GetCtrlByName( const wchar_t* szCtrlName ) const;

	/// Get a unique layout ID
	virtual uint32 GetLayoutID(){ return m_LayoutID; }

	/// Get the focus control
	GUIControl* GetFocusCtrl() { return m_pFocusCtrl; }

	/// Get the focus control
	void SetFocusCtrl( GUIControl* pCtrl ) { m_pFocusCtrl = pCtrl; }

	/// Add a control to the layout
	void AddControl( GUIControl* pNewCtrl );

	/// Remove a control from the layout, but do not free the memory
	bool RemoveControl( GUIControl* pCtrl );

	/// Get all the controls of a type in a layout
	ControlList GetAllControlsOfType( GUIDefines::EControlType controlType );

	/// A message handler called when the layout is activated
	virtual void OnActivate();

	/// A message handler called when the layout is deactivated
	virtual void OnDeactivate() {}

	/// Center the control horizontally
	void CenterCtrlHorz( GUIControl* pCtrl );

	/// Center the control vertically
	void CenterCtrlVert( GUIControl* pCtrl );

	/// Move a control to the front of the layout, in front of the rest of the controls
	void BringCtrlToFront( GUIControl* pCtrl );

#ifdef _DEBUG
	/// Move a control to the back of the layout, behind the rest of the controls
	void SendCtrlToBack( GUIControl* pCtrl );

	/// Move a control back in the Z order
	void MoveCtrlBack( GUIControl* pCtrl );

	/// Move a control forward in the Z order
	void MoveCtrlForward( GUIControl* pCtrl );

	/// Remove a control from the layout and free the memory
	void DeleteControl( GUIControl* pCtrl );

#endif // _DEBUG
};

#endif // __GUILayout_h