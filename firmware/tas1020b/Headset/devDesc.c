//==================================================
// Texas Instruments Strictly Private
// Copyright 2000, Texas Instruments Inc.
//==================================================
/*==================================================
DEVICEDESC.C
Device Descriptors
//================================================*/
#include <Reg52.h>
#include "..\rom\types.h"
#include "..\rom\usb.h"
#include "..\rom\UsbEng.h"
#include "..\rom\usbhid.h"
#include "..\rom\usbdfu.h"
#include "usbapp.h"
#include "devDesc.h"

//
// Device descriptor
//
DEVICE_DESCRIPTOR code DevDesc =
{
    sizeof(DEVICE_DESCRIPTOR),              // 18 bytes long
    DESC_TYPE_DEVICE,                       // device descriptor type
    MAKE_LITTLE_ENDIAN(0x0110),             // USB spec 1.10, LSB
    0x00,                                   // class code - enumarated at interface
    0x00,                                   // subclass code
    0x00,                                   // protocol code
    DEV_MAX_EP0_PKT,                        // max EP0 packet size
    MAKE_LITTLE_ENDIAN(DEV_VENDOR_ID),      // Vendor id
    MAKE_LITTLE_ENDIAN(DEV_PRODUCT_ID),     // Product id
    MAKE_LITTLE_ENDIAN(DEV_DEVICE_REL),     // Device release # (bcd)
    0x01,                                   // iManufactirer
    0x02,                                   // iProduct
    0x00,                                   // iSerialNumber
    0x01                                    // bNumConfigurations
};

#ifdef _HID_
//
// Hid report descriptor for mouse
//



byte code DevHidReportDesc[] = {   
#ifdef _SI4711_
	//for i2c (got from RocketFM)
    0x05, 0x0c,              // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,              // USAGE (Consumer Control)
    0xa1, 0x01,              // COLLECTION (Application)
    0x15, 0x00,              //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,        //   LOGICAL_MAXIMUM (255)
    0x09, 0x01,              //   USAGE (Consumer Control)
    0x75, 0x08,              //   REPORT_SIZE (8)
    0x95, 0x2a,              //   REPORT_COUNT (42)
    0x81, 0x02,              //   INPUT (Data,Var,Abs)
    0x09, 0x01,              //   USAGE (Consumer Control)
    0x75, 0x08,              //   REPORT_SIZE (8)
    0x95, 0x2a,              //   REPORT_COUNT (42)
    0x91, 0x02,              //   OUTPUT (Data,Var,Abs)
    0xc0                     // END_COLLECTION
#else
	//for mouse
    0x05, 0x01,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x00,              // USAGE (none)
    0xA1, 0x01,              // COLLECTION (Application)
    0x05, 0x01,              // #1   Usage Page (Generic Desktop)
    0x09, 0x30,              //      Usage (X)
    0x15, 0x00,              //      LOGICAL_MINIMUM (0)
    0x25, 0xFF,              //      LOGICAL_MAXIMUM (255)
    0x75, 0x08,              //      REPORT_SIZE (8)
    0x95, 0x2A,              //      REPORT_COUNT (42)
    0x81, 0x02,              //      Input (Data, Var, abs) 
    0x05, 0x01,              // #2   Usage Page (Generic Desktop)
    0x09, 0x31,              //      Usage (Y)
    0x15, 0x00,              //      LOGICAL_MINIMUM (0)
    0x25, 0xFF,              //      LOGICAL_MAXIMUM (255)
    0x75, 0x08,              //      REPORT_SIZE (8)
    0x95, 0x2A,              //      REPORT_COUNT (42)
    0x91, 0x02,              //      Output (Data, Var, abs)
    0xC0                     // END_COLLECTION
#endif
};   
#endif

