//
// USBAPP.H - This header contains defines which are used in the 3200 baseline
// which are not contained in the TAS1020 ROM common headers.
//
#ifndef _USBAPP_H
#define _USBAPP_H

/////////////////////////////////////////////////////////////////////////////////
// REG_STC1.H
/////////////////////////////////////////////////////////////////////////////////
// Global control register values
#define CODEC_PORT_ENABLE       0x01
#define LOW_POWER_DISABLE       0x04
#define XINT_ENABLE             0x40
#define P1_PULLUP_DISABLE		0x20

#define EP_BASE_ADDR(addr)      (byte)((((WORD)addr - STC_BUFFER_BASE_ADDR)) / 8)
#define EP_BUFFER_SIZE(size)    ((size) / 8 + ((size) % 8 ? 1 : 0))

/////////////////////////////////////////////////////////////////////////////////
// TYPES.H
/////////////////////////////////////////////////////////////////////////////////
#ifndef NULL
#define NULL    ((void *)0)
#endif

#define LANGID_US_ENGLISH   0x409
//
// Language ID descriptor.
//
#define TYPEDEF_LANGID_DESC(typedef_name, number_of_langids)    \
typedef struct typedef_name##_tag           \
{                                           \
    byte bLength;                           \
    byte bDescriptorType;                   \
    word wLANGID[number_of_langids];        \
} typedef_name

//
// Unicode string descriptor
//
#define TYPEDEF_UNICODE_STRING_DESC(typedef_name, str_len)  \
typedef struct typedef_name##_tag           \
{                                           \
    byte bLength;                           \
    byte bDescriptorType;                   \
    byte bString[str_len];                  \
} typedef_name

/////////////////////////////////////////////////////////////////////////////////
// USBAUDIO.H
/////////////////////////////////////////////////////////////////////////////////
//
// USB audio spec release 1.0
//
#define AUD_BCD_REL_1_0      0x0100

//
// Determines the minimum number of bytes required to hold the specified number
// of bits.
//
#define BITMAP_BYTES(n)     (((n) / 8) + (((n) % 8) ? 1 : 0))


// Audio class type
#define USB_AUDIO_CLASS_CODE            0x01

// Audio subclass types
#define AUD_SUBCLASS_UNDEFINED          0x00
#define AUD_SUBCLASS_AUDIOCONTROL       0x01
#define AUD_SUBCLASS_AUDIOSTREAMING     0x02
#define AUD_SUBCLASS_MIDISTREAMING      0x03

//
// Audio class specific interface descriptor types
//
#define AUD_CS_UNDEFINED        0x20
#define AUD_CS_DEVICE           0x21
#define AUD_CS_CONFIGURATION    0x22
#define AUD_CS_STRING           0x23
#define AUD_CS_INTERFACE        0x24
#define AUD_CS_ENDPOINT         0x25

//
// Audio class
//

//
// Audio class-specific AS interface desc subtype.
//
#define AUD_AS_SUBTYPE_UNDEFINED        0x00
#define AUD_AS_SUBTYPE_GENERAL          0x01
#define AUD_AS_SUBTYPE_FORMAT_TYPE      0x02
#define AUD_AS_SUBTYPE_FORMAT_SPECIFIC  0x03

//
// USB terminal types
//
#define AUD_USB_TYPE_UNDEFINED      0x0100
#define AUD_USB_TYPE_USB_STREAMING  0x0101
#define AUD_USB_TYPE_VENDOR         0x01FF

//
// Input terminal type
//
#define AUD_IT_TYPE_MICROPHONE  0x201


//
// Output terminal type
//
#define AUD_OT_TYPE_SPEAKER     0x301
#define AUD_OT_TYPE_HEADPHONE   0x302


//
// Processing unit process types
//
#define AUD_PROCESS_UNDEFINED           0x00
#define AUD_UP_DOWN_MIX_PROCESS         0x01
#define AUD_DOLBY_PROLOGIC_PROCESS      0x02
#define AUD_3D_STREREO_EXTENDER_PROCESS 0x03
#define AUD_REVERBERATION_PROCESS       0x04

