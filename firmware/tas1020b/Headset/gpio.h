//=============================================================================
// GPIO.H
//
// This module contains bit definitions for GPIO pins.
//
// Texas Instruments Strictly Private 
// Copyright 2001, Texas Instruments Inc. 
//============================================================================= 
#ifndef _GPIO_H
#define _GPIO_H

// GPIO Port1 definitions
sbit SPIMISO	= P1^7;	//input
sbit SPIMOSI	= P1^6;	//output
sbit SPISS	    = P1^5;	//output
sbit SPISCLK	= P1^4; //output
sbit SpkEnOut   = P1^2;

// GPIO Port3 definitions
sbit FELed      = P3^0; // program running LED
sbit TXLed      = P3^1; // frontend enabled led
sbit XINTPin    = P3^2; // external interrupt pin
sbit ASQLOWLed  = P3^3; // asq low
sbit ASQHILed   = P3^4; // asq high
sbit ASQOVRMLed = P3^5; // asq overmodulated


#define LED_ON      	FALSE
#define LED_OFF     	TRUE

#define INIT_GPIO()		SPIMISO = 1;                      \
                        SPIMOSI = SPISS = SPISCLK = 0;    \
                        SpkEnOut = 0;                     \
						CPTCTL = 0x00;                    \
                        XINTPin   = FALSE;                \
                        FELed   = LED_OFF;               \
                        TXLed   = LED_OFF;                \
                        ASQLOWLed   = LED_OFF;            \
                        ASQHILed   = LED_OFF;             \
                        ASQOVRMLed   = LED_OFF;

// USB HID PC Command/Request Bytes Definition
// MSB - PC command
#define PCRequestError		0x80
#define PCTransfer			0x02
// LSB - PC Request
#define RequestDone         0x80
enum {
	RequestNone = 0,
	RequestCpuId,
	RequestSi4711Reset,     	//reset
	RequestSi4711Access,    	//low level access
	RequestSi4711GetProp,   	//medium level get prop
	RequestSi4711SetProp,   	//medium level set prop
	RequestSi4711PowerStatus,
	RequestSi4711PowerUp,   	//high level power up
	RequestSi4711PowerDown, 	//high level power down
	RequestSi4711AudioEnable,   //this MUST BE!!! called after setting config to enable audio.
	RequestSi4711AudioDisable,
	RequestEepromSectionRead,
	RequestEepromSectionWrite,
	RequestSi4711AsqStatus,
	RequestSi4711TuneStatus,
	RequestUnknown
};

enum {
	SI4711_OK = 0,
	SI4711_TIMEOUT,
	SI4711_COMM_ERR,
	SI4711_BAD_ARG,
	SI4711_NOCTS,
	SI4711_ERROR_UNKNOWN,
};

#endif