// Configuration descriptor
CONFIG_DESCRIPTOR_DATA code ConfigDesc =
{
    // Configuration
    {
        sizeof(CONFIG_DESCRIPTOR),             // bLength
        DESC_TYPE_CONFIG,	 	 	           // bDescriptorType
        MAKE_LITTLE_ENDIAN(sizeof(CONFIG_DESCRIPTOR_DATA)), // wTotalLength
        DEV_MAX_IF,	 	 	 	               // bNumInterfaces
        0x01,	 	 	 	                   // bConfigurationValue
        0x00,	 	 	 	                   // iConfiguration
        DEV_CONFIG_ATTRIBUTES,                 // bmAttributes
        DEV_CONFIG_MAX_POWER                   // MaxPower
    },
    
    // ============================================
    // Audio Control Descriptor
    // ===========================================
    {
        // ============================================
        // Audio Control Standard Interface Descriptor
        // ===========================================
        {
            sizeof(INTERFACE_DESCRIPTOR),	 	// bLength
            DESC_TYPE_INTERFACE,	 	 	 	// bDescriptorType
            DEV_AUD_CTLR_IF_ID,	 	 	 	    // bInterfaceNumber
            0x00,	 	 	 	                // bAlternateSetting
            0x00,	 	 	 	                // bNumEndpoints (default)
            USB_AUDIO_CLASS_CODE,               // bInterfaceClass
            AUD_SUBCLASS_AUDIOCONTROL,          // bInterfaceSubClass 
            0x00,	 	 	 	                // bInterfaceProtocol
            0x00                                // iInterface
        },
       
        
        // ==========================================================
        // Audio Control Interface Header (Class Specific interface)
        // ==========================================================
        {        
            sizeof(AUD_AC_INTERFACE_DESCX),      // bLength
            AUD_CS_INTERFACE,	 	 	 	     // bDescriptorType
            AUD_SUBTYPE_HEADER,                  // bDescriptorSubtype
            MAKE_LITTLE_ENDIAN(AUD_BCD_REL_1_0), // BcdADC
            MAKE_LITTLE_ENDIAN(sizeof(AC_INTERFACE) 
                 - sizeof(INTERFACE_DESCRIPTOR)),// wTotalLength
            DEV_NUM_AS_INTERFACES,               // bInCollection
            {
                DEV_SPK_IF_ID	 	 	 	     // baInterfaceNr[1]
#ifdef _MIC_
				,
                DEV_MIC_IF_ID	 	 	 	     // baInterfaceNr[2]
#endif
            }
        },

        //-------- For Host USB audio stream out -----
        // ============================================
        // Audio Control Input Terminal Descriptor
        // ===========================================
        {        
            sizeof(AUD_IT_DESC),	 	 	 	 // bLength
            AUD_CS_INTERFACE,	 	 	 	     // bDescriptorType
            AUD_SUBTYPE_IT,	 	 	 	         // bDescriptorSubtype
            DEV_HOST_IT_ID,	 	 	 	         // bTerminalID
            MAKE_LITTLE_ENDIAN(AUD_USB_TYPE_USB_STREAMING), // wTerminalType
            0x00,	 	 	 	                 // bAssocTerminal
            DEV_NUM_USB_SPK_CHANNELS,	 	 	 // bNrChannels
            MAKE_LITTLE_ENDIAN(CHANNELS_STEREO), // wChannelConfig
            0x00,                                // iChannelNames
            0x00                                 // iTerminal
        },    

#ifdef _MIC_FEED_BACK
        //---------- For Microphone Feedback ----------
        // ============================================
        // Audio Control Input Terminal Descriptor
        // ===========================================
        {    
            sizeof(AUD_IT_DESC),	 	 	 	 // bLength
            AUD_CS_INTERFACE,	 	 	 	     // bDescriptorType
            AUD_SUBTYPE_IT,	 	 	 	         // bDescriptorSubtype
            DEV_MICIN_IT_ID,	 	 	 	     // bTerminalID
            MAKE_LITTLE_ENDIAN(AUD_IT_TYPE_MICROPHONE),	// wTerminalType
            DEV_MICREC_IT_ID,                    // bAssocTerminal
            DEV_NUM_MIC_CHANNELS,	 	 	 	 // bNrChannels
            MAKE_LITTLE_ENDIAN(AUD_LOC_LEFT),    // wChannelConfig
            0x00,                                // iChannelNames
            0x00                                 // iTerminal
        },    
        
        //-------- For Microphone Feedback ----------
        // ============================================
        // Audio Control Feature Unit Descriptor
        // ===========================================
        {
            sizeof(AUD_MIC_FU_DESC),             // bLength
            AUD_CS_INTERFACE,	 	 	 	     // bDescriptorType
            AUD_SUBTYPE_FU,	 	 	             // bDescriptorSubtype
            DEV_MICIN_FU_ID,                     // bUnitId
            DEV_MICIN_IT_ID,                     // bSourceId
            DEV_FU_CONTROL_SIZE,                 // bControlSize
            {
                MASTER_MIC_CHANNEL_CNTL,         // bmaControls[0]
                INDIVIDUAL_MIC_CHANNEL_CNTL      // bmaControls[1]
            },
            0x00                                 //iFeature
        },

        //--------- For Microphone Mixer ---------
        // ============================================
        // Audio Control Mixer Unit Descriptor
        // ===========================================
        {        
            sizeof(AUD_SPK_MIX_DESC),            // bLength
            AUD_CS_INTERFACE,                    // bDescriptorType
            AUD_SUBTYPE_MIXER,                   // bDescriptorSubtype
            DEV_SPK_MIXER_ID,                    // bTerminalID
            DEV_NUM_SPK_MIX_INPUT,               // bNrInPins
            DEV_HOST_IT_ID,                      // baSourceId[1]
            DEV_MICIN_FU_ID,                     // baSourceId[2]
            DEV_NUM_SPK_MIX_OUT_CH,              // bNrChannels
            MAKE_LITTLE_ENDIAN(AUD_LOC_LEFT|AUD_LOC_RIGHT), //wChannelConfig
            0x00,                                // iChannelNames
            { DEV_MIXER_BMCONTROL_VALUES },      // bmControls
            0x00                                 // iMixer
        },
#endif
        
        //---------------- For Speaker Ouput ----------
        // ============================================
        // Audio Control Feature Unit Descriptor
        // ===========================================
        {
            sizeof(AUD_SPK_FU_DESC),             // bLength
            AUD_CS_INTERFACE,                    // bDescriptorType
            AUD_SUBTYPE_FU,                      // bDescriptorSubtype
            DEV_SPK_FU_ID,                       // bUnitId
#ifdef _MIC_FEED_BACK                            // bSourceId
            DEV_SPK_MIXER_ID,
#else
			DEV_HOST_IT_ID,
#endif
            DEV_FU_CONTROL_SIZE,                 // bControlSize                             
            {
               MASTER_SPK_CHANNEL_CNTL,          // bmaControls[0]
               INDIVIDUAL_SPK_CHANNEL_CNTL,      // bmaControls[1]
               INDIVIDUAL_SPK_CHANNEL_CNTL       // bmaControls[2]
            },
            0x00                                 // iFeature
        },

        //------------ For Speaker Ouput ----------
        // ============================================
        // Audio Control Output Terminal Descriptor
        // ===========================================
        {    
            sizeof(AUD_OT_DESC),                  // bLength
            AUD_CS_INTERFACE,                     // bDescriptorType
            AUD_SUBTYPE_OT,                       // bDescriptorSubtype
            DEV_SPK_OT_ID,                        // bTerminalID
            MAKE_LITTLE_ENDIAN(AUD_OT_TYPE_SPEAKER),  // wTerminalType
            0x00,                                 // bAssocTerminal
            DEV_SPK_FU_ID,                        // bSourceID
            0x00                                  // iTerminal
        },

#ifdef _MIC_
        //---------------- For Microphone ------------
        // ============================================
        // Audio Control Input Terminal Descriptor
        // ===========================================
        {        
            sizeof(AUD_IT_DESC),                  // bLength
            AUD_CS_INTERFACE,                     // bDescriptorType
            AUD_SUBTYPE_IT,                       // bDescriptorSubtype
            DEV_MICREC_IT_ID,                     // bTerminalID
            MAKE_LITTLE_ENDIAN(AUD_IT_TYPE_MICROPHONE), // wTerminalType           
            DEV_MICIN_IT_ID,                      // bAssocTerminal
            DEV_NUM_MIC_CHANNELS,	 	 	 	  // bNrChannels
            MAKE_LITTLE_ENDIAN(AUD_LOC_LEFT),     // wChannelConfig
            0x00,                                 // iChannelNames
            0x00                                  // iTerminal
        },
        
        //---------------- For Microphone ------------
        // ============================================
        // Audio Control Feature Unit
        // ===========================================
        {        
            sizeof(AUD_MIC_FU_DESC),             // bLength
            AUD_CS_INTERFACE,                    // bDescriptorType
            AUD_SUBTYPE_FU,                      // bDescriptorSubtype
            DEV_MICREC_FU_ID,                    // bUnitId
            DEV_MICREC_IT_ID,                    // bSourceId
            DEV_FU_CONTROL_SIZE,                 // bControlSize
            {                                    // bmaControls[]
                MASTER_MIC_CHANNEL_CNTL,         //   master mute,volume
                INDIVIDUAL_MIC_CHANNEL_CNTL      //   channle 1
            },
            0x00                                 // iFeature
        },

#ifdef _DEV_MICREC_SELECTOR_ADD
        // ============================================
        // Audio Control Mic Record Selector Descriptor
        // ===========================================
        {        
            sizeof(AUD_MICREC_SELECTOR_DESC), // bLength
            AUD_CS_INTERFACE,                 // bDescriptorType
            AUD_SUBTYPE_SEL,                  // bDescriptorSubtype
            DEV_MICREC_SEL_ID,                // bUnitId
            DEV_NUM_MICREC_SELECTOR_INPINS,   // bNumber of input pins
            { DEV_MICREC_FU_ID },             // baSourceID
            0x00                              // iSelector
        },
#else
#ifdef _DEV_MICREC_MIXER_ADD
        //---------------- For Micrecording Mixer ------------------
        // ============================================
        // Audio Control Mixer Unit Descriptor
        // ===========================================
        {        
            sizeof(AUD_MICREC_MIX_DESC),       // bLength
            AUD_CS_INTERFACE,                  // bDescriptorType
            AUD_SUBTYPE_MIXER,                 // bDescriptorSubtype
            DEV_MICREC_MIXER_ID,               // bTerminalID
            DEV_NUM_MICREC_MIX_INPUT,          // BNrInPins
            DEV_MICREC_FU_ID,                  // baSourceID[1]
            DEV_NUM_MICREC_MIX_OUT_CH,         // bNrChannels
            MAKE_LITTLE_ENDIAN(AUD_LOC_LEFT),  // wChannelConfig,(left,right)
            0x00,                              // iChannelNames
            {DEV_MICREC_MIXER_BMCONTROL_VALUES}, // bmControl
            0x00                               // iMixer
        },
#endif

// Don't use mixer or selector
#endif

        // ============================================
        // Audio Control Output Terminal Descriptor
        // ===========================================
        {       
            sizeof(AUD_OT_DESC),                 // bLength
            AUD_CS_INTERFACE,                    // bDescriptorType
            AUD_SUBTYPE_OT,                      // bDescriptorSubtype
            DEV_MICREC_OT_ID,                    // bTerminalID
            MAKE_LITTLE_ENDIAN(AUD_USB_TYPE_USB_STREAMING), // wTerminalType
            0x00,                                // bAssocTerminal
#ifdef _DEV_MICREC_SELECTOR_ADD
            DEV_MICREC_SEL_ID,                   // bSourceID
#else                        
#ifdef _DEV_MICREC_MIXER_ADD
            DEV_MICREC_MIXER_ID,
#else
            DEV_MICREC_FU_ID,
#endif
#endif
            0x00                                 // iTerminal
        }
#endif //ifdef _MIC_
    },
    ////////////////////////////////////////////////////////////////////////////
    // Speaker streaming interface
    ////////////////////////////////////////////////////////////////////////////
    // ====================================================
    // Speaker Streaming Interface Descriptor: Alternate #0
    // ====================================================
    {        
        sizeof(INTERFACE_DESCRIPTOR),      // bLength
        DESC_TYPE_INTERFACE,               // bDescriptorType
        DEV_SPK_IF_ID,                     // bInterfaceNumber
        DEV_SPK_SETTING_0_BANDWIDTH,       // bAlternateSetting
        0x00,                              // bNumEndpoints
        USB_AUDIO_CLASS_CODE,              // bInterfaceClass
        AUD_SUBCLASS_AUDIOSTREAMING,       // bInterfaceSubClass
        0x00,                              // bInterfaceProtocol
        0x00                               // iInterface
    },
        
    // ====================================================
    // Speaker Streaming Interface Descriptor: Alternate #1
    // ===================================================
    {     
        sizeof(INTERFACE_DESCRIPTOR),      // bLength
        DESC_TYPE_INTERFACE,               // bDescriptorType
        DEV_SPK_IF_ID,                     // bInterfaceNumber
        DEV_SPK_SETTING_FULL_BANDWIDTH,    // bAlternateSetting
        0x01,                              // bNumEndpoints
        USB_AUDIO_CLASS_CODE,              // bInterfaceClass
        AUD_SUBCLASS_AUDIOSTREAMING,       // bInterfaceSubClass
        0x00,                              // bInterfaceProtocol
        0x00                               // iInterface
    },

    // =====================================================
    // Speaker Streaming Class-specific Interface Descriptor
    // ====================================================
    {       
        sizeof(AUD_AS_INTERFACE_DESC),     // bLength
        AUD_CS_INTERFACE,                  // bDescriptorType
        AUD_AS_SUBTYPE_GENERAL,            // bDescriptorSubtype
        DEV_HOST_IT_ID,                    // bTerminalLink
        1,                                 // bDelay:1 frame
        MAKE_LITTLE_ENDIAN(AUD_TYPE1_PCM)  // wFormatTag (PCM)
    },
        
    // ================================================
    // Speaker Streaming Data Type 1 Format Descriptor
    // ===============================================
    {
        {
			sizeof(DATA_TYPE1_SPK_FREQ),        // bLength
	        AUD_CS_INTERFACE,                   // bDescriptorType
	        AUD_AS_SUBTYPE_FORMAT_TYPE,	 	    // bDescriptorSubtype
	        AUD_FORMAT_TYPE_I,	 	 	 	    // bFormatType
	        DEV_NUM_USB_SPK_CHANNELS,           // bNrChannels
	        DEV_SPK_SUBFRAME_SIZE,              // bSubFrameSize (bytes)
	        16,                                 // bBitResolution
	        DEV_NUM_SPK_FREQUENCIES             // # of Freq
		},
#ifdef	_FREQ_SPK_RANGE_	 	 	 	        // bSamFreqType,1 fixed freq
        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_8) },   	// iSamFreq,8 KHZ
        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_48) }    // iSamFreq,48 KHZ				
