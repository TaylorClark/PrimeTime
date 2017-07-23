/*=================================================================================================

	TraceAssist.h
	Base Library
	Trace Assisting Header
	\author Taylor Clark
	\Date May 7, 2006

	This header contains the definitions for the classes that assist in tracing errors.

=================================================================================================*/

#pragma once
#ifndef __TraceAssist_h
#define __TraceAssist_h

namespace TraceAssist
{
	void* CreateOutputWindow( void* pInstance );

	void* GetTraceHWnd();

	void FreeOutputWindow();
};

#endif // __TraceAssist_h