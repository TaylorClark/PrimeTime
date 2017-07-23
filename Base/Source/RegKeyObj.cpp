//=============================================================================
/*!
	\file RegKeyObj.cpp
	Die By The Cannon
	Registry Object Source
	\author Taylor Clark
	\date August 12, 2004

  This file contains the implementation of the registry object class.
*/
//=============================================================================

#include "stdafx.h"
#include <windows.h>
#include "..\RegKeyObj.h"
#include <shlwapi.h>
#include <tchar.h>



namespace
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	CreateKey  Global
	///
	///	\param hParentKey The key that will be parent to this key
	///	\param szKeyName The name of the new key
	///	\param outKey The key reference to store the created key
	///	\return Whether or not the key was created
	///
	///	Create a key in the registry
	///
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool CreateKey( HKEY hParentKey, const wchar_t *szKeyName, HKEY &outKey )
	{
		// If we have bad data, bail
		if( !szKeyName )
			return false;

		// Create the registry key
		DWORD retVal = RegCreateKeyExW( hParentKey,
										szKeyName,
										0,
										NULL,
										REG_OPTION_NON_VOLATILE,
										KEY_READ | KEY_WRITE,
										NULL,
										&outKey,
										NULL );
		if( retVal != ERROR_SUCCESS )
		{
			wchar_t buffer[256];
			FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							0,
							0,
							(wchar_t*)buffer,
							256,
							NULL );
			return false;
		}

		// We created the key
		return true;
	}
} // End unnamed namespace


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::Init  Public
///
///	\param rootKey The predefined root key to use in the registry
///	\param szKeyPath The name of the parent key directory for all key values
///						read and saved by this object.
///	\param failIfMissing Set this value to true if you want this function to return false if the
///						specified key does not exist.  If this value is true then the key will be
///						created if it doesn't exits.  This value defaults to true.
///
///	\return Whether or not the object initialized correctly
///
///	Initialize the registry object class so it is prepared to read and write
///	data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::Init( ERootKey rootKey, const wchar_t *szKeyPath )
{
	// Get the root key constant
	HKEY winRootKey = HKEY_LOCAL_MACHINE;
	if( rootKey == RK_LocalMachine )
		winRootKey = HKEY_LOCAL_MACHINE;
	else if( rootKey == RK_CurrentUser )
		winRootKey = HKEY_CURRENT_USER;
	else if( rootKey == RK_Users )
		winRootKey = HKEY_USERS;
	else if( rootKey == RK_CurrentConfig )
		winRootKey = HKEY_CURRENT_CONFIG;
	else if( rootKey == RK_ClassesRoot )
		winRootKey = HKEY_CLASSES_ROOT;

	// Open the base parent key
	/*HKEY hBaseParentKey;
	if( RegOpenKeyEx( winRootKey,
						szKeyPath,
						0,
						KEY_READ | KEY_WRITE,
						&hBaseParentKey ) != ERROR_SUCCESS )
	{
		return false;
	}*/

	// Open the base parent key
	if( RegOpenKeyExW( winRootKey,
						szKeyPath,
						0,
						KEY_READ | KEY_WRITE,
						&m_hParentKey ) != ERROR_SUCCESS )
	{
		// The key couldn't be opened so let's try and create it
		if( !::CreateKey( winRootKey, szKeyPath, m_hParentKey ) )
			return false;
	}

	// Close the base parent key
	//RegCloseKey( hBaseParentKey );

	// We succeeded in creating the key
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::DoesValueExist  Public
///
///	\param szValueName The name of the value to check for
///	\return True if the value exists, false otherwise
///
///	Check if a value exists for this registry key.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::DoesValueExist( const wchar_t* szValueName )
{
	// If there is no parent, bail
	if( !m_hParentKey )
		return false;

	// Find the size of the value
	DWORD dwBufferSize = 0;
	LONG errorRet = RegQueryValueExW( m_hParentKey,
										szValueName,
										0,
										NULL,
										NULL,
										&dwBufferSize );

	// If we could not get the buffer size or the size is invalid then most likely the value does
	// not exist
	if( errorRet != ERROR_SUCCESS || dwBufferSize < 1 )
		return false;

	// If we got here the value exists
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::DoesSubkeyExist  Public
///
///	\param szSubkeyName The name of the subkey to check for
///	\return True if the subkey exists, false otherwise
///
///	Check if a subkey exists.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::DoesSubkeyExist( const wchar_t* szSubkeyName )
{
	// If we are not initialized yet then bail
	if( m_hParentKey == NULL )
		return false;

	// Open the base parent key
	HKEY hTempKey;
	if( RegOpenKeyExW( m_hParentKey,
						szSubkeyName,
						0,
						KEY_READ | KEY_WRITE,
						&hTempKey ) != ERROR_SUCCESS )
	{
		// The key couldn't be opened so it doesn't exist
		return false;
	}

	// Otherwise the key exists
	RegCloseKey( hTempKey );
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::GetValue  Public
///
///	\param szValueName The name of the value to get
///	\param psOutData The string to store the data in
///	\return Whether or not the key's value was successfully read
///
///	Get the value of a registry key.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::GetValue( const char* szValueName, std::string* psOutData ) const
{
	// If there is no parent, bail
	if( !m_hParentKey || !psOutData )
		return false;

	// Find the size of the value
	DWORD dwBufferSize = 0;
	LONG errorRet = RegQueryValueExA( m_hParentKey,
										szValueName,
										0,
										NULL,
										NULL,
										&dwBufferSize );

	// If we could not get the buffer size or the size is invalid, bail
	if( errorRet != ERROR_SUCCESS || dwBufferSize < 1 )
		return false;

	// Allocate a buffer for the data
	BYTE *pBuffer = new BYTE[ dwBufferSize + 1 ];
	if( !pBuffer )
		return false;

	// Get the value
	if( RegQueryValueExA( m_hParentKey,
							szValueName,
							0,
							NULL,
							pBuffer,
							&dwBufferSize ) != ERROR_SUCCESS )
	{
		delete [] pBuffer;
		return false;
	}

	// If the retrieved string is not null terminated then do so
	char* szBuffer = reinterpret_cast< char* >( pBuffer );
	int lastCharIndex = (dwBufferSize / sizeof(char)) - 1;
	if( szBuffer[ lastCharIndex ] != '\0' )
		szBuffer[ lastCharIndex + 1 ] = '\0';

	// Store the value
	*psOutData = szBuffer;

	// Free the buffer
	delete [] pBuffer;

	// We got the value sucessfully
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::GetValue  Public
///
///	\param szValueName The name of the value to get
///	\param psOutData The string to store the data in
///	\return Whether or not the key's value was successfully read
///
///	Get the value of a registry key using Unicode strings.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::GetValue( const wchar_t* szValueName, std::wstring* psOutData ) const
{
	// If there is no parent, bail
	if( !m_hParentKey || !psOutData )
		return false;

	// Find the size of the value
	DWORD dwBufferSize = 0;
	LONG errorRet = RegQueryValueExW( m_hParentKey,
										szValueName,
										0,
										NULL,
										NULL,
										&dwBufferSize );

	// If we could not get the buffer size or the size is invalid, bail
	if( errorRet != ERROR_SUCCESS || dwBufferSize < 1 )
		return false;

	// Allocate a buffer for the data, add a wchar_t in case it is not already NULL terminated
	BYTE *pBuffer = new BYTE[ dwBufferSize + sizeof(wchar_t) ];
	if( !pBuffer )
		return false;

	// Get the value
	if( RegQueryValueExW( m_hParentKey,
							szValueName,
							0,
							NULL,
							pBuffer,
							&dwBufferSize ) != ERROR_SUCCESS )
	{
		delete [] pBuffer;
		return false;
	}

	// If the retrieved string is not null terminated then do so
	wchar_t* szBuffer = reinterpret_cast< wchar_t* >( pBuffer );
	int lastCharIndex = (dwBufferSize / sizeof(wchar_t)) - 1;
	if( szBuffer[ lastCharIndex ] != _T('\0') )
		szBuffer[ lastCharIndex + 1 ] = _T('\0');

	// Store the value
	*psOutData = szBuffer;

	// Free the buffer
	delete [] pBuffer;

	// We got the value sucessfully
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::SetValue  Public
///
///	\param szValueName The name of the value to set
///	\param szData The data to store
///	\param dataLen The number of bytes to store
///	\return Whether or not the key's value was successfully stored
///
///	Set the value of a registry key.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::SetValue( const wchar_t* szValueName, const BYTE *pData, int dataLen ) const
{
	// If there is no parent key, bail
	if( !m_hParentKey )
		return false;
	
	// Set the value
	if( RegSetValueExW( m_hParentKey,
						szValueName,
						0,
						REG_BINARY,
						pData,
						static_cast< DWORD >( dataLen ) ) != ERROR_SUCCESS )
	{
		return false;
	}

	// We set the value correctly
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::SetValue  Public
///
///	\param sValueName The name of the value to set
///	\param sOutData The null-terminated string to store in the key
///	\return Whether or not the key's value was successfully stored
///
///	Set the value of a registry key.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::SetValue( const char* szValueName, const char* szData ) const
{
	// If there is no parent key, bail
	if( !m_hParentKey )
		return false;
	
	// Set the value
	if( RegSetValueExA( m_hParentKey,
						szValueName,
						0,
						REG_SZ,
						reinterpret_cast< const BYTE* >( szData ),
						static_cast< DWORD >( strlen(szData) + 1 ) ) != ERROR_SUCCESS )
	{
		return false;
	}

	// We set the value correctly
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::SetValue  Public
///
///	\param sValueName The name of the value to set
///	\param szData The null-terminated string to store in the key
///	\return Whether or not the key's value was successfully stored
///
///	Set the value of a registry key using Unicode strings.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::SetValue( const wchar_t* szValueName, const wchar_t* szData ) const
{
	// If there is no parent key, bail
	if( !m_hParentKey )
		return false;
	
	// Set the value
	if( RegSetValueExW( m_hParentKey,
						szValueName,
						0,
						REG_SZ,
						reinterpret_cast< const BYTE* >( szData ),
						static_cast< DWORD >( (wcslen(szData) + 1) * sizeof(wchar_t) ) ) != ERROR_SUCCESS )
	{
		return false;
	}

	// We set the value correctly
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RegKeyObj::RemoveSubkey  Public
///
///	\param szKeyName The name of the key to remove
///	\return Whether or not the key was removed
///
///	Remove a key from the registry
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RegKeyObj::RemoveSubkey( const wchar_t* szKeyName )
{
	// If there is no parent key, bail
	if( !m_hParentKey )
		return false;

	// Set the value
	DWORD deleteResult = RegDeleteValue( m_hParentKey, szKeyName );
	if( deleteResult != ERROR_SUCCESS )
	{
		wchar_t buffer[256];
		FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,
						0,
						0,
						(wchar_t*)buffer,
						256,
						NULL );

		return false;
	}

	// We removed the key
	return true;
}