#else
        {
//	        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_8 )},	// iSamFreq,8 KHZ
//	        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_11025 )},// iSamFreq,11.025 KHZ
//	        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_2205 )},	// iSamFreq,22.05 KHZ
            { SAMPLE_FREQ_HZ(SAMPLING_FREQ_441 )},	// iSamFreq,44.1 KHZ
//            { SAMPLE_FREQ_HZ(SAMPLING_FREQ_48 )}	// iSamFreq,48 KHZ
	 	}	
#endif
    },

    // ================================================
    // Speaker Streaming Endpoint Descriptor
    // ===============================================
    {
        sizeof(AUD_STD_AS_ISO_EP_DESC),       // bLength
        DESC_TYPE_ENDPOINT,                   // bDescriptorType
        DEV_SPK_EP_ID,                        // bEndpointAddress
        AUD_EP_ISO|AUD_EP_ISO_ADAPT,          // bmAttributes,adaptive
        MAKE_LITTLE_ENDIAN(DEV_SPK_MAX_PKT),  // wMaxPacketSize
        0x01,                                 // bInterval
        0x00,                                 // bRefresh
        0x00                                  // bSynchAddress
    },

    // ================================================================
    // Speaker Streaming Class-Specific Audio Data Endpoint Descriptor
    // ================================================================
    {
        sizeof(AUD_CS_AS_ISO_EP_DESC),  // bLength
        AUD_CS_ENDPOINT,                // bDescriptorType
        AUD_CS_EP_DESC_EP_GENERAL,      // bDescriptorSubtype
        AUD_CS_AS_EP_ISO_SAMPLE_FREQ,   // bmAttributes
        0,                              // bLockDelayUnits
        MAKE_LITTLE_ENDIAN(0)           // wLockDelay
    }

