//=================================================================================================
/*!
	\file IGUIMsgHandler.h
	GUI Library
	Graphical User Interface Message Handler Base Class
	\author Taylor Clark
	\date July 15, 2006

	This file contains the definition for the base GUI message handler class.
*/
//=================================================================================================

#pragma once
#ifndef __IGUIMsgHandler_h
#define __IGUIMsgHandler_h

class GUIMessage;


//-------------------------------------------------------------------------------------------------
/*!
	\class IGUIMsgHandler
	\brief An interface for classes that want to handler GUI messages.
*/
//-------------------------------------------------------------------------------------------------
class IGUIMsgHandler
{
public:

	/// Handle a GUI message
	virtual bool HandleMessage( GUIMessage* ) = 0;
};


#endif // __IGUIMsgHandler_h