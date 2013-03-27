//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
/*================================================== 
Device.c: routines to handle device record
//================================================*/

#define DEVICE_NAME "TAS1020B"
#define REVISION_MAJOR 1
#define REVISION_MINOR "$Rev: 342 $"

#define _DEVICE_
#include <Reg52.h>
#include <stddef.h>
#include "..\rom\types.h"
#include "..\rom\reg_stc1.h"
#include "..\rom\Mmap.h"
#include "..\rom\UsbEng.h"
#include "..\rom\usb.h"
#include "..\rom\RomBoot.h"
#include "..\rom\usbAudio.h"
#include "..\rom\usbHid.h"
#include "..\rom\usbDfu.h"
#include "..\rom\xdata.h"
#include "..\rom\devref.h"
#include "..\rom\ROmFunc.h"
#include "..\rom\hwMacro.h"

#include "delay.h"
#include "usbapp.h"
#include "devRCode.h"
#include "device.h"
#include "Codec.h"
#include "DevFuncMacro.h"
#include "Softpll.h"
#include "devdesc.h"
#include "DevXdata.h"
#include "gpio.h"
#include "device.h"
#include "spicomm.h"
#include "Si47xx.h"
#include "Si47xx_cmddefs.h"
#include "Si47xx_propdefs.h"
#include "eepromConfig.h"

#define TIMER1_PERIOD		60000			// 60 msec
#define TIMER1_UNITS		50

extern byte UsbAddress;

// Device record
DEV_DEVICE_STRUCT AppDevice;
DEV_SOFT_ACG_STRUCT AppDeviceSoftAcg;

unsigned int asq_delayed=0;

// this is defined to save RAM space
bit AppDeviceAnalogMode=FALSE;
bit AppDeviceSpkcurMute;
bit AppDeviceSpkpreMute;
bit AppDeviceMicReccurMute;
bit AppDeviceMicIncurMute;

// Reset & suspense Bit flag for App in App running
bit FePoweredUp;
bit TxPoweredUp;
bit AppResetFlag;
bit AppSuspendFlag;

// for testing DFU DnLoad/Upload when target is DFU_TARGET_OTHER
byte *DevTestDfuDataPtr;

// Speaker Amp status
bit SpkEnaStatus;
bit SpkEnaStatusChanged;

// prototypes
void devInit();
void devSleepModeOn();
void devSleepModeOff();
void SetLedState(byte LedState);
void devCheckReset();
void devUpdateCodec();
void HID_ACCESS();
void READ_ASQ_STATUS();

// For hardware guys to debug
#define RAM(ram_addr) (*((unsigned char xdata *) ram_addr))

// init TAS1020 global control register to enable XINT
#define GLOBCTL_INIT  LOW_POWER_DISABLE  | XINT_ENABLE

#ifdef _DEBUG_RESET_ 
sbit FalseReset = P3^0;
#endif

////////////////// ///////// DEVICE RECORD	/////////////////////////
/*-------------------------------------------------------------------
main(): This main is for device including the audio/hid/dfu
-------------------------------------------------------------------*/
void main()
{
	INIT_GPIO();
	TxPoweredUp = FALSE;
	FePoweredUp = FALSE;
#ifdef _DEBUG_RESET_ 
    FalseReset = 0;
#endif

    // Give the XRAM time to powered up as the MCU of the emulator is faster
#ifdef _EMULATOR_
    delay(80);	
    GLOBCTL = GLOBCTL_INIT;
	delay(100);
#else
    GLOBCTL = GLOBCTL_INIT;
#endif

    // ROM code using PDATA starting at 0xFA00
	P2 = 0xFA;
	RomRecord.state  = ROM_APP_RUNNING;

	devInit();
	
	// using ROM DFU handler
	devRomFunction(ROM_INIT_DFU_STATE);
			
   	// using ROM USB engine
	USBENGINE_EP0_SIZE = DEV_MAX_EP0_PKT;
	devRomFunction(ROM_ENG_USB_INIT);

	// wait till codec inited (USB Host must select device configuration)
	while (AppDevice.configSetting == 0 && asq_delayed < 20000) {
		devCheckReset();
		asq_delayed++;
	}

//#define NO_ANALOG
#ifdef NO_ANALOG
	if(asq_delayed == 20000)
		AppDeviceAnalogMode=TRUE;
#else
	while (AppDevice.configSetting == 0)
		devCheckReset();
#endif

usb_pc_answered:
	asq_delayed=0;
	// Take Codec (Si471x) out of reset
	si47xxReset();
    // Turn on status LED
    //USBLed = LED_ON;
	/* start transmitter, if eeprom says yes */
	if(configCheckStartupNeeded()) {
		if(!si47xxPowerUp() || !si47xxFMTX_hardware_cfg() || !configSetStartupConfig()) {
			si47xxReset();
		} else {
			/* if we started stand-alone (out-of PC), just loop here infinetily */
			while(AppDeviceAnalogMode) {
				READ_ASQ_STATUS();
				if(AppDevice.configSetting == 0) {
					devCheckReset();
				} else {
					//we have answer from PC (usb devce connected...)
					AppDeviceAnalogMode=FALSE;
					goto usb_pc_answered;
				}
			}
		}
	}

	AppResetFlag = FALSE;
	AppSuspendFlag = FALSE;
	SpkEnOut = 0;

	while(1) {
        // For DFU mode
		if (RomRecord.state == ROM_DFU_MODE) {
			// Setup for reenumeration
        	// as device in DFU mode
			USBCTL = 0;
			USBFADR = 0;
			PARAMS_DFU_SETUP(DFU_TARGET_EEPROM, 0);
	        // this is for testing DFU_TARGET_OTHER	
		    // DevTestDfuDataPtr = (unsigned char xdata *)0x6000;
			devRomFunction(ROM_RUN_DFU_MODE);
		}
		devCheckReset();
		devUpdateCodec();
#if 1
		// Handle SUSPENSE/RESUME
		if ((AppSuspendFlag == TRUE) && (AppResetFlag == FALSE)) {
        	devSleepModeOn();
#ifndef _EMULATOR_
			PCON = 0x09;
#endif
   	  		while (AppSuspendFlag == TRUE);
      	 	devSleepModeOff();		
			// Delay a bit (approx. 5 ms) to account for button debounce 
	        // if it was used to do a remote wake up. 
    		delay(20);
       	} else {
			HID_ACCESS(); //get HID report from PC & write to SI4711
			READ_ASQ_STATUS();
		}
#else
	HID_ACCESS(); //get HID report from PC & write to SI4711
#endif
	}
	return;

	// Just a dummy code to force the compiler to keep 
	// DevFunctionEntryParser() in the final codes
	DevFunctionEntryParser(0, 0);

}