#ifdef _MIC_
	, //may be last
    ////////////////////////////////////////////////////////////////////////////
    // Mic streaming interface
    ////////////////////////////////////////////////////////////////////////////
    // ======================================================
    // Mic Streaming Standard Interface: Alternate Setting 0
    // ======================================================
    {
        sizeof(INTERFACE_DESCRIPTOR),     // bLength
        DESC_TYPE_INTERFACE,              // bDescriptorType
        DEV_MIC_IF_ID,                    // bInterfaceNumber
        DEV_MIC_SETTING_0_BANDWIDTH,      // bAlternateSetting
        0,                                // bNumEndpoints
        USB_AUDIO_CLASS_CODE,             // bInterfaceClass
        AUD_SUBCLASS_AUDIOSTREAMING,      // bInterfaceSubClass
        0,                                // bInterfaceProtocol
        0                                 // iInterface
    },

    // ======================================================
    // Mic Streaming Standard Interface: Alternate Setting 1
    // ======================================================
    {
        sizeof(INTERFACE_DESCRIPTOR),     // bLength
        DESC_TYPE_INTERFACE,              // bDescriptorType
        DEV_MIC_IF_ID,                    // bInterfaceNumber
        DEV_MIC_SETTING_FULL_BANDWIDTH,   // bAlternateSetting
        1,                                // bNumEndpoints
        USB_AUDIO_CLASS_CODE,             // bInterfaceClass
        AUD_SUBCLASS_AUDIOSTREAMING,      // bInterfaceSubClass
        0,                                // bInterfaceProtocol
        0                                 // iInterface
    },

    // ==================================================
    // Mic Streaming Class-specific Interface Descriptor
    // ==================================================
    {
        sizeof(AUD_AS_INTERFACE_DESC),    // bLength
        AUD_CS_INTERFACE,                 // bDescriptorType
        AUD_AS_SUBTYPE_GENERAL,           // bDescriptorSubtype
        DEV_MICREC_OT_ID,                 // bTerminalLink,output terminal
        1,                                // bDelay 1 frame
        MAKE_LITTLE_ENDIAN(AUD_TYPE1_PCM) // wFormat PCM
    },

    // ===========================================
    //  Mic Streaming Format Type Descriptor
    // ===========================================
    {
        {
            sizeof(DATA_TYPE1_MIC_FREQ),     // bLength
	        AUD_CS_INTERFACE,                // bDescriptorType
	        AUD_AS_SUBTYPE_FORMAT_TYPE,	 	 // bDescriptorSubtype
	        AUD_FORMAT_TYPE_I,	 	 	 	 // bFormatType
	        DEV_NUM_MIC_CHANNELS,            // bNrChannels
	        DEV_SPK_SUBFRAME_SIZE,           // bSubFrameSize (bytes)
	        16,                              // bBitResolution
	        DEV_NUM_MIC_FREQUENCIES			 // bSamFreqType
		},
#ifdef	_FREQ_MIC_RANGE_	 	 	 	     // 1 fixed frequency
        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_8) }, // iSamFreq,8 KHZ
        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_48) } // iSamFreq,48 KHZ		
