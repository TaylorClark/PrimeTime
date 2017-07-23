//=================================================================================================
/*!
	\file StringFuncs.h
	Base Library
	String Functions Header
	\author Taylor Clark
	\date December 27, 2005

	This header contains the declarations for the string utility functions.
*/
//=================================================================================================

#pragma once
#ifndef __StringFuncs_h
#define __StringFuncs_h

#include "Types.h"
#include <string>
#include <vector>

namespace TCBase
{
	class ISerializer;
}


namespace TCBase
{
	/// Narrow a multi-byte string
	std::string Narrow( const wchar_t* szWideStr );
	
	/// Narrow a multi-byte string
	std::string Narrow( const std::wstring& sWideStr );

	/// Widen a single-byte string
	std::wstring Widen( const char* szStr );
	
	/// Widen a single-byte string
	std::wstring Widen( const std::string& sStr );

	/// Read an array of wchar_t variables from a file. This is needed because wchar_t are 4-bytes
	/// on a Mac and the flag to force them to 2-bytes has issues.
	std::wstring ReadChars( ISerializer& serializer, uint32 numChars );

	/// Read an array of wchar_t variables from a file. This is needed because wchar_t are 4-bytes
	/// on a Mac and the flag to force them to 2-bytes has issues.
	void ReadChars( ISerializer& serializer, wchar_t* pBuffer, uint32 numChars );

	void WriteChars( ISerializer& serializer, const std::wstring& string );

	void WriteChars( ISerializer& serializer, const wchar_t* pBuffer, uint32 numChars );

	/// Convert a Unicode string to an array of 16-bit unsigned integers. This is used to assist
	/// in dealing with 2-byte characters on PC and 4-byte on Mac. The returned array must be
	/// freed after use.
	uint16* StringToArray( const std::wstring& str );

	/// Convert a Unicode string to an array of 16-bit unsigned integers. This is used to assist
	/// in dealing with 2-byte characters on PC and 4-byte on Mac. The returned array must be
	/// freed after use.
	uint16* StringToArray( const wchar_t* pStr, uint32 numChars );

	/// Split path
	bool SplitPath( const wchar_t* szFullPath, wchar_t* szDriveBuffer, uint32 driveBufferLen,
				   wchar_t* szDirBuffer, uint32 dirBufferLen,
				   wchar_t* szFileNameBuffer, uint32 fileNameBufferLen,
				   wchar_t* szExtBuffer, uint32 extBufferLen );
	
	/// Split a string up by a delimiter
	std::vector<std::wstring> Split(const wchar_t* szStringToSplit, wchar_t delim);

	/// Make a path relative to another path
	std::wstring MakePathRelative( const wchar_t* szBasePath, const wchar_t* szDestPath );

	/// Ensure a path has all backslashes and a trailing backslash
	void EnsureValidPathSlashes( std::wstring& sPath );

	/// Combine a base path with a relative path to make a full path
	std::wstring CombinePaths( const wchar_t* szBasePath, const wchar_t* szRelPath );
	std::wstring CombinePaths( const std::wstring& sBasePath, const std::wstring& sRelPath );
	
	///	Remove leading and trailing whitespace (spaces and tabs)
	void Trim( std::wstring& source );
	
	///	Test is a string ends with another string
	bool EndsWith( const wchar_t* szStr, const wchar_t* szTestEndsWith );

	///	Test is a string begins with another string
	bool StartsWith( const wchar_t* szStr, const wchar_t* szTestStartsWith );

	///	Test is a string begins with another string
	bool StartsWith( const char* szStr, const char* szTestStartsWith );

	///	Append a parameter name, and optional value, to a URI
	void AppendQueryStringParameter( std::wstring& sUri, const wchar_t* szParamName, const wchar_t* szParamValue = NULL );
};


#endif // __StringFuncs_h