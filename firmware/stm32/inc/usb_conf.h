/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : usb_conf.h
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Mass Storage Demo configuration  header
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H

/*------------------------------------------------------------------------------
                    Select the audio frequency 
   WARNING:
   Before selecting the audio frequency, make sure the configuration of the PLLs
   allows acceptable accuracy for both the I2S clock and the USB clock !
   PLLs are configured to have optimum accuracy on the following audio frequencies.
------------------------------------------------------------------------------*/

/* PLLs in this demo are well configured for all these frequencies */
#ifdef EXTERNAL_CRYSTAL_14_7456MHz
 //#define AUDIO_FREQ_96K
 #define AUDIO_FREQ_48K
 //#define AUDIO_FREQ_32K 
 //#define AUDIO_FREQ_16K
 //#define AUDIO_FREQ_8K
#endif /* EXTERNAL_CRYSTAL_14_7456MHz */

/* PLLs in this demo are well configured for all these frequencies */
#ifdef EXTERNAL_CRYSTAL_25MHz             
 #define AUDIO_FREQ_16K
 //#define AUDIO_FREQ_8K
#endif /* EXTERNAL_CRYSTAL_25MHz */ 

/* These frequencies lead to non integer number of data per frame --> low quality */
//#define AUDIO_FREQ_44K
//#define AUDIO_FREQ_22K
//#define AUDIO_FREQ_11K

/*----------------------------------------------------------------------------*/

/* Size of the receive Buffer = NumberOfSamplesPerSecond x 2 x 2 (Stereo, 16bits) 
    Do not modify these lines ! */
#ifdef AUDIO_FREQ_96K
 #define ISOC_BUFFER_SZE  (uint32_t)(2*2*96)
#elif defined AUDIO_FREQ_48K
 #define ISOC_BUFFER_SZE  (uint32_t)(2*2*48)
#elif defined AUDIO_FREQ_44K
 #define ISOC_BUFFER_SZE  (uint32_t)(2*2*44)
#elif defined AUDIO_FREQ_32K
 #define ISOC_BUFFER_SZE  (uint32_t)(2*2*32)
#elif defined AUDIO_FREQ_22K
 #define ISOC_BUFFER_SZE  (uint32_t)(2*2*22)
#elif defined AUDIO_FREQ_16K
 #define ISOC_BUFFER_SZE  (uint32_t)(2*2*16)
#elif defined AUDIO_FREQ_11K
 #define ISOC_BUFFER_SZE  (uint32_t)(2*2*11)
#elif defined AUDIO_FREQ_8K
 #define ISOC_BUFFER_SZE  (uint32_t)(2*2*8)
#endif

/*------------------------------------------------------------------------------
               Select the number of sub-buffers (even number)
       The choice of this value depends on the accuracy of the I2S clock
                  and the availability of the RAM space.
------------------------------------------------------------------------------*/
//#define NUM_SUB_BUFFERS             4             /* Min value */
//#define NUM_SUB_BUFFERS             8
//#define NUM_SUB_BUFFERS             16
//#define NUM_SUB_BUFFERS             32
//#define NUM_SUB_BUFFERS             64
#define NUM_SUB_BUFFERS             100
//#define NUM_SUB_BUFFERS             128
//#define NUM_SUB_BUFFERS             200
//#define NUM_SUB_BUFFERS             192
//#define NUM_SUB_BUFFERS             256
//#define NUM_SUB_BUFFERS             300           
/*----------------------------------------------------------------------------*/

/* Define for the audio volume output */
#define AUDIO_STREAMING_VOLUME        0xB8

/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/

#define EP_NUM              (2)

#ifndef STM32F10X_CL
/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x10)
#define ENDP0_TXADDR        (0x50)

/* EP1  */
/* buffer base address */
#define ENDP1_BUF0Addr      (0x90)
#define ENDP1_BUF1Addr      (0xC0)

/*-------------------------------------------------------------*/
/* -------------------   ISTR events  -------------------------*/
/*-------------------------------------------------------------*/
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */
#define IMR_MSK (CNTR_CTRM  | CNTR_SOFM  | CNTR_RESETM )

/*#define CTR_CALLBACK*/
/*#define DOVR_CALLBACK*/
/*#define ERR_CALLBACK*/
/*#define WKUP_CALLBACK*/
/*#define SUSP_CALLBACK*/
/*#define RESET_CALLBACK*/
#define SOF_CALLBACK
/*#define ESOF_CALLBACK*/

#endif /* STM32F10X_CL */

