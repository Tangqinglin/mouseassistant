

#include "comsemphore.h"

/************************************************************************/
//进程内线程间互斥
//
//返回值：=0 成功，其它，失败
/************************************************************************/
CM_RET Mutex_Create (CM_MUTEX_SEM *ms)
{
	CM_RET   ret;

#ifdef _WIN32
	ret = CD_SUCCESS;
	InitializeCriticalSection(ms);
#else
	ret = pthread_mutex_init (ms, 0);
	if ( ret != 0 )
	{
		printf ("GLBSEM gs_mutex_create error: mutex semaphore   (error=%d).", ret);
		return (ret);
	}

#endif

	return (ret);
}

CM_RET Mutex_Destroy (CM_MUTEX_SEM *ms)
{
	CM_RET ret = CD_SUCCESS;
#ifdef _WIN32
	DeleteCriticalSection (ms);
#else
	ret = pthread_mutex_destroy (ms);

	if ( ret != 0 )
	{
		/* Log a major error here */
		printf ("GLBSEM gs_mutex_destroy error: failed to destroy mutex semaphore  (error=%d)", ret);
	}
#endif
	return (ret);
}

CM_RET Mutex_Get_tm (CM_MUTEX_SEM *ms)
{
	CM_RET ret = CD_SUCCESS;

#ifdef _WIN32
	EnterCriticalSection (ms);
#else
	ret = pthread_mutex_lock (ms);
#endif


	return (ret);
}

CM_BOOLEAN Mutex_Try_tm (CM_MUTEX_SEM *ms)
{
	CM_BOOLEAN ret;

#ifdef _WIN32
	ret=TryEnterCriticalSection (ms);
#else
	ret = pthread_mutex_trylock(ms);
#endif
	return (ret);
}

CM_RET Mutex_Release (CM_MUTEX_SEM *ms)
{
	CM_RET ret = CD_SUCCESS;
#ifdef _WIN32
	LeaveCriticalSection (ms);
#else
	ret = pthread_mutex_unlock (ms);
#endif


	return (ret);
}

//////////////////////////////////////////////////////////////////////
//事件对象
//
//
//////////////////////////////////////////////////////////////////////
CM_RET Get_Event(CM_EVENT_SEM *es,CM_BOOLEAN manualReset)
{
	CM_RET iRet=CD_SUCCESS;
#ifdef _WIN32

	*es = CreateEvent(NULL,         // no security attributes
		manualReset,  // manual-reset event
		FALSE,        // initial state is signaled
		NULL	  // object name
		); 
	if (*es == NULL)
	{
		printf ("GLBSEM error: CreateEvent failed for event semaphore, rc = %d", CM_GET_THREAD_ID());
		iRet=CD_FAILURE;
	}

#else
	 /* crete mutex for the condition variable */
	if ( Mutex_Create (&es->mutex) != CD_SUCCESS )
	{
		return (CD_FAILURE);
	}

	/* Condition Variable attributes are currently not implemented, use NULL */
	iRet = pthread_cond_init (&es->cond, NULL);
	if ( iRet != 0 )
	{
		printf ("GLBSEM gs_get_event_sem error: thread (pthread_cond_init failed result=%d),", iRet);
		Mutex_Destroy (&es->mutex);
		return (CD_FAILURE);
	}
  /* successfully initialized the event semaphore */
  es->manualReset = manualReset;
#endif
	return (iRet);
}

