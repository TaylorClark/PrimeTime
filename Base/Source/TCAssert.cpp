#include "../TCAssert.h"
#include <string>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
//#include <shellapi.h>



// if this number is not high enough, CreateAssertDialogTemplate() will
// overwrite memory and CreateDialogIndirect() will crash
#define DIALOG_TEMPLATE_BYTES	768

// static text message
#define IDC_STATIC1				1000
#define IDC_STATIC2				1001

// read-only text edit box for assert message 
#define IDC_RO_EDIT				1002 

// allocates and fills in a DLGTEMPLATE struct as well as the subsequent controls
DLGTEMPLATE* CreateAssertDialogTemplate(const wchar_t* pAssertText, const wchar_t* pMsgText, const wchar_t* pFilename, int32 lineNum);

// frees memory allocated for a DLGTEMPLATE
void DestroyDialogTemplate(DLGTEMPLATE* pDlgTemplate);

// the message handling function for the assert dialog
BOOL CALLBACK AssertDialogProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
 
const int ASSERT_BUF_SIZE = 1024;
static wchar_t g_pAssertText[ASSERT_BUF_SIZE];


//-------------------------------------------------------------------
int32 sysassert(const wchar_t* pAssertText, const wchar_t* pMsgText, const wchar_t* pFilename, int lineNum)
{
	const std::wstring strAssertText( pAssertText ? pAssertText : L"" );
	const std::wstring strMsgText( pMsgText ? pMsgText : L"" );


	HINSTANCE		hInst;
	LPDLGTEMPLATE	pDlgTemplate;
	int32			retval = 0;	

	// only allow one instance of the assert dialog
	static bool bAsserting = false;
	if( bAsserting )
		return LT_ASSERT_CONTINUE;
	bAsserting = true;

	// find the client window
	hInst = GetModuleHandle(NULL);

	// create the assert dialog box template.  This is an annoying but useful way to
	// make a dialog box from within a static lib where a resource template won't
	// work.  Additionally, when compiled out, there's no extra resource overhead.
	pDlgTemplate = CreateAssertDialogTemplate( strAssertText.c_str(), strMsgText.c_str(), pFilename, lineNum );
	
	// if the dialog creation fails for some reason, break into the debugger
	if( !pDlgTemplate )
		return LT_ASSERT_DEBUG;

	// client hides the cursor, so we need to show it if we want folks to use the buttons!
	//ShowCursor(true);

	// create & show the dialog box
	retval = (int32)DialogBoxIndirect(hInst, pDlgTemplate, NULL, AssertDialogProc);

	// release the dialog template's memory
	DestroyDialogTemplate(pDlgTemplate);

	// if we continue on, we need to hide the cursor again.
	///ShowCursor(false);

	bAsserting = false;

	// return the value from DialogBoxIndirect representing the buttons pressed by the user
	return retval;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CopyToWideChar  Global
///
///	\param ppStrOut A pointer to a pointer to the destination memory address
///	\param pStrIn The source data that will be copied to the destination memory
///	\param bufferSize The available bytes left in *ppStrOut
///	\returns The number of bytes written to *ppStrOut
///
///	Copy an array of wide characters to a memory address and advance the destination pointer past
///	the copied data
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static uint32 CopyToWideChar(wchar_t** ppStrOut, const wchar_t* pStrIn, uint32 bufferSize )
{ 
	// Get the length of the string to concatanate
    size_t dwLen  = wcslen( pStrIn );

	if( dwLen * sizeof(wchar_t) > bufferSize )
		throw "Not enough memory to assert";

	// Append to the string
    wchar_t* strOut = *ppStrOut;
	wcsncpy_s( strOut, dwLen + 1, pStrIn, dwLen );
	
	// Add 1 to the length because wcsncpy_s appends a NULL terminator if there is space
	dwLen++;

	// Step past the concatenated string
    *ppStrOut += dwLen;

	// Return the number of bytes copied
	return (uint32)(dwLen * sizeof(wchar_t));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AddDialogControl  Global
///
///	\param pp A pointer to a pointer to the destination memory address
///	\param bufferSize The available bytes left in *pp
///	\returns The number of bytes written to *pp
///
///	Add a control to the dialog template in memory
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static uint32 AddDialogControl(WORD** pp, uint32 bufferSize, DWORD dwStyle, SHORT x, SHORT y, 
                             SHORT cx, SHORT cy, WORD id,  
                             const wchar_t* strClassName, const wchar_t* strTitle) 
{
	// Store the position of the data pointer before data is traversed
	uint8* pDataStart = (uint8*)*pp;

    // DWORD align the current pointer
	DLGITEMTEMPLATE* p = (DLGITEMTEMPLATE*)( ( ( (int64)*pp + 3) >> 2) << 2 );
 
    p->style           = dwStyle | WS_CHILD | WS_VISIBLE;
    p->dwExtendedStyle = 0L;
    p->x               = x;
    p->y               = y;
    p->cx              = cx;
    p->cy              = cy;
    p->id              = id;
 
	// Advance the pointer past this control's data
    *pp = (WORD*)(++p);
 
	// Get the position of the data pointer to determine the number of bytes remaining
	uint8* pDataCur = (uint8*)*pp;
	bufferSize -= (uint32)(pDataCur - pDataStart);

	// Set the control class name
    bufferSize -= CopyToWideChar( (wchar_t**)pp, strClassName, bufferSize );

	// Set the control text
    bufferSize -= CopyToWideChar( (wchar_t**)pp, strTitle, bufferSize );
 
	// Skip "extra stuff"
    (*pp)++;

	// Return the number of bytes written
	pDataCur = (uint8*)*pp;
	return (uint32)(pDataCur - pDataStart);
} 
 

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CreateAssertDialogTemplate  Global
///
///	\param pAssertText The text to be displayed for the failed assertion expression
///	\param pMsgText The helpful text message
///	\param pMsgText The helpful text message
///	\param pFilename The file name in which the assertion occurred
///	\param lineNum The source file line number on which the assertion occurred
///	\returns The dialog template that can be used in DialogBoxIndirect to display a window
///
///	Generate the assert dialog into an in-memory dialog template
///
///////////////////////////////////////////////////////////////////////////////////////////////////
DLGTEMPLATE* CreateAssertDialogTemplate(const wchar_t* pAssertText, const wchar_t* pMsgText, 
										const wchar_t* pFilename, int32 lineNum) 
{ 
	// get the application name (argv[0])
	int aArgc = 0;
	wchar_t** aArgvW = CommandLineToArgvW(GetCommandLineW(), &aArgc);
	wchar_t szUnknown[1]; szUnknown[0] = NULL;

	// copy the assert message into a static buffer.  Gotta love those CRLF combos...
	const wchar_t WC_LF = L'\n';
	const wchar_t WC_CR = L'\r';

	// If there is a message to display
	if( wcslen(pMsgText) > 0 )
	{
		swprintf_s(g_pAssertText,
					ASSERT_BUF_SIZE,
					L"Assertion Failure: %s%c%c%c%c%s%c%c%c%cApp: %s%c%cFile: %s%c%cLine: %i%c%c",
									pAssertText, WC_CR, WC_LF, WC_CR, WC_LF,
									pMsgText, WC_CR, WC_LF, WC_CR, WC_LF,
									aArgvW ? aArgvW[0] : szUnknown, WC_CR, WC_LF, 
									pFilename, WC_CR, WC_LF, 
									lineNum, WC_CR, WC_LF);
	}
	// Else there is no message to display, just a failed boolean expression
	else
	{
		swprintf_s(	g_pAssertText,
					ASSERT_BUF_SIZE,
					L"Assertion Failure: %s%c%c%c%cApp: %s%c%cFile: %s%c%cLine: %i%c%c%c%c",
									pAssertText, WC_CR, WC_LF, WC_CR, WC_LF,
									aArgvW ? aArgvW[0] : szUnknown, WC_CR, WC_LF, 
									pFilename, WC_CR, WC_LF,
									lineNum, WC_CR, WC_LF, WC_CR, WC_LF);
	}


	// Allocate ample memory for building the template 

		
	DLGTEMPLATE* pDlgTemplate = (DLGTEMPLATE*)new char[DIALOG_TEMPLATE_BYTES]; 
	if( !pDlgTemplate )
		return NULL;

	// Clear the dialog data
    memset( pDlgTemplate, 0, DIALOG_TEMPLATE_BYTES); 
     
    // Fill in the DLGTEMPLATE info 
    pDlgTemplate->style = DS_SYSMODAL | DS_MODALFRAME | DS_NOIDLEMSG | DS_SETFOREGROUND | 
                           DS_3DLOOK | DS_CENTER | WS_POPUP | WS_VISIBLE | 
                           WS_CAPTION | WS_SYSMENU | DS_SETFONT; 
    pDlgTemplate->dwExtendedStyle = 0L; 
    pDlgTemplate->cdit = 7; // The number of controls
    pDlgTemplate->x = 0; 
    pDlgTemplate->y = 0; 
    pDlgTemplate->cx = 320; 
    pDlgTemplate->cy = 200;
 
    // Add menu array, class array, dlg title, font size and font name 
    DLGTEMPLATE* pdt = pDlgTemplate; 
    WORD* pw = (WORD*)(++pdt);
	uint32 bufferSize = DIALOG_TEMPLATE_BYTES - sizeof(DLGTEMPLATE);

	// Clear the menu array
    *pw++ = L'\0';
	bufferSize -= sizeof(WORD);

	// Clear the class value
    *pw++ = L'\0';
	bufferSize -= sizeof(WORD);

	// Set the title
    bufferSize -= CopyToWideChar( (wchar_t**)&pw, L"PrimeTime ASSERT", bufferSize );

	// Skip the font size
    *pw++ = 8;
	bufferSize -= sizeof(WORD) * 8;

	// Set the font name
    bufferSize -= CopyToWideChar( (wchar_t**)&pw, L"Arial", bufferSize );
  
   
	// Add the Continue button 
    bufferSize -= AddDialogControl(&pw, bufferSize, BS_PUSHBUTTON | WS_TABSTOP, 7, 172, 75, 14,                
                     LT_ASSERT_CONTINUE, L"BUTTON", L"Continue"); 
	
	// Add the Debug button (default)
    bufferSize -= AddDialogControl(&pw, bufferSize, BS_DEFPUSHBUTTON | WS_TABSTOP, 84, 172, 75, 14,  
                      LT_ASSERT_DEBUG, L"BUTTON", L"Debug"); 
	
	// Add the Ignore button 
	bufferSize -= AddDialogControl(&pw, bufferSize, BS_PUSHBUTTON | WS_TABSTOP, 161, 172, 75, 14,  
                      LT_ASSERT_IGNORE, L"BUTTON", L"Ignore"); 

	// Add the Abort button 
    bufferSize -= AddDialogControl(&pw, bufferSize, BS_PUSHBUTTON | WS_TABSTOP, 238, 172, 75, 14,  
                      LT_ASSERT_ABORT, L"BUTTON", L"Abort"); 
 

	// Add the "Assert" text 
    bufferSize -= AddDialogControl(&pw, bufferSize, SS_LEFT, 8, 5, 304, 36,  
                      IDC_STATIC1, L"STATIC", L""); 

	// Add the edit control for the assert message
    bufferSize -= AddDialogControl(&pw, bufferSize, ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_LEFT | WS_BORDER | WS_TABSTOP,  
                      7, 42, 304, 85,  
                      IDC_RO_EDIT, L"EDIT", L""); 

	// Add the button description box 
    bufferSize -= AddDialogControl(&pw, bufferSize, SS_LEFT, 8, 138, 304, 25,  
                      IDC_STATIC2, L"STATIC", L""); 

    return pDlgTemplate; 
} 


//----------------------------------------------------------------------------- 
void DestroyDialogTemplate( DLGTEMPLATE* pDlgTemplate )
{
	if( pDlgTemplate )
		delete [] pDlgTemplate;
}
 

//----------------------------------------------------------------------------- 
BOOL CALLBACK AssertDialogProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM) 
{ 

    // Handle the initialization message 
    if (WM_INITDIALOG == uiMsg) { 
        // Set text for dialog items 
		SetDlgItemTextW(hDlg, IDC_STATIC1, L"This dialog is the result of a failed condition or a redirected output message.  If you feel you are seeing this message in error, please cut and paste the text below into an email to taylor@taylorclarksoftware.com." );
		SetDlgItemTextW(hDlg, IDC_STATIC2, L"`Debug' brings up the debugger.  `Ignore' will ignore this breakpoint for the rest of the program's execution.  `Abort' terminates the program." );
		SetDlgItemTextW(hDlg, IDC_RO_EDIT, g_pAssertText); 
		// beep at the user      
		MessageBeep(MB_ICONEXCLAMATION);
		// handled the message
        return TRUE; 
    } 
     
	// handle (some) command messages
    if (WM_COMMAND == uiMsg)
	{ 
        // Handle the case when the user hits a button
        if( LT_ASSERT_CONTINUE == LOWORD(wParam) ||
			LT_ASSERT_ABORT    == LOWORD(wParam) ||
			LT_ASSERT_IGNORE   == LOWORD(wParam) ||
			LT_ASSERT_DEBUG    == LOWORD(wParam))
		{
			// terminate the dialog box
            EndDialog(hDlg, LOWORD(wParam));
			
			// We handled the message
            return TRUE; 
        } 
    } 

	// didn't handle the message
    return FALSE; 
}
#else
int32 sysassert(const wchar_t* pAssertText, const wchar_t* pMsgText, const wchar_t* pFilename, int lineno)
{
	return 0;
}
#endif
