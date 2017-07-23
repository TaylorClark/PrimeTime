//=================================================================================================
/*!
	\file Main.cpp
	Net Message Generator
	Main Source
	\author Taylor Clark
	\date September 21, 2006

	This file contains the main function, the entry point for the application.
*/
//=================================================================================================

#include <string>
#include <fstream>
#include <list>
#include <time.h>
using namespace std;

struct NetMsgMember
{
	wstring sName;
	int size;
	wstring type;
	wstring sComment;
};

typedef list< NetMsgMember > MsgMemberList;

struct NetMsgTemplate
{
	wstring sName;

	wstring sComment;

	MsgMemberList members;

	void Clear()
	{
		sName = L"";
		sComment = L"";
		members.clear();
	}
};

typedef list<NetMsgTemplate> NetMsgList;

NetMsgList ParseFile( const wchar_t* szFile );

void ExportTemplates( NetMsgList& msgTemplates, const wchar_t* szOutFile );

int wmain( int argv, wchar_t* argc[] )
{
	const wchar_t* szInFile = L"msg_templates.txt";
	if( argv > 1 )
		szInFile = argc[1];

	const wchar_t* szOutFile = L"GameNetMsgs.h";
	if( argv > 2 )
		szOutFile = argc[2];
		
	NetMsgList msgTemplates = ParseFile( szInFile );

	ExportTemplates( msgTemplates, szOutFile );
	return 0;
}

struct ParsingData
{
	bool IsInTemplate;
};

bool ParseLine( const wchar_t* szLine, NetMsgMember& member )
{
	int strLen = (int)wcslen( szLine );
	if( strLen == 0 )
		return false;
		
	// Read up to the comma to get the type
	const wchar_t* szCurPos = szLine;
	const wchar_t* szCommaChar = wcschr( szCurPos, L',' );
	if( !szCommaChar )
		return false;
	member.type = wstring( szCurPos, szCommaChar - szCurPos );
	szCurPos = szCommaChar + 1;

	// Read up to the comma for the size
	szCommaChar = wcschr( szCurPos, L',' );
	if( !szCommaChar )
		return false;
	member.size = _wtol( wstring( szCurPos, szCommaChar - szCurPos ).c_str() );
	szCurPos = szCommaChar + 1;

	// Read up to the comma for the name
	szCommaChar = wcschr( szCurPos, L',' );
	if( !szCommaChar )
	{
		if( szCurPos - szLine == strLen )
			return false;
		member.sName = wstring( szCurPos, strLen - (szCurPos - szLine) );
		return true;
	}
	else
		member.sName = wstring( szCurPos, szCommaChar - szCurPos );
	szCurPos = szCommaChar + 1;

	// Get the length of the remaining string which is the comment
	int commentLen = (int)( strLen - (szCurPos - szLine) );
	if( commentLen <= 0 )
		return true;
	member.sComment = wstring( szCurPos, commentLen );

	// Return success
	return true;
}