#ifdef STM32F10X_CL
/*******************************************************************************
*                              FIFO Size Configuration
*  
*  (i) Dedicated data FIFO SPRAM of 1.25 Kbytes = 1280 bytes = 320 32-bits words
*      available for the endpoints IN and OUT.
*      Device mode features:
*      -1 bidirectional CTRL EP 0
*      -3 IN EPs to support any kind of Bulk, Interrupt or Isochronous transfer
*      -3 OUT EPs to support any kind of Bulk, Interrupt or Isochronous transfer
*
*  ii) Receive data FIFO size = RAM for setup packets + 
*                   OUT endpoint control information +
*                   data OUT packets + miscellaneous
*      Space = ONE 32-bits words
*     --> RAM for setup packets = 4 * n + 6 space
*        (n is the nbr of CTRL EPs the device core supports) 
*     --> OUT EP CTRL info      = 1 space
*        (one space for status information written to the FIFO along with each 
*        received packet)
*     --> data OUT packets      = (Largest Packet Size / 4) + 1 spaces 
*        (MINIMUM to receive packets)
*     --> OR data OUT packets  = at least 2*(Largest Packet Size / 4) + 1 spaces 
*        (if high-bandwidth EP is enabled or multiple isochronous EPs)
*     --> miscellaneous = 1 space per OUT EP
*        (one space for transfer complete status information also pushed to the 
*        FIFO with each endpoint's last packet)
*
*  (iii)MINIMUM RAM space required for each IN EP Tx FIFO = MAX packet size for 
*       that particular IN EP. More space allocated in the IN EP Tx FIFO results
*       in a better performance on the USB and can hide latencies on the AHB.
*
*  (iv) TXn min size = 16 words. (n  : Transmit FIFO index)
*   (v) When a TxFIFO is not used, the Configuration should be as follows: 
*       case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
*       --> Txm can use the space allocated for Txn.
*       case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
*       --> Txn should be configured with the minimum space of 16 words
*  (vi) The FIFO is used optimally when used TxFIFOs are allocated in the top 
*       of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
*******************************************************************************/

#define RX_FIFO_SIZE                          128
#define TX0_FIFO_SIZE                          64
#define TX1_FIFO_SIZE                          64
#define TX2_FIFO_SIZE                          16
#define TX3_FIFO_SIZE                          16

/* OTGD-FS-DEVICE IP interrupts Enable definitions */
/* Uncomment the define to enable the selected interrupt */
//#define INTR_MODEMISMATCH
#define INTR_SOFINTR
#define INTR_RXSTSQLVL           /* Mandatory */
//#define INTR_NPTXFEMPTY
//#define INTR_GINNAKEFF
//#define INTR_GOUTNAKEFF
//#define INTR_ERLYSUSPEND
#define INTR_USBSUSPEND          /* Mandatory */
#define INTR_USBRESET            /* Mandatory */
#define INTR_ENUMDONE            /* Mandatory */
//#define INTR_ISOOUTDROP
//#define INTR_EOPFRAME
//#define INTR_EPMISMATCH
#define INTR_INEPINTR            /* Mandatory */
#define INTR_OUTEPINTR           /* Mandatory */
//#define INTR_INCOMPLISOIN
//#define INTR_INCOMPLISOOUT
#define INTR_WKUPINTR            /* Mandatory */

/* OTGD-FS-DEVICE IP interrupts subroutines */
/* Comment the define to enable the selected interrupt subroutine and replace it
   by user code */
#define  INTR_MODEMISMATCH_Callback      NOP_Process
/* #define  INTR_SOFINTR_Callback           NOP_Process */
#define  INTR_RXSTSQLVL_Callback         NOP_Process
#define  INTR_NPTXFEMPTY_Callback        NOP_Process
#define  INTR_NPTXFEMPTY_Callback        NOP_Process
#define  INTR_GINNAKEFF_Callback         NOP_Process
#define  INTR_GOUTNAKEFF_Callback        NOP_Process
#define  INTR_ERLYSUSPEND_Callback       NOP_Process
#define  INTR_USBSUSPEND_Callback        NOP_Process
#define  INTR_USBRESET_Callback          NOP_Process
#define  INTR_ENUMDONE_Callback          NOP_Process
#define  INTR_ISOOUTDROP_Callback        NOP_Process
#define  INTR_EOPFRAME_Callback          NOP_Process
#define  INTR_EPMISMATCH_Callback        NOP_Process
#define  INTR_INEPINTR_Callback          NOP_Process
#define  INTR_OUTEPINTR_Callback         NOP_Process
#define  INTR_INCOMPLISOIN_Callback      NOP_Process
#define  INTR_INCOMPLISOOUT_Callback     NOP_Process
#define  INTR_WKUPINTR_Callback          NOP_Process

/* Isochronous data update */
/* #define  INTR_RXSTSQLVL_ISODU_Callback   NOP_Process */

#endif /* STM32F10X_CL */

/* CTR service routines */
/* associated to defined endpoints */
#define  EP1_IN_Callback   NOP_Process
#define  EP2_IN_Callback   NOP_Process
#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process

/*#define  EP1_OUT_Callback   NOP_Process*/
#define  EP2_OUT_Callback   NOP_Process
#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process


#endif /* __USB_CONF_H */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
