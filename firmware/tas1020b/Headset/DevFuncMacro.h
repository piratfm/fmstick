//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
/*================================================== 
DevFuncMacros: macro for function parameters
//================================================*/
#ifndef _DEVFUNCMACRO_H
#define _DEVFUNCMACRO_H

#ifdef _TAS1020_
// ROM record
#define ROM_STATE             RomRecord.state

// USB Engine
#define USBENGINE_EP0_SIZE    EngParms.ep0MaxPacket

// Define here to use indirect pointer
#ifdef _USING_INDIRECT_POINTER_
#define DEV_DATA_IN           AppDevXternaldata->DevSharedData.in
#define DEV_DATA_OUT          AppDevXternaldata->DevSharedData.out
#else
#define DEV_DATA_IN           Externaldata.DevSharedData.in
#define DEV_DATA_OUT          Externaldata.DevSharedData.out
#endif

// Return parameters
#define RET_DATA_BYTE         DEV_DATA_OUT.Value
#define RET_DATA_WORD         DEV_DATA_OUT.wValue
#define RET_DATA_PTR          DEV_DATA_OUT.pData

////////////////// USED BY USBENG.C /////////////////////////
#define PARAMS_USERINTHANDLER_INTVECTOR         DEV_DATA_IN.DevInt.Vector
          
////////////////// USED BY USB.C /////////////////////////
#define PARAMS_GETIFCLASS_IFID                  DEV_DATA_IN.DevClass.Index
#define PARAMS_GETEPCLASS_EPID                  DEV_DATA_IN.DevClass.Index
#define PARAMS_USBVENDORHANDLER_USBREQ_PTR      DEV_DATA_IN.DevUsbReq.UsbReq
#define PARAMS_USBSTANDARDHANDLER_USBREQ_PTR    DEV_DATA_IN.DevUsbReq.UsbReq
#define PARAMS_CLASSHANDLER_USBREQ_PTR          DEV_DATA_IN.DevUsbReq.UsbReq

// DEVICE GET FUNCTIONS
#define PARAMS_GETIFSTATUS_IFID                 DEV_DATA_IN.DevStatus.Index
#define PARAMS_GETEPSTATUS_EPID                 DEV_DATA_IN.DevStatus.Index
#define PARAMS_GETDEVDESC_SIZE)                 DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETCONFIGDESC_SIZE               DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETCONFIGDESC_CONFIGID           DEV_DATA_IN.DevDesc.Index
#define PARAMS_GETHIDDESC_SIZE                  DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETHIDDESC_IFID                  DEV_DATA_IN.DevDesc.Index
#define PARAMS_GETHIDREPORTDESC_SIZE            DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETHIDREPORTDESC_IFID            DEV_DATA_IN.DevDesc.Index
#define PARAMS_GETSTRDESC_SIZE                  *DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETSTRDESC_STRID                 DEV_DATA_IN.DevDesc.Index
#define PARAMS_GETIF_IFID                       DEV_DATA_IN.DevIf.Index

// DEVICE SET/CLEAR FUNCTIONS
#define PARAMS_SETEPFEATURE_EPID                DEV_DATA_IN.DevFeature.Index
#define PARAMS_SETCONFIG_CONFIGID               DEV_DATA_IN.DevConfig.Index
#define PARAMS_SETIF_IFID                       DEV_DATA_IN.DevIf.Index
#define PARAMS_SETIF_SETTING                    DEV_DATA_IN.DevIf.Setting
#define PARAMS_CLEAREPFEATURE_EPID              DEV_DATA_IN.DevFeature.Index

////////////////// USED BY USBAUDIO.C /////////////////////////
// SET REQUESTS
#define PARAMS_SETMIXER_INPUT                   DEV_DATA_IN.DevMixer.Input
#define PARAMS_SETMIXER_OUTPUT                  DEV_DATA_IN.DevMixer.Output
#define PARAMS_SETMIXER_TYPEVAL                 DEV_DATA_IN.DevMixer.TypeVal
#define PARAMS_SETMIXER_WLENGTH                 DEV_DATA_IN.DevMixer.wLength
#define PARAMS_SETMUTE_UNITID                   DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETMUTE_CN                       DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETMUTEALL_UNITID                DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETMUTEALL_NUMCN                 DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETVOL_UNITID                    DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETVOL_CN                        DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETBASS_UNITID                   DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETBASS_CN                       DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETTREBLE_UNITID                 DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETTREBLE_CN                     DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETVOLALL_UNITID                 DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETVOLALL_NUMCN                  DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETBASSALL_UNITID                DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETBASSALL_NUMCN                 DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETTREBLEALL_UNITID              DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETTREBLEALL_NUMCN               DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETFREQ_EPID                     DEV_DATA_IN.DevAudio.Index

