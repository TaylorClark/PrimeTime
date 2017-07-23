/*=================================================================================================

	\file MsgLogger.h
	Base Library
	Message Logger Header
	\author Taylor Clark
	\Date April 30, 2006

	This header contains the definitions for the classes that manages message logging.

=================================================================================================*/

#pragma once
#ifndef __MsgLogger_H
#define __MsgLogger_H

#include <iostream>
#include <string>
#include <list>
#include <sstream>


//-------------------------------------------------------------------------------------------------
///
/// \class MsgLogger
///
///	This class encapsulates sending error messages out to a log file.
///
//-------------------------------------------------------------------------------------------------
class MsgLogger
{
public:
	enum MsgImportance
	{
		MI_Note = 0,
		MI_Warning,
		MI_Error,
		MI_CriticalError,
		MI_Todo,
		MI_COUNT
	};

private:
	typedef wchar_t charT;

	
	//---------------------------------------------------------------------------------------------
	///
	/// \class Listener
	///
	///	The base class for message listeners.
	///
	//---------------------------------------------------------------------------------------------
	class Listener
	{
	public:

		/// The destructor
		virtual ~Listener() {}

		/// Output a message
		virtual void Output( const std::wstring& sMsg ) = 0;
	};

	
	//---------------------------------------------------------------------------------------------
	///
	/// \class ListenerStream
	///
	///	The listener class for outputting messages to a file.
	///
	//---------------------------------------------------------------------------------------------
	class ListenerStream : public Listener
	{
		/// The output file
		std::basic_ostream<charT>* m_pOutStream;

		/// If the stream is a file stream
		bool isFile;

	public:

		/// Initialize the stream using a file
		ListenerStream( const wchar_t* szFileName, bool appendAndClampSize );

		/// Initialize the stream using an output stream
		ListenerStream( std::basic_streambuf< charT >* pCharBuf );

		/// The destructor
		~ListenerStream();

		/// Size down the log file if it is too big
		void MaintainLogFileSize( const wchar_t* szFileName );

		/// Output a message
		virtual void Output( const std::wstring& sMsg );
	};

#ifdef WINDOWS
	//---------------------------------------------------------------------------------------------
	///
	/// \class ListenerWindow
	///
	///	The listener class for outputting messages to a file.
	///
	//---------------------------------------------------------------------------------------------
	class ListenerWindow : public Listener
	{
		/// The output window
		void* m_pWindow;

	public:

		/// The basic constructor
		ListenerWindow( void* pWindow );

		/// Output a message
		virtual void Output( const std::wstring& sMsg );
	};
#endif

private:

	/// The list of listeners to output messages to
	typedef std::list< Listener* > ListenList;
	ListenList m_Listeners;

	/// Output an error to the log file
	void OutputErrorString( const wchar_t* szErrorStr );

	/// Output the log initiating string to a listener
	void WriteLogInit( Listener* pListener );

	/// The default constructor, private because this class follows the singleton patter
	MsgLogger(){}

public:

	/// The accessor to retrieve the one and only instance
	static MsgLogger& Get()
	{
		static MsgLogger s_Logger;
		return s_Logger;
	}

	/// The destructor closes the log file if it is open
	~MsgLogger();

	/// Close all of the output streams
	void Term()
	{
		// Free the listeners
		for( ListenList::iterator iterListener = m_Listeners.begin(); iterListener != m_Listeners.end(); ++iterListener )
			delete *iterListener;
		m_Listeners.clear();
	}

#ifdef WINDOWS
	/// Add a listener that outputs to a window
	void AddListener( void* pWindow )
	{
		// Ensure a non-NULL pointer
		if( !pWindow )
			return;

		// Attempt to create the window
		ListenerWindow* pNewListener = NULL;
		try
		{
			pNewListener = new ListenerWindow( pWindow );
		}
		catch(...){}

		// If a window was created then add it to our listener list
		if( pNewListener )
			m_Listeners.push_back( pNewListener );
		WriteLogInit( pNewListener );
	}
#endif

	/// Add a listener that outputs to a stream, provided to be used with the standard output streams
	void AddListener( std::basic_streambuf< charT >* pBuf )
	{
		if( !pBuf )
			return;
		
		// Create the stream listener
		Listener* pNewListener = new ListenerStream( pBuf );
		m_Listeners.push_back( pNewListener );
		WriteLogInit( pNewListener );
	}

	/// Add a listener that outputs to a file
	void AddListener( const wchar_t* szFileName, bool appendAndClampSize = false )
	{
		if( !szFileName )
			return;

		// Create the stream listener
		Listener* pNewListener = new ListenerStream( szFileName, appendAndClampSize );
		m_Listeners.push_back( pNewListener );
		WriteLogInit( pNewListener );
	}

	/// Output a message
	template< class T>
	void Output( T val )
	{
		// Get the string/value into a string
		std::basic_stringstream< wchar_t > outStr( std::ios::out );
		outStr << val;

		// Output the string to the listeners
		OutputErrorString( outStr.str().c_str() );
	}

	/// Output a message
	void Output( MsgImportance level, const wchar_t* szStr, ... );

	/// Output a message
	void OutputInfo( int lineNumber, const wchar_t* szFile, MsgImportance level, const wchar_t* szStr, ... );
};

/// The wide file define
#ifndef __WFILE__
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#endif

#define MSG_LOGGER_OUT( level, szStr, ... ) MsgLogger::Get().OutputInfo( __LINE__, __WFILE__, level, szStr, ## __VA_ARGS__ )
//template<>
//MsgLogger& operator << ( MsgLogger& logger, const char* szStr );


#endif // __MsgLogger_H