// ResultsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrimeTimeUpdater.h"
#include "ResultsDlg.h"


// CResultsDlg dialog

IMPLEMENT_DYNAMIC(CResultsDlg, CDialog)

CResultsDlg::CResultsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResultsDlg::IDD, pParent)
{

}

CResultsDlg::~CResultsDlg()
{
}

void CResultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResultsDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()



///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CResultsDlg::AppendNewLineResultText  Public
///
///	\param sText The string to append to the text
///
///	Concatenate more text to the result text.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResultsDlg::AppendNewLineResultText( CString sText )
{
	// Add the text on
	if( m_ResultsText.GetLength() > 0 )
		m_ResultsText += L"\r\n";
	m_ResultsText += sText;
}

void CResultsDlg::SetMultilineResultText( const wchar_t* szText )
{
	// Get the string length and validate the string
	if( !szText || wcslen( szText ) == 0 )
		return;

	// Go through the string and find new-line characters
	CString sText( szText );
	int curCharPos = 0;
	CString sCurLine = sText.Tokenize( L"\n", curCharPos );
	while( curCharPos != -1 )
	{
		// Display the line of text
		AppendNewLineResultText( sCurLine );

		// Advance past this line and the new-line character
		sCurLine = sText.Tokenize( L"\n", curCharPos );
	}
}


BOOL CResultsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SYSTRAY_ICON));
	SetIcon( hIcon, FALSE );

	// Get the size of the dialog
	RECT winRect;
	GetClientRect( &winRect );

	// Get the size of the editbox
	RECT ctlRect;
	CWnd* pCtl = GetDlgItem( IDC_INFO_EDIT );
	if( pCtl )
	{
		pCtl->GetClientRect( &ctlRect );

		// Store the difference
		m_EditBoxSizeOffset.x = winRect.right - ctlRect.right;
		m_EditBoxSizeOffset.y = winRect.bottom - ctlRect.bottom;
	}	

	// Store the offset of the buttons from the bottom of the dialog
	pCtl = GetDlgItem( IDOK );
	if( pCtl )
	{
		// Get the top of the button in the dialog's client coordinates
		POINT testPt;
		testPt.y = 0;
		pCtl->ClientToScreen( &testPt );
		ScreenToClient( &testPt );

		m_ButtonVertOffset = winRect.bottom - testPt.y;
	}

	// Set the display text
	SetDlgItemText( IDC_INFO_EDIT, m_ResultsText );

	return TRUE;
}

// CResultsDlg message handlers

void CResultsDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// Resize the text box
	CWnd* pCtl = GetDlgItem( IDC_INFO_EDIT );
	if( pCtl )
		pCtl->SetWindowPos( NULL, 0, 0, cx - m_EditBoxSizeOffset.x, cy - m_EditBoxSizeOffset.y, SWP_NOMOVE );

	// Move the OK and cancel buttons (2 is hardcoded due to the horizontal positioning)
	//UINT ctlIDs[2] = { IDOK, IDCANCEL };
	//for( int ctlIndex = 0; ctlIndex < 2; ++ctlIndex )
	//{
		pCtl = GetDlgItem( IDOK );
		if( pCtl )
		{
			// Get the client rectangle
			RECT ctlRect;
			pCtl->GetClientRect( &ctlRect );
			pCtl->ClientToScreen( &ctlRect );
			ScreenToClient( &ctlRect );

			int ctlHalfWidth = (ctlRect.right - ctlRect.left) / 2;
			int ctlLeft = (cx / 2) - ctlHalfWidth;
			
			// Set the button position
			pCtl->SetWindowPos( NULL, ctlLeft, cy - m_ButtonVertOffset, 0, 0, SWP_NOSIZE );
		}
	//}
}

void CResultsDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// Set the info
	if( lpMMI )
	{
		lpMMI->ptMinTrackSize.x = 250;
		lpMMI->ptMinTrackSize.y = 275;
	}

	CDialog::OnGetMinMaxInfo(lpMMI);
}
