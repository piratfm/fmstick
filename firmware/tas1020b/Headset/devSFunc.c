//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
/*================================================== 
Devsfunc.c: routines to handle and update device 
record based on requests from host
//================================================*/
#define _DEVICE_
#include "..\rom\types.h"
#include <Reg52.h>
#include "..\rom\reg_stc1.h"
#include "..\rom\RomBoot.h"
#include "..\rom\Mmap.h"
#include "..\rom\UsbEng.h"
#include "..\rom\usb.h"
#include "..\rom\usbAudio.h"
#include "..\rom\usbHid.h"
#include "..\rom\usbDfu.h"
#include "..\rom\xdata.h"
#include "..\rom\devref.h"
#include "..\rom\RomFunc.h"
#include "..\rom\hwMacro.h"
#include "delay.h"
#include "usbapp.h"
#include "device.h"
#include "devRCode.h"
#include "Codec.h"
#include "DevFuncMacro.h"
#include "Softpll.h"
#include "gpio.h"
#include "devXdata.h"

// Value representing negative infinity dB.
//
#define USB_NEG_INFINITY        0x8000

// For DFU 
DFU_STATUS_STRUCT DfuStatusData;

// Fix for Resume ISR bug in the ROM code
// This saves and restores the value of the
// device USB address
byte UsbAddress;

extern byte idata PCCommand, PCRequest;
//extern bit PENIRQFlag;

// proto
void devUserIntHandler();
void devGetStrDesc();
void devSetIf();
void devGetEpClass();
void devGetIfClass();
void devGetEpStatus();
void devGetIf();
void devSetEpFeature();
void devSetConfig();
void devClearEpFeature();
void devSetMixer();
void devSetMute();
void devSetVol();
void devSetFreq();

