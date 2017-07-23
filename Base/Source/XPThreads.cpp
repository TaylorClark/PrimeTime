/*******************************************************************************
** FILE:			XPThreads.h

** DESCRIPTION: 

** AUTHOR:			Amit Gupta

** CREATED:		
********************************************************************************/

#include "../XPThreads.h"
	

/******************************************************************************
** FUNCTION:	Constructor 
*******************************************************************************/
XPThreads::XPThreads(ThreadProc ptrCallback, long nTimeout)
: m_Callback(ptrCallback)
, m_Threadid(0)
#if WIN32
, m_nTimeout(nTimeout)
//, m_ptrThread(NULL)
#endif
{
}

/******************************************************************************
** FUNCTION:	Destructor 
*******************************************************************************/
XPThreads::~XPThreads() throw()
{
	//Stop();	
}


/******************************************************************************
** FUNCTION:	Stop 
*******************************************************************************/
void XPThreads::Stop()
{
#if WIN32
	/*
	//waiting for the thread to terminate
	if (m_ptrThread) 
	{
		if(WAIT_TIMEOUT == ::WaitForSingleObject (m_ptrThread, m_nTimeout))
			::TerminateThread (m_ptrThread, 1);

		::CloseHandle (m_ptrThread);
	}
	*/
#endif
}

/******************************************************************************
** FUNCTION:	Run 
*******************************************************************************/
bool XPThreads::Run( void* pThreadArgument )
{
	bool bRetVal = false;
	
	// Uses a loop so we can break out of this block and have one return
	if(!IsThreadCreated())
	{
#if WIN32
		_beginthread( m_Callback, 0, pThreadArgument );
		/*
		m_ptrThread = ::CreateThread (NULL, 0, 
			(unsigned long (__stdcall *)(void *))m_Callback, 
			pThreadArgument, 0, &m_Threadid);

		if(m_ptrThread != NULL)
			bRetVal = true;
		*/
	
#else
	    pthread_attr_t attr;
	
		/* Initialize and set thread detached attribute */
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);				
	
		if(pthread_create(&m_Threadid, &attr, m_Callback, pThreadArgument))
		{
		   /* Free attribute and wait  */
			pthread_attr_destroy(&attr);

			bRetVal = true;
		}
#endif
	}
	
	return bRetVal;
}

/******************************************************************************
** FUNCTION:	SetThreadProc
*******************************************************************************/
void XPThreads::SetThreadProc(ThreadProc ptrCallback)
{
	m_Callback = ptrCallback;
}

/******************************************************************************
** FUNCTION:	IsThreadCreated
*******************************************************************************/
bool XPThreads::IsThreadCreated()
{
	return m_Threadid ? true : false;
}

