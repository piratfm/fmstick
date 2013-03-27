//================================================== 
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//================================================== 
/*==================================================  
USBDFU.H: Handler for USB Dfu class
Refernce: USB DFU Specs, Revision 1.0
History:
- 02/28/2000: Initial

================================================== */
#ifndef _USBDFU_H
#define _USBDFU_H
#include "usb.h"
#include "eeprom.h"

// DFU Specific Class-Specific Requests
#define DFU_DETACH      0
#define DFU_DNLOAD      1
#define DFU_UPLOAD      2
#define DFU_GETSTATUS   3
#define DFU_CLRSTATUS   4
#define DFU_GETSTATE    5
#define DFU_ABORT       6

// DFU statuses
#define DFU_STATUS_OK               0x00  // No error condition present
#define DFU_STATUS_errTARGET        0x01  // File is not targeted file used by this device
#define DFU_STATUS_errFILE          0x02  // File is OK but failed vendor-specific request
#define DFU_STATUS_errWRITE         0x03  // Device is unable to write memory 
#define DFU_STATUS_errERASE         0x04  // Memory erase function failed
#define DFU_STATUS_errCHECK_ERASED  0x05  // Memory erase check failed
#define DFU_STATUS_errPROG          0x06  // Program memory function failed
#define DFU_STATUS_errVERIFY        0x07  // Programmed memory failed verification
#define DFU_STATUS_errADDRESS       0x08  // Received program address out of range
#define DFU_STATUS_errNOTDONE       0x09  // Received DFU_DNLOAD with wLength = 0, but 
// device does not think it has all the data yet
#define DFU_STATUS_errFIRMWARE      0x0A  // Device's firmware is corrupted. It can not 
// return to (non-DFU) operations
#define DFU_STATUS_errVENDOR        0x0B  // Vendor-specific string error
#define DFU_STATUS_errUSBR          0x0C  // Device detected unexpected USB reset signal
#define DFU_STATUS_errPOR           0x0D  // Device detected unexpected power on reset
#define DFU_STATUS_errUNKNOWN       0x0E  // Unknown error
#define DFU_STATUS_errSTALLEDPKT    0x0F  // Device stalled an unexpected request

// DFU states
#define DFU_STATE_appIDLE           0     // Device is running its normal application
#define DFU_STATE_appDETACH         1     // Device received DFU_DETACH and waiting for USB reset
#define DFU_STATE_dfuIDLE           2     // Device is operating in DFU mode and waiting for requests
#define DFU_STATE_dfuDNLOAD_SYNC    3     // Device has received a block and waiting for host to
// solicit the status via DFU_GETSTATUS
#define DFU_STATE_dfuDNBUSY         4     // Device is programming a control-write block into its
// nonvolatile memories
#define DFU_STATE_dfuDNLOAD_IDLE    5     // Device is processing a download operation
// Expecting DFU_DNLOAD requests
#define DFU_STATE_dfuMANIFEST_SYNC  6     // Device has recieved the final block of firmware fromm
// host and is waiting for receipt of DFU_GETSTATUS to begin
// the manifestation phase. Or device has completed the
// manifestion phase and waiting for DFU_GETSTATUS.
#define DFU_STATE_dfuMANIFEST       7     // Device in Manifestation phase (Not all devices can response
// to DFU_GETSTATUS when in this state)
#define DFU_STATE_dfuMANIFEST_WAIT_RESET  8 // Device has programmed its memories and is waiting for a
// USB reset or a power on reset
#define DFU_STATE_dfuUPLOAD_IDLE    9     // Device is processing an upload operation. Expecting
// DFU_UPLOAD requests
#define DFU_STATE_dfuERROR          10    // An error occurred. Awaiting DFU_CLRSTATUS request

#define DFU_NUM_STATES              11

// Status
typedef struct _dfu_status_struct_ {
    byte status;    // result of most execution of the most request
    byte pollTime[3]; // minimum time in msec host wait before request status
    byte state;       // state right after response to this request
    byte istring;     // string describe this status
} DFU_STATUS_STRUCT;

