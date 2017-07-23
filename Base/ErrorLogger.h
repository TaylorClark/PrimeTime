/*=================================================================================================

	ErrorLogger.h
	\author Taylor Clark
	\Date October 12, 2005

	This header contains the definitions for the classes that manages error logging.

=================================================================================================*/

#pragma once
#ifndef __ErrorLogger_H
#define __ErrorLogger_H

#include <iostream>
#include <fstream>
#include <string>


//-------------------------------------------------------------------------------------------------
///
/// \class ErrorLogger
///
///	This class encapsulates sending error messages out to a log file.
///
//-------------------------------------------------------------------------------------------------
class ErrorLogger// : std::basic_ostream< char >
{
	typedef char charT;
	typedef std::char_traits<char> traits;

private:

	/// The output file stream
	std::ofstream m_OutStream;

	/// Size down the log file if it is too big
	void MaintainLogFileSize();

public:

	ErrorLogger()// : std::basic_ostream< charT, traits >( new std::basic_stringbuf<charT, traits>( ios_base::out ) )
	{
	}

	/// The destructor closes the log file if it is open
	~ErrorLogger()
	{
		if( m_OutStream.is_open() )
			m_OutStream.close();
	}

	/// Output an error to the log file
	void OutputErrorString( std::string errorStr, bool addNewLine = true );

	/// Output an error to the log file
	void OutputErrorString( const char* szErrorStr, bool addNewLine = true )
	{
		OutputErrorString( std::string(szErrorStr), addNewLine );
	}

	template<class T>
	ErrorLogger& operator << ( T val )
	{
		std::cout << val;
		return *this;
	}

	template<class T>
	ErrorLogger& operator << ( const char* szStr )
	{
		OutputErrorString( szStr, false );
		std::cout << szStr;
		return *this;
	}

	template<class T>
	ErrorLogger& operator << ( char* szStr )
	{
		OutputErrorString( szStr, false );
		std::cout << szStr;
		return *this;
	}

	template<class T>
	ErrorLogger& operator << ( std::string sStr )
	{
		OutputErrorString( sStr, false );
		std::cout << sStr;
		return *this;
	}
};



//template<>
//ErrorLogger& operator << ( ErrorLogger& logger, const char* szStr );


#endif // __ErrorLogger_H