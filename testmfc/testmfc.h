// testmfc.h : main header file for the testmfc DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CtestmfcApp
// See testmfc.cpp for the implementation of this class
//

class CtestmfcApp : public CWinApp
{
public:
	CtestmfcApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
