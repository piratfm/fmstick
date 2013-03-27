//=============================================================================
// SI47XX.H
//
// This module contains functions definitions for Si4711 Controlling.
//
// Strictly Private 
// Copyright 2011, tipok.
//============================================================================= 
#ifndef _SI47XX_H
#define _SI47XX_H

#include "..\rom\types.h"

#define SI4711_I2C_ADDR_1  0xC6//0x63
#define SI4711_I2C_ADDR_2  0x11

extern word idata DeviationValueThatSet;
extern word idata DeviationValueCounted;

/* buffers */
extern byte idata buff_request[8];
extern byte idata buff_response[8];
extern byte idata buff_ret; //i2c return buffer
/* flags used in main */
extern byte idata i2cFlag;
extern bit TxPoweredUp;
extern bit FePoweredUp;

/* TX TUNE STATUS */
extern word idata TxFreq;
extern byte idata TxPower;
extern byte idata TxAntCap;
extern byte idata TxRnl;

/* ASQ STATUS */
extern byte idata asq_level;
extern bit  idata asq_overmod;
extern bit  idata asq_ialh;
extern bit  idata asq_iall;
extern bit  idata AppDeviceAnalogMode;


/* Low level functions */
/* Medium level functions */
bit si47xx_command(byte cmd_size, byte *cmd, byte reply_size, byte *reply);
bit si47xx_set_property(word propNumber, word propValue);
bit si47xx_get_property(word propNumber, word *propValue);

/* High level functions */
bit si47xxReset();
bit si47xxPowerUp();
bit si47xxFMTX_hardware_cfg();
bit si47xxPowerDown();
bit si47xxSetAudio(bit toEnable);

#endif // _SI47XX_H