/*-------------------------------------------------------------------
DevFunctionEntryParser(): Device function entry parser
-------------------------------------------------------------------*/
void DevFunctionEntryParser(byte Cmd, void xdata *Data)
{
#ifdef _DFU_
    word temp;
#endif

	VAR_NOT_USED(Data);
    
#ifdef _USING_INDIRECT_POINTER_
    AppDevXternaldata = (XDATA_STRUCT xdata *)Data;
#endif
    
    RET_DATA_BYTE = EVENT_OK;
    switch(Cmd)
    {
    ////////////////// USED BY USBENG.C /////////////////////////
    case DEV_USERINTHANDLER:
        devUserIntHandler();
        break;
        
    ////////////////// USED BY USB.C /////////////////////////
    case DEV_GETIFCLASS:
        devGetIfClass();
        break;
        
    case DEV_GETEPCLASS:
        devGetEpClass();
        break;
        
    case DEV_GETCONFIG:
        RET_DATA_PTR = (byte *)&AppDevice.configSetting;
        break;
        
    case DEV_GETDEVICESTATUS:
        RET_DATA_PTR = (byte *)&AppDevice.devStatus;
        break;
        
    case DEV_GETIFSTATUS:
        RET_DATA_PTR = (byte *)&AppDevice.dummy.wData[0];
        break;
        
    case DEV_GETEPSTATUS:
        devGetEpStatus();
        break;
        
    case DEV_GETDEVDESC:
        // Save 
        UsbAddress = USBFADR;
        PARAMS_GETSTRDESC_SIZE = sizeof(DEVICE_DESCRIPTOR);
        RET_DATA_PTR = (byte *)&DevDesc;
#ifdef _DFU_	
        if (RomRecord.state == ROM_DFU_MODE)
            RET_DATA_PTR = (byte *)&DfuDevDesc;
#endif
        break;
        
    case DEV_GETCONFIGDESC:
        // Application is running in DFU mode
        PARAMS_GETSTRDESC_SIZE = sizeof(ConfigDesc);
        RET_DATA_PTR = (byte *)&ConfigDesc;
#ifdef _DFU_
        if(RomRecord.state == ROM_DFU_MODE)
        {	
            RET_DATA_PTR = (byte *)&AppDfuConfigDesc;
            PARAMS_GETSTRDESC_SIZE = sizeof(AppDfuConfigDesc);
        }
#endif
        break;

#ifdef _HID_
    case DEV_GETHIDDESC:
        if (PARAMS_GETHIDDESC_IFID == DEV_HID_IF_ID)
        {
            PARAMS_GETSTRDESC_SIZE = sizeof(ConfigDesc.HidDesc);
            RET_DATA_PTR = (byte *)&ConfigDesc.HidDesc;
        }
        break;

    case DEV_GETHIDREPORTDESC:
        if (PARAMS_GETHIDDESC_IFID == DEV_HID_IF_ID)
        {
            PARAMS_GETSTRDESC_SIZE = GetHidReportDescSize();
            RET_DATA_PTR = DevHidReportDesc;
        }
        break;
#endif
        
    case DEV_GETSTRDESC:
        devGetStrDesc();
        break;
        
    case DEV_GETIF:
        devGetIf();
        break;
        
    // Set requests
    case DEV_SETREMOTEWAKEUP:
        AppDevice.devStatus |= DEV_STATUS_REMOTE_WAKEUP;
        break;
        
    case DEV_SETEPFEATURE:
        devSetEpFeature();
        break;
        
    case DEV_SETCONFIG:
        devSetConfig();
        break;
        
    case DEV_SETIF:
        devSetIf();
        break;
        
    case DEV_CLRREMOTEWAKEUP:
        AppDevice.devStatus &= ~DEV_STATUS_REMOTE_WAKEUP;
        break;
        
    case DEV_CLEAREPFEATURE:
        devClearEpFeature();
        break;

#ifdef _SI4711_
    ////////////////// USED BY USBAUDIO.C /////////////////////////
    // SET REQUESTS
    case DEV_SETMIXERALL:
        break;

	case DEV_SETMIXER:
        devSetMixer();
        break;

	// As there is only master mute and no specific channel
    // So set mute and set mute all are the same
    case DEV_SETMUTEALL:
    case DEV_SETMUTE:
        devSetMute();
        break;
        
    case DEV_SETVOL:
        devSetVol();
        break;
        
    case DEV_SETBASS:
        break;
    case DEV_SETTREBLE:
        break;
    case DEV_SETVOLALL:           
        break;
    case DEV_SETBASSALL:
        break;
    case DEV_SETTREBLEALL:
        break;
    case DEV_SETFREQ:
        break;
        
    // GET REQUESTS
    case DEV_GETUNITIDTYPE:
        switch (PARAMS_GETUNITIDTYPE_UNITID)
        {																		
        case DEV_SPK_FU_ID: 				
            RET_DATA_BYTE = AUD_SUBTYPE_FU;					
            break;															
        case DEV_SPK_MIXER_ID:
            RET_DATA_BYTE = AUD_SUBTYPE_MIXER; 				
            break;				
        default:															
            RET_DATA_BYTE = AUD_SUBTYPE_UNDEF; 			
            break;															
        }	
        break;
        
    case DEV_GETMIXER: 
        // Only support first form, return 0DB for gain and 1 DB for resolution
        if (((PARAMS_GETMIXER_INPUT == 0) &&
            (PARAMS_GETMIXER_OUTPUT == 0)) ||
            ((PARAMS_GETMIXER_INPUT == 0xFF) &&
            (PARAMS_GETMIXER_OUTPUT == 0xFF)))
			break;

        AppDevice.dummy.wData[0] = 0x0000;
        if (PARAMS_GETMIXER_TYPEVAL == AUD_GET_RES)		 
           AppDevice.dummy.wData[0] = 0x01;
        RET_DATA_PTR = (byte *)&AppDevice.dummy.wData[0];
		break;
        
    case DEV_GETMUTE:
        switch (PARAMS_GETMUTE_UNITID)
        {
        case DEV_SPK_FU_ID:
            AppDevice.dummy.wData[0] = AppDeviceSpkcurMute;
            break;
        default:
            RET_DATA_BYTE = EVENT_ERROR;
            break;
        }
        if (RET_DATA_BYTE == EVENT_OK)
        {
            AppDevice.dummy.wData[0] = AppDevice.dummy.wData[0] << 8;
            RET_DATA_PTR = (byte *)&AppDevice.dummy.wData[0];
        }
        break;
        
    case DEV_GETVOL:
        switch (PARAMS_GETVOL_UNITID)
        {
        case DEV_SPK_FU_ID:
            switch (PARAMS_GETVOL_TYPEVAL)
            {
            case AUD_GET_MIN:
                AppDevice.dummy.wData[0] = DEV_SPK_MIN_VOL;
                break;
            case AUD_GET_MAX:
                AppDevice.dummy.wData[0] = DEV_SPK_MAX_VOL;
                break;
            case AUD_GET_RES:
                AppDevice.dummy.wData[0] = DEV_SPK_RES_VOL;
                break;
            case AUD_GET_CUR:
                switch (PARAMS_GETVOL_UNITID)
                {
                case DEV_SPK_FU_ID:
                    AppDevice.dummy.wData[0] = AppDevice.spk.curVol[PARAMS_GETVOL_CN - 1];
                    break;
                }
                break;
                default:
                    RET_DATA_BYTE = EVENT_ERROR;
                    break;
            }
            break;
            
        default:
            RET_DATA_BYTE = EVENT_ERROR;
            break;
        }
        
        if (RET_DATA_BYTE == EVENT_OK)
        {
            AppDevice.dummy.wData[0] = MAKE_LITTLE_ENDIAN(AppDevice.dummy.wData[0]);
            RET_DATA_PTR = (byte *)&AppDevice.dummy.wData[0];
        }
        break;
        
    case DEV_GETBASS:
        break;
            
    case DEV_GETTREBLE:
        break;
                
    case DEV_GETMUTEALL:
        break;
    case DEV_GETVOLALL:
        break;
    case DEV_GETBASSALL:
        break;
    case DEV_GETTREBLEALL:
        break;
    case DEV_GETFREQ:
        break;
 
#endif




    ////////////////// USED BY USBHID.C /////////////////////////
    case DEV_HIDGETREPORT:
        switch(PARAMS_HIDGETREPORT_IFID)
        {
#ifdef _HID_
        case DEV_HID_IF_ID:
            break;
#endif
        default:
            RET_DATA_BYTE = EVENT_ERROR;
            break;
        }
        if (RET_DATA_BYTE == EVENT_OK)
            RET_DATA_PTR = (byte *)&AppDevice.dummy.wData[0];
        break;
    
    case DEV_HIDSETREPORT:
        break;
        
    case DEV_HIDGETIDLE:
    case DEV_HIDGETPROTO:
        RET_DATA_PTR = 0;
        break;
        
    case DEV_HIDSETIDLE:
    case DEV_HIDSETPROTO:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
        
        ////////////////// MISCELLANEOUS /////////////////////////
#ifdef _DFU_
    case DEV_DFUGETSTATUS:
        DfuStatusData.status = PARAMS_DFUGETSTATUS_STATUS;
        DfuStatusData.state  = PARAMS_DFUGETSTATUS_STATE;
        RET_DATA_PTR = (byte *)&DfuStatusData;
        for (temp = 0;temp < 3; temp++)
        {
            if (DfuStatusData.state == DFU_STATE_appIDLE)
                DfuStatusData.pollTime[2 - temp] = DevDfuPollTimeTable[temp];
            else if (DfuStatusData.state == DFU_STATE_appDETACH)
                DfuStatusData.pollTime[2 - temp] = DevDfuPollTimeTable[temp + sizeof(DFU_POLLSTATE_STRUCT)];
            else 
            {
                RET_DATA_PTR = 0;
                break;
            }
        }
        break;

        // ROM informs App of start of UP/DN load
    case DEV_DFUDNLOAD_START:
    case DEV_DFUUPLOAD_START:
        DevTestDfuDataPtr = (unsigned char xdata *)0x6000;
        break;

    case DEV_DFUDNLOAD:
        DEV_DFU_USB_STATUS = EVENT_OK;
        DEV_DFU_STATUS = DFU_STATUS_OK;
        DEV_DFU_LOAD_STATUS = DFU_LOAD_NOW;
        for (temp = 0; temp < PARAMS_DFUDNLOAD_LEN; temp++, DevTestDfuDataPtr++)
            *DevTestDfuDataPtr = PARAMS_DFUDNLOAD_PDATA_PTR[temp];

        // The application will has its own header to figure out
        // if download is completed
        if (DevTestDfuDataPtr >= 0x8000)
        {
            DevTestDfuDataPtr = (unsigned char xdata *)0x6000;
            DEV_DFU_LOAD_STATUS = DFU_LOAD_COMPLETED;
        }
        break;

    case DEV_DFUUPLOAD:
        DEV_DFU_USB_STATUS = EVENT_OK;
        DEV_DFU_STATUS = DFU_STATUS_OK;
        if (DevTestDfuDataPtr >= 0x8000)
        {
            DEV_DFU_XFER_LENGTH = 0;
            DevTestDfuDataPtr = (unsigned char xdata *) 0x6000;
        }
        else 
        {
            for (temp = 0; temp < PARAMS_DFUUPLOAD_LEN; 
                 temp++, DevTestDfuDataPtr++)
                PARAMS_DFUUPLOAD_PDATA_PTR[temp] = *DevTestDfuDataPtr;
            DEV_DFU_XFER_LENGTH = PARAMS_DFUUPLOAD_LEN;
        }
        break;
        
    case DEV_DFUCHKMANIFEST:
        DEV_DFU_MNFSTATE = DFU_MNF_PHASE_COMPLETED;
        break;
#endif        
        // These need to redefine
    case DEV_USERSUSPENSE:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
        
    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }
}