// GET REQUESTS
#define PARAMS_GETUNITIDTYPE_UNITID             DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETMIXER_INPUT                   DEV_DATA_IN.DevMixer.Input
#define PARAMS_GETMIXER_OUTPUT                  DEV_DATA_IN.DevMixer.Output
#define PARAMS_GETMIXER_TYPEVAL                 DEV_DATA_IN.DevMixer.TypeVal
#define PARAMS_GETMIXER_WLENGTH                 DEV_DATA_IN.DevMixer.wLength
#define PARAMS_GETMUTE_UNITID                   DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETMUTE_CN                       DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETVOL_UNITID                    DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETVOL_CN                        DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETVOL_TYPEVAL                   DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETBASS_UNITID                   DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETBASS_CN                       DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETBASS_TYPEVAL                  DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETTREBLE_UNITID                 DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETTREBLE_CN                     DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETTREBLE_TYPEVAL                DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETMUTEALL_UNITID                DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETMUTEALL_CN                    DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETMUTEALL_TYPEVAL               DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETVOLALL_UNITID                 DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETVOLALL_CN                     DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETVOLALL_TYPEVAL                DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETBASSALL_UNITID                DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETBASSALL_CN                    DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETBASSALL_TYPEVAL               DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETTREBLEALL_UNITID              DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETTREBLEALL_CN                  DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETTREBLEALL_TYPEVAL             DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETFREQ_EPID                     DEV_DATA_IN.DevAudio.Index

////////////////// USED BY USBHID.C /////////////////////////
#define PARAMS_HIDGETREPORT_IFID                DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDGETREPORT_REPORTTYPE          DEV_DATA_IN.DevHid.ReportId
#define PARAMS_HIDGETREPORT_REPORTID            DEV_DATA_IN.DevHid.ReportType
#define PARAMS_HIDSETREPORT_IFID                DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDSETREPORT_REPORTTYPE          DEV_DATA_IN.DevHid.ReportId
#define PARAMS_HIDSETREPORT_REPORTID            DEV_DATA_IN.DevHid.ReportType
#define PARAMS_HIDGETIDLE_IFID                  DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDSETIDLE_IFID                  DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDGETPROTOCOL_IFID              DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDSETPROTOCOL_IFID              DEV_DATA_IN.DevHid.Index

////////////////// USED BY USBDFU.C /////////////////////////
#define PARAMS_DFUDNLOAD_LEN                    DEV_DATA_IN.DevDfu.wLength
#define PARAMS_DFUDNLOAD_PDATA_PTR              DEV_DATA_IN.DevDfu.pData
#define PARAMS_DFUUPLOAD_LEN                    DEV_DATA_IN.DevDfu.wLength
#define PARAMS_DFUUPLOAD_PDATA_PTR              DEV_DATA_IN.DevDfu.pData
#define PARAMS_DFUGETSTATUS_STATE               DEV_DATA_IN.DevDfuStatus.state
#define PARAMS_DFUGETSTATUS_STATUS              DEV_DATA_IN.DevDfuStatus.status
#define PARAMS_DFU_SETUP_TARGET                 DEV_ROMFUNC_DATA.DevDfuSetup.Target
#define PARAMS_DFU_SETUP_POLLTABLEPTR           DEV_ROMFUNC_DATA.DevDfuSetup.PollTablePtr

#else                   // TAS1020A

// ROM record
#define ROM_STATE             RomRecord.state

// USB Engine
#define USBENGINE_EP0_SIZE    UsbRequest.ep0MaxPacket

// Define here to use indirect pointer
#ifdef _USING_INDIRECT_POINTER_
#define DEV_DATA_IN           AppDevXternaldata->DevSharedData.in      
#define DEV_DATA_OUT          AppDevXternaldata->DevSharedData.out
#else
#define DEV_DATA_IN           Externaldata.DevSharedData.in      
#define DEV_DATA_OUT          Externaldata.DevSharedData.out
#endif

