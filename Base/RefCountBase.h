//=================================================================================================
/*!
	\file RefCountBase.h
	Base Library
	Reference Counted Object Header
	\author Taylor Clark
	\date March 2, 2006

	This header contains the definition for the reference counted object class.
*/
//=================================================================================================

#pragma once
#ifndef __RefCountBase_h
#define __RefCountBase_h


//-------------------------------------------------------------------------------------------------
/*!
	\class RefCountBase
	\brief Represents a base class to use for counting the references to an object.
*/
//-------------------------------------------------------------------------------------------------
class RefCountBase
{
private:

	/// The reference count
	int m_RefCount;

public:

	/// The default constructor
	RefCountBase() : m_RefCount( 0 )
	{
	}

	/// Increment the reference count
	void AddRef(){ ++m_RefCount; }

	/// Decrement the reference count
	void RemoveRef(){ --m_RefCount; }

	/// Get the number of refernces to this object
	int GetRefCount() const { return m_RefCount; }
};

#endif // __RefCountBase_h