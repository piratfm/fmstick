//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
#ifndef DEVICE_H
#define DEVICE_H
#include "devDesc.h"

// Misc
#define STALLSpkEp	  STALLOutEp1
#define STALLHidEp	  STALLInEp3
#define STALLMicEp	  STALLInEp2

#define STALLClrSpkEp STALLClrOutEp1
#define STALLClrHidEp STALLClrInEp3
#define STALLClrMicEp STALLClrInEp2

// Enpoint state, bit sensitive.
#define	DEV_EP_STATE_HALT		0x0100
#define	DEV_EP_STATE_ENABLE		0x0200
#define DEV_EP_STATE_DISABLE	0x0400

// Speaker constant paramters
// Note: In order to conserve code space, the resolution is reported to be 1 dB units,
// although the AC97 codec volume resolution is actually in 1.5 dB units. Essentially,
// we represent that the device provides the equivalent number of volume steps (i.e.
// the AC97 range consist of 31 1.5 dB steps, while we will present 31 1 db steps).
//
#define DEV_SPK_LEFT_CN		    0x00
#define DEV_SPK_RIGHT_CN		0x01

#define DEV_SPK_MSK_VOL	    0x1F
#define DEV_SPK_MAX_VOL		((char) 0)          // 0 dB
#define DEV_SPK_MIN_VOL     ((char) -31)        // 46.5 dB
#define DEV_SPK_RES_VOL		0x01                // 1 dB
#define DEV_SPK_CUR_VOL		((char) -15)        // 22.5 dB

#define DEV_SPK_MAX_VOL_BOUND   0x0000

#define DEV_SPK_MSK_MUTE	0x01
#define DEV_SPK_MUTE_ON		0x01
#define DEV_SPK_MUTE_OFF	0x00

#define DEV_SPK_MSK_BASS	0x7F
#define DEV_SPK_MAX_BASS	0x60
#define DEV_SPK_MIN_BASS	0x00
#define DEV_SPK_RES_BASS	0x01
#define DEV_SPK_CUR_BASS	0x30

#define DEV_SPK_MSK_TREBLE	0x7F
#define DEV_SPK_MAX_TREBLE	0x60
#define DEV_SPK_MIN_TREBLE	0x00
#define DEV_SPK_RES_TREBLE	0x01
#define DEV_SPK_CUR_TREBLE	0x30

// Microphone constant paramters
#define DEV_MICREC_MSK_VOL	0x0F
#define DEV_MICREC_MAX_VOL	((char) 15)         // 22.5 dB
#define DEV_MICREC_MIN_VOL	((char) 0)          // 0 dB
#define DEV_MICREC_RES_VOL	0x01                // 1 dB
#define DEV_MICREC_CUR_VOL	((char) 8)          // 12 dB
#define DEV_MICREC_MSK_MUTE	0x01
#define DEV_MICREC_MUTE_ON	0x01
#define DEV_MICREC_MUTE_OFF	0x00

#define DEV_MICIN_MSK_VOL	0x1F

#define DEV_MICIN_MAX_VOL	((char) 8)          // 12 dB
#define DEV_MICIN_MIN_VOL	((char) -23)        // -34.5 dB
#define DEV_MICIN_RES_VOL	0x01                // 1 dB
#define DEV_MICIN_CUR_VOL	((char) -7)         // -10.5 dB
#define DEV_MICIN_MSK_MUTE	0x01
#define DEV_MICIN_MUTE_ON	0x01
#define DEV_MICIN_MUTE_OFF	0x00

// LineIn constant paramters
#define DEV_LINEIN_MSK_VOL	0x1F
#define DEV_LINEIN_MAX_VOL	((char) 8)          // 12 db
#define DEV_LINEIN_MIN_VOL	((char) -23)        // -34.5 db
#define DEV_LINEIN_RES_VOL	0x01                // 1 db
#define DEV_LINEIN_CUR_VOL	((char) -7)         // -10.5 db

