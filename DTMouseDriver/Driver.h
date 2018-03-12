/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>

#include "device.h"
#include "queue.h"
#include "trace.h"

#include <ntstrsafe.h>
#include <devguid.h>

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD DTMouseDriverEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP DTMouseDriverEvtDriverContextCleanup;

EXTERN_C_END
