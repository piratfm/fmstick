/*
 * Low-Level API for FmStick transmitter
 * $Id$
 * (part of FmStick).
 *
 * (C) 2011 Serge <piratfm@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define FMTX
#include <string.h>
#ifdef WIN32
    #include <malloc.h>
#else
    #include <stdlib.h>
#endif
#include "hidapi.h"
#include "fmtx.h"
#include "bitstream.h"
#include "logging.h"
#include "commanddefs.h"
#include "propertydefs.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define FALSE 0
#define TRUE 1
typedef int BOOL;

//HID\VID_0451&PID_2100&REV_0101&MI_02
const unsigned short _usVID=0x0451;  /*!< USB vendor ID. */
const unsigned short _usPID=0x2100;  /*!< USB product ID. */

/* static vars */
EXTERN FMTX_MODE_ENUM fmtxCmdStatus;
EXTERN FMTX_SPECIAL_ENUM fmtxCmdSpecial;
eeprom_data    fmtxCmdEEPROM;


static const char *si471x_requests[] = {
    "NONE",
    "CPU ID",
    "Frontend Reset",
    "Frontend Access",
    "Get Property",
    "Set Property",
    "Frontend Power Status",
    "Frontend Power Up",
    "Frontend Power Down",
    "Frontend Enable Audio",
    "Frontend Disable Audio",
    "EEPROM Read",
    "EEPROM Write",
    "unknown"
};
#define Si471xRequestStr(x) (x < RequestUnknown ? si471x_requests[x] : si471x_requests[RequestUnknown])


static const char *si471x_statuses[] = {
    "OK",
    "I2C Timeout",
    "I2C Communication Error",
    "Bad Argument",
    "No CTS!",
    "unknown"
};
#define Si471xStatusStr(x) (x < SI4711_ERROR_UNKNOWN ? si471x_statuses[x] : si471x_statuses[SI4711_ERROR_UNKNOWN])

static int16_t getDefaultProp(int16_t i16Prop);




/** \brief Open FMTX device.
  * \return Pointer to the FMTX HID device or NULL if none found
  *
  * This function looks for FMTX devices connected to the computer and
  * opens the first one found.
  */
static hid_device *fmtxOpen(void)
{
    struct hid_device_info *phdi=NULL;
    hid_device *phd=NULL;
    char *pszPath=NULL;
#if 0
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x0, 0x0);
    printf("Devices %p\n", devs);
    cur_dev = devs;
    while (cur_dev) {
        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        printf("\n");
        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printf("  Product:      %ls\n", cur_dev->product_string);
        printf("  Release:      %hx\n", cur_dev->release_number);
        printf("  Interface:    %d\n",  cur_dev->interface_number);
        printf("\n");
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
#endif
    phdi=hid_enumerate(_usVID,_usPID);
    if (phdi==NULL)
        return NULL;

    pszPath=strdup(phdi->path);
    if (pszPath==NULL)
        return NULL;

    hid_free_enumeration(phdi);
    phdi=NULL;

    if ((phd=hid_open_path(pszPath)) == NULL)
    {
        free(pszPath);
        pszPath=NULL;

        return NULL;
    }

    free(pszPath);
    pszPath=NULL;

    return phd;
}


/** \brief Close FMTX HID device. */
static void fmtxClose(hid_device *phd)
{
    hid_close(phd);
}


EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppDeviceFound(void)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    hid_device *phd=NULL;
    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;
    fmtxClose(phd);
    return FMTX_MODE_OK;
}

/** \brief Get FMTX mode.
  * \return The current FMTX mode.
  * \sa FMTX_MODE_ENUM
  */
EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppFeGetMode(int *transmitting)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711PowerStatus;
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_NONE;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestSi4711PowerStatus|RequestDone)) {
        logwrite(LOG_DEBUG, "FMTX: %s FePoweredUp: %d, TxPoweredUp: %d", __FUNCTION__,
                 aucBufIn[2], aucBufIn[3]);
        if(aucBufIn[2]) {
            *transmitting = aucBufIn[3];
            return FMTX_MODE_POWER_UP;
        } else
            return FMTX_MODE_POWER_DOWN;
    }
    return FMTX_MODE_NONE;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppFeUp(void)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711PowerUp;
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_TRANSMITTING;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestSi4711PowerUp|RequestDone))
        return FMTX_MODE_POWER_UP;
    return FMTX_MODE_NONE;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppFeDown(void)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711PowerDown;
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_NONE;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestSi4711PowerDown|RequestDone))
        return FMTX_MODE_POWER_DOWN;
    return FMTX_MODE_NONE;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppFeReset(void)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711Reset;
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_NONE;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestSi4711Reset|RequestDone))
        return FMTX_MODE_POWER_DOWN;
    return FMTX_MODE_NONE;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppIdString(char *cpuid, char *rev)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];
    memset(cpuid, 0x00, 32);
    memset(rev, 0x00, 32);
    uint16_t rev_major = 0;
    char *str = NULL;
    int str_len = 0;

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);



    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestCpuId;
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_NONE;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestCpuId|RequestDone)) {
        //bReportOut:
        //  +-----------+-----------+--------------+---------+-------------+---------+---------------+
        //  | PCCommand | PCRequest | REV_MAJOR[2] | STR_LEN | REV_MINOR[] | STR_LEN | DEVICE_NAME[] |
        //  +-----------+-----------+--------------+---------+-------------+---------+---------------+
        //print_bytes(LOG_ERROR, (char *) aucBufIn, 32);
        rev_major = aucBufIn[2] << 8 | aucBufIn[3];

        str_len = aucBufIn[4];
        str = (char *) &aucBufIn[5];
        print_bytes(LOG_DUMP, (char *) str, str_len);

        if(str_len > 9) {
            if(!strncmp(&str[str_len-3], " $", 2))
                str[str_len-3] = '\0';
            if(!strncmp(str, "$Rev: ", 6))
                str+=6;
        }
        snprintf(rev, 32, "%d.%s", rev_major, str_len < 24 ? str : "");

        str = (char *) &aucBufIn[4+str_len+1+1];
        str_len = aucBufIn[4+str_len+1];
        print_bytes(LOG_DUMP, (char *) str, str_len);
        snprintf(cpuid, 32, "%s", str_len < 24 ? str : "");
        return FMTX_MODE_OK;
    }
    return FMTX_MODE_NONE;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppEnableAudio(void)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711AudioEnable;
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_NONE;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestSi4711AudioEnable|RequestDone)) {
        //errcode 1=sr, 2=fmt
        if(aucBufIn[2]!=SI4711_OK){
            logwrite(LOG_ERROR, "USB: Error in %s", (aucBufIn[2]==1 ? "SR" : (aucBufIn[2]==3 ? "FMT" : "UNK")));
            return FMTX_MODE_NONE;
        }
        return FMTX_MODE_OK;
    }
    return FMTX_MODE_NONE;
}


EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppDisableAudio(void)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711AudioDisable;
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_NONE;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestSi4711AudioDisable|RequestDone)) {
        //errcode 1=sr, 2=fmt
        if(aucBufIn[2]!=SI4711_OK){
            logwrite(LOG_ERROR, "USB: Error in %s", (aucBufIn[2]==1 ? "SR" : (aucBufIn[2]==3 ? "FMT" : "UNK")));
            return FMTX_MODE_NONE;
        }
        return FMTX_MODE_OK;
    }
    return FMTX_MODE_NONE;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppAsqStatus(uint8_t *flags, int8_t *level)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];
    *flags = 0x00;
    *level = -70;
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711AsqStatus;
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_POWER_DOWN;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestSi4711AsqStatus|RequestDone)) {
        //errcode 1=sr, 2=fmt
        if(aucBufIn[2]!=SI4711_OK){
            logwrite(LOG_ERROR, "USB: Error in %s", (aucBufIn[2]==1 ? "SR" : (aucBufIn[2]==3 ? "FMT" : "UNK")));
            return FMTX_MODE_NONE;
        }
        if(aucBufIn[3])
            *flags |= TX_ASQ_STATUS_OUT_OVERMOD;
        if(aucBufIn[4])
            *flags |= TX_ASQ_STATUS_OUT_IALL;
        if(aucBufIn[5])
            *flags |= TX_ASQ_STATUS_OUT_IALH;

        *level = aucBufIn[6];
        return FMTX_MODE_OK;
    }
    return FMTX_MODE_NONE;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppEEPROMTuneStatus(uint16_t *freq, uint8_t *power, uint8_t *rfcap)
{
    int i;
    uint8_t *data =NULL;
    /* getting from eeprom image */
    for (i=0; i < fmtxCmdEEPROM.numCmds; i++){
        data = &fmtxCmdEEPROM.commandData[i*FMTX_EEPROM_CMDSIZE];
        if(data[0]==TX_TUNE_FREQ){
            *freq =data[2] << 8 | data[3];
        }
        if(data[0]==TX_TUNE_POWER){
            *power =data[3];
            *rfcap =data[4];
        }
    }

    return FMTX_MODE_OK;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppGetTuneStatus(uint16_t *freq, uint8_t *power, uint8_t *rfcap)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];
    *freq = 0;
    *power = 0;
    *rfcap = 0;
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    if(fmtxCmdSpecial == FMTX_SPECIAL_EEPROM) return fmtxIoAppEEPROMTuneStatus(freq, power, rfcap);
    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711TuneStatus;

    logwrite(LOG_XTREME, "USB: Sending %d bytes:", 2);
    print_bytes(LOG_DUMP, (char *) &aucBufOut[1], 2);

    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    logwrite(LOG_XTREME, "USB: Received:");
    print_bytes(LOG_DUMP, (char *) aucBufIn, 32);

    /* first check status bytes then check which mode */
    if (aucBufIn[0]==(PCTransfer | PCRequestError))
        return FMTX_MODE_NONE;
    /* bad command */
    if (aucBufIn[0]!=PCTransfer)
        return FMTX_MODE_NONE;
    /* check if request done */
    if (aucBufIn[1]==(RequestSi4711TuneStatus|RequestDone)) {
        //errcode 1=sr, 2=fmt
        if(aucBufIn[2]!=SI4711_OK){
            logwrite(LOG_ERROR, "USB: Error in %s", (aucBufIn[2]==1 ? "SR" : (aucBufIn[2]==3 ? "FMT" : "UNK")));
            return FMTX_MODE_NONE;
        }
        *freq = aucBufIn[3] << 8 | aucBufIn[4];
        *power = aucBufIn[5];
        *rfcap = aucBufIn[6];
        return FMTX_MODE_OK;
    }
    return FMTX_MODE_NONE;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxTransmitterGetTuneStatus(double *freq, uint8_t *power, double *rfcap)
{
    uint8_t rfcap0;
    uint16_t freq0;

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    if(fmtxIoAppGetTuneStatus(&freq0, power, &rfcap0) != FMTX_MODE_OK)
        return FMTX_MODE_NONE;

    *freq = freq0/100.0;
    *rfcap = rfcap0 * 0.25;
//    if(freq0!=0 && *power != 0)
//        return FMTX_MODE_TRANSMITTING;
    return FMTX_MODE_OK;
}

static int si471x_ShowStatus(uint8_t u8Status)
{
    logwrite(LOG_DEBUG, "STATUS: %s|%s|%s|%s|%s",
             (u8Status & CTS)?"CTS":"cts",
             (u8Status & ERR)?"ERR":"err",
             (u8Status & RDSINT)?"RDSINT":"rdsint",
             (u8Status & ASQINT)?"ASQINT":"asqint",
             (u8Status & STCINT)?"STCINT":"stcint");

    return (u8Status & CTS)?1:0;
}


static int eeprom_usbAccess(uint8_t *data, uint8_t DataSize, uint16_t offset, int write)
{
    hid_device *phd=NULL;
    uint8_t aucBufIn[43];
    uint8_t aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    if(DataSize>16)
        return FMTX_MODE_NONE;

    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    //bReportOut:
    //  +-----------+-----------+-----------+-----------+-------+
    //  | PCCommand | PCRequest | Offset[0] | Offset[1] | bytes |
    //  +-----------+-----------+-----------+-----------+-------+
    //02 09 00 00 7f 00 00 5c 00 00 00 00 00 00 00 00
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = write ? RequestEepromSectionWrite : RequestEepromSectionRead;
    aucBufOut[3] = 0xff & (offset >> 8);
    aucBufOut[4] = 0xff & offset;
    aucBufOut[5] = DataSize;
    if(write)
        memcpy(&aucBufOut[6], data, DataSize);

    logwrite(LOG_XTREME, "USB: Sending %d bytes:", DataSize+4);
    print_bytes(LOG_DUMP, (char *) &aucBufOut[1], 32);

    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    logwrite(LOG_XTREME, "USB: Received:");
    print_bytes(LOG_DUMP, (char *) aucBufIn, 32);
//  +-----------+-----------+--------+---------+------+---------+
//  | PCCommand | PCRequest | Status | Data[0] | ...  | Data[n] |
//  +-----------+-----------+--------+---------+------+---------+
    if(aucBufIn[2]!=0x01) {
        logwrite(LOG_ERROR, "USB: I2C_TRANSFER failed");
        //sometimes it timeoutes, but the CTS=0, so we must re-read this byte again.
        return FMTX_MODE_NONE;
    }

    if(!write)
        memcpy(data, &aucBufIn[3], DataSize);
    return FMTX_MODE_OK;
}


