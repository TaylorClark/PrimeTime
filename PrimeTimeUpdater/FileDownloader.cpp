#include "FileDownloader.h"
#include <fstream>
#include "md5.h"
#include <streambuf>
#include "Base/DynamicBuffer.h"
#include "Base/MsgLogger.h"

static const wchar_t* FTP_USER = L"ptdownload";
static const wchar_t* FTP_PASS = L"update";

const wchar_t* szAppName = L"PrimeTimeUpdater";

static CInternetSession g_InternetSess( szAppName );


// A simple structure to encapsulate the data needed to download any file and clean up
struct InternetFileInfo
{
	// The default constructor
	InternetFileInfo() : pFile(0),
						pFtpConn(0)
	{}

	// The file stream that can download the requested file
	CStdioFile* pFile;

	// The FTP connection stream if FTP is the protocol used to transfer data
	CFtpConnection* pFtpConn;

	// Close the file and connection stream
	void CloseAll()
	{
		// If a file is open
		if( pFile )
		{
			pFile->Close();
			delete pFile;
			pFile = 0;
		}

		// If there is an FTP connection
		if( pFtpConn )
		{
			pFtpConn->Close();
			delete pFtpConn;
			pFtpConn = 0;
		}
	}

};

/// Open a file that can be downloaded
InternetFileInfo GetInternetStream( const wchar_t* szSrcFile );


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileDownloader::StoreBufferToFile  Public
///
///	\param szDestFile The file name in which to store the data
///
///	Store the downloaded data to a file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDownloader::StoreBufferToFile( const wchar_t* szDestFile )
{
	// Ensure valid data
	if( !szDestFile || !m_pFileData || m_FileDataLen == 0 )
		return;

	// Open the output file
	std::ofstream outFile( szDestFile, std::ios_base::out | std::ios_base::binary );
	if( !outFile )
		return;
	
	// Write the data to file
	outFile.write( (const char*)m_pFileData, (std::streamsize)m_FileDataLen );

	// Close the file
	outFile.close();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileDownloader::Free  Public
///
///	Free the downloaded data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDownloader::Free()
{
	m_FileDataLen = 0;
	if( m_pFileData )
		delete [] m_pFileData;
	m_pFileData = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileDownloader::DownloadToBuffer  Public
///
///	\param szSrcFile The file to download including the protocol prefix (i.e. http://, ftp://,
///			file://_
///	\param pProgressCB The callback function that receives the file download progress
///
///	Download a file into a buffer in memory.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool FileDownloader::DownloadToBuffer( const wchar_t* szSrcFile, ProgressCB* pProgressCB, uint32 expectedSize )
{
	return DownloadToStream( szSrcFile, pProgressCB, 0, expectedSize );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileDownloader::DownloadToFile  Public
///
///	\param szSrcFile The file to download including the protocol prefix (i.e. http://, ftp://,
///			file://_
///	\param pProgressCB The callback function that receives the file download progress
///	\param szDestFile The name of the file into which the data is stored
///
///	Download a file into a local file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool FileDownloader::DownloadToFile( const wchar_t* szSrcFile, const wchar_t* szDestFile, ProgressCB* pProgressCB, uint32 expectedSize )
{
	// Create the output file
	std::basic_ofstream<uint8> outFile;
	outFile.open( szDestFile, std::ios_base::out | std::ios_base::binary );
	if( !outFile )
		return false;

	// Free the buffer
	if( m_pFileData )
		delete [] m_pFileData;
	m_pFileData = 0;

	// Download the file
	bool retVal = DownloadToStream( szSrcFile, pProgressCB, (OutputStream*)&outFile, expectedSize );
	outFile.close();

	// If the download failed then remove the destination file
	if( !retVal )
		DeleteFile( szSrcFile );

	// Return the download success value
	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileDownloader::DownloadToStream  Private
///
///	\param szSrcFile The file to download including the protocol prefix (i.e. http://, ftp://,
///			file://_
///	\param pProgressCB The callback function that receives the file download progress
///	\param pOutStream If the downloaded data is being saved to file then this is the stream into
///						which data is read
///	\param 
///
///	Download a file into a local file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool FileDownloader::DownloadToStream( const wchar_t* szSrcFile, ProgressCB* pProgressCB, OutputStream* pOutStream, uint32 expectedSize )
{
	/*HINTERNET hInternet = InternetOpen( szAppName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	HINTERNET hInetFile = InternetOpenUrl( hInternet, szSrcFile, NULL, 0, 0, 0 );
	if( hInetFile )
	{
		uint8 buffer[128] = {0};
		DWORD bytesRead = 0;
		InternetReadFile( hInetFile, buffer, 128, &bytesRead );

		InternetCloseHandle( hInetFile );
		hInetFile = 0;
	}

	HINTERNET hFtpConn = InternetConnect( hInternet, L"ftp.primetimemath.net", INTERNET_DEFAULT_FTP_PORT, L"anonymous@primetimemath.net", NULL, INTERNET_SERVICE_FTP, 0, 0 );
	DWORD buffLen = MAX_PATH;
	wchar_t buff[MAX_PATH] = {0};
	FtpGetCurrentDirectory( hFtpConn, buff, &buffLen );
	std::wstring sPath = buff;
	sPath += szSrcFile;
	hInetFile = FtpOpenFile( hFtpConn, sPath.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0 );
	if( hInetFile )
	{
		uint8 buffer[128] = {0};
		DWORD bytesRead = 0;
		InternetReadFile( hInetFile, buffer, 128, &bytesRead );

		InternetCloseHandle( hInetFile );
		hInetFile = 0;
	}

	InternetCloseHandle( hFtpConn );
	InternetCloseHandle( hInternet );*/
	
	const UINT READ_AMT = 1024;

	bool finishedSuccessfully = true;
	uint8* pTempFileBuffer = 0;
	DynamicBuffer fileMemBuffer;
	InternetFileInfo downloadInfo;
	try
	{
		// Open the file
		downloadInfo = GetInternetStream( szSrcFile );
		CStdioFile* pFile = downloadInfo.pFile;
		
		// If the file failed to open then bail
		if( !pFile )
		{
			// Return failure
			m_sErrorStr = L"Failed to open file/resolve URL (";
			m_sErrorStr += szSrcFile;
			m_sErrorStr += L")";
			return false;
		}

		// Attempt to get the file size
		if( expectedSize == 0 && false )
		{
			// This is wrapped in a try block because I could not find a way to check if this call
			// would be valid - TWC
			try
			{
				expectedSize = (uint32)pFile->SeekToEnd();
				pFile->SeekToBegin();
			}
			catch(...){}
		}

		// Free the current file data buffer
		if( m_pFileData )
			delete [] m_pFileData;
		m_pFileData = 0;

		// Allocate a temporary buffer into which data will be read
		pTempFileBuffer = new uint8[ READ_AMT ];
			
		// If there is not enough memory free for the temporary buffer
		if( !pTempFileBuffer )
		{
			return false;
		}

		// Set the download flag
		m_IsDownloading = true;

		// Download the file
		m_FileDataLen = 0;
		MSG msg;
		for(;;)
		{
			// Read a chunk of data and if less than the requested amount of data is read then
			// bail
			UINT numBytesRead = pFile->Read( pTempFileBuffer, READ_AMT );
			if( numBytesRead <= 0 )
				break;

			// If the file is being read into a buffer
			if( pOutStream )
				pOutStream->write( pTempFileBuffer, numBytesRead );
			// Else write the data into a memory stream
			else
				fileMemBuffer.Write( pTempFileBuffer, numBytesRead );

			// Store the total number of bytes read
			m_FileDataLen += numBytesRead;

			// Pump messages
			while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
			{
				GetMessage( &msg, NULL, 0, 0 );
				DispatchMessage( &msg );
			}

			// Check if the application should quit
			if( false )
			{
				finishedSuccessfully = false;
				break;
			}

			// Update the progress callback
			if( pProgressCB )
				pProgressCB( m_FileDataLen, expectedSize );

			// Wait to cap the bandwidth usage
			Sleep( 10 );
		}

		// If the file was downloaded unsuccessufully
		if( !finishedSuccessfully )
			Free();
		// Else store the buffer
		else
			m_pFileData = (uint8*)fileMemBuffer.Release();
	}
	catch( CInternetException* pExc )
	{
		// Format the error message
		wchar_t szMsgBuffer[512] = {0};
		pExc->GetErrorMessage( szMsgBuffer, 512 );

		m_sErrorStr = L"Failed to open URL (Exception caught): ";
		m_sErrorStr += szMsgBuffer;

		pExc->Delete();
	}
	
	// Close the internet streams
	downloadInfo.CloseAll();

	// If there is a buffer created then free it
	if( pTempFileBuffer )
		delete [] pTempFileBuffer;
	pTempFileBuffer = 0;

	// Clear the download flag
	m_IsDownloading = false;

	return finishedSuccessfully;
}


InternetFileInfo GetInternetStream( const wchar_t* szSrcFile )
{
	InternetFileInfo retData;

	// Ensure a valid string, it should be at least 7 characters long (ftp://*)
	if( !szSrcFile || wcslen( szSrcFile ) < 7 )
		return retData;

	try
	{
		/// Create the internet session
		g_InternetSess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 3);
		g_InternetSess.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
		g_InternetSess.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);

		// If the URL is an FTP
		const wchar_t* szFTPProtocol = L"ftp://";
		if( _wcsnicmp( szSrcFile, szFTPProtocol, wcslen(szFTPProtocol) ) == 0 )
		{
			// Initialize the URL parse structure
			wchar_t szHostBuffer[ MAX_PATH ] = {0};
			wchar_t szUrlPathBuffer[ MAX_PATH ] = {0};
			URL_COMPONENTS urlInfo;
			memset( &urlInfo, 0, sizeof(urlInfo) );
			urlInfo.dwStructSize = sizeof(urlInfo);
			urlInfo.dwHostNameLength = MAX_PATH;
			urlInfo.lpszHostName = szHostBuffer;
			urlInfo.dwUrlPathLength = MAX_PATH;
			urlInfo.lpszUrlPath = szUrlPathBuffer;

			// Parse the URL
			InternetCrackUrl( szSrcFile, 0, ICU_DECODE, &urlInfo );

			// Create the FTP connection
			retData.pFtpConn = g_InternetSess.GetFtpConnection( szHostBuffer, FTP_USER, FTP_PASS );
			
			// Open the file stream
			if( retData.pFtpConn )
				retData.pFile = retData.pFtpConn->OpenFile( szUrlPathBuffer + 1, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY );
		}
		// Else open the file normally
		else
			retData.pFile = g_InternetSess.OpenURL( szSrcFile, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD );
	}
	catch( CInternetException* pExc )
	{
		std::wstring sMsg;
		// Format the error message
		wchar_t szMsgBuffer[512] = {0};
		pExc->GetErrorMessage( szMsgBuffer, 512 );
		sMsg = L"Caught internet exception in GetInternetStream : ";
		sMsg += szMsgBuffer;

		// Close the handles if needed
		retData.CloseAll();
		MSG_LOGGER_OUT( MsgLogger::MI_Error, sMsg.c_str() );
		pExc->Delete();
	}
	catch(...)
	{
		// Close the handles if needed
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Caught unknown exception in GetInternetStream" );
		retData.CloseAll();
	}

	// Return the opened connection
	return retData;
}