////////////////// ///////// DEVICE RECORD	/////////////////////////
/*-------------------------------------------------------------------
devInit(): initialize device
-------------------------------------------------------------------*/
void devInit()
{
    coCodecInited = FALSE;
	softPllInit();

	// Init device
#ifdef SELFPOWER_REMOTE
	AppDevice.devStatus = DEV_STATUS_SELFPOWER;
#else
#ifdef SELFPOWER_NOREMOTE
	AppDevice.devStatus = DEV_STATUS_SELFPOWER;
#else
	AppDevice.devStatus = 0;
#endif
#endif
	AppResetFlag = AppSuspendFlag = FALSE;
    AppDevice.dummy.wData[0] = 0;
    AppDevice.dummy.wData[1] = 0;
    AppDevice.dummy.bData[0] = 0;
    AppDevice.dummy.bData[1] = 0;
	AppDevice.dummy.bData[2] = 0;

    // Init configuration
    AppDevice.configSetting = 0;

    // Init audio control interface
    AppDevice.audCtlrIf.curSetting = 0;
    AppDevice.audCtlrIf.epState = DEV_EP_STATE_HALT;

    // Init speaker parameters
    AppDeviceSpkcurMute = DEV_SPK_MUTE_OFF;
    AppDeviceSpkpreMute = DEV_SPK_MUTE_ON; 
    AppDevice.spk.curVol[DEV_SPK_LEFT_CN] = 
    AppDevice.spk.curVol[DEV_SPK_RIGHT_CN] = DEV_SPK_CUR_VOL;
    AppDevice.spk.preVol[DEV_SPK_LEFT_CN] =
    AppDevice.spk.preVol[DEV_SPK_RIGHT_CN] = DEV_SPK_MIN_VOL;

    // Set current interface alternate setting
    AppDevice.spkIf.curSetting =
    AppDevice.dfuIf.curSetting = 0;

    // Initialize endpoint states
    AppDevice.spkIf.epState = 
    AppDevice.hidIf.epState = DEV_EP_STATE_HALT;

    // DFU interface use control endpoint
    // Sampling frequency
    AppDevice.spk.freq = 0xAC44;
}



