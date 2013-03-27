//==================================================
// Texas Instruments Strictly Private
// Copyright 1999, Texas Instruments Inc.
//==================================================
/*=======================================================
devDesc.h: header file for DevDesc.cpp which contains all
the descriptors.
=======================================================*/
#include "..\rom\types.h"
#include "..\rom\usb.h"
#include "..\rom\usbaudio.h"
#include "..\rom\usbhid.h"

#ifndef _DEVDESC_H
#define _DEVDESC_H

#define DEV_VENDOR_ID       0x0451      // Vendor id
#define DEV_PRODUCT_ID      0x2100      // Product id (TSC2100

#ifdef _TAS1020_
#define DEV_DEVICE_REL      0x0100      // Device release 
#else
#define DEV_DEVICE_REL      0x0101      // Device release 
#endif

#ifdef _DFU_
#define DFU_IF_COUNT        1
#else
#define DFU_IF_COUNT        0
#endif

#ifdef _HID_
#define HID_IF_COUNT        1
#else
#define HID_IF_COUNT        0
#endif

#ifdef _MIC_
#define DEV_MAX_IF (3 + DFU_IF_COUNT + HID_IF_COUNT) // #of interfaces
#else
#define DEV_MAX_IF (2 + DFU_IF_COUNT + HID_IF_COUNT) // #of interfaces
#endif

#define DEV_MAX_EP0_PKT 	0x08

//
// Configuration descriptor 
//

// attributes
#define CFG_DESC_ATTR_DEFAULT   0x80     // Reserved bit, must be set to 1

// power
#ifdef SELFPOWER   // when SELF-POWER 

#ifdef REMOTE      // if REMOTE
#define DEV_CONFIG_ATTRIBUTES   CFG_DESC_ATTR_DEFAULT | CFG_DESC_ATTR_SELF_POWERED | CFG_DESC_ATTR_REMOTE_WAKE
#else              // if Non-remote 
#define DEV_CONFIG_ATTRIBUTES   CFG_DESC_ATTR_DEFAULT | CFG_DESC_ATTR_SELF_POWERED
#endif
#define DEV_CONFIG_MAX_POWER    0       // 0 mA

#else             // when bus powered

#ifdef REMOTE     // if REMOTE
#define DEV_CONFIG_ATTRIBUTES   CFG_DESC_ATTR_DEFAULT | CFG_DESC_ATTR_REMOTE_WAKE
#define DEV_CONFIG_MAX_POWER    250     // 500 mA
#else             // if Non-REMOTE
#define DEV_CONFIG_ATTRIBUTES   CFG_DESC_ATTR_DEFAULT
#define DEV_CONFIG_MAX_POWER    (100/2) // MaxPower 100 mA
#endif

#endif                                

// Frequencies   
#define SAMPLING_FREQ_48		48000
#define SAMPLING_FREQ_441		44100
#define SAMPLING_FREQ_2205		22050
#define SAMPLING_FREQ_11025		11025
#define SAMPLING_FREQ_8			8000					

// Packet size
#define DEV_SPK_MAX_PKT         192   // 2channels, 16bit samples@48Khz
#define DEV_MIC_MAX_PKT         192   // 2channel, 16bit samples@48Khz

// mic channel controls
#define MASTER_MIC_CHANNEL_CNTL       AUD_FU_BM_MUTE | AUD_FU_BM_VOLUME
#define INDIVIDUAL_MIC_CHANNEL_CNTL   0X00

// speaker channel controls
#define MASTER_SPK_CHANNEL_CNTL       AUD_FU_BM_MUTE
#define INDIVIDUAL_SPK_CHANNEL_CNTL   AUD_FU_BM_VOLUME

// Stereo channels
#define CHANNELS_STEREO               AUD_LOC_LEFT | AUD_LOC_RIGHT

// bandwith settings
#define DEV_SPK_SETTING_0_BANDWIDTH     0 // Zero bandwidth alternate setting
#define DEV_SPK_SETTING_FULL_BANDWIDTH  1 // Full bandwidth alternate setting

#define DEV_MIC_SETTING_0_BANDWIDTH     0 // Zero bandwidth alternate setting
#define DEV_MIC_SETTING_FULL_BANDWIDTH  1 // Full bandwidth alternate setting