#define DEV_LINEIN_MSK_MUTE	0x01
#define DEV_LINEIN_MUTE_ON	0x01
#define DEV_LINEIN_MUTE_OFF	0x00


#define	DEV_STATUS_SELFPOWER		0x0100
#define DEV_STATUS_REMOTE_WAKEUP	0x0200

// structures

// Speaker setup
typedef struct _dev_spk_struct {
	// Current left/right volumes
	char curVol[DEV_NUM_SPK_CHANNELS];

	// Previous left/right volumes
    char preVol[DEV_NUM_SPK_CHANNELS];

	// Current Bass for master channel
	byte curBass;

	// Previous Bass for master channel
	byte preBass;

	// Current Treble for master channel
	byte curTreble;

	// Previous Treble for master channel
	byte preTreble;

  // Sampling frequency
  word  freq;

} DEV_SPK_STRUCT;

// Basic microphone set
typedef struct _dev_micRec_struct {
	// Current volume
	char curVol;

	// Previous volume
	char preVol;

  // Sampling frequency
  word  freq;
} DEV_MICREC_STRUCT;

// Basic microphone feadback set
typedef struct _dev_micIn_struct {
	// Current volume
	char curVol;

	// Previous volume
	char preVol;

} DEV_MICIN_STRUCT;

// Basic Linein set
typedef struct _dev_lineIn_struct {
	// Current volume
	char curVol[2];

	// Previous volume
	char preVol[2];

} DEV_LINEIN_STRUCT;

// Basic interface structures
typedef struct _dev_interface_struct {
	// Interface current setting
	byte curSetting;

	// End point state
	word epState;
} DEV_INTERFACE_STRUCT;

// Device record
typedef struct _dev_device_struct {
	// Device selfpowered/remote wakeup
	word devStatus;

	// Configuration current setting
	byte configSetting;

	// Interfaces		
	// Audio control
	struct _dev_interface_struct audCtlrIf;

	// Speaker set setup parameters
	struct _dev_spk_struct spk;

	// microphone Record setup paramters
	struct _dev_micRec_struct micRec;

	// Line In setup paramters
	struct _dev_lineIn_struct lineIn;	

	// microphone Feed back setup paramters
	struct _dev_micIn_struct micIn;

	// Speaker set interface 
	struct _dev_interface_struct spkIf;

	// Microphone Record interface
	struct _dev_interface_struct micRecIf;

	// HID interface for the speaker set
	struct _dev_interface_struct hidIf;

	// DFU interface for the speaker set
	struct _dev_interface_struct dfuIf;

    // Dummy values
	union
	{
    	word wData[2];
		byte bData[3];
		
	}dummy;

    // Mute temporary storage
    byte mute;

} DEV_DEVICE_STRUCT;

// References
extern DEV_DEVICE_STRUCT AppDevice;
extern bit AppDeviceSpkcurMute;
extern bit AppDeviceSpkpreMute;
extern bit AppDeviceMicReccurMute;
extern bit AppDeviceMicIncurMute;
extern bit SpkEnaStatusChanged;
extern bit SpkEnaStatus;
extern bit AppResetFlag;
extern bit AppSuspendFlag;
extern DFU_STATUS_STRUCT DfuStatusData;

// Structures to handle PLL
typedef struct _tag_DEV_SOFT_ACG {
    word cap1;
    word cap2;
    word frq;
    word acc;
    word oldAcc;
    byte count;
    byte content;
} DEV_SOFT_ACG_STRUCT;

extern DEV_SOFT_ACG_STRUCT AppDeviceSoftAcg;

// for testing DFU DnLoad/Upload when target is DFU_TARGET_OTHER
extern byte *DevTestDfuDataPtr;

// extern prototypes
extern void devInitSTC();
extern void DevFunctionEntryParser(byte ,void xdata *);
#endif
