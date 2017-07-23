// FileProgressDlg.cpp : implementation file
//

#include "PrimeTimeUpdater.h"
#include "FileProgressDlg.h"


const int PROGRESS_RANGE = 200;

// CFileProgressDlg dialog

IMPLEMENT_DYNAMIC(CFileProgressDlg, CDialog)

CFileProgressDlg::CFileProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileProgressDlg::IDD, pParent)
{

}

CFileProgressDlg::~CFileProgressDlg()
{
}

void CFileProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRIMARY_PROGRESS, m_ctlPrimaryProgress);
	DDX_Control(pDX, IDC_SECONDARY_PROGRESS, m_ctlSecondaryProgress);
	DDX_Control(pDX, IDC_PRIMARY_LABEL, m_ctlPrimaryLabel);
	DDX_Control(pDX, IDC_SECONDARY_LABEL, m_ctlSecondaryLabel);
}


BEGIN_MESSAGE_MAP(CFileProgressDlg, CDialog)
END_MESSAGE_MAP()


/// Set the primary text
void CFileProgressDlg::SetPrimaryText( const wchar_t* szText )
{
	m_ctlPrimaryLabel.SetWindowTextW( szText );
}

/// Set the primary progress
void CFileProgressDlg::SetPrimaryProgress( float32 normPercent )
{
	m_ctlPrimaryProgress.SetPos( (int)(normPercent * (float32)PROGRESS_RANGE) );
}

/// Set the secondary text
void CFileProgressDlg::SetSecondaryText( const wchar_t* szText )
{
	m_ctlSecondaryLabel.SetWindowText( szText );
}

/// Set the secondary progress
void CFileProgressDlg::SetSecondaryProgress( float32 normPercent )
{
	m_ctlSecondaryProgress.SetPos( (int)(normPercent * (float32)PROGRESS_RANGE) );
}

BOOL CFileProgressDlg::OnInitDialog()
{
	BOOL retValue = CDialog::OnInitDialog();

	HICON hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SYSTRAY_ICON));
	SetIcon( hIcon, FALSE );

	m_ctlPrimaryProgress.SetRange32( 0, PROGRESS_RANGE );
	m_ctlSecondaryProgress.SetRange32( 0, PROGRESS_RANGE );

	return retValue;
}