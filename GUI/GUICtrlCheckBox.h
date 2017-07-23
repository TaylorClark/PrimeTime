//=================================================================================================
/*!
	\file GUICtrlCheckBox.h
	GUI Library
	Graphical User Interface Check Box Control Header
	\author Taylor Clark
	\date September 7, 2006

	This file contains the definition for the GUI check box control class.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlCheckBox_h
#define __GUICtrlCheckBox_h

#include "Graphics2D/RefSprite.h"
#include "GUIControl.h"
#include <string>


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlCheckbox
	\brief The class that represents a button in a GUI layout

	This class defines the GUI button control.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlCheckBox : public GUIControl
{
public:
	/// The callback function prototype for when the checkbox's checked state changes
	typedef void (*OnCheckedChangeCB)( GUICtrlCheckBox* );

private:
 
	/// The image to use when the check box is idle
	RefSpriteHndl m_NormalImage;

	/// The image to use when the mouse is over the button
	RefSpriteHndl m_OverImage;

	/// The image to use for the checked image
	RefSpriteHndl m_CheckMarkImage;

	/// The offset from the control's position to draw the checked image
	Vector2i m_CheckedImageOffset;

	/// Whether or not the mouse is over the button
	bool m_MouseIsOver;

	/// Whether or not the check box is checked
	bool m_IsChecked;

	/// The callback function for when the checked state changes
	OnCheckedChangeCB m_pOnCheckedChangedCB;

	/// Get the bounding box for this control
	virtual Box2i InternalGetBoundBox() const
	{
		// Store the bounding box
		Box2i retBox;
		if( m_MouseIsOver && m_OverImage.GetObj() )
			retBox.size = m_OverImage.GetObj()->GetSrcRect().size;
		else if( m_NormalImage.GetObj() )
			retBox.size = m_NormalImage.GetObj()->GetSrcRect().size;
		retBox.pos = m_Pos;

		// Return the bounding box
		return retBox;
	}

	/// Calculate the check mark sprite offset
	void CalcCheckSpriteOffset();

public:

	/// The default constructor
	GUICtrlCheckBox();

	/// Get the type of GUI control this represents
	virtual GUIDefines::EControlType GetCtrlType()
	{
		return GUIDefines::CT_CheckBox;
	}

	/// Load/save the control data
	virtual void SubclassTransferData( Serializer& serializer );

	/// Set the callback function for when the button is pressed
	void SetCallback( OnCheckedChangeCB pCallbackFunc )
	{
		m_pOnCheckedChangedCB = pCallbackFunc;
	}

	/// Handle a mouse button being pressed
	virtual bool OnMouseDown( GUIDefines::EMouseButtons btn, Point2i mousePos );

	/// Handle the mouse cursor entering this control
	virtual bool OnMouseEnter();

	/// Handle the mouse cursor leaving this control
	virtual bool OnMouseLeave();

	/// Draw the control
	virtual void Draw() const;

	/// Update the control based on its parameters so it displays correctly
	virtual void ReformatControl(){ CalcCheckSpriteOffset(); }

	/// Force a call to the callback function
	void ExecuteCallback()
	{
		if( m_pOnCheckedChangedCB )
			m_pOnCheckedChangedCB( this );
	}

	/// Can the user interact with this control
	virtual bool IsInteractive() const { return true; }

	/// Get the checked state
	bool GetChecked() const { return m_IsChecked; }

	/// Set the checked state
	void SetCheckState( bool isChecked, bool executeCallback = false )
	{
		m_IsChecked = isChecked;

		// If the user wants to call the callback and there is a callback function
		if( executeCallback && m_pOnCheckedChangedCB )
			m_pOnCheckedChangedCB( this );
	}

	/// Set the button data
	void SetData( const wchar_t* szName, RefSpriteHndl upSprite, RefSpriteHndl overSprite, RefSpriteHndl checkSprite )
	{
		m_sName = szName;

		m_NormalImage = upSprite;
		m_OverImage = overSprite;
		m_CheckMarkImage = checkSprite;
	}
};

#endif // __GUICtrlCheckBox_h