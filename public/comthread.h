
#ifndef _CYTHREAD_H_
#define _CYTHREAD_H_

#include "platformdef.h"
#include "comtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CD_TIMEOUT		2
#define DEF_TOSTRING(x) #x

#ifdef  WIN32
#define AtomLockIncrement(a)     InterlockedExchangeAdd(&a, 1); 
#define AtomLockDecrement(a)     InterlockedExchangeAdd(&a, -1); 
#else
#define AtomLockIncrement(a)     __sync_fetch_and_add(&a, 1); 
#define AtomLockDecrement(a)     __sync_fetch_and_add(&a, -1); 
#endif

#if defined(_WIN32)				/* VC++, 32-Bit		*/

#define CM_THREAD_ID 		unsigned
#define CM_GET_THREAD_ID()		GetCurrentThreadId()
#define CM_MUTEX_SEM 		CRITICAL_SECTION
#define CM_EVENT_SEM 		HANDLE
#if (_MSC_VER >= 1300)
#define CM_THREAD_HANDLE 	uintptr_t
#else
#define CM_THREAD_HANDLE 	unsigned long
#endif
#define CM_THREAD_ARG 		void *
#define CM_THREAD_CALL_CONV	__cdecl
#define CM_THREAD_RET 		void *
#define CM_THREAD_RET_VAL	0
#endif  /* defined(_WIN32) */

#if defined(_LINUX)
#include <sys/prctl.h>

typedef struct
{
	CM_INT				predicate;		/* predicate to change value	*/
	pthread_mutex_t  	mutex;			/* mutex sem protecting the cond*/
	pthread_cond_t		cond;			/* condition variable		*/
	CM_BOOLEAN			manualReset;	/* type os event semaphore	*/
} CM_EVENT_SEM;

#define CM_GET_THREAD_ID()		pthread_self()
#define CM_MUTEX_SEM 			pthread_mutex_t

#define CM_THREAD_ID 		pthread_t
#define CM_THREAD_HANDLE 	pthread_t
#define CM_THREAD_ARG 		void *
#define CM_THREAD_CALL_CONV
#define CM_THREAD_RET 		void *
#define CM_THREAD_RET_VAL	NULL

#endif /* defined UNIX, linux,... */

CM_RET Thread_Start (CM_THREAD_RET (CM_THREAD_CALL_CONV *threadFunc) (CM_THREAD_ARG),
					 CM_THREAD_ARG     threadArg, 
					 CM_THREAD_HANDLE *threadHandleOut,
					 CM_THREAD_ID     *threadIdOut,
					 CM_INT           stackSize,
					 const CM_CHAR          *Name);

CM_RET Thread_Wait (CM_THREAD_HANDLE threadHandle, 
					CM_LONG timeout);

CM_RET Thread_Close (CM_THREAD_HANDLE threadHandle);

//获得线程个数
CM_INT GetThreadCnt();

#ifdef __cplusplus
}
#endif

#endif  //_CYTHREAD_H_



