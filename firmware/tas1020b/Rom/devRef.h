//=============================================================================
// devRef.h
//
// This module contains the definition of the application function table that
// is used by parts of the ROM code to call application specific functions.
//
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//============================================================================= 
#ifndef DEFREF_H
#define DEFREF_H

#ifndef NULL
#define NULL        0x00
#endif

#define DEV_FUNCTION_ENTRY      0x0100

#define DEV_FUNCTION(CMD, PTR)              ((void (code *) (byte, void xdata *)) DEV_FUNCTION_ENTRY)(CMD, (void xdata *)(PTR))

//
// Application function numbers.
//

////////////////// USED BY USBENGINE.C /////////////////////////
#define DEV_USERUNDEFINED                   0              
#define DEV_USERINTHANDLER                  1

////////////////// USED BY USB.C /////////////////////////
#define  DEV_GETIFCLASS                     2
#define  DEV_GETEPCLASS                     3
#define  DEV_USBVENDORHANDLER               4 
#define  DEV_USBSTANDARDHANDLER             5


// DEV_ICE GET FUNCTIONS
#define  DEV_GETCONFIG                      6
#define  DEV_GETDEVICESTATUS                7
#define  DEV_GETIFSTATUS                    8
#define  DEV_GETEPSTATUS                    9
#define  DEV_GETDEVDESC                     10
#define  DEV_GETCONFIGDESC                  11
#define  DEV_GETHIDDESC                     12
#define  DEV_GETHIDREPORTDESC               13
#define  DEV_GETSTRDESC                     14
#define  DEV_GETIF                          15

// DEV_ICE SET/CLEAR FUNCTIONS
#define  DEV_SETREMOTEWAKEUP                16
#define  DEV_SETEPFEATURE                   17
#define  DEV_SETCONFIG                      18
#define  DEV_SETIF                          19
#define  DEV_CLRREMOTEWAKEUP                20
#define  DEV_CLEAREPFEATURE                 21

////////////////// USED BY USBAUDIO.C /////////////////////////
// SET REQUESTS
#define  DEV_SETMIXERALL                    22
#define  DEV_SETMIXER                       23
#define  DEV_SETMUTE                        24
#define  DEV_SETMUTEALL                     25
#define  DEV_SETVOL                         26
#define  DEV_SETBASS                        27
#define  DEV_SETTREBLE                      28
#define  DEV_SETVOLALL                      29
#define  DEV_SETBASSALL                     30
#define  DEV_SETTREBLEALL                   31
#define  DEV_SETFREQ                        32

// GET REQUESTS
#define  DEV_GETUNITIDTYPE				          33
#define  DEV_GETMIXER                       34
#define  DEV_GETMUTE                        35
#define  DEV_GETVOL                         36
#define  DEV_GETBASS                        37
#define  DEV_GETTREBLE                      38
#define  DEV_GETMIXERALL                    39
#define  DEV_GETMUTEALL                     40
#define  DEV_GETVOLALL                      41
#define  DEV_GETBASSALL                     42
#define  DEV_GETTREBLEALL                   43
#define  DEV_GETFREQ                        44

////////////////// USED BY USBHID.C /////////////////////////
#define  DEV_HIDGETREPORT                   45
#define  DEV_HIDGETIDLE                     46
#define  DEV_HIDGETPROTO                    47
#define  DEV_HIDSETREPORT                   48
#define  DEV_HIDSETIDLE                     49
#define  DEV_HIDSETPROTO                    50

////////////////// USED BY  /////////////////////////
#define DEV_DFUDNLOAD                       51
#define DEV_DFUUPLOAD                       52
#define DEV_DFUDNLOAD_START                 53
#define DEV_DFUUPLOAD_START                 54
                            
// DFU get status used when App in IDLE or DETACH mode
#define DEV_DFUGETSTATUS                    55
#define DEV_DFUCHKMANIFEST                  56

// These need to redefine
#define DEV_USERSUSPENSE                    57
#define DEV_CLASSHANDLER                    58
#define DEV_AUDIOCLASSHANDLER               59

// Macros
#define DEV_DATA_IN           Externaldata.DevSharedData.in      
#define DEV_DATA_OUT          Externaldata.DevSharedData.out
#define DEV_DATA_BYTE         DEV_DATA_OUT.Value
#define DEV_DATA_WORD         DEV_DATA_OUT.wValue
#define DEV_DATA_PTR          DEV_DATA_OUT.pData
#define DEV_ROMFUNC_DATA      Externaldata.DevSharedData.romFunc
#define DEV_DFU_USB_STATUS    Externaldata.DevSharedData.out.DevDfuRetValue.Status
#define DEV_DFU_STATUS        Externaldata.DevSharedData.out.DevDfuRetValue.DfuStatus
#define DEV_DFU_LOAD_STATUS   Externaldata.DevSharedData.out.DevDfuRetValue.LoadStatus
#define DEV_DFU_XFER_LENGTH   Externaldata.DevSharedData.out.DevDfuRetValue.Length
#define DEV_DFU_MNFSTATE      Externaldata.DevSharedData.out.DevDfuRetValue.MnfState

