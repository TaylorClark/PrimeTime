// SysTrayIconDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrimeTimeUpdater.h"
#include "SysTrayIconDlg.h"


const UINT WM_NOTIFYICON_EVT = WM_USER + 5;

// CSysTrayIconDlg dialog

IMPLEMENT_DYNAMIC(CSysTrayIconDlg, CDialog)

CSysTrayIconDlg::CSysTrayIconDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSysTrayIconDlg::IDD, pParent)
{

}

CSysTrayIconDlg::~CSysTrayIconDlg()
{
	// Remove the icon
	Shell_NotifyIcon( NIM_DELETE, &m_NotifyIcon );
}

void CSysTrayIconDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSysTrayIconDlg, CDialog)
	ON_MESSAGE(WM_NOTIFYICON_EVT, OnNotifyIcon)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSysTrayIconDlg message handlers

BOOL CSysTrayIconDlg::OnInitDialog()
{
	ShowWindow( SW_HIDE );

	CDialog::OnInitDialog();

	// Iniitialize the notify icon data structure
	m_NotifyIcon.cbSize = sizeof(m_NotifyIcon);
	m_NotifyIcon.dwInfoFlags = NIIF_NONE;
	m_NotifyIcon.dwState = 0;
	m_NotifyIcon.dwStateMask = 0;
	m_NotifyIcon.hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SYSTRAY_ICON) );
	m_NotifyIcon.hWnd = m_hWnd;
	swprintf_s( m_NotifyIcon.szInfo, L"NotifiyIcon" );
	m_NotifyIcon.szInfoTitle[0] = 0;
	m_NotifyIcon.szTip[0] = 0;
	m_NotifyIcon.uCallbackMessage = WM_NOTIFYICON_EVT;
	m_NotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE;
	m_NotifyIcon.uID = 0;
	m_NotifyIcon.uTimeout = 0;
	m_NotifyIcon.uVersion = NOTIFYICON_VERSION;

	Shell_NotifyIcon( NIM_ADD, &m_NotifyIcon );

	return 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CPrimeTimeUpdaterApp::OnNotifyIcon  Protected
///
///	A message handler to handle any message sent by the notification icon.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CSysTrayIconDlg::OnNotifyIcon( WPARAM wParam, LPARAM lParam )
{
	// The lParam stores the message ID, handle mouse down messages
	if( lParam == WM_LBUTTONDOWN || lParam == WM_RBUTTONDOWN )
	{
		int x = 3;
		x += 5;
		// Display the window, the OnSize function will handle removal of the notify icon
		//ShowWindow( SW_RESTORE );
	}

	return 0;
}