//=============================================================================
/*!
	\file RegKeyObj.h
	Taylor Clark's Utility Library
	Registry Object Header
	\author Taylor Clark
	\date August 11, 2004

  This header contains the class definition for registry object class.  This
  class simplifies creating, save data to, and loading data from registry keys.
*/
//=============================================================================

#pragma once

#ifndef __RegKeyObj_h
#define __RegKeyObj_h

#include <string>
#include <windows.h>


//-------------------------------------------------------------------------------------------------
/// 
///	\class RegKeyObj
///	\brief Encapsulates working with registry data.
///
///	A RegObj encapsulates all of the basic functionality of working with
///	windows registry data.
///
//-------------------------------------------------------------------------------------------------
class RegKeyObj
{
public:

	// The root key to use
	enum ERootKey
	{
		RK_LocalMachine,
		RK_CurrentUser,
		RK_Users,
		RK_CurrentConfig,
		RK_ClassesRoot,
		RK_COUNT
	};

	/// The constructor
	RegKeyObj() : m_hParentKey( NULL )	
	{}

	/// The destructor
	~RegKeyObj()
	{
		Close();
	}

	/// Initialize the object to use a parent key
	bool Init( ERootKey rootKey, const wchar_t* szParentKey );

	/// Close the registry key
	void Close()
	{
		// If we have a key, close it
		if( m_hParentKey )
			RegCloseKey( m_hParentKey );
		m_hParentKey = NULL;
	}

	/// Check if a subkey exists
	bool DoesSubkeyExist( const wchar_t* szSubkeyName );

	/// Check if a value exists
	bool DoesValueExist( const wchar_t* szValueName );

	/// Get a key's value
	bool GetValue( const char* szValueName, std::string* psOutData ) const;
	bool GetValue( const wchar_t* szValueName, std::wstring* psOutData ) const;

	/// Set a key's value
	bool SetValue( const char* szValueName, const char* szData ) const;
	bool SetValue( const wchar_t* szValueName, const wchar_t* szData ) const;

	/// Set a key's binary value
	bool SetValue( const wchar_t* szValueName, const BYTE* szData, int dataLen ) const;

	/// Remove a subkey
	bool RemoveSubkey( const wchar_t* szKeyName );

private:

	//! The parent key handle
	HKEY m_hParentKey;
};

#endif // __RegKeyObj_h