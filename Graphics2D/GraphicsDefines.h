//=================================================================================================
/*!
	\file GraphicsDefines.h
	Graphics Library
	Graphics Related Defines
	\author Taylor Clark
	\date June 22, 2006

	This file contains enum and constant for the graphics library.
*/
//=================================================================================================

#pragma once
#ifndef __GraphicsDefines_h
#define __GraphicsDefines_h

#include "Base/Types.h"


namespace GraphicsDefines
{
	/// The draw sprite effect flags
	enum EDrawEffectFlags
	{
		DEF_FlipHoriz = 1,
		DEF_FlipVert = 2,
		DEF_Rotate_90 = 4,
		DEF_Rotate_180 = 8,
		DEF_Rotate_270 = 16
	};

	typedef uint32 (*ModPixelCB)( uint32 inColor );
};

#endif // __GraphicsDefines_h