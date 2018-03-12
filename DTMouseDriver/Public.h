/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/
#ifndef _PUBLIC_H
#define _PUBLIC_H
//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_DTMouseDriver,
    0x7661323c,0xad59,0x404a,0x9d,0xb7,0x09,0x9e,0x54,0x5c,0x9b,0x4e);
// {7661323c-ad59-404a-9db7-099e545c9b4e}

#define IOCTL_INDEX             0x800

#define IOCTL_DT_SET_MOUSE_DATA CTL_CODE( FILE_DEVICE_MOUSE,   \
                                                        IOCTL_INDEX,    \
                                                        METHOD_BUFFERED,    \
                                                        FILE_WRITE_DATA)

DEFINE_GUID(GUID_DEVINTERFACE_DTMOUSE,
	0x16867996, 0x8D00, 0x4802, 0x97, 0xD6, 0x0C, 0x28, 0x41, 0x62, 0x19, 0x81);
//{16867996 - 8D00 - 4802 - 97D6 - 0C2841621981}


#endif
