/*=================================================================================================

	Types.h
	Base Library
	Base Types Header
	\author Taylor Clark
	\Date May 6, 2006

	This header contains the definitions for the base types.

=================================================================================================*/

#pragma once
#ifndef __Types_H
#define __Types_H

#ifdef WIN32
// Integral Types
typedef __int8 int8;
typedef unsigned __int8 uint8;

typedef __int16 int16;
typedef unsigned __int16 uint16;

typedef __int32 int32;
typedef unsigned __int32 uint32;

typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
#include <stdint.h>

typedef int8_t int8;
typedef uint8_t uint8;

typedef int16_t int16;
typedef uint16_t uint16;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;
#endif

// Floating Point Types
typedef float float32;
typedef double float64;


#if (defined(WINVER) && (WINVER != 0x0500))
#error WINVER setting is incorrect, please ensure Types.h is included before any other headers
#endif

#ifndef WINVER                          // Specifies that the minimum required platform is Windows 2000.
#define WINVER 0x0500
#endif

#if (defined(_WIN32_WINNT) && (_WIN32_WINNT != 0x0500))
#error _WIN32_WINNT setting is incorrect, please ensure Types.h is included before any other headers
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows 2000.
#define _WIN32_WINNT 0x0500     // Change this to the appropriate value to target other versions of Windows.
#endif

#if (defined(_WIN32_WINDOWS) && (_WIN32_WINDOWS != 0x0500))
#error _WIN32_WINDOWS setting is incorrect, please ensure Types.h is included before any other headers
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif

#if (defined(_WIN32_IE) && (_WIN32_IE != 0x0500))
#error _WIN32_IE setting is incorrect, please ensure Types.h is included before any other headers
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 5.0.
#define _WIN32_IE 0x0500
#endif

#if (defined(NTDDI_VERSION) && (NTDDI_VERSION != NTDDI_WIN2K))
#error NTDDI_VERSION setting is incorrect, please ensure Types.h is included before any other headers
#endif

#ifndef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN2K
#endif

#endif // __Types_H