/*===================================================================
devGetEpClass(): Get the class type based on the endpoint id
===================================================================*/
void devGetEpClass()
{
    switch(PARAMS_GETEPCLASS_EPID)
    {
    case DEV_HID_EP_ID:
    case DEV_HIDO_EP_ID:
        RET_DATA_WORD = USB_HID_CLASS;
        break;
    case DEV_MIC_EP_ID:
    case DEV_SPK_EP_ID:
        RET_DATA_WORD = USB_AUD_CLASS;
        break;
    default:
        RET_DATA_WORD = USB_UNKNOWN_CLASS;
        break;
    }
}

/*===================================================================
devGetIfClass(): Get the class type based on the interface id
===================================================================*/
void devGetIfClass()
{
    // Check interface ID
    switch(PARAMS_GETIFCLASS_IFID)
    {
#ifdef _HID_
    case DEV_HID_IF_ID:
        RET_DATA_WORD = USB_HID_CLASS;
        break;
#endif
    case DEV_AUD_CTLR_IF_ID:
        RET_DATA_WORD = USB_AUD_CLASS;
        break;
#ifdef _DFU_
    case DEV_DFU_IF_ID:
        RET_DATA_WORD = USB_DFU_CLASS;
        break;
#endif
    default:
        RET_DATA_WORD = USB_UNKNOWN_CLASS;
        break;
    }
}

