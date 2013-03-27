//================================================== 
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//================================================== 
#ifndef EEPROM_H
#define EEPROM_H
#include "i2c.h"

// Eeprom parameters: timeouts are in microsec
#define EEPROM_I2C_ADDR           0xA0
#define EEPROM_MSEC_TIMEOUT_READ  1000
#define EEPROM_MSEC_TIMEOUT_WRITE 1000

// Vendor/Product Strings will follow the eeprom header
#define EEPROM_START_OFFSET         0x00
#define EEPROM_HEADERCHKSUM_OFFSET  EEPROM_START_OFFSET
#define EEPROM_HEADERSIZE_OFFSET    0x01
#define EEPROM_SIGNATURES_OFFSET    0x02

typedef struct _eeprom_Header_ {
    byte headerChksum;      // checksum by simple adding excluding check sum
    byte headerSize;        // Size of header including strings if applied
    byte signatures[2];
#define EEPROM_SIGNATURE_H  0x12
#define EEPROM_SIGNATURE_L  0x34
    
    word vendorId;          // Vendor Id
    word productId;         // Product Id
    byte productVersion;    // Product version
    byte FirmwareVersion;   // Firmware version
    
    // If any Vendor/Product string exists, the first string after header
    // is the Language ID string
    // Bit set to support the related attribute  
    byte usbAttribute;    
#define EEPROM_SUPPORT_STR_BIT  0x01
#define EEPROM_SELF_POWER       0x04
#define EEPROM_BUS_POWER        0x08
    
    byte maxPower;         // Case if bus powered else zero
    
    // bit definitions:
    // if related bit off, the feature is not supported
    // defined as nearly as possible to the I2C attributes in I2c.h
    byte attribute;        // Eeprom characters
#define EEPROM_CPU_SPEED_24MHZ           0x01
#define EEPROM_HEADER_OVERWRITE          0x02
#define EEPROM_SUPPORT_400_KHZ           0x08
    
    byte wPageSize;          // page size for writing if applied
    
    byte dataType;          // data type
#define EEPROM_APPCODE_TYPE     0x01
#define EEPROM_DEVICE_TYPE      0x02
#define EEPROM_APPCODE_UPDATING 0x03
#define EEPROM_UNEXIST          0xFE
#define EEPROM_INVALID          0xFF
    
    byte rPageSize;         // page size for reading
    // Set to zero to read all
#define EEPROM_RPAGESIZE_ALL    0x00
    
    word payloadSize;       // Size of application code
} EEPROM_HEADER_STRUCT;

#define EEPROM_HEADER_SIZE  sizeof(EEPROM_HEADER_STRUCT)

// prototypes
void eepromExist();
bit eepromBoot();
bit eepromCheckFirmware(byte *);

#endif