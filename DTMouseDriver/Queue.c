/*++

Module Name:

    queue.c

Abstract:

    This file contains the queue entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "queue.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, DTLM_QueueInitialize)
//#pragma alloc_text (PAGE, DTLM_EvtIoInternalDeviceControl)
//#pragma alloc_text (PAGE, DTLM_EvtIoDefault)
#endif

#pragma warning(push)
#pragma warning(disable:4055) // type case from PVOID to PSERVICE_CALLBACK_ROUTINE
#pragma warning(disable:4152) // function/data pointer conversion in expression

NTSTATUS DTLM_QueueInitialize(
    _In_ WDFDEVICE Device
    )
/*++

Routine Description:

     The I/O dispatch callbacks for the frameworks device object
     are configured in this function.

     A single default I/O Queue is configured for parallel request
     processing, and a driver context memory allocation is created
     to hold our structure QUEUE_CONTEXT.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    VOID

--*/
{
    WDFQUEUE queue;
    NTSTATUS status= STATUS_SUCCESS;
    WDF_IO_QUEUE_CONFIG queueConfig;

    PAGED_CODE();

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
		WdfIoQueueDispatchParallel
        );

	queueConfig.EvtIoRead = DTLM_EvtIoRead;
	queueConfig.EvtIoWrite = DTLM_EvtIoWrite;
	queueConfig.EvtIoInternalDeviceControl = DTLM_EvtIoInternalDeviceControl;
    queueConfig.EvtIoDeviceControl = DTLM_EvtIoDeviceControl;
	queueConfig.EvtIoCanceledOnQueue = DTLM_EvtCanceledOnQueue;
	queueConfig.EvtIoResume = DTLM_EvtIoResume;
    queueConfig.EvtIoStop = DTLM_EvtIoStop;
	queueConfig.EvtIoDefault = DTLM_EvtIoDefault;
	queueConfig.PowerManaged = WdfFalse;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 WDF_NO_OBJECT_ATTRIBUTES,
                 &queue
                 );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    return status;
}

VOID
DTLM_EvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
    )
/*++
Routine Description:

    This event is invoked when the framework receives IRP_MJ_DEVICE_CONTROL request.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    OutputBufferLength - Size of the output buffer in bytes

    InputBufferLength - Size of the input buffer in bytes

    IoControlCode - I/O control code.

Return Value:

    VOID

--*/
{
	WDF_REQUEST_PARAMETERS        devParam;

    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p OutputBufferLength %d InputBufferLength %d IoControlCode %d", 
                Queue, Request, (int) OutputBufferLength, (int) InputBufferLength, IoControlCode);

	WdfRequestGetParameters(Request, &devParam);
	KdPrint(("DT ########################################### type=%d \n", devParam.Type));
    WdfRequestComplete(Request, STATUS_SUCCESS);

    return;
}

VOID DTLM_EvtIoStop(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ ULONG ActionFlags
)
/*++

Routine Description:

    This event is invoked for a power-managed queue before the device leaves the working state (D0).

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    ActionFlags - A bitwise OR of one or more WDF_REQUEST_STOP_ACTION_FLAGS-typed flags
                  that identify the reason that the callback function is being called
                  and whether the request is cancelable.

Return Value:

    VOID

--*/
{
	WDF_REQUEST_PARAMETERS        devParam;
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p ActionFlags %d", 
                Queue, Request, ActionFlags);

    //
    // In most cases, the EvtIoStop callback function completes, cancels, or postpones
    // further processing of the I/O request.
    //
    // Typically, the driver uses the following rules:
    //
    // - If the driver owns the I/O request, it calls WdfRequestUnmarkCancelable
    //   (if the request is cancelable) and either calls WdfRequestStopAcknowledge
    //   with a Requeue value of TRUE, or it calls WdfRequestComplete with a
    //   completion status value of STATUS_SUCCESS or STATUS_CANCELLED.
    //
    //   Before it can call these methods safely, the driver must make sure that
    //   its implementation of EvtIoStop has exclusive access to the request.
    //
    //   In order to do that, the driver must synchronize access to the request
    //   to prevent other threads from manipulating the request concurrently.
    //   The synchronization method you choose will depend on your driver's design.
    //
    //   For example, if the request is held in a shared context, the EvtIoStop callback
    //   might acquire an internal driver lock, take the request from the shared context,
    //   and then release the lock. At this point, the EvtIoStop callback owns the request
    //   and can safely complete or requeue the request.
    //
    // - If the driver has forwarded the I/O request to an I/O target, it either calls
    //   WdfRequestCancelSentRequest to attempt to cancel the request, or it postpones
    //   further processing of the request and calls WdfRequestStopAcknowledge with
    //   a Requeue value of FALSE.
    //
    // A driver might choose to take no action in EvtIoStop for requests that are
    // guaranteed to complete in a small amount of time.
    //
    // In this case, the framework waits until the specified request is complete
    // before moving the device (or system) to a lower power state or removing the device.
    // Potentially, this inaction can prevent a system from entering its hibernation state
    // or another low system power state. In extreme cases, it can cause the system
    // to crash with bugcheck code 9F.
    //

	WdfRequestGetParameters(Request, &devParam);
	KdPrint(("DT ########################################### type=%d \n", devParam.Type));
	WdfRequestComplete(Request, STATUS_SUCCESS);
    return;
}