// Return parameters
#define RET_DATA_BYTE         DEV_DATA_OUT.Value
#define RET_DATA_WORD         DEV_DATA_OUT.wValue
#define RET_DATA_PTR          DEV_DATA_OUT.pData

// Access to the passing parameters by accessing to 
// the UsbRequest Data in internal RAM
#ifdef _DIRECT_ACCESS_USBREQUEST_

////////////////// USED BY USBENG.C /////////////////////////
#define PARAMS_USERINTHANDLER_INTVECTOR         UsbRequest.intSource
          
////////////////// USED BY USB.C /////////////////////////
#define PARAMS_GETIFCLASS_IFID                  UsbRequest.lowIndex
#define PARAMS_GETEPCLASS_EPID                  UsbRequest.lowIndex
#define PARAMS_USBVENDORHANDLER_USBREQ_PTR      &UsbRequest
#define PARAMS_USBSTANDARDHANDLER_USBREQ_PTR    &UsbRequest
#define PARAMS_CLASSHANDLER_USBREQ_PTR          &UsbRequest

// DEVICE GET FUNCTIONS
#define PARAMS_GETIFSTATUS_IFID                 UsbRequest.lowIndex
#define PARAMS_GETEPSTATUS_EPID                 UsbRequest.lowIndex
#define PARAMS_GETDEVDESC_SIZE                  UsbRequest.xferLength
#define PARAMS_GETCONFIGDESC_SIZE               UsbRequest.xferLength
#define PARAMS_GETCONFIGDESC_CONFIGID           UsbRequest.hiwValue
#define PARAMS_GETHIDDESC_SIZE                  UsbRequest.xferLength
#define PARAMS_GETHIDDESC_IFID                  UsbRequest.lowIndex
#define PARAMS_GETHIDREPORTDESC_SIZE            UsbRequest.xferLength
#define PARAMS_GETHIDREPORTDESC_IFID            UsbRequest.lowIndex
#define PARAMS_GETSTRDESC_SIZE                  UsbRequest.xferLength
#define PARAMS_GETSTRDESC_STRID                 UsbRequest.lowValue
#define PARAMS_GETIF_IFID                       UsbRequest.lowIndex

// DEVICE SET/CLEAR FUNCTIONS
#define PARAMS_SETEPFEATURE_EPID                UsbRequest.lowIndex
#define PARAMS_SETCONFIG_CONFIGID               UsbRequest.lowValue
#define PARAMS_SETIF_IFID                       UsbRequest.lowIndex
#define PARAMS_SETIF_SETTING                    UsbRequest.lowValue
#define PARAMS_CLEAREPFEATURE_EPID              UsbRequest.lowIndex

////////////////// USED BY USBAUDIO.C /////////////////////////
// SET REQUESTS
#define PARAMS_SETMIXER_UNITID                  UsbRequest.hiwIndex
#define PARAMS_SETMIXER_INPUT                   UsbRequest.hiwValue
#define PARAMS_SETMIXER_OUTPUT                  UsbRequest.lowValue
#define PARAMS_SETMIXER_TYPEVAL                 UsbRequest.bRequest
#define PARAMS_SETMIXER_WLENGTH                 UsbRequest.wLength
#define PARAMS_SETMUTE_UNITID                   UsbRequest.hiwIndex
#define PARAMS_SETMUTE_CN                       UsbRequest.lowValue
#define PARAMS_SETMUTEALL_UNITID                UsbRequest.hiwIndex
#define PARAMS_SETMUTEALL_NUMCN                 UsbRequest.wLength
#define PARAMS_SETVOL_UNITID                    UsbRequest.hiwIndex
#define PARAMS_SETVOL_CN                        UsbRequest.lowValue
#define PARAMS_SETBASS_UNITID                   UsbRequest.hiwIndex
#define PARAMS_SETBASS_CN                       UsbRequest.lowValue
#define PARAMS_SETTREBLE_UNITID                 UsbRequest.hiwIndex
#define PARAMS_SETTREBLE_CN                     UsbRequest.lowValue
#define PARAMS_SETVOLALL_UNITID                 UsbRequest.hiwIndex
#define PARAMS_SETVOLALL_NUMCN                  (UsbRequest.wLength >> 1)
#define PARAMS_SETBASSALL_UNITID                UsbRequest.hiwIndex
#define PARAMS_SETBASSALL_NUMCN                 UsbRequest.wLength
#define PARAMS_SETTREBLEALL_UNITID              UsbRequest.hiwIndex
#define PARAMS_SETTREBLEALL_NUMCN               UsbRequest.wLength
#define PARAMS_SETFREQ_EPID                     UsbRequest.lowIndex                                          

