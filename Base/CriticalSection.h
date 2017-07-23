//=================================================================================================
/*!
	\file CriticalSection.h
	Base Library
	Critical Section Header
	\author Taylor Clark
	\date March 18, 2010

  This header contains the class definition for cross-platform critical section class used to
  synchronize multi-threaded code.

*/
//=================================================================================================

#pragma once

#ifndef __CriticalSection_h
#define __CriticalSection_h


namespace TCBase
{
	
/**
 * @class A wrapper-class around Critical Section functionality, WIN32 & PTHREADS.
 */
class CriticalSection
{
private:
	void* _pSection;

public:
	/**
	 * @brief CriticalSection class constructor.
	 */
	CriticalSection();

	/**
	 * @brief CriticalSection class destructor
	 */
	~CriticalSection(void);

	/**
	 * @fn void WaitForFinish(void)
	 * @brief Waits for the critical section to unlock.
	 * This function puts the waiting thread in a waiting
	 * state.
	 * @see TryEnter()
	 * @return void
	 */
	void WaitForFinish(void);

	/**
	 * @fn void Enter(void)
	 * @brief Wait for unlock and enter the CriticalSection object.
	 * @see TryEnter()
	 * @return void
	 */
	void Enter(void);

	/**
	 * @fn void Leave(void)
	 * @brief Leaves the critical section object.
	 * This function will only work if the current thread
	 * holds the current lock on the CriticalSection object
	 * called by Enter()
	 * @see Enter()
	 * @return void
	 */
	void Leave(void);

	/**
	 * @fn bool TryEnter(void)
	 * @brief Attempt to enter the CriticalSection object
	 * @return bool(true) on success, bool(false) if otherwise
	 */
	bool TryEnter(void);

};

};

#endif // __CriticalSection_h