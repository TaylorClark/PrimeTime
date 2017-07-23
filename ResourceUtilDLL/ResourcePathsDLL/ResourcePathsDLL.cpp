// ResourcePathsDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ResourcePathsDLL.h"
#include "ResPathDlg.h"
#include "ResChooserDlg.h"
#include <string>
#include "Base/StringFuncs.h"
#include "ResourceTools/ResourceToolsDB.h"
#include "Resource/Resource.h"

#include "Base/NumFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CWinApp theApp;

// The one and only application object

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// Display a dialog
		theApp;
	}

	return nRetCode;
}

//extern "C" int APIENTRY
//BOOL WINAPI DllMain(HINSTANCE hInstance, ULONG dwReason, LPVOID lpReserved)
//{
//	int x = 3;
//	x += x;
//
//	return 0;
//}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RunResDlg  Global
///
///	Run the path options dialog and store any changes from the dialog.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
//__declspec(dllexport) bool RunResDlg(void)
//bool RunResDlg(void)
//{
//	// Ensure we are using the DLL's resources
//	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
//	//HINSTANCE hPrevResHandle = AfxGetResourceHandle();
//	//AfxSetResourceHandle( AfxGetInstanceHandle() );
//
//	// Initialize the registry key object
//	RegKeyObj regKey;
//	regKey.Init( RegKeyObj::RK_CurrentUser, ResourceToolsDB::REGPATH_RootKeyPath );
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


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RunResChooserDlgFilter  Global
///
///	\param pResID The resource ID to fill in
///	\param typeFilter A resource type that, if set to a valid type, will only allow that type of
///						resource to be selected
///	\returns True if the user selected a resource, false otherwise
///
///	Display the resource chooser dialog and store the selected resource ID if one is chosen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RunResChooserDlg_Filter( HWND hWnd, const wchar_t* szRootResPath, ResourceID* pResID, EResourceType typeFilter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	if( !pResID )
		return false;
	
	InitCommonControls();
	theApp.InitInstance();

	//AfxSetResourceHandle( ::GetModuleHandle(NULL) );
	//CWinApp app( L"test" );
	CWinApp* pApp = AfxGetApp();

	// Display the window and handle if the user made changes
	CWnd* pWnd = CWnd::FromHandle( hWnd );
	CResChooserDlg resChooserDlg( szRootResPath, pWnd );
	resChooserDlg.SetFilter( typeFilter );
	//HINSTANCE hDlg = AfxFindResourceHandle( MAKEINTRESOURCE(IDD_DIALOG_RESCHOOSER), RT_DIALOG );
	//resChooserDlg.Create( IDD_DIALOG_RESCHOOSER, NULL );
	int dlgRetVal = (int)resChooserDlg.DoModal();
	if( dlgRetVal == -1 )
	{
		std::wstring sMsg(L"Returned -1:");
		sMsg += TCBase::EasyIToA( GetLastError() );
		MessageBox( NULL, sMsg.c_str(), L"Failed to select resource", MB_OK );
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


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RunResChooserDlg  Global
///
///	\param pResID The resource ID to fill in
///	\returns True if the user selected a resource, false otherwise
///
///	Display the resource chooser dialog and store the selected resource ID if one is chosen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
//bool RunResChooserDlg( ResourceID* pResID )
//{
//	return RunResChooserDlg_Filter( 0, pResID, RT_Error );
//}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RunResChooserDlg  Global
///
///	\param knownResources The list of resources to allow the user to choose from
///	\param pResID The resource ID to fill in
///	\param typeFilter A resource type that, if set to a valid type, will only allow that type of
///						resource to be selected
///	\returns True if the user selected a valid resource, false otherwise
///
///	Display the resource chooser dialog and store the selected resource ID if one is chosen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RunResChooserDlgList_Filter( HWND hWnd, const std::vector< ResourceMgr::KnownResourceItem >& knownResources, ResourceID* pResID, EResourceType typeFilter )
{
	// If the user did not pass in a valid resource ID to fill in then bail
	if( !pResID )
		return false;

	// Ensure we are using the DLL's resources
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	// Display the window and handle if the user made changes
	CWnd* pWnd = CWnd::FromHandle( hWnd );
	CResChooserDlg resChooserDlg( knownResources, pWnd );
	resChooserDlg.SetFilter( typeFilter );
	
	//resChooserDlg.ShowWindow(SW_SHOW);

	HINSTANCE hPrevResHandle = AfxGetResourceHandle();
	AfxSetResourceHandle( ::GetModuleHandle(NULL) );
	bool pressedOK = (resChooserDlg.DoModal() == IDOK);
	AfxSetResourceHandle( hPrevResHandle );

	// If the user hit OK then store the paths
	if( pressedOK )
	{
		// Store the resource ID
		*pResID = resChooserDlg.GetSelectedResourceID();

		// Return the user selected a resource ID
		return true;
	}

	// The user did not hit OK
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RunResChooserDlg  Global
///
///	\param pResID The resource ID to fill in
///
///	Display the resource chooser dialog and store the selected resource ID if one is chosen.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RunResChooserDlgList( HWND hWnd, const std::vector< ResourceMgr::KnownResourceItem >& knownResources, ResourceID* pResID )
{
	return RunResChooserDlgList_Filter( hWnd, knownResources, pResID, RT_Error );
}