#ifndef __UpdateMgr_h
#define __UpdateMgr_h
#pragma once

#include <afxinet.h>
#include "Base/DataBlock.h"
#include "Base/Types.h"
#include <string>
#include <sstream>


//-------------------------------------------------------------------------------------------------
/*!
	\class FileDownloader
	\brief Represents an object that can download files from a URL and report back the progress.
*/
//-------------------------------------------------------------------------------------------------
class FileDownloader
{
public:

	typedef void (ProgressCB)( uint32 bytesDownloaded, uint32 totalSize );

private:

	/// The file data buffer
	uint8* m_pFileData;

	/// The file data length
	uint32 m_FileDataLen;

	/// If the downloader is currently downloading
	bool m_IsDownloading;

	/// The last error string
	std::wstring m_sErrorStr;

	typedef std::basic_ostream< uint8 > OutputStream;

	/// Download a file to a storage object
	bool DownloadToStream( const wchar_t* szSrcFile, ProgressCB* pProgressCB, OutputStream* pOutStream = 0, uint32 expectedSize = 0 );

public:

	/// The default constructor
	FileDownloader() : m_FileDataLen( 0 ),
						m_IsDownloading( false ),
						m_pFileData( 0 )
	{}

	~FileDownloader(){ Free(); }

	/// Download a file synchronously (But still pumps messages)
	bool DownloadToBuffer( const wchar_t* szSrcFile, ProgressCB* pProgressCB = 0, uint32 expectedSize = 0 );

	bool DownloadToFile( const wchar_t* szSrcFile, const wchar_t* szDestFile, ProgressCB* pProgressCB = 0, uint32 expectedSize = 0 );

	/// Store the downloaded data to a file
	void StoreBufferToFile( const wchar_t* szDestFile );

	/// Get the file data
	DataBlock GetFileData() const { return DataBlock( m_pFileData, m_FileDataLen ); }

	/// Get the last occurred error string
	const wchar_t* GetLastErrorStr() const { return m_sErrorStr.c_str(); }

	/// Free the downloaded data
	void Free();
};

#endif // __UpdateMgr_h