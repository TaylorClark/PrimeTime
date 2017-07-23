#include "../CriticalSection.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

namespace TCBase
{

/**
 * @brief CriticalSection class constructor.
 */
CriticalSection::CriticalSection() : _pSection( 0 )
{ 
#ifdef WIN32
	_pSection = new CRITICAL_SECTION();

	if (0 == InitializeCriticalSectionAndSpinCount( static_cast<LPCRITICAL_SECTION>( _pSection ), 0 ))
		throw("Could not create a CriticalSection");
#else
	_pSection = new pthread_mutex_t();
	
	if (pthread_mutex_init(static_cast<pthread_mutex_t*>( _pSection ), NULL) != 0)
		throw("Could not create a CriticalSection");
#endif
}


/**
 * @brief CriticalSection class destructor
 */
CriticalSection::~CriticalSection(void)
{
	this->WaitForFinish(); // acquire ownership
#ifdef WIN32
	LPCRITICAL_SECTION pSection = static_cast<LPCRITICAL_SECTION>( _pSection );
	DeleteCriticalSection( pSection );
	delete pSection;
#else
	pthread_mutex_t* pSection = static_cast<pthread_mutex_t*>( _pSection );
	pthread_mutex_destroy( pSection );
	delete pSection;
#endif

	_pSection = 0;
}


/**
 * @fn void WaitForFinish(void)
 * @brief Waits for the critical section to unlock.
 * This function puts the waiting thread in a waiting
 * state.
 * @see TryEnter()
 * @return void
 */
void CriticalSection::WaitForFinish(void)
{
	while(!this->TryEnter())
	{
	#ifdef WIN32
		Sleep(1); // put waiting thread to sleep for 1ms
	#else
		usleep(1000); // put waiting thread to sleep for 1ms (1000us)
	#endif
	};
}


/**
 * @fn void Enter(void)
 * @brief Wait for unlock and enter the CriticalSection object.
 * @see TryEnter()
 * @return void
 */
void CriticalSection::Enter(void)
{
	WaitForFinish(); // acquire ownership
#ifdef WIN32
	//EnterCriticalSection( static_cast<LPCRITICAL_SECTION>( _pSection ) );
#else
	//pthread_mutex_lock( static_cast<pthread_mutex_t*>( _pSection ) );
#endif
}

/**
 * @fn void Leave(void)
 * @brief Leaves the critical section object.
 * This function will only work if the current thread
 * holds the current lock on the CriticalSection object
 * called by Enter()
 * @see Enter()
 * @return void
 */
void CriticalSection::Leave(void)
{
#ifdef WIN32
	LeaveCriticalSection( static_cast<LPCRITICAL_SECTION>( _pSection ) );
#else
	pthread_mutex_unlock( static_cast<pthread_mutex_t*>( _pSection ) );
#endif
}

/**
 * @fn bool TryEnter(void)
 * @brief Attempt to enter the CriticalSection object
 * @return bool(true) on success, bool(false) if otherwise
 */
bool CriticalSection::TryEnter(void)
{
	// Attempt to acquire ownership:
#ifdef WIN32
	return(TRUE == TryEnterCriticalSection( static_cast<LPCRITICAL_SECTION>( _pSection ) ));
#else
	return(0 == pthread_mutex_trylock( static_cast<pthread_mutex_t*>( _pSection ) ));
#endif
}

};