//
// Interface descriptors
//
// interface Ids/numbers
#define DEV_AUD_CTLR_IF_ID          0x00
#define DEV_SPK_IF_ID               0x01

#ifdef _MIC_
#define DEV_MIC_IF_ID               0x02
#undef  LAST_INTERFACE
#define LAST_INTERFACE              DEV_MIC_IF_ID
#endif

#ifdef  _HID_

#ifdef _MIC_
#define DEV_HID_IF_ID               (DEV_MIC_IF_ID + 1)
#else
#define DEV_HID_IF_ID               (DEV_SPK_IF_ID + 1)
#endif

#undef  LAST_INTERFACE
#define LAST_INTERFACE              DEV_HID_IF_ID
#endif

#define DEV_DFU_IF_ID               (LAST_INTERFACE + 1)

#define DEV_SPK_SETTING_0           0x00
#define DEV_SPK_SETTING_1           0x01
#define DEV_MIC_SETTING_0           0x00
#define DEV_MIC_SETTING_1           0x01

// Audio Control definitions
#define DEV_MICIN_IT_ID             0x01
#define DEV_MICIN_FU_ID             0x02
#define DEV_HOST_IT_ID              0x03
#define DEV_SPK_MIXER_ID            0x04
#define DEV_SPK_FU_ID               0x05
#define DEV_SPK_OT_ID               0x06
#define DEV_MICREC_IT_ID            0x07
#define DEV_MICREC_FU_ID            0x08
#define DEV_MICREC_SEL_ID           0x09
#define DEV_MICREC_MIXER_ID         0x0A
#define DEV_MICREC_OT_ID            0x0B

// Endpoints.
#define DEV_AUD_CTLR_EP_ID          0x00
#define DEV_SPK_EP_ID               0x01
#define DEV_MIC_EP_ID               0x82
#define DEV_HID_EP_ID               0x83
#define DEV_HIDO_EP_ID              0x04

// HID definitions 
#define DEV_HID_MAX_PKT             0x2A   // maximum packet size
#define DEV_HID_PRATE               0x0A   // 10ms interval  

// String descriptors
#define DEV_LANG_STR_IDX            0x00
#define DEV_COMPANY_STR_IDX         0x01
#define DEV_PROD_STR_IDX            0x02
#define DEV_FW_VERSION_IDX          0xFF

#ifdef  _MIC_
#define DEV_NUM_AS_INTERFACES       2       // # of audio streaming interfaces
#else
#define DEV_NUM_AS_INTERFACES       1       // # of audio streaming interfaces
#endif

#ifdef  _STEREO_MIC
#define DEV_NUM_MIC_CHANNELS        2       // # of channels for mic (i.e. stereo)
#else
#define DEV_NUM_MIC_CHANNELS        1       // # of channels for mic (i.e. mono)
#endif
#define DEV_NUM_USB_SPK_CHANNELS    2       // # of channels of audio coming from host
#define DEV_NUM_SPK_CHANNELS        2       // # of channels output from device.
#define DEV_FU_CONTROL_SIZE         1       // FU bmaControl size.
#define DEV_NUM_UP_DN_MIX_MODES     2       // # of up/down mixer modes supported.
#define DEV_NUM_HID_DESCRIPTORS     1       // # of HID descriptors.
#ifdef	_FREQ_MIC_RANGE_
#define	DEV_NUM_MIC_FREQUENCIES     0       // Using frequency Range for iMAC
#else
#define DEV_NUM_MIC_FREQUENCIES     1       // # of discrete frequencies
#endif

#ifdef	_FREQ_SPK_RANGE_
#define	DEV_NUM_SPK_FREQUENCIES     0       // Using frequency Range for iMAC
#else
#define DEV_NUM_SPK_FREQUENCIES     1       // # of discrete frequencies
#endif

#define DEV_SPK_SUBFRAME_SIZE	    2
#define DEV_NUM_BYTE_PER_SAMPLE (DEV_SPK_SUBFRAME_SIZE * DEV_NUM_SPK_CHANNELS) 

// Mixer definitions
#define DEV_MIXER_BMCONTROL_VALUES  0x00    // No programable
#define DEV_NUM_SPK_MIX_INPUT       2       // #of Mic feedback mixer inputs
#define DEV_NUM_SPK_MIX_IN_CH       3       // #of Mic feedback input channels  
#define DEV_NUM_SPK_MIX_OUT_CH      2       // #of Mic feedback output channels
  
