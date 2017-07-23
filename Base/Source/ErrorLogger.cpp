/*=================================================================================================

	ErrorLogger.cpp
	Gamers Floating Profile Saver
	\author Taylor Clark
	\Date October 12, 2005

=================================================================================================*/

#include <time.h>
#include "ErrorLogger.h"
using namespace std;


// The file name for the log file, should be in the same directory as the executing app
static const char* FILE_NAME = "errors.log";

// The max size allowed for the log file, must be greater than 2 so resizing doesn't have
// issues
#define MAX_LOG_SIZE 8192



///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ErrorLogger::OutputErrorString  Public
///
///	\param errorStr The error string to output
///	\param addNewLine If we should add a new line after this line, defaults to true
///
///	Output an error string to the log file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ErrorLogger::OutputErrorString( std::string errorStr, bool addNewLine )
{
	// Have we opened the file yet
	if( !m_OutStream.is_open() )
	{
		// Open the file
		m_OutStream.open( FILE_NAME, ios_base::out | ios_base::app );

		// Make sure the file isn't too big
		MaintainLogFileSize();
	}

	// If we are adding a new line then preface the error with data/time
	std::string sError;
	if( addNewLine )
	{
		time_t curTime = time(NULL);
		char szTimeDate[128] = {0};
		strftime(szTimeDate, 128, "%F at %T", localtime( &curTime ) );
		
		sError = szTimeDate;
		sError += "-";
	}

	// Add in the error string
	sError += errorStr;
	
	// If we are adding a new line
	if( addNewLine )
		sError += "\n";

	// Output the string
	m_OutStream.write( sError.c_str(), (std::streamsize)sError.length() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ErrorLogger::MaintainLogFileSize  Private
///
///	Size down the log file if it is too big.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ErrorLogger::MaintainLogFileSize()
{
	// Get the size of the file
	m_OutStream.seekp( 0, ios_base::end );
	int fileSize = (int)m_OutStream.tellp();

	// If the file isn't too big then bail
	if( fileSize < MAX_LOG_SIZE )
		return;

	// Close the file
	m_OutStream.close();

	// Open the file for input and output
	ifstream logFileStream;
	logFileStream.open( FILE_NAME );

	// If we failed to open the file then bail
	if( !logFileStream )
		return;

	// Get to the half way point in the file
	int halfFilePos = (fileSize / 2);
	logFileStream.seekg( halfFilePos, ios_base::beg );

	// Read until a new line character
	fstream::char_type charBuffer[1] = {0};
	logFileStream.read( charBuffer, 1 );
	while( charBuffer[0] != '\n' && logFileStream.good() )
		logFileStream.read( charBuffer, 1 );

	// If we failed to find the new line character
	fstream::char_type *szHalfFile;
	int bufferSize = 0;
	if( !logFileStream.good() )
	{
		// Read in half of the file
		bufferSize = halfFilePos;
		szHalfFile = new fstream::char_type[ halfFilePos ];
		logFileStream.seekg( halfFilePos, ios_base::beg );
		logFileStream.read( szHalfFile, halfFilePos );
	}
	// Else copy the last half of the file
	else
	{
		// Get the position in the file to start reading
		int filePos = logFileStream.tellg();
		bufferSize = fileSize - filePos;

		// Read in the data
		szHalfFile = new fstream::char_type[ bufferSize ];
		logFileStream.read( szHalfFile, bufferSize );
	}

	// Close the file for reading
	logFileStream.close();

	// Clear the log file and clear it
	m_OutStream.open( FILE_NAME, ios_base::out | ios_base::trunc );

	// Write the last half of the old file
	m_OutStream.write( szHalfFile, bufferSize );

	// Free the buffer
	delete [] szHalfFile;

	// Write out a line indicating the resize
	std::string sResize = "\n----- File resized on ";
		
	time_t curTime = time(NULL);
	char szTimeDate[128] = {0};
	strftime(szTimeDate, 128, "%F at %T", localtime( &curTime ) );
	
	sResize += szTimeDate;	
	
	sResize += " -----\n";

	// Write out the string
	m_OutStream.write( sResize.c_str(), (int)sResize.length() );
}