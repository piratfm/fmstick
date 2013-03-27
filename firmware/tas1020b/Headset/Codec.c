//==================================================
// Texas Instruments Strictly Private
// Copyright 1999, Texas Instruments Inc.
//==================================================
/*==================================================
Codec.c: routines for prgramming the AC 97 Codec
Note: Data values are provided by the device record
//================================================*/
#include <Reg52.h>
#include "..\rom\types.h"
#include "..\rom\Reg_stc1.h"
#include "..\rom\usbdfu.h"
#include "usbapp.h"
#include "device.h"
#include "softpll.h"
#include "delay.h"
#include "Codec.h"
#include "gpio.h"
#include "delay.h"
#include "Si47xx.h"
#include "si47xx_propdefs.h"
#include "eepromConfig.h"

bit coCodecInited = 0;

/*===================================================================
coInitCodec(): initialize the codec
===================================================================*/
void coInitCodec()
{

    if (!coCodecInited) {

    USBIMSK &= ~0x10;
    softPllInit();
    USBIMSK |= 0x10;

    // configure DMA       
	DMATSH0  = 0x40;     // 2 BYTEs per time slot 
	DMATSL0  = 0x03;     // Slots 0&1
	DMACTL0  = 0x01;     // Out EP1 to C-port 
    DMACTL0 |= 0x80;     // enable OEP1
#ifdef _MIC_
	DMATSH1  = 0x40;     // 2 BYTEs per time slot
//#ifdef _STEREO_MIC_
//    DMATSL1  = 0x05;     // Slots 0&1
//#else
    DMATSL1  = 0x01;     // Slots 0
//#endif
	DMACTL1  = 0x0A;     // In EP2 from C-port      
    DMACTL1 |= 0x80;     // enable EP2 from C-port  
#endif
    // disable C-port  
    GLOBCTL &= ~CODEC_PORT_ENABLE;
    // Configure the C-port   
    CPTCNF1 = 0x0C;     // I2S mode4
	//CPTCNF1 = (0x0C|0x03);     // I2S mode4
    CPTCNF2 = 0xCD;     // 16-bit per time slot  
    CPTCNF3 = 0xAC;	    // byte inverse & 1clk delay
    CPTCNF4 = 0x03;     
    // Enable C-port
    GLOBCTL |= CODEC_PORT_ENABLE;
    }    
    coCodecInited = TRUE;
}

/*===================================================================
coColdReset(): Warm reset Codec
===================================================================*/
void coColdReset()
{

	//dont use it yet
	//CPTCTL = 0x00; // Set reset state
    // Turn on Codec
    //CPTCTL = 0x01;
    coInitCodec();
}


/*===================================================================
coSpkUpdate(): Update Speaker
===================================================================*/
void coSpkUpdate()
{
    byte value;
	// left channel set to minimum value
    value = DEV_SPK_MSK_VOL;

    // case both left and right channel: init at minus infinitive 
    // which considered as mute so turn on mute
    //if ((AppDevice.spk.preVol[DEV_SPK_LEFT_CN] < DEV_SPK_MIN_VOL) &&
    //    (AppDevice.spk.preVol[DEV_SPK_RIGHT_CN] < DEV_SPK_MIN_VOL))
    //    value |= SI471X_MUTE_ON;

    // left channel
    if (AppDevice.spk.preVol[DEV_SPK_LEFT_CN] > DEV_SPK_MIN_VOL)
        value = LSB((DEV_SPK_MAX_VOL_BOUND - 
            AppDevice.spk.preVol[DEV_SPK_LEFT_CN])/DEV_SPK_RES_VOL) & DEV_SPK_MSK_VOL;
	//DeviationValueCounted = (DeviationValueThatSet * (DEV_SPK_MSK_VOL - value)) / DEV_SPK_MSK_VOL;
	DeviationValueCounted = DeviationValueThatSet - (DeviationValueThatSet/DEV_SPK_MSK_VOL)*value;

	if(FePoweredUp)
		si47xx_set_property(TX_AUDIO_DEVIATION, DeviationValueCounted);

	// case mute
    //if (AppDeviceSpkpreMute == TRUE)
    //    value |= SI471X_MUTE_ON;

	//SET VALUE TO SI471X!
    //CPTDATH = value;

    // right channel set to minimum value
    value = 0x1F;
    if (AppDevice.spk.preVol[DEV_SPK_RIGHT_CN] > DEV_SPK_MIN_VOL)
        value = LSB((DEV_SPK_MAX_VOL_BOUND - 
            AppDevice.spk.preVol[DEV_SPK_RIGHT_CN])/DEV_SPK_RES_VOL) & 0x1F;    

}

