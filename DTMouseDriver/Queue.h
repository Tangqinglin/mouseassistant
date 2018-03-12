/*++

Module Name:

    queue.h

Abstract:

    This file contains the queue definitions.

Environment:

    Kernel-mode Driver Framework

--*/

EXTERN_C_START

//
// This is the context that can be placed per queue
// and would contain per queue information.
//
typedef struct _QUEUE_CONTEXT {

    ULONG PrivateDeviceData;  // just a placeholder

} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, QueueGetContext)

NTSTATUS
DTLM_QueueInitialize(
    _In_ WDFDEVICE Device
    );

//
// Events from the IoQueue object
//
VOID DTLM_EvtIoDefault(
	_In_ WDFQUEUE Queue,
	_In_ WDFREQUEST Request);

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL DTLM_EvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_STOP DTLM_EvtIoStop;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL DTLM_EvtIoInternalDeviceControl;


VOID DTLM_EvtIoRead(
	IN WDFQUEUE         Queue,
	IN WDFREQUEST       Request,
	IN size_t            Length
);

VOID DTLM_EvtIoWrite(
	IN WDFQUEUE         Queue,
	IN WDFREQUEST       Request,
	IN size_t            Length
);

VOID DTLM_EvtCanceledOnQueue(
	IN WDFQUEUE   Queue,
	IN WDFREQUEST Request
);

VOID DTLM_EvtIoResume(
	IN WDFQUEUE   Queue,
	IN WDFREQUEST Request
);

//VOID DTLM_ServiceDpc(
//	_In_ struct _KDPC  *Dpc,
//	_In_opt_ PVOID  DeferredContext,
//	_In_opt_ PVOID  SystemArgument1,
//	_In_opt_ PVOID  SystemArgument2);

VOID DTLM_EvtIoDeviceControlFromRawPdo(
	IN WDFQUEUE      Queue,
	IN WDFREQUEST    Request,
	IN size_t        OutputBufferLength,
	IN size_t        InputBufferLength,
	IN ULONG         IoControlCode
);

EXTERN_C_END