// GET REQUESTS
#define PARAMS_GETUNITIDTYPE_UNITID             UsbRequest.hiwIndex                                               
#define PARAMS_GETMIXER_UNITID                  UsbRequest.hiwIndex
#define PARAMS_GETMIXER_INPUT                   UsbRequest.hiwValue
#define PARAMS_GETMIXER_OUTPUT                  UsbRequest.lowValue
#define PARAMS_GETMIXER_TYPEVAL                 UsbRequest.bRequest
#define PARAMS_GETMIXER_WLENGTH                 UsbRequest.wLength
#define PARAMS_GETMUTE_UNITID                   UsbRequest.hiwIndex
#define PARAMS_GETMUTE_CN                       UsbRequest.lowValue
#define PARAMS_GETVOL_UNITID                    UsbRequest.hiwIndex
#define PARAMS_GETVOL_CN                        UsbRequest.lowValue
#define PARAMS_GETVOL_TYPEVAL                   UsbRequest.bRequest
#define PARAMS_GETBASS_UNITID                   UsbRequest.hiwIndex
#define PARAMS_GETBASS_CN                       UsbRequest.lowValue
#define PARAMS_GETBASS_TYPEVAL                  UsbRequest.bRequest
#define PARAMS_GETTREBLE_UNITID                 UsbRequest.hiwIndex
#define PARAMS_GETTREBLE_CN                     UsbRequest.lowValue
#define PARAMS_GETTREBLE_TYPEVAL                UsbRequest.bRequest
#define PARAMS_GETMUTEALL_UNITID                UsbRequest.hiwIndex
#define PARAMS_GETMUTEALL_NUMCN                 UsbRequest.wLength
#define PARAMS_GETMUTEALL_TYPEVAL               UsbRequest.bRequest
#define PARAMS_GETVOLALL_UNITID                 UsbRequest.hiwIndex
#define PARAMS_GETVOLALL_NUMCN                  (UsbRequest.wLength >> 1)
#define PARAMS_GETVOLALL_TYPEVAL                UsbRequest.bRequest
#define PARAMS_GETBASSALL_UNITID                UsbRequest.hiwIndex
#define PARAMS_GETBASSALL_NUMCN                 UsbRequest.wLength
#define PARAMS_GETBASSALL_TYPEVAL               UsbRequest.bRequest
#define PARAMS_GETTREBLEALL_UNITID              UsbRequest.hiwIndex
#define PARAMS_GETTREBLEALL_NUMCN               UsbRequest.wLength
#define PARAMS_GETTREBLEALL_TYPEVAL             UsbRequest.bRequest
#define PARAMS_GETFREQ_EPID                     UsbRequest.wLength

////////////////// USED BY USBHID.C /////////////////////////
#define PARAMS_HIDGETREPORT_IFID                UsbRequest.lowIndex
#define PARAMS_HIDGETREPORT_REPORTTYPE          UsbRequest.hiwValue
#define PARAMS_HIDGETREPORT_REPORTID            UsbRequest.lowValue
#define PARAMS_HIDSETREPORT_IFID                UsbRequest.lowIndex
#define PARAMS_HIDSETREPORT_REPORTTYPE          UsbRequest.hiwValue
#define PARAMS_HIDSETREPORT_REPORTID            UsbRequest.lowValue
#define PARAMS_HIDGETIDLE_IFID                  UsbRequest.lowIndex
#define PARAMS_HIDSETIDLE_IFID                  UsbRequest.lowIndex
#define PARAMS_HIDGETPROTOCOL_IFID              UsbRequest.lowIndex
#define PARAMS_HIDSETPROTOCOL_IFID              UsbRequest.lowIndex