////////////////// USED BY USBENG.C /////////////////////////
#define PARAMS_USERINTHANDLER(INTVECTOR)        DEV_DATA_IN.DevInt.Vector = INTVECTOR 
          
////////////////// USED BY USB.C /////////////////////////
#define PARAMS_GETIFCLASS(IFID)                 DEV_DATA_IN.DevClass.Index = IFID
#define PARAMS_GETEPCLASS(EPID)                 DEV_DATA_IN.DevClass.Index = EPID
#define PARAMS_USBVENDORHANDLER(USBREQ)         DEV_DATA_IN.DevUsbReq.UsbReq = USBREQ
#define PARAMS_USBSTANDARDHANDLER(USBREQ)       DEV_DATA_IN.DevUsbReq.UsbReq = USBREQ


// DEVICE GET FUNCTIONS
#define PARAMS_GETIFSTATUS(IFID)                DEV_DATA_IN.DevStatus.Index = IFID
#define PARAMS_GETEPSTATUS(EPID)                DEV_DATA_IN.DevStatus.Index = EPID
#define PARAMS_GETDEVDESC(SIZE)                 DEV_DATA_IN.DevDesc.Size = SIZE
#define PARAMS_GETCONFIGDESC(SIZE, CONFIGID)    DEV_DATA_IN.DevDesc.Size = SIZE; \
                                                DEV_DATA_IN.DevDesc.Index = CONFIGID
#define PARAMS_GETHIDDESC(SIZE, IFID)           DEV_DATA_IN.DevDesc.Size = SIZE; \
                                                DEV_DATA_IN.DevDesc.Index = IFID
#define PARAMS_GETHIDREPORTDESC(SIZE, IFID)     DEV_DATA_IN.DevDesc.Size = SIZE; \
                                                DEV_DATA_IN.DevDesc.Index = IFID
#define PARAMS_GETSTRDESC(SIZE, STRID)          DEV_DATA_IN.DevDesc.Size = SIZE; \
                                                DEV_DATA_IN.DevDesc.Index = STRID
#define PARAMS_GETIF(IFID)                      DEV_DATA_IN.DevIf.Index = IFID

// DEVICE SET/CLEAR FUNCTIONS
#define PARAMS_SETEPFEATURE(EPID)               DEV_DATA_IN.DevFeature.Index = EPID
#define PARAMS_SETCONFIG(CONFIGID)              DEV_DATA_IN.DevConfig.Index = CONFIGID
#define PARAMS_SETIF(IFID, SETTING)             DEV_DATA_IN.DevIf.Index = IFID; \
                                                DEV_DATA_IN.DevIf.Setting = SETTING;
#define PARAMS_CLEAREPFEATURE(EPID)             DEV_DATA_IN.DevFeature.Index = EPID

////////////////// USED BY USBAUDIO.C /////////////////////////
// SET REQUESTS
#define PARAMS_SETMIXER(UNITID, INPUT, OUTPUT, TYPEVAL, WLENGTH) DEV_DATA_IN.DevMixer.UnitId = UNITID; \
                                                DEV_DATA_IN.DevMixer.Input = INPUT; \
                                                DEV_DATA_IN.DevMixer.Output = OUTPUT; \
                                                DEV_DATA_IN.DevMixer.TypeVal = TYPEVAL; \
                                                DEV_DATA_IN.DevMixer.wLength = WLENGTH
#define PARAMS_SETMUTE(UNITID, CN)              DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = CN
#define PARAMS_SETMUTEALL(UNITID, NUMCN)        DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = NUMCN
#define PARAMS_SETVOL(UNITID, CN)               DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = CN
#define PARAMS_SETBASS(UNITID, CN)              DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = CN
#define PARAMS_SETTREBLE(UNITID, CN)            DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = CN
#define PARAMS_SETVOLALL(UNITID, NUMCN)         DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = NUMCN
#define PARAMS_SETBASSALL(UNITID, NUMCN)        DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = NUMCN
#define PARAMS_SETTREBLEALL(UNITID, NUMCN)      DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = NUMCN
#define PARAMS_SETFREQ(EPID)                    DEV_DATA_IN.DevAudio.Index = EPID
                                                

// GET REQUESTS
#define PARAMS_GETUNITIDTYPE(UNITID)            DEV_DATA_IN.DevAudio.Index = UNITID
                                                