/*===================================================================
devSetIf(): Set alternating Interface setting
===================================================================*/
void devSetIf()
{
    switch (PARAMS_SETIF_IFID)
    {
    case DEV_SPK_IF_ID:
        if ((PARAMS_SETIF_SETTING == DEV_SPK_SETTING_0) ||
            (PARAMS_SETIF_SETTING == DEV_SPK_SETTING_1))
        {
            AppDevice.spkIf.curSetting = PARAMS_SETIF_SETTING;
            if (AppDevice.spkIf.curSetting == DEV_SPK_SETTING_0)
            {
                AppDevice.spkIf.epState |= DEV_EP_STATE_HALT;
                STALLSpkEp;
                DMACTL0 = 0x00;     // Out EP1 to C-port 
                //  Status = devSetSpkIf0();
#ifdef _MIC_
                if (AppDevice.micRecIf.curSetting == DEV_MIC_SETTING_0)
                {
#endif
                    SpkEnaStatusChanged = 1;
                    SpkEnaStatus = 0;
#ifdef _MIC_
                }
#endif
            }
            else
            {
                AppDevice.spkIf.epState &= ~DEV_EP_STATE_HALT;
                STALLClrSpkEp;
                DMACTL0 = 0x81;     // Out EP1 to C-port 
#ifdef _MIC_
                if (AppDevice.micRecIf.curSetting == DEV_MIC_SETTING_0) 
                {
#endif
                    SpkEnaStatusChanged = 1;
                    SpkEnaStatus = 1;
#ifdef _MIC_
                }
#endif
                //				  Status = devSetSpkIf1();
            }
        }
        else
            RET_DATA_BYTE = EVENT_ERROR;
        break;
#ifdef _MIC_
    case DEV_MIC_IF_ID: 
        if ((PARAMS_SETIF_SETTING == DEV_SPK_SETTING_0) ||
            (PARAMS_SETIF_SETTING == DEV_SPK_SETTING_1))
        {
            AppDevice.micRecIf.curSetting = PARAMS_SETIF_SETTING;
            if (AppDevice.micRecIf.curSetting == DEV_MIC_SETTING_0)
            {
                AppDevice.micRecIf.epState |= DEV_EP_STATE_HALT;
                STALLMicEp;
                if (AppDevice.spkIf.curSetting == DEV_SPK_SETTING_0)
                {
                    SpkEnaStatusChanged = 1;
                    SpkEnaStatus = 0;
                }
                //Status = devSetMicIf0();
            }
            else
            {
                AppDevice.micRecIf.epState &= ~DEV_EP_STATE_HALT;
                STALLClrMicEp;
                if (AppDevice.spkIf.curSetting == DEV_SPK_SETTING_0)
                {
                    SpkEnaStatusChanged = 1; 
                    SpkEnaStatus = 1;
                }
                //Status = devSetMicIf1();
            }
        }
        else
            RET_DATA_BYTE = EVENT_ERROR;
        break;
#endif
    case DEV_AUD_CTLR_IF_ID:

#ifdef _HID_
    case DEV_HID_IF_ID:
#endif

#ifdef _DFU_
    case DEV_DFU_IF_ID: 
#endif
        if (PARAMS_SETIF_SETTING != 0)
            RET_DATA_BYTE = EVENT_ERROR;
        break;


    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }

    return;
}

