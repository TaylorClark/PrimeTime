//=================================================================================================
/*!
	\file TCAssert.h
	Base Library
	Assert Header
	\author Taylor Clark
	\date December 27, 2005

	This file contains the declaration for the assert functions.  This code is based off the
	assert functions from The Matrix Online.
*/
//=================================================================================================

#pragma once
#ifndef __TCAssert_h
#define __TCAssert_h

#include "Types.h"
#include <stdlib.h>

#ifdef _DEBUG
#ifdef WIN32
#define USE_TCASSERT
#endif
#endif

/// The basic assert
#ifdef USE_TCASSERT

/// The wide file define
#ifndef __WFILE__
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#endif


// these are return codes for the assert dialog as well as its control codes

//! The user wishes to break into the debugger.
#define	LT_ASSERT_DEBUG		1100
//! The user wishes to abort the program run.
#define	LT_ASSERT_ABORT		1101
//! The user wishes to ignore this assert for the res of this run.
#define	LT_ASSERT_IGNORE	1102
//! The user wishes to continue the program run.
#define	LT_ASSERT_CONTINUE	1103


// function to create & display the assert dialog
int32 sysassert(const wchar_t* pAssertText, const wchar_t* pMsgText, const wchar_t* pFilename, int lineno);


/*! 
\param exp The expression to test.

LTASSERT(exp) is the standard single parameter assert.  It tests (exp), and if the result
is false, a dialog box is shown with several options.
*/
#define TCASSERT(exp)	TCASSERTX(exp, 0)


/*!
\param exp The expression to test.
\param msg A null-terminated string to print in the assert dialog if \i exp fails.

LTASSERTX(exp, msg) is a 2 parameter assert where you can pass an extra string (msg) to be
displayed in the assert dialog if the (exp) is false.
*/
#define TCASSERTX(exp, msg) {static bool bIgnoreAlways = false;								\
								if ( !(exp) && !bIgnoreAlways ) {							\
									switch (sysassert(L#exp, msg,  __WFILE__, __LINE__)) {		\
										case LT_ASSERT_DEBUG:  {__asm { int 3 } } break;	\
										case LT_ASSERT_ABORT:  abort(); break;				\
										case LT_ASSERT_IGNORE: bIgnoreAlways = true; break;	\
										case LT_ASSERT_CONTINUE:							\
										default: break;	}}}; ((void)0)	


// TCBREAK() causes an immediate assert, with no expression to test.
#define TCBREAK()		TCBREAKX(0)

// TCBREAK() causes an immediate assert and displays a message, with no expression to test
#define TCBREAKX(msg)		{static bool bIgnoreAlways = false;								\
								if ( !bIgnoreAlways ) {										\
									switch (sysassert(L"LTBREAK", msg,  __WFILE__, __LINE__)) {		\
										case LT_ASSERT_DEBUG: {__asm { int 3 } } break;	\
										case LT_ASSERT_ABORT:  abort(); break;				\
										case LT_ASSERT_IGNORE: bIgnoreAlways = true; break;	\
										case LT_ASSERT_CONTINUE:							\
										default: break;	}}};	((void)0)

// A Compile time assert
#define CT_TCASSERT(X)		{ char CT_Assert[(int)(X)]; CT_Assert[0]; }



#else
	#define ASSERT( condition, message ) ((void)0)
	#define BREAK( message ) ((void)0)

	#define TCASSERT( exp ) ((void)0)
	#define TCASSERTX( exp, msg ) ((void)0)
	#define TCBREAK()	((void)0)
	#define TCBREAKX(msg) ((void)0)
	#define CT_TCASSERT(X) ((void)0)

#endif


#endif // __TCAssert_h