/*-------------------------------------------------------------------
devUpdateCodec(): Check device audio paramters and update Codec
Return: TRUE if codec update, else FALSE 
-------------------------------------------------------------------*/
void devUpdateCodec()
{
	bit status;
    bit changeStatus;
	
	status = changeStatus = FALSE;
    
    // check speaker
    if (AppDevice.spk.curVol[DEV_SPK_LEFT_CN] != AppDevice.spk.preVol[DEV_SPK_LEFT_CN])
    {
        status = TRUE;
        if (AppDevice.spk.curVol[DEV_SPK_LEFT_CN] < AppDevice.spk.preVol[DEV_SPK_LEFT_CN])
        {
            AppDevice.spk.preVol[DEV_SPK_LEFT_CN] -= DEV_SPK_RES_VOL;
            if ((AppDevice.spk.preVol[DEV_SPK_LEFT_CN] < DEV_SPK_MIN_VOL) ||
                (AppDevice.spk.curVol[DEV_SPK_LEFT_CN] > AppDevice.spk.preVol[DEV_SPK_LEFT_CN]))
                AppDevice.spk.preVol[DEV_SPK_LEFT_CN] = AppDevice.spk.curVol[DEV_SPK_LEFT_CN];             
        }
        else 
        {
            AppDevice.spk.preVol[DEV_SPK_LEFT_CN] += DEV_SPK_RES_VOL;
            if (AppDevice.spk.curVol[DEV_SPK_LEFT_CN] < AppDevice.spk.preVol[DEV_SPK_LEFT_CN])
                AppDevice.spk.preVol[DEV_SPK_LEFT_CN] = AppDevice.spk.curVol[DEV_SPK_LEFT_CN];
        }
    }
    
    if (AppDevice.spk.curVol[DEV_SPK_RIGHT_CN] != AppDevice.spk.preVol[DEV_SPK_RIGHT_CN])
    {
        status = TRUE;
        if (AppDevice.spk.curVol[DEV_SPK_RIGHT_CN] < AppDevice.spk.preVol[DEV_SPK_RIGHT_CN])
        {
            AppDevice.spk.preVol[DEV_SPK_RIGHT_CN] -= DEV_SPK_RES_VOL;
            if ((AppDevice.spk.preVol[DEV_SPK_RIGHT_CN] < DEV_SPK_MIN_VOL) ||
                (AppDevice.spk.curVol[DEV_SPK_RIGHT_CN] > AppDevice.spk.preVol[DEV_SPK_RIGHT_CN]))
                AppDevice.spk.preVol[DEV_SPK_RIGHT_CN] = AppDevice.spk.curVol[DEV_SPK_RIGHT_CN];
        }
        else 
        {
            AppDevice.spk.preVol[DEV_SPK_RIGHT_CN] += DEV_SPK_RES_VOL;
            if (AppDevice.spk.curVol[DEV_SPK_RIGHT_CN] < AppDevice.spk.preVol[DEV_SPK_RIGHT_CN])
                AppDevice.spk.preVol[DEV_SPK_RIGHT_CN] = AppDevice.spk.curVol[DEV_SPK_RIGHT_CN];
        }
    }
    
    if (AppDeviceSpkcurMute != AppDeviceSpkpreMute)
    {
        status = TRUE;
        AppDeviceSpkpreMute = AppDeviceSpkcurMute;
    }
    
    if (status)
    {
        changeStatus = TRUE;
        status = FALSE;
        coSpkUpdate();
    }
    
    // Any thing changed
    if (changeStatus == TRUE)
    {
        delay(5);
    }
}







/*-------------------------------------------------------------------
devInitSTC(): Initilize STC for the specific device operations
-------------------------------------------------------------------*/
void devInitSTC() 
{
	// ISO OUT EP # 1 
    OEPBSIZ1  = EP_BUFFER_SIZE(sizeof(Xdata.OutEp1XBuffer));
    OEPBBAX1  = EP_BASE_ADDR(ALIGNED_XDATA_ADDR(OutEp1XBuffer));
	OEPBBAY1  = 0;		// Y buffer is not used
	OEPCNF1   = 0x43; 	// ISO OUT EP, 4 BYTEs/sample 
	OEPDCNTX1 = 0x00; 	// Clear X data count
	OEPDCNTY1 = 0x00;   // Clear Y data count
	OEPCNF1  |= 0x80; 	// Enable the ISO OUT EP

#ifdef _MIC_
	// ISO IN EP # 2 
    IEPBSIZ2 = EP_BUFFER_SIZE(sizeof(Xdata.InEp2XBuffer));
    IEPBBAX2 = EP_BASE_ADDR(ALIGNED_XDATA_ADDR(InEp2XBuffer));
	IEPBBAY2 =  0;		// Y buffer is not used
//#ifdef _STEREO_MIC_
//	IEPCNF2 = 0x43; 	// ISO IN EP, 4 BYTEs/sample 
//#else
	IEPCNF2 = 0x41; 	// ISO IN EP, 2 BYTEs/sample     
//#endif
	IEPDCNTX2 = 0x80; 	// Clear X data count
	IEPDCNTY2 = 0x80;   // Clear Y data count
    IEPCNF2  |= 0x80;   // enable the ISO IN EP
#endif

	// Interrupt Endpoint for HID - IN EP#3
    IEPBSIZ3 = EP_BUFFER_SIZE(sizeof(Xdata.InEp3));   
    IEPBBAX3 = EP_BASE_ADDR(ALIGNED_XDATA_ADDR(InEp3));
	IEPBBAY3 =  0;		// Y buffer is not used
	IEPCNF3   = 0x80; 	// ena interrupt IN EP & no interrupt
	IEPDCNTX3 = 0x80;   // set NACK bit & clear X data counter
	IEPDCNTY3 = 0x80;   // set NACK bit & clear Y data counter

	// Interrupt Endpoint for HID - OUT EP#4
    OEPBSIZ4  = EP_BUFFER_SIZE(sizeof(Xdata.OutEp4));
    OEPBBAX4  = EP_BASE_ADDR(ALIGNED_XDATA_ADDR(OutEp4));
	OEPBBAY4  = 0;		// Y buffer is not used
	OEPCNF4   = 0x84; 	// ena interrupt OUT EP & interrupt
	OEPDCNTX4 = 0x00; 	// Clear NACK bit & X data counter 
	OEPDCNTY4 = 0x00;   // Clear NACK bit & Y data counter 
 
	// Disable all unused Endpoints 
	IEPCNF1 = 0;
	IEPCNF4 = 0;
	IEPCNF5 = 0;
	IEPCNF6 = 0;
	IEPCNF7 = 0;
	OEPCNF2 = 0;
	OEPCNF3 = 0;
	OEPCNF5 = 0;
	OEPCNF6 = 0;
	OEPCNF7 = 0;

}

