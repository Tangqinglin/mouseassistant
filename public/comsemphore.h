
#ifndef _CYSEMPHORE_H_
#define _CYSEMPHORE_H_

#include "platformdef.h"
#include "comtypes.h"
#include "comthread.h"

#ifdef __cplusplus
extern "C" {
#endif
	///////////////////////////////////////////////////////////
	//互斥接口
	CM_RET Mutex_Create (CM_MUTEX_SEM *ms);
	CM_RET Mutex_Destroy (CM_MUTEX_SEM *ms);
	CM_RET Mutex_Get_tm (CM_MUTEX_SEM *ms);
	CM_BOOLEAN Mutex_Try_tm (CM_MUTEX_SEM *ms);
	CM_RET Mutex_Release (CM_MUTEX_SEM *ms);

	//////////////////////////////////////////////////////////////
	//事件接口
	CM_RET Get_Event(CM_EVENT_SEM *es,CM_BOOLEAN manualReset);
	CM_RET Wait_Event (CM_EVENT_SEM *es, CM_UINT32 timeout);
	CM_RET Signal_Event(CM_EVENT_SEM *es);
	CM_VOID Reset_Event (CM_EVENT_SEM *es);
	CM_VOID Release_Event (CM_EVENT_SEM *es);

#ifdef __cplusplus
}
#endif

#endif  //_CYSEMPHORE_H_



