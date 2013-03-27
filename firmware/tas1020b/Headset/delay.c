//=============================================================================
// delay.c
//
// This module contains I2C read/write routines.
// Note: This functions cannot be used by code running at a higher priority
// than the timer 0 interrupt.
//
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//============================================================================= 
#define _DELAY_C
#include <reg52.h>
#include "..\rom\types.h"
#include "delay.h"

//
// Function:    Delay a specified number of microseconds. This function will use
//              Timer 0 to determine the approximate amount of time to delay.
//              This function is guaranteed to delay at least the specified
//              amount of time. At a minimum the function will delay 250
//              microseconds.
//
// Input:       units - specifies the number of 250 microsecond units to delay.
//					
// Output:      none
//
//=============================================================================
void delay(unsigned int units)
{
    INIT_TIMER0; 
    SETUP_TX0(GRANULARITY);
    START_TIMER0;

    while(1) 
    { 

        if (TF0 == 1) 
        { 
            STOP_TIMER0; 
            TF0 = 0; 
            SETUP_TX0(GRANULARITY);        
            START_TIMER0; 
            units--; 
            if (!units) 
                break; 
        } 
    } 

    STOP_TIMER0;
}


