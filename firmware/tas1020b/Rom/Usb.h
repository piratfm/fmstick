//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
#ifndef _USB_H
#define _USB_H

#include "types.h"

#define USB_SPEC_REV_BCD		0x0100	// BCD coded rev level of USB spec


// DEVICE_REQUEST Structure

#define SIZEOF_STANDARD_REQUEST 0x08
typedef struct _STANDARD_REQUEST
{
    BYTE	bmRequest;		// See bit definitions below
    BYTE	bRequest; 		// See value definitions below
    WORD	wValue; 			// Meaning varies with request type
    WORD	wIndex; 			// Meaning varies with request type
    WORD	wLength;			// Number of bytes of data to transfer (MSByte)
} STANDARD_REQUEST;


// Bit definitions for DEVICE_REQUEST.bmRequestType

// Bit 7: 	Data direction
#define 	USB_REQ_TYPE_OUTPUT 0x00		// 0 = Host sending data to device
#define 	USB_REQ_TYPE_WRITE	USB_REQ_TYPE_OUTPUT
#define 	USB_REQ_TYPE_INPUT	0x80		// 1 = Device sending data to host
#define 	USB_REQ_TYPE_READ 	USB_REQ_TYPE_INPUT

// Bit 6-5: Type
#define 	USB_REQ_TYPE_MASK 0x60	    // Mask value for bits 6-5
#define 	USB_REQ_TYPE_STANDARD 0x00	// 00 = Standard USB request
#define 	USB_REQ_TYPE_CLASS	0x20	  // 01 = Class specific
#define 	USB_REQ_TYPE_VENDOR 0x40	  // 10 = Vendor specific

// Bit 4-0: Recipient
#define 	USB_REQ_TYPE_RECIP_MASK 0x1F	// Mask value for bits 4-0
#define 	USB_REQ_TYPE_DEVICE 0x00	    // 00000 = Device
#define 	USB_REQ_TYPE_INTERFACE	0x01	// 00001 = Interface
#define 	USB_REQ_TYPE_ENDPOINT 0x02	  // 00010 = Endpoint
#define 	USB_REQ_TYPE_OTHER	0x03	    // 00011 = Other


// Values for DEVICE_REQUEST.bRequest

// Standard Device Requests
#define 	USB_REQ_GET_STATUS		0
#define 	USB_REQ_CLEAR_FEATURE 	1
#define 	USB_REQ_SET_FEATURE 	3
#define 	USB_REQ_SET_ADDRESS 	5
#define 	USB_REQ_GET_DESCRIPTOR		6
#define 	USB_REQ_SET_DESCRIPTOR		7
#define 	USB_REQ_GET_CONFIGURATION 8
#define 	USB_REQ_SET_CONFIGURATION 9
#define 	USB_REQ_GET_INTERFACE 	10
#define 	USB_REQ_SET_INTERFACE 	11
#define 	USB_REQ_SYNCH_FRAME 	12


// Descriptor Type Values

#define DESC_TYPE_DEVICE				0x01	
#define DESC_TYPE_CONFIG				0x02	
#define DESC_TYPE_STRING				0x03
#define DESC_TYPE_INTERFACE 		0x04	
#define DESC_TYPE_ENDPOINT			0x05
#define DESC_TYPE_HID 					0x21
#define DESC_TYPE_HID_REPORT		0x22
#define DESC_TYPE_HID_PHYSICAL	0x23


// Feature Selector Values
// Remote wakeup (Type 1)
#define FEATURE_REMOTE_WAKEUP 	1 

// Endpoint stall (Type 0
#define FEATURE_ENDPOINT_STALL	0 

#define EP_STATUS_HALT							0x01

// DEVICE_DESCRIPTOR structure

#define SIZEOF_DEVICE_DESC 0x12
typedef struct _DEVICE_DESCRIPTOR
{
    BYTE	bLength;		        // Length of this descriptor (12h bytes)
    BYTE	bDescriptorType;	  // Type code of this descriptor (01h)
    WORD	bcdUsb; 		        // Release of USB spec (0210h = rev 2.10)
    BYTE	bDeviceClass; 	    //Device's base class code
    BYTE	bDeviceSubClass;	  //Device's sub class code
    BYTE	bDeviceProtocol;	  //Device's protocol type code
    BYTE	bMaxPacketSize0;	  //End point 0's max packet size (8/16/32/64)
    WORD	idVendor; 	        // Vendor ID for device
    WORD	idProduct;		      // Product ID for device
    WORD	bcdDevice;		      // Revision level of device
    BYTE	iManufacturer;		  // Index of manufacturer name string desc
    BYTE	iProduct; 	        // Index of product name string desc
    BYTE	iSerialNumber;		  // Index of serial number string desc
    BYTE	bNumConfigurations; // Number of configurations supported
    
} DEVICE_DESCRIPTOR;

