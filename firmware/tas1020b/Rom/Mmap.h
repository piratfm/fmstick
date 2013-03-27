//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
//================================================== 
// Mmap.h
// Mapping for the STC buffer RAM usage
// note:
// - Term programmed xx values are values be used to
//   program the registers. These values are real values
//   divided by 8.
// - Related to the size defines, RSIZE means the real 
//   size value, other wise, the size value is 
//   devided by 8 be used to program to the registers.
//   Ex.: INPACK_REALSIZE = 0x40, but the programmed
//   value is INPACK_SIZE = INPACK_REALSIZE/8 = 0x8 
//
// - The offset values is from STC_BUFFER_BASE_ADDR. 
//   These are used for programming the I/O endpoint 
//   buffer 
// - All programmed values are multiple of 8 bytes. 
//   Ex.: if value is 1, value is actually 8 bytes.
// - The rest of the external RAM from USB_EP0_ADDR_END
//   is used either by ROM DFU mode or application. It
//   can not be used at the same time. So if the Application
//   is running, the ROM DFU is not running and visa versa.
//==================================================
#ifndef _MMAP_H
#define _MMAP_H

#include "xdata.h"

// For PROGRAM RAM
// Note: If running on the emulator, you will need to 
// setup the xdata space to specify whether a certain
// range points to the target device or to the emulator.
//
//#ifdef DEBUG_ROM
//#define PROG_RAM_ADDR_START 0x0000
//#define PROG_RAM_ADDR_END   0x2000
//#define PROG_RAM_SIZE       0x2000
//#else
#define PROG_RAM_ADDR_START 0x0
#define PROG_RAM_ADDR_END   0x1800
#define PROG_RAM_SIZE       0x1800
//#endif

#define PROG_SHADOW_RAM_ADDR_START  PROG_RAM_ADDR_START
#define PROG_SHADOW_RAM_SIZE        PROG_RAM_SIZE
#define PROG_SHADOW_RAM_ADDR_END    PROG_RAM_ADDR_END

// input/output packet block for control endpoint zero
// this value is only used to compute the IN offset value for programming
// the IN_PACK address even though the TAS 1020 starting at 0xFA10.
#define STC_BUFFER_BASE_ADDR  0xF800

// This value is known only at compiled time
// If the size is more than 0x40, we have to
// update this value
#define ROM_TEMP_PDATAGROUP_SIZE   0x28

#define ROM_CONFIG_SIZE       (sizeof(XDATA_STRUCT) + sizeof(DFU_STRUCT) + ROM_TEMP_PDATAGROUP_SIZE)

 
#define ROM_CONFIG            stc_sfr_array(XDATA_BUFFER_START)

#define STC_IO_BUFFER_BASE_ADDR  ((((((word)XDATA_BUFFER_START + ROM_CONFIG_SIZE)) + 0x10) >> 4) << 4)

#define INPACK_RSIZE          UsbRequest.ep0MaxPacket
#define INPACK_ADDR           STC_IO_BUFFER_BASE_ADDR
#define INPACK                stc_sfr_array(INPACK_ADDR)
#define INPACK_OFFSET         ((INPACK_ADDR - STC_BUFFER_BASE_ADDR) >> 3)
#define INPACK_SIZE			      (INPACK_RSIZE >> 3)
#define OUTPACK_ADDR          (INPACK_ADDR + INPACK_RSIZE)
#define OUTPACK               stc_sfr_array(OUTPACK_ADDR)
#define OUTPACK_OFFSET        (INPACK_OFFSET + INPACK_SIZE)
#define OUTPACK_RSIZE         INPACK_RSIZE
#define OUTPACK_SIZE					(OUTPACK_RSIZE >> 3)

// Temprary storage for transaction either IN or OUT. The size is supposed to be
// be defined by the application. This buffer is intended to be used for passing
// values by the Usb engine/Usb higher protocol handlers and applications
#define USB_EP0_XFERDATA_ADDR (OUTPACK_ADDR + OUTPACK_RSIZE)
#define USB_EP0_XFERDATA      stc_sfr_array(USB_EP0_XFERDATA_ADDR)

// This is used by eeprom and dfu
#define USB_DFU_EP0_XFERDATA_ADDR (INPACK_ADDR + 0x80)
#define USB_DFU_ADDR_START    (INPACK_ADDR + 0x180)

#endif