typedef struct _dfu_pollstate_struct_ {
    byte pollTime[3]; // minimum time in msec host wait before request status
    byte state;       // state right after response to this request
} DFU_POLLSTATE_STRUCT;

#define DFU_NUM_POLLSTATES      (DFU_NUM_STATES - 4)

#define DFU_POLLTIME_TABLE_SIZE (sizeof(DFU_POLLSTATE_STRUCT) * DFU_NUM_POLLSTATES)

// DFU bit definitions

// DFU related structures
typedef struct _Dfu_Struct_ {
    byte  state;           // DFU states
    byte  status;          // DFU status
    byte  timer0;
    word  timer;
#define DFU_MNF_TIMER_MIN  0x0010   // the minimum time set for manifest mode
    
    word  headerCount;     // header of image is limited to 255
    
    // The target and download status defines if the firmware is corrupted
    // Ex.: target = EEPROM, dl
    byte  loadStatus;     // Download status
#define DFU_LOAD_NOT        0x00    // No data write yet
#define DFU_LOAD_HEADER     0x01
#define DFU_LOAD_NOW        0x02    // Down load in in process
#define DFU_LOAD_COMPLETED  0x03    // Download completed
#define DFU_LOAD_ERROR      0xFF    // Download error
    
    byte  target;           // target type
#define DFU_TARGET_UNDEF   0x00
#define DFU_TARGET_RAM     0x01
#define DFU_TARGET_EEPROM  0x02
#define DFU_TARGET_OTHER   0x03
#define DFU_TARGET_VENDOR  0x04
    
    byte mnfState;
#define DFU_MNF_NOT             0x00    // not in manifestation phase yet
#define DFU_MNF_PHASE_IN_PROG   0x01    // Manifestation phase in progress
#define DFU_MNF_PHASE_COMPLETED 0x02    // Manifestattion completed
    
    // Counter for total data set to send/receive
    //  word dataPayload;
    
    // Counter for total data remaining to be sent/received
    word dataRemain;
    
    // Buffer address
    word bufferAddr;
    
} DFU_STRUCT;

// Internal defines for the state machine with request events
#define DFU_MAC_DEF_OK      0x00
#define DFU_MAC_REQ_ERROR   0x01
#define DFU_MAC_GET_STATUS  0x02
#define DFU_MAC_GET_STATE   0x03
#define DFU_GET_STATE       0x03

// DFU Default descriptors
// Device descriptor
#define DFU_BCDUSB_L        0x00
#define DFU_BCDUSB_H        0x01
#define DFU_BCLASSCODE      0xFE
#define DFU_BSUBCLASSCODE   0x01
#define DFU_BMAXPACKETSIZE0 64
#define DFU_IDVENDOR_OFFSET 0x07
#define DFU_IDVENDOR_L      0xFF
#define DFU_IDVENDOR_H      0xFF
#define DFU_IDPRODUCT_OFFSET 0x09
#define DFU_IDPRODUCT_L     0xFE
#define DFU_IDPRODUCT_H     0xFF
#define DFU_BCDDEVICE_L     0x00
#define DFU_BCDDEVICE_H     0x01
#define DFU_STATUS_REMOTEWAKEUP   0x02

// Configuration descriptor
#define DFU_SIZEOF_CONFIG_BLOCK   25
#define DFU_NUM_IF              0x01
#define DFU_CONFIG_VALUE        0x01
#define DFU_MAXPOWER            (500 >> 2)

// Interface descriptor
#define DFU_IFNUMBER            0x00
#define DFU_INTERFACE_OFFSET    0x09 
#define DFU_IF_SETTING          0x00

// DFU Functional Descriptor
#define DFU_FUNC_DESC_SIZE      0x07
#define DFU_FUNC_DESC_OFFSET    18
#define DFU_FUNC_DESC_TYPE      0x21
#define DFU_MANIFESTTOL_BIT     0x04
#define DFU_UPLOAD_CAP_BIT      0x02
#define DFU_DNLOAD_CAP_BIT      0x01
#define DFU_DETACH_TIMEOUT      0x100
#define DFU_DETACH_TIMEOUT_H    0x01
#define DFU_DETACH_TIMEOUT_L    0x00
#define DFU_WTRANSFER_SIZE_H    0x00     
#define DFU_WTRANSFER_SIZE_L    DFU_BMAXPACKETSIZE0     