#define USB_DEVICE_DESC_IDVENDOR_OFFSET     8
#define USB_DEVICE_DESC_IDPRODUCT_OFFSET    10
#define USB_DEVICE_DESC_CONFIG_SIZE_OFFSET  7


// CONFIG_DESCRIPTOR structure

#define SIZEOF_CONFIG_DESCRIPTOR 0x09
typedef struct _CONFIG_DESCRIPTOR
{
    BYTE	bLength;		          // Length of this descriptor (9h bytes)
    BYTE	bDescriptorType;	    // Type code of this descriptor (02h)
    WORD	wTotalLength; 	      // Size of this config desc plus all interface,
    //	endpoint, class, and vendor descriptors
    BYTE	bNumInterfaces; 	    // Number of interfaces in this config
    BYTE	bConfigurationValue;	// Value to use in SetConfiguration command
    BYTE	iConfiguration; 	    // Index of string desc describing this config
    BYTE	bmAttributes; 	      // See CFG_DESC_ATTR_xxx values below
    BYTE	MaxPower;   	        // Power used by this config in 2mA units
    
} CONFIG_DESCRIPTOR;

#define USB_CONFIG_DESC_MAXPOWER_OFFSET 8


// Bit definitions for CONFIG_DESCRIPTOR.bmAttributes
#define 	CFG_DESC_ATTR_BUS_POWERED 0x80	  // Bit 7: If set, device is bus powered
#define 	CFG_DESC_ATTR_SELF_POWERED	0x40	// Bit 6: If set, device is self powered
#define 	CFG_DESC_ATTR_REMOTE_WAKE 0x20	  // Bit 5: If set, device supports remote wakeup


// INTERFACE_DESCRIPTOR structure

#define SIZEOF_INTERFACE_DESCRIPTOR 0x09
typedef struct _INTERFACE_DESCRIPTOR
{
    BYTE	bLength;						// Length of this descriptor (9h bytes)
    BYTE	bDescriptorType;		// Type code of this descriptor (04h)
    BYTE	bInterfaceNumber; 	// Zero based index of interface in the configuration
    BYTE	bAlternateSetting;	// Alternate setting number of this interface
    BYTE	bNumEndpoints;		  // Number of endpoints in this interface
    BYTE	bInterfaceClass;	  // Interface's base class code
    BYTE	bInterfaceSubClass; // Interface's sub class code
    BYTE	bInterfaceProtocol; // Interface's protocol type code
    BYTE	iInterface; 	      // Index of string desc describing this interface
    
} INTERFACE_DESCRIPTOR;



// ENDPOINT_DESCRIPTOR structure

#define SIZEOF_ENDPOINT_DESCRIPTOR 0x07
typedef struct _ENDPOINT_DESCRIPTOR
{
    BYTE	bLength;					// Length of this descriptor (7h bytes)
    BYTE	bDescriptorType;	// Type code of this descriptor (05h)
    BYTE	bEndpointAddress; // See EP_DESC_ADDR_xxx values below
    BYTE	bmAttributes; 		// See EP_DESC_ATTR_xxx value below
    WORD	wMaxPacketSize; 	// Max packet size of endpoint
    BYTE	bInterval;				// Polling interval of endpoint in milliseconds
    
} ENDPOINT_DESCRIPTOR;



// Bit definitions for EndpointDescriptor.EndpointAddr

#define 	EP_DESC_ADDR_EP_NUM 0x0F	// Bit 3-0: Endpoint number
#define 	EP_DESC_ADDR_DIR_IN 0x80	// Bit 7: Direction of endpoint, 1/0 = In/Out


// Bit definitions for EndpointDescriptor.EndpointFlags

#define 	EP_DESC_ATTR_TYPE_MASK	0x03	// Mask value for bits 1-0
#define 	EP_DESC_ATTR_TYPE_CONT	0x00	// Bit 1-0: 00 = Endpoint does control transfers
#define 	EP_DESC_ATTR_TYPE_ISOC	0x01	// Bit 1-0: 01 = Endpoint does isochronous transfers
#define 	EP_DESC_ATTR_TYPE_BULK	0x02	// Bit 1-0: 10 = Endpoint does bulk transfers
#define 	EP_DESC_ATTR_TYPE_INT 	0x03	// Bit 1-0: 11 = Endpoint does interrupt transfers

// Local defines
#define USB_AUD_CLASS     0x0100
#define USB_HID_CLASS     0x0300
#define USB_DFU_CLASS     0xFE01
#define USB_UNKNOWN_CLASS 0xFF00

// prototypes
void usbProtocolHandler(void);
void usbStandardHandler(void);

#endif