static int si471x_usbAccess(uint8_t *request, int ReqSize, uint8_t *reply, int *ReplySize)
{
    hid_device *phd=NULL;
    unsigned char aucBufIn[43];
    unsigned char aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    if(ReqSize>8)
        return FMTX_MODE_NONE;

    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    //bReportOut:
    //  +-----------+-----------+---------+---------+--------+- ... -+--------+
    //  | PCCommand | PCRequest | ArgsLen | Command | Arg[1] |  ...  | Arg[7] |
    //  +-----------+-----------+---------+---------+--------+- ... -+--------+
    aucBufOut[0] = 0; // Report ID, ignored
    aucBufOut[1] = PCTransfer;
    aucBufOut[2] = RequestSi4711Access;
    aucBufOut[3] = ReqSize;
    memcpy(&aucBufOut[4], request, ReqSize);

    logwrite(LOG_XTREME, "USB: Sending %d bytes:", ReqSize+3);
    print_bytes(LOG_DUMP, (char *) &aucBufOut[1], 32);

    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    logwrite(LOG_XTREME, "USB: Received:");
    print_bytes(LOG_DUMP, (char *) aucBufIn, 32);
//  +-----------+-----------+-------------+------------+---------+---------+---------+ ... -+----------+
//  | PCCommand | PCRequest | WriteStatus | ReadStatus | RespLen |  Status | Resp[1] | ...  | Resp[15] |
//  +-----------+-----------+-------------+------------+---------+---------+---------+ ... -+----------+
    if(aucBufIn[2]!=1) {
        logwrite(LOG_ERROR, "USB: command failed (%d): %s, returned (FALSE)", aucBufIn[2], Si471xStatusStr(aucBufIn[2]));
        //sometimes it timeoutes, but the CTS=0, so we must re-read this byte again.
        //return 1;
    }

    if(aucBufIn[3]!=SI4711_OK) {
        logwrite(LOG_ERROR, "USB: I2C_READ failed (%d): %s", aucBufIn[3], Si471xStatusStr(aucBufIn[3]));
        return FMTX_MODE_NONE;
    }

    if(aucBufIn[4] > 16) {
        logwrite(LOG_ERROR, "USB: I2C_READ failed, too much bytes received: %d", aucBufIn[4]);
        return FMTX_MODE_NONE;
    }

    logwrite(LOG_XTREME, "USB: Volume: %d.%d (deviation: %d)", (char ) aucBufIn[21], (char ) aucBufIn[22], (uint16_t ) (aucBufIn[23] << 8 | aucBufIn[24]));

    memcpy(reply, &aucBufIn[5], aucBufIn[4]);
    *ReplySize=aucBufIn[4];
    return FMTX_MODE_OK;
}

static int si471x_cmdEEPROMAccess(uint8_t *request, int ReqSize, uint8_t *reply, int *ReplySize)
{
    int i;
    uint8_t *data=NULL;

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    if(ReqSize < 1 || ReqSize > 8)
        return FMTX_MODE_NONE;

    logwrite(LOG_DEBUG, "EEPROM: Emulate command %02x", request[0]);
    for(i=0;i<16;i++)
        reply[i] = 0x00;

    switch(request[0]){
    /* forbidden commands */
    case POWER_UP:
    case POWER_DOWN:
    case GET_PROPERTY:
    case SET_PROPERTY:
    case TX_TUNE_STATUS:
        logwrite(LOG_ERROR, "EEPROM: %s: mad cmd %02x not allowed!", __FUNCTION__, request[0]);
        return FMTX_MODE_NONE;
        break;
    /* setting to eeprom image */
    case TX_RDS_BUFF:
        reply[2] = (FMTX_MAX_EEPROM_CMDS - fmtxCmdEEPROM.numCmds)*3;
    case TX_RDS_PS:
    case TX_TUNE_FREQ:
    case TX_TUNE_POWER:
        if(fmtxCmdEEPROM.numCmds > FMTX_MAX_EEPROM_CMDS) {
            logwrite(LOG_ERROR, "EEPROM: %s: error setting cmd %02x not enough space!", __FUNCTION__, request[0]);
            return FMTX_MODE_NONE;
        }
        data = &fmtxCmdEEPROM.commandData[fmtxCmdEEPROM.numCmds*FMTX_EEPROM_CMDSIZE];

        for(i=0;i<ReqSize;i++)
            data[i] = request[i];
        for(;i<8;i++)
            data[i] = 0x00;

        fmtxCmdEEPROM.numCmds++;
        break;
    default:
        *ReplySize=0;
        return FMTX_MODE_NONE;
    }

    reply[0] = 0x80;
    *ReplySize=16;
    return FMTX_MODE_OK;
}



/*
 * i2c transfer to device (write command and read status and response)
 */
EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppCommand(uint8_t u8Cmd, uint8_t *pu8Resp, uint8_t u8Amount, ...)
{
    uint8_t buf_in[8];
    uint8_t buf_out[16];
    uint8_t i;
    int bytes;
    va_list vl;

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    if(u8Amount>7)
        return FMTX_MODE_NONE;

    memset(buf_in, 0x00, 8);
    memset(buf_out, 0x00, 16);
    buf_in[0] = u8Cmd;
    /* variable args handling */
    va_start(vl,u8Amount);
    for (i=0; i<u8Amount; i++) {
        buf_in[1+i] = (uint8_t ) va_arg(vl,int);
    }
    va_end(vl);

    logwrite(LOG_DEBUG, "USB: Sending device Cmd: %02x %d-args...", u8Cmd, u8Amount);
    print_bytes(LOG_DUMP, (char *) &buf_in[1], 7);

    bytes=0;

    /* cmd for eeprom ? */
    if(fmtxCmdSpecial == FMTX_SPECIAL_EEPROM) {
        if(si471x_cmdEEPROMAccess(buf_in, u8Amount+1, buf_out, &bytes) != FMTX_MODE_OK)
            return FMTX_MODE_NONE;
    } else {
        if(si471x_usbAccess(buf_in, u8Amount+1, buf_out, &bytes) != FMTX_MODE_OK)
            return FMTX_MODE_NONE;
    }

    if(bytes==0) {
        logwrite(LOG_ERROR, "I2C: Status not received!");
        return FMTX_MODE_NONE;
    }

    if (buf_out[0] & ERR) {
        logwrite(LOG_ERROR, "I2C: Status not have got  ERR flag!");
    }

    if(!(buf_out[0] & CTS)) {
        logwrite(LOG_ERROR, "I2C: Status not have CTS!");
    }


    memcpy(pu8Resp, buf_out, bytes);
    logwrite(LOG_DEBUG, "USB: Cmd %02x answered: %02x... ", u8Cmd, buf_out[0]);
    print_bytes(LOG_DUMP, (char *) &buf_out[0], 16);

    si471x_ShowStatus(pu8Resp[0]);

    return FMTX_MODE_OK;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppSetEEPROMProperty(int16_t i16Prop, int16_t i16Val)
{
    uint8_t *data;
    if(fmtxCmdEEPROM.numCmds > FMTX_MAX_EEPROM_CMDS) {
        logwrite(LOG_ERROR, "EEPROM: %s: error setting prop %04x => %04x not enough space!", __FUNCTION__, i16Prop, i16Val);
        return FMTX_MODE_NONE;
    }

    if(getDefaultProp(i16Prop) == i16Val){
        logwrite(LOG_ERROR, "EEPROM: %s: prop %04x have default value %04x saving space!", __FUNCTION__, i16Prop, i16Val);
        return FMTX_MODE_OK;
    }


    data = &fmtxCmdEEPROM.commandData[fmtxCmdEEPROM.numCmds*FMTX_EEPROM_CMDSIZE];

    data[0] = SET_PROPERTY;
    data[1] = 0x00;
    data[2] = 0xff & (i16Prop>>8);
    data[3] = 0xff & i16Prop;
    data[4] = 0xff & (i16Val>>8);
    data[5] = 0xff & i16Val;
    data[6] = 0x00;
    data[7] = 0x00;
    fmtxCmdEEPROM.numCmds++;
    return FMTX_MODE_OK;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppSetProperty(int16_t i16Prop, int16_t i16Val)
{
    hid_device *phd=NULL;
    uint8_t aucBufIn[43];
    uint8_t aucBufOut[43];

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    if(i16Prop == GPO_IEN ||
            i16Prop == DIGITAL_INPUT_FORMAT ||
            i16Prop == DIGITAL_INPUT_SAMPLE_RATE ||
            i16Prop == REFCLK_FREQ ||
            i16Prop == REFCLK_PRESCALE) {
        logwrite(LOG_ERROR, "FMTX: %s: Setting porperty %04x - is not allowed!", __FUNCTION__, i16Prop);
        return FMTX_MODE_NONE;
    }

    if(fmtxCmdSpecial == FMTX_SPECIAL_EEPROM) return fmtxIoAppSetEEPROMProperty(i16Prop, i16Val);

    memset(aucBufOut, 0x00, 43);
    aucBufOut[0] = 0x00;            //report number, would be unused!
    aucBufOut[1] = PCTransfer;      //
    aucBufOut[2] = RequestSi4711SetProp;
    aucBufOut[3] = i16Prop >> 8;
    aucBufOut[4] = i16Prop;
    aucBufOut[5] = i16Val >> 8;
    aucBufOut[6] = i16Val;

    logwrite(LOG_DEBUG, "USB: Sending device \"%s\" %04x => %04x request...", Si471xRequestStr(aucBufOut[2]), i16Prop, i16Val);
    print_bytes(LOG_DUMP, (char *) &aucBufOut[1], 6);

    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;

    /* Send a BL Query Command */
    //bReportOut:
    //  +-----------+-----------+-------+-------+------+------+
    //  | PCCommand | PCRequest | AddrH | AddrL | ValH | ValL |
    //  +-----------+-----------+-------+-------+------+------+
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);

    fmtxClose(phd);
    phd = NULL;

    logwrite(LOG_XTREME, "USB: Received:");
    print_bytes(LOG_DUMP, (char *) aucBufIn, 32);
//  +-----------+-----------+-------------+------------+----------+-----------+
//  | PCCommand | PCRequest | WriteStatus | ReadStatus | NumBytes | CMDStatus |
//  +-----------+-----------+-------------+------------+----------+-----------+
    if(aucBufIn[0] & PCRequestError) {
        logwrite(LOG_ERROR, "USB: request error");
        goto set_prop_err;
    }

    if(!(aucBufIn[1] & RequestDone)) {
        logwrite(LOG_ERROR, "USB: request is not done!");
        goto set_prop_err;
    }

    if(aucBufIn[8]!=SI4711_OK) {
        logwrite(LOG_ERROR, "USB: Device request \"%s\" failed (%d): %s", Si471xRequestStr(RequestSi4711SetProp), aucBufIn[2], Si471xStatusStr(aucBufIn[2]));
        goto set_prop_err;
    }

    if ( aucBufIn[7] & ERR) {
        logwrite(LOG_ERROR, "SI471X: Answers: Error setting property 0x%04x to \"0x%04x\".", i16Prop, i16Val);
        goto set_prop_err;
    }

    if ( !(aucBufIn[7] & CTS)) {
        logwrite(LOG_ERROR, "SI471X: Answers: NO CTS! When setting property 0x%04x.", i16Prop);
        goto set_prop_err;
    }

    if(aucBufIn[6]!=1) {
        logwrite(LOG_ERROR, "USB: Device request \"%s\" failed (%02x): %s (false!)", Si471xRequestStr(RequestSi4711SetProp), aucBufIn[6]);
        goto set_prop_err;
    }

    return FMTX_MODE_OK;
set_prop_err:
    return FMTX_MODE_NONE;
}

static int16_t getDefaultProp(int16_t i16Prop)
{
    switch (i16Prop){
    /* hardcoded values */
    case DIGITAL_INPUT_SAMPLE_RATE:
        return 44100;
        break;
    case REFCLK_FREQ:
        return 31250;
        break;
    case REFCLK_PRESCALE:
        return 192;
        break;
    case GPO_IEN:
    case DIGITAL_INPUT_FORMAT:
        return 0;
        break;

    /* user-defined values */
    case TX_LINE_INPUT_LEVEL:
        return 0x327C;
        break;
    case TX_LINE_INPUT_MUTE:
        return 0x00;
        break;
    case TX_COMPONENT_ENABLE:
        return 0x03;
        break;
    case TX_AUDIO_DEVIATION:
        return 6825;
        break;
    case TX_PILOT_DEVIATION:
        return 675;
        break;
    case TX_RDS_DEVIATION:
        return 200;
        break;
    case TX_PREEMPHASIS:
        return 0x00;
        break;
    case TX_PILOT_FREQUENCY:
        return 19000;
        break;
    case TX_ACOMP_ENABLE:
        return 0x02;
        break;
    case TX_ACOMP_THRESHOLD:
        return -40;
        break;
    case TX_ACOMP_ATTACK_TIME:
        return 0;
        break;
    case TX_ACOMP_RELEASE_TIME:
        return 0x04;
        break;
    case TX_ACOMP_GAIN:
        return 15;
        break;
    case TX_LIMITER_RELEASE_TIME:
        return 0x0066;
        break;
    case TX_ASQ_INT_SELECT:
        return 0x00;
        break;
    case TX_ASQ_LEVEL_LOW:
    case TX_ASQ_DURATION_LOW:
    case TX_ASQ_LEVEL_HIGH:
    case TX_ASQ_DURATION_HIGH:
        return 0x00;
        break;
    case TX_RDS_INT_SOURCE:
        return 0x00;
        break;
    case TX_RDS_PI:
        return 0x40A7;
        break;
    case TX_RDS_PS_MIX:
        return 0x03;
        break;
    case TX_RDS_PS_MISC:
        return 0x1008;
        break;
    case TX_RDS_PS_REPEAT_COUNT:
        return 3;
        break;
    case TX_RDS_PS_MESSAGE_COUNT:
        return 1;
        break;
    case TX_RDS_PS_AF:
        return  0xE0E0;
        break;
    case TX_RDS_FIFO_SIZE:
        return  0x00;
        break;
    }
    return  0x00;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppGetEEPROMProperty(int16_t i16Prop, int16_t *pi16Val)
{
    int i, found=0;
    uint8_t *data=NULL;

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    if(fmtxCmdSpecial != FMTX_SPECIAL_EEPROM)
        return FMTX_MODE_NONE;

    for (i=0; i < fmtxCmdEEPROM.numCmds; i++){
        data = &fmtxCmdEEPROM.commandData[i*FMTX_EEPROM_CMDSIZE];
        //this is set property command!
        if(data[0]==SET_PROPERTY && data[1]==0x00){
            if(data[2]==(0xff & (i16Prop>>8)) && data[3]==(0xff & i16Prop)) {
                *pi16Val = (int16_t ) ((data[4] & 0x00FF) << 8) | data[5];
                found=1;
            }
        }
    }

    if(!found) *pi16Val = getDefaultProp(i16Prop);
    logwrite(LOG_DEBUG, "EEPROM: Answers: => 0x%04x.", *pi16Val);
    return FMTX_MODE_OK;
}


EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxIoAppGetProperty(int16_t i16Prop, int16_t *pi16Val)
{
    hid_device *phd=NULL;
    uint8_t aucBufIn[43];
    uint8_t aucBufOut[43];

    logwrite(LOG_DEBUG, "USB: Sending device \"%s\" %04x => ?? request...", Si471xRequestStr(RequestSi4711GetProp), i16Prop);

    if(fmtxCmdSpecial == FMTX_SPECIAL_EEPROM) return fmtxIoAppGetEEPROMProperty(i16Prop, pi16Val);

    memset(aucBufOut, 0x00, 43);
    aucBufOut[0] = 0x00;            //report number, would be unused!
    aucBufOut[1] = PCTransfer;      //
    aucBufOut[2] = RequestSi4711GetProp;
    aucBufOut[3] = i16Prop >> 8;
    aucBufOut[4] = i16Prop;

    print_bytes(LOG_DUMP, (char *) &aucBufOut[1], 4);


    phd = fmtxOpen();
    if (phd == NULL)
        return FMTX_MODE_NONE;
    /* Send a BL Query Command */
    //bReportOut:
    //  +-----------+-----------+-------+-------+
    //  | PCCommand | PCRequest | AddrH | AddrL |
    //  +-----------+-----------+-------+-------+
    hid_write(phd, aucBufOut, 43);
    memset(aucBufIn, 0xCC, 42); // Clear out the response buffer
    hid_read(phd, aucBufIn, 42);
    fmtxClose(phd);
    phd = NULL;

    logwrite(LOG_XTREME, "USB: Received:");
    print_bytes(LOG_DUMP, (char *) aucBufIn, 32);
//  +-----------+-----------+-----------+
//  | PCCommand | PCRequest | CMDStatus |
//  +-----------+-----------+-----------+
    if(aucBufIn[0] & PCRequestError) {
        logwrite(LOG_ERROR, "USB: request error");
        goto get_prop_err;
    }

    if(!(aucBufIn[1] & RequestDone)) {
        logwrite(LOG_ERROR, "USB: request is not done!");
        goto get_prop_err;
    }

    if(aucBufIn[8]!=SI4711_OK) {
        logwrite(LOG_ERROR, "USB: Device request \"%s\" failed (%d): %s", Si471xRequestStr(RequestSi4711GetProp), aucBufIn[8], Si471xStatusStr(aucBufIn[8]));
        goto get_prop_err;
    }

    // buf[5]- is write status
    // buf[6]- is read status
    if ( aucBufIn[7] & ERR) {
        logwrite(LOG_ERROR, "SI471X: Answers: Error getting property 0x%04x.", i16Prop);
        goto get_prop_err;
    }

    if ( !(aucBufIn[7] & CTS)) {
        logwrite(LOG_ERROR, "SI471X: Answers: NO CTS! When getting property 0x%04x.", i16Prop);
        goto get_prop_err;
    }

    if(aucBufIn[6]!=1) {
        logwrite(LOG_ERROR, "USB: Device request \"%s\" failed (%02x): (false!)", Si471xRequestStr(RequestSi4711SetProp), aucBufIn[6]);
        goto get_prop_err;
    }

    *pi16Val = (int16_t ) ((aucBufIn[4] & 0x00FF) << 8) | aucBufIn[5];
    logwrite(LOG_DEBUG, "SI471X: Answers: => 0x%04x.", *pi16Val);
    return FMTX_MODE_OK;
get_prop_err:
    return FMTX_MODE_NONE;
}




/*******************************************************
 * Medium-level API
 ******************************************************/
EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxEEPROMInfoClean(void)
{
    fmtxCmdEEPROM.syncWord = 0;
    fmtxCmdEEPROM.numCmds = 0;
    fmtxCmdEEPROM.startupFlag = 0;
    memset(fmtxCmdEEPROM.commandData, 0xCC, FMTX_EEPROM_CMDSIZE*FMTX_MAX_EEPROM_CMDS);
    return FMTX_MODE_OK;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxEEPROMReadConfig(void)
{
    uint8_t cmdChunk[8];
    int i;
    if(fmtxCmdSpecial != FMTX_SPECIAL_EEPROM)
        return FMTX_MODE_NONE;

    logwrite(LOG_DEBUG, "EEPROM: Reading header.");
    /* get chunks header */
    if(eeprom_usbAccess(cmdChunk, FMTX_EEPROM_CMDSIZE, TX_CONFIG_OFFSET + 0, 0)!=FMTX_MODE_OK)
        return FMTX_MODE_NONE;
    print_bytes(LOG_ERROR, (char *) cmdChunk, FMTX_EEPROM_CMDSIZE);

    fmtxCmdEEPROM.syncWord = (cmdChunk[0] << 8) | cmdChunk[1];
    fmtxCmdEEPROM.numCmds = cmdChunk[2];
    fmtxCmdEEPROM.startupFlag = cmdChunk[3];

    if(fmtxCmdEEPROM.syncWord != 0xB00B){
        logwrite(LOG_ERROR, "EEPROM: Bad Sync Word: %04X.", fmtxCmdEEPROM.syncWord);
        return FMTX_MODE_POWER_DOWN;
    }

    if(fmtxCmdEEPROM.numCmds > FMTX_MAX_EEPROM_CMDS){
        logwrite(LOG_ERROR, "EEPROM: Too much commands: %d.", fmtxCmdEEPROM.numCmds);
        return FMTX_MODE_NONE;
    }
    memset(fmtxCmdEEPROM.commandData, 0xCC, FMTX_EEPROM_CMDSIZE*FMTX_MAX_EEPROM_CMDS);

    for (i=0; i < fmtxCmdEEPROM.numCmds; i++){
        if(eeprom_usbAccess(&fmtxCmdEEPROM.commandData[i*FMTX_EEPROM_CMDSIZE], FMTX_EEPROM_CMDSIZE, TX_CONFIG_OFFSET + FMTX_EEPROM_CMDSIZE*(i+1), 0)!=FMTX_MODE_OK)
            return FMTX_MODE_NONE;
        print_bytes(LOG_ERROR, (char *) &fmtxCmdEEPROM.commandData[i*FMTX_EEPROM_CMDSIZE], FMTX_EEPROM_CMDSIZE);
    }
    return FMTX_MODE_OK;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxEEPROMWriteConfig(int autorun)
{
    uint8_t configData[8];
    /* sample config:
    header: b0 0b 1b 01 ff ff ff ff
            *AudioProps*
            *TxTune*
            *TxProps*
            *RDS PS Msgs*
            *RDS PS*
            *RDS RT*
            *TX component enable*
    */
    int i;
    fmtxCmdEEPROM.startupFlag = autorun;
    fmtxCmdEEPROM.syncWord = 0xb00b;
    memset(configData, 0xCC, 8);
    configData[0] = (0xff00 & fmtxCmdEEPROM.syncWord) >> 8;
    configData[1] = 0xff & fmtxCmdEEPROM.syncWord;
    configData[2] = fmtxCmdEEPROM.numCmds;
    configData[3] = 0xff & autorun;
    if(eeprom_usbAccess(configData, FMTX_EEPROM_CMDSIZE, TX_CONFIG_OFFSET, 1)!=FMTX_MODE_OK)
        return FMTX_MODE_NONE;

    for (i=0; i < fmtxCmdEEPROM.numCmds; i++){
        print_bytes(LOG_ERROR, (char *) &fmtxCmdEEPROM.commandData[i*FMTX_EEPROM_CMDSIZE], FMTX_EEPROM_CMDSIZE);
        if(eeprom_usbAccess(&fmtxCmdEEPROM.commandData[i*FMTX_EEPROM_CMDSIZE], FMTX_EEPROM_CMDSIZE, TX_CONFIG_OFFSET + FMTX_EEPROM_CMDSIZE*(i+1), 1)!=FMTX_MODE_OK)
            return FMTX_MODE_NONE;
    }
    return FMTX_MODE_OK;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxEEPROMGetStartupFlag(void)
{
    if(fmtxCmdEEPROM.syncWord != 0xB00B)
        return 0;
    return fmtxCmdEEPROM.startupFlag;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxEEPROMSetStartupFlag(uint8_t flag, int force)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    if(fmtxCmdEEPROM.syncWord != 0xB00B)
        return FMTX_MODE_NONE;
    fmtxCmdEEPROM.startupFlag = flag;
    if(force)
        return eeprom_usbAccess(&flag, 1, TX_CONFIG_OFFSET + 2+1, 1);
    else
        return FMTX_MODE_OK;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxEEPROMWriteFwChunk(const uint8_t *data, int size, uint16_t offset)
{
    return eeprom_usbAccess((uint8_t *) data, size, offset, 1);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL FMTX_MODE_ENUM fmtxEEPROMReadFwChunk(uint8_t *data, int size, uint16_t offset)
{
    return eeprom_usbAccess(data, size, offset, 0);
}




/* audio compressor/limiter */
EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioGetAcompFlags()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_ACOMP_ENABLE, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (u16Val & 0x00FF);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAcompFlags(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ACOMP_ENABLE, u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL int16_t fmtxAudioGetAcompThreshold()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    int16_t i16Val=0;
    if(fmtxIoAppGetProperty(TX_ACOMP_THRESHOLD, &i16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return i16Val;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAcompThreshold(int16_t i16Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ACOMP_THRESHOLD, i16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL double fmtxAudioGetAcompAttackTime()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty( TX_ACOMP_ATTACK_TIME, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return 0.5 * ((u16Val & 0x000F) + 1);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAcompAttackTime(double dVal)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    uint16_t u16Val = dVal/0.5 - 1;

    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ACOMP_ATTACK_TIME, u16Val > 9 ? 9 : u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);

}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioGetAcompReleaseTimeId()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty( TX_ACOMP_RELEASE_TIME, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return u16Val & 0x0007;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAcompReleaseTimeId(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ACOMP_RELEASE_TIME, u8Val > 4 ? 4 : u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioGetAcompGain()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty( TX_ACOMP_GAIN, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return u16Val & 0x003F;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAcompGain(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ACOMP_GAIN, u8Val > 20 ? 20 : u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL double fmtxAudioGetLimiterReleaseTime()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty( TX_LIMITER_RELEASE_TIME, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    if(u16Val==0)
        return 0;
    return 512.0/u16Val;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetLimiterReleaseTime(double dVal)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    uint16_t u16Val = 512.0/dVal;
    if(u16Val < 5)
        u16Val=5;
    else if(u16Val > 2000)
        u16Val=2000;

    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_LIMITER_RELEASE_TIME, u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

/* audio signal measurement */
EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioGetAsqIntFlags()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty( TX_ASQ_INT_SELECT, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return u16Val & 0x0007;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudiosetAsqIntFlags(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ASQ_INT_SELECT, u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL int8_t fmtxAudioGetAsqLevelLow()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    int16_t i16Val=0;
    if(fmtxIoAppGetProperty( TX_ASQ_LEVEL_LOW, &i16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return i16Val & 0x00FF;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAsqLevelLow(int8_t i8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ASQ_LEVEL_LOW, i8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint16_t fmtxAudioGetAsqDurationLow()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty( TX_ASQ_DURATION_LOW, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return u16Val;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAsqDurationLow(uint16_t u16Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ASQ_DURATION_LOW, (int16_t ) u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL int8_t fmtxAudioGetAsqLevelHigh()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    int16_t i16Val=0;
    if(fmtxIoAppGetProperty( TX_ASQ_LEVEL_HIGH, &i16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return i16Val & 0x00FF;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAsqLevelHigh(int8_t i8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ASQ_LEVEL_HIGH, i8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return(fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint16_t fmtxAudioGetAsqDurationHigh()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty( TX_ASQ_DURATION_HIGH, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return u16Val;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetAsqDurationHigh(uint16_t u16Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_ASQ_DURATION_HIGH, (int16_t ) u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint16_t fmtxAudioGetInputLevels()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    int16_t i16Val=0;
    if(fmtxIoAppGetProperty( TX_LINE_INPUT_LEVEL, &i16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;

    return (uint16_t ) i16Val;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetInputLevels(uint16_t atten, uint16_t level)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    uint16_t result;

    if(level > 1023)
        level = 1023;
    result = ((atten << TX_LINE_INPUT_LEVEL_LIATTEN_SHFT) & TX_LINE_INPUT_LEVEL_LIATTEN_MASK ) | (level & TX_LINE_INPUT_LEVEL_LILEVEL_MASK);

    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_LINE_INPUT_LEVEL, (int16_t ) result) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint16_t fmtxAudioGetInputMute()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty( TX_LINE_INPUT_MUTE, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return u16Val & (TX_LINE_INPUT_MUTE_RIMUTE_MASK|TX_LINE_INPUT_MUTE_LIMUTE_MASK);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxAudioSetInputMute(uint16_t u16Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    u16Val = u16Val & (TX_LINE_INPUT_MUTE_RIMUTE_MASK|TX_LINE_INPUT_MUTE_LIMUTE_MASK);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_LINE_INPUT_MUTE, (int16_t ) u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}


/* transmitting parameters */
EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterGetComponentFlags()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    int16_t i16Val=0;
    if(fmtxIoAppGetProperty(TX_COMPONENT_ENABLE, &i16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (i16Val & 0x00FF);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterSetComponentFlags(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_COMPONENT_ENABLE, u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterGetPreemphasisId()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    int16_t i16Val=0;
    if(fmtxIoAppGetProperty(TX_PREEMPHASIS, &i16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (i16Val & TX_PREMPHASIS_MASK);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterSetPreemphasisId(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_PREEMPHASIS, TX_PREMPHASIS_MASK & u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint32_t fmtxTransmitterGetAudioDeviation()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_AUDIO_DEVIATION, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (uint32_t ) u16Val*10;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterSetAudioDeviation(uint32_t u32Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_AUDIO_DEVIATION, u32Val > 90000 ? 90000 : u32Val/10) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint32_t fmtxTransmitterGetPilotDeviation()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_PILOT_DEVIATION, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (uint32_t ) u16Val*10;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterSetPilotDeviation(uint32_t u32Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_PILOT_DEVIATION, u32Val > 90000 ? 90000 : u32Val/10) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint32_t fmtxTransmitterGetRDSDeviation()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_RDS_DEVIATION, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (uint32_t ) u16Val*10;
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterSetRDSDeviation(uint32_t u32Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_RDS_DEVIATION, u32Val > 90000 ? 90000 : u32Val/10) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint16_t fmtxTransmitterGetPilotFrequency()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_PILOT_FREQUENCY, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (u16Val);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterSetPilotFrequency(uint16_t u16Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_PILOT_FREQUENCY, u16Val > 19000 ? 19000 : u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}



EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterSetTuneFreq(double freq)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    uint8_t buff[16];
    uint16_t freq0 = freq*100;
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppCommand(TX_TUNE_FREQ, buff, 3, 0x00, freq0>>8, 0x00FF & freq0) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxTransmitterSetTunePower(uint8_t power, double rfcap)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    uint8_t buff[16];
    uint8_t rfcap0 = rfcap/0.25;
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppCommand(TX_TUNE_POWER, buff, 4, 0x00, 0x00, power, rfcap0) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}


/* RDS parameters */
EXTERN FMTX_API_EXPORT FMTX_API_CALL uint16_t fmtxRDSGetPI()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_RDS_PI, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (u16Val);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t  fmtxRDSSetPI(uint16_t u16Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_RDS_PI, (int16_t ) u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSGetPsMixId()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_RDS_PS_MIX, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (u16Val & 0x0007);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetPsMixId(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_RDS_PS_MIX, u8Val > 6 ? 6 : u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint16_t fmtxRDSGetPsMiscFlags()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_RDS_PS_MISC, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (u16Val);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetPsMiscFlags(uint16_t u16Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_RDS_PS_MISC, (int16_t) u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSGetPsRepeatCount()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_RDS_PS_REPEAT_COUNT, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (u16Val & 0x00FF);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetPsRepeatCount(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(u8Val < 1)
        u8Val=1;
    if(fmtxIoAppSetProperty(TX_RDS_PS_REPEAT_COUNT, u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSGetPsMessageCount()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_RDS_PS_MESSAGE_COUNT, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (u16Val & 0x000F);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetPsMessageCount(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(u8Val < 1)
        u8Val=1;
    else if(u8Val > 12)
        u8Val=12;
    if(fmtxIoAppSetProperty(TX_RDS_PS_MESSAGE_COUNT, u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSGetPsAFStatus(double *freq)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_RDS_PS_AF, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    if(u16Val == 0xE0E0){
        *freq=0;
        return 0;
    } else {
        *freq=87.5 + (0.1*(u16Val & 0x00FF));
        return 1;
    }
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetPsAF(double freq, uint8_t enabled)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    uint16_t u16Val=0;
    if(enabled) {
        u16Val = (freq-87.5)/0.1;
        if(u16Val>0xCD)
            u16Val=0xCD;
        u16Val |= 0xE100;
    } else {
        u16Val=0xE0E0;
    }

    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_RDS_PS_AF, (int16_t) u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}


EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSGetFifoSize()
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    uint16_t u16Val=0;
    if(fmtxIoAppGetProperty(TX_RDS_FIFO_SIZE, (int16_t *) &u16Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (u16Val & TX_RDS_FIFO_SIZE_MASK);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetFifoSize(uint8_t u8Val)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppSetProperty(TX_RDS_FIFO_SIZE, u8Val) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}


EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSGetPsMessageById(uint8_t Id, char *messages)
{
    int i;
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    memset(messages, ' ', 8);

    uint8_t *data=NULL;
    if(fmtxCmdEEPROM.syncWord != 0xB00B ||
            !fmtxCmdEEPROM.numCmds ||
            fmtxCmdEEPROM.numCmds > FMTX_MAX_EEPROM_CMDS) {
        messages[0] = 'U'; messages[1] = 'n'; messages[2] = 'k'; messages[3] = 'n';
        messages[4] = 'o'; messages[5] = 'w'; messages[6] = 'n'; messages[7] = '.';
        return FMTX_MODE_NONE;
    }

    for (i=0; i < fmtxCmdEEPROM.numCmds; i++){
        data = &fmtxCmdEEPROM.commandData[i*FMTX_EEPROM_CMDSIZE];
        //this is set property command!
        if(data[0]==TX_RDS_PS && data[1]==Id*2){
            messages[0] = data[2];
            messages[1] = data[3];
            messages[2] = data[4];
            messages[3] = data[5];
        }
        if(data[0]==TX_RDS_PS && data[1]==(Id*2+1)){
            messages[4] = data[2];
            messages[5] = data[3];
            messages[6] = data[4];
            messages[7] = data[5];
        }
    }

    messages[8] = 0;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetPsMessageById(uint8_t Id, char *messages)
{
    uint8_t buff[16];
    uint8_t msg[8];
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    memset(msg, ' ', 8);
    memcpy(msg, messages, strlen(messages) > 8 ? 8 : strlen(messages));
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppCommand(TX_RDS_PS, buff, 5, Id*2, msg[0], msg[1], msg[2], msg[3]) != FMTX_MODE_OK) {
        fmtxCmdStatus=FMTX_MODE_NONE;
        return (fmtxCmdStatus);
    }

    if(fmtxIoAppCommand(TX_RDS_PS, buff, 5, Id*2 + 1, msg[4], msg[5], msg[6], msg[7]) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSGetRtMessage(char *messages)
{
    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);
    fmtxCmdStatus=FMTX_MODE_OK;
    memset(messages, 0x00, 65);
    int i;
    uint8_t *data=NULL;
    char *eos_char;

    if(fmtxCmdEEPROM.syncWord != 0xB00B ||
            !fmtxCmdEEPROM.numCmds ||
            fmtxCmdEEPROM.numCmds > FMTX_MAX_EEPROM_CMDS) {
        messages[0] = 'U'; messages[1] = 'n'; messages[2] = 'k'; messages[3] = 'n';
        messages[4] = 'o'; messages[5] = 'w'; messages[6] = 'n'; messages[7] = '.';
        messages[8] = '\0';
        return FMTX_MODE_NONE;
    }


    for (i=0; i < fmtxCmdEEPROM.numCmds; i++){
        data = &fmtxCmdEEPROM.commandData[i*FMTX_EEPROM_CMDSIZE];
        //this is set property command!
        if(data[0]==TX_RDS_BUFF && data[1]==TX_RDS_BUFF_IN_LDBUFF && data[2]==0x20 && data[3] < 16){
            messages[data[3]*4]   = data[4];
            messages[data[3]*4+1] = data[5];
            messages[data[3]*4+2] = data[6];
            messages[data[3]*4+3] = data[7];
        }
    }

    eos_char = strchr(messages, 0x0d);
    if(eos_char) {
        *eos_char = 0x00;
    }
#if 0
    do {
        eos_char = strrchr(messages, 0x20);
        if(eos_char)
        *eos_char = 0x00;
    } while(eos_char && eos_char-messages == strlen(messages)-1);
#endif


    messages[64] = 0x00;
    return (fmtxCmdStatus);
}



static int rtplus_toggle_bit = TRUE; //XXX:used to save RT+ toggle bit value
#define RTPLUS_GROUP_ID 0b1011

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetRtPlusInfo(int content1, int content1_pos, int content1_len,
                                                                   int content2, int content2_pos, int content2_len)
{
    uint8_t buff[16];
    char msg[6];
    fmtxCmdStatus=FMTX_MODE_OK;

    //check if we have RT+ at all
    if(!content1_len && !content2_len)
        return (fmtxCmdStatus);

    memset (msg, 0x00, 6);
    bitstream_t *bs=NULL;
    if(!bs_create(&bs) || !bs_attach(bs, (uint8_t *) msg, 6))
        return FMTX_MODE_NONE;

    bs_put(bs, 0x00, 3);                    //seek to start pos
    rtplus_toggle_bit = !rtplus_toggle_bit; //if we using RT+, update information!
    bs_put(bs, rtplus_toggle_bit, 1);       //Item toggle bit
    bs_put(bs, 1, 1);                       //Item running bit

    if(content1_len) {
        bs_put(bs, content1, 6);        //RT content type 1
        bs_put(bs, content1_pos, 6);    //start marker 1
        bs_put(bs, content1_len, 6);    //length marker 1
    }

    if(content2_len) {
        bs_put(bs, content2, 6);        //RT content type 2
        bs_put(bs, content2_pos, 6);    //start marker 2
        bs_put(bs, content2_len, 5);    //length marker 2 (5 bits!)
    }

    if(fmtxIoAppCommand(TX_RDS_BUFF, buff, 7, TX_RDS_BUFF_IN_LDBUFF,
                        RTPLUS_GROUP_ID << 4,
                        msg[0], msg[1], msg[2], msg[3], msg[4]) != FMTX_MODE_OK){
        fmtxCmdStatus=FMTX_MODE_NONE;
        return (fmtxCmdStatus);
    }
    logwrite(LOG_DEBUG, "RDS: circular: %d/%d, fifo: %d/%d", buff[3], buff[2], buff[5], buff[4]);

    //send RT+ announces
    //  FmRadioController::HandleRDSData
    //  FmRadioRDSParser::ParseRDSData  RDSGroup:6
    //  !!TEST FmRadioRDSParser::RT+ GROUP_TYPE_3A : RT+ Message
    //  !!TEST FmRadioRDSParser::RT+ Message RT+ flag : 0 0, RT+ AID : 4bd7 19415 (flag : 0 0)
    //  !!TEST FmRadioRDSParser::RT+ Message application group type code : 16 22
    if(fmtxIoAppCommand(TX_RDS_BUFF, buff, 7, TX_RDS_BUFF_IN_LDBUFF,
                        0x30, //RDS GROUP: 3A
                        RTPLUS_GROUP_ID << 1, //we are describing RT+ group 11A
                        //RTPLUS_GROUP_ID, //we are describing RT+ group 11A
                        0, //xxx.y.zzzz
                               //xxx  - rfu
                               //y    - cb flag (for template number)
                               //zzzz - for rds server needs.
                        0, //template id=0
                        0x4B, 0xD7 //it's RT+
                        ) != FMTX_MODE_OK){
        fmtxCmdStatus=FMTX_MODE_NONE;
        return (fmtxCmdStatus);
    }
    logwrite(LOG_DEBUG, "RDS: circular: %d/%d, fifo: %d/%d", buff[3], buff[2], buff[5], buff[4]);
    return (fmtxCmdStatus);
}


EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSetRtMessage(const char *messages)
{
    uint8_t i, avaliableBytes, msgChars, blockPtr;
    uint8_t buff[16];
    char msg[64];

    memset (msg, 0x20, 64);
    fmtxCmdStatus=FMTX_MODE_OK;
    logwrite(LOG_DUMP, "FMTX: %s msg: \"%s\"", __FUNCTION__, messages);

    msgChars = strlen(messages);
    if (msgChars > 64){
            logwrite(LOG_INFO, "RDS: RT %d chars - is too long, max RadioText length is %d chars",
                                                    strlen(messages), 64);
            msgChars=64;
    } /*else if (msgChars < 63) {
        msg[msgChars+1] = 0x0d;
    }*/

    //return if msg is empty and we using eeprom
    if( (fmtxCmdSpecial == FMTX_SPECIAL_EEPROM) && !msgChars)
        return (fmtxCmdStatus);

    if(fmtxIoAppCommand(TX_RDS_BUFF, buff, 7, TX_RDS_BUFF_IN_MTBUFF, 0, 0, 0, 0, 0, 0) != FMTX_MODE_OK){
        fmtxCmdStatus=FMTX_MODE_NONE;
        return (fmtxCmdStatus);
    }

    //return if msg is empty (we cleared circular buffer)
    //TODO: check if we must force RT to be whilteline only (so we remove all RT content this way).
    if(!msgChars)
        return (fmtxCmdStatus);

    avaliableBytes = buff[2];
    if (avaliableBytes < 64) {
            logwrite(LOG_ERROR, "RDS: RT: Avaliable only %d chars, but message creates %d chars, please shorter your radio text message!",
                                                    avaliableBytes, 64);
            fmtxCmdStatus=FMTX_MODE_NONE;
            return (fmtxCmdStatus);
    }

    memcpy(msg, messages, msgChars);
    for(i=0;i<16;i++){
            blockPtr = i*4;

            if(fmtxIoAppCommand(TX_RDS_BUFF, buff, 7, TX_RDS_BUFF_IN_LDBUFF, 0x20, i, msg[blockPtr], msg[blockPtr+1], msg[blockPtr+2], msg[blockPtr+3]) != FMTX_MODE_OK){
                fmtxCmdStatus=FMTX_MODE_NONE;
                return (fmtxCmdStatus);
            }

            logwrite(LOG_DEBUG, "RDS: circular: %d/%d, fifo: %d/%d", buff[3], buff[2], buff[5], buff[4]);
    }
    return (fmtxCmdStatus);
}

EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSendTimeStamp()
{
    fmtxCmdStatus=FMTX_MODE_OK;
    uint8_t buff[16];
    uint32_t MJD;
    int y, m, d, k;
    struct tm  *ltm;
    int8_t offset;
    struct timezone tz;
    struct timeval tv;
    gettimeofday( &tv, &tz);

    logwrite(LOG_DUMP, "FMTX: %s", __FUNCTION__);

    ltm=localtime(&tv.tv_sec);
    offset = tz.tz_minuteswest / 30;

    y = ltm->tm_year;
    m = ltm->tm_mon + 1;
    d = ltm->tm_mday;
    k = 0;
    if((m == 1) || (m == 2))
        k = 1;

    MJD = 14956 + d + ((int) ((y - k) * 365.25)) + ((int) ((m + 1 + k * 12) * 30.6001));

    /* this is reversed because we have negative value for eastern of GMT */
    if (offset > 0)
        offset = (abs(offset) & 0x1F) | 0x20;
    else
        offset = abs(offset) & 0x1F;

    /* send timestamp */
    logwrite(LOG_DEBUG, "RDS: local timestamp MJD:%d, %d:%d %s%d/2", MJD, ltm->tm_hour, ltm->tm_min, (offset & 0x20)?"-":"+", offset);
    if(fmtxIoAppCommand(TX_RDS_BUFF, buff, 7, TX_RDS_BUFF_IN_FIFO|TX_RDS_BUFF_IN_LDBUFF, 0x40, MJD >> 15, MJD >> 7, MJD << 1|((ltm->tm_hour & 0x1F)>> 4), ((ltm->tm_hour & 0x1F)<< 4)|((ltm->tm_min & 0x3F)>> 2), ((ltm->tm_min & 0x3F)<< 6)|offset) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}


EXTERN FMTX_API_EXPORT FMTX_API_CALL uint8_t fmtxRDSSendCustomGroup(uint8_t flags, uint8_t B0, uint8_t B1, uint8_t C0, uint8_t C1, uint8_t D0, uint8_t D1)
{
    uint8_t buff[16];
    fmtxCmdStatus=FMTX_MODE_OK;
    if(fmtxIoAppCommand(TX_RDS_BUFF, buff, 7, flags, B0, B1, C0, C1, D0, D1) != FMTX_MODE_OK)
        fmtxCmdStatus=FMTX_MODE_NONE;
    return (fmtxCmdStatus);
}
