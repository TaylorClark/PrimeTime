#include "stdafx.h"
#include "PrimeTimeUpdater.h"
#include "Base/FileFuncs.h"
#include "Base/MsgLogger.h"
#include "Base/PTDefines.h"
#include <stdio.h>
#include <process.h>
#include <fstream>

static const wchar_t* TEMP_FILE_PREFIX = L"TEMP_PT_";
typedef std::list< std::wstring > StringList;
extern std::wstring g_sResultsDlgMsg;

std::wstring WriteOverwriteBatch(const wchar_t* szNewUpdaterFile);
void RunFile( const wchar_t* szFile );


static void RevertFileNames( const StringList& renamedFiles )
{
	std::wstring sAppDir = TCBase::GetAppPath();

	// Not all files could be renamed so undo the renaming of files that did get changed
	for( StringList::const_iterator iterFile = renamedFiles.begin(); iterFile != renamedFiles.end(); ++iterFile )
	{
		// Get the temporary and original file names
		std::wstring sOrigFileName = sAppDir + *iterFile;
		std::wstring sTempName = sAppDir + TEMP_FILE_PREFIX + *iterFile;

		// Rename the old file back to the original name
		if( _wrename( sOrigFileName.c_str(), sTempName.c_str() ) != 0 )
		{
			g_sResultsDlgMsg += L"Failed to re-rename ";
			g_sResultsDlgMsg += *iterFile;
			g_sResultsDlgMsg += L", this may cause your game not to work.  Please visit the website to redownload the needed files.";

			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to revert rename from %s to %s", sOrigFileName.c_str(), sTempName.c_str() );
		}
	}
}

