//=================================================================================================
/*!
	\file FileFuncs.h
	Base Library
	File Functions Header
	\author Taylor Clark
	\date August 27, 2006

	This header contains the declarations for the file utility functions.
*/
//=================================================================================================

#pragma once
#ifndef __FileFuncs_h
#define __FileFuncs_h

#include <list>
#include <string>
#include "Types.h"


namespace TCBase
{
	/// Represents the command line parameters
	class ParamList
	{
	public:
		/// Represents a command line option with any parameters used by the option
		struct CmdLineParam
		{
			/// The option specified (With no leading slash)
			std::wstring sOption;

			/// The parameters for this option
			std::list< std::wstring > sParameters;
		};
		
		std::list< CmdLineParam > m_ParamList;

		mutable std::list< CmdLineParam >::const_iterator m_ParamListIter;

	public:

		/// The default constructor
		ParamList()
		{
			m_ParamListIter = m_ParamList.end();
		}

		/// Test if an option was specified
		bool HasOption( const wchar_t* szOption ) const;

		// Get the parameters associated with an option
		std::list< std::wstring > GetOptionParams() const;

		/// Parse a command line string into this data structure
		void ParseCmdLineOptions( const char* szParam );

		/// Parse a command line string into this data structure
		void ParseCmdLineOptions( int argc, const char* argv[] );

		/// Get the first option
		const CmdLineParam* GetFirstOption() const;

		/// Get the next option after calling GetFirstOption to initialize the enumeration
		const CmdLineParam* GetNextOption() const;
	};

	/// Get the application path for the current process
	std::wstring GetAppPath();

	/// Init the application path for the current process, used by the Mac code as the application
	/// path is passed in as the 0th argument
	void InitAppPathFromArg( const char* szArg0 );

	/// Get the application file name for the current process
	std::wstring GetAppFileName();

	/// Get the application file name, including path information, for the current process
	std::wstring GetAppFullPath();

	/// Get the directory path to where files can be created/modified since Vista/Windows 7 do not
	/// allow writing to the Program Files directory
	std::wstring GetUserFilesPath();

	/// Copy a file (Can't be called FileCopy because Windows reserved that name)
	void MakeFileCopy( const wchar_t* szSource, const wchar_t* szDest );

	/// Get the file path, with trailing backslash, from a full path
	std::wstring GetFilePath( const wchar_t* szFullPath );

	/// Get the file name, with extension, from a full path
	std::wstring GetFileNameWithExt( const wchar_t* szFullPath );

	/// Get the free drive space
	uint64 GetFreeDriveSpace( wchar_t driveLetter );

	// Find files matching a search pattern The returned list items do not contain path information
	std::list<std::wstring> FindFiles( const wchar_t* szDirectory, const wchar_t* szSearchPattern );

	/// Determine if a file or directory exists
	bool DoesFileExist( const wchar_t* szFullPath );

	/// Determine if a file or directory exists
	bool DoesFileExist( const char* szFullPath );
	/// Delete a file
	bool TCDeleteFile( const wchar_t* szFile );

	/// Rename a file
	bool RenameFile( const wchar_t* szOldFileName, const wchar_t* szNewFileName );

	/// Create a directory
	bool CreateDir( const wchar_t* szDirPath );

	/// Delete a directory and all of the files it contains (Backslashes allowed at end)
	bool DeleteDir( const wchar_t* szDirPath );

	/// Get a file's size
	uint64 GetFileSize( const wchar_t* szFullPath );

	/// Get a file's size
	uint64 GetFileSize( std::wifstream& inStream );

	/// Get a temporary file name
	std::wstring GetTmpFileName( const wchar_t* szExt );
};

#endif // __FileFuncs_h