// testmfc.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "testmfc.h"
#include "Resource/Resource.h"
#include "ResChooserDlg.h"
//#include "ResPathDlg.h"
#include "Base/NumFuncs.h"
#include "Base/RegKeyObj.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CtestmfcApp

BEGIN_MESSAGE_MAP(CtestmfcApp, CWinApp)
END_MESSAGE_MAP()


// CtestmfcApp construction

CtestmfcApp::CtestmfcApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CtestmfcApp object

CtestmfcApp theApp;


// CtestmfcApp initialization

BOOL CtestmfcApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


bool RunResChooserDlg_Filter( HWND hWnd, unsigned int* pResID, EResourceType typeFilter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	if( !pResID )
		return false;
	
	// Display the window and handle if the user made changes
	CWnd* pWnd = CWnd::FromHandle( hWnd );
	CResChooserDlg resChooserDlg( pWnd );
	resChooserDlg.SetFilter( typeFilter );
	//HINSTANCE hDlg = AfxFindResourceHandle( MAKEINTRESOURCE(IDD_DIALOG_RESCHOOSER), RT_DIALOG );
	//resChooserDlg.Create( IDD_DIALOG_RESCHOOSER, NULL );
	int dlgRetVal = (int)resChooserDlg.DoModal();
	if( dlgRetVal == -1 )
	{
		std::wstring sMsg(L"Returned -1:");
		sMsg += TCBase::EasyIToA( GetLastError() );
		MessageBox( NULL, sMsg.c_str(), L"aewhkt", MB_OK );
	}

	bool pressedOK = (dlgRetVal == IDOK);

	// If the user hit OK then store the paths
	bool retVal = false;
	if( pressedOK )
	{
		// Store the resource ID
		*pResID = resChooserDlg.GetSelectedResourceID();
		
		retVal = true;
	}

	// The user did not hit OK
	pWnd->Detach();
	return retVal;
}

//bool RunResDlg(void)
//{
//	// Ensure we are using the DLL's resources
//	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
//	//HINSTANCE hPrevResHandle = AfxGetResourceHandle();
//	//AfxSetResourceHandle( AfxGetInstanceHandle() );
//
//	// Initialize the registry key object
//	RegKeyObj regKey;
//	regKey.Init( RegKeyObj::RK_LocalMachine, ResourceToolsDB::REGPATH_RootKeyPath );
//
//	// Get the resource path
//	std::wstring sResPath;
//	regKey.GetValue( ResourceToolsDB::REGPATH_ResPath, &sResPath );
//
//	// Get the game path
//	std::wstring sGamePath;
//	regKey.GetValue( ResourceToolsDB::REGPATH_GamePath, &sGamePath );
//
//	// Display the window and handle if the user made changes
//	CResPathDlg resDlg( sResPath.c_str(), sGamePath.c_str() );
//	bool pressedOK = (resDlg.DoModal() == IDOK);
//
//	// Reset the resources
//	//AfxSetResourceHandle( hPrevResHandle );
//
//	// If the user hit OK then store the paths
//	if( pressedOK )
//	{
//		// Store the paths
//		sResPath = resDlg.GetPathRes();
//		regKey.SetValue( ResourceToolsDB::REGPATH_ResPath, sResPath.c_str() );
//
//		sGamePath = resDlg.GetPathGame();
//		regKey.SetValue( ResourceToolsDB::REGPATH_GamePath, sGamePath.c_str() );
//
//		return true;
//	}
//
//	// The user did not hit OK
//	return false;
//}