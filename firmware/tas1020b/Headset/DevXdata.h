//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
#ifndef _DEVXDATA_H
#define _DEVXDATA_H

//
// Rounds specified number to the next multiple of 8.
//
#define ROUND_UP(s)     ((s) + ((8 - ((s) % 8)) % 8))

// 
// Hid report structure.
//
typedef struct _HID_REPORT
{
    byte bReportItem[42];
}   HID_REPORT;

typedef struct _HIDO_REPORT
{
    byte bReportOut[42];
}   HIDO_REPORT;

//
// USB in/out endpoint buffers sizes
//
#define OUTEP1_BUFFER_SIZE          (DEV_SPK_MAX_PKT)
#define INEP2_BUFFER_SIZE           (DEV_MIC_MAX_PKT)

//
// Note elements in the structures need to be 8 byte aligned.
//

typedef struct ALIGNED_XDATA_BUFFERS_tag
{
    //
    // USB AS out
    //
    BYTE OutEp1XBuffer[ROUND_UP(OUTEP1_BUFFER_SIZE)];

    //
    // USB AS in
    //
    BYTE InEp2XBuffer[ROUND_UP(INEP2_BUFFER_SIZE)];

    //
    // HID report buffer
    //
    union
    {
        HID_REPORT HidReport;
        BYTE BufferIn[ROUND_UP(sizeof(HID_REPORT))];
    } InEp3;

    union
    {
        HIDO_REPORT HidOReport;
        BYTE BufferOut[ROUND_UP(sizeof(HIDO_REPORT))];
    } OutEp4;

        
} ALIGNED_XDATA_BUFFERS;

//
// Start address for xdata buffer data
//
#define USB_EP0_XFERDATA_SIZE 0x10

#define ALIGNED_XDATA_BUFFERS_ADDR	(INPACK_ADDR + 0x10 + USB_EP0_XFERDATA_SIZE)
#define ALIGNED_XDATA_ADDR(s)       (ALIGNED_XDATA_BUFFERS_ADDR + offsetof(ALIGNED_XDATA_BUFFERS, s))

extern ALIGNED_XDATA_BUFFERS xdata Xdata;

#endif