VOID DTLM_EvtIoInternalDeviceControl(
	IN WDFQUEUE      Queue,
	IN WDFREQUEST    Request,
	IN size_t        OutputBufferLength,
	IN size_t        InputBufferLength,
	IN ULONG         IoControlCode
)
/*++
Routine Description:
This routine is the dispatch routine for internal device control requests.
There are two specific control codes that are of interest:
IOCTL_INTERNAL_MOUSE_CONNECT:
Store the old context and function pointer and replace it with our own.
This makes life much simpler than intercepting IRPs sent by the RIT and
modifying them on the way back up.--*/
{

	PDEVICE_CONTEXT           devExt;
	PCONNECT_DATA               connectData;
	NTSTATUS                   status = STATUS_SUCCESS;
	WDFDEVICE                 hDevice;
	size_t                           length;

	UNREFERENCED_PARAMETER(OutputBufferLength);
	UNREFERENCED_PARAMETER(InputBufferLength);

	//PAGED_CODE(); //导致系统崩溃
	
	hDevice = WdfIoQueueGetDevice(Queue);
	devExt = DeviceGetContext(hDevice);

	switch (IoControlCode) {
		// Connect a mouse class device driver to the port driver.
	case IOCTL_INTERNAL_MOUSE_CONNECT:
		// Only allow one connection.
		if (devExt->UpperConnectData.ClassService != NULL) {
			status = STATUS_SHARING_VIOLATION;
			break;
		}
		// Copy the connection parameters to the device extension.
		status = WdfRequestRetrieveInputBuffer(Request,
			sizeof(CONNECT_DATA),&connectData,&length);

		if (!NT_SUCCESS(status)) {
			KdPrint(("WdfRequestRetrieveInputBuffer failed %x\n", status));
			break;
		}
		devExt->UpperConnectData = *connectData;

		// Hook into the report chain.  Everytime a mouse packet is reported to
		// the system, DTLM_ServiceCallback will be called
		//connectData->ClassDeviceObject = WdfDeviceWdmGetDeviceObject(hDevice);
		//connectData->ClassService = DTLM_ServiceCallback;

		//KeSetTimerEx(&devExt->ServiceTme, devExt->dueTime,1000,&devExt->ServiceDpc);
		break;

		// Disconnect a mouse class device driver from the port driver.
	case IOCTL_INTERNAL_MOUSE_DISCONNECT:
		// Clear the connection parameters in the device extension.
		//KeCancelTimer(&devExt->ServiceTme);
		devExt->UpperConnectData.ClassDeviceObject = NULL;
	    devExt->UpperConnectData.ClassService = NULL;

		status = STATUS_NOT_IMPLEMENTED;
		break;
		// Might want to capture this in the future.  For now, then pass it down
		// the stack.  These queries must be successful for the RIT to communicate
		// with the mouse.
		//
	case IOCTL_MOUSE_QUERY_ATTRIBUTES:
	default:
		length = IOCTL_MOUSE_QUERY_ATTRIBUTES;
		length = IOCTL_MOUSE_INSERT_DATA;
		length = IOCTL_INTERNAL_MOUSE_ENABLE;
		length = IOCTL_INTERNAL_MOUSE_DISABLE;
		KdPrint(("DT #####################L=%d type=%d \n", __LINE__, IoControlCode));
		break;
	}

	if (!NT_SUCCESS(status)) {
		WdfRequestComplete(Request, status);
		return;
	}

	//直接结束
	WdfRequestComplete(Request, status);

	//DTLM_DispatchPassThrough(Request, WdfDeviceGetIoTarget(hDevice));
}

VOID DTLM_EvtIoDefault(
	_In_ WDFQUEUE Queue,
	_In_ WDFREQUEST Request)
{
	NTSTATUS                   status = STATUS_SUCCESS;
	WDF_REQUEST_PARAMETERS        devParam;

	//PAGED_CODE();
	UNREFERENCED_PARAMETER(Queue);

	WdfRequestGetParameters(Request, &devParam);
	KdPrint(("DT #####################L=%d type=%d \n", __LINE__, devParam.Type));
	//直接结束
	WdfRequestComplete(Request, status);
}

