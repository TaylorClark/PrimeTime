/*=================================================================================================

	\file StringFuncs.cpp
	Base Library
	String Functions Source
	\author Taylor Clark
	\Date December 27, 2005

	This source file contains the implementation of the message logging class.

=================================================================================================*/

#include "../StringFuncs.h"
#include <iostream>
#include <list>
#include <cctype>
#include <vector>
#include <sstream>
#include <algorithm>
#include "../ISerializer.h"

#ifndef WIN32
#include <unistd.h>
#include <sys/param.h>
#define _MAX_PATH MAXPATHLEN
#endif

typedef std::list< std::wstring > StringList;


namespace TCBase
{

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Split  Static Global
///
/// Split a string up by a delimiter
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::wstring> Split(const wchar_t* szStringToSplit, wchar_t delim)
{
	std::vector<std::wstring> retList;

    std::wstringstream ss(szStringToSplit);
    std::wstring item;
    while(std::getline(ss, item, delim))
        retList.push_back(item);

    return retList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetDirectoryList  Static Global
///
///	\param sDirPath The directory to parse up
///	\returns A list of the directory names that make up a path, excluding the drive and file.
///
///	Break a path up into a list of directory names.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static std::list<std::wstring> GetDirectoryList( const std::wstring& sDirPath )
{
	// Break down the base path into a list of directories
	const wchar_t* PATH_DELIM = L"/\\";
	StringList dirList;
	std::wstring::size_type pathCurPos = 0;
	std::wstring::size_type pathSlashPos = sDirPath.find_first_of( PATH_DELIM );
	while( pathSlashPos != std::wstring::npos )
	{
		// If we have a valid string
		if( pathSlashPos > pathCurPos )
		{
			// Store the directory name
			std::wstring sDir = sDirPath.substr( pathCurPos, (pathSlashPos - pathCurPos) );	
			
			// If the directory is not the current directory string
			if( sDir != L"." )
				dirList.push_back( sDir );
		}

		// Find the next directory
		pathCurPos = pathSlashPos + 1;
		pathSlashPos = sDirPath.find_first_of( PATH_DELIM, pathCurPos );
	}

	// Return the list
	return dirList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  ParamList::ReadChars  Public
///	\param inFile The file to read
///	\param retList The command line parameters stored in a list of data structures
///
///	Parse the command line parameters.
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring ReadChars( ISerializer& serializer, uint32 numChars )
{
	wchar_t* pBuffer = new wchar_t[numChars + 1];

	ReadChars( serializer, pBuffer, numChars );
	pBuffer[numChars] = 0;

	std::wstring retStr = pBuffer;

	delete [] pBuffer;

	return retStr;
}

void ReadChars( ISerializer& serializer, wchar_t* pBuffer, uint32 numChars )
{
	uint16* pTempBuffer = new uint16[ numChars ];

	serializer.AddData( pTempBuffer, numChars );
	//pInStream->read( reinterpret_cast<char*>( pTempBuffer ), numChars * sizeof(uint16) );

	for( uint32 curCharIndex = 0; curCharIndex < numChars; ++curCharIndex )
		pBuffer[curCharIndex] = static_cast<wchar_t>( pTempBuffer[curCharIndex] );

	delete [] pTempBuffer;
}

uint16* StringToArray( const std::wstring& str )
{
	return StringToArray( str.c_str(), str.length() );
}

uint16* StringToArray( const wchar_t* pStr, uint32 numChars )
{
	uint16* pTempBuffer = new uint16[ numChars ];

	for( uint32 curCharIndex = 0; curCharIndex < numChars; ++curCharIndex )
		pTempBuffer[curCharIndex] = static_cast<uint16>( pStr[curCharIndex] );

	return pTempBuffer;
}

void WriteChars( ISerializer& serializer, const std::wstring& string )
{
	WriteChars( serializer, string.c_str(), (uint32)string.size() );
}

void WriteChars( ISerializer& serializer, const wchar_t* pBuffer, uint32 numChars )
{
	uint16* pTempBuffer = StringToArray( pBuffer, numChars );

	serializer.AddData( pTempBuffer, numChars );
	//pOutStream->write( reinterpret_cast<char*>( pTempBuffer ), numChars * sizeof(uint16) );

	delete [] pTempBuffer;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Narrow  Global
///
///	\param szWideStr The wide character string
///	\returns A pointer to a buffer containing the narrowed string, this memory must be freed by the
///				user.
///
///	Narrow a multi-byte string.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Narrow( const wchar_t* szWideStr )
{
	// Get the length of the string
	size_t strCharLen = (szWideStr != NULL) ? wcslen( szWideStr ) : 0;
	if( strCharLen == 0 )
		return std::string();

	// Allocate memory for the narrow string
	char* szRetStr = new char[ strCharLen + 1 ];

	// Copy, character for character, the lower byte of the multibyte string
	for( size_t charIndex = 0; charIndex < strCharLen; ++charIndex )
		szRetStr[ charIndex ] = std::use_facet< std::ctype<wchar_t> >( std::locale() ).narrow(szWideStr[ charIndex ], '?' );
	
	// Close the string with a NULL character and return it
	szRetStr[ strCharLen ] = 0;
	std::string sRet = szRetStr;
	delete [] szRetStr;
	return sRet;
}
	
/// Narrow a multi-byte string
std::string Narrow( const std::wstring& sWideStr )
{
	return Narrow( sWideStr.c_str() );
}
	
/// Widen a single-byte string
std::wstring Widen( const std::string& sStr )
{
	return Widen( sStr.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Widen  Global
///
///	\param szStr The character string
///	\returns A pointer to a buffer containing the widened string, this memory must be freed by the
///				user.
///
///	Widen a single-byte string.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring Widen( const char* szStr )
{
	// Get the length of the string
	size_t strCharLen = (szStr != NULL) ? strlen( szStr ) : 0;
	if( strCharLen == 0 )
		return std::wstring();

	// Allocate memory for the narrow string
	wchar_t* szRetStr = new wchar_t[ strCharLen + 1 ];

	// Copy, character for character, the string
	for( size_t charIndex = 0; charIndex < strCharLen; ++charIndex )
		szRetStr[ charIndex ] = std::use_facet< std::ctype<wchar_t> >( std::locale() ).widen( szStr[ charIndex ] );

	// Close the string with a NULL character and bail
	szRetStr[ strCharLen ] = 0;
	std::wstring sRetStr( szRetStr );
	delete [] szRetStr;
	return sRetStr;
}


bool SplitPath( const wchar_t* szFullPath, wchar_t* szDriveBuffer, uint32 driveBufferLen,
									wchar_t* szDirBuffer, uint32 dirBufferLen,
									wchar_t* szFileNameBuffer, uint32 fileNameBufferLen,
									wchar_t* szExtBuffer, uint32 extBufferLen )
{
	//_wsplitpath_s( szFullPath, szDriveBuffer, driveBufferLen, szDirBuffer, dirBufferLen, szFileNameBuffer, fileNameBufferLen, szExtBuffer, extBufferLen );
	
	uint32 pathLen = wcslen( szFullPath );
	
	// Find the colon for the drive
	uint32 charIndex = 0;
	while( szFullPath[charIndex] != ':' && charIndex < pathLen )
		++charIndex;

	// If a drive was found
	if( charIndex < pathLen && szDriveBuffer != NULL  )
	{
#ifdef WIN32
	wcsncpy_s(szDriveBuffer, driveBufferLen, szFullPath, charIndex + 1);
#else
	wcsncpy(szDriveBuffer, szFullPath, charIndex + 1);
#endif
	}

	// Find the first slash
	charIndex = 0;
	while( szFullPath[charIndex] != '/' && szFullPath[charIndex] != '\\' && charIndex < pathLen )
		++charIndex;
	
	if( charIndex == pathLen )
		return false;
	
	uint32 pathStartIndex = charIndex;
	const wchar_t* szSubFullPath = szFullPath + pathStartIndex;
	
	// Get the index of the last slash
	uint32 pathEndIndex = charIndex;
	while( charIndex < pathLen )
	{
		if( szFullPath[charIndex] == '/' || szFullPath[charIndex] == '\\' )
			pathEndIndex = charIndex;
		
		++charIndex;
	}
	
	// Store the path
#ifdef WIN32
	wcsncpy_s( szDirBuffer, dirBufferLen, szSubFullPath, pathEndIndex - pathStartIndex + 1 );
#else
	wcsncpy( szDirBuffer, szSubFullPath, pathEndIndex - pathStartIndex + 1 );
#endif
	
	szSubFullPath = szFullPath + pathEndIndex + 1;
	
	// Find the last period which represents start of the extension
	charIndex = pathLen - 1;
	while( szFullPath[charIndex] != '.' && charIndex > pathEndIndex )
		--charIndex;
	
	// If we found an extension
	if( charIndex > pathEndIndex )
	{
		// Grab the file name
#ifdef WIN32
		wcsncpy_s( szFileNameBuffer, fileNameBufferLen, szSubFullPath, (charIndex - pathEndIndex) - 1 );
#else
		wcsncpy( szFileNameBuffer, szSubFullPath, (charIndex - pathEndIndex) - 1 );
#endif

		// Grab the extenion
		szSubFullPath = szFullPath + charIndex;
#ifdef WIN32
		wcsncpy_s( szExtBuffer, extBufferLen, szSubFullPath, (pathLen - charIndex) + 1 );
#else
		wcsncpy( szExtBuffer, szSubFullPath, pathLen - charIndex + 1 );
#endif
	}
	// Otherwise store the text after the last slash as the file name
	else
	{
#ifdef WIN32
		wcsncpy_s( szFileNameBuffer, fileNameBufferLen, szSubFullPath, (pathLen - pathEndIndex) + 1 );
#else
		wcsncpy( szFileNameBuffer, szSubFullPath, pathLen - pathEndIndex + 1 );
#endif
	}
	
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MakePathRelative  Global
///
///	\param szBasePath The base path.  If the path does not end in a backslash then one is added.
///	\param szDestPath The path to make relative to the passed-in base path
///	\returns A string representing the destination path relative to the base path.  If there are
///				no common directories then the returned path is the destination path.
///
///	Make a path relative to another path.  For example, a base path of C:\Root\Dir1\ABC\ and a
///	destination path of C:\Root\Dir1\Hey\test.txt would return .\..\test.txt.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring MakePathRelative( const wchar_t* szInBasePath, const wchar_t* szDestPath )
{
	// If invalid values were passed in
	if( !szDestPath )
		return std::wstring();
	if( !szInBasePath || wcslen(szInBasePath) < 1 )
		return std::wstring( szDestPath );

	// Ensure the base directory ends in a backslash
	size_t basePathLen = wcslen(szInBasePath);
	std::wstring sBasePath = szInBasePath;
	if( sBasePath[ basePathLen - 1 ] != L'\\' && sBasePath[ basePathLen - 1 ] != L'/' )
		sBasePath += L'\\';

	// Break down the base path string
	wchar_t szBaseDrive[ _MAX_PATH ] = {0};
	wchar_t szBaseDir[ _MAX_PATH ] = {0};
	SplitPath( sBasePath.c_str(), szBaseDrive, _MAX_PATH, szBaseDir, _MAX_PATH, NULL, 0, NULL, 0 );

	// Break down the destination path string
	wchar_t szDestDrive[ _MAX_PATH ] = {0};
	wchar_t szDestDir[ _MAX_PATH ] = {0};
	wchar_t szDestFile[ _MAX_PATH ] = {0};
	wchar_t szDestExt[ _MAX_PATH ] = {0};
	SplitPath( szDestPath, szDestDrive, _MAX_PATH, szDestDir, _MAX_PATH, szDestFile, _MAX_PATH, szDestExt, _MAX_PATH );
	
	// If the drives differ then bail since there is no way to make that relative
	if( towlower(szBaseDrive[0]) != towlower(szDestDrive[0]) )
		return std::wstring( szDestPath );

	// Make the paths lower case for consistancy
	std::wstring sBaseDir( szBaseDir ); 
	std::transform(sBaseDir.begin(), sBaseDir.end(), sBaseDir.begin(), towlower);

	std::wstring sDestDir( szDestDir );
	std::transform(sDestDir.begin(), sDestDir.end(), sDestDir.begin(), towlower);
	
	// Break down the base path and then destination path into a list of directory names
	StringList sBaseDirList = GetDirectoryList( sBaseDir );
	StringList sDestDirList = GetDirectoryList( sDestDir );

	// Go through the directories until we find a mismatch
	int dirMatchCount = 0;
	StringList::iterator iterBaseDir = sBaseDirList.begin();
	StringList::iterator iterDestDir = sDestDirList.begin();
	while( iterBaseDir != sBaseDirList.end() && iterDestDir != sDestDirList.end() )
	{
		// If these directories don't match then stop looping
		if( wcscmp( iterBaseDir->c_str(), iterDestDir->c_str() ) != 0 )
			break;

		// Step to the next directory
		++iterBaseDir;
		++iterDestDir;
		++dirMatchCount;
	}

	// If no directories matched then bail
	if( dirMatchCount == 0 )
		return std::wstring( szDestPath );

	// Start to build the relative path
	std::wstring sRelPath = L".\\";

	// Add back steps until we have reached the last matching directory
	for( ; iterBaseDir != sBaseDirList.end(); ++iterBaseDir )
			sRelPath += L"..\\";

	// Add the remaining destination path
	for( ; iterDestDir != sDestDirList.end(); ++iterDestDir )
			sRelPath += *iterDestDir + L'\\';

	// Add the file name
	sRelPath += szDestFile;
	sRelPath += szDestExt;

	// Return the relative path
	return sRelPath;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EnsureValidPathSlashes  Global
///
///	\param sStr The path that is to be validated
///
///	Ensure a path has all backslashes, or forward slashes on Mac, a trailing slash, and remove
///	all instances of the current directory string ".\" except for the first one.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void EnsureValidPathSlashes( std::wstring& sPath )
{
	// If the length is too short then bail
	if( sPath.length() < 2 )
		return;

	// If the path is long enough ensure that it doesn't have a file name at the end
	std::wstring::size_type dotPos = sPath.find_last_of( L"." );
	bool hasFileName = false;
	if( dotPos != std::wstring::npos && dotPos > (sPath.length() - 5) )
		hasFileName = true;

#ifdef WIN32
	const wchar_t slashToRemove = L'/';
	const wchar_t slashToUse = L'\\';
#else
	const wchar_t slashToRemove = L'\\';
	const wchar_t slashToUse = L'/';
#endif

	// Add a trailing slash if there isn't a file name included in the path
	if( !hasFileName )
	{
		wchar_t lastChar = sPath[ sPath.length() - 1 ];
		if( lastChar != slashToRemove && lastChar != slashToUse )
			sPath += slashToUse;
	}

	// Ensure proper slashes
	for( std::wstring::size_type charIndex = 0; charIndex < sPath.length(); ++charIndex )
	{
		if( sPath[ charIndex ] == slashToRemove )
			sPath[ charIndex ] = slashToUse;
	}

	// Remove all instances of .\ except for the first one
#ifdef WIN32
	const wchar_t* CUR_DIR_STR = L"\\.\\";
#else
	const wchar_t* CUR_DIR_STR = L"/./";
#endif
	const std::wstring::size_type CUR_DIR_STR_LEN = wcslen( CUR_DIR_STR );
	std::wstring::size_type subStrPos = sPath.find( CUR_DIR_STR, 1 );
	while( subStrPos != std::wstring::npos )
	{
		// Remove the current directory string
		sPath = sPath.erase( subStrPos + 1, CUR_DIR_STR_LEN - 1 );
		subStrPos = sPath.find( CUR_DIR_STR, 1 );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CombinePaths  Global
///
///	\param sBasePath The base path
///	\param sRelPath The relative path
///
/// Combine a base path with a relative path to make a full path
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring CombinePaths( const std::wstring& sBasePath, const std::wstring& sRelPath )
{
	return CombinePaths( sBasePath.c_str(), sRelPath.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CombinePaths  Global
///
///	\param szBasePath The base path
///	\param szRelPath The relative path
///	\returns The full path to the relative file
///
/// Combine a base path with a relative path to make a full path
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring CombinePaths( const wchar_t* szBasePath, const wchar_t* szRelPath )
{
	// Break down the base path and relative string
	wchar_t szBaseDrive[ _MAX_PATH ] = {0};
	wchar_t szDir[ _MAX_PATH ] = {0};
	SplitPath( szBasePath, szBaseDrive, _MAX_PATH, szDir, _MAX_PATH, NULL, 0, NULL, 0 );
	StringList baseDirs = GetDirectoryList( std::wstring( szDir ) );
	
	wchar_t szRelDrive[_MAX_PATH ] = {0};
	wchar_t szRelFile[_MAX_PATH ] = {0};
	wchar_t szRelExt[ _MAX_PATH ] = {0};
	SplitPath( szRelPath, szRelDrive, _MAX_PATH, szDir, _MAX_PATH, szRelFile, _MAX_PATH, szRelExt, _MAX_PATH );
	StringList relDirs = GetDirectoryList( std::wstring( szDir ) );

	// If the relative path has a drive then it is not relative
	if( wcslen( szRelDrive ) != 0 )
		return std::wstring(szRelPath);

	// Add the base directories to the stack
	StringList combineDirs;
	for( StringList::iterator iterCurDir = baseDirs.begin(); iterCurDir != baseDirs.end(); ++iterCurDir )
		combineDirs.push_back( *iterCurDir );

	// Go through the relative paths and handle back step directories
	const wchar_t* BACKSTEP_DIR = L"..";
	for( StringList::iterator iterCurDir = relDirs.begin(); iterCurDir != relDirs.end(); ++iterCurDir )
	{
		// If this a back step, then pop a directory off, otherwise push this directory on
		if( *iterCurDir == BACKSTEP_DIR )
			combineDirs.pop_back();
		else
			combineDirs.push_back( *iterCurDir );

	}

	// Initialize the relative path by starting with the base drive
	std::wstring sRetPath = szBaseDrive;
	sRetPath += L"\\";

	// Go through the directories
	for( StringList::iterator iterCurDir = combineDirs.begin(); iterCurDir != combineDirs.end(); ++iterCurDir )
	{
		// Add this directory to the return string
		sRetPath += *iterCurDir;
		sRetPath += L"\\";
	}

	// Add on the file name
	sRetPath += szRelFile;
	sRetPath += szRelExt;

	// Return the path
	return sRetPath;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Trim()  Global
///	\param str The string to trim
///
///	Remove leading and trailing whitespace (spaces and tabs)
///////////////////////////////////////////////////////////////////////////////////////////////////
void Trim( std::wstring& str )
{
	std::wstring retStr;

	// Find the start and end of non-whitespace characters
    size_t startpos = str.find_first_not_of(L" \t");
    size_t endpos = str.find_last_not_of(L" \t");

    // if all spaces or empty return an empty string
	if(( std::wstring::npos == startpos ) || ( std::wstring::npos == endpos))
    {
       str = L"";
    }
    else
        str = str.substr( startpos, endpos-startpos+1 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  EndsWith()  Global
///	\param szStr The string to test
///	\param szTestEndsWith The string the test string may end with
///
///	Test if a string ends with another string
///////////////////////////////////////////////////////////////////////////////////////////////////
bool EndsWith( const wchar_t* szStr, const wchar_t* szTestEndsWith )
{
	// Bail on empty strings
	size_t strLen = szStr ? wcslen(szStr) : 0;
	size_t endsWithLen = szTestEndsWith ? wcslen(szTestEndsWith) : 0;
	if( strLen == 0 || endsWithLen == 0 )
		return false;

	// If the source string is short than the ends-with string then it can't end with it
	if( strLen < endsWithLen )
		return false;

	const wchar_t* szEndPart = szStr + (strLen - endsWithLen);

	return wcscmp(szEndPart,szTestEndsWith) == 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  StartsWith()  Global
///	\param szStr The string to test
///	\param szTestEndsWith The string the test string may start with
///
///	Test if a string starts with another string
///////////////////////////////////////////////////////////////////////////////////////////////////
bool StartsWith( const wchar_t* szStr, const wchar_t* szTestStartsWith )
{
	// Bail on empty strings
	size_t strLen = szStr ? wcslen(szStr) : 0;
	size_t startsWithLen = szTestStartsWith ? wcslen(szTestStartsWith) : 0;
	if( strLen == 0 || startsWithLen == 0 )
		return false;

	// If the source string is short than the starts-with string then it can't end with it
	if( strLen < startsWithLen )
		return false;

	return wcsncmp(szStr,szTestStartsWith,startsWithLen) == 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  StartsWith()  Global
///	\param szStr The string to test
///	\param szTestEndsWith The string the test string may start with
///
///	Test if a string starts with another string
///////////////////////////////////////////////////////////////////////////////////////////////////
bool StartsWith( const char* szStr, const char* szTestStartsWith )
{
	// Bail on empty strings
	size_t strLen = szStr ? strlen(szStr) : 0;
	size_t startsWithLen = szTestStartsWith ? strlen(szTestStartsWith) : 0;
	if( strLen == 0 || startsWithLen == 0 )
		return false;

	// If the source string is short than the starts-with string then it can't end with it
	if( strLen < startsWithLen )
		return false;

	return strncmp(szStr,szTestStartsWith,startsWithLen) == 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  AppendQueryStringParameter  Global
///	\param sUri The URI onto which the parameter will be added
///	\param szParamName The name of the parameter to add
///	\param szParamValue The optional value for the parameter. If this is NULL then just the name
///	is concatenated to the URI
///
///	Append a parameter name, and optional value, to a URI
///////////////////////////////////////////////////////////////////////////////////////////////////
void AppendQueryStringParameter( std::wstring& sUri, const wchar_t* szParamName, const wchar_t* szParamValue )
{
	// If there are no parameters yet than add a ?
	if( sUri.find_first_of( L"?" ) == std::wstring::npos )
		sUri += L"?";
	// Otherwise there are parameters so use a &
	else
		sUri += L"&";

	sUri += szParamName;

	if( szParamValue )
	{
		sUri += L"=";
		sUri += szParamValue;
	}
}

}
