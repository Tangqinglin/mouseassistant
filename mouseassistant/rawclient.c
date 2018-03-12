/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    RawClient.h

Abstract:

--*/


#include "rawclient.h"


#pragma warning(disable:4201)

#include <setupapi.h>
#include <winioctl.h>

#pragma warning(default:4201)

//-----------------------------------------------------------------------------
// 4127 -- Conditional Expression is Constant warning
//-----------------------------------------------------------------------------
#define WHILE(constant) \
__pragma(warning(disable: 4127)) while(constant); __pragma(warning(default: 4127))

//*** 注意 ***
//返回的数据一定要用free释放
//*** 注意 ***
PSP_DEVICE_INTERFACE_DETAIL_DATA SearchRawPath(LPGUID pGuid)
{
	HDEVINFO                            hardwareDeviceInfo;
	SP_DEVICE_INTERFACE_DATA            deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA    deviceInterfaceDetailData = NULL;
	ULONG                               predictedLength = 0;
	ULONG                               requiredLength = 0;
	ULONG                               i = 0;

	// Open a handle to the device interface information set of all
	// present toaster class interfaces.
	hardwareDeviceInfo = SetupDiGetClassDevs(
		pGuid,
		NULL, // Define no enumerator (global)
		NULL, // Define no
		(DIGCF_PRESENT | // Only Devices present
			DIGCF_DEVICEINTERFACE)); // Function class devices.
	if (INVALID_HANDLE_VALUE == hardwareDeviceInfo)
	{
		printf("SetupDiGetClassDevs failed: %x\n", GetLastError());
		return NULL;
	}
	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	i = 0;
	// Enumerate devices of toaster class
	do {
		if (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, 0, // No care about specific PDOs
			pGuid, i, &deviceInterfaceData))
		{
			if (deviceInterfaceDetailData) {
				free(deviceInterfaceDetailData);
				deviceInterfaceDetailData = NULL;
			}
			// First find out required length of the buffer
			if (!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo,&deviceInterfaceData,
				NULL, // probing so no output buffer yet
				0, // probing so output buffer length of zero
				&requiredLength,NULL))// not interested in the specific dev-node
			{ 
				if (ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
					printf("SetupDiGetDeviceInterfaceDetail failed %d\n", GetLastError());
					SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
					return NULL;
				}
			}
			predictedLength = requiredLength;
			deviceInterfaceDetailData = malloc(predictedLength);
			if (deviceInterfaceDetailData) {
				deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			}
			else {
				printf("Couldn't allocate %d bytes for device interface details.\n", predictedLength);
				SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
				return NULL;
			}


			if (!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInterfaceData, deviceInterfaceDetailData,
				predictedLength, &requiredLength, NULL))
			{
				printf("Error in SetupDiGetDeviceInterfaceDetail\n");
				SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
				free(deviceInterfaceDetailData);
				return NULL;
			}
			else
				break;
		}
		else if (ERROR_NO_MORE_ITEMS != GetLastError()) {
			free(deviceInterfaceDetailData);
			deviceInterfaceDetailData = NULL;
			continue;
		}
		else
			break;

	} WHILE(TRUE);
	SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

	return deviceInterfaceDetailData;
}

//
//
//
int SetMouseData(const char *DevicePath, MOUSE_INPUT_DATA  *pMsData)
{
	HANDLE                  file;
	ULONG                   requiredLength = 0;
    file = CreateFile (DevicePath,
                        GENERIC_READ | GENERIC_WRITE,0,NULL, // no SECURITY_ATTRIBUTES structure
                        OPEN_EXISTING, // No special create flags
                        0, // No special attributes
                        NULL);

    if (INVALID_HANDLE_VALUE == file) {
        return GetLastError();
    }
    
    //
    // Send an IOCTL to retrive the keyboard attributes
    // These are cached in the kbfiltr
    //
    
    if (!DeviceIoControl (file,IOCTL_DT_SET_MOUSE_DATA,
		pMsData, sizeof(MOUSE_INPUT_DATA), NULL, 0,
        &requiredLength, NULL)) 
	{
        CloseHandle(file);
        return  GetLastError();
    } 
    
    CloseHandle(file);
    return 0;
}



