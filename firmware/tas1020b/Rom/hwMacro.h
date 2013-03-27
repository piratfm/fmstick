//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
#ifndef _HWMACRO_H
#define _HWMACRO_H

// Stall endpoints
#define STALLInEp0  {IEPCNF0 |= 0x08;}
#define STALLOutEp0 {OEPCNF0 |= 0x08;}
#define STALLInEp1  {IEPCNF1 |= 0x08;}
#define STALLOutEp1 {OEPCNF1 |= 0x08;}
#define STALLInEp2  {IEPCNF2 |= 0x08;}
#define STALLOutEp2 {OEPCNF2 |= 0x08;}
#define STALLInEp3  {IEPCNF3 |= 0x08;}
#define STALLOutEp3 {OEPCNF3 |= 0x08;}
#define STALLInEp4  {IEPCNF4 |= 0x08;}
#define STALLOutEp4 {OEPCNF4 |= 0x08;}
#define STALLInEp5  {IEPCNF5 |= 0x08;}
#define STALLOutEp5 {OEPCNF5 |= 0x08;}
#define STALLInEp6  {IEPCNF6 |= 0x08;}
#define STALLOutEp6 {OEPCNF6 |= 0x08;}
#define STALLInEp7  {IEPCNF7 |= 0x08;}
#define STALLOutEp7 {OEPCNF7 |= 0x08;}

// Clear stall endpoints
#define STALLClrInEp0  {IEPCNF0 &= ~0x08;}
#define STALLClrOutEp0 {OEPCNF0 &= ~0x08;}
#define STALLClrInEp1  {IEPCNF1 &= ~0x08;}
#define STALLClrOutEp1 {OEPCNF1 &= ~0x08;}
#define STALLClrInEp2  {IEPCNF2 &= ~0x08;}
#define STALLClrOutEp2 {OEPCNF2 &= ~0x08;}
#define STALLClrInEp3  {IEPCNF3 &= ~0x08;}
#define STALLClrOutEp3 {OEPCNF3 &= ~0x08;}
#define STALLClrInEp4  {IEPCNF4 &= ~0x08;}
#define STALLClrOutEp4 {OEPCNF4 &= ~0x08;}
#define STALLClrInEp5  {IEPCNF5 &= ~0x08;}
#define STALLClrOutEp5 {OEPCNF5 &= ~0x08;}
#define STALLClrInEp6  {IEPCNF6 &= ~0x08;}
#define STALLClrOutEp6 {OEPCNF6 &= ~0x08;}
#define STALLClrInEp7  {IEPCNF7 &= ~0x08;}
#define STALLClrOutEp7 {OEPCNF7 &= ~0x08;}


// Toggle Data 1/0 or 0/1
#define TOGGLEInEp0Data {IEPCNF0 |= 0x20;}
#define TOGGLEOutEp0Data {OEPCNF0 |= 0x20;}

// Empty Packet RX/OUT endpoint FIFO
#define EMPTYOutEp0			{OEPDCNTX0 = 0;}

// Flush TX/IN endpoint 0 FIFO
#define EMPTYInEp0			{IEPDCNTX0 = 0x80;}

// Prepare zero packet length TX/IN endpoint FIFO                                
/* clear buffer for next packet */
#define ZEROPACKInEp0		{IEPDCNTX0 = 0x00;}

#endif