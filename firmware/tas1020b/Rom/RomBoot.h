//================================================== 
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//================================================== 
/*==================================================  
RomBoot.h: 
History:
- 03/13/2000: Initial
================================================== */
#ifndef _ROMBOOT_H
#define _ROMBOOT_H

// Boot type
#define ROM_EEPROM_BOOT   1
#define ROM_HOST_BOOT     2

// ROM states
#define ROM_BOOT_START      0x00
#define ROM_BOOT_IN_PROCESS 0x01
#define ROM_APP_READY       0x02
#define ROM_APP_RUNNING     0x03
#define ROM_APP_DETACH      0x04
#define ROM_DFU_MODE        0x05

// prototypes


#endif