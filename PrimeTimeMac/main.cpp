
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include "PrimeTime/ApplicationSFML.h"
#include "Base/MsgLogger.h"
#include <Carbon/Carbon.h>


uint32 PT_VERSION = 18;

// ' ' (An empty message)
// Don't modify  0xFFFF, 0x000F, 0x9AB5, 0xD431, it's used to find the custom message data
const wchar_t CUST_MSG[76] = { 0xFFFF, 0x000F, 0x9AB5, 0xD431, 18884, 22020, 50873, 1, 96, 29883, 52221, 17697, 2504, 46268, 23630, 49804, 36415, 40871, 26827, 18954, 52856, 13531, 19154, 12360, 26345, 17575, 2520, 16094, 2921, 24324, 36394, 48084, 6813, 2399, 48777, 47993, 39409, 34390, 49391, 17467, 51274, 13666, 40167, 10592, 23377, 14305, 17088, 4005, 12065, 22734, 50984, 47689, 3143, 36938, 16966, 2532, 34939, 43777, 40339, 25017, 42894, 2795, 19104, 26554, 5048, 4142, 10889, 110, 9050, 41517, 10400, 13584, 45806, 20919, 22006, 39195 };

extern uint8 PictureData[];

// Don't modify  0xFFFF, 0x0F0F, 0x88BF, 0xD431, it's used to find the custom message data
const wchar_t CUST_PIC_MSG[76] = { 0xFFFF, 0x0F0F, 0x88BF, 0xD431, 18884, 22020, 50873, 1, 96, 29883, 52221, 17697, 2504, 46268, 23630, 49804, 36415, 40871, 26827, 18954, 52856, 13531, 19154, 12360, 26345, 17575, 2520, 16094, 2921, 24324, 36394, 48084, 6813, 2399, 48777, 47993, 39409, 34390, 49391, 17467, 51274, 13666, 40167, 10592, 23377, 14305, 17088, 4005, 12065, 22734, 50984, 47689, 3143, 36938, 16966, 2532, 34939, 43777, 40339, 25017, 42894, 2795, 19104, 26554, 5048, 4142, 10889, 110, 9050, 41517, 10400, 13584, 45806, 20919, 22006, 39195 };

// Index 4 is year of expiration + 2010, index 5 is month 1-12, index 6 is day 1-31
// These are 16 bit to make the customization code simpler
const uint16 ExpirationDate[10] = { 0x6543, 0x5432, 0x5321, 0x3210, 2, 6, 30, 0xF8F9, 0xFFFF, 0xFFFF };


void RunGame(TCBase::ParamList cmdLineParams);

bool EvaluationExpirationDateChecker();

void DisplayFatalErrorMsg( const wchar_t* szMsg )
{
}

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main( int argc, const char* argv[] )
{
	TCBase::ParamList cmdList;
	
	TCBase::InitAppPathFromArg( argv[0] );
	
	cmdList.ParseCmdLineOptions(argc,argv);
	RunGame(cmdList);
	
    return EXIT_SUCCESS;
}

void RunGame(TCBase::ParamList cmdLineParams)
{
#ifdef EVALUATION
	if( !EvaluationExpirationDateChecker() )
		return;
#endif
	
#ifndef _PT_DEMO
	//if( !cmdLineParams.HasOption( L"runoncd" ) )
	//	MsgLogger::Get().AddListener( (GameMgr::GetUserFilesPath() + PTDefines::LOG_PRE_FILE_NAME + L"1." + PTDefines::LOG_FILE_EXT).c_str() );
#endif
	if( !ApplicationSFML::InitApp( cmdLineParams, CUST_MSG + 7, PictureData + 14, CUST_PIC_MSG + 7 ) )
		return;

	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Entering game..." );

	volatile uint64_t keys[] = {0xffff33a45bcc8009ULL, 0x0000000100000002ULL, 0xaadc0a081dffb05eULL, 0x0101020103010002ULL};
	ApplicationBase::GameKey = keys[2];
	
	ApplicationBase::SendAppConnectMessage(L"as");
	
	ApplicationBase::Get()->Run();
	
	MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Terminating game..." );
	
	ApplicationBase::TermApp();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// EvaluationExpirationDateChecker Global
/// \returns True if play is allowed, false if the game is expired
///
/// Prevent the game from being played if it's passed its expiration date.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool EvaluationExpirationDateChecker()
{
	std::wstring sReferenceToPreventRemoval( (wchar_t*)ExpirationDate );
	
	// Get the current date
	time_t curTime = time(NULL);
	tm curTm = *localtime( &curTime );
	int curYear = curTm.tm_year + 1900;
	int curMonth = curTm.tm_mon + 1;
	int curDay = curTm.tm_mday;
	
	// Get the expiration date
	int32 expirationYear = 2010 + ExpirationDate[4];
	int32 expirationMonth = ExpirationDate[5];
	int32 expirationDay = ExpirationDate[6];
	
	// If we're in the end year
	bool allowPlay = true;
	if( curYear == expirationYear )
	{
		// If we're in March
		if( curMonth == expirationMonth )
		{
			// If it's the last day
			if( curDay < expirationDay )
			{
				//MessageBox( NULL, _T("Don't forget, this evaluation copy of Prime Time expires at the end of this month. Please visit www.PrimeTimeMath.com for information on how to buy a full copy that never expires."), _T("Prime Time: Math Adventure"), MB_OK | MB_ICONSTOP );
			}
			else
				allowPlay = false;
		}
		else if( curMonth > expirationMonth )
			allowPlay = false;
	}
	else if( curYear > expirationYear )
		allowPlay = false;
	
	if( !allowPlay )
	{
		//MessageBox( NULL, _T("We really hope your students have enjoyed Prime Time, but the time limit for this copy of Prime Time has expired. Please visit www.PrimeTimeMath.com for information on how to buy a full copy. Thanks for playing!"), _T("Prime Time: Math Adventure"), MB_OK | MB_ICONSTOP );
	}
	
	return allowPlay;
}