//
// Up/down mixer controls
//
#define AUD_UP_DOWN_MIX_ENABLE          0x01
#define AUD_UP_DOWN_MIX_MODE_SELECT     0x02

//
// Channel spatial locations
//
#define AUD_LOC_MASTER          0x0000
#define AUD_LOC_LEFT            0x0001
#define AUD_LOC_RIGHT           0x0002
#define AUD_LOC_CENTER          0x0004
#define AUD_LOC_LFE             0x0008
#define AUD_LOC_LEFT_SURROUND   0x0010
#define AUD_LOC_RIGHT_SURROUND  0x0020
#define AUD_LOC_LEFT_CENTER     0x0040
#define AUD_LOC_RIGHT_CENTER    0x0080
#define AUD_LOC_SURROUND        0x0100
#define AUD_LOC_SIDE_LEFT       0x0200
#define AUD_LOC_SIDE_RIGHT      0x0400
#define AUD_LOC_TOP             0x0800


//
// Format types
//
#define AUD_FORMAT_TYPE_UNDEFINED   0x00
#define AUD_FORMAT_TYPE_I           0x01
#define AUD_FORMAT_TYPE_II          0x02
#define AUD_FORMAT_TYPE_III         0x03

//
// TYPE I data formats
//
#define AUD_TYPE1_UNDEFINED         0x0000
#define AUD_TYPE1_PCM               0x0001
#define AUD_TYPE1_PCM8              0x0002
#define AUD_TYPE1_IEEE_FLOAT        0x0003
#define AUD_TYPE1_ALAW              0x0004
#define AUD_TYPE1_MULAW             0x0005

//
// Sampling frequency
//
#define SAMPLE_FREQ_HZ(n)           (BYTE)(0x000000FF & (n)), \
                                    (BYTE)((0x0000FF00 & (n)) >> 8), \
                                    (BYTE)((0x00FF0000 & (n)) >> 16) 
//
// TYPE II data formats
//
#define AUD_TYPE2_UNDEFINED         0x1000
#define AUD_TYPE2_MPEG              0x1001
#define AUD_TYPE2_AC3               0x1002

//
// TYPE III data formats
//
#define AUD_TYPE3_UNDEFINED         0x2000
#define AUD_TYPE3_AC3               0x2001
#define AUD_TYPE3_MPEG1_LAYER1      0x2002
#define AUD_TYPE3_MPEG1_LAYER2_3    0x2003
#define AUD_TYPE3_MPEG2_EXT         0x2004
#define AUD_TYPE3_MPEG2_LAYER1_LS   0x2005



// specific for the application
#define AUD_STREAMING_INTERFACE_NR	0x2
#define AUD_NUMBER_CHANNELS		    0x2

// ===== Audio Control Interface ===================================*/

// Class Specific AC Descriptor

#define TYPEDEF_AUD_AC_INTERFACE_DESC(typedef_name, number_of_ifs) \
typedef struct typedef_name##_tag {     \                                                        
	BYTE bLength;                       \                                                                   
	BYTE bDescriptorType;		        \                      
	BYTE bDescriptorSubtype;	        \                  
    WORD bcdADC;                        \
	WORD wTotalLength;		            \ 
	BYTE bInCollection;			        \
	BYTE baInterfaceNr[number_of_ifs];  \ 
} typedef_name






// Mixer Unit typedef
#define TYPEDEF_MIXER_UNIT_DESC(typedef_name, num_in_pins, num_in_ch, num_out_ch ) \
typedef struct typedef_name##_tag       \
{                                       \
    BYTE bLength;                       \ 
    BYTE bDescriptorType;               \
    BYTE bDescriptorSubtype;            \ 
    BYTE bUnitID;                       \
    BYTE bNrInPins;                     \
    BYTE baSourceID[num_in_pins];       \ 
    BYTE bNrChannels;                   \
    WORD wChannelConfig;                \
    BYTE iChannelNames;                 \
    BYTE bmControls[BITMAP_BYTES(num_in_ch * num_out_ch)];  \ 
    BYTE iMixer;                        \   
}   typedef_name