#else
		{
//	        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_8 )},	 // iSamFreq,8 KHZ
//	        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_11025 )}, // iSamFreq,11.025 KHZ
//	        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_2205 )},	 // iSamFreq,22.05 KHZ
	        { SAMPLE_FREQ_HZ(SAMPLING_FREQ_441 )},	 // iSamFreq,44.1 KHZ
//            { SAMPLE_FREQ_HZ(SAMPLING_FREQ_48 )}	 // iSamFreq,48 KHZ
	 	}	
#endif
    },

    // ================================================
    // Mic Streaming Endpoint Descriptor
    // ===============================================
    {
        sizeof(AUD_STD_AS_ISO_EP_DESC),       // bLength
        DESC_TYPE_ENDPOINT,                   // bDescriptorType
        DEV_MIC_EP_ID,                        // bEndpointAddress
        AUD_EP_ISO|AUD_EP_ISO_SYNC,           // bmAttributes,sync
        MAKE_LITTLE_ENDIAN(DEV_MIC_MAX_PKT),  // wMaxPacketSize
        1,                                    // bInterval
        0,                                    // bRefresh
        0                                     // bSynchAddress
    },


    // ============================================================
    // Mic Streaming Class-Specific Audio Data Endpoint Descriptor
    // ============================================================
    {
        sizeof(AUD_CS_AS_ISO_EP_DESC),        // bLength
        AUD_CS_ENDPOINT,                      // bDescriptorType
        AUD_CS_EP_DESC_EP_GENERAL,            // bDescriptorSubtype
        AUD_CS_AS_EP_ISO_SAMPLE_FREQ,         // bmAttributes
        0,                                    // bLockDelayUnits
        MAKE_LITTLE_ENDIAN(0)                 // wLockDelay
    }
