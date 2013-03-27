//=============================================================================
// ROMCODE.C
//
// This module contains stub functions used to access functionality provided in
// the ROM.
//
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//============================================================================= 
#pragma REGPARMS
#include "..\ROM\types.h"
#include "..\ROM\Reg_stc1.h"
#include "..\ROM\RomFunc.h"

//
// External int 0 in ROM
//
#define ROM_EXTINT0_ADDR            0x8003

//
// This is a stub function for the external interrupt 0
// ISR in the ROM. The constant USE_ROM_EXT_INT0 should be
// defined if the user wants to use the ROM based USB engine.
//
#ifdef USE_ROM_EXT_INT0

#ifdef _TAS1020_
extern byte UsbAddress;
#endif

void UseRomExtInt0() interrupt 0
{
#ifdef _TAS1020_

#pragma ASM
    // The Ext int 0 routine in the TAS1020 rom
    // does not correctly restore the content of
    // of IE on exit.
    push        IE
    call        ROM_EXTINT0_ADDR
    pop         IE
#pragma ENDASM

#else           // TAS1020A

#pragma ASM
    LJMP        ROM_EXTINT0_ADDR
#pragma ENDASM

#endif
}

#endif

// Jump to ROM function entry point
word devRomFunction(byte Function)
{
    Function = Function;   
#pragma ASM
    LJMP        PUBLIC_ROM_FUNCTION_ENTRY
#pragma ENDASM
    return 1;
}
