//=================================================================================================
/*!
	\file GUIControl.cpp
	2D Graphics Engine
	GUI Control Source
	\author Taylor Clark
	\date February 11, 2006

	This source file contains the implementation for the GUI control class.
*/
//=================================================================================================

#include "../GUIControl.h"
#include "../GUICtrlButton.h"
#include "../GUICtrlLabel.h"
#include "../GUICtrlList.h"
#include "../GUICtrlSprite.h"
#include "../GUICtrlTextBox.h"
#include "../GUICtrlScrollList.h"
#include "../GUICtrlCheckBox.h"
#include "GamePlay/GameMgrCtrl.h"

// Initialize the static members
float GUIControl::sm_FrameTime = 0.0f;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIControl::CreateControl()  Static Public
///
///	\param ctrlType The type of control to create
///	\returns A pointer to the created control, which the caller is responsible for freeing, or
///			NULL if the type was unrecognized.
///
///	Create a control based on the type enumeration4.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUIControl* GUIControl::CreateControl( GUIDefines::EControlType ctrlType, GUILayout* pContainingLayout )
{
	GUIControl* pRetCtrl = NULL;

	// Handle the type
	switch( ctrlType )
	{
	case GUIDefines::CT_Button:
		pRetCtrl = new GUICtrlButton();
		break;

	case GUIDefines::CT_Label:
		pRetCtrl = new GUICtrlLabel();
		break;

	case GUIDefines::CT_List:
		pRetCtrl = new GUICtrlList();
		break;

	case GUIDefines::CT_Sprite:
		pRetCtrl = new GUICtrlSprite();
		break;

	case GUIDefines::CT_TextBox:
		pRetCtrl = new GUICtrlTextBox();
		break;

	case GUIDefines::CT_ScrollList:
		pRetCtrl = new GUICtrlScrollList();
		break;

	case GUIDefines::CT_CheckBox:
		pRetCtrl = new GUICtrlCheckBox();
        break;
            
    default:
        // Stop warnings on mac
        break;
	}

	// Store the layout
	if( pRetCtrl )
		pRetCtrl->m_pContainingLayout = pContainingLayout;

	// The type is unknown so return NULL
	return pRetCtrl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIControl::TransferData()  Public
///
///	\param serializer The serializer to load/save the data
///
///	Load or save the control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIControl::TransferData( Serializer& serializer )
{
	// The name
	serializer.AddData( m_sName );

	// The position
	serializer.AddData( m_Pos.x );
	serializer.AddData( m_Pos.y );

	// The static visual bool
	serializer.AddData( m_IsStaticVisual );

	// The subclass data
	SubclassTransferData( serializer );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIControl::GetCtrlTypeName()  Static Public
///
///	\param ctrlType The type of control to create
///	\returns A string describing the control type.
///
///	Get a string describing the type of a control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* GUIControl::GetCtrlTypeName( GUIDefines::EControlType ctrlType )
{
	// Handle the type
	switch( ctrlType )
	{
	case GUIDefines::CT_Button:
		return L"Button";

	case GUIDefines::CT_Label:
		return L"Label";

	case GUIDefines::CT_List:
		return L"List";

	case GUIDefines::CT_Sprite:
		return L"Sprite";

	case GUIDefines::CT_TextBox:
		return L"Text Box";

	case GUIDefines::CT_ScrollList:
		return L"Scrollable List";

	case GUIDefines::CT_CheckBox:
        return L"Check Box";
            
    default:
        // Stop warnings on mac
        break;
	}

	// The type is unknown so return NULL
	return L"UNKNOWN";
}