/*====================================================================
devSleepModeOff(): Take device hardare out of sleep mode.
=====================================================================*/
void devSleepModeOff()
{
#ifndef _DFU_DEV_ONLY_
    // Turn on codec
	coColdReset();
	if(FePoweredUp)
		si47xxSetAudio(TRUE);

    // turn the power amp on
	SpkEnOut = 0;
#endif

    // Turn on miscellaneous hardware
	//USBLed = LED_ON;
}

/*====================================================================
devSleepModeOn(): Set device hardware to sleep mode, low power mode.
=====================================================================*/
void devSleepModeOn()
{
	//USBLed = LED_OFF;
	FELed = LED_OFF;

#ifndef _DFU_DEV_ONLY_
    // turn off power amp
	SpkEnOut = 1;

#endif
    // Set miscellaneous hardware to sleep mode
	if(FePoweredUp)
		si47xxSetAudio(FALSE);
}

/*--------------------------------------------------------------
devCheckReset: Check for USB Reset      
Method: If the USB RESET is true, then there is no SOF/PSOF 
interrupt. Otherwise, if the USB RESET is a spurious reset,
there will be SOF/PSOF interrupt as normally.
This routine based on the following:
- When there is a USB RESET interrupt the GlDevResetFlag 
is set to TRUE. 
- When there is a SOF or PSOF interrupt the GlDevResetFlag1 
is reset to FALSE. That means the SOF INT has to be enabled.
if GlDevResetFlag flag is set, the routine set GlDevResetFlag1
flag to TRUE and wait for at least more than two time frame,
2 ms to make sure that the SOF/PSOF interrupts happens if there
is any. It then checks the GlDevResetFlag1 flag. If the 
GlDevResetFlag1 flag is not reset, that means there is no 
SOF/PSOF interrupts. That means the USB Reset is real. In this
case the the USB RESET is handled as appropriate. Otherwise,
the USB RESET is spurious and there is no action.

Note: The delay should not be more than 10 ms as if the USB RESET
is real, due to specs, the device has to be ready to reponse
after 10 msec. 
--------------------------------------------------------------*/
void devCheckReset()
{
    if (AppResetFlag == TRUE)
    {
        USBFADR = 0;

#ifdef  _TAS1020_
        UsbAddress = 0;
#endif
            
        // Case we get USB RESET after SUSPEND and no RESUME
        if (AppSuspendFlag == TRUE)
        {
			AppSuspendFlag = FALSE;
        }
        AppResetFlag = FALSE;
    }
} 