//
// Class specific AC interface descriptor...2 interfaces
//
TYPEDEF_AUD_AC_INTERFACE_DESC(AUD_AC_INTERFACE_DESCX, DEV_NUM_AS_INTERFACES);

// 
// Feature unit descriptor for mic...1 channel (mono)
//
TYPEDEF_FEATURE_UNIT_DESC(AUD_MIC_FU_DESC, DEV_NUM_MIC_CHANNELS, DEV_FU_CONTROL_SIZE);

//
// Up/down mixer unit descriptor...6 logical channels in, 2 modes supported.
//
TYPEDEF_UP_DOWN_MIXER_UNIT_DESC(AUD_UP_DOWN_MIX_DESC, DEV_NUM_UP_DN_MIX_MODES);

//
// Mic feedback mixer unit descriptor...2 inputs sources, 2 channels on input, 2 channels of output
//
TYPEDEF_MIXER_UNIT_DESC(AUD_SPK_MIX_DESC, DEV_NUM_SPK_MIX_INPUT, DEV_NUM_SPK_MIX_IN_CH, DEV_NUM_SPK_MIX_OUT_CH);

//
//  Feature unit descriptor...2 output channels
//
TYPEDEF_FEATURE_UNIT_DESC(AUD_SPK_FU_DESC, DEV_NUM_SPK_CHANNELS, DEV_FU_CONTROL_SIZE);

//
//  Mic Recording Selector descriptor...1 pin in
//  This is for Net Meeting. HN 10/25/00
#define DEV_NUM_MICREC_SELECTOR_INPINS  1
TYPEDEF_SELECTOR_UNIT_DESC(AUD_MICREC_SELECTOR_DESC,DEV_NUM_MICREC_SELECTOR_INPINS);
//
// Mic recording mixer unit descriptor...1 input sources, 
// 1 channels on input, 2 channels of output
//
#define DEV_NUM_MICREC_MIX_INPUT  1
#define DEV_NUM_MICREC_MIX_IN_CH  DEV_NUM_MIC_CHANNELS // #of Mic record input channels  
#define DEV_NUM_MICREC_MIX_OUT_CH DEV_NUM_MIC_CHANNELS // #of Mic record output channels

#define DEV_MICREC_MIXER_BMCONTROL_VALUES  0x00    // No programable

TYPEDEF_MIXER_UNIT_DESC(AUD_MICREC_MIX_DESC, DEV_NUM_MICREC_MIX_INPUT, DEV_NUM_MICREC_MIX_IN_CH, DEV_NUM_MICREC_MIX_OUT_CH);
 
#ifdef	_FREQ_MIC_RANGE_
// iMAC does not supports well discrete frequencies yet, HN, 10/24/00
typedef DATA_TYPE1_FREQ_RANGE DATA_TYPE1_MIC_FREQ;
#else
// Type 1 data, discrecte frequencies.
TYPEDEF_DATA_TYPE1_DISCRETE(DATA_TYPE1_MIC_FREQ, DEV_NUM_MIC_FREQUENCIES);
#endif

#ifdef	_FREQ_SPK_RANGE_
// iMAC does not supports well discrete frequencies yet, HN, 10/24/00
typedef DATA_TYPE1_FREQ_RANGE DATA_TYPE1_SPK_FREQ;//
#else
// Type 1 data, discrecte frequencies.
TYPEDEF_DATA_TYPE1_DISCRETE(DATA_TYPE1_SPK_FREQ, DEV_NUM_SPK_FREQUENCIES);
#endif

//
// HID descriptor with a single class descriptor.
//
TYPEDEF_HID_DESC(HID_DESC, DEV_NUM_HID_DESCRIPTORS);
TYPEDEF_LANGID_DESC(LANGID_DESC, 1);