CM_RET Wait_Event (CM_EVENT_SEM *es, CM_UINT32 timeout)
{
	CM_RET ret;
#ifdef _WIN32
	CM_UINT32 rc;
	rc = WaitForSingleObject(*es, timeout);

	if (rc == WAIT_FAILED)
	{
		printf ("GLBSEM error: WaitEvent rc=0x%04x, GLE=0x%04x, Thread %08lx", 
			(CM_UINT) rc, (CM_UINT)GetLastError (), CM_GET_THREAD_ID());

		ret = CD_FAILURE;
	}
	else if (rc == WAIT_TIMEOUT)
		ret = CD_TIMEOUT;
	else
		ret = CD_SUCCESS;
#else
	struct timespec tsDelta;
	struct timespec tsAbstime;
	struct timeval  curtimeval;

	Mutex_Get_tm (&es->mutex);

	 if ( timeout >= 0 )
	 {
		 tsDelta.tv_sec =   timeout / 1000L;
		 tsDelta.tv_nsec = (timeout % 1000L) * 1000000L;  /* nanoseconds */

		 gettimeofday (&curtimeval,NULL);
		 tsAbstime.tv_sec  = curtimeval.tv_sec;		/* convert timeval to timespec	*/
		 tsAbstime.tv_nsec = curtimeval.tv_usec*1000;	

		 tsAbstime.tv_sec  += tsDelta.tv_sec;
		 tsAbstime.tv_nsec += tsDelta.tv_nsec;	
		 /* adjust if (nsec) > 1000000000	*/
		 if (tsAbstime.tv_nsec / 1000000000 > 0)
		 {		/* in this case, should be EXACTLY = 1	*/
			 tsAbstime.tv_sec++;	/* add 1 second	*/
			 tsAbstime.tv_nsec = tsAbstime.tv_nsec % 1000000000;
		 }
	 }
	 if ( timeout < 0 )
      /* wait indefinitely	*/
      ret = pthread_cond_wait (&es->cond, &es->mutex);
    else
      /* wait for a period of time, if timeout=0 then fun returns immediatelly */
      ret = pthread_cond_timedwait (&es->cond, &es->mutex, &tsAbstime);

	 Mutex_Release (&es->mutex);
#endif

	return ret;
}

CM_RET Signal_Event(CM_EVENT_SEM *es)
{
	CM_RET        result = CD_FAILURE;
#ifdef _WIN32
	CM_BOOLEAN rc;

	rc = SetEvent(*es);
	if (rc == CD_FALSE)
	{
		printf ("GLBSEM error: SetEvent GLE=0x%04x", (CM_UINT) GetLastError ());
	}
	else
		result=CD_SUCCESS;
#else 
	
	if (!es)
	{
		return (result);
	}
	Mutex_Get_tm (&es->mutex);

//	es->predicate = predicate;
	if (es->manualReset == CD_TRUE)
		result = pthread_cond_broadcast (&es->cond);
	else
		result = pthread_cond_signal (&es->cond);

	/* release the mutex semaphore for the conditional variable	*/
	Mutex_Release (&es->mutex);

	if (result != 0)
	{
		printf ("GLBSEM _gs_sig_event_sem  (pthread_cond_signal(broadcast) failed result=%d)", result);
	}
	else
		result=CD_SUCCESS;
#endif
	return result;
}

CM_VOID Reset_Event (CM_EVENT_SEM *es)
{
#ifdef _WIN32
	CM_BOOLEAN rc;
	rc = ResetEvent (*es);
	if (rc == CD_FALSE)
	{
		printf ("GLBSEM error: ResetEvent gle=0x%04x", (unsigned int)GetLastError ());
	}
#else
	Mutex_Get_tm (&es->mutex);

	es->predicate = 0;

	/* release the mutex semaphore for the conditional variable	*/
	Mutex_Release (&es->mutex);
#endif
}


CM_VOID Release_Event (CM_EVENT_SEM *es)
{
	
#ifdef _WIN32
	CM_BOOLEAN rc = CloseHandle (*es);

	if (rc == CD_FALSE)
	{
		printf ("GLBSEM error: CloseHandle GLE=0x%04x", (unsigned int)GetLastError ());
	}
#else	
	CM_RET ret;
	ret = Mutex_Destroy (&es->mutex);
	if ( ret != CD_SUCCESS)
		return;  /* mutex probably busy, a thread is in the gs_wait_event_sem */

	/* free the conditional variable	*/
	ret = pthread_cond_destroy (&es->cond);
	if (ret != 0)
	{
		printf ("GLBSEM gs_free_event_sem error (pthread_cond_destroy failed result=%d)", ret);
	}
#endif

}

