//=================================================================================================
/*!
	\file GUIEditDlg.h
	2D Game Engine
	Graphical User Interface Edit Dialog Header
	\author Taylor Clark
	\date February 6, 2006

	This file contains the definition for the GUI edit dialog class.
*/
//=================================================================================================

#pragma once
#ifndef __GUIEditDlg_h
#define __GUIEditDlg_h

#include "Base/Types.h"
#include <windows.h>
#include <vector>
#include <string>
#include "GUI/GUIListItem.h"

#ifndef _DEBUG
#error This file must only be included in debug
#endif

// Forward declarations
class GUIControl;
class GUICtrlProperty;

typedef std::vector< std::wstring > StringList;


//-------------------------------------------------------------------------------------------------
/*!
	\class GUIEditDlg
	\brief The GUI editor dialog
*/
//-------------------------------------------------------------------------------------------------
class GUIEditDlg
{
private:

	/// The selected property
	GUICtrlProperty* m_pActiveProp;

	/// The index of the selected property in the list box
	int m_SelPropIndex;

	/// The selected control
	GUIControl* m_pSelCtrl;

	/// The default constructor
	GUIEditDlg() : m_HWnd( NULL ),
					m_pActiveProp( NULL ),
					m_pSelCtrl( NULL ),
					m_SelPropIndex( -1 )
	{
	}

public:

	HWND m_HWnd;

	static GUIEditDlg& Get()
	{
		static GUIEditDlg s_Dlg;
		return s_Dlg;
	}

	void PumpMessages();

	void Init( HWND pParentHWnd );

	void Close();

	void SetReferenceControl( GUIControl* pCtrl );

	/// Get the currently selected control
	GUIControl* GetReferenceControl(){ return m_pSelCtrl; }

	/// Show the dialog
	void Show();

	/// Hide the dialog
	void Hide();

	bool IsVisible() const
	{
		return IsWindowVisible( m_HWnd ) == TRUE;
	}

	void OnListItemSel();

	void OnTextChange( const wchar_t* szText );
};

/// Display the dialog
bool DoListEditDlgModal( HWND pParentHWnd, std::vector<GUIListItem>* pStringList );

#endif // __GUIEditDlg_h