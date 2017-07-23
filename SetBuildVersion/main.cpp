#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>


// The string to find in the file
static const wchar_t* DEFINE_STR = L"const uint32 PT_VERSION = ";


int wmain( int argc, wchar_t* argv[] )
{
	// Ensure there are arguments
	if( argc < 3 )
	{
		std::wcout << L"Ensure that the path to the main.cpp is passed as the first parameter then the build number as the second." << std::endl;
		return 0;
	}

	// Store the build number
	if( _wtoi( argv[2] ) == 0 )
	{
		std::wcout << L"The specified build number is invalid." << std::endl;
		return 0;
	}

	// Ensure the file exists
	if( _waccess_s( argv[1], 0 ) != 0 )
	{
		std::wcout << L"The specified file does not exist." << std::endl;
		return 0;
	}

	// Open the file
	std::wfstream mainFileStream( argv[1] );
	if( !mainFileStream )
	{
		std::wcout << L"Failed to open specified file." << std::endl;
		return 0;
	}

	// Read the file until the #define is found
	const std::wfstream::streamoff DEFINE_STR_LEN = (std::wfstream::streamoff)wcslen( DEFINE_STR );
	const unsigned int READ_BUF_LEN = 256;
	wchar_t szReadBuffer[READ_BUF_LEN] = {0};
	while( mainFileStream.good() )
	{
		// Store the line position
		std::wfstream::pos_type linePos = mainFileStream.tellg();

		// Read the line
		mainFileStream.getline( szReadBuffer, READ_BUF_LEN );

		// If this is the line
		if( wcsncmp( DEFINE_STR, szReadBuffer, DEFINE_STR_LEN ) == 0 )
		{
			// Get to the position in the line where the number would be
			mainFileStream.seekp( linePos + DEFINE_STR_LEN );
			mainFileStream.write( argv[2], 2 );
			break;
		}
	}

	std::wcout << L"Build version in '" << argv[1] << "' successfully set to " << argv[2] << std::endl;

	// Close the file and bail
	mainFileStream.close();
	return 1;
}