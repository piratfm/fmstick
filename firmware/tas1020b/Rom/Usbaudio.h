//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
#ifndef _USBAUDIO_H
#define _USBAUDIO_H

// USB AUDIO definitions
#define AUD_GET_REQ		0x80			
#define AUD_EP_REQ		0x02

// Audio Class-Specific AC interface Desc Subtype
#define AUD_SUBTYPE_UNDEF	0x00

#define AUD_SUBTYPE_MASK		0x0F
#define AUD_SUBTYPE_UNDEF		0x00
#define AUD_SUBTYPE_HEADER	0x01
#define AUD_SUBTYPE_IT			0x02
#define AUD_SUBTYPE_OT			0x03
#define AUD_SUBTYPE_MIXER		0x04
#define AUD_SUBTYPE_SEL			0x05
#define AUD_SUBTYPE_FU			0x06
#define AUD_SUBTYPE_PROC		0x07
#define AUD_SUBTYPE_EXT			0x08

// Feature unit control
#define AUD_FU_UNDEF_CNTL		0x00
#define AUD_FU_MUTE_CNTL		0x01
#define AUD_FU_VOLUME_CNTL	0x02
#define AUD_FU_BASS_CNTL		0x03
#define AUD_FU_MID_CNTL			0x04
#define AUD_FU_TREBLE_CNTL	0x05
#define AUD_FU_GEQU_CNTL		0x06
#define AUD_FU_AGC_CNTL			0x07
#define AUD_FU_DELAY_CNTL		0x08
#define AUD_FU_BBOOST_CNTL	0x09
#define AUD_FU_LOUD_CNTL		0x0A

// Audio Class-Specific Request Codes
#define AUD_REQ_UNDEFINED		0x00
#define AUD_SET_CUR					0x01
#define AUD_SET_MIN					0x02
#define AUD_SET_MAX					0x03
#define AUD_SET_RES					0x04
#define AUD_SET_MEM					0x05
#define AUD_GET_CUR					0x81
#define AUD_GET_MIN					0x82
#define AUD_GET_MAX					0x83
#define AUD_GET_RES					0x84
#define AUD_GET_MEM					0x85
#define AUD_GET_STAT				0xFF

// specific for the application
#define AUD_STREAMING_INTERFACE_NR	0x2
#define AUD_NUMBER_CHANNELS		0x2

// ===== Audio Control Interface ===================================*/
// Class Specific AC Descriptor
typedef struct _AUD_AC_INTERFACE_DESC {
	BYTE bLength;
	BYTE bDescriptorType;		// CS_INTERFACE descriptor type
	BYTE bDescriptorSubtype;	// HEADER descriptor subtype
	BYTE bcdADC_H;				// BCD, Release Number in BCD code
	BYTE bcdADC_L;				// BCD, Release Number in BCD code
	BYTE wTotalLength_H;		// Total number of bytes for this interface
	BYTE wTotalLength_L;		// including this descriptor
	BYTE bInCollection;			// number of AudioStreaming Interfaces
	BYTE baInterfaceNr[AUD_STREAMING_INTERFACE_NR];
} AUD_AC_INTERFACE_DESC;

// Input Terminal Descriptor
typedef struct _AUD_IT_DESC {
	BYTE bLength;
	BYTE bDescriptorType;		// CS_INTERFACE descriptor type
	BYTE bDescriptorSubtype;	// INPUT_TERMINAL descriptor subtype
	BYTE bTerminalID;			// Constant Terminal ID
	WORD wTerminalType;			// Terminal Type. See USB Terminal Types
	BYTE bAssocTerminal;		// ID of the Output Terminal associated with this terminal
	BYTE bNrChannels;			// Number of logical output channels
	WORD wChannelConfig;		// Bitmap, spatial location of the logical channels
	BYTE iChannelNames;			// Index of a string descriptor for the name of
								// the first logical channel.
	BYTE iTerminal;				// Index of string Input Terminal descriptor
} AUD_IT_DESC;