#endif //ifdef _MIC_

#ifdef _HID_
	,//may be last
    
    ////////////////////////////////////////////////////////////////////////////
    // HID interface
    ////////////////////////////////////////////////////////////////////////////
    // ================================================
    // HID Interface Descriptor
    // ===============================================
   {
        sizeof(INTERFACE_DESCRIPTOR),  // Length = 9
        DESC_TYPE_INTERFACE,           // interface descriptor type
        DEV_HID_IF_ID,                 // bInterfaceNumber
        0,                             // bAltenatersetting 0
        2,                             // bNumberEndpoints
        USB_HID_CLASS_CODE,            // bInterfaceClass: HID
        0,                             // bInterfaceSubclass
        0,                             // binterfaceProtocol
        0                              // iInterface
    },

    // ================================================
    // HID (Class) Descriptor
    // ================================================
    {
        sizeof(HID_DESC),                           // Length = 9
        DESC_TYPE_HID,                              // HID descriptor type
        MAKE_LITTLE_ENDIAN(HID_SPEC_VERSION),       // bcdHID: 01.00
        0x00,                                       // bCountryCode
        DEV_NUM_HID_DESCRIPTORS,                    // bNumDescriptors
        DESC_TYPE_HID_REPORT,                       // bDescriptorType
        MAKE_LITTLE_ENDIAN(sizeof(DevHidReportDesc))// bDescriptorLength
    },

    // ================================================
    // HID Endpoint descriptor
    // ================================================
    {
        sizeof(ENDPOINT_DESCRIPTOR),         // Length = 7 
        DESC_TYPE_ENDPOINT,                  // endpoint descriptor type
        DEV_HID_EP_ID,                       // bEndpointAddress
        EP_DESC_ATTR_TYPE_INT,               // Transfer Type
        MAKE_LITTLE_ENDIAN(DEV_HID_MAX_PKT), // wMaxpacketSize
        DEV_HID_PRATE                        // bInterval
    },

    // ================================================
    // HID Output Endpoint descriptor
    // ================================================
    {
        sizeof(ENDPOINT_DESCRIPTOR),         // Length = 7 
        DESC_TYPE_ENDPOINT,                  // endpoint descriptor type
        DEV_HIDO_EP_ID,                      // bEndpointAddress
        EP_DESC_ATTR_TYPE_INT,               // Transfer Type
        MAKE_LITTLE_ENDIAN(DEV_HID_MAX_PKT), // wMaxpacketSize
        DEV_HID_PRATE                        // bInterval
    }

