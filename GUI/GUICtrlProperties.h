//=================================================================================================
/*!
	\file GUICtrlProperties.h
	2D Game Engine
	Graphical User Interface Control Properties Header
	\author Taylor Clark
	\date February 4, 2006

	This file contains the definition for the GUI control property classes.
*/
//=================================================================================================

#pragma once
#ifndef __GUICtrlProperties_h
#define __GUICtrlProperties_h

#ifndef _DEBUG
#error GUI control properties are only used in debug mode
#endif

#include "GUIDefines.h"
#include <string>
#include <vector>
#include "Graphics2D/TCFont.h"
#include "Graphics2D/RefSprite.h"
#include "Resource/ResourceMgr.h"
#include "GUIListItem.h"
#include "Base/NumFuncs.h"

// Ignore the warning for unreferenced formal parameters
#pragma warning( push )
#pragma warning( disable : 4100 )


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlProperty
	\brief The base class for GUI control properties.

	This class is the base class for all GUI control property classes.  A GUI control property is
	used for the in-game GUI editor to list editable properties about a control.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlProperty
{
	/// The name of the property
	std::wstring m_sName;

public:

	/// The default constructor
	GUICtrlProperty( const wchar_t* szName ) : m_sName( szName )
	{
	}

    /// Virtual to ensure all derived destructors get called
    virtual ~GUICtrlProperty()
    {
    }
    
	/// Get a string the represents the property
	virtual const wchar_t* ValToString() const{ return NULL; };

	/// Parse the value from a string
	virtual void ValFromString( const wchar_t* szValStr ){};

	/// Get the type of property
	virtual GUIDefines::EPropType GetType() const = 0;

	/// Get a descriptive string for a property type
	static const wchar_t* GetPropDescStr( GUIDefines::EPropType propType )
	{
		switch( propType )
		{
		case GUIDefines::PT_Int:
			return L"Integer";

		case GUIDefines::PT_String:
			return L"String";

		case GUIDefines::PT_StringList:
			return L"String List";

		case GUIDefines::PT_Font:
			return L"Font";

		case GUIDefines::PT_Sprite:
			return L"Sprite";

		case GUIDefines::PT_Boolean:
            return L"Boolean";
                
        default:
            // Stop warnings on mac
            break;
		}

		return NULL;
	}

	/// Get the name of the property
	const wchar_t* GetName() const { return m_sName.c_str(); }
};

