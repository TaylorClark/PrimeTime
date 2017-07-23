// ../Source/ModListDlg.cpp : implementation file
//

#include "../ModListDlg.h"


// CModListDlg dialog

IMPLEMENT_DYNAMIC(CModListDlg, CDialog)

CModListDlg::CModListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModListDlg::IDD, pParent)
{

}

CModListDlg::~CModListDlg()
{
}

void CModListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VALUEEDIT, m_ctlNewValueEdit);
	DDX_Control(pDX, IDC_VALUELIST, m_ctlStrList);
}


BEGIN_MESSAGE_MAP(CModListDlg, CDialog)
	ON_BN_CLICKED(IDC_ADDBUTTON, &CModListDlg::OnBnClickedAddbutton)
END_MESSAGE_MAP()


// CModListDlg message handlers

void CModListDlg::OnBnClickedAddbutton()
{
	// Get the text from the edit box
	CString newValueText;
	m_ctlNewValueEdit.GetWindowTextW( newValueText );

	// If it is empty then bail
	if( newValueText.GetLength() < 1 )
		return;

	// Add the string to the list
	m_ctlStrList.AddString( (LPCTSTR)newValueText );
}
