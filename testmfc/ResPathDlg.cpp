// ResPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResPathDlg.h"
#include "Base/DirDialog.h"
#include "Base/StringFuncs.h"


// CResPathDlg dialog

IMPLEMENT_DYNAMIC(CResPathDlg, CDialog)

CResPathDlg::CResPathDlg( const wchar_t *szResPath, const wchar_t *szGamePath )
	: CDialog(CResPathDlg::IDD, NULL)
{
	// Store the paths
	m_sResPath = szResPath;
	m_sGamePath = szGamePath;
}

CResPathDlg::~CResPathDlg()
{
}

void CResPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESPATH_EDIT, m_ctlPathRes);
	DDX_Control(pDX, IDC_GAMEPATH_EDIT, m_ctlPathGame);
}


BEGIN_MESSAGE_MAP(CResPathDlg, CDialog)
	ON_BN_CLICKED(ID_BUTTONOK, &CResPathDlg::OnBnClickedButtonok)
	ON_BN_CLICKED(ID_BUTTONCANCEL, &CResPathDlg::OnBnClickedButtoncancel)
	ON_BN_CLICKED(IDC_BUTTON_RESPATH, &CResPathDlg::OnBnClickedButtonRespath)
	ON_BN_CLICKED(IDC_BUTTON_GAMEPATH, &CResPathDlg::OnBnClickedButtonGamepath)
END_MESSAGE_MAP()


// CResPathDlg message handlers

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResPathDlg::OnBnClickedButtonok  Public
///
///	A message handler called when the user clicks the OK button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResPathDlg::OnBnClickedButtonok()
{
	// Store the paths
	CString sResPath;
	m_ctlPathRes.GetWindowTextW( sResPath );
	m_sResPath = sResPath;

	CString sGamePath;
	m_ctlPathGame.GetWindowTextW( sGamePath );
	m_sGamePath = sGamePath;

	// Ensure trailing slashes
	TCBase::EnsureValidPathSlashes( m_sResPath );
	TCBase::EnsureValidPathSlashes( m_sGamePath );

	// We are done
	EndDialog( IDOK );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResPathDlg::OnBnClickedButtoncancel  Public
///
///	A message handler called when the user clicks the cancel button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResPathDlg::OnBnClickedButtoncancel()
{
	// End the dialog
	EndDialog( IDCANCEL );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResPathDlg::OnInitDialog  Protected
///
///	A message handler called when the dialog is initialized.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CResPathDlg::OnInitDialog()
{
	// Initialize the base class
	CDialog::OnInitDialog();

	// Set the window text
	m_ctlPathRes.SetWindowTextW( m_sResPath.c_str() );
	m_ctlPathGame.SetWindowTextW( m_sGamePath.c_str() );

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResPathDlg::OnBnClickedButtonRespath  Public
///
///	A message handler called when the user clicks on the resource path directory chooser button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResPathDlg::OnBnClickedButtonRespath()
{
	// Bring up a directory chooser
	CBrowseDialog dirDlg;
	if( !dirDlg.DoBrowse() )
		return;

	// Store the path
	m_ctlPathRes.SetWindowTextW( dirDlg.GetSelDir().c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResPathDlg::OnBnClickedButtonGamepath  Public
///
///	A message handler called when the user clicks on the game path directory chooser button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResPathDlg::OnBnClickedButtonGamepath()
{
	// Bring up a directory chooser
	CBrowseDialog dirDlg;
	if( !dirDlg.DoBrowse() )
		return;

	// Store the path
	m_ctlPathGame.SetWindowTextW( dirDlg.GetSelDir().c_str() );
}
