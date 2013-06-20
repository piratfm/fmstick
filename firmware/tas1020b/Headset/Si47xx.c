//===========================================================
// Silicon Labs Interface, Strictly Private
// Copyright 2011, tipok
//===========================================================
/*===========================================================
Si47xx.c: routines for prgramming the SI4711 FM Transmitter
Note: Data values are provided by the device record
//==========================================================*/
#define _SI47XX_C
#include <Reg52.h>
#include <stddef.h>
#include "..\rom\types.h"
#include "..\rom\reg_stc1.h"
#include "..\rom\I2c.h"
#include "..\rom\xdata.h"
#include "..\rom\ROmFunc.h"

#include "gpio.h"
#include "Si47xx.h"
#include "Si47xx_cmddefs.h"
#include "Si47xx_propdefs.h"
#include "devRCode.h"
#include "delay.h"

#define POWERUP_TIME 110    // Powerup delay in milliseconds

word idata DeviationValueThatSet = 6825; //default
word idata DeviationValueCounted = 6825;

byte idata buff_request[8];
byte idata buff_response[8];
byte idata buff_ret; //i2c return buffer

/* TX TUNE STATUS */
word idata TxFreq = 0;
byte idata TxPower = 0;
byte idata TxAntCap = 0;
byte idata TxRnl = 0;

byte idata asq_level = 0;
bit  idata asq_overmod=FALSE;
bit  idata asq_ialh=FALSE;
bit  idata asq_iall=FALSE;


bit si47xxReset()
{
	CPTCTL = 0x00;
	wait_ms(1);
	CPTCTL = 0x01;
	wait_ms(1);

	FePoweredUp = FALSE;
	FELed = LED_OFF;

	TxPoweredUp = FALSE;
	TXLed = LED_OFF;

	ASQLOWLed   = LED_OFF;
    ASQHILed   = LED_OFF;
    ASQOVRMLed   = LED_OFF;

	return TRUE;
}

// Function: Waits for an I2C operation to complete within some specified
//					 number of microseconds.
//
// Input: 	I2CStatusWaitValue - The I2C status register value which will
//							indicate that the I2C operation is completed.
//					nTimeoutMicroSec - number of microseconds to wait.
//
// Returns	1 is operation is completed successfully within the timeout
//					period. 0 o.w.
//
byte si47xxWaitOnI2C(byte StatusWaitValue)
{
    word timeout = I2C_DELAY;

    while (!(I2CSTA & (StatusWaitValue | ERROR)) && timeout)
        timeout--;
    
    if((timeout != 0) && (!(I2C_ERROR)))
		return SI4711_OK;
	if (I2C_ERROR)
		return SI4711_COMM_ERR;
	if(timeout == 0)
		return SI4711_TIMEOUT;
	return SI4711_COMM_ERR;
}


//-----------------------------------------------------------------------------
// Helper function that is used to write to the part which abstracts what
// bus mode is currently being used.
//-----------------------------------------------------------------------------
bit si47xx_lowWrite(byte number_bytes, byte *data_out)
{
	bit ret=TRUE;
	buff_ret = SI4711_ERROR_UNKNOWN;
   	if(!number_bytes)
		return FALSE;

   	if(number_bytes < 2)
		number_bytes=2;

		// Setup I2C for write current address mode
   	//I2CSTA &= ~FREQ_400KHZ; //100 kHz
   	//I2CSTA |= FREQ_400KHZ;    //400 kHz (maybe would be ok in poduction boards)
	I2CSTA &= CLEAR_ALL;
    I2CADR  = I2C_WRITE_ADDR(SI4711_I2C_ADDR_1);

	for (number_bytes -= 2; number_bytes; number_bytes--) {
        // Write byte
        I2CDATO = *data_out++;      //Data byte
	  	buff_ret = si47xxWaitOnI2C(XMIT_DATA_EMPTY);
   		if(buff_ret != SI4711_OK) {
			//goto write_err;
			ret=FALSE;
			break;
		}
    }
   
    I2CDATO = *data_out++;      //Data byte
	buff_ret = si47xxWaitOnI2C(XMIT_DATA_EMPTY);
   	if(buff_ret != SI4711_OK) {
		//goto write_err;
		ret=FALSE;
	}

	I2CSTA |= STOP_WRITE;
	I2CDATO = *data_out++;      //Data byte
    buff_ret = si47xxWaitOnI2C(XMIT_DATA_EMPTY);
	if(buff_ret != SI4711_OK)
		ret=FALSE;
//write_err:
	I2CSTA &= CLEAR_ALL;
	//I2CSTA &= ~FREQ_400KHZ; //100 kHz
	return ret;
}

