/////////////////////////////////////////////////////////////////////////////
// MODULE  : DirDialog.h
// PURPOSE : Allows selection of a directory
//
// (c) 2001 LithTech, Inc.  All Rights Reserved DO NOT DISTRIBUTE
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __DIRDIALOGNOMFC_H__
#define __DIRDIALOGNOMFC_H__

#pragma once
#include <string>

class CBrowseDialog
{
private:
	enum Mode
	{
		DIRECTORIES,
		COMPUTERS,
		PRINTERS
	};

	//call to set the mode of the dialog, defaults to directories in constructor
	void					SetMode(Mode mode);

	HWND					m_hParent;		//parent window

	wchar_t					m_strInitDir[_MAX_PATH];	//(in)path to start browsing from, if left blank, will start from root
	wchar_t					m_strSelDir[_MAX_PATH];		//(in)currently selected directory
	wchar_t					m_strTitle[_MAX_PATH];		//(in)title of dialog window
	unsigned int			m_Flags;					//(in)additional flags for BROWSEINFO struct, will be or'ed with m_InternalFlags

	int						m_iImageIndex;				//(out)the image associated with m_strPath in the dialog

	/// When DoBrowse returns true, this stores the selected directory
	std::wstring			m_sSelDir;

	static int __stdcall BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

public:

	CBrowseDialog();
	virtual ~CBrowseDialog();
	
	// Set the initial directory
	void SetInitialDir( const std::wstring& sDir );

	//called to create modeless directory browse dialog
	bool DoBrowse();

	/// Get the selected directory
	const std::wstring& GetSelDir() const { return m_sSelDir; }

private:

	unsigned int			m_InternalFlags;//set my SetMode() function
};

#endif//__DIRDIALOGNOMFC__



