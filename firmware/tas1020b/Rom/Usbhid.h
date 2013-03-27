//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
#ifndef _USBHID_H
#define _USBHID_H

#define HID_GET_REPORT		0x01
#define HID_GET_IDLE			0x02
#define HID_GET_PROTOCOL	0x03

#define HID_SET_REPORT		0x09
#define HID_SET_IDLE			0x0A
#define HID_SET_PROTOCOL	0x0B

// prototype
extern void hidHandler();

#endif