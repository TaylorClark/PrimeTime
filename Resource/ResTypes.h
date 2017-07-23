//=================================================================================================
/*!
	\file ResTypes.h
	Resources Library
	Resource Types Header
	\author Taylor Clark
	\date March 4, 2007

	This header contains the definition for the resource types.
*/
//=================================================================================================

#pragma once
#ifndef __ResTypes_h
#define __ResTypes_h

// The resource ID type
typedef uint32 ResourceID;

// Ignore the warning for using a micrsoft extension to the language (Defining the enum type)
#pragma warning( disable : 4480 )


// The resource types
#ifdef WIN32
enum EResourceType : int32
#else
enum EResourceType
#endif
{
	RT_Error = -1,
	RT_Image = 0,
	RT_Sound,
	RT_Music,
	RT_Font,
	RT_Sprite,
	RT_COUNT
};

#ifdef WIN32
enum EImageResourceType : int32
#else
enum EImageResourceType
#endif
{
	IRT_Bitmap = 0,
	IRT_Jpeg,
	IRT_BitmapRLE
};

#endif // __ResTypes_h