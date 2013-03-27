//=============================================================================
// i2c.h
//
// This module contains I2C related defines.
// Note: This functions, I2CRead and I2CWrite, cannot be used by code running at
// a higher priority than the timer 0 interrupt.
//
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//============================================================================= 

#ifndef _I2C_H
#define _I2C_H

#define I2C_STOP              0x01    // Send out stop after final read/write operation.
#define I2C_START             0x02    // Send out slave address and subaddress.
#define I2C_ADDR_TYPE_BIT     0x04    // Bit on for byte addressing, off for word addressing
#define I2C_BYTE_ADDR_TYPE    0x00    // Device access using byte address
#define I2C_WORD_ADDR_TYPE    0x04    // Device access using word address
#define I2C_400_KHZ           0x08
#define I2C_READ              0x10    // Read from I2C bus.
#define I2C_WRITE             0x20    // Write to I2C bus.          

#define I2C_READ_ADDR(addr)    (addr | 0x01)    // I2C read address
#define I2C_WRITE_ADDR(addr)   (addr & 0xFE)    // I2C write address

#define I2C_ERROR             (I2CSTA & ERROR)  // Indicates if slave dev. did not respond.
#define I2C_MAX_ACK_TRIES   	100
//
// I2C control/status register bits
//

#define STOP_WRITE      0x01        // Stop after write transaction
#define STOP_READ       0x02        // Stop after read transaction
#define XMIT_INT_ENABLE 0x04        // Transmit interrupt enable
#define XMIT_DATA_EMPTY 0x08        // Transmit data register empty
#define FREQ_400KHZ     0x10        // Frequency select
#define ERROR           0x20        // Error condition
#define RCV_INT_ENABLE  0x40        // Receive interrupt enable
#define RCV_DATA_FULL   0x80        // Receive data register full

#define CLEAR_ALL       0x54

#define I2C_DELAY 		20000		// 20000 usec.

//
// Sets up I2C read/write parameters.
//
// Input:   Slave - Address of I2C slave device shift to the left one bit.
//          Sub - Subaddress from which to read/write.
//          Buffer - Pointer to source/destination buffer.
//          Len - Number of bytes to write.
//          Flag -
//              I2C_START : Start transaction. Send I2C slave addr 
//                  and subaddr (don't care value) before reading in bytes.
//              I2C_STOP : Stop transaction after last byte is read.
//          	I2C_WORD_ADDR_TYPE: Indicates SubAddr represents a word value.
//
//
#define SET_I2C(Slave, Sub, Buffer, Len, Flag) \
		I2CParams.SlaveAddr = (Slave); \
		I2CParams.SubAddr = (Sub); \
		I2CParams.pBuffer = (Buffer); \
		I2CParams.nLen = (Len); \
		I2CParams.Flags = (Flag); 

extern bit I2CAccess();
extern bit WaitOnI2C(byte StatusWaitValue);

#endif
