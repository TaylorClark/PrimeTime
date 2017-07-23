// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ProgressDlg.h"


// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{

}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_PROGRESS, m_ctlProgressBar);
	DDX_Control(pDX, IDC_PROG_LABEL, m_ctlProgLabel);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
END_MESSAGE_MAP()


// CProgressDlg message handlers
void CProgressDlg::SetProgress( float prog )
{
	int min = 0;
	int max = 0;

	m_ctlProgressBar.GetRange( min, max );

	int val = min + (int)((float)(max - min) * prog);
	m_ctlProgressBar.SetPos( val );
}

void CProgressDlg::SetLabelText( const wchar_t* szText )
{
	m_ctlProgLabel.SetWindowTextW( szText );
}