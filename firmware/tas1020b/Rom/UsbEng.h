//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
/****************************************************************************** 
UsbEng.h
header file.
Note: As the module goal is to use the global variable allocated in XDATA
the defined structures only contain variable in byte size.
******************************************************************************/
#ifndef _USBENG_H
#define _USBENG_H

// usb FW result
#define EVENT_OK		          0
#define EVENT_ERROR		        1

// For handling USB data tx/Rx
#define ENG_TX_COMPLETE       1
#define ENG_TX_INCOMPLETE     0
#define ENG_RX_ERROR          0
#define ENG_RX_COMPLETE       1
#define ENG_RX_INCOMPLETE     2


// this structures is used internally for 
// the operation of the USB engine
typedef struct _Eng_Parms_Struct_{
    // usb event types
#define USB_NO_EVENT			0x00
#define USB_READ_EVENT		0x01
#define USB_WRITE_EVENT		0x02
#define USB_RESET_EVENT		0x03
#define USB_SUSPEND_EVENT	0x04
#define USB_RESUME_EVENT	0x05
    byte event;
    
    // usb state machine defines  
#define USB_WAIT_SETUP		0x01
#define USB_WAIT_DATA_OUT	0x02
#define USB_WAIT_DATA_IN	0x03
#define USB_WAIT_FW_RESULT	0x04
#define USB_WAIT_FW_DATA	0x05
#define USB_WAIT_IN_ACK		0x06
#define USB_WAIT_OUT_ACK	0x07
#define USB_WAIT_ADDR_ACK 0x08
    byte state;
    
    // store interrupt source
    //  move to UsbRequest: byte intSource;  
    
    // bit flags
#define ENG_BIT_EXCEPT      0x01
#define ENG_BIT_SUSPEND     0x02
    byte bitFlags; 
    
    // device address
    //  byte address;
    
    // result of tx/rx operation
    byte xferStatus;
    
    // use for keep track of data received or sent
    word dataCount;
    
    //  Max control endpoint packet size
    //  moved to UsbRequest: byte ep0MaxPacket;
} ENG_PARSM_STRUCT; 


// References
extern ENG_PARSM_STRUCT data EngParms;

// prototypes
void engSuspendIntrHandler(void);
void engResumeIntrHandler(void);
void engXintHandler (void);
void engResumeIntrHandler (void);
void engSuspendIntrHandler (void);
void engEp0SetupDone(void);
void engEp0TxDone(void);
void engEp0RxDone(void);
void engUsbClearEvent(void);
void engLoadTxFifo();
void engSaveRxFifo(void);
void engUsbInit(void);
void engUsbInitAcg(void);  
void engSoftPll(void);
void engInitWaiSetup();



#endif