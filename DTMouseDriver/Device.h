/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include "public.h"
#include <kbdmou.h>
#include <ntddmou.h>
//#include <ntdd8042.h>
#include <wdf.h>

EXTERN_C_START

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
    ULONG PrivateDeviceData;  // just a placeholder
							  //
							  // Previous hook routine and context
							  //                               
	PVOID UpperContext;

	// Context for IsrWritePort, QueueMousePacket
	//
	IN PVOID CallContext;

	// The real connect data that this driver reports to
	//
	CONNECT_DATA UpperConnectData;

	//KDPC           ServiceDpc;
	//KTIMER         ServiceTme;
	//LARGE_INTEGER     dueTime;

	// Queue for handling requests that come from the rawPdo
	//
	WDFQUEUE rawPdoQueue;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
DTMouseDriverCreateDevice(
	_Inout_ PWDFDEVICE_INIT DeviceInit
);



//
// IOCTL Related defintions
//


#define  DTLM_DEVICE_ID L"Raw\\DT6677"


typedef struct _RPDO_DEVICE_DATA
{

	ULONG InstanceNo;

	//
	// Queue of the parent device we will forward requests to
	//
	WDFQUEUE ParentQueue;

} RPDO_DEVICE_DATA, *PRPDO_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(RPDO_DEVICE_DATA, PdoGetData)


NTSTATUS
DTLM_CreateRawPdo(
	WDFDEVICE       Device,
	ULONG           InstanceNo
);

EXTERN_C_END
