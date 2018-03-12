/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    RawClient.h

Abstract:


--*/

#include "platformdef.h"
#include <wtypes.h>
#include <initguid.h>
#include <ntddkbd.h>
#include <setupapi.h>
#include "ntddmou.h"

#include "..\DTMouseDriver\Public.h"

#ifdef __cplusplus
extern "C" {
#endif

	PSP_DEVICE_INTERFACE_DETAIL_DATA SearchRawPath(LPGUID pGuid);

	int SetMouseData(const char *DevicePath, MOUSE_INPUT_DATA  *pMsData);

#ifdef __cplusplus
}
#endif