#pragma warning( pop )


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlPropInt
	\brief The class for the int GUI control property.

	This is the integer GUI control property class.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlPropInt : public GUICtrlProperty
{
	// Pointer to the int we modify
	int* m_pRefInt;

	
public:

	/// The default constructor
	GUICtrlPropInt( const wchar_t* szName, int* pInt ) : GUICtrlProperty( szName ),
														m_pRefInt( pInt )
	{
	}

	/// Get a string the represents the property
	virtual const wchar_t* ValToString() const
	{
		// If there is no integer return a 0 value
		if( !m_pRefInt )
			return L"0";

		// Get the value into a string
		const int BUFFER_SIZE = 16;
		static wchar_t szRetStr[ BUFFER_SIZE ] = {0};
		swprintf( szRetStr, BUFFER_SIZE, L"%d", *m_pRefInt );

		// Return the string
		return szRetStr;
	}

	/// Parse the value from a string
	virtual void ValFromString( const wchar_t* szValStr )
	{
		// Parse the value
		if( m_pRefInt )
			*m_pRefInt = TCBase::WToI( szValStr );
	}


	/// Get the type of property
	virtual GUIDefines::EPropType GetType() const { return GUIDefines::PT_Int; }

	/// Set the string
	void Set( int val )
	{
		if( m_pRefInt )
			*m_pRefInt = val;
	}

	/// Get the string
	const int* Get(){ return m_pRefInt; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlPropString
	\brief The class for the string GUI control property.

	This is the string GUI control property class.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlPropString : public GUICtrlProperty
{
protected:

	/// The pointer to the string to reference
	std::wstring* m_pRefStr;

public:

	/// The default constructor
	GUICtrlPropString( const wchar_t* szName, std::wstring* pStr ) : GUICtrlProperty( szName ),
														m_pRefStr( pStr )
	{
	}

	/// Get the type of property
	virtual GUIDefines::EPropType GetType() const { return GUIDefines::PT_String; }

	/// Get a string the represents the property
	virtual const wchar_t* ValToString() const
	{
		// If there is no integer return a 0 value
		if( !m_pRefStr )
			return L"";

		// Return the string
		return m_pRefStr->c_str();
	}

	/// Parse the value from a string
	virtual void ValFromString( const wchar_t* szValStr )
	{
		// Parse the value
		if( m_pRefStr )
			*m_pRefStr = szValStr;
	}

	/// Set the string
	void Set( const std::wstring& sStr )
	{
		if( m_pRefStr )
			*m_pRefStr = sStr;
	}

	/// Get the string
	const std::wstring* Get(){ return m_pRefStr; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlPropStringList
	\brief The class for the string list GUI control property.

	This is the string list GUI control property class.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlPropStringList : public GUICtrlProperty
{
protected:

	/// The pointer to the string vector to reference
	std::vector< GUIListItem >* m_pRefVector;

public:

	/// The default constructor
	GUICtrlPropStringList( const wchar_t* szName, std::vector< GUIListItem >* pStrList ) : GUICtrlProperty( szName ),
														m_pRefVector( pStrList )
	{
	}

	/// Get the type of property
	virtual GUIDefines::EPropType GetType() const { return GUIDefines::PT_StringList; }

	/// Get a string the represents the property
	virtual const wchar_t* ValToString() const
	{
		static std::wstring sRetVal;
		sRetVal.clear();

		if( m_pRefVector && m_pRefVector->size() > 0 )
			sRetVal += (*m_pRefVector)[0].sText + L",";
		sRetVal += L"...";

		// Return the string
		return sRetVal.c_str();
	}

	/// Get the string list
	std::vector< GUIListItem >* Get(){ return m_pRefVector; }
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlPropFont
	\brief The class for the font GUI control property.

	This is the font GUI control property class.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlPropFont : public GUICtrlProperty
{
	// Pointer to the int we modify
	TCFontHndl* m_pFontHndl;

	
public:

	/// The default constructor
	GUICtrlPropFont( const wchar_t* szName, TCFontHndl* pFontHndl ) : GUICtrlProperty( szName ),
														m_pFontHndl( pFontHndl )
	{
	}

	/// Get a string the represents the property
	virtual const wchar_t* ValToString() const
	{
		// Get the font
		if( !m_pFontHndl || !m_pFontHndl->GetObj() )
			return L"NONE";

		// Get the value into a string
		const int BUFFER_SIZE = 24;
		static wchar_t szRetStr[ BUFFER_SIZE ] = {0};
		swprintf( szRetStr, BUFFER_SIZE, L"%s", m_pFontHndl->GetObj()->GetName().c_str() );

		// Return the string
		return szRetStr;
	}

	/// Parse the value from a string
	virtual void ValFromString( const wchar_t* )
	{
	}

	/// Get the type of property
	virtual GUIDefines::EPropType GetType() const { return GUIDefines::PT_Font; }

	void Set( TCFontHndl& newFont )
	{
		if( m_pFontHndl )
			*m_pFontHndl = newFont;
	}
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlPropSprite
	\brief The class for the sprite GUI control property.

	This is the sprite GUI control property class.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlPropSprite : public GUICtrlProperty
{
	// Pointer to the sprite we modify
	RefSpriteHndl* m_pSpriteHndl;

	
public:

	/// The default constructor
	GUICtrlPropSprite( const wchar_t* szName, RefSpriteHndl* pSpriteHndl ) : GUICtrlProperty( szName ),
														m_pSpriteHndl( pSpriteHndl )
	{
	}

	/// Get a string the represents the property
	virtual const wchar_t* ValToString() const
	{
		// Get the font
		if( !m_pSpriteHndl || !m_pSpriteHndl->GetObj() )
			return L"NONE";

		// Get the value into a string
		const int BUFFER_SIZE = 24;
		static wchar_t szRetStr[ BUFFER_SIZE ] = {0};
		swprintf( szRetStr, BUFFER_SIZE, L"%s", m_pSpriteHndl->GetObj()->GetName().c_str() );

		// Return the string
		return szRetStr;
	}

	/// Parse the value from a string
	virtual void ValFromString( const wchar_t* )
	{
	}

	/// Get the type of property
	virtual GUIDefines::EPropType GetType() const { return GUIDefines::PT_Sprite; }

	void Set( RefSpriteHndl& newSpr )
	{
		if( m_pSpriteHndl )
			*m_pSpriteHndl = newSpr;
	}
};


//-------------------------------------------------------------------------------------------------
/*!
	\class GUICtrlPropBool
	\brief The class for the boolean GUI control property.

	This is the boolean GUI control property class.
*/
//-------------------------------------------------------------------------------------------------
class GUICtrlPropBool : public GUICtrlProperty
{
	// Pointer to the int we modify
	bool* m_pRefBool;

	
public:

	/// The default constructor
	GUICtrlPropBool( const wchar_t* szName, bool* pBool ) : GUICtrlProperty( szName ),
														m_pRefBool( pBool )
	{
	}

	/// Get a string the represents the property
	virtual const wchar_t* ValToString() const
	{
		// If there is no integer return a 0 value
		if( !m_pRefBool )
			return L"null";

		// Get the value into a string
		if( *m_pRefBool )
			return L"true";

		return L"false";
	}

	/// Parse the value from a string
	virtual void ValFromString( const wchar_t* szValStr )
	{
		// Parse the value
		if( m_pRefBool )
		{
			std::wstring sVal( szValStr );
			if( sVal == L"true" )
				*m_pRefBool = true;
			else
				*m_pRefBool = false;
		}
	}


	/// Get the type of property
	virtual GUIDefines::EPropType GetType() const { return GUIDefines::PT_Boolean; }

	/// Set the bool
	void Set( bool val )
	{
		if( m_pRefBool )
			*m_pRefBool = val;
	}

	/// Get the bool
	const bool* Get(){ return m_pRefBool; }
};

#endif // __GUICtrlProperties_h