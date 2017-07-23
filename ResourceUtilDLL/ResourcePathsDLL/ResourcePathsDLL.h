// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RESOURCEPATHSDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RESOURCEPATHSDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef RESOURCEPATHSDLL_EXPORTS
#define RESOURCEPATHSDLL_API __declspec(dllexport)
#else
#define RESOURCEPATHSDLL_API __declspec(dllimport)
#endif

// This class is exported from the ResourcePathsDLL.dll
class RESOURCEPATHSDLL_API CResourcePathsDLL {
public:
	CResourcePathsDLL(void);
	// TODO: add your methods here.
};

extern RESOURCEPATHSDLL_API int nResourcePathsDLL;

RESOURCEPATHSDLL_API int fnResourcePathsDLL(void);