// Endpoints
#define DFU_STATUS_EP_HALT      0x02

// String descriptors
#define DFU_ILANG_STR_ID        0x00
#define DFU_IMANF_STR_ID        0x01
#define DFU_IPRODUTC_STR_ID     (DFU_IMANF_STR_ID + 1)
#define DFU_IIF_STR_ID          (DFU_IPRODUTC_STR_ID + 1)

#define DFU_LANGID_SIZE         0x04
#define DFU_MANF_STR_SIZE     	16
#define DFU_PROD_STR_SIZE       16
#define DFU_EEPROM_STR_SIZE     14    
#define DFU_RAM_STR_SIZE        8
#define DFU_OTHER_STR_SIZE      12

// DFU device record for running in DFU mode
// Device record
typedef struct _dfu_device_struct {
    // Device selfpowered/remote wakeup
    byte devStatus[2];
    
    // Configuration current setting
    byte configSetting;
    
    // Interface current setting
    byte ifCurSetting;
    
    // End point state
    byte epState[2];
    
    // Dummy
    byte dummy[2];
    
} DFU_DEVICE_STRUCT;

// Intend to replace the memory map
typedef struct _dfu_data_struct_ {
    EEPROM_HEADER_STRUCT eepromHeaderData;
    byte eepromHeaderExt[0x1F];
    byte deviceDesc[SIZEOF_DEVICE_DESC];
    byte configDesc[DFU_SIZEOF_CONFIG_BLOCK];
    byte langStrDesc[DFU_LANGID_SIZE];
    byte vendorStrDesc[0x40];
    byte prodStrDesc[0x40];
    byte ifStrDesc[0x20];
    DFU_POLLSTATE_STRUCT pollTimeTable[DFU_NUM_POLLSTATES];
    byte getBuffer[0x40];
    byte tempBuffer[0x40];
} DFU_DATA_STRUCT;

// references
// DFU state machine data
extern DFU_STRUCT volatile pdata DfuStateMachine;
extern DFU_DEVICE_STRUCT data DfuDevice;
extern DFU_DATA_STRUCT volatile xdata DfuData;


//#define DFU_FUNC_DESC           stc_sfr_array(DFU_CONFIG_DESC_ADDR + DFU_FUNC_DESC_OFFSET)
#define DFU_FUNC_DESC           stc_sfr_array(&DfuData.configDesc[DFU_FUNC_DESC_OFFSET])
#define DFU_FUNC_ATTRIB_OFFSET  2 
#define DFU_FuncManTolBit       (DFU_FUNC_DESC[DFU_FUNC_ATTRIB_OFFSET] & DFU_MANIFESTTOL_BIT)
#define DFU_FuncUpLoadCapBit    (DFU_FUNC_DESC[DFU_FUNC_ATTRIB_OFFSET] & DFU_UPLOAD_CAP_BIT)
#define DFU_FuncDnLoadCapBit    (DFU_FUNC_DESC[DFU_FUNC_ATTRIB_OFFSET] & DFU_DNLOAD_CAP_BIT)

extern byte code DfuDeviceDesc[];
extern byte code DfuConfigDesc[];
extern byte code DfuLangIDDesc[];
extern byte code DfuManfStrDesc[];
extern byte code DfuProductStrDesc[];
extern byte code DfuEepromStrDesc[];
extern byte code DfuRamStrDesc[];
extern byte code DfuOtherStrDesc[];


// Prototypes
void dfuHandler();
void dfuSetup(byte Target, byte *PollTimeTablePtr);
void dfuGetStrDesc();
void dfuHwdStateMachine();
void dfuWritePattern(byte *buffer, word size, byte pattern);
void dfuInitStateMachine();

#endif