NetMsgList ParseFile( const wchar_t* szFile )
{
	NetMsgList retList;

	wifstream inFile( szFile );
	if( !inFile )
		return retList;

	ParsingData parseData;
	parseData.IsInTemplate = false;

	const unsigned int STR_BUFF_SIZE = 256;
	wchar_t szStr[ STR_BUFF_SIZE ] = { 0 };
	NetMsgTemplate curTemplate;
	for( ; inFile.good(); )
	{
		inFile.getline( szStr, STR_BUFF_SIZE );
		int strLen = (int)wcslen( szStr );

		// If this line is empty
		if( strLen == 0 )
		{
			if( parseData.IsInTemplate )
			{
				if( curTemplate.sName.size() > 0 )
				{
					retList.push_back( curTemplate );
					curTemplate.Clear();
				}
				parseData.IsInTemplate = false;
			}
			continue;
		}

		// If this line is a comment then bail
		if( strLen > 1 && szStr[0] == L'/' && szStr[1] == L'/' )
			continue;

		// If the current line is not in a template then start a new one
		if( !parseData.IsInTemplate )
		{
			parseData.IsInTemplate = true;
			curTemplate.Clear();

			// Find the end of the line or a comma
			int charIndex = 0;
			while( szStr[charIndex] != L',' && charIndex < strLen )
				++charIndex;

			curTemplate.sName = wstring( szStr, charIndex );

			// If there is a comma
			if( szStr[charIndex++] == L',' )
				curTemplate.sComment = wstring( szStr + charIndex, strLen - charIndex );
		}
		else
		{
			NetMsgMember newMemeber;
			if( ParseLine( szStr, newMemeber ) )
				curTemplate.members.push_back( newMemeber );
		}
	}

	if( parseData.IsInTemplate )
	{
		if( curTemplate.sName.size() > 0 )
		{
			retList.push_back( curTemplate );
			curTemplate.Clear();
		}
		parseData.IsInTemplate = false;
	}

	return retList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ExportTemplates  Global
///
///	\param msgTemplates The list of network message templates
///	\param szOutFile The output file path
///
///	Export the read-in network message templates
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ExportTemplates( NetMsgList& msgTemplates, const wchar_t* szOutFile )
{
	FILE* pOutFile = 0;
	if( _wfopen_s( &pOutFile, szOutFile, L"w" ) != 0 )
		return;

	fwprintf_s( pOutFile, L"//=================================================================================================\n" );
	fwprintf_s( pOutFile, L"/*!\n" );
	fwprintf_s( pOutFile, L"\t\\file GameNetMsgs.h\n" );
	fwprintf_s( pOutFile, L"\tGame Play Library\n" );
	fwprintf_s( pOutFile, L"\tAuto-Generated Game Network Message Header\n" );
	fwprintf_s( pOutFile, L"\t\\author The NetMsgGen application, but that is by Taylor Clark\n" );

	struct tm   newTime;
    time_t      szClock;

    // Get time in seconds
    time( &szClock );

    // Convert time to struct tm form 
    localtime_s( &newTime, &szClock );

	fwprintf_s( pOutFile, L"\t\\date %s\n", _wasctime( &newTime ) );
	fwprintf_s( pOutFile, L"\n" );
	fwprintf_s( pOutFile, L"\tThis file is auto-generated contains the definitions for the game network message objects.  It\n" );
	fwprintf_s( pOutFile, L"\tshould not be editted directly, but rather the source file should be altered and this file\n" );
	fwprintf_s( pOutFile, L"\tregenerated.\n" );
	fwprintf_s( pOutFile, L"*/\n" );
	fwprintf_s( pOutFile, L"//=================================================================================================\n" );
	fwprintf_s( pOutFile, L"\n" );
	fwprintf_s( pOutFile, L"#pragma once\n" );
	fwprintf_s( pOutFile, L"#ifndef __GameNetMsgs_h\n" );
	fwprintf_s( pOutFile, L"#define __GameNetMsgs_h\n" );
	fwprintf_s( pOutFile, L"\n" );
	fwprintf_s( pOutFile, L"#include \"Network/NetMsg.h\"\n" );
	fwprintf_s( pOutFile, L"#include \"GameDefines.h\"\n" );
	fwprintf_s( pOutFile, L"\n" );

	// Generate the registration code
	for( NetMsgList::iterator iterMsg = msgTemplates.begin(); iterMsg != msgTemplates.end(); ++iterMsg )
		fwprintf_s( pOutFile, L"NetworkMgr::Get().RegisterNetMsg( GNMI_%s, NetMsg%s::CreateMsg );\n", iterMsg->sName.c_str(), iterMsg->sName.c_str() );
	fwprintf_s( pOutFile, L"\n" );

	// Write the enum
	fwprintf_s( pOutFile, L"/// The game network message IDs\n" );
	fwprintf_s( pOutFile, L"enum EGameNetMsgIDs\n" );
	fwprintf_s( pOutFile, L"{\n" );
	fwprintf_s( pOutFile, L"\tGNMI_Error = -1,\n" );
	for( NetMsgList::iterator iterTemplate = msgTemplates.begin(); iterTemplate != msgTemplates.end(); ++iterTemplate )
		fwprintf_s( pOutFile, L"\tGNMI_%s,\n", iterTemplate->sName.c_str() );
	fwprintf_s( pOutFile, L"\tGNMI_COUNT\n" );
	fwprintf_s( pOutFile, L"};\n" );
	fwprintf_s( pOutFile, L"\n" );

	// Go through the templates
	for( NetMsgList::iterator iterMsg = msgTemplates.begin(); iterMsg != msgTemplates.end(); ++iterMsg )
	{
		// Get the class name
		wstring sClassName( L"NetMsg" );
		sClassName += iterMsg->sName;

		// Prepend the member names with m_
		for( MsgMemberList::iterator iterMember = iterMsg->members.begin(); iterMember != iterMsg->members.end(); ++iterMember )
		{
			wstring sName( L"m_" );
			sName += iterMember->sName;

			iterMember->sName = sName;
		}

		fwprintf_s( pOutFile, L"\n" );
		fwprintf_s( pOutFile, L"//-------------------------------------------------------------------------------------------------\n" );
		fwprintf_s( pOutFile, L"/*!\n" );
		fwprintf_s( pOutFile, L"\t\\class %s\n", sClassName.c_str() );
		if( iterMsg->sComment.size() > 0 )
			fwprintf_s( pOutFile, L"\t\\brief %s\n", iterMsg->sComment.c_str() );
		fwprintf_s( pOutFile, L"*/\n" );
		fwprintf_s( pOutFile, L"//-------------------------------------------------------------------------------------------------\n" );
		
		fwprintf_s( pOutFile, L"class %s : public NetMsg\n", sClassName.c_str() );
		fwprintf_s( pOutFile, L"{\n" );

		fwprintf_s( pOutFile, L"public:\n" );
		fwprintf_s( pOutFile, L"\n" );

		// Declare the variables
		for( MsgMemberList::const_iterator iterMember = iterMsg->members.begin(); iterMember != iterMsg->members.end(); ++iterMember )
		{
			// If this member has a comment then write it out
			if( iterMember->sComment.size() > 0 )
				fwprintf_s( pOutFile, L"\t/// %s\n", iterMember->sComment.c_str() );
			fwprintf_s( pOutFile, L"\t%s %s;\n", iterMember->type.c_str(), iterMember->sName.c_str() );
			fwprintf_s( pOutFile, L"\n" );
		}

		// Write the constructor
		fwprintf_s( pOutFile, L"\t/// The default constructor\n" );
		fwprintf_s( pOutFile, L"\t%s()\n", sClassName.c_str() );
		fwprintf_s( pOutFile, L"\t{\n" );
		fwprintf_s( pOutFile, L"\t}\n" );
		fwprintf_s( pOutFile, L"\n" );


		// The create message function
		fwprintf_s( pOutFile, L"\t/// Create this message upon receiving data\n" );
		fwprintf_s( pOutFile, L"\tstatic NetMsg* CreateMsg( NetDataBlock dataBlock )\n" );
		fwprintf_s( pOutFile, L"\t{\n" );

		// If this class has no members
		if( iterMsg->members.size() == 0 )
		{
			fwprintf_s( pOutFile, L"\t\tif( dataBlock.GetRemainingBytes() > 0 )\n" );
			fwprintf_s( pOutFile, L"\t\t\treturn 0;\n", sClassName.c_str() );
			fwprintf_s( pOutFile, L"\t\treturn new %s();\n", sClassName.c_str() );
		}
		else
		{
			// Go through the members
			fwprintf_s( pOutFile, L"\t\t// Ensure the data block contains the right amount of data (Calculated during message\n\t\t// generation)\n" );
			fwprintf_s( pOutFile, L"\t\tif( dataBlock.GetRemainingBytes() < " );
			int numBytes = 0;
			for( MsgMemberList::const_iterator iterMember = iterMsg->members.begin(); iterMember != iterMsg->members.end(); ++iterMember )
			{
				if( iterMember->type == L"std::wstring" )
					numBytes += 2;
				else
					numBytes += iterMember->size / 8;
			}
			fwprintf_s( pOutFile, L"%d )\n", numBytes );
			fwprintf_s( pOutFile, L"\t\t\treturn 0;\n" );
			fwprintf_s( pOutFile, L"\n" );

			fwprintf_s( pOutFile, L"\t\t// Create the new message and fill in the data\n" );
			fwprintf_s( pOutFile, L"\t\t%s* pNewMsg = new %s();\n", sClassName.c_str(), sClassName.c_str() );

			for( MsgMemberList::const_iterator iterMember = iterMsg->members.begin(); iterMember != iterMsg->members.end(); ++iterMember )
			{
				if( iterMember->type == L"std::wstring" )
					fwprintf_s( pOutFile, L"\t\tpNewMsg->%s = dataBlock.ReadString();", iterMember->sName.c_str() );
				else if( iterMember->type == L"bool" )
					fwprintf_s( pOutFile, L"\t\tpNewMsg->%s = dataBlock.ReadUint8() != 0;", iterMember->sName.c_str() );
				else
					fwprintf_s( pOutFile, L"\t\tpNewMsg->%s = (%s)dataBlock.ReadUint%d();", iterMember->sName.c_str(), iterMember->type.c_str(), iterMember->size );
				fwprintf_s( pOutFile, L"\n" );
			}
			fwprintf_s( pOutFile, L"\n" );
			fwprintf_s( pOutFile, L"\t\t// Return the message\n" );
			fwprintf_s( pOutFile, L"\t\treturn pNewMsg;\n" );
		}
		fwprintf_s( pOutFile, L"\t}\n" );
		fwprintf_s( pOutFile, L"\n" );


		// The storage function
		fwprintf_s( pOutFile, L"\t/// Fill in a buffer with the message data\n" );
		if( iterMsg->members.size() == 0 )
			fwprintf_s( pOutFile, L"\tvirtual uint32 StoreInBuffer( NetDataBlockOut& ) const { return 0; }\n" );
		else
		{
			fwprintf_s( pOutFile, L"\tvirtual uint32 StoreInBuffer( NetDataBlockOut& dataOut ) const\n" );
			fwprintf_s( pOutFile, L"\t{\n" );
			fwprintf_s( pOutFile, L"\t\tuint32 startPos = dataOut.GetCurPosition();\n" );
			fwprintf_s( pOutFile, L"\n" );

			for( MsgMemberList::const_iterator iterMember = iterMsg->members.begin(); iterMember != iterMsg->members.end(); ++iterMember )
			{
				if( iterMember->size == 8 )
					fwprintf_s( pOutFile, L"\t\tdataOut.WriteVal8( (uint8)%s );\n", iterMember->sName.c_str() );
				else if( iterMember->size == 16 )
					fwprintf_s( pOutFile, L"\t\tdataOut.WriteVal16( (uint16)%s );\n", iterMember->sName.c_str() );
				else if( iterMember->size == 32 )
					fwprintf_s( pOutFile, L"\t\tdataOut.WriteVal32( (uint32)%s );\n", iterMember->sName.c_str() );
				else if( iterMember->type == L"std::wstring" )
					fwprintf_s( pOutFile, L"\t\tdataOut.WriteString( %s );\n", iterMember->sName.c_str() );
			}
			fwprintf_s( pOutFile, L"\n" );
			fwprintf_s( pOutFile, L"\t\treturn dataOut.GetCurPosition() - startPos;\n" );
			fwprintf_s( pOutFile, L"\t}\n" );
		}
		fwprintf_s( pOutFile, L"\n" );


		// The message ID accessor
		fwprintf_s( pOutFile, L"\t/// Get the message type ID\n" );
		fwprintf_s( pOutFile, L"\tvirtual uint16 GetMsgTypeID() const { return GNMI_%s; }\n", iterMsg->sName.c_str() );


		// Close the class
		fwprintf_s( pOutFile, L"};\n" );
		fwprintf_s( pOutFile, L"\n" );
	}

	// Close the file
	fwprintf_s( pOutFile, L"#endif // __GameNetMsgs_h" );
}