// Selector Unit typedef
#define TYPEDEF_SELECTOR_UNIT_DESC(typedef_name, num_input) \
typedef struct typedef_name##_tag       \
{                                       \
    BYTE bLength;                       \
    BYTE bDescriptorType;               \
    BYTE bDescriptorSubtype;            \
    BYTE bUnitID;                       \ 
    BYTE bNrInPins;                     \
    BYTE baSourceID[num_input];         \   
    BYTE iSelector;                     \
}   typedef_name


// Feature Unit typedef
#define TYPEDEF_FEATURE_UNIT_DESC(typedef_name, ch_count, bmaControlSize) \
typedef struct typedef_name##_tag       \
{                                       \
    BYTE bLength;                       \ 
    BYTE bDescriptorType;               \ 
    BYTE bDescriptorSubtype;            \ 
    BYTE bUnitID;                       \
    BYTE bSourceID;                     \
    BYTE bControlSize;                  \
    BYTE bmaControls[(ch_count + 1) * bmaControlSize]; \
    BYTE iFeature;                      \
} typedef_name


// Feature unit bitmap control bits (bmaControls values)
//

#define AUD_FU_BM_MUTE          0x01
#define AUD_FU_BM_VOLUME        0x02
#define AUD_FU_BM_BASS          0x04
#define AUD_FU_BM_MID           0x08
#define AUD_FU_BM_TREBLE        0x10
#define AUD_FU_BM_GEQ           0x20
#define AUD_FU_BM_AUTO_GAIN     0x40
#define AUD_FU_BM_DELAY         0x80
#define AUD_FU_BM_BASS_BOOST    0x0100
#define AUD_FU_BM_LOUDNESS      0x0200



// Up/down mixer Unit typedef
#define TYPEDEF_UP_DOWN_MIXER_UNIT_DESC(typedef_name, num_modes) \
typedef struct typedef_name##_tag       \
{                                       \
    BYTE bLength;                       \
    BYTE bDescriptorType;               \
    BYTE bDescriptorSubtype;            \
    BYTE bUnitID;                       \
    WORD wProcessType;                  \
    BYTE bNrInPins;                     \
    BYTE baSourceID;                    \
    BYTE bNrChannels;                   \
    WORD wChannelConfig;                \
    BYTE iChannelNames;                 \
    BYTE bControlSize;                  \
    BYTE bmControls;                    \
    BYTE iProcessing;                   \
    BYTE bNrModes;                      \
    WORD waModes[num_modes];            \
} typedef_name


/* ===== Audio Streaming Interface ================================*/
// Class-Specific AS Interface Descriptor


//
// Std. AS Isochronous audio data endpoint descriptor.
//
typedef struct AUD_STD_AS_ISO_EP_DESC_tag
{
    BYTE bLength;
    BYTE bDescriptortype;                   // ENDPOINT descriptor
    BYTE bEndpointAddress;                  // Endpoint address
    BYTE bmAttributes;                      // endpoint attributes
    WORD wMaxPacketSize;                    // Max packet size.
    BYTE bInterval;                         // Polling interval ( = 1)
    BYTE bRefresh;                          // Rest to 0.
    BYTE bSynchAddress;                     // Ep used for synchronous comm.
} AUD_STD_AS_ISO_EP_DESC;

// Iso ep attributes
#define AUD_EP_ISO                          0x01    // iso ep
#define AUD_EP_ISO_ASYNC                    0x04    // Iso/asynchronous
#define AUD_EP_ISO_ADAPT                    0x08    // Iso/Adaptive
#define AUD_EP_ISO_SYNC                     0x0C    // Iso/synchronous

