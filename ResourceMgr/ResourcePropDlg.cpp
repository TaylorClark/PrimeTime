// ResourcePropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResourcePropDlg.h"
#include "Base/StringFuncs.h"
#include "Base/FileFuncs.h"

// CResourcePropDlg dialog

IMPLEMENT_DYNAMIC(CResourcePropDlg, CDialog)

CResourcePropDlg::CResourcePropDlg(ResourceToolsDB::ToolsResourceInfo& resInfo, const std::wstring& sResRootPath, CWnd* pParent /*=NULL*/)
	: m_ResInfo( resInfo ),
		m_sResRootPath( sResRootPath ),
		CDialog(CResourcePropDlg::IDD, pParent)
{

}

CResourcePropDlg::~CResourcePropDlg()
{
}

void CResourcePropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESPATH_EDIT, m_ctlFullPathText);
	DDX_Control(pDX, IDC_NAME_EDIT, m_ctlNameText);
	DDX_Control(pDX, IDC_RESID_EDIT, m_ctlResIDText);
	DDX_Control(pDX, IDC_IDDEF_EDIT, m_ctlIDDefEdit);
	DDX_Control(pDX, IDC_BADPATH_STATIC, m_ctlBadPathStatic);
	DDX_Control(pDX, IDC_RLEBITMAP_CHECK, m_ctlPackRLECheck);
}


BEGIN_MESSAGE_MAP(CResourcePropDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CResourcePropDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_RESPATH_EDIT, &CResourcePropDlg::OnEnChangeRespathEdit)
END_MESSAGE_MAP()


// CResourcePropDlg message handlers

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnInitDialog  Protected
///
///	Called when the dialog is initialized.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CResourcePropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Get the full resource path
	std::wstring sResPath = TCBase::CombinePaths( m_sResRootPath, m_ResInfo.sFilePath );

	// Set the path text
	m_ctlFullPathText.SetWindowTextW( sResPath.c_str() );

	// If the resource is a sprite or font then don't allow name changing
	if( m_ResInfo.resType == RT_Sprite )
	{
		m_ctlNameText.SetWindowTextW( _T("Name is set in the Sprite Editor tool." ) );
		m_ctlNameText.SetReadOnly( TRUE );
	}
	// If the resource is a font
	else if( m_ResInfo.resType == RT_Font )
	{
		m_ctlNameText.SetWindowTextW( _T("Name is set in the Font Editor tool." ) );
		m_ctlNameText.SetReadOnly( TRUE );
	}
	// Else allow name setting
	else
	{
		m_ctlNameText.SetWindowTextW( m_ResInfo.sName.c_str() );
		m_ctlNameText.SetLimitText( ResourceMgr::RES_IDX_NAME_LEN - 1 );
	}

	// If the image is a bitmap
	if( m_ResInfo.resType == RT_Image && (m_ResInfo.imgResType == IRT_Bitmap || m_ResInfo.imgResType == IRT_BitmapRLE) )
	{
		if( m_ResInfo.imgResType == IRT_BitmapRLE )
			m_ctlPackRLECheck.SetCheck( TRUE );
		else
			m_ctlPackRLECheck.SetCheck( FALSE );

		m_ctlPackRLECheck.ShowWindow( SW_SHOW );
	}
	else
		m_ctlPackRLECheck.ShowWindow( SW_HIDE );

	// Set the ID define
	m_ctlIDDefEdit.SetWindowTextW( m_ResInfo.sResHeaderDefine.c_str() );

	// Set the resource ID text
	CString sResID;
	sResID.Format( _T("%u"), m_ResInfo.resID );
	m_ctlResIDText.SetWindowTextW( sResID );

	m_ctlNameText.SetFocus();
	m_ctlNameText.SetSel( m_ctlNameText.GetWindowTextLength(), m_ctlNameText.GetWindowTextLength() );

	// Return TRUE unless you set the focus to a control
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnBnClickedOk  Protected
///
///	A message handler called when the user hits the OK button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourcePropDlg::OnBnClickedOk()
{
	// Retrieve the path from the text box
	CString sPath;
	m_ctlFullPathText.GetWindowTextW( sPath );

	// Store the resource path in its relative form
	m_ResInfo.sFilePath = TCBase::MakePathRelative( m_sResRootPath.c_str(), (LPCTSTR)sPath );
	TCBase::EnsureValidPathSlashes( m_ResInfo.sFilePath );

	// If the resource is not a sprite or font
	if( m_ResInfo.resType != RT_Sprite && m_ResInfo.resType != RT_Font )
	{
		// Store the name
		CString sName;
		m_ctlNameText.GetWindowTextW( sName );
		m_ResInfo.sName = sName;
	}

	// Get the ID define text
	CString sIDDef;
	m_ctlIDDefEdit.GetWindowTextW( sIDDef );
	m_ResInfo.sResHeaderDefine = sIDDef;
	
	// Get the resource ID text
	CString sResID;
	m_ctlResIDText.GetWindowTextW( sResID );
	ResourceID newResID = (ResourceID)_wtoi( (LPCTSTR)sResID );
	if( newResID != m_ResInfo.resID )
	{
		int msgRet = MessageBox( _T("Are you sure you want to change the resource ID?"), _T("Resource ID Change Confirmation"), MB_YESNO | MB_ICONQUESTION );
		if( msgRet == IDYES )
			m_ResInfo.resID = newResID;
	}

	// If the image is a bitmap
	if( m_ctlPackRLECheck.IsWindowVisible() )
	{
		if( m_ctlPackRLECheck.GetCheck() )
			m_ResInfo.imgResType = IRT_BitmapRLE;
		else
			m_ResInfo.imgResType = IRT_Bitmap;
	}

	// Complete the dialog
	OnOK();
}

void CResourcePropDlg::CheckPathForValidity()
{
	CString sRelativeResPath;
	m_ctlFullPathText.GetWindowText( sRelativeResPath );

	std::wstring sFullResPath = TCBase::CombinePaths( m_sResRootPath, (LPCTSTR)sRelativeResPath );

	if( TCBase::DoesFileExist( sFullResPath.c_str() ) )
		m_ctlBadPathStatic.ShowWindow( SW_HIDE );
	else
		m_ctlBadPathStatic.ShowWindow( SW_SHOW );
}

HBRUSH CResourcePropDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID() == IDC_BADPATH_STATIC )
		pDC->SetTextColor( RGB(255,0,0) );

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CResourcePropDlg::OnEnChangeRespathEdit()
{
	CheckPathForValidity();
}
