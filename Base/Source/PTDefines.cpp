#include "../PTDefines.h"


const wchar_t* PTDefines::DUMP_PRE_FILE_NAME = L"PTErrorReport_";
const wchar_t* PTDefines::DUMP_FILE_EXT = L"dmp";
const wchar_t* PTDefines::LOG_PRE_FILE_NAME = L"PTLog_";
const wchar_t* PTDefines::LOG_FILE_EXT = L"log";

const char* PTDefines::UPDATER_OPTION_SILENT = "/silent";
const wchar_t* PTDefines::WUPDATER_OPTION_SILENT = L"/silent";

const wchar_t* PTDefines::UPDATE_FOLDER = L"DLUpdates\\";

const char* PTDefines::UPDATER_EXE_FILE = "PrimeTimeUpdater.exe";
const wchar_t* PTDefines::WUPDATER_EXE_FILE = L"PrimeTimeUpdater.exe";

#ifdef _DEBUG
const char* PTDefines::GAME_EXE_FILE = "PrimeTime_dbg.exe";
const wchar_t* PTDefines::WGAME_EXE_FILE = L"PrimeTime_dbg.exe";
#else
const char* PTDefines::GAME_EXE_FILE = "PrimeTime.exe";
const wchar_t* PTDefines::WGAME_EXE_FILE = L"PrimeTime.exe";
#endif