// Output Terminal Descriptor
typedef struct _AUD_OT_DESC {
	BYTE bLength;
	BYTE bDescriptorType;		// CS_INTERFACE descriptor type
	BYTE bDescriptorSubtype;	// OUTPUT_TERMINAL descriptor subtype
	BYTE bTerminalID;			// Constant Terminal ID
	WORD wTerminalType;			// Terminal Type. See USB Terminal Types
	BYTE bAssocTerminal;		// ID of the Input Terminal associated with this terminal
	BYTE bSourceID;				// ID of the Unit/Terminal connected to this terminal
	BYTE iTerminal;				// Index of string Output Terminal descriptor
} AUD_OT_DESC;

// Feature Unit Descriptor
typedef struct _AUD_FU_DESC {
	BYTE bLength;
	BYTE bDescriptorType;		// CS_INTERFACE descriptor type
	BYTE bDescriptorSubtype;	// FEATURE_UNIT descriptor subtype
	BYTE bUnitID;				// Constant Unit ID
	BYTE bSourceID;				// Constant ID of Unit/Terminal connected to this Unit
	BYTE bControlSize;			// size in bytes of one bmControl Array item.
	
	// Bitmap: D0:Mute D1:Volume D2:Bass D3:Mid D4:Treble 
	// D5:Graphic Equalizer D6 Automatic Gain D7: Delay 
	// D8 Bass Boost D9:Loudness, D10.. :Reserved
	BYTE bmControls[AUD_NUMBER_CHANNELS];	
	BYTE iFeature;				// Index of string Feature Unit descriptor
} AUD_FU_DESC;

// Processing Unit Descriptor... and related descriptors :
// Up/Down Mix, Dolby Prologic, 3D-Stereo Extender, Reverberation,
// Chorus, and Dynamic Range Compressor. Later, HN 06/08/99

// Extension Unit Descriptor ... Later, HN 06/08/99

// Associated Interface Descriptor ... Later, HN 06/08/99

/* ===== Audio Streaming Interface ================================*/
// Class-Specific AS Interface Descriptor
typedef struct _AUD_AS_INTERFACE_DESC {
	BYTE bLength;
	BYTE bDescriptorType;		// CS_INTERFACE descriptor type
	BYTE bDescriptorSubtype;	// AS_GENERAL descriptor subtype
	BYTE bTerminalLink;			// The terminal ID of the terminal connected
								// to the endpoint of this interface
	BYTE bDelay;				// Delay introduced by the device, value in number
								// of frames
	WORD wFormatTag;			// Audio Data Format supported by this interface
								// see USB Audio Data Format
} AUD_AS_INTERFACE_DESC;

// Class-Specific AS Isochronous Audio Data Endpoint Descriptor
typedef struct _AUD_AS_ISO_EP_DESC {
	BYTE bLength;
	BYTE bDescriptorType;		// CS_ENDPOINT descriptor type
	BYTE bDescriptorSubtype;	// EP_GENERAL descriptor subtype

	// Bitmap, set to 1 to dicate support for the related control
	// D0: Sampling Frequency, D1: Pitch, D2..D6: Reserved,
	// D7:MaxPacketsOnly
	BYTE bmAttributes;

	// Units used for the wLockDelay field
	// 0:Undefied, 1:Milliseconds, 2:Decoded PCM samples			
	BYTE bLockDelayUnits;		
								
								// of frames
	WORD wLockDelay;			// Time for locking
} AUD_AS_ISO_EP_DESC;

// AS Isochronous Synch Endpoint Descriptor, Later HN 06/08/99

/* ===== USB Audio Requests =======================================*/
// Note: If a device does not support a specific request, the device
// must indicate this by stalling the control pipe when receiving the 
// request.

#define AUD_BMREQ_SET_EP		0x22
#define AUD_BMREQ_SET_EIDIF	0x21
#define AUD_BMREQ_GET_EP		0xA2
#define AUD_BMREQ_GET_EIDIF	0xA1


#define	AUD_RQ_UNDEFINED	0x00

#define	AUD_SET_CUR			0x01
#define	AUD_SET_MIN			0x02
#define	AUD_SET_MAX			0x03
#define	AUD_SET_RES			0x04
#define	AUD_SET_MEM			0x05

#define	AUD_GET_CUR			0x81
#define	AUD_GET_MIN			0x82
#define	AUD_GET_MAX			0x83
#define	AUD_GET_RES			0x84
#define	AUD_GET_MEM			0x85
#define	AUD_GET_CSTAT		0xFF

