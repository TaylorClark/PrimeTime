//=================================================================================================
/*!
	\file RefCountHandle.h
	Base Library
	Reference Counting Handle Header
	\author Taylor Clark
	\date March 2, 2006

	This header contains the definition for the reference counted object class.
*/
//=================================================================================================

#pragma once
#ifndef __RefCountHandle_h
#define __RefCountHandle_h


//-------------------------------------------------------------------------------------------------
/*!
	\class RefCountBase
	\brief Represents a base class to use for counting the references to an object.  When wrapped
	around a class that derives from RefCountBase it will auto-update the number of references to
	the wrapped object.
*/
//-------------------------------------------------------------------------------------------------
template< typename T >
class RefCountHandle
{
private:

	/// The referenced object
	T* m_pRefObj;

public:

	/// The default constructor
	RefCountHandle() : m_pRefObj( 0 )
	{
	}

	/// The constructor used to intialize using a reference counted object
	RefCountHandle( T* pObj ) : m_pRefObj( pObj )
	{
		if( m_pRefObj )
			m_pRefObj->AddRef();
	}

	/// The copy constructor
	RefCountHandle( const RefCountHandle<T>& copyObj ) : m_pRefObj( copyObj.m_pRefObj )
	{
		if( m_pRefObj )
			m_pRefObj->AddRef();
	}

	/// The destructor
	~RefCountHandle()
	{
		if( m_pRefObj )
			m_pRefObj->RemoveRef();
	}

	/// Release the object
	void ReleaseHandle()
	{
		if( m_pRefObj )
			m_pRefObj->RemoveRef();
		m_pRefObj = 0;
	}

	/// Get the object
	T* operator ->() { return m_pRefObj; }

	/// Get the object
	T* GetObj() { return m_pRefObj; }

	/// Get the object
	const T* operator ->() const { return m_pRefObj; }

	/// Get the object
	const T* GetObj() const { return m_pRefObj; }

	/// Assignment operator
	RefCountHandle<T>& operator= ( T* pObj )
	{
		// If we currently are referencing and object then remove our reference count
		if( m_pRefObj )
			m_pRefObj->RemoveRef();

		// Store the object
		m_pRefObj = pObj;

		// If we are assigned to a valid object that increase its ref count
		if( m_pRefObj )
			m_pRefObj->AddRef();

		return *this;
	}

	/// Assignment operator
	RefCountHandle<T>& operator= ( RefCountHandle<T> obj )
	{
		// If we currently are referencing and object then remove our reference count
		if( m_pRefObj )
			m_pRefObj->RemoveRef();

		// Store the object
		m_pRefObj = obj.m_pRefObj;

		// If we are assigned to a valid object that increase its ref count
		if( m_pRefObj )
			m_pRefObj->AddRef();

		return *this;
	}

	/// Comparison
	bool operator ==( const RefCountHandle<T>& obj )
	{
		return m_pRefObj == obj.m_pRefObj;
	}
};

#endif // __RefCountHandle_h