// Class-Specific AS Isochronous Audio Data Endpoint Descriptor
typedef struct _AUD_CS_AS_ISO_EP_DESC {
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
} AUD_CS_AS_ISO_EP_DESC;


// 
// Class specific ISO ep attributes.
//
#define AUD_CS_AS_EP_ISO_SAMPLE_FREQ            0x01
#define AUD_CS_AS_EP_ISO_PITCH                  0x02
#define AUD_CS_AS_EP_ISO_MAX_PKT                0x80

//
// Audio class specific ep descriptor subtypes.
//
#define AUD_CS_EP_DESC_SUBTYPE_UNDEFINED        0x00
#define AUD_CS_EP_DESC_EP_GENERAL               0x01

//
// Data type I header.
//
typedef struct DATA_TYPE1_HDR_tag
{
    BYTE bLength;
    BYTE bDescriptorType;       // CS_INTERFACE descriptor type
    BYTE bDescriptorSubtype;    // FORMAT_TYPE descriptor subtype
    BYTE bFormatType;           // FORMAT_TYPE_1
    BYTE bNrChannels;           // Number of physical channels
    BYTE bSubframeSize;         // Number of bytes in audio subframe(1,2,3 or 4).
    BYTE bBitResolution;        // Number of effectively used bits.
    BYTE bSamFrequencyType;     // How sampling frequency can be programmed.
}   DATA_TYPE1_HDR;

#define TYPEDEF_DATA_TYPE1_DISCRETE(typedef_name, num_frequencies) \
typedef struct typedef_name##_tag                       \
{                                                       \
    DATA_TYPE1_HDR  Header;                             \
    BYTE tSamFreq[num_frequencies][3];                  \
} typedef_name
    


typedef struct DATA_TYPE1_FREQ_RANGE_tag                       
{                                                       
    DATA_TYPE1_HDR  Header;                             
    BYTE tLowerSamFreq[3];                                 
    BYTE tUpperSamFreq[3];                                 
} DATA_TYPE1_FREQ_RANGE;
 




/////////////////////////////////////////////////////////////////////////////////
// USBDFU.H
/////////////////////////////////////////////////////////////////////////////////

#define DFU_VENDOR_ID       0xFFFF
#define DFU_PRODUCT_ID      0xFFFE
#define DFU_BCDDEVICE       0x0100


//
// DFU functional descriptor
//
typedef struct DFU_FUNCTIONAL_DESCtag
{
    byte bLength;                           // Size of descriptor
    byte bDescriptorType;                   // Descriptor type
    byte bmAttributes;                      // DFU attributes
                                            // bit 7..3 - reserved
                                            // bit 2 - able to communicate via
                                            // USB after manifestation phase.
                                            // 0 => no, must see reset
                                            // 1 => yes
    WORD wDetachTimeOut;                    // # of msec device will wait.
    WORD wTransferSize;                     // Max # of bytes/control write.
} DFU_FUNCTIONAL_DESC;

/////////////////////////////////////////////////////////////////////////////////
// USBHID.H
/////////////////////////////////////////////////////////////////////////////////

#define HID_SPEC_VERSION    0x0100      // v 1.0 spec.

#define HID_REPORT_TYPE_INPUT   0x01
#define HID_REPORT_TYPE_OUTPUT  0x02
#define HID_REPORT_TYPE_FEATURE 0x03

#define USB_HID_CLASS_CODE      0x03

//
// HID descriptor.
//
#define TYPEDEF_HID_DESC(typedef_name, num_descriptors) \
    typedef struct typedef_name##_tag   \
{                                   \
    BYTE bLength;                   \
    BYTE bDescriptorType;           \
    WORD bcdHID;                    \
    BYTE bCountryCode;              \
    BYTE bNumDescriptors;           \
    struct                          \
    {                               \
    BYTE bDescriptorType;       \
    WORD wDescriptorLength;     \
    } Descriptor[num_descriptors];  \
} typedef_name

#endif
