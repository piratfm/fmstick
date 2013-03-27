//=============================================================================
// delay.h
//
// This module contains macros used for implementing delays. 
//
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//============================================================================= 
#ifndef _DELAY_H
#define _DELAY_H


#include "..\rom\Reg_stc1.h"
#include "..\rom\types.h"

#define TIMER0_MODE_16_BIT   0x01        // 8-bit timer mode

#define MAX_TIMER_COUNT     0xFFFF      // Max timer tic's 
	
#define IS_24_MHZ 			0x80		// Indicates if MCU running at 24 MHz	

#define GRANULARITY     250     // 250 usec.

//
// Setup TH0 and TL0 for GRANULARITY usec units
//
#define SETUP_TX0(granularity) \
TH0 = (byte)((MAX_TIMER_COUNT - (granularity << ((GLOBCTL & IS_24_MHZ) ? 1 : 0)) - 1) >> 8); \
TL0 = (byte)((MAX_TIMER_COUNT - (granularity << ((GLOBCTL & IS_24_MHZ) ? 1 : 0)) - 1) & 0x00FF)

//
// Initialize timer 0
//
//
#define INIT_TIMER0  TMOD &= ~0x03;\
            TMOD |= TIMER0_MODE_16_BIT


//
// Start timer 0
//
#define START_TIMER0        TR0 = 1

//
// Stop timer 0
//
#define STOP_TIMER0        TR0 = 0

// For Timer 1 Setup
//
// Setup TH1 and TL0 for GRANULARITY usec units
//
#define SETUP_TX1(granularity) \
TH1 = (byte)((MAX_TIMER_COUNT - (granularity << ((GLOBCTL & IS_24_MHZ) ? 1 : 0)) - 1) >> 8); \
TL1 = (byte)((MAX_TIMER_COUNT - (granularity << ((GLOBCTL & IS_24_MHZ) ? 1 : 0)) - 1) & 0x00FF)

//
// Initialize timer 1
//
//

//
// Start timer 1
//
#define START_TIMER1        TR1 = 1

//
// Stop timer 1
//
#define STOP_TIMER1        TR1 = 0

#ifndef _DELAY_C
extern void delay(unsigned int units);
#define wait_ms(x) delay(x*4)

#endif

#endif