#endif

#ifdef _DFU_
    , //may be last
    // ============================================ 
    // Standard DFU interface
    // ============================================
        {
    sizeof(INTERFACE_DESCRIPTOR), 	// bLength
    DESC_TYPE_INTERFACE,			// bDescriptorType
    DEV_DFU_IF_ID,              	// bInterfaceNumber
    0x00,                       	// bAlternateSetting
    0x00,                       	// bNumEndpoints
    DFU_BCLASSCODE,					// bInterfaceClass
    DFU_BSUBCLASSCODE,				// bInterfaceSubClass
    0x00,                       	// bInterfaceProtocol
    0x00                        	// iInterface
        },

    // ============================================ 
    // DFU Functional Descriptor
    // ============================================
        {
    sizeof(DFU_FUNCTIONAL_DESC),			// bLength
    DFU_FUNC_DESC_TYPE,						// bDescriptorType
    DFU_MANIFESTTOL_BIT | DFU_UPLOAD_CAP_BIT | DFU_DNLOAD_CAP_BIT,// bmAttributes
    MAKE_LITTLE_ENDIAN(DFU_DETACH_TIMEOUT),	// wDetachTimeOut
    MAKE_LITTLE_ENDIAN(DFU_BMAXPACKETSIZE0)	// wTransferSize
	}
#endif

};

