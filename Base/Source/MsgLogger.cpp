/*=================================================================================================

	\file MsgLogger.cpp
	Base Library
	Message Logger Source
	\author Taylor Clark
	\Date April 30, 2006

	This source file contains the implementation of the message logging class.

=================================================================================================*/

#include "../MsgLogger.h"
#include "../Types.h"
#include "../StringFuncs.h"
#include <time.h>
#include <fstream>
#include "../Types.h"
#include <stdio.h>
#include <stdarg.h>
#include "../CriticalSection.h"
using namespace std;


// The buffer used for forming messages
static const uint32 g_MSG_BUFFER_SIZE = 1023;
//static wchar_t g_szMsgBuffer[g_MSG_BUFFER_SIZE + 1] = {0};

// The max size allowed for the log file, must be greater than 2k so resizing doesn't have
// issues
const uint32 MIN_LOG_SIZE = 1024 * 2;
const uint32 MAX_LOG_SIZE = 1024 * 10;

/// Get a description string based on the message importance level
static const wchar_t* GetLevelString( MsgLogger::MsgImportance msgImportance );

TCBase::CriticalSection g_MsgLoggerCritSec;
#ifdef WINDOWS

#include <windows.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ListenerWindow::ListenerWindow  Public
///
///	The constructor to create a listener that outputs its messages to a file stream.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MsgLogger::ListenerWindow::ListenerWindow( void* pWindow )
{
	// Verify the data
	if( !pWindow )
		return;//throw L"Can't create ListenerWindow with NULL pointer.";
	if( !IsWindow( (HWND)pWindow ) )
		return;//throw L"Can't create ListenerWindow with a pointer that is not window.";

	// Store the pointer
	m_pWindow = pWindow;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ListenerWindow::Output  Public
///
///	Output a message to a listener window object.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgLogger::ListenerWindow::Output( const std::wstring& sMsg )
{
	// Ensure the window is still valid
	if( !IsWindow( (HWND)m_pWindow ) )
		return;

	// Get the necessary size of the buffer
	const wchar_t* NEW_LINE = L"\r\r\n";

	//uint32 bufferLen = (uint32)SendMessage( (HWND)m_pWindow, WM_GETTEXTLENGTH, 0, 0 );
	uint32 bufferLen = GetWindowTextLength( (HWND)m_pWindow );
	bufferLen += (uint32)wcslen( sMsg.c_str() );
	bufferLen += (uint32)wcslen( NEW_LINE );
	bufferLen++;

	// Allocate a buffer for the text
	wchar_t* szNewWinText = new wchar_t[ bufferLen ];

	// Get the window text
	//SendMessage( (HWND)m_pWindow, WM_GETTEXT, bufferLen, (LPARAM)szNewWinText );
	GetWindowTextW( (HWND)m_pWindow, szNewWinText, bufferLen );

	// Add on the new line then the new text
	wcscat_s( szNewWinText, bufferLen, NEW_LINE );
	wcscat_s( szNewWinText, bufferLen, sMsg.c_str() );

	// Set the text
	SetWindowTextW( (HWND)m_pWindow, szNewWinText );

	// Free the buffer
	delete [] szNewWinText;

	// Get the number of lines
	int numLines = (int)SendMessage( (HWND)m_pWindow, EM_GETLINECOUNT, 0, 10 );
	SendMessage( (HWND)m_pWindow, EM_LINESCROLL, 0, (LPARAM)numLines );
}

#endif // WIN32

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ListenerStream::ListenerStream  Public
///
///	The constructor to create a listener that outputs its messages to a file stream.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MsgLogger::ListenerStream::ListenerStream( const wchar_t* szFileName, bool appendAndClampSize  )
{
	// This is a file stream
	isFile = true;

	// If the file is to be appended to and the size clamped
	if( appendAndClampSize )
	{
		// Open the file
		m_pOutStream = new std::wofstream( TCBase::Narrow(szFileName).c_str(), ios_base::out | ios_base::app );

		// Make sure the file isn't too big
		MaintainLogFileSize( szFileName );
	}
	// Else just open, overwriting if it already exists, the file
	else
		m_pOutStream = new std::wofstream( TCBase::Narrow(szFileName).c_str(), ios_base::out | ios_base::trunc );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ListenerStream::ListenerStream  Public
///
///	The constructor to initialize a listener from a output stream.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MsgLogger::ListenerStream::ListenerStream( std::basic_streambuf< charT >* pCharBuf )
{
	// This is not a file stream
	isFile = false;

	/// Create the stream
	m_pOutStream = new std::basic_ostream< charT >( pCharBuf );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ListenerStream::Output  Public
///
///	Output a message to a listener stream object.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgLogger::ListenerStream::Output( const std::wstring& sMsg )
{
	if( !m_pOutStream )
		return;

	*m_pOutStream << sMsg.c_str() << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ListenerStream::~ListenerStream  Public
///
///	The default destructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MsgLogger::ListenerStream::~ListenerStream()
{
	if( !m_pOutStream )
		return;

	// If this output stream is a file stream
	if( isFile )
	{
		std::ofstream* pOutFile = (std::ofstream*)m_pOutStream;
		pOutFile->close();
	}

	delete m_pOutStream;
	m_pOutStream = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MsgLogger::Output  Public
///
///	\param UNNAMED The importance level of the message
///	\param szStr The error string to output
///
///	Output an error string to the listeners using a similar format as printf().
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgLogger::Output( MsgImportance, const wchar_t* szStr, ... )
{
	static wchar_t _szMsgBuffer[ 1024 ] = {0};

	// Create the string
	va_list varArgs;
	va_start( varArgs, szStr );
	vswprintf( _szMsgBuffer, g_MSG_BUFFER_SIZE, szStr, varArgs );
	va_end( varArgs );

	// Output the string to the listeners
	OutputErrorString( _szMsgBuffer );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MsgLogger::Output  Public
///
///	\param lineNumber The line number of this message
///	\param szFile The source file containing the message
///	\param level The importance level of the message
///	\param szStr The error string to output
///
///	Output an error string to the listeners using a similar format as printf().
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgLogger::OutputInfo( int lineNumber, const wchar_t* szFile, MsgImportance level, const wchar_t* szStr, ... )
{
	static wchar_t s_szMsgBuffer[ 1024 ] = {0};

	// Create the message string
	va_list varArgs;
	va_start( varArgs, szStr );
	vswprintf( s_szMsgBuffer, g_MSG_BUFFER_SIZE, szStr, varArgs );
	va_end( varArgs );

	// Get the length of the message string
	size_t strLen = wcslen( s_szMsgBuffer );

	// Append the file info
	swprintf( (wchar_t*)(s_szMsgBuffer + strLen), (g_MSG_BUFFER_SIZE - strLen) + 1, L" (%s: %s on line %d)", GetLevelString(level), szFile, lineNumber );

	// Output the string to the listeners
	OutputErrorString( s_szMsgBuffer );	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MsgLogger::OutputErrorString  Public
///
///	\param pListener The listener to which we are writing the log initiation string
///
///	Output the log initiating string to a listener.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgLogger::WriteLogInit( Listener* pListener )
{
	if( !pListener )
		return;

	const wchar_t* LOG_INIT_STR = L"----- Log started at ";

	// If we are adding a new line then preface the error with data/time
	std::wstring sError = LOG_INIT_STR;
	
	// Create the output string
	time_t curTime = time(NULL);
	wchar_t szTimeDate[128] = {0};
#ifdef WIN32
	tm timeInfo;
	localtime_s( &timeInfo, &curTime );

	wcsftime(szTimeDate, 128, L"%X on %x", &timeInfo );
#else
	wcsftime(szTimeDate, 128, L"%x on %X", localtime( &curTime ) );
#endif
	sError += szTimeDate;
	sError += L" -----";

	// Output the string
	pListener->Output( sError );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MsgLogger::MsgLogger  Public
///
///	The default constructor
///
///////////////////////////////////////////////////////////////////////////////////////////////////
//MsgLogger::MsgLogger()
//{
//	// Create the critical section
//	InitializeCriticalSection( &g_MsgLoggerCritSec );
//}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MsgLogger::~MsgLogger  Public
///
///	The destructor closes the log file if it is open.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MsgLogger::~MsgLogger()
{
	Term();

	// Free the critical section
	//DeleteCriticalSection( &g_MsgLoggerCritSec );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MsgLogger::OutputErrorString  Public
///
///	\param errorStr The error string to output
///
///	Output an error string to the log file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgLogger::OutputErrorString( const wchar_t* szErrorStr )
{
	std::wstring sMsg( szErrorStr );

	g_MsgLoggerCritSec.Enter();
	//EnterCriticalSection( &g_MsgLoggerCritSec ) ;
	
	// Go through each listener and output the string
	for( ListenList::iterator iterListener = m_Listeners.begin(); iterListener != m_Listeners.end(); ++iterListener )
		(*iterListener)->Output( sMsg );

	// Free the mutex
	g_MsgLoggerCritSec.Leave();
	//LeaveCriticalSection( &g_MsgLoggerCritSec );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ListenerStream::MaintainLogFileSize  Public
///
///	Size down the log file if it is too big.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgLogger::ListenerStream::MaintainLogFileSize( const wchar_t* szFileName )
{
	if( !m_pOutStream || !isFile )
		return;

	// Get the size of the file
	std::wofstream& outFile = *(std::wofstream*)m_pOutStream;
	outFile.seekp( 0, ios_base::end );
	uint32 fileSize = (uint32)outFile.tellp();

	// If the file isn't too big then bail
	if( fileSize < MAX_LOG_SIZE )
		return;

	// Close the file
	outFile.close();

	// Open the file for input
	std::wifstream logFileInStream( TCBase::Narrow(szFileName).c_str(), std::ios_base::in );

	// If we failed to open the file then bail
	if( !logFileInStream )
		return;

	// Get to the point in the file that is 1k less than the max
	uint32 splitFileSize = MAX_LOG_SIZE - 1024;
	uint32 splitFilePos = fileSize - splitFileSize;
	logFileInStream.seekg( splitFilePos, ios_base::beg );

	// Read until a new line character
	wchar_t charBuffer[1] = {0};
	logFileInStream.read( charBuffer, 1 );
	while( charBuffer[0] != '\n' && logFileInStream.good() )
		logFileInStream.read( charBuffer, 1 );

	// If no new line character was found then start reading at the split file position
	if( !logFileInStream.good() )
		logFileInStream.seekg( splitFilePos, ios_base::beg );
	// Else get the current position
	else
	{
		// Get the position in the file to start reading
		uint32 curFilePos = (uint32)logFileInStream.tellg();
		splitFileSize = fileSize - curFilePos;		
	}

	// Read in the data
	wchar_t* szHalfFile = new wchar_t[ splitFileSize ];
	logFileInStream.read( szHalfFile, splitFileSize );

	// Close the file for reading
	logFileInStream.close();

	// Clear the log file and clear it
	outFile.open( TCBase::Narrow(szFileName).c_str(), ios_base::out | ios_base::trunc );
	
	// Write out a line indicating the resize
	std::wstring sResize = L"----- Log file resized on ";
	
	time_t curTime = time(NULL);
	wchar_t szTimeDate[128] = {0};
#ifdef WIN32
	tm timeInfo;
	localtime_s( &timeInfo, &curTime );

	wcsftime(szTimeDate, 128, L"%c", &timeInfo );
#else
	wcsftime(szTimeDate, 128, L"%c", localtime( &curTime ) );
#endif
	sResize += szTimeDate;
	sResize += L" -----\n";
	
	// Write out the string
	outFile.write( sResize.c_str(), (streamsize)sResize.length() );

	// Write the last half of the old file
	outFile.write( szHalfFile, splitFileSize );
	
	// Free the buffer
	delete [] szHalfFile;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GetLevelString  Global
///
///	Get a description string based on the message importance level.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* GetLevelString( MsgLogger::MsgImportance msgImportance )
{
	switch( msgImportance )
	{
	case MsgLogger::MI_Note:
		return L"Note";
		
	case MsgLogger::MI_Warning:
		return L"Warning";
		
	case MsgLogger::MI_Error:
		return L"Error";

	case MsgLogger::MI_CriticalError:
		return L"CRITICAL ERROR";

	case MsgLogger::MI_Todo:
		return L"To Do";
            
    default:
        // Stop warnings on mac
        break;
	}

	return L"Unknown Error Level";
}