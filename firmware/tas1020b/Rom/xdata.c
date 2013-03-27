//=============================================================================
// xdata.c
//
// This module contains the structure containing xdata data structures.
//
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//============================================================================= 

#define _XDATA_C
#include "..\Rom\types.h"
#include "..\Rom\Eeprom.h"
#include "..\Rom\usbeng.h"
#include "..\Rom\romboot.h"
#include "..\Rom\usbdfu.h"
#include "..\Rom\xdata.h"

// XDATA_STRUCT pdata Externaldata _at_ XDATA_BUFFER_START;
XDATA_STRUCT volatile pdata Externaldata;

// USB Request
// USB_REQUEST_STRUCT pdata UsbRequest;
USB_REQUEST_STRUCT volatile data UsbRequest _at_ (0x80 - sizeof(USB_REQUEST_STRUCT));

//usb state machine		
//ENG_PARSM_STRUCT pdata EngParms;
ENG_PARSM_STRUCT volatile data EngParms _at_ (0x80 - sizeof(USB_REQUEST_STRUCT) - sizeof(ENG_PARSM_STRUCT));

// DFU state machine data
DFU_STRUCT volatile pdata DfuStateMachine;

// DFU mode device record
//DFU_DEVICE_STRUCT pdata DfuDevice;