//-----------------------------------------------------------------------------
// Helper function that is used to read from the part which abstracts what
// bus mode is currently being used.
//-----------------------------------------------------------------------------
bit si47xx_lowRead(byte number_bytes, byte *data_in)
{
	bit ret=TRUE;
	buff_ret = SI4711_ERROR_UNKNOWN;
   	if(!number_bytes)
		return FALSE;
   	if(number_bytes < 2)
		number_bytes=2;

	//I2CSTA &= ~FREQ_400KHZ;   //100 kHz
	//I2CSTA |= FREQ_400KHZ;    //400 kHz (maybe would be ok in poduction boards)
	I2CSTA	&= CLEAR_ALL;
    I2CADR	= I2C_READ_ADDR(SI4711_I2C_ADDR_1);
    I2CDATO = 0xFF;

    for (number_bytes-=2; number_bytes; number_bytes--) {
 	    buff_ret = si47xxWaitOnI2C(RCV_DATA_FULL);
		if(buff_ret != SI4711_OK) {
		  	//goto read_err;
			ret=FALSE;
			break;
		}
        // Read byte
        *data_in++ = I2CDATI;
    }

    buff_ret = si47xxWaitOnI2C(RCV_DATA_FULL);
	if(buff_ret != SI4711_OK)
	  	ret=FALSE;
    
    I2CSTA |= STOP_READ;
    
    *data_in++ = I2CDATI;
    
    buff_ret = si47xxWaitOnI2C(RCV_DATA_FULL);
	if(buff_ret != SI4711_OK)
	  	ret=FALSE;
    
    // Read byte
    *data_in++ = I2CDATI;
    
    I2CSTA &= CLEAR_ALL;
	return ret;
}

//-----------------------------------------------------------------------------
// This command returns the status
//-----------------------------------------------------------------------------
byte si47xx_readStatus()
{
    byte status;
   
    si47xx_lowRead(1, &status);

    return status;
}

//-----------------------------------------------------------------------------
// Command that will wait for CTS before returning
//-----------------------------------------------------------------------------
bit si47xx_waitForCTS()
{
    word i=400; //~200ms

    // Loop until CTS is found or stop due to the counter running out.
    while (--i && !(si47xx_readStatus() & CTS))
    {
        delay(2); //500us
    }

    // If the i is equal to 0 then something must have happened.
    // It is recommended that the controller do some type of error
    // handling in this case.
	if(i==0)
		return FALSE;
	else
		return TRUE;
}

//-----------------------------------------------------------------------------
// Sends a command to the part and returns the reply bytes
//-----------------------------------------------------------------------------
bit si47xx_command(byte cmd_size, byte *cmd, byte reply_size, byte *reply)
{
    // It is always a good idea to check for cts prior to sending a command to
    // the part.
	si47xx_waitForCTS();
	si47xx_lowWrite(cmd_size, cmd);
	si47xx_waitForCTS();

	// If the calling function would like to have results then read them.
    if(reply_size)
    {
		reply[0] = 0;
        si47xx_lowRead(reply_size, reply);
    }
	return TRUE;
}

