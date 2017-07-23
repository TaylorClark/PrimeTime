//=================================================================================================
/*!
	\file Resource.h
	Resources Library
	Resource Base Class Header
	\author Taylor Clark
	\date March 3, 2006

	This header contains the definition for the resource base class.
*/
//=================================================================================================

#pragma once
#ifndef __Resource_h
#define __Resource_h

#include "Base/Types.h"
#include <string>
#include "Base/RefCountBase.h"
#include "Base/RefCountHandle.h"
#include "ResTypes.h"

class ResourceMgr;

//-------------------------------------------------------------------------------------------------
/*!
	\class Resource
	\brief The base class of resource objects.
*/
//-------------------------------------------------------------------------------------------------
class Resource : public RefCountBase
{
private:

	friend class ResourceMgr;

	/// The name of the resource
	std::wstring m_sName;

	/// The assignement operator, private so it can not be used
	Resource& operator =( const Resource& ){}

protected:

	/// The resource ID
	const ResourceID m_ResID;

public:

	Resource( ResourceID resID ) : m_ResID( resID )
	{
	}

	/// The copy constructor, private so it can not be used
	Resource( const Resource& copy ) : m_ResID( copy.m_ResID ),
										m_sName( copy.m_sName )
	{}

	/// The destructor
	virtual ~Resource() {}

	/// Get the name of the resource
	const std::wstring& GetName() const { return m_sName; }

	/// Get the resource ID
	ResourceID GetResID() const { return m_ResID; }

	/// Get the resource type
	virtual EResourceType GetResType() const = 0;

	/// Release the resources, if any, used by this resource
	virtual void ReleaseSubResources(){};

	/// Get a descriptive string for a resource type
	static const wchar_t* GetResTypeStr( EResourceType resType )
	{
		switch( resType )
		{
		case RT_Image:
			return L"Image";

		case RT_Music:
			return L"Music";

		case RT_Sound:
			return L"Sound";

		case RT_Font:
			return L"Font";

		case RT_Sprite:
			return L"Sprite";
                
        default:
            break;
		}

		return L"Bad Resource Type";
	}
};

/// A reference counted reference sprite
typedef RefCountHandle<Resource> ResourceHndl;

#endif // __Resource_h