/*===================================================================
devUserIntHandler(): User interrupt handler
===================================================================*/
void devUserIntHandler()
{
    switch(PARAMS_USERINTHANDLER_INTVECTOR)
    {
    // Start of frame
    case PSOF_INT:
    case SOF_INT:
        softPll();
        break;

    // Suspend
    case SUSR_INT:
#ifdef _TAS1020_
        // Fix for bug in the ROM code
        // Save the USB address
        UsbAddress = USBFADR;
#endif
        AppSuspendFlag = TRUE;
        break;

    // Resume
    case RESR_INT:
#ifdef _TAS1020_
        // Fix for bug in the ROM code
        // Restore the USB addess as the ROM code Resume ISR 
        // bug clears the address
        USBFADR = UsbAddress;
#endif
        if (AppSuspendFlag)
        {
            // turn off external interrupt
            AppSuspendFlag = FALSE;
        }
        break;

    // Reset
    case RSTR_INT:
        AppResetFlag ^= 1;
        AppSuspendFlag = FALSE;
        USBFADR = UsbAddress;
        break;

/*     case XINT_INT:
        break;
*/
    // USB Output EndPoint#4 interrupt
    case OEP4_INT:
        // load PC command/request
        PCCommand = Xdata.OutEp4.HidOReport.bReportOut[0];
        PCRequest = Xdata.OutEp4.HidOReport.bReportOut[1];
        OEPDCNTX4 = 0;       // clear the NACK bit & X counter
        if (!PCRequest)
        {                    // when PC request-error occurs --
            PCRequest = RequestDone;
            Xdata.InEp3.HidReport.bReportItem[0] = PCRequestError | PCCommand;
            Xdata.InEp3.HidReport.bReportItem[1] = PCRequest;
            while (!(IEPDCNTX3&0x80));
            IEPDCNTX3 = sizeof(Xdata.InEp3.HidReport);
        }
        break;

    default:
        break;
    }
}