#ifdef _TAS1020_
#define UNICODE_PRODUCT_NAME    "T""\X00" \
                                "A""\X00" \
                                "S""\X00" \
                                "1""\X00" \
                                "0""\X00" \
                                "2""\X00" \
                                "0""\X00" \
                                " ""\X00" \
                                "U""\X00" \
                                "S""\X00" \
                                "B""\X00" \
                                " ""\X00" \
                                "H""\X00" \
                                "e""\X00" \
                                "a""\X00" \
                                "d""\X00" \
                                "s""\X00" \
                                "e""\X00" \
                                "t""\X00" \
                                " ""\X00" \
                                "R""\X00" \
                                "e""\X00" \
                                "f""\X00" \
                                "e""\X00" \
                                "r""\X00" \
                                "e""\X00" \
                                "n""\X00" \
                                "c""\X00" \
                                "e""\X00" \
                                " ""\X00" \
                                "D""\X00" \
                                "e""\X00" \
                                "s""\X00" \
                                "i""\X00" \
                                "g""\X00" \
                                "n"
#else
#define UNICODE_PRODUCT_NAME    "T""\X00" \
                                "A""\X00" \
                                "S""\X00" \
                                "1""\X00" \
                                "0""\X00" \
                                "2""\X00" \
                                "0""\X00" \
                                "A""\X00" \
                                "/""\X00" \
                                "S""\X00" \
                                "I""\X00" \
                                "4""\X00" \
                                "7""\X00" \
                                "1""\X00" \
                                "1""\X00" \
                                " ""\X00" \
                                "T""\X00" \
                                "r""\X00" \
                                "a""\X00" \
                                "n""\X00" \
                                "s""\X00" \
                                "m""\X00" \
                                "i""\X00" \
                                "t""\X00" \
                                "t""\X00" \
                                "e""\X00" \
                                "r"
#endif

#define UNICODE_VENDOR_NAME     "T""\X00" \
                                "e""\X00" \
                                "x""\X00" \
                                "a""\X00" \
                                "s""\X00" \
                                " ""\X00" \
                                "I""\X00" \
                                "n""\X00" \
                                "s""\X00" \
                                "t""\X00" \
                                "r""\X00" \
                                "u""\X00" \
                                "m""\X00" \
                                "e""\X00" \
                                "n""\X00" \
                                "t""\X00" \
                                "s"

#define UNICODE_FW_VERSION      "V""\X00" \
                                "0""\X00" \
                                "1""\X00" \
                                "0""\X00" \
                                "1"

TYPEDEF_UNICODE_STRING_DESC(FW_VERSION_DESC, sizeof(UNICODE_FW_VERSION));
TYPEDEF_UNICODE_STRING_DESC(PRODUCT_DESC, sizeof(UNICODE_PRODUCT_NAME));
TYPEDEF_UNICODE_STRING_DESC(VENDOR_DESC, sizeof(UNICODE_VENDOR_NAME));

// 
// Audio control descriptor
//
typedef struct AC_INTERFACE_tag
{
    INTERFACE_DESCRIPTOR StdACInterfaceDesc;     // Std. Audio control interface.
    AUD_AC_INTERFACE_DESCX ClassACInterfaceDesc; // Class specific AC interface.
    AUD_IT_DESC USBOutInputTermDesc;             // USB out (from host) input terminal

#ifdef _MIC_FEED_BACK
    AUD_IT_DESC MicFeedbackInputTermDesc;       // Mic input terminal
    AUD_MIC_FU_DESC MicFeedbackFUDesc;          // Mic functional unit descriptor.
    AUD_SPK_MIX_DESC SpkMixDesc;                // Speaker mixer unit descriptor.
#endif

    AUD_SPK_FU_DESC SpeakerFUDesc;              // Speaker output feature unit descriptor
    AUD_OT_DESC SpeakerOutputTermDesc;          // Speaker output terminal descriptor
#ifdef _MIC_
    AUD_IT_DESC MicInputTermDesc;               // Mic input terminal
    AUD_MIC_FU_DESC MicFUDesc;                  // Mic feature unit descriptor.
#endif

#ifdef _DEV_MICREC_SELECTOR_ADD
    AUD_MICREC_SELECTOR_DESC MicSELDesc;        // Mic Selector desccritor   
#else
// Here we use the mixer to see if it works with netMeeting
#ifdef _DEV_MICREC_MIXER_ADD
    AUD_MICREC_MIX_DESC MicRecMixerDesc;
#endif
#endif

#ifdef _MIC_    
    AUD_OT_DESC MicOutputTermDesc;              // USB in (to host) output terminal
#endif

} AC_INTERFACE;

