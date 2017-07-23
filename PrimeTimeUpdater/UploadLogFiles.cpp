#include "stdafx.h"
#include <afxinet.h>
#include <string>
#include <list>
#include "Base/FileFuncs.h"
#include "Base/StringFuncs.h"
#include "Base/NumFuncs.h"
#include "Base/MsgLogger.h"
//#include "Base/RegKeyObj.h"
#include "Base/PTDefines.h"

// The log-in information for the upload account. This is based on the assumption that
// PrimeTimeMath.net will be in my ownership as long as the game is around.
static const wchar_t* FTP_URL = L"ftp.primetimemath.com";
static const wchar_t* FTP_USER = L"ptupload";
static const wchar_t* FTP_PASS = L"updatey";

typedef std::list< std::wstring > StringList;
const uint64 MAX_UPLOAD_FILE_SIZE = 1024 * 100;

extern void PumpMessages();


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FindFilesToUpload  Global
///
///	\param fileList The list to fill with file names
///
///	File all of the files that should be uploaded.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void FindFilesToUpload( StringList& fileList )
{
	// Find all dump files in the games folder
	const uint32 NUM_FIND_STRINGS = 2;
	std::wstring FIND_FILES[NUM_FIND_STRINGS];
	FIND_FILES[0] = TCBase::GetAppPath() + PTDefines::DUMP_PRE_FILE_NAME + L"*." + PTDefines::DUMP_FILE_EXT;
	FIND_FILES[1] = TCBase::GetAppPath() + PTDefines::LOG_PRE_FILE_NAME + L"*." + PTDefines::LOG_FILE_EXT;	

	for( uint32 curFindStrIndex = 0; curFindStrIndex < NUM_FIND_STRINGS; ++curFindStrIndex )
	{
		// Get the find string
		std::wstring sFindStr = FIND_FILES[curFindStrIndex];

		// Get the search handle
		WIN32_FIND_DATA findData;
		HANDLE hSearchFiles = FindFirstFile( sFindStr.c_str(), &findData );
		if( hSearchFiles != INVALID_HANDLE_VALUE )
		{
			// Go through the files that match the search pattern
			do
			{
				// Get the file name without the path
				wchar_t szFileName[ MAX_PATH ] = {0};
				wchar_t szFileExt[ MAX_PATH ] = {0};
				_wsplitpath_s( findData.cFileName, NULL, 0, NULL, 0, szFileName, MAX_PATH, szFileExt, MAX_PATH );
				std::wstring sFileName = szFileName;
				sFileName += szFileExt;

				// Determine the file size
				std::wstring sFullFilePath = TCBase::GetAppPath() + sFileName;
				uint64 fileSize = TCBase::GetFileSize( sFullFilePath.c_str() );

				// If the file has a valid size then store it for upload
				if( fileSize > 0 && fileSize < MAX_UPLOAD_FILE_SIZE )
					fileList.push_back( sFileName );
				// Otherwise delete it
				else
				{
					std::wstring sMsg = L"Deleting upload-file (";
					sMsg += sFullFilePath + L") because it is too big (";
					sMsg += TCBase::EasyUI64ToA( fileSize );
					sMsg + L" bytes) for uploading.";
					MSG_LOGGER_OUT( MsgLogger::MI_Note, sMsg.c_str() );
					DeleteFile( sFullFilePath.c_str() );
				}

			}while( FindNextFile( hSearchFiles, &findData ) == TRUE );

			// Close the search handle
			FindClose( hSearchFiles );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  UploadFile  Global
///
///	\param szFile The local file to upload
///
///	Upload a file to the Prime Time FTP.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool UploadFile( const wchar_t* szLocalFile, const wchar_t* szFtpFileName )
{
	CInternetSession g_InternetSess( L"PrimeTimeUploader" );

	// Create the FTP connection
	CFtpConnection* pFtpConn = 0;
	try
	{
		pFtpConn = g_InternetSess.GetFtpConnection( FTP_URL, FTP_USER, FTP_PASS );
	}
	catch( CInternetException* pExc )
	{
		std::wstring sMsg = L"Caught internet exception in UploadFile when trying to GetFtpConnection : ";
		
		// Format the exception message
		wchar_t szMsgBuffer[512] = {0};
		pExc->GetErrorMessage( szMsgBuffer, 512 );
		sMsg += szMsgBuffer;

		// Log the message
		MSG_LOGGER_OUT( MsgLogger::MI_Error, sMsg.c_str() );
		pExc->Delete();
		return false;
	}
	if( !pFtpConn  )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to connect to upload FTP." );
		return false;
	}

	// Upload the file
	bool retValue = true;
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Uploading %s to %s", szLocalFile, szFtpFileName );
	if( pFtpConn->PutFile( szLocalFile, szFtpFileName, FTP_TRANSFER_TYPE_BINARY ) == 0 )
	{
		DWORD errorCode = GetLastError();
		if( errorCode == 32 )
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to put file on FTP upload directory becuase the source file is already opened by another process (Error code: %u)", errorCode );
		else
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to put file on FTP upload directory (Error code: %u)", errorCode );
		retValue = false;
	}

	// Free the FTP connection
	pFtpConn->Close();
	delete pFtpConn;

	// Return success
	return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  UploadLogFiles  Global
///
///	Upload debugging files (logs and dumps) to a specified server.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void UploadLogFiles()
{
	// Get the files that need to be uploaded	
	StringList filesToUpload;
	FindFilesToUpload( filesToUpload );
	PumpMessages();

	// Get the player's key
	std::wstring sFilePrefix;
	/*RegKeyObj ptRegKey;
	if( ptRegKey.Init( RegKeyObj::RK_LocalMachine, PTDefines::REGKEY_RootPath ) )
	{
		ptRegKey.GetValue( PTDefines::REGKEY_UniqueID, &sFilePrefix );
		ptRegKey.Close();
	}*/

	// If the key couldn't be retrieved the prefix the file with an attempted unique prefix
	if( sFilePrefix.length() == 0 )
	{
		sFilePrefix = L"NK_";
		sFilePrefix += TCBase::EasyIToA( rand() % 65536 );	
	}

	// Add the time
	tm timeStruct;
	__time32_t localTime = _time32( NULL );
	_localtime32_s( &timeStruct, &localTime );
	sFilePrefix += L"_";
	sFilePrefix += TCBase::EasyIToA( timeStruct.tm_hour );
	sFilePrefix += TCBase::EasyIToA( timeStruct.tm_min );
	sFilePrefix += TCBase::EasyIToA( timeStruct.tm_sec );
	sFilePrefix += L"_";

	// Go through each file and upload
	for( StringList::iterator iterFile = filesToUpload.begin(); iterFile != filesToUpload.end(); ++iterFile )
	{
		// Get the file paths
		std::wstring sLocalPath = TCBase::GetAppPath() + *iterFile;
		std::wstring sUploadPath = sFilePrefix + *iterFile;

		// Upload the file and, if successful, remove the file
		if( UploadFile( sLocalPath.c_str(), sUploadPath.c_str() ) )
			DeleteFile( sLocalPath.c_str() );
		PumpMessages();
	}
}