static bool RenameFiles( const GameUpdateInfo& updateInfo, StringList& renamedFiles )
{
	std::wstring sAppDir = TCBase::GetAppPath();

	// Go through the local files to update and prepend the file names with a prefix to clear
	// the way for the downloaded files to be copied over
	bool allFilesRenamed = true;
	for( StringList::const_iterator iterFile = updateInfo.filesToCopyOver.begin();
			iterFile != updateInfo.filesToCopyOver.end();
			++iterFile )
	{
		// If this file is the updater then skip it and let it be handled in CopyFiles()
		if( *iterFile == PTDefines::WUPDATER_EXE_FILE )
			continue;

		// Get the full path to the local file name
		std::wstring sCurFileName = sAppDir + *iterFile;

		// If the file does not exist then skip the renaming of this file
		if( !TCBase::DoesFileExist( sCurFileName.c_str() ) )
			continue;

		// Create the temporary name
		std::wstring sTempName = sAppDir + TEMP_FILE_PREFIX + *iterFile;

		// See if a file with the temporary name already exists
		if( TCBase::DoesFileExist( sTempName.c_str() ) )
			DeleteFile( sTempName.c_str() );

		// Rename the old file
		if( _wrename( sTempName.c_str(), sCurFileName.c_str() ) != 0 )
		{
			allFilesRenamed = false;
			int errorVal = 0;
			std::wstring sReason;
			if( _get_errno( &errorVal ) != 0 )
			{
				sReason = L"unknown";
			}
			else
			{
				if( errorVal == EACCES )
					sReason = *iterFile + L" could not be copied ";
				else if( errorVal == ENOENT )
					sReason = *iterFile + L" does not exist";
				else if( errorVal == EINVAL )
					sReason = *iterFile + L" is an invalid name";
			}

			// Display a useful message
			g_sResultsDlgMsg += L"Update failed because ";
			g_sResultsDlgMsg += sReason;

			break;
		}
		// Else renaming succeeded so store the file in the list of files that were renamed (Just
		// in case the file name needs to be reverted)
		else
			renamedFiles.push_back( *iterFile );
	}

	// If all of the files were renamed then return success
	if( allFilesRenamed )
		return true;

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CopyFiles  Global
///
///	\param updateInfo The game update info
///	\returns True if all of the update files were copied to the game directory, false otherwise
///
///	Copy the downloaded update files to the game directory
///
///////////////////////////////////////////////////////////////////////////////////////////////////
static bool CopyFiles( const GameUpdateInfo& updateInfo )
{
	std::wstring sAppDir = TCBase::GetAppPath();

	// Copy the files from the download folder to the game folder
	StringList copiedFiles;
	bool updateUpdater = false;
	bool allFilesCopied = true;
	for( StringList::const_iterator iterFile = updateInfo.filesToCopyOver.begin();
			iterFile != updateInfo.filesToCopyOver.end();
			++iterFile )
	{
		// If this file is the updater
		if( *iterFile == PTDefines::WUPDATER_EXE_FILE )
		{
			updateUpdater = true;
			continue;
		}
		
		// Get the destination file path
		std::wstring sDestFileName = sAppDir + *iterFile;

		// Get the downloaded file path
		std::wstring sUpdateFile = sAppDir + PTDefines::UPDATE_FOLDER + *iterFile;

		// Copy this file over (Overwrite if needed)
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Copying from %s to %s", sUpdateFile.c_str(), sDestFileName.c_str() );
		if( CopyFile( sUpdateFile.c_str(), sDestFileName.c_str(), FALSE ) == 0 )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to copy from %s to %s", sUpdateFile.c_str(), sDestFileName.c_str() );
			allFilesCopied = false;
			break;
		}
		else
			copiedFiles.push_back( *iterFile );
	}

	// If all files were copied then return success
	if( allFilesCopied )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Update files successfully copied" );

		// If the updater needs updating
		if( updateUpdater )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Updating updater EXE" );

			// Create the batch file to overwrite the updater
			std::wstring sNewUpdateFile = sAppDir + PTDefines::UPDATE_FOLDER + PTDefines::WUPDATER_EXE_FILE;
			std::wstring sOverwriteBat = WriteOverwriteBatch(sNewUpdateFile.c_str());

			// Run the batch file to overwrite the updater as soon as possible
			RunFile( sOverwriteBat.c_str() );
		}

		return true;
	}

	// All of the files were not copied so delete the copied files
	for( StringList::const_iterator iterFile = copiedFiles.begin(); iterFile != copiedFiles.end(); ++iterFile )
	{
		// Get the destination file path
		std::wstring sCopiedFileName = sAppDir + *iterFile;

		// Delete the file
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Deleting copied file %s, because copy step failed", sCopiedFileName.c_str() );
		DeleteFile( sCopiedFileName.c_str() );
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RemoveOldFiles  Global
///
///	\param updateInfo The game update info
///	\returns True if all of the update files were deleted along with the downloads folder, false
///				otherwise
///
///	Remove the temporarily renamed backup files and the update folder and its files.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RemoveOldFiles( const GameUpdateInfo& updateInfo )
{
	std::wstring sAppDir = TCBase::GetAppPath();

	// Delete the files that were copied over from the updates directory
	for( StringList::const_iterator iterFile = updateInfo.filesToCopyOver.begin();
				iterFile != updateInfo.filesToCopyOver.end();
				++iterFile )
	{
		// If this file is the updater then skip it since the batch file will take care of it
		if( *iterFile == PTDefines::WUPDATER_EXE_FILE )
			continue;

		// Get the temporary file name
		std::wstring sTempName = sAppDir + TEMP_FILE_PREFIX + *iterFile;

		// Delete the file
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Deleting old file %s", sTempName.c_str() );
		DeleteFile( sTempName.c_str() );
	}

	// Delete the download folder
	std::wstring dlFolder = sAppDir + PTDefines::UPDATE_FOLDER;
	TCBase::DeleteDir( dlFolder.c_str() );

	// Return success
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ApplyUpdate  Global
///
///	\param updateInfo The object storing what files to copy to the game directory
///
///	Update the game files with the downloaded files.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ApplyUpdate( GameUpdateInfo updateInfo )
{
	// Rename the local files
	StringList renamedFiles;
	if( !RenameFiles( updateInfo, renamedFiles ) )
	{
		// Revert the file names
		RevertFileNames( renamedFiles );
		g_sResultsDlgMsg += L"Update cannot complete because not all local files could be modified (One or more may be in use)";
		return;
	}

	// Copy the downloaded files into the application directory
	if( !CopyFiles( updateInfo ) )
	{
		RevertFileNames( renamedFiles );
		g_sResultsDlgMsg += L"Update cannot complete because not all local files could be modified (One or more may be in use)";
		return;
	}

	// Remove the old files
	RemoveOldFiles( updateInfo );

	g_sResultsDlgMsg += L"Prime Time has been successfully updated.";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RunFile()  Global
///
///	Create a new process and run the file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void RunFile( const wchar_t* szFile )
{
	// Determine if there is a possible update then attempt to run the auto-updater in update mode
	//const uint32 PARAM_BUF_SIZE = 128;
	//char szParam[PARAM_BUF_SIZE] = {0};
	
	// Run the auto-updater in silent mode
	//strcpy_s( szParam, PARAM_BUF_SIZE, PTDefines::UPDATER_OPTION_SILENT );

	// Start the autp-updater
	STARTUPINFO startupInfo;
	memset( &startupInfo, 0, sizeof(startupInfo) );
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION procInfo;
	memset( &procInfo, 0, sizeof(procInfo) );

	CreateProcess( szFile, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &procInfo );
}


std::wstring WriteOverwriteBatch( const wchar_t* szNewUpdaterFile )
{
	std::wstring sBatFileName = TCBase::GetTmpFileName(L"bat");

	// Open the file
	std::wofstream outFile( sBatFileName.c_str() );
	if( !outFile )
		return std::wstring();

	// Write the batch file
	std::wstring sOldUpdaterFile = TCBase::GetAppPath() + PTDefines::WUPDATER_EXE_FILE;
	const uint32 NUM_LINES = 6;
	std::wstring sLines[NUM_LINES];
	int lineIndex = 0;
	sLines[lineIndex++] = L":Repeat";

	// Add a delay to avoid being a CPU hog
	sLines[lineIndex++] = L"ping -n 1 -w 500 1.1.1.1 >NUL";
	
	// Remove the old updater
	sLines[lineIndex] = L"del \"";
	sLines[lineIndex] += sOldUpdaterFile;
	sLines[lineIndex++] += L"\"";

	// If the file still exists the try again
	sLines[lineIndex] = L"if exist \"";
	sLines[lineIndex] += sOldUpdaterFile;
	sLines[lineIndex++] += L"\" goto Repeat";

	// Move the new file to the old file
	sLines[lineIndex] = L"move /y \"";
	sLines[lineIndex] += sOldUpdaterFile;
	sLines[lineIndex] += L"\" \"";
	sLines[lineIndex] += szNewUpdaterFile;
	sLines[lineIndex++] += L"\"";

	// Delete this batch file
	sLines[lineIndex] = L"del \"";
	sLines[lineIndex] += sBatFileName;
	sLines[lineIndex] += L"\"";


	for( int lineIndex = 0; lineIndex < NUM_LINES; ++lineIndex )
		outFile << sLines[lineIndex].c_str() << std::endl;

	// Close the file
	outFile.close();

	return sBatFileName;
}