VOID DTLM_EvtIoRead(
	IN WDFQUEUE         Queue,
	IN WDFREQUEST       Request,
	IN size_t            Length
)
{
	NTSTATUS                   status = STATUS_SUCCESS;
	WDF_REQUEST_PARAMETERS        devParam;
	UNREFERENCED_PARAMETER(Queue);
	UNREFERENCED_PARAMETER(Length);

	WdfRequestGetParameters(Request, &devParam);
	KdPrint(("DT ########################################### type=%d \n", devParam.Type));
	WdfRequestComplete(Request, status);
}

VOID DTLM_EvtIoWrite(
	IN WDFQUEUE         Queue,
	IN WDFREQUEST       Request,
	IN size_t            Length
)
{
	NTSTATUS                   status = STATUS_SUCCESS;
	WDF_REQUEST_PARAMETERS        devParam;
	UNREFERENCED_PARAMETER(Queue);
	UNREFERENCED_PARAMETER(Length);

	WdfRequestGetParameters(Request, &devParam);
	KdPrint(("DT ########################################### type=%d \n", devParam.Type));
	WdfRequestComplete(Request, status);
}

VOID DTLM_EvtCanceledOnQueue(
	IN WDFQUEUE   Queue,
	IN WDFREQUEST Request
)
{
	NTSTATUS                   status = STATUS_SUCCESS;
	WDF_REQUEST_PARAMETERS        devParam;
	UNREFERENCED_PARAMETER(Queue);

	WdfRequestGetParameters(Request, &devParam);
	KdPrint(("DT ########################################### type=%d \n", devParam.Type));
	WdfRequestComplete(Request, status);
}

VOID DTLM_EvtIoResume(
	IN WDFQUEUE   Queue,
	IN WDFREQUEST Request
)
{
	NTSTATUS                   status = STATUS_SUCCESS;
	WDF_REQUEST_PARAMETERS        devParam;
	UNREFERENCED_PARAMETER(Queue);

	WdfRequestGetParameters(Request, &devParam);
	KdPrint(("DT ########################################### type=%d \n", devParam.Type));
	WdfRequestComplete(Request, status);
}


VOID DTLM_EvtIoDeviceControlFromRawPdo(
	IN WDFQUEUE      Queue,
	IN WDFREQUEST    Request,
	IN size_t        OutputBufferLength,
	IN size_t        InputBufferLength,
	IN ULONG         IoControlCode
)
/*++
Routine Description:
This routine is the dispatch routine for device control requests.
Arguments:

Queue - Handle to the framework queue object that is associated
with the I/O request.
Request - Handle to a framework request object.

OutputBufferLength - length of the request's output buffer,
if an output buffer is available.
InputBufferLength - length of the request's input buffer,
if an input buffer is available.

IoControlCode - the driver-defined or system-defined I/O control code
(IOCTL) that is associated with the request.

Return Value:
VOID
--*/
{
	NTSTATUS status = STATUS_SUCCESS;
	WDFDEVICE hDevice;
	WDFMEMORY inMemory;
	PDEVICE_CONTEXT devExt;
	size_t bytesTransferred = 0;
	MOUSE_INPUT_DATA upData;
	ULONG InDataConsumed = 1;
	PMOUSE_INPUT_DATA InputDataEnd;

	UNREFERENCED_PARAMETER(OutputBufferLength);

	//KdPrint(("DT MOUSE_EvtIoInternalDeviceControl\n"));

	hDevice = WdfIoQueueGetDevice(Queue);
	devExt = DeviceGetContext(hDevice);

	// Process the ioctl and complete it when you are done.
	switch (IoControlCode) {
	case IOCTL_DT_SET_MOUSE_DATA:
		// Buffer is too small, fail the request
		//
		if (InputBufferLength < sizeof(MOUSE_INPUT_DATA)) {
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		status = WdfRequestRetrieveInputMemory(Request, &inMemory);
		if (!NT_SUCCESS(status)) {
			KdPrint(("DT WdfRequestRetrieveOutputMemory failed %x\n", status));
			break;
		}
		status = WdfMemoryCopyToBuffer(inMemory,
			0, &upData,sizeof(MOUSE_INPUT_DATA));
		if (!NT_SUCCESS(status)) {
			KdPrint(("DT WdfMemoryCopyFromBuffer failed %x\n", status));
			break;
		}
		bytesTransferred = sizeof(MOUSE_INPUT_DATA);

		InputDataEnd = &upData;
		InputDataEnd += InDataConsumed;
		(*(PSERVICE_CALLBACK_ROUTINE)devExt->UpperConnectData.ClassService)(
			devExt->UpperConnectData.ClassDeviceObject,
			&upData,
			InputDataEnd,
			&InDataConsumed);
		KdPrint(("DT... mouse data  x=%d   y=%d\n", upData.LastX, upData.LastY));
		break;
	default:
		status = STATUS_NOT_IMPLEMENTED;
		break;
	}

	WdfRequestCompleteWithInformation(Request, status, bytesTransferred);

	return;
}