typedef struct CONFIG_DESCRIPTOR_DATA_tag
{   
    CONFIG_DESCRIPTOR ConfigDesc;               // Configuration descriptor
    AC_INTERFACE AcInterface;                   // AC interface

    ////////////////////////////////////////////////////////////////////////////
    // Speaker streaming interface
    ////////////////////////////////////////////////////////////////////////////   
                                                        // Speaker Std. AS interface, 
    INTERFACE_DESCRIPTOR SpeakerStdASInterface0Desc;    // Alternate setting 0 (zero bandwidth)
                                                        // Speaker Std. AS interface, 
    INTERFACE_DESCRIPTOR SpeakerStdASInterface1Desc;    // Alternate setting 1
    AUD_AS_INTERFACE_DESC SpeakerClassInterfaceDesc;    // Class specific AS general interface descriptor
    DATA_TYPE1_SPK_FREQ SpeakerDataType1FormatDesc;     // Data type 1 format descriptor
    AUD_STD_AS_ISO_EP_DESC SpeakerStdIsoAudioEpDesc;    // Endpoint descriptor
    AUD_CS_AS_ISO_EP_DESC SpeakerClassIsoAudioEpDesc;   // Class specific ISO audio data endpoint.

#ifdef _MIC_
    ////////////////////////////////////////////////////////////////////////////
    // Mic streaming interface
    ////////////////////////////////////////////////////////////////////////////
                                                        // Std. AS interface, 
    INTERFACE_DESCRIPTOR MicStdASInterface0Desc;        // Alternate setting 0 (zero bandwidth)
                                                        // Std. AS interface, 
    INTERFACE_DESCRIPTOR MicStdASInterface1Desc;        // Alternate setting 1
    AUD_AS_INTERFACE_DESC MicClassInterfaceDesc;        // Class specific AS general interface descriptor
    DATA_TYPE1_MIC_FREQ MicDataType1FormatDesc;         // Data type 1 format descriptor
    AUD_STD_AS_ISO_EP_DESC MicStdIsoAudioEpDesc;        // Endpoint descriptor
    AUD_CS_AS_ISO_EP_DESC MicClassIsoAudioEpDesc;       // Class specific ISO audio data endpoint.
#endif

#ifdef _HID_
    ////////////////////////////////////////////////////////////////////////////
    // HID interface
    ////////////////////////////////////////////////////////////////////////////
    INTERFACE_DESCRIPTOR HidStdInterfaceDesc;    // Hid std. interface descriptor.
    HID_DESC HidDesc;                            // Hid descriptor.
    ENDPOINT_DESCRIPTOR HidEpDesc;               // Hid endpoint descriptor.
    ENDPOINT_DESCRIPTOR HidEpODesc;             // Hid output endpoint descriptor.
#endif

#ifdef _DFU_
    ////////////////////////////////////////////////////////////////////////////
    // DFU run-time descriptors
    ////////////////////////////////////////////////////////////////////////////
	INTERFACE_DESCRIPTOR DfuInterfaceDesc;		 // Dfu class i/f descriptor.
	DFU_FUNCTIONAL_DESC DfuFunctionalDesc;		 // Dfu functional descriptor.
#endif

} CONFIG_DESCRIPTOR_DATA;

typedef struct DFU_CONFIG_DESCRIPTOR_DATA_tag
{   
    CONFIG_DESCRIPTOR ConfigDesc;                // Configuration descriptor
	INTERFACE_DESCRIPTOR DfuInterfaceDesc;		 // Dfu class i/f descriptor.
	DFU_FUNCTIONAL_DESC DfuFunctionalDesc;		 // Dfu functional descriptor.

} DFU_CONFIG_DESCRIPTOR_DATA;

// Proto 
extern LANGID_DESC code DevLangIdDesc;
extern PRODUCT_DESC code DevProductStringDesc;
extern VENDOR_DESC code DevCompanyStringDesc;
extern FW_VERSION_DESC code DevFwVersion;
extern DEVICE_DESCRIPTOR code DevDesc;
extern byte code DevHidReportDesc[];
extern CONFIG_DESCRIPTOR_DATA code ConfigDesc;
extern DEVICE_DESCRIPTOR code DfuDevDesc;
extern DFU_CONFIG_DESCRIPTOR_DATA code AppDfuConfigDesc;
extern WORD GetHidReportDescSize();
extern byte code DevDfuPollTimeTable[];

#endif