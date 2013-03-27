//=============================================================================
// RomFunc.h
//
// This module contains the function number for functions that are exported
// by the ROM for use by the application FW.
//
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//============================================================================= 

//
// Public rom function entry point.
//
#define PUBLIC_ROM_FUNCTION_ENTRY       0x9FFD
//
// Rom function numbers.
//

#define ROM_GET_PARAM_ADDR      0
#define ROM_I2C_ACCESS          1
#define ROM_BOOT_RESET_CPU      2
#define ROM_RESET_CPU           3
#define ROM_ENG_USB_INIT        4
#define ROM_INIT_DFU_STATE      5
#define ROM_RUN_DFU_MODE        6

extern word RomFunction(byte );