/*===================================================================
IEP3_HID(): Prepare Data to send from SI4711 to PC through IEP3 HID
===================================================================*/
void HID_ACCESS()
{
#ifdef _HID_
    if ( (PCCommand == PCTransfer) && !(PCRequest&RequestDone) ) {
        switch (PCRequest) {
			case(RequestSi4711Reset):
				//bReportOut:
				//  +-----------+-----------+
				//  | PCCommand | PCRequest |
				//  +-----------+-----------+
				si47xxReset();
				break;
			case(RequestCpuId):
				//  +-----------+-----------+--------------+----------+-------------+----------+---------------+
				//  | PCCommand | PCRequest | REV_MINOR[2] | STR_SIZE | REV_MAJOR[] | STR_SIZE | DEVICE_NAME[] |
				//  +-----------+-----------+--------------+----------+-------------+----------+---------------+
				Xdata.InEp3.HidReport.bReportItem[2] = (0xFF00 & REVISION_MAJOR) >> 8;
				Xdata.InEp3.HidReport.bReportItem[3] = 0x00FF & REVISION_MAJOR;
				Xdata.InEp3.HidReport.bReportItem[4] = sizeof(REVISION_MINOR);
				AppDevice.dummy.bData[0] = 0;
				for(AppDevice.dummy.bData[0] = 0;
					AppDevice.dummy.bData[0] < sizeof(REVISION_MINOR);
						AppDevice.dummy.bData[0]++)
					Xdata.InEp3.HidReport.bReportItem[5 + AppDevice.dummy.bData[0]] =
							 REVISION_MINOR[AppDevice.dummy.bData[0]];

 			    Xdata.InEp3.HidReport.bReportItem[5 + AppDevice.dummy.bData[0]] = sizeof(DEVICE_NAME);
				AppDevice.dummy.bData[1] = AppDevice.dummy.bData[0] + 5 + 1;

				for(AppDevice.dummy.bData[0] = 0;
					AppDevice.dummy.bData[0] < sizeof(DEVICE_NAME);
						AppDevice.dummy.bData[0]++)
					Xdata.InEp3.HidReport.bReportItem[AppDevice.dummy.bData[1] + AppDevice.dummy.bData[0]] =
							 DEVICE_NAME[AppDevice.dummy.bData[0]];
				break;
			case(RequestSi4711PowerStatus):
				//  +-----------+-----------+-------------+----------------+
				//  | PCCommand | PCRequest | IsPoweredUp | IsTransmitting |
				//  +-----------+-----------+-------------+----------------+
				Xdata.InEp3.HidReport.bReportItem[2] = FePoweredUp;
				Xdata.InEp3.HidReport.bReportItem[3] = TxPoweredUp;
				break;
			case(RequestSi4711PowerUp):
				if(FePoweredUp) {
					PCCommand |= PCRequestError;
					break;
				} else {
					FePoweredUp=TRUE;
				}
				//bReportOut:
				//  +-----------+-----------+---------+-----------+
				//  | PCCommand | PCRequest | IsError | ErrorCode |
				//  +-----------+-----------+---------+-----------+
				buff_ret = SI4711_COMM_ERR;
				Xdata.InEp3.HidReport.bReportItem[3] = 1;
				si47xxPowerUp();
				if(!(buff_response[0] & CTS)) {
					Xdata.InEp3.HidReport.bReportItem[2] = buff_ret;
					break;
				}

				buff_ret = SI4711_COMM_ERR;
				Xdata.InEp3.HidReport.bReportItem[3] = 2;
				si47xxFMTX_hardware_cfg();
				Xdata.InEp3.HidReport.bReportItem[2] = buff_ret;
				FELed = LED_ON;
				break;
			case(RequestSi4711PowerDown):
				if(!FePoweredUp) {
					PCCommand |= PCRequestError;
					break;
				}
				//bReportOut:
				//  +-----------+-----------+---------+-----------+
				//  | PCCommand | PCRequest | IsError | ErrorCode |
				//  +-----------+-----------+---------+-----------+
				buff_ret = SI4711_COMM_ERR;
				si47xxPowerDown();
				if(buff_ret != SI4711_OK || !(buff_response[0] & CTS)) {
					Xdata.InEp3.HidReport.bReportItem[2] = buff_ret;
					si47xxReset(); //force!
				}

				break;
			case(RequestSi4711AudioEnable):
				if(!FePoweredUp) {
					PCCommand |= PCRequestError;
					break;
				}
				buff_ret = SI4711_COMM_ERR;
				si47xxSetAudio(TRUE);
				Xdata.InEp3.HidReport.bReportItem[2] = buff_ret;
				break;
			case(RequestSi4711AudioDisable):
				if(!FePoweredUp) {
					PCCommand |= PCRequestError;
					break;
				}
				buff_ret = SI4711_COMM_ERR;
				si47xxSetAudio(FALSE);
				Xdata.InEp3.HidReport.bReportItem[2] = buff_ret;
				break;
			case(RequestEepromSectionRead):
				//bReportOut:
				//  +-----------+-----------+---------+-------+
				//  | PCCommand | PCRequest | Offset  | bytes |
				//  +-----------+-----------+---------+-------+
				if(Xdata.OutEp4.HidOReport.bReportOut[4] > 32) {
					PCCommand |= PCRequestError;
					break;
				}
				AppDevice.dummy.wData[0] =
					MAKE_WORD(Xdata.OutEp4.HidOReport.bReportOut[2], Xdata.OutEp4.HidOReport.bReportOut[3]);
				if(AppDevice.dummy.wData[0] + Xdata.OutEp4.HidOReport.bReportOut[4] > 0x2000) {
					PCCommand |= PCRequestError;
					break;
				}

				i2cFlag = I2C_READ | I2C_START | I2C_STOP | I2C_WORD_ADDR_TYPE;
				SET_I2C(EEPROM_I2C_ADDR, AppDevice.dummy.wData[0],
						&Xdata.InEp3.HidReport.bReportItem[3],
						 Xdata.OutEp4.HidOReport.bReportOut[4], i2cFlag);

				Xdata.InEp3.HidReport.bReportItem[2] = devRomFunction(ROM_I2C_ACCESS);
				//bReportItem:
				//  +-----------+-----------+--------+---------+- ... -+---------+
				//  | PCCommand | PCRequest | status | data[0] |  ...  | data[n] |
				//  +-----------+-----------+--------+---------+- ... -+---------+
				break;
			case(RequestEepromSectionWrite):
				//bReportOut:
				//  +-----------+-----------+---------+-------+---------+- ... -+---------+
				//  | PCCommand | PCRequest | Offset  | bytes | data[0] |  ...  | data[n] |
				//  +-----------+-----------+---------+-------+---------+- ... -+---------+
				if(Xdata.OutEp4.HidOReport.bReportOut[4] > 32) {
					PCCommand |= PCRequestError;
					break;
				}
				AppDevice.dummy.wData[0] =
					MAKE_WORD(Xdata.OutEp4.HidOReport.bReportOut[2], Xdata.OutEp4.HidOReport.bReportOut[3]);
			    if(AppDevice.dummy.wData[0] + Xdata.OutEp4.HidOReport.bReportOut[4] > 0x2000) {
					PCCommand |= PCRequestError;
					break;
				}

				i2cFlag = I2C_WRITE | I2C_START | I2C_STOP | I2C_WORD_ADDR_TYPE;
				SET_I2C(EEPROM_I2C_ADDR, AppDevice.dummy.wData[0],
						&Xdata.OutEp4.HidOReport.bReportOut[5],
						 Xdata.OutEp4.HidOReport.bReportOut[4], i2cFlag);

				Xdata.InEp3.HidReport.bReportItem[2] = devRomFunction(ROM_I2C_ACCESS);
				//bReportItem:
				//  +-----------+-----------+--------+
				//  | PCCommand | PCRequest | status |
				//  +-----------+-----------+--------+
				break;
			case(RequestSi4711SetProp):
				if(!FePoweredUp) {
					PCCommand |= PCRequestError;
					break;
				}
				//bReportOut:
				//  +-----------+-----------+---------+
				//  | PCCommand | PCRequest | IsError |
				//  +-----------+-----------+---------+
				//reversal order here
				AppDevice.dummy.wData[0] =
					MAKE_WORD(Xdata.OutEp4.HidOReport.bReportOut[2], Xdata.OutEp4.HidOReport.bReportOut[3]);

				//must set other way
				if(AppDevice.dummy.wData[0] == REFCLK_PRESCALE ||
					AppDevice.dummy.wData[0] == REFCLK_FREQ ||
					AppDevice.dummy.wData[0] == DIGITAL_INPUT_FORMAT ||
					AppDevice.dummy.wData[0] == DIGITAL_INPUT_SAMPLE_RATE ||
					AppDevice.dummy.wData[0] == GPO_IEN) {
					Xdata.InEp3.HidReport.bReportItem[2] = SI4711_BAD_ARG;
					PCCommand |= PCRequestError;
					break;
				}

				AppDevice.dummy.wData[1] =
					MAKE_WORD(Xdata.OutEp4.HidOReport.bReportOut[4], Xdata.OutEp4.HidOReport.bReportOut[5]);

				buff_ret = SI4711_COMM_ERR;
				Xdata.InEp3.HidReport.bReportItem[6] =
							si47xx_set_property(AppDevice.dummy.wData[0], AppDevice.dummy.wData[1]);
				Xdata.InEp3.HidReport.bReportItem[7] = buff_response[0];
				Xdata.InEp3.HidReport.bReportItem[8] = buff_ret;

				Xdata.InEp3.HidReport.bReportItem[2] = Xdata.OutEp4.HidOReport.bReportOut[2];
				Xdata.InEp3.HidReport.bReportItem[3] = Xdata.OutEp4.HidOReport.bReportOut[3];
				Xdata.InEp3.HidReport.bReportItem[4] = Xdata.OutEp4.HidOReport.bReportOut[4];
				Xdata.InEp3.HidReport.bReportItem[5] = Xdata.OutEp4.HidOReport.bReportOut[5];


				// hack for set volume
				if(AppDevice.dummy.wData[0] == TX_AUDIO_DEVIATION) {
						DeviationValueThatSet = AppDevice.dummy.wData[1];
						coSpkUpdate();
				}

				//bReportOut:
				//  +-----------+-----------+---------+------------+------+------+
				//  | PCCommand | PCRequest | IsError | IsErrorGet | ValH | ValL |
				//  +-----------+-----------+---------+------------+------+------+
#ifndef _PROP_RECHECK_ //re-check out property!
				AppDevice.dummy.wData[1] = 0;
				break;
#endif
			case(RequestSi4711GetProp):
				if(!FePoweredUp) {
					PCCommand |= PCRequestError;
					break;
				}

				//bReportOut:
				//  +-----------+-----------+---------+
				//  | PCCommand | PCRequest | IsError |
				//  +-----------+-----------+---------+
				AppDevice.dummy.wData[0] =
					MAKE_WORD(Xdata.OutEp4.HidOReport.bReportOut[2], Xdata.OutEp4.HidOReport.bReportOut[3]);
				AppDevice.dummy.wData[1] = 0;
				buff_ret = SI4711_COMM_ERR;
				Xdata.InEp3.HidReport.bReportItem[6] =
						si47xx_get_property(AppDevice.dummy.wData[0], &AppDevice.dummy.wData[1]);
				Xdata.InEp3.HidReport.bReportItem[7] = buff_response[0];
				Xdata.InEp3.HidReport.bReportItem[8] = buff_ret;

				Xdata.InEp3.HidReport.bReportItem[2] = Xdata.OutEp4.HidOReport.bReportOut[2];
				Xdata.InEp3.HidReport.bReportItem[3] = Xdata.OutEp4.HidOReport.bReportOut[3];
				Xdata.InEp3.HidReport.bReportItem[4] = MSB(AppDevice.dummy.wData[1]);
				Xdata.InEp3.HidReport.bReportItem[5] = LSB(AppDevice.dummy.wData[1]);
				break;

			case(RequestSi4711Access):
				if(!FePoweredUp) {
					PCCommand |= PCRequestError;
					break;
				}

				Xdata.InEp3.HidReport.bReportItem[2] = 0;
				Xdata.InEp3.HidReport.bReportItem[3] = 0;
				Xdata.InEp3.HidReport.bReportItem[4] = 0;
				
				if(Xdata.OutEp4.HidOReport.bReportOut[2] < 1) {
					PCCommand |= PCRequestError;
					Xdata.InEp3.HidReport.bReportItem[2] = FALSE;
					Xdata.InEp3.HidReport.bReportItem[3] = SI4711_BAD_ARG;
					break;
				}


				//bReportOut:
				//  +-----------+-----------+---------+---------+--------+- ... -+--------+
				//  | PCCommand | PCRequest | ArgsLen | Command | Arg[1] |  ...  | Arg[7] |
				//  +-----------+-----------+---------+---------+--------+- ... -+--------+
//bReportItem:
//  +-----------+-----------+-------------+------------+---------+---------+---------+ ... -+----------+
//  | PCCommand | PCRequest | WriteStatus | ReadStatus | RespLen |  Status | Resp[1] | ...  | Resp[15] |
//  +-----------+-----------+-------------+------------+---------+---------+---------+ ... -+----------+

				//do it other way!
				if(Xdata.OutEp4.HidOReport.bReportOut[3] == POWER_UP ||
				   Xdata.OutEp4.HidOReport.bReportOut[3] == POWER_DOWN ||
				   Xdata.OutEp4.HidOReport.bReportOut[3] == TX_ASQ_STATUS ||
				   Xdata.OutEp4.HidOReport.bReportOut[3] == TX_TUNE_STATUS ||
				   Xdata.OutEp4.HidOReport.bReportOut[3] == SET_PROPERTY ||
				   Xdata.OutEp4.HidOReport.bReportOut[3] == GET_PROPERTY ) {
					Xdata.InEp3.HidReport.bReportItem[2] = FALSE;
					Xdata.InEp3.HidReport.bReportItem[3] = SI4711_BAD_ARG;
					break;
				}

				/* Write command */
				Xdata.InEp3.HidReport.bReportItem[4] = 16; // RespLen
				buff_ret = SI4711_COMM_ERR;
				Xdata.InEp3.HidReport.bReportItem[2] = si47xx_command(Xdata.OutEp4.HidOReport.bReportOut[2], &Xdata.OutEp4.HidOReport.bReportOut[3],
								Xdata.InEp3.HidReport.bReportItem[4], &Xdata.InEp3.HidReport.bReportItem[5]);
				 
				Xdata.InEp3.HidReport.bReportItem[3] = buff_ret;
				
				/* Set specific delay, depends on what command sent. */
				if(Xdata.OutEp4.HidOReport.bReportOut[3] == TX_TUNE_POWER) {
					if(Xdata.OutEp4.HidOReport.bReportOut[6] >= 88) {
						TxPoweredUp = TRUE;
					} else {
//						TXLed = LED_OFF;
						TxPoweredUp = FALSE;
						ASQLOWLed = LED_OFF;
						ASQHILed = LED_OFF;
						ASQOVRMLed = LED_OFF;
					}
				}

				Xdata.InEp3.HidReport.bReportItem[21] = AppDevice.spk.preVol[DEV_SPK_LEFT_CN];
				Xdata.InEp3.HidReport.bReportItem[22] = AppDevice.spk.preVol[DEV_SPK_RIGHT_CN];
				Xdata.InEp3.HidReport.bReportItem[23] = MSB(DeviationValueCounted);
				Xdata.InEp3.HidReport.bReportItem[24] = LSB(DeviationValueCounted);
				break;
			case(RequestSi4711AsqStatus):
				if(!FePoweredUp || !TxPoweredUp) {
					PCCommand |= PCRequestError;
					break;
				}
				//bReportOut:
				//  +-----------+-----------+---------+---------+------+------+-------+
				//  | PCCommand | PCRequest | IsError | overmod | iall | ialh | level |
				//  +-----------+-----------+---------+---------+------+------+-------+
				Xdata.InEp3.HidReport.bReportItem[2] = 0x00;
				Xdata.InEp3.HidReport.bReportItem[3] = asq_overmod;
				Xdata.InEp3.HidReport.bReportItem[4] = asq_iall;
				Xdata.InEp3.HidReport.bReportItem[5] = asq_ialh;
				Xdata.InEp3.HidReport.bReportItem[6] = asq_level;
				break;
			case(RequestSi4711TuneStatus):
				if(!FePoweredUp) {
					PCCommand |= PCRequestError;
					break;
				}
				//bReportOut:
				//  +-----------+-----------+---------+---------+---------+-------+--------+-----+
				//  | PCCommand | PCRequest | IsError | FREQ[m] | FREQ[l] | power | antcap | rnl |
				//  +-----------+-----------+---------+---------+---------+-------+--------+-----+
				Xdata.InEp3.HidReport.bReportItem[2] = 0x00;
				Xdata.InEp3.HidReport.bReportItem[3] = MSB(TxFreq);
				Xdata.InEp3.HidReport.bReportItem[4] = LSB(TxFreq);
				Xdata.InEp3.HidReport.bReportItem[5] = TxPower;
				Xdata.InEp3.HidReport.bReportItem[6] = TxAntCap;
				Xdata.InEp3.HidReport.bReportItem[7] = TxRnl;
				break;
            default:
                PCCommand |= PCRequestError; 
                break;
        }
        // send data to PC through USP HID
        PCRequest |= RequestDone;
        Xdata.InEp3.HidReport.bReportItem[0] = PCCommand;
        Xdata.InEp3.HidReport.bReportItem[1] = PCRequest;
        while (!(IEPDCNTX3&0x80));
        IEPDCNTX3 = sizeof(Xdata.InEp3.HidReport);
    }
#endif
}