/*===================================================================
devGetStrDesc(): Get string descriptor using string index
===================================================================*/
void devGetStrDesc()
{
    switch(PARAMS_GETSTRDESC_STRID)
    {
    case DEV_LANG_STR_IDX:
        PARAMS_GETSTRDESC_SIZE = sizeof(DevLangIdDesc);
        RET_DATA_PTR = (byte *)&DevLangIdDesc;
        break;

    case DEV_COMPANY_STR_IDX:
        PARAMS_GETSTRDESC_SIZE = sizeof(DevCompanyStringDesc);
        RET_DATA_PTR = (byte *)&DevCompanyStringDesc;
        break;

    case DEV_PROD_STR_IDX:
        PARAMS_GETSTRDESC_SIZE = sizeof(DevProductStringDesc);
        RET_DATA_PTR = (byte *)&DevProductStringDesc;
        break;

    case DEV_FW_VERSION_IDX:
        PARAMS_GETSTRDESC_SIZE = sizeof(DevFwVersion);
        RET_DATA_PTR = (byte *)&DevFwVersion;
        break;

    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }
}

/*===================================================================
devGetEpStatus(): Get endpoint status
===================================================================*/
void devGetEpStatus()
{
    switch (PARAMS_GETEPSTATUS_EPID)
    {
    case DEV_AUD_CTLR_EP_ID:
        RET_DATA_PTR = (byte *)&AppDevice.audCtlrIf.epState;
        break;
    case DEV_SPK_EP_ID:
        RET_DATA_PTR = (byte *)&AppDevice.spkIf.epState;
        break;
    case DEV_MIC_EP_ID:
        RET_DATA_PTR = (byte *)&AppDevice.micRecIf.epState;
        break;
    case DEV_HID_EP_ID:
    case DEV_HIDO_EP_ID:
        RET_DATA_PTR = (byte *)&AppDevice.hidIf.epState;
        break;
    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }
}

/*===================================================================
devGetIf(): Get interface setting
===================================================================*/
void devGetIf()
{
    switch(PARAMS_GETIF_IFID)
    {
    case DEV_AUD_CTLR_IF_ID:
        RET_DATA_PTR = (byte *)&AppDevice.audCtlrIf.curSetting;
        break;
    case DEV_SPK_IF_ID:
        RET_DATA_PTR = (byte *)&AppDevice.spkIf.curSetting;
        break;
#ifdef _MIC_
    case DEV_MIC_IF_ID:
        RET_DATA_PTR = (byte *)&AppDevice.micRecIf.curSetting;
        break;
#endif
#ifdef _HID_
    case DEV_HID_IF_ID:
        RET_DATA_PTR = (byte *)&AppDevice.hidIf.curSetting;
        break;
#endif
#ifdef _DFU_
    case DEV_DFU_IF_ID:
        RET_DATA_PTR = (byte *)&AppDevice.dfuIf.curSetting;
        break;
#endif
    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }
}

/*===================================================================
devSetEpFeature(): Set endpoint feature
===================================================================*/
void devSetEpFeature()
{
    switch(PARAMS_SETEPFEATURE_EPID)
    {
    case DEV_AUD_CTLR_EP_ID:
        AppDevice.audCtlrIf.epState |= DEV_EP_STATE_HALT;
        break;
    case DEV_SPK_EP_ID:
        AppDevice.spkIf.epState |= DEV_EP_STATE_HALT;
        STALLSpkEp;
        break;
    case DEV_MIC_EP_ID:
        AppDevice.micRecIf.epState |= DEV_EP_STATE_HALT;
        STALLMicEp;
        break;
    case DEV_HID_EP_ID:
    case DEV_HIDO_EP_ID:
        AppDevice.hidIf.epState |= DEV_EP_STATE_HALT;
        STALLHidEp;
        break;
    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }
}