#ifdef _DFU_
// Application DFU mode device/configuration descriptors
DEVICE_DESCRIPTOR code DfuDevDesc = 
{
    sizeof(DEVICE_DESCRIPTOR),		// blength
    DESC_TYPE_DEVICE,          		// Device descriptor type
    MAKE_LITTLE_ENDIAN(0x0100),		// USB spec 1.00, LSB
    DFU_BCLASSCODE,            		// Application specific Class code
    DFU_BSUBCLASSCODE,         		// DFU code
    0x00,                      		// No class specific protocol
    0x40,							// max EP0 packet size
    MAKE_LITTLE_ENDIAN(DFU_VENDOR_ID),
    MAKE_LITTLE_ENDIAN(DFU_PRODUCT_ID),	
    MAKE_LITTLE_ENDIAN(DFU_BCDDEVICE),	
    0x01,					  		// iManufactirer
    0x02,					  		// iProduct
    0x00,					  		// iSerialNumber
    0x01					  		// bNumConfigurations
};

DFU_CONFIG_DESCRIPTOR_DATA code AppDfuConfigDesc = 
{
	{
    sizeof(CONFIG_DESCRIPTOR),      // bLength
    DESC_TYPE_CONFIG,				// bDescriptorType
    MAKE_LITTLE_ENDIAN(sizeof(DFU_CONFIG_DESCRIPTOR_DATA)), // wTotalLength
    DFU_NUM_IF,						// bNumInterfaces
    DFU_CONFIG_VALUE,				// bConfigurationValue
    0x00,							// iConfiguration
    CFG_DESC_ATTR_SELF_POWERED,	
    DEV_CONFIG_MAX_POWER 			// MaxPower 100 mA
	},

    //
	// DFU mode interface descriptor
	//
	{
    sizeof(INTERFACE_DESCRIPTOR), 	// bLength
    DESC_TYPE_INTERFACE,			// bDescriptorType
    DFU_IFNUMBER,//DEV_DFU_IF_ID,                	// bInterfaceNumber
    DFU_IF_SETTING,              	// bAlternateSetting
    0x00,                       	// bNumEndpoints
    DFU_BCLASSCODE,                 // bInterfaceClass
    DFU_BSUBCLASSCODE,				// bInterfaceSubClass
    0x00,                         	// bInterfaceProtocol
    DFU_IIF_STR_ID                  // iInterface
	},

	//
    // DFU Functional Descriptor
	//
	{
	sizeof(DFU_FUNCTIONAL_DESC),            // bLength
    DFU_FUNC_DESC_TYPE,				        // bDescriptorType
    DFU_MANIFESTTOL_BIT | DFU_UPLOAD_CAP_BIT | DFU_DNLOAD_CAP_BIT,// bmAttributes
    MAKE_LITTLE_ENDIAN(DFU_DETACH_TIMEOUT),	// wDetachTimeOut
    MAKE_LITTLE_ENDIAN(DFU_BMAXPACKETSIZE0)	// wTransferSize
	}
        
};
#endif

LANGID_DESC code DevLangIdDesc =
{
    sizeof(LANGID_DESC),
    DESC_TYPE_STRING,
    MAKE_LITTLE_ENDIAN(LANGID_US_ENGLISH)
};

// 
// Unicode product name descriptor
//
PRODUCT_DESC code DevProductStringDesc =
{
    sizeof(PRODUCT_DESC),
    DESC_TYPE_STRING,
    UNICODE_PRODUCT_NAME
};

//
// Unicode vendor name descriptor
//
VENDOR_DESC code DevCompanyStringDesc = 
{
    sizeof(VENDOR_DESC),
    DESC_TYPE_STRING,
    UNICODE_VENDOR_NAME
};

//
// FW version
//
FW_VERSION_DESC code DevFwVersion =
{
    sizeof(FW_VERSION_DESC),
    DESC_TYPE_STRING,
    UNICODE_FW_VERSION
};

#ifdef _HID_
// 
// GetHidReportDescSize - Returns the hid report descriptor size.
//
WORD GetHidReportDescSize()
{
    return sizeof(DevHidReportDesc);
}
#endif

#ifdef _DFU_
// Poll time table handles the appIDLE and appDDETACH only
byte code DevDfuPollTimeTable[2 * sizeof(DFU_POLLSTATE_STRUCT)] = {
    0x00, 0x01, 0xFF, DFU_STATE_appIDLE,
    0x00, 0x01, 0xFF, DFU_STATE_appDETACH
};
#endif