void READ_ASQ_STATUS()
{
	byte status=0;
	//bit old_val;
	if(!FePoweredUp)
		return;

	asq_delayed++;
	if(asq_delayed < 10000)
		return;

	asq_delayed=0;
	FELed = !FELed;

	buff_request[0] = GET_INT_STATUS;

	si47xx_command(1, buff_request, 1, &status);
	if(!(status & CTS) || (status & ERR)) {
		FELed = !FELed;
		return;
	}

	if(status & STCINT) {
		buff_request[0] = TX_TUNE_STATUS;
		buff_request[1] = TX_TUNE_STATUS_IN_INTACK;
		si47xx_command(2, buff_request, 7, buff_response);
		if(!(buff_response[0] & CTS) || (buff_response[0] & ERR)) {
			FELed = !FELed;
			return;
		}

		TxFreq = MAKE_WORD(buff_response[2],buff_response[3]);
		TxPower = buff_response[5];
		TxAntCap = buff_response[6];
		TxRnl = buff_response[7];
		if(TxFreq && TxPower)
			TXLed = LED_ON;
		else
			TXLed = LED_OFF;
	}

	if(!TxPoweredUp)
		return;

	buff_request[0] = TX_ASQ_STATUS;
	/* if interrupt, then clean counters,
     * in other case, just read current level */
	if(status & ASQINT)
		buff_request[1] = TX_ASQ_STATUS_IN_INTACK;
	else
		buff_request[1] = 0x00;

	si47xx_command(2, buff_request, 5, buff_response);
	if(!(buff_response[0] & CTS) || (buff_response[0] & ERR)) {
		FELed = !FELed;
		return;
	}

	asq_level = buff_response[4];

	if(status & ASQINT) {
		asq_overmod = buff_response[1] & TX_ASQ_STATUS_OUT_OVERMOD;
		asq_iall = buff_response[1] & TX_ASQ_STATUS_OUT_IALL;
		asq_ialh = buff_response[1] & TX_ASQ_STATUS_OUT_IALH;

		ASQLOWLed = asq_iall ? LED_ON : LED_OFF;
		ASQHILed = asq_ialh ? LED_ON : LED_OFF;
		ASQOVRMLed = asq_overmod ? LED_ON : LED_OFF;
	}

	//old_val = ASQLOWLed;
	//ASQLOWLed = ASQHILed = ASQOVRMLed = !old_val;

}