#define PARAMS_GETMIXER(UNITID, INPUT, OUTPUT, TYPEVAL, WLENGTH) DEV_DATA_IN.DevMixer.UnitId = INPUT; \
                                                DEV_DATA_IN.DevMixer.Input = INPUT; \
                                                DEV_DATA_IN.DevMixer.Output = OUTPUT; \
                                                DEV_DATA_IN.DevMixer.TypeVal = TYPEVAL; \
                                                DEV_DATA_IN.DevMixer.wLength = WLENGTH
#define PARAMS_GETMUTE(UNITID, CN)              DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = CN 
#define PARAMS_GETVOL(UNITID, CN, TYPEVAL)      DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = CN; \
                                                DEV_DATA_IN.DevAudio.TypeVal = TYPEVAL
#define PARAMS_GETBASS(UNITID, CN, TYPEVAL)     DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = CN; \
                                                DEV_DATA_IN.DevAudio.TypeVal = TYPEVAL
#define PARAMS_GETTREBLE(UNITID, CN, TYPEVAL)   DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = CN; \
                                                DEV_DATA_IN.DevAudio.TypeVal = TYPEVAL
#define PARAMS_GETMUTEALL(UNITID, NUMCN, TYPEVAL)  DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = NUMCN; \
                                                DEV_DATA_IN.DevAudio.TypeVal = TYPEVAL
#define PARAMS_GETVOLALL(UNITID, NUMCN, TYPEVAL)   DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = NUMCN; \
                                                DEV_DATA_IN.DevAudio.TypeVal = TYPEVAL
#define PARAMS_GETBASSALL(UNITID, NUMCN, TYPEVAL)  DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = NUMCN; \
                                                DEV_DATA_IN.DevAudio.TypeVal = TYPEVAL
#define PARAMS_GETTREBLEALL(UNITID, NUMCN,TYPEVAL) DEV_DATA_IN.DevAudio.Index = UNITID; \
                                                DEV_DATA_IN.DevAudio.Channel = NUMCN; \
                                                DEV_DATA_IN.DevAudio.TypeVal = TYPEVAL
#define PARAMS_GETFREQ(EPID)                    DEV_DATA_IN.DevAudio.Index = EPID

////////////////// USED BY USBHID.C /////////////////////////
#define PARAMS_HIDGETREPORT(IFID, REPORTTYPE, REPORTID)     DEV_DATA_IN.DevHid.Index = IFID; \
                                                DEV_DATA_IN.DevHid.ReportId = REPORTID; \
                                                DEV_DATA_IN.DevHid.ReportType = REPORTTYPE
#define PARAMS_HIDSETREPORT(IFID, REPORTTYPE, REPORTID)     DEV_DATA_IN.DevHid.Index = IFID; \
                                                DEV_DATA_IN.DevHid.ReportId = REPORTID; \
                                                DEV_DATA_IN.DevHid.ReportType = REPORTTYPE
#define PARAMS_HIDGETIDLE(IFID)                 DEV_DATA_IN.DevHid.Index = IFID
#define PARAMS_HIDSETIDLE(IFID)                 DEV_DATA_IN.DevHid.Index = IFID
#define PARAMS_HIDGETPROTOCOL(IFID)             DEV_DATA_IN.DevHid.Index = IFID
#define PARAMS_HIDSETPROTOCOL(IFID)             DEV_DATA_IN.DevHid.Index = IFID

#define PARAMS_USBHIDHANDLER(USBREQ)            DEV_DATA_IN.DevUsbReq.UsbReq = USBREQ

#define PARAMS_DFUDNLOAD(LEN, PDATA)            DEV_DATA_IN.DevDfu.wLength = LEN; \
                                                DEV_DATA_IN.DevDfu.pData = PDATA
#define PARAMS_DFUUPLOAD(LEN, PDATA)            DEV_DATA_IN.DevDfu.wLength = LEN; \
                                                DEV_DATA_IN.DevDfu.pData = PDATA

#define PARAMS_CLASSHANDLER(USBREQUEST)         DEV_DATA_IN.DevUsbReq.UsbReq = (byte xdata *)USBREQUEST
#define PARAMS_AUDIOCLASSHANDLER(USBREQUEST)    DEV_DATA_IN.DevUsbReq.UsbReq = (byte xdata *)USBREQUEST

#define PARAMS_DFUGETSTATUS(STATE, STATUS)      DEV_DATA_IN.DevDfuStatus.state = STATE; \
                                                DEV_DATA_IN.DevDfuStatus.status = STATUS
#define PARAMS_DFU_SETUP(TARGET, POLLTABLEPTR)  DEV_ROMFUNC_DATA.DevDfuSetup.Target = TARGET; \
                                                DEV_ROMFUNC_DATA.DevDfuSetup.PollTablePtr = POLLTABLEPTR

#endif