////////////////// USED BY USBDFU.C /////////////////////////
#define PARAMS_DFUDNLOAD_LEN                    UsbRequest.wLength
#define PARAMS_DFUDNLOAD_PDATA_PTR              DEV_DATA_IN.DevDfu.pData
#define PARAMS_DFUUPLOAD_LEN                    UsbRequest.wLength
#define PARAMS_DFUUPLOAD_PDATA_PTR              DEV_DATA_IN.DevDfu.pData
#define PARAMS_DFUGETSTATUS_STATE               DEV_DATA_IN.DevDfuStatus.state
#define PARAMS_DFUGETSTATUS_STATUS              DEV_DATA_IN.DevDfuStatus.status
#define PARAMS_DFU_SETUP_TARGET                 DEV_ROMFUNC_DATA.DevDfuSetup.Target
#define PARAMS_DFU_SETUP_POLLTABLEPTR           DEV_ROMFUNC_DATA.DevDfuSetup.PollTablePtr

// Access to the passing parameters by accessing the shared external buffer RAM
#else   // _DIRECT_ACCESS_USBREQUEST_

////////////////// USED BY USBENG.C /////////////////////////
#define PARAMS_USERINTHANDLER_INTVECTOR         DEV_DATA_IN.DevInt.Vector
          
////////////////// USED BY USB.C /////////////////////////
#define PARAMS_GETIFCLASS_IFID                  DEV_DATA_IN.DevClass.Index
#define PARAMS_GETEPCLASS_EPID                  DEV_DATA_IN.DevClass.Index
#define PARAMS_USBVENDORHANDLER_USBREQ_PTR      DEV_DATA_IN.DevUsbReq.UsbReq
#define PARAMS_USBSTANDARDHANDLER_USBREQ_PTR    DEV_DATA_IN.DevUsbReq.UsbReq
#define PARAMS_CLASSHANDLER_USBREQ_PTR          DEV_DATA_IN.DevUsbReq.UsbReq

// DEVICE GET FUNCTIONS
#define PARAMS_GETIFSTATUS_IFID                 DEV_DATA_IN.DevStatus.Index
#define PARAMS_GETEPSTATUS_EPID                 DEV_DATA_IN.DevStatus.Index
#define PARAMS_GETDEVDESC_SIZE                  *DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETCONFIGDESC_SIZE               *DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETCONFIGDESC_CONFIGID           DEV_DATA_IN.DevDesc.Index
#define PARAMS_GETHIDDESC_SIZE                  *DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETHIDDESC_IFID                  DEV_DATA_IN.DevDesc.Index
#define PARAMS_GETHIDREPORTDESC_SIZE            *DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETHIDREPORTDESC_IFID            DEV_DATA_IN.DevDesc.Index
#define PARAMS_GETSTRDESC_SIZE                  *DEV_DATA_IN.DevDesc.Size
#define PARAMS_GETSTRDESC_STRID                 DEV_DATA_IN.DevDesc.Index
#define PARAMS_GETIF_IFID                       DEV_DATA_IN.DevIf.Index

// DEVICE SET/CLEAR FUNCTIONS
#define PARAMS_SETEPFEATURE_EPID                DEV_DATA_IN.DevFeature.Index
#define PARAMS_SETCONFIG_CONFIGID               DEV_DATA_IN.DevConfig.Index
#define PARAMS_SETIF_IFID                       DEV_DATA_IN.DevIf.Index
#define PARAMS_SETIF_SETTING                    DEV_DATA_IN.DevIf.Setting
#define PARAMS_CLEAREPFEATURE_EPID              DEV_DATA_IN.DevFeature.Index

////////////////// USED BY USBAUDIO.C /////////////////////////
// SET REQUESTS
#define PARAMS_SETMIXER_INPUT                   DEV_DATA_IN.DevMixer.Input
#define PARAMS_SETMIXER_OUTPUT                  DEV_DATA_IN.DevMixer.Output
#define PARAMS_SETMIXER_TYPEVAL                 DEV_DATA_IN.DevMixer.TypeVal
#define PARAMS_SETMIXER_WLENGTH                 DEV_DATA_IN.DevMixer.wLength
#define PARAMS_SETMUTE_UNITID                   DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETMUTE_CN                       DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETMUTEALL_UNITID                DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETMUTEALL_NUMCN                 DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETVOL_UNITID                    DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETVOL_CN                        DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETBASS_UNITID                   DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETBASS_CN                       DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETTREBLE_UNITID                 DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETTREBLE_CN                     DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETVOLALL_UNITID                 DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETVOLALL_NUMCN                  DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETBASSALL_UNITID                DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETBASSALL_NUMCN                 DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETTREBLEALL_UNITID              DEV_DATA_IN.DevAudio.Index
#define PARAMS_SETTREBLEALL_NUMCN               DEV_DATA_IN.DevAudio.Channel
#define PARAMS_SETFREQ_EPID                     DEV_DATA_IN.DevAudio.Index                                          

// GET REQUESTS
#define PARAMS_GETUNITIDTYPE_UNITID             DEV_DATA_IN.DevAudio.Index                                                
#define PARAMS_GETMIXER_INPUT                   DEV_DATA_IN.DevMixer.Input
#define PARAMS_GETMIXER_OUTPUT                  DEV_DATA_IN.DevMixer.Output
#define PARAMS_GETMIXER_TYPEVAL                 DEV_DATA_IN.DevMixer.TypeVal
#define PARAMS_GETMIXER_WLENGTH                 DEV_DATA_IN.DevMixer.wLength
#define PARAMS_GETMUTE_UNITID                   DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETMUTE_CN                       DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETVOL_UNITID                    DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETVOL_CN                        DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETVOL_TYPEVAL                   DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETBASS_UNITID                   DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETBASS_CN                       DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETBASS_TYPEVAL                  DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETTREBLE_UNITID                 DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETTREBLE_CN                     DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETTREBLE_TYPEVAL                DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETMUTEALL_UNITID                DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETMUTEALL_CN                    DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETMUTEALL_TYPEVAL               DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETVOLALL_UNITID                 DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETVOLALL_CN                     DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETVOLALL_TYPEVAL                DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETBASSALL_UNITID                DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETBASSALL_CN                    DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETBASSALL_TYPEVAL               DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETTREBLEALL_UNITID              DEV_DATA_IN.DevAudio.Index
#define PARAMS_GETTREBLEALL_CN                  DEV_DATA_IN.DevAudio.Channel
#define PARAMS_GETTREBLEALL_TYPEVAL             DEV_DATA_IN.DevAudio.TypeVal
#define PARAMS_GETFREQ_EPID                     DEV_DATA_IN.DevAudio.Index

////////////////// USED BY USBHID.C /////////////////////////
#define PARAMS_HIDGETREPORT_IFID                DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDGETREPORT_REPORTTYPE          DEV_DATA_IN.DevHid.ReportId
#define PARAMS_HIDGETREPORT_REPORTID            DEV_DATA_IN.DevHid.ReportType
#define PARAMS_HIDSETREPORT_IFID                DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDSETREPORT_REPORTTYPE          DEV_DATA_IN.DevHid.ReportId
#define PARAMS_HIDSETREPORT_REPORTID            DEV_DATA_IN.DevHid.ReportType
#define PARAMS_HIDGETIDLE_IFID                  DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDSETIDLE_IFID                  DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDGETPROTOCOL_IFID              DEV_DATA_IN.DevHid.Index
#define PARAMS_HIDSETPROTOCOL_IFID              DEV_DATA_IN.DevHid.Index

////////////////// USED BY USBDFU.C /////////////////////////
#define PARAMS_DFUDNLOAD_LEN                    DEV_DATA_IN.DevDfu.wLength
#define PARAMS_DFUDNLOAD_PDATA_PTR              DEV_DATA_IN.DevDfu.pData
#define PARAMS_DFUUPLOAD_LEN                    DEV_DATA_IN.DevDfu.wLength
#define PARAMS_DFUUPLOAD_PDATA_PTR              DEV_DATA_IN.DevDfu.pData
#define PARAMS_DFUGETSTATUS_STATE               DEV_DATA_IN.DevDfuStatus.state
#define PARAMS_DFUGETSTATUS_STATUS              DEV_DATA_IN.DevDfuStatus.status
#define PARAMS_DFU_SETUP_TARGET                 DEV_ROMFUNC_DATA.DevDfuSetup.Target
#define PARAMS_DFU_SETUP_POLLTABLEPTR           DEV_ROMFUNC_DATA.DevDfuSetup.PollTablePtr

#endif  // _DIRECT_ACCESS_USBREQUEST_

#endif  // TAS1020 / TAS1020A

#endif
