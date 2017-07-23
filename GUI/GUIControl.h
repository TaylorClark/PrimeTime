//=================================================================================================
/*!
	\file GUILayout.h
	2D Game Engine
	Graphical User Interface Control Base Header
	\author Taylor Clark
	\date January 31, 2006

	This file contains the definition for the GUI layout class.
*/
//=================================================================================================

#pragma once
#ifndef __GUIControl_h
#define __GUIControl_h

#include "Math/Point2i.h"
#include "Math/Box2i.h"
#include "GUIDefines.h"
#include <list>
#include <string>
#include "Base/Serializer.h"
#include "Base/IUpdatable.h"

#ifdef _DEBUG
#include "GUICtrlProperties.h"
#endif

// Forward declarations
class GUICtrlProperty;
class GUILayout;


// Ignore the warning for unreferenced formal parameters since they are helpful in the prototypes
// for this class
#pragma warning( push )
#pragma warning( disable : 4100 )

//-------------------------------------------------------------------------------------------------
/*!
	\class GUIControl
	\brief The class that encapsulates all of a GUI layout's data

	This class defines the GUI layout class
*/
//-------------------------------------------------------------------------------------------------
class GUIControl
{
protected:

	/// The position of the top-left of the control in screen coordinates
	Point2i m_Pos;

	/// The name of the control
	std::wstring m_sName;

	/// The layout that contains this control
	GUILayout* m_pContainingLayout;

	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const = 0;

	/// Load/save the sub-class control data
	virtual void SubclassTransferData( Serializer& serializer ){};

	/// The current frame time
	static float sm_FrameTime;

	/// If the control is drawn
	bool m_IsVisible;

	/// If the control is static, meaning for the duration that a layout is active its graphics
	/// will not change
	bool m_IsStaticVisual;

public:

	// The constructor is in a #ifndef since the debug version is below
#ifndef _DEBUG
	/// The default constructor
	GUIControl() : m_pContainingLayout( 0 ),
					m_IsVisible( true ),
					m_IsStaticVisual( false )
	{}

	/// The destructor
	virtual ~GUIControl() {}
#endif

	/// Handle a key being pressed down
	virtual bool OnKeyDown( unsigned int windowsKeyCode ){ return false; }
	
	/// Handle a key being released
	virtual bool OnKeyUp( unsigned int windowsKeyCode ){ return false; }

	/// Handle a keyboard character pressed and released
	virtual bool OnKeyChar( wchar_t charKey ){ return false; }

	/// Handle a mouse button being pressed
	virtual bool OnMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos ){ return false; }

	/// Handle a mouse button being released
	virtual bool OnMouseUp( GUIDefines::EMouseButtons btn, Point2i mousePos ){ return false; }

	/// Handle a mouse button being released after a mouse down message, but the cursor not over
	/// the control
	virtual bool OnMouseUpNotOver( GUIDefines::EMouseButtons btn, Point2i mousePos ){ return false; }

	/// Handle the mouse cursor entering this control
	virtual bool OnMouseEnter(){ return false; }

	/// Handle the mouse cursor leaving this control
	virtual bool OnMouseLeave(){ return false; }

	/// Draw the control
	virtual void Draw() const = 0;

	/// Can the user interact with this control
	virtual bool IsInteractive() const = 0;

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType() = 0;

	/// Update the control
	virtual void Update( float32 ) {}

	/// Create a control based on the type enumeration
	static GUIControl* CreateControl( GUIDefines::EControlType ctrlType, GUILayout* pContainingLayout );

	/// Get a string describing a type of control
	static const wchar_t* GetCtrlTypeName( GUIDefines::EControlType ctrlType );

	/// Set the frame time for controls to reference
	static void SetFrameTime( float frameTime ){ sm_FrameTime = frameTime; }
	
	/// Update the control based on its parameters so it displays correctly
	virtual void ReformatControl(){}

	/// Get the bounding box for this control
	Box2i GetBoundBox() const
	{
		// Get the calculated bounding box
		Box2i retBox = InternalGetBoundBox();

		// Make sure the box is not too small so that we can't click on it
		if( retBox.size.x < 20 )
			retBox.size.x = 20;
		if( retBox.size.y < 20 )
			retBox.size.y = 20;

		// Return the box
		return retBox;
	}

	/// Load/save the control
	void TransferData( Serializer& serializer );

	/// Get if this control can receive focus
	virtual bool CanReceiveFocus() const { return false; }

	/// Get the control name
	const std::wstring& GetName() const { return m_sName; }

	/// Get the containing layout
	GUILayout* GetContainingLayout() const { return m_pContainingLayout; }

	/// Set the containing layout
	void SetContainingLayout( GUILayout* pLayout ){ m_pContainingLayout = pLayout; }

	/// Set the position
	virtual void SetPos( const Point2i& pos ){ m_Pos = pos; }

	/// Get the position
	const Point2i& GetPos() const { return m_Pos; }

	/// Set the visibility
	void SetVisible( bool isVisible ) { m_IsVisible = isVisible; }

	/// Get the visibility
	bool IsVisible() const { return m_IsVisible; }

	/// Get if the control is visibly static
	bool IsStaticVisual() const { return m_IsStaticVisual; }

#ifdef _DEBUG

public:

	std::list< GUICtrlProperty* > m_PropList;

	GUIControl() : m_pContainingLayout( 0 ),
					m_IsVisible( true ),
					m_IsStaticVisual( false )
	{
		GUICtrlProperty* pNewProp = new GUICtrlPropString( L"Name", &m_sName );
		m_PropList.push_back( pNewProp );

		pNewProp = new GUICtrlPropInt( L"X", &m_Pos.x );
		m_PropList.push_back( pNewProp );

		pNewProp = new GUICtrlPropInt( L"Y", &m_Pos.y );
		m_PropList.push_back( pNewProp );

		pNewProp = new GUICtrlPropBool( L"Static", &m_IsStaticVisual );
		m_PropList.push_back( pNewProp );
	}

	virtual ~GUIControl()
	{
		for( std::list< GUICtrlProperty* >::iterator iterProp = m_PropList.begin();
				iterProp != m_PropList.end();
				++iterProp )
		{
			if( *iterProp )
				delete *iterProp;
		}
		m_PropList.clear();
	}
#endif
};

#pragma warning( pop )

#endif // __GUIControl_h