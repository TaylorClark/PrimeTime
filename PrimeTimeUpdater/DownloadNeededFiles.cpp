#include "stdafx.h"
#include "PrimeTimeUpdater.h"
#include "Base/FileFuncs.h"
#include "FileDownloader.h"
#include "FileProgressDlg.h"
#include <direct.h>
#include "Base/MsgLogger.h"
#include "Base/StringFuncs.h"
#include "Base/NumFuncs.h"
#include "MD5.h"
#include <set>
#include "Base/PTDefines.h"

static const wchar_t* UPDATE_XML_URL = L"http://www.primetimemath.com/public_ftp/pt_updates.xml";
//static const wchar_t* UPDATE_XML_URL = L"http://www.primetimemath.com/Updates/pt_updates_example.xml";
extern ApplicationUpdates ParseUpdateXML( DataBlock xmlData );
extern bool g_IsInPassiveMode;
extern std::wstring g_sResultsDlgMsg;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MakeMD5  Global
///
///	\param szFilePath The full path of the file for which an MD5 checksum is generated
///	\returns The MD5 code, in a string, of the file, or an empty string on failure
///
///	Generate an MD5 for a file
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring MakeMD5( const wchar_t* szFilePath )
{
	// Open the file
	FILE* pFile = 0;
	if( _wfopen_s( &pFile, szFilePath, L"rb" ) != 0 )
		return std::wstring();

	// Generate the MD5
	MD5 fileMD5( pFile );

	// Close the file
	fclose( pFile );

	// Return the MD5
	return TCBase::Widen( fileMD5.hex_digest() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileDLProgressCB  Global
///
///	\param bytesDownloaded The number of bytes that have been downloaded
///	\param totalSize The total number of bytes to download
///
///	Update the user interface to reflect the download progress
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDLProgressCB( uint32 bytesDownloaded, uint32 totalSize )
{
	const uint32 MSG_BUFFER_SIZE = 128;
	wchar_t szBuffer[ MSG_BUFFER_SIZE ] = {0};
	float32 percentComplete = 0.0f;

	// If the total size is unknown
	if( totalSize == 0 )
	{
		percentComplete = 0.5f;
		uint32 kbDown = bytesDownloaded / 1024;
		swprintf_s( szBuffer, MSG_BUFFER_SIZE, L"%uKB downloaded (Total size unknown)", kbDown );
	}
	// Else use the estimated file size
	else
	{
		// Update the progress dialog
		percentComplete = (float32)bytesDownloaded / (float32)totalSize;
		
		uint32 kbDown = bytesDownloaded / 1024;
		uint32 totalKB = totalSize / 1024;

		// Update the progress message
		swprintf_s( szBuffer, MSG_BUFFER_SIZE, L"%u of %uKB downloaded", kbDown, totalKB );
	}

	// Update the secondary progress
	UpdateSecondaryProgress( szBuffer, percentComplete );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetLocalFileVersion  Global
///
///	\param appUpdates The updates from the downloaded XML file
///	\param szFileName The full local file path
///	\returns The version of the local file, multiplied by 100 like the other version values
///
///	Get the version of a local file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 GetLocalFileVersion( const ApplicationUpdates& appUpdates, const wchar_t* szFullFileName )
{
	// Get the MD5 for the local file
	std::wstring sCurFileMD5 = MakeMD5( szFullFileName );
	if( sCurFileMD5.size() == 0 )
		return 0;

	// Get the file name of the local file
	std::wstring sFileName = TCBase::GetFileNameWithExt( szFullFileName );

	// Go through the application updates and compare MD5s
	for( ApplicationUpdates::const_iterator iterUpdate = appUpdates.begin(); iterUpdate != appUpdates.end(); ++iterUpdate )
	{
		// Go through the files in this update
		for( std::list< VersionUpdate::File >::const_iterator iterFile = iterUpdate->second.files.begin(); iterFile != iterUpdate->second.files.end(); ++iterFile )
		{
			// If this is not the file name then skip it
			if( iterFile->sFileNameWExt != sFileName )
				continue;
			
			// If our file matches this MD5
			if( iterFile->sMD5 == sCurFileMD5 )
				return iterUpdate->first;
		}
	}

	// Return version 1 by default
	return 100;
}


struct FileToDownload
{
	FileToDownload()
	{
		Clear();
	}

	void Clear()
	{
		sFileName = L"";
		sDownloadURL = L"";
		version = 0;
		sMD5 = L"";
		expectedSize = 0;
	}

	std::wstring sFileName;

	std::wstring sDownloadURL;

	int32 version;

	std::wstring sMD5;

	uint32 expectedSize;
};

typedef std::map< std::wstring, FileToDownload > FileDownloadMap;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetUniqueFiles  Global
///
///	\param appUpdates The updates from the downloaded XML file
///	\returns The list of files that have updates online
///
///	Get the unique files listed in an application update.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
FileDownloadMap GetUniqueFiles( const ApplicationUpdates& appUpdates )
{
	FileDownloadMap retMap;

	// Go through all of the updates
	for( ApplicationUpdates::const_iterator iterUpdate = appUpdates.begin(); iterUpdate != appUpdates.end(); ++iterUpdate )
	{
		// Go through the files in this update
		for( std::list< VersionUpdate::File >::const_iterator iterFile = iterUpdate->second.files.begin(); iterFile != iterUpdate->second.files.end(); ++iterFile )
		{
			// Create the new entry
			FileToDownload curFile;
			curFile.sDownloadURL = iterFile->sURL;
			curFile.sFileName = iterFile->sFileNameWExt;
			curFile.sMD5 = iterFile->sMD5;
			curFile.version = iterUpdate->first;
			curFile.expectedSize = iterFile->expectedFileSize;
			FileDownloadMap::value_type newEntry( iterFile->sFileNameWExt, curFile );

			// If the file is already in the set then skip it
			FileDownloadMap::iterator iterCurFile = retMap.find( iterFile->sFileNameWExt );
			if( iterCurFile != retMap.end() )
			{
				// If this version is newer than the one already stored in the return map
				if( iterUpdate->first > iterCurFile->second.version )
					iterCurFile->second = newEntry.second;
			}
			// Else store the new entry
			else
				retMap.insert( newEntry );
		}
	}

	return retMap;
}


typedef std::map< std::wstring, int32 > StringVerMap;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetLocalFileVersions  Global
///
///	\param appUpdates The updates from the downloaded XML file
///	\returns The map taking a file name (no path information) as a key and returing a version
///
///	Get the versions of local files related to the downloaded XML file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
StringVerMap GetLocalFileVersions( const ApplicationUpdates& appUpdates )
{
	StringVerMap retMap;

	// Get all of the unique files in all of the supplied updates
	FileDownloadMap uniqueSet = GetUniqueFiles( appUpdates );

	// Go through the files
	for( FileDownloadMap::const_iterator iterCurFile = uniqueSet.begin(); iterCurFile != uniqueSet.end(); ++iterCurFile )
	{
		// Get the full path for the file
		std::wstring sFullFilePath = TCBase::GetAppPath() + iterCurFile->first.c_str();
		if( _waccess_s( sFullFilePath.c_str(), 0 ) != 0 )
			continue;

		// Get the local file version
		int32 localVer = GetLocalFileVersion( appUpdates, sFullFilePath.c_str() );
		if( localVer == 0 )
		{
			// Output an error message
			std::wstring sMsg = L"The retrieval of the version for the local file ";
			sMsg += sFullFilePath;
			sMsg += L" failed.";
			MSG_LOGGER_OUT( MsgLogger::MI_Error, sMsg.c_str() );

			continue;
		}

		// If the file is already in the map
		StringVerMap::iterator iterFile = retMap.find( iterCurFile->first );
		if( iterFile != retMap.end() )
		{
			if( localVer > iterFile->second )
				iterFile->second = localVer;
		}
		// Else store the file
		else
			retMap.insert( std::pair< std::wstring, int32 >( iterCurFile->first, localVer ) );
	}

	return retMap;
}

typedef std::list<FileToDownload> FilesToDownload;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  BuildFilesToDownload  Global
///
///	\param appUpdates The updates from the downloaded XML file
///	\param retInfo The object storing what files are needed for the update
///	\returns The map taking a file name (no path information) as a key and returing download
///				information.
///
///	Build a list of the files that need to be downloaded to get the newest versions.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
FilesToDownload BuildFilesToDownload( const ApplicationUpdates& appUpdates, GameUpdateInfo& retInfo )
{
	// Get the versions of the downloadable files
	FileDownloadMap downloadMap = GetUniqueFiles( appUpdates );
	
	// Store the update files into the game update info
	for( FileDownloadMap::iterator iterFile = downloadMap.begin(); iterFile != downloadMap.end(); ++iterFile )
		retInfo.filesToCopyOver.push_back( iterFile->first );

	// Get the versions of the local files in the game directory
	StringVerMap localFileVerMap = GetLocalFileVersions( appUpdates );

	// Go through the local files and remove any up-to-date files from the list to download
	for( StringVerMap::iterator iterLocalFile = localFileVerMap.begin(); iterLocalFile != localFileVerMap.end(); ++iterLocalFile )
	{
		// If the files-to-download list does not include this file name then don't bother checking
		// the version
		FileDownloadMap::iterator iterFileToDL = downloadMap.find( iterLocalFile->first );
		if( iterFileToDL == downloadMap.end() )
			continue;

		// If the version of this local file is up-to-date then remove it from the list to download
		if( iterLocalFile->second >= iterFileToDL->second.version )
		{
			downloadMap.erase( iterFileToDL );

			// Also remove the file from the game update info since this is not a file that will
			// need to be copied over to complete the update
			retInfo.filesToCopyOver.remove( iterLocalFile->first );
		}
	}

	// Check the downloads folder for existing files
	std::wstring sFindDir = TCBase::GetAppPath() + PTDefines::UPDATE_FOLDER;
	std::wstring sFindStr = sFindDir + L"*.*";
	WIN32_FIND_DATA findData;
	HANDLE hSearchFiles = FindFirstFile( sFindStr.c_str(), &findData );
	if( hSearchFiles != INVALID_HANDLE_VALUE )
	{
		do
		{
			// Get the file name without the path
			wchar_t szFileName[ MAX_PATH ] = {0};
			wchar_t szFileExt[ MAX_PATH ] = {0};
			_wsplitpath_s( findData.cFileName, NULL, 0, NULL, 0, szFileName, MAX_PATH, szFileExt, MAX_PATH );
			std::wstring sFileName = szFileName;
			sFileName += szFileExt;

			// If the file name is part of the files to download
			FileDownloadMap::iterator iterDloadFile = downloadMap.find( sFileName );
			if( iterDloadFile != downloadMap.end() )
			{
				// If the MD5's don't match then remove the file from the updates folder because it
				// is invalid
				std::wstring localMD5 = MakeMD5( (sFindDir + iterDloadFile->first).c_str() );
				if( localMD5 != iterDloadFile->second.sMD5 )
					DeleteFile( findData.cFileName );
				// Else the MD5's match so there is no need to download the file
				else
					downloadMap.erase( iterDloadFile );
			}

		}while( FindNextFile( hSearchFiles, &findData ) == TRUE );

		// Close the search handle
		FindClose( hSearchFiles );
	}

	// Store the map values in a list since fast look-up isn't needed anymore
	FilesToDownload retList;
	for( FileDownloadMap::iterator iterCurFile = downloadMap.begin(); iterCurFile != downloadMap.end(); ++iterCurFile )
		retList.push_back( iterCurFile->second );

	// Return the list of files that need to be downloaded to complete an update
	return retList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PumpMessages  Global
///
///	Handle window messages to prevent freezing.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void PumpMessages()
{
	// Pump messages
	MSG msg;
	while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
		GetMessage( &msg, NULL, 0, 0 );
		DispatchMessage( &msg );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetVersionString  Global
///
///	\param version The version value which is scaled by 100
///	\returns A descriptive string of the version
///
///	Get a descriptive string for a version
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring GetVersionString( int32 version )
{
	// Convert the version, which is scaled by 100, to a string
	std::wstring sRet = L"version ";
	sRet += TCBase::EasyIToA( version / 100 );
	sRet += L".";
	sRet += TCBase::EasyIToA( version % 100 );

	return sRet;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DownloadNeededFiles  Global
///
///	\returns True if all files that are needed to complete an update are downloaded, false
///				otherwise
///
///	Download all of the files needed for an update.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GameUpdateInfo DownloadNeededFiles()
{
	GameUpdateInfo retInfo;

	// Ensure the updates directory exists
	_wmkdir( (TCBase::GetAppPath() + PTDefines::UPDATE_FOLDER ).c_str() );
	SetFileAttributes( (TCBase::GetAppPath() + PTDefines::UPDATE_FOLDER ).c_str(), FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN );

	// Update the progress dialog
	UpdatePrimaryProgress( L"Downloading update information", 0.1f );
	UpdateSecondaryProgress( L"", 0.0f );

	// Pump messages to update the interface
	PumpMessages();

	// Download the update info XML data
	FileDownloader file;
	if( !file.DownloadToBuffer( UPDATE_XML_URL, FileDLProgressCB ) )
	{
		std::wstring sMsg = L"Failed to download update index : ";
		sMsg += file.GetLastErrorStr();

		MsgLogger::Get().Output( sMsg.c_str() );
		g_sResultsDlgMsg += sMsg + L"\n";
		
		return retInfo;
	}

	// Update the progress
	UpdatePrimaryProgress( L"Reading update information", 0.2f );
	UpdateSecondaryProgress( L"", 0.0f );
	
	// Pump messages
	PumpMessages();

	// Parse the XML data
	ApplicationUpdates updates = ParseUpdateXML( file.GetFileData() );
	file.Free();
	g_sResultsDlgMsg += L"The update index file was downloaded successfully.\n";
	
	// Update the progress
	UpdatePrimaryProgress( L"Determining local versions", 0.3f );
	
	// Pump messages
	PumpMessages();

	// Go through the local files, already downloaded files, and update files and build a list of
	// which files need to be downloaded
	FilesToDownload neededFiles = BuildFilesToDownload( updates, retInfo );

	if( neededFiles.size() == 0 )
	{
		retInfo.isUpdateReady = false;
		g_sResultsDlgMsg += L"There are no updates available at this time.\n";
	}
	else
		retInfo.isUpdateReady = true;

	// Go through the list of remaining files to download and retrieve them
	int32 fileCount = 1;
	for( FilesToDownload::iterator iterDlFile = neededFiles.begin(); iterDlFile != neededFiles.end(); ++iterDlFile )
	{
		// Update the primary text
		std::wstring sPrimText = L"Downloading ";
		sPrimText += iterDlFile->sFileName;
		sPrimText += L" (";
		sPrimText += TCBase::EasyIToA( fileCount );
		sPrimText += L" of ";
		sPrimText += TCBase::EasyIToA( (int32)neededFiles.size() );
		sPrimText += L" files)";
		UpdatePrimaryProgress( sPrimText.c_str(), 0.5f );
	
		// If the download fails
		std::wstring sDestFile = TCBase::GetAppPath() + PTDefines::UPDATE_FOLDER + iterDlFile->sFileName;
		if( !file.DownloadToFile( iterDlFile->sDownloadURL.c_str(), sDestFile.c_str(), FileDLProgressCB ) )
		{
			std::wstring sError = L"The file ";
			sError += iterDlFile->sFileName;
			sError += L" failed to be downloaded for ";
			sError += GetVersionString( iterDlFile->version );
			MSG_LOGGER_OUT( MsgLogger::MI_Error, sError.c_str() );

			// Add to the results dialog text
			g_sResultsDlgMsg += L"Download of ";
			g_sResultsDlgMsg += iterDlFile->sFileName + L", ";
			g_sResultsDlgMsg += GetVersionString(iterDlFile->version) + L", failed\n";

			retInfo.isUpdateReady = false;
		}
		// Else the file was downloaded successfully
		else
		{
			// Check the MD5 of the file
			std::wstring sCurFileMD5 = MakeMD5( sDestFile.c_str() );

			// If the files don't match then delete the file
			bool fileIsValid = true;
			if( sCurFileMD5 != iterDlFile->sMD5 )
			{
				std::wstring sError = L"The file ";
				sError += iterDlFile->sFileName;
				sError += L" was downloaded successfully, but the MD5 did not match for version ";
				sError += iterDlFile->version;

				MSG_LOGGER_OUT( MsgLogger::MI_Error, sError.c_str() );

				fileIsValid = false;
			}
			// Else check the file sizes if there is one to check against
			else if( iterDlFile->expectedSize != 0 )
			{
				uint32 fileSize = (uint32)TCBase::GetFileSize( sDestFile.c_str() );

				// Compare the size
				if( fileSize != iterDlFile->expectedSize )
				{
					fileIsValid = false;

					std::wstring sError = L"The file ";
					sError += iterDlFile->sFileName;
					sError += L" was downloaded successfully, but the size did not match for version ";
					sError += iterDlFile->version;
					sError += L". Expected ";
					sError += TCBase::EasyUIToA( iterDlFile->expectedSize );
					sError += L" but downloaded ";
					sError += TCBase::EasyUIToA( fileSize );
					sError += L" bytes.";

					MSG_LOGGER_OUT( MsgLogger::MI_Error, sError.c_str() );
				}
			}

			// Store a friendly message
			g_sResultsDlgMsg += L"Downloaded " + iterDlFile->sFileName + L", " + GetVersionString( iterDlFile->version );

			// If the file is invalid then delete it
			if( !fileIsValid )
			{
				g_sResultsDlgMsg += L", but the downloaded file was corrupted so it had to be deleted\n";

				DeleteFile( sDestFile.c_str() );
				retInfo.isUpdateReady = false;
			}
			else
				g_sResultsDlgMsg += L"\n";
		}

		// Pump messages
		PumpMessages();

		// Increment the downloaded file count
		fileCount++;
	}

	// Return the data structure containing the update info
	return retInfo;
}