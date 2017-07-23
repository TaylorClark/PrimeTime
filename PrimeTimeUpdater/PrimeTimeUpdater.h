// PrimeTimeUpdater.h : main header file for the PROJECT_NAME application
//

#pragma once

#include "stdafx.h"
#ifndef __AFXWIN_H__
//	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "Base/Types.h"
#include <list>
#include <map>


/// The structure storing a version's update data
struct VersionUpdate
{
	struct File
	{
		File()
		{
			Clear();
		}

		void Clear()
		{
			sMD5 = L"";
			sURL = L"";
			sFileNameWExt = L"";
			expectedFileSize = 0;
		}

		std::wstring sMD5;

		std::wstring sURL;

		std::wstring sFileNameWExt;

		uint32 expectedFileSize;
	};

	/// The version multiplied by 100, for example version 1.2 is stored as 120
	int32 version;

	/// The files that are part of this update
	std::list< File > files;
};

typedef std::map< int32, VersionUpdate > ApplicationUpdates;


/// The structure storing the information needed for a game update
struct GameUpdateInfo
{
	/// The default constructor
	GameUpdateInfo() : isUpdateReady( false )
	{}

	/// The files to copy over to the game directory (No path information)
	std::list< std::wstring > filesToCopyOver;

	/// Whether or not the update is ready to occur at this point
	bool isUpdateReady;
};


void UpdatePrimaryProgress( const wchar_t* szMsg, float32 normPct );
void UpdateSecondaryProgress( const wchar_t* szMsg, float32 normPct );

// CPrimeTimeUpdaterApp:
// See PrimeTimeUpdater.cpp for the implementation of this class
class CPrimeTimeUpdaterApp : public CWinApp
{
public:
	CPrimeTimeUpdaterApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPrimeTimeUpdaterApp theApp;