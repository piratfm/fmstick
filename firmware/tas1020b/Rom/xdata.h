//=============================================================================
// xdata.h
//
// This module contains defines and structures related to data stored in 
// xdata.
//
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//============================================================================= 

#ifndef _XDATA_H
#define _XDATA_H


#define XDATA_BUFFER_START	0xFA10


typedef struct _Rom_Struct_ {
    // Boot Type: from eeprom or from host
    byte bootType; 
    
    // Rom state: In the boot process or ready for Application
    byte state;
    
    // ROM attribute, bit definitions
    // bit set to supporthe feature
    byte attribute;
#define ROM_MICROSPEED_24MHZ        0x01
#define ROM_MICROSPEED_12MHZ        0x00
#define ROM_EEPROM_WORD_ACCESS_MODE 0x02
#define ROM_EEPROM_BYTE_ACCESS_MODE 0x00
    
//    byte dummy;
} ROM_STRUCT;

typedef struct _Usb_Request_Struct_{
    byte bmRequest;
    byte bRequest;
    byte lowValue;
    byte hiwValue;
    byte lowIndex;
    byte hiwIndex;
    word wLength;
    byte status;
    byte *dataPtr;
    word xferLength;
    word temp;
    byte intSource;
    byte ep0MaxPacket;
} USB_REQUEST_STRUCT;

typedef struct _tag_I2C_PARAMS
{
    BYTE SlaveAddr;
    unsigned int SubAddr;
    BYTE *pBuffer;
    unsigned int nLen;
    BYTE Flags;
    
} I2C_PARAMS;


typedef struct _tag_SHARED_DATA
{
    BYTE Status;
    union
    {
        I2C_PARAMS I2CParams;
    } Params;
    
} SHARED_DATA;

typedef struct _tag_DEV_INTVECTOR
{
    byte Vector;
}   DEV_INTVECTOR;

typedef struct _tag_DEV_CLASS
{
    byte Index;
}   DEV_CLASS;

typedef struct _tag_DEV_STATUS
{
    byte Index;
}   DEV_STATUS;

typedef struct _tag_DEV_USB_REQUEST
{
    byte *UsbReq;
}   DEV_USB_REQUEST;


typedef struct _tag_DEV_DESC
{
    word *Size;
    byte Index;
}   DEV_DESC;

typedef struct _tag_DEV_IF
{
    byte Index;
    byte Setting;
}   DEV_IF;


typedef struct _tag_DEV_CONFIG
{
    byte Index;
}   DEV_CONFIG;

typedef struct _tag_DEV_FEATURE
{
    byte Index;
}   DEV_FEATURE;

typedef struct _tag_DEV_MIXER
{
    byte UnitId;
    byte TypeVal;
    byte Input;
    byte Output;
    word wLength;
}   DEV_MIXER;

typedef struct _tag_DEV_HID
{
    byte Index;
    byte ReportType;
    byte ReportId;
}   DEV_HID;

typedef struct _tag_DEV_AUDIO
{
    byte Index;
    byte Channel;
    byte TypeVal;
}   DEV_AUDIO;

typedef struct _tag_DEV_DFU
{
    word wLength;
    byte *pData;
}   DEV_DFU;

typedef struct _tag_DEV_DFU_GETSTATUS
{
    byte state;
    byte status;
}   DEV_DFU_GETSTATUS;

typedef struct _tag_DEV_DFU_SETUP
{
    byte Target;
    byte *PollTablePtr;
} DEV_DFU_SETUP;

typedef struct _tag_DEV_DFU_RETURN
{
    byte Status;
    byte DfuStatus;
    byte LoadStatus;
    byte MnfState;
    word Length;
} DEV_DFU_RETURN;

typedef struct _tag_DEV_FUNC_PARAM
{
    union
    {
        DEV_DFU_SETUP DevDfuSetup;
    } romFunc;
    
    union
    {
        word wValue;
        byte Value;
        byte *pData;
        DEV_DFU_RETURN DevDfuRetValue;
    } out;
    
    union
    {
        DEV_INTVECTOR DevInt;
        DEV_CLASS DevClass;
        DEV_STATUS DevStatus;
        DEV_USB_REQUEST DevUsbReq;
        DEV_DESC DevDesc;
        DEV_IF DevIf;
        DEV_CONFIG DevConfig;
        DEV_FEATURE DevFeature;
        DEV_MIXER DevMixer;
        DEV_HID DevHid; 
        DEV_AUDIO DevAudio;
        DEV_DFU DevDfu;
        DEV_DFU_GETSTATUS DevDfuStatus;
    } in;
    
} DEV_FUNC_PARAM;

typedef struct  _tag_XDATA
{
    DEV_FUNC_PARAM DevSharedData; 
    SHARED_DATA RomSharedData;
    ROM_STRUCT  RomRecord;
    
    BYTE I2CStatusWaitValue;
    unsigned int nTimeoutMicroSec;
} XDATA_STRUCT;

#ifndef _XDATA_C
extern XDATA_STRUCT volatile pdata Externaldata;
extern USB_REQUEST_STRUCT volatile data UsbRequest;
#endif

#define RomRecord       Externaldata.RomRecord
#define I2CParams		Externaldata.RomSharedData.Params.I2CParams
#define I2CStatusWaitValue 	Externaldata.I2CStatusWaitValue
#define nTimeoutMicroSec	Externaldata.nTimeoutMicroSec 
#define DEV_SHARED_DATA     Externaldata.DevSharedData

#endif