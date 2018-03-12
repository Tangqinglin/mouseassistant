
#include "comthread.h"
/************************************************************************/

/************************************************************************/
static CM_INT s_iThreadCnt=0;
typedef struct
{
	CM_THREAD_RET (CM_THREAD_CALL_CONV *threadFunc) (CM_THREAD_ARG);
	CM_THREAD_ARG threadArg;
	CM_CHAR          *Name;
}tagTransmitter;

CM_THREAD_RET CM_THREAD_CALL_CONV TransmitterThread(CM_THREAD_ARG pArg)
{
	tagTransmitter *pTrtter=(tagTransmitter *)pArg;
	printf("**** TransmitterThread start name:%s \n",pTrtter->Name);
#ifndef WIN32
	prctl(PR_SET_NAME, (unsigned long)pTrtter->Name); 
#endif
	AtomLockIncrement(s_iThreadCnt);
	pTrtter->threadFunc(pTrtter->threadArg);
	AtomLockDecrement(s_iThreadCnt);
	free(pTrtter);
	return CM_THREAD_RET_VAL; 
}

CM_RET Thread_Start (CM_THREAD_RET (CM_THREAD_CALL_CONV *threadFunc) (CM_THREAD_ARG),
					 CM_THREAD_ARG     threadArg, 
					 CM_THREAD_HANDLE *threadHandleOut,
					 CM_THREAD_ID     *threadIdOut,
					 CM_INT           stackSize,
					 const CM_CHAR          *Name
					 )
{
#ifdef _LINUX
	pthread_attr_t attr,*pattr=0;
#endif
	CM_RET           ret;
#ifdef WIN32
	CM_THREAD_ID threadId;
#endif
	CM_THREAD_HANDLE threadHandle;
	tagTransmitter *pTrtter=(tagTransmitter *)malloc(sizeof(tagTransmitter));
	pTrtter->threadArg=threadArg;
	pTrtter->threadFunc=threadFunc;
	pTrtter->Name = (CM_CHAR*)Name;
#ifdef _LINUX
	if(stackSize>102400)
	{
		if(pthread_attr_init(&attr) == 0 && pthread_attr_setstacksize(&attr, stackSize) == 0)
			pattr = &attr; 
		else 
			printf("@@@@ pthread_attr_setstacksize to %d error!\r\n",stackSize);
	}
	ret = pthread_create (&threadHandle,  /* pointer for thread pointer	*/
		pattr,     /* if NULL, use default thread attr	*/
		TransmitterThread,	   /* thread funct to be executed	*/
		(CM_THREAD_ARG)pTrtter);	   /* thread funct argument		*/
	*threadHandleOut = threadHandle;
	*threadIdOut = threadHandle;	/* use thread handle as id	*/

	pthread_detach(threadHandle);
	if (ret != 0 )
#else
	threadHandle = (CM_THREAD_HANDLE)CreateThread(NULL, stackSize, 
		(LPTHREAD_START_ROUTINE)TransmitterThread, (LPVOID)pTrtter, 0, (LPDWORD)&threadId);
	CloseHandle((HANDLE)(threadHandle));

	*threadHandleOut=threadHandle;
	*threadIdOut = threadId;	/* use thread handle as id	*/

	if (threadHandle == 0)
#endif
	{
		free(pTrtter);
		ret = CD_FAILURE;
		printf ("Thread_Start create thread failed\n");
	}
	else
	{
		/* Thread created successfully */
		ret = CD_SUCCESS;
	}
	
	return (ret);
}

/************************************************************************/

/************************************************************************/
CM_RET Thread_Wait (CM_THREAD_HANDLE threadHandle, CM_LONG timeout)
{
	CM_RET ret;

#ifdef _LINUX
	CM_INT result;
	result = pthread_join (threadHandle, NULL);
	if ( result == 0 )
	{
		ret = CD_SUCCESS;
	}
	else
	{
		ret = CD_FAILURE;
	}
#else
	CM_INT32 result;
	result = WaitForSingleObject ((HANDLE)threadHandle, timeout);
	if (result != WAIT_OBJECT_0)
	{
		ret = CD_FAILURE;
	}
	else
	{
		ret = CD_SUCCESS;
	}
#endif
	

	return (ret);
}

/************************************************************************/

/************************************************************************/

CM_RET Thread_Close (CM_THREAD_HANDLE threadHandle)
{
	CM_RET ret = CD_SUCCESS;

#ifdef _LINUX
	/* nothing to clean up here */
#else
	if (threadHandle)
    {
		CloseHandle ((HANDLE)threadHandle);
	}
#endif
	return (ret);
}


CM_INT GetThreadCnt()
{
	return s_iThreadCnt;
}