/*===================================================================
devSetConfig(): Set configuration
===================================================================*/
void devSetConfig()
{
    AppDevice.configSetting = PARAMS_SETCONFIG_CONFIGID;
#ifndef _DFU_DEV_ONLY_
    if (AppDevice.configSetting == 0)
    {
        SpkEnaStatusChanged = 0;
        SpkEnaStatus = 0;
        AppDevice.spkIf.curSetting = DEV_SPK_SETTING_0;
#ifdef _MIC_
        AppDevice.micRecIf.curSetting = DEV_MIC_SETTING_0;
#endif
        AppDevice.spkIf.epState |= DEV_EP_STATE_HALT;
#ifdef _MIC_
        AppDevice.micRecIf.epState |= DEV_EP_STATE_HALT;
#endif
		AppDevice.hidIf.epState |= DEV_EP_STATE_HALT;
#ifdef _MIC_
		STALLMicEp;
#endif
        STALLHidEp;
        STALLSpkEp; 
    }
    else
    {
        devInitSTC();
        coInitCodec();
        AppDevice.spkIf.epState &= ~DEV_EP_STATE_HALT;
#ifdef _MIC_
        AppDevice.micRecIf.epState &= ~DEV_EP_STATE_HALT;
#endif
        AppDevice.hidIf.epState &= ~DEV_EP_STATE_HALT;
        STALLClrSpkEp;
        STALLClrMicEp;
        STALLClrHidEp;
    }
#endif

}

/*===================================================================
devClearEpFeature(): Clear endpoint feature
===================================================================*/
void devClearEpFeature()
{
    switch(PARAMS_CLEAREPFEATURE_EPID)
    {
    case DEV_AUD_CTLR_EP_ID:
        AppDevice.audCtlrIf.epState &= ~DEV_EP_STATE_HALT;
        break;
    case DEV_SPK_EP_ID:
        AppDevice.spkIf.epState &= ~DEV_EP_STATE_HALT;
        STALLClrSpkEp;
        break;
    case DEV_MIC_EP_ID:
        AppDevice.micRecIf.epState &= ~DEV_EP_STATE_HALT;
        STALLClrMicEp;
        break;
    case DEV_HID_EP_ID:
    case DEV_HIDO_EP_ID:
        AppDevice.hidIf.epState &= ~DEV_EP_STATE_HALT;
        STALLClrHidEp;
        break;
    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }
}


/*===================================================================
devSetMixer(): Set mixer gain
===================================================================*/
void devSetMixer()
{
    // third form 
    if ((PARAMS_SETMIXER_INPUT == 0) &&
        (PARAMS_SETMIXER_OUTPUT == 0))
    {
    }
    // second form
    else if ((PARAMS_SETMIXER_INPUT == 0xFF) &&
        (PARAMS_SETMIXER_OUTPUT == 0xFF))
    {
    }
    // first form
    else
    {
    }
}

/*===================================================================
devSetMute(): Set Mute
===================================================================*/
void devSetMute()
{
    switch (PARAMS_SETMUTE_UNITID)
    {
    case DEV_SPK_FU_ID:
        AppDeviceSpkcurMute = USB_EP0_XFERDATA[0] & DEV_SPK_MSK_MUTE;
        break;
    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }
}

/*===================================================================
devSetVol(): Set Volume
===================================================================*/
void devSetVol()
{
    switch (PARAMS_SETVOL_UNITID)														\
    {
    case DEV_SPK_FU_ID:
        AppDevice.spk.curVol[PARAMS_SETVOL_CN - 1] = 
               MAKE_BIG_ENDIAN(*(word *)&USB_EP0_XFERDATA[0]);
        break;
    default:
        RET_DATA_BYTE = EVENT_ERROR;
        break;
    }
}