// Note: the Set Request and Get Request have the same format
// the difference is in the direction bit in bmRequest
#define	DEF_AUD_BASE_REQUEST \
	BYTE bmRequest;	\
	BYTE bRequest;	\
	WORD wValue;	\
	WORD wIndex;	\
	WORD wLength;

typedef struct _AUD_BASE_REQUEST {
	DEF_AUD_BASE_REQUEST
} AUD_BASE_REQUEST;

/*-- Terminal Control Requests, Later HN 06/08/99 ---------------- */ 

/*-- Mixer Unit Control Requests, Later HN 06/08/99 -------------- */

/*-- Selector Unit Control Requests, Later HN 06/08/99 ----------- */

/*-- Feature Unit Control Requests ------------------------------- */
// In some case, ther eare two formats for one type of control such
// as Volume, Bass, Treble. So the wLength fields are different depend
// on each format..
// First format: When the CN field (Channel Number) is not 0xFF
// Second formt: When the CN field is 0xFF, all available channels
// are effected.
#define	AUD_ALL_CN	0xFF

// Mute Control Request
typedef struct _AUD_MUTE_CTL_REQ {
	DEF_AUD_BASE_REQUEST
	BYTE bMute;		// Bool: TRUE for mute and FALSE for not mute
} AUD_MUTE_CTL_REQ;

// Volume Control Request
typedef struct _AUD_VOL_CTL_REQ1 {
	DEF_AUD_BASE_REQUEST
	WORD wVolume;
} AUD_VOL_CTL_REQ1;

typedef struct _AUD_VOL_CTL_REQ2 {
	DEF_AUD_BASE_REQUEST
	WORD wVolume[AUD_NUMBER_CHANNELS];
} AUD_VOL_CTL_REQ2;

// Bass Control Request
typedef struct _AUD_BASS_CTL_REQ1 {
	DEF_AUD_BASE_REQUEST
	WORD wVolume;
} AUD_BASS_CTL_REQ1;

typedef struct _AUD_BASS_CTL_REQ2 {
	DEF_AUD_BASE_REQUEST
	WORD wBass[AUD_NUMBER_CHANNELS];
} AUD_BASS_CTL_REQ2;

// Mid Control Parameter Blocks
typedef struct _AUD_MID_CTL_REQ1 {
	DEF_AUD_BASE_REQUEST
	WORD wMid;
} AUD_MID_CTL_REQ1;

typedef struct _AUD_MID_CTL_REQ2 {
	DEF_AUD_BASE_REQUEST
	WORD wMid[AUD_NUMBER_CHANNELS];
} AUD_MID_CTL_REQ2;

// Treble Control Request
typedef struct _AUD_TREBLE_CTL_REQ1 {
	DEF_AUD_BASE_REQUEST
	WORD wTreble;
} AUD_TREBLE_CTL_REQ1;

typedef struct _AUD_TREBLE_CTL_REQ2 {
	DEF_AUD_BASE_REQUEST
	WORD wTreble[AUD_NUMBER_CHANNELS];
} AUD_TREBLE_CTL_REQ2;

// Graphic Equalizer Control Request, Later HN 06/08/99

// Automatic Gain Control Request, Later HN 06/08/99 

// Delay Control Request, Later HN 06/08/99

// Bass Boost Request Block, Later HN 06/08/99


/*-- Processing Unit Control Requests, Later HN 06/08/99 ---------- */

/*-- Extension Unit Control Requests, Later HN 06/08/99 ----------- */

/*-- Audio Streaming Requests ------------------------------------- */
// these requests direct to the endpoint
// Sampling Frequency Request
typedef struct _AUD_EP_SAMPLEFREQ_CTL_REQ {
	DEF_AUD_BASE_REQUEST
	BYTE tSampleFreq[3];		// sampling frequency in HZ
} AUD_EP_SAMPLEFREQ_CTL_REQ;

// Pitch Control Request
typedef struct _AUD_EP_PITCH_CTL_PB {
	DEF_AUD_BASE_REQUEST
	BYTE bPitchEnable;		// Bool
} AUD_EP_PITCH_CTL_REQ;

#define AUD_UNDEFINED_CTL 0x0000
#define AUD_SAMP_FREQ_CTL 0x0001
#define AUD_PITCH_CTL     0x0002

// Prototypes
extern void audHandler();


#endif