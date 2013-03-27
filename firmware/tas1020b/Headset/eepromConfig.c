#define _EEPROMCONFIG_C
#include <Reg52.h>
#include <stddef.h>
#include "..\rom\types.h"
#include "..\rom\reg_stc1.h"
#include "..\rom\I2c.h"
#include "..\rom\eeprom.h"
#include "..\rom\xdata.h"
#include "..\rom\ROmFunc.h"

#include "gpio.h"
#include "eepromConfig.h"
#include "delay.h"
#include "devRCode.h"
#include "si47xx.h"
#include "si47xx_cmddefs.h"
#include "si47xx_propdefs.h"

byte idata i2cFlag;

/* we can use 
byte idata buff_request[8];
byte idata buff_response[8];
byte idata buff_ret;
*/

bit configCheckStartupNeeded()
{
	i2cFlag = I2C_READ | I2C_START | I2C_STOP | I2C_WORD_ADDR_TYPE;
	//read sync b00b and flags value:
	SET_I2C(EEPROM_I2C_ADDR, TX_CONFIG_OFFSET, buff_request, 8, i2cFlag);
	if(!devRomFunction(ROM_I2C_ACCESS))
		return FALSE;

	if(buff_request[0] != 0xb0 ||
       buff_request[1] != 0x0b)
		return FALSE; //no sync

	//check autostart:
	if(!buff_request[CONF_NUM_CMDS_OFFSEET])
		return FALSE; //no commands
	if(!buff_request[CONF_NUM_CMDS_OFFSEET+1])
		return FALSE; //no autorun flag
	return TRUE; // have commands
}

bit configSetStartupConfig()
{
	byte commands = 0;
	byte index;
	word addr;

	wait_ms(5);

	i2cFlag = I2C_READ | I2C_START | I2C_STOP | I2C_WORD_ADDR_TYPE;
	SET_I2C(EEPROM_I2C_ADDR, TX_CONFIG_OFFSET+CONF_NUM_CMDS_OFFSEET, &commands, 1, i2cFlag);
	if(!devRomFunction(ROM_I2C_ACCESS))
		return FALSE;
	if(!commands || commands > 62)
		return FALSE;

	wait_ms(5);

	for(index=1; index<=commands; index++) {
		addr = TX_CONFIG_OFFSET+(8*index);
		i2cFlag = I2C_READ | I2C_START | I2C_STOP | I2C_WORD_ADDR_TYPE;
		SET_I2C(EEPROM_I2C_ADDR, addr, &buff_request, 8, i2cFlag);
		if(!devRomFunction(ROM_I2C_ACCESS))
			return FALSE;

		wait_ms(5);

		si47xx_command(8, buff_request, 8, buff_response);
//		if(buff_ret != SI4711_OK)
//			return FALSE;
		/* wait for tSTC and tCOMP */
		switch(buff_request[0]) {
		case TX_TUNE_FREQ:
			wait_ms(100);
			break;
		case TX_TUNE_POWER:
			wait_ms(20);
			if(buff_request[3] >= 88) {
				TxPoweredUp = TRUE;
			} else {
				TXLed = LED_OFF;
				TxPoweredUp = FALSE;
				ASQLOWLed = LED_OFF;
				ASQHILed = LED_OFF;
				ASQOVRMLed = LED_OFF;
			}
			break;
		case SET_PROPERTY:
			wait_ms(10);
			if(buff_request[2] == 0x21 &&
			   buff_request[3] == 0x01) {
				DeviationValueThatSet = MAKE_WORD(buff_request[4], buff_request[5]);
				DeviationValueCounted = DeviationValueThatSet;
			}
			break;
		default:
			wait_ms(5);
			break;
		}
	}
	//set audio config after it!
	si47xxSetAudio(TRUE);
//	if(buff_ret != SI4711_OK)
//		return FALSE;
	return TRUE;
}