//-----------------------------------------------------------------------------
// Set the passed property number to the passed value.
//
// Inputs:
//      propNumber:  The number identifying the property to set
//      propValue:   The value of the property.
//-----------------------------------------------------------------------------
bit si47xx_set_property(word propNumber, word propValue)
{
    // Put the ID for the command in the first byte.
    buff_request[0] = SET_PROPERTY;

	// Initialize the reserved section to 0
    buff_request[1] = 0;

	// Put the property number in the third and fourth bytes.
    buff_request[2] = (byte) MSB(propNumber);
	buff_request[3] = (byte) LSB(propNumber);

	// Put the property value in the fifth and sixth bytes.
    buff_request[4] = (byte) MSB(propValue);
    buff_request[5] = (byte) LSB(propValue);

    // Invoke the command
	return si47xx_command(6, buff_request, 1, buff_response);
}

bit si47xx_get_property(word propNumber, word *propValue)
{
	bit ret = FALSE;
	// Put the ID for the command in the first byte.
    buff_request[0] = GET_PROPERTY;

	// Initialize the reserved section to 0
    buff_request[1] = 0;

	// Put the property number in the third and fourth bytes.
    buff_request[2] = (byte) MSB(propNumber);
	buff_request[3] = (byte) LSB(propNumber);

    // Invoke the command
	ret = si47xx_command(6, buff_request, 4, buff_response);

	// Get the property value from the fourth and fifth bytes.
	*propValue = MAKE_WORD(buff_response[2], buff_response[3]);
	return ret;
}

/* High level functions */
bit si47xxPowerUp(void)
{
//	bit ret;
	buff_request[0] = POWER_UP;
	buff_request[1] = POWER_UP_IN_FUNC_FMTX;

	if(AppDeviceAnalogMode)
		buff_request[2] = POWER_UP_IN_OPMODE_TX_ANALOG;
	else
		buff_request[2] = POWER_UP_IN_OPMODE_TX_DIGITAL;

    // Powerup the device
	si47xx_command(3, buff_request, 0, buff_response);

	wait_ms(POWERUP_TIME);

	FePoweredUp = TRUE;
	FELed = LED_ON;

	TxPoweredUp = FALSE;
	TXLed = LED_OFF;

	//return si47xx_waitForCTS();
	si47xx_waitForCTS();
	return TRUE;
}

bit si47xxFMTX_hardware_cfg (void)
{
	si47xx_set_property(GPO_IEN, 0);
	wait_ms(10);
	si47xx_set_property(REFCLK_FREQ, 31250);
	wait_ms(10);
	si47xx_set_property(REFCLK_PRESCALE, 192);
	wait_ms(10);
	return TRUE;
}

/* High level functions */
bit si47xxPowerDown(void)
{
	if(FePoweredUp != TRUE)
		return FALSE;
	buff_request[0] = POWER_DOWN;
    // Powerdown the device
	FePoweredUp = FALSE;
	FELed = LED_OFF;
	TxPoweredUp = FALSE;
	TXLed = LED_OFF;
    ASQLOWLed   = LED_OFF;
    ASQHILed   = LED_OFF;
    ASQOVRMLed   = LED_OFF;

   	return si47xx_command(1, buff_request, 1, buff_response);
}

bit si47xxSetAudio(bit toEnable)
{
	if(AppDeviceAnalogMode) {
		//si47xx_set_property(DIGITAL_INPUT_SAMPLE_RATE, toEnable ? 44100 : 0);
		//wait_ms(10);
		//si47xx_set_property(DIGITAL_INPUT_FORMAT, 0);
	} else {
		si47xx_set_property(DIGITAL_INPUT_SAMPLE_RATE, toEnable ? 44100 : 0);
		wait_ms(10);
		si47xx_set_property(DIGITAL_INPUT_FORMAT, 0x0008);
	}
	wait_ms(10);
	return TRUE;
}

