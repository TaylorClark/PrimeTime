/*=================================================================================================

	\file FileFuncs.cpp
	Base Library
	File Functions Source
	\author Taylor Clark
	\Date August 27, 2006

	This source file contains the implementation of the number utility functions.

=================================================================================================*/

#include "../Types.h"
#include "../FileFuncs.h"
#include "../StringFuncs.h"
#include <stdlib.h>
#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#include <shellapi.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <mach-o/dyld.h>
#include <sys/param.h>
#define MAX_PATH MAXPATHLEN
#include <sys/mount.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
#include <stdio.h>
#include <pwd.h>
#endif
#include <errno.h>
#include <fstream>

static std::wstring g_sAppPath;
static std::wstring g_sAppFileName;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ParseOptionParams  Static Global
///
///	\param sOption A string containing a command line parameter's options
///	\returns The command line parameter options stored in a list of strings
///
///	Parse a command line parameter's options.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static std::list< std::wstring > ParseOptionParams( const std::string& sOption )
{
	std::list< std::wstring > retList;

	// Find the first character position
	std::string::size_type charPos = sOption.find_first_not_of( " " );
	while( charPos != std::string::npos && charPos < sOption.length() )
	{
		// Find the end of this option
		std::string::size_type wordEndPos = sOption.find_first_of( " ", charPos );

		// Get the length of this option
		std::string::size_type wordLen = 0;
		if( wordEndPos == std::string::npos )
			wordLen = sOption.length() - charPos;
		else
			wordLen = wordEndPos - charPos;
		
		// Get the word
		if( wordLen <= 0 )
			break;
		std::wstring sCurWord = TCBase::Widen( sOption.substr( charPos, wordLen ).c_str() );

		// If the word is empty then skip it
		if( sCurWord.find_first_not_of( L" " ) == std::string::npos )
			break;

		// Store the word
		retList.push_back( sCurWord );

		// Step ahead
		charPos = wordEndPos + 1;
	}

	// Return the list
	return retList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ParamList::ParseCmdLineOptions  Public
///
///	\param sOption A string containing a command line parameters
///	\param retList The command line parameters stored in a list of data structures
///
///	Parse the command line parameters.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void TCBase::ParamList::ParseCmdLineOptions( const char* szParam )
{
	std::string sParam( szParam );
	if( sParam.length() < 1 )
		return;

	// Read up until the first slash
	std::string::size_type slashPos = sParam.find_first_of( "/" );
	
	// Read through the parameters
	while( slashPos != std::string::npos )
	{
		CmdLineParam newParam;

		// Read until the space to get the option name's length
		std::string::size_type spacePos = sParam.find_first_of( " ", slashPos + 1 );
		std::string::size_type optionNameLen = 0;
		if( spacePos == std::string::npos )
			optionNameLen = sParam.length() - (slashPos + 1);
		else
			optionNameLen = spacePos - (slashPos + 1);
		if( optionNameLen <= 0 )
			break;

		// Get the option
		newParam.sOption = TCBase::Widen( sParam.substr( slashPos + 1, optionNameLen ).c_str() );

		// If this is the final option and there are no parameters
		std::string::size_type paramStartPos = slashPos + optionNameLen + 1;
		if( paramStartPos >= sParam.length() )
		{
			m_ParamList.push_back( newParam );
			break;
		}

		// Read until the next slash
		std::string::size_type nextSlashPos = sParam.find_first_of( "/", paramStartPos );

		// Get the option parameters length
		std::string::size_type optionsLen = 0;
		if( nextSlashPos == std::string::npos )
			optionsLen = sParam.length() - paramStartPos;
		else
			optionsLen = nextSlashPos - paramStartPos;

		// Parse the options
		if( optionsLen <= 0 )
			break;
		std::string sOptions( sParam.substr( paramStartPos, optionsLen ) );
		newParam.sParameters = ParseOptionParams( sOptions );
		m_ParamList.push_back( newParam );

		// Update the slash position
		slashPos = nextSlashPos;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ParamList::ParseCmdLineOptions  Public
///
///	\param sOption A string containing a command line parameters
///	\param retList The command line parameters stored in a list of data structures
///
///	Parse the command line parameters.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void TCBase::ParamList::ParseCmdLineOptions( int argc, const char* argv[] )
{
	std::string fullCommandLine;

	for( int curArgIndex = 1; curArgIndex < argc; ++curArgIndex )
	{
		if( curArgIndex > 1 )
			fullCommandLine += " ";

		fullCommandLine += argv[curArgIndex];
	}

	ParseCmdLineOptions( fullCommandLine.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ParamList::HasOption  public
///
///	\param szOption The option to search for
///	\returns True if the option was found, false otherwise
///
/// Test if an option was specified
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool TCBase::ParamList::HasOption( const wchar_t* szOption ) const
{
	// Go through the parameters
	for( std::list< CmdLineParam >::const_iterator iterOption = m_ParamList.begin(); iterOption != m_ParamList.end(); ++iterOption )
	{
		if( iterOption->sOption == szOption )
			return true;
	}

	// The option was not found
	return false;
}

/// Get the first option
const TCBase::ParamList::CmdLineParam* TCBase::ParamList::GetFirstOption() const
{
	// Set the iterator to the beginning of the list
	m_ParamListIter = m_ParamList.begin();

	// If there are no command line options then return NULL
	if( m_ParamListIter == m_ParamList.end() )
		return 0;

	return &( *m_ParamListIter );
}

/// Get the next option after calling GetFirstOption to initialize the enumeration
const TCBase::ParamList::CmdLineParam* TCBase::ParamList::GetNextOption() const
{
	// Step forward one option
	++m_ParamListIter;

	// If this is the end then return NULL
	if( m_ParamListIter == m_ParamList.end() )
		return 0;

	return &( *m_ParamListIter );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  InitAppPaths  Global
///
///	Initialize the global application path variables with information from the current process.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void InitAppPaths( const wchar_t* szFullAppPath )
{
#ifdef WIN32
	std::wstring sAppPath;
	std::wstring sAppFileName;

	// Split the path into its components
	wchar_t szDrive[ MAX_PATH ] = {0};
	wchar_t szDir[ MAX_PATH ] = {0};
	wchar_t szFile[ MAX_PATH ] = {0};
	wchar_t szExt[ MAX_PATH ] = {0};
	TCBase::SplitPath( szFullAppPath, szDrive, MAX_PATH, szDir, MAX_PATH, szFile, MAX_PATH, szExt, MAX_PATH );
	
	// Store the file name
	sAppFileName = szFile;
	sAppFileName += szExt;

	// If the directory is empty
	if( wcslen( szDir ) == 0 )
	{
		// Attempt to get the command line
		wchar_t* szCmdLine = GetCommandLineW();
		if( szCmdLine && wcslen(szCmdLine) > 0 )
		{
			int32 startCharIndex = 0;
			int32 pathLen = 0;
			
			// If the executable is surrounded with qoutes
			if( szCmdLine[0] == L'\"' )
			{
				startCharIndex = 1;

				wchar_t* pEndQoutePos = wcschr( szCmdLine + 1, L'\"' );
				if( pEndQoutePos == 0 )
					pathLen = (int32)( wcslen(szCmdLine) - 1 );
				else
					pathLen = (int32)( (pEndQoutePos - szCmdLine) - 1 );
			}
			// Else the path is not surrounded by quotes so it is most likely just the exe name
			else
			{
				wchar_t* pEndPos = wcschr( szCmdLine, L' ' );
				if( pEndPos == 0 )
					pathLen = (int32)( wcslen(szCmdLine) );
				else
					pathLen = (int32)(pEndPos - szCmdLine);
			}

			// Store the command line path
			std::wstring sCmdPath( szCmdLine + startCharIndex, pathLen );

			// Try to break up this string
			TCBase::SplitPath( sCmdPath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, szFile, MAX_PATH, szExt, MAX_PATH );

			// Restore the file name just to be consistent
			sAppFileName = szFile;
			sAppFileName += szExt;

			// If the path is relative
			if( wcslen(szDrive) == 0 )
			{
				// Get the current working directory
#ifdef WIN32
				wchar_t* szCWD = _wgetcwd( NULL, 0 );
				std::wstring sCwd = szCWD;
				free( szCWD );
#else
				char* szCWD = getcwd( NULL, 0 );
				std::wstring sCwd = TCBase::Widen( szCWD );
				free( szCWD );
#endif

				// Try to combine the paths
				sAppPath = TCBase::CombinePaths( sCwd, sCmdPath );

				// Try to break up this string
				TCBase::SplitPath( sAppPath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, szFile, MAX_PATH, szExt, MAX_PATH );

				sAppPath = szDrive;
				sAppPath += szDir;			
			}
			// Else the path is absolute so use it
			else
			{
				sAppPath = szDrive;
				sAppPath += szDir;
			}
		}
		// Else the current working directory must be used
		else
		{
			// Get the current working directory
			wchar_t* szCwd = _wgetcwd( NULL, 0 );

			// If the directory is valid
			if( szCwd )
			{
				// Store the path
				sAppPath = szCwd;
			}
			// Else the directory is invalid
			else
			{
				// Can't really do anyting
			}

			// Free the buffer that was allocated for the current working directory
			free( szCwd );
		}
	}
	// Else store the application path
	else
	{
		sAppPath = szDrive;
		sAppPath += szDir;
	}
	
	// Store the values
	g_sAppPath = sAppPath;
	g_sAppFileName = sAppFileName;
	
#else
	wchar_t szDrive[ MAX_PATH ] = {0};
	wchar_t szDir[ MAX_PATH ] = {0};
	wchar_t szFile[ MAX_PATH ] = {0};
	wchar_t szExt[ MAX_PATH ] = {0};
	TCBase::SplitPath( szFullAppPath, szDrive, MAX_PATH, szDir, MAX_PATH, szFile, MAX_PATH, szExt, MAX_PATH );
		
	g_sAppPath = szDrive;
	g_sAppPath += szDir;
	
	g_sAppFileName = szFile;
	g_sAppFileName += szExt;
#endif

	// Ensure valid slashes
	TCBase::EnsureValidPathSlashes( g_sAppFileName );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  InitAppPaths  Global
///
///	Initialize the global application path variables with information from the current process.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void InitAppPaths()
{
	std::wstring sAppPath;

#ifdef WIN32
	// Get the full application path
	wchar_t szFullAppPath[ MAX_PATH ];
	GetModuleFileNameW( NULL, szFullAppPath, MAX_PATH );

	sAppPath = szFullAppPath;
	
#else
	char appPathBuffer[MAX_PATH];
    readlink("/proc/self/exe", appPathBuffer, MAX_PATH);
	
	uint32_t pathLen = MAX_PATH;
	_NSGetExecutablePath( appPathBuffer, &pathLen);
	
	sAppPath = TCBase::Widen( appPathBuffer );
#endif	

	InitAppPaths( sAppPath.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	InitAppPathFromArg  Global
//
/// Init the application path for the current process, used by the Mac code as the application
/// path is passed in as the 0th argument
///////////////////////////////////////////////////////////////////////////////////////////////////
void TCBase::InitAppPathFromArg( const char* szArg0 )
{
	InitAppPaths( TCBase::Widen(szArg0).c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GetAppPath  Global
///	\returns The complete path, minus a file name, of the executable for the current process.
///
///	Get the application path for the current process. The returned string contains a trailing
///	backslash.
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring TCBase::GetAppPath()
{
	if( g_sAppPath.size() == 0 )
		InitAppPaths();

	return g_sAppPath;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetAppFileName  Global
///
///	\returns The file name, with no path, of the executable for the current process. For example,
///				if the current executable is 'C:\Test\TestApp.exe' then this function will return
///				'TestApp.exe'.
///
///	Get the application path for the current process.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring TCBase::GetAppFileName()
{
	if( g_sAppFileName.size() == 0 )
		InitAppPaths();

	return g_sAppFileName;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetAppFullPath  Global
///
///	\returns The file name, with full path, of the executable for the current process. For example,
///				if the current executable is 'C:\Test\TestApp.exe' then this function will return
///				'C:\Test\TestApp.exe'.
///
/// Get the application file name, including path information, for the current process
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring TCBase::GetAppFullPath()
{
	if( g_sAppFileName.size() == 0 )
		InitAppPaths();

	return g_sAppPath + g_sAppFileName;
}


#ifdef WIN32
///////////////////////////////////////////////////////////////////////////////////////////////////
//	GetMyDocumentsPath  Global
//
/// Get the full path to the current user's MyDocuments folder
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring GetMyDocumentsPath()
{
	wchar_t PathBuffer[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, PathBuffer, CSIDL_MYDOCUMENTS, 0);

	return std::wstring(PathBuffer) + L"\\";
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
//	GetUserFilesPath  Global
//
/// Get the directory path to where files can be created/modified since Vista/Windows 7 do not
/// allow writing to the Program Files directory
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring TCBase::GetUserFilesPath()
{
#ifdef WIN32
	std::wstring retPath = GetMyDocumentsPath() + L"My Games\\";

	if( !TCBase::DoesFileExist( retPath.c_str() ) )
		TCBase::CreateDir( retPath.c_str() );
	
	retPath += L"\\Prime Time\\";

	if( !TCBase::DoesFileExist( retPath.c_str() ) )
		TCBase::CreateDir( retPath.c_str() );

	return retPath;
#else
	const char *szHomeDir = getenv("HOME");

	if( !szHomeDir )
	{
        struct passwd* pwd = getpwuid(getuid());

        if (pwd)
           szHomeDir = pwd->pw_dir;
    }

	if( szHomeDir )
	{
		std::wstring homeDir = TCBase::Widen(szHomeDir);

		// Sometimes the home path can have more than one value (eg. user/great; test)
		std::wstring::size_type semiPos = homeDir.find_first_of(L';');
		if( semiPos != std::wstring::npos )
			homeDir = homeDir.substr( 0, semiPos );
		
		homeDir += L"/Library/Application Support/Prime Time/";
		
		if( !TCBase::DoesFileExist( homeDir.c_str() ) )
			TCBase::CreateDir(homeDir.c_str());
		
		return homeDir;
	}

	//TODO What should we do here? We can't find the user directory
    return TCBase::GetAppPath();
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  CopyFile  Global
///
///	\returns The file name, with full path, of the executable for the current process. For example,
///				if the current executable is 'C:\Test\TestApp.exe' then this function will return
///				'C:\Test\TestApp.exe'.
///
///	Copy a file (Can't be called FileCopy because Windows reserved that name). This function
///	overwrites the destination file if it already exists.
///////////////////////////////////////////////////////////////////////////////////////////////////
void TCBase::MakeFileCopy( const wchar_t* szSource, const wchar_t* szDest )
{
	std::ifstream sourceFile(TCBase::Narrow(szSource).c_str(), std::fstream::binary);
	std::ofstream destFile(TCBase::Narrow(szDest).c_str(), std::fstream::trunc|std::fstream::binary);

	// Copy the data
	if( sourceFile.good() && destFile.good() )
		destFile << sourceFile.rdbuf();

	destFile.close();
	sourceFile.close();
}


/// Get the file path (Drive and directory), with trailing backslash, from a full path
std::wstring TCBase::GetFilePath( const wchar_t* szFullPath )
{
	// Get the file name
	const size_t BUFF_SIZE = 256;
	wchar_t szDrive[ BUFF_SIZE ] = {0};
	wchar_t szDir[ BUFF_SIZE ] = {0};
	if( TCBase::SplitPath( szFullPath, szDrive, BUFF_SIZE, szDir, BUFF_SIZE, NULL, 0, NULL, 0 ) == false )
		return std::wstring();

	// Return the file name
	std::wstring retStr = szDrive;
	retStr += szDir;
	return retStr;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetFileNameWithExt()  Global
///
///	\param szFullPath The full file path or URL
///	\returns The file name with extension or an empty string on failure
///
/// Get the file name, with extension, from a full path.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring TCBase::GetFileNameWithExt( const wchar_t* szFullPath )
{
	// Get the file name
	const size_t BUFF_SIZE = 256;
	wchar_t szFileName[ BUFF_SIZE ] = {0};
	wchar_t szFileExt[ BUFF_SIZE ] = {0};
	if( TCBase::SplitPath( szFullPath, NULL, 0, NULL, 0, szFileName, BUFF_SIZE, szFileExt, BUFF_SIZE ) == false )
		return std::wstring();

	// Return the file name
	std::wstring retStr = szFileName;
	retStr += szFileExt;
	return retStr;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetFreeDriveSpace()  Global
///
///	\param szDriveLetter The letter of the drive to check
///	\returns The number of free bytes on the drive or 0 on a bad drive passed in
///
///	Get the free space remaining on a drive.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
uint64 TCBase::GetFreeDriveSpace( wchar_t driveLetter )
{
	// Convert the letter to lower case if needed
	if( driveLetter >= 'A' && driveLetter <= 'Z' )
		driveLetter = (driveLetter - 'A') + 'a';

	// Get the free drive space data
	uint64 remainingBytes = 0;
	
#ifdef WIN32
	// Ensure a valid drive
	int32 driveIndex = driveLetter -= 'a';
	if( driveIndex < 0 || driveIndex >= 26 )
		return 0;
	unsigned long driveBitmask = _getdrives();
	if( (driveBitmask & (1 << driveIndex)) == 0 )
		return 0;
	
	// Increment the drive index since, to _getdiskfree(), 1 means A, 2 means B, and so forth
	++driveIndex;
	
	_diskfree_t freeDiskData;
	if( _getdiskfree( (unsigned int)driveIndex, &freeDiskData ) != 0 )
		return 0;
	
	// Calculate the remaining bytes
	remainingBytes = (uint64)freeDiskData.avail_clusters;
	remainingBytes *= (uint64)freeDiskData.sectors_per_cluster;
	remainingBytes *= (uint64)freeDiskData.bytes_per_sector;
#else
	wchar_t drivePathBuffer[2] = { driveLetter, 0 };
	std::string drivePath = TCBase::Narrow( std::wstring(drivePathBuffer).c_str() );
	
	struct statfs driveInfo;
	statfs( drivePath.c_str(), &driveInfo );
	
	remainingBytes = driveInfo.f_bavail * driveInfo.f_bsize;
#endif

	

	// Return the free data
	return remainingBytes;
}


std::list<std::wstring> TCBase::FindFiles( const wchar_t* szDirectory, const wchar_t* szSearchPattern )
{
	std::list<std::wstring> retList;
	
#ifdef WIN32
	std::wstring sFullSearchPattern = szDirectory;
	if( sFullSearchPattern.length() == 0 )
		return retList;

	if( sFullSearchPattern[sFullSearchPattern.length() - 1] != '\\' )
		sFullSearchPattern += L"\\";
	sFullSearchPattern += szSearchPattern;

	// Find the first resource data file
	WIN32_FIND_DATA findFileData;
	HANDLE hCurFile = FindFirstFile( sFullSearchPattern.c_str(), &findFileData );

	
	// If there are no resource files then bail
	if( hCurFile == INVALID_HANDLE_VALUE )
		return retList;

	// Go through and find resource data files
	do
	{
		// Get the file name
		std::wstring sCurFile = findFileData.cFileName;

		retList.push_back( sCurFile );

		// Go to the next file
	}while( FindNextFile( hCurFile, &findFileData ) );
#else
	struct dirent *pDirEntry;
	
	std::string narrowedPattern = TCBase::Narrow( szSearchPattern );
	
    DIR *pDirInfo = opendir( TCBase::Narrow(szDirectory).c_str() );
    if (pDirInfo == NULL)
        return retList;	
    
	while ((pDirEntry = readdir(pDirInfo)) != NULL)
	{
		// Skip directories
		if (pDirEntry->d_type == DT_DIR)
			continue;
		
        if (fnmatch(narrowedPattern.c_str(), pDirEntry->d_name, FNM_CASEFOLD) == 0 )
			retList.push_back( TCBase::Widen( pDirEntry->d_name ) );
    }
    closedir(pDirInfo);
#endif
	
	return retList;
}


/// Determine if a file or directory exists
bool TCBase::DoesFileExist( const wchar_t* szFullPath )
{
#ifdef WIN32
	if( _waccess_s( szFullPath, 0 ) == 0 )
		return true;
#else
	if( access( TCBase::Narrow(szFullPath).c_str(), 0 ) == 0 )
		return true;
#endif
	return false;
}

bool TCBase::DoesFileExist( const char* szFullPath )
{
	return DoesFileExist( TCBase::Widen( szFullPath ).c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  TCDeleteFile()  Global
///	\param szFullPath The full file path to delete
///	\returns True if the file was deleted
///
/// Delete a file.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool TCBase::TCDeleteFile( const wchar_t* szFile )
{
#ifdef WIN32
	return _wremove( szFile ) == 0;
#else
	return remove( TCBase::Narrow(szFile).c_str() ) == 0;
#endif
}

/// Rename a file
bool TCBase::RenameFile( const wchar_t* szOldFileName, const wchar_t* szNewFileName )
{
#ifdef WIN32
	return _wrename( szOldFileName, szNewFileName ) == 0;
#else
	return rename( TCBase::Narrow(szOldFileName).c_str(), TCBase::Narrow(szNewFileName).c_str() );
#endif
}

/// Create a directory
bool TCBase::CreateDir( const wchar_t* szDirPath )
{
#ifdef WIN32
	return _wmkdir( szDirPath ) == 0;
#else
	return mkdir( TCBase::Narrow(szDirPath).c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
#endif
}


/// Delete a directory and all of the files it contains (Backslashes allowed at end)
bool TCBase::DeleteDir( const wchar_t* szDirPath )
{
#ifdef WIN32
	// If the string is invalid (It at least needs to be something like "c:")
	if( !szDirPath || wcslen( szDirPath ) < 2 )
		return false;
	std::wstring sDirPath = szDirPath;
	EnsureValidPathSlashes( sDirPath );

	// Fill in the file operation structure
	wchar_t szTempFileBuffer[ MAX_PATH ] = {0};
	//memset( szTempFileBuffer, 0, sizeof(szTempFileBuffer) );
	swprintf( (wchar_t*)szTempFileBuffer, MAX_PATH, L"%s*", sDirPath.c_str() );
	SHFILEOPSTRUCTW fileOp;
	memset( &fileOp, 0, sizeof(fileOp) );
	fileOp.wFunc = FO_DELETE;
	fileOp.pFrom = szTempFileBuffer;
	fileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;

	// Delete the directory's contents
	SHFileOperationW( &fileOp );

	// Delete the directory
	RemoveDirectoryW( sDirPath.c_str() );

	return true;
#else
	DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];
	
	std::string narrowedPath = TCBase::Narrow(szDirPath);
    dir = opendir( narrowedPath.c_str() );
    if (dir == NULL)
        return false;	
    
	while ((entry = readdir(dir)) != NULL)
	{
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
		{
            snprintf(path, (size_t) PATH_MAX, "%s/%s", narrowedPath.c_str(), entry->d_name);
			
            if (entry->d_type == DT_DIR)
                TCBase::DeleteDir( TCBase::Widen(path).c_str() );			
            
			remove( path );
        }
		
    }
    closedir(dir);
	
	return true;
#endif
	// Check the downloads folder for existing files
	//std::wstring sFindStr = sDirPath + L"*";
	//WIN32_FIND_DATA findData;
	//bool hasSubDir = false;
	//HANDLE hSearchFiles = FindFirstFile( sFindStr.c_str(), &findData );
	//if( hSearchFiles != INVALID_HANDLE_VALUE )
	//{
	//	do
	//	{
	//		// Ignore the current and back directories
	//		if( wcscmp( findData.cFileName, L"." ) == 0
	//			|| wcscmp( findData.cFileName, L".." ) == 0 )
	//			continue;

	//		// If the file is a directory then the directory can't be deleted
	//		if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
	//		{
	//			hasSubDir = true;
	//			break;
	//		}

	//	}while( FindNextFile( hSearchFiles, &findData ) == TRUE );

	//	// Close the search handle
	//	FindClose( hSearchFiles );
	//}

	//// If there are subdirectories then bail
	//if( hasSubDir )
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetFileSize()  Global
///
///	\param szFullPath The full path of the file whose size is to be returned
///	\returns The size of the file in bytes
///
///	Get the size of a file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
uint64 TCBase::GetFileSize( const wchar_t* szFullPath )
{
	std::ifstream inFile( TCBase::Narrow(szFullPath).c_str(), std::ios_base::in );
	if( !inFile.good() )
		return 0;

	// Get the local file size
	inFile.seekg( 0, std::ios_base::end );
	uint64 retVal = (uint64)inFile.tellg();

	// Close the file
	inFile.close();

	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GetFileSize()  Global
///	\param inStream The file stream for which we will find the size
///	\returns The size of the file in bytes
///
///	Get the size of a file.
///////////////////////////////////////////////////////////////////////////////////////////////////
uint64 TCBase::GetFileSize( std::wifstream& inFile )
{
	if( !inFile.good() )
		return 0;

	// Get the local file size
	inFile.seekg( 0, std::ios_base::end );
	uint64 retVal = (uint64)inFile.tellg();

	// Go back to the beginning
	inFile.seekg( 0, std::ios_base::beg );

	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetTempFileName()  Global
///
///	\param szExt The extension of the resulting file name, may be NULL and the leading '.' is
///				optional
///	\returns The full path temporary file name
///
/// Get a temporary file name
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring TCBase::GetTmpFileName( const wchar_t* szExt )
{
	// Generate the batch file name
#ifdef WIN32
	wchar_t szTempNameBuffer[MAX_PATH];
	if( _wtmpnam_s( szTempNameBuffer, MAX_PATH ) != 0 )
		return std::wstring();

	// Prepend with the path
	wchar_t szPathBuffer[MAX_PATH];
	_wgetcwd( szPathBuffer, MAX_PATH );
	
	// Combine the paths
	std::wstring sRetStr = CombinePaths( szPathBuffer, szTempNameBuffer );
#else
	char szTempNameBuffer[MAX_PATH];
	
	tmpnam( szTempNameBuffer );
	
	std::wstring sRetStr = TCBase::Widen( szTempNameBuffer );
#endif

	// If the caller wants to add an extension
	if( szExt != NULL && wcslen( szExt ) > 0 )
	{
		// If the extension does not have a dot then add one
		if( szExt[0] != L'.' && sRetStr[ sRetStr.length() - 1] != L'.' )
			sRetStr += L".";

		// Add the extension
		sRetStr += szExt;
	}

	// Return the file name
	return sRetStr;
}