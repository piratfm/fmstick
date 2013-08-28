/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : audiocodec_si4711.h
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : This file contains all the functions prototypes for the
*                      SI4711 codec firmware driver.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUDIOCODEC_SI4711_H
#define __AUDIOCODEC_SI4711_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Codec output DEVICE */
#define OutputDevice_SPEAKER          1
#define OutputDevice_HEADPHONE        2
#define OutputDevice_BOTH             3
#define OutputDevice_AUTO             4

/* Volume Levels values */
#define DEFAULT_VOLMIN                0x00
#define DEFAULT_VOLMAX                0xFF
#define DEFAULT_VOLSTEP               0x04

/* Codec POWER DOWN modes */
#define CodecPowerDown_HW             1
#define CodecPowerDown_SW             2

/* MUTE commands */
#define MUTE_ON                       1
#define MUTE_OFF                      0

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

uint32_t CODEC_Config(uint16_t I2S_Standard, uint16_t I2S_MCLKOutput, uint8_t Volume);
uint32_t CODEC_Run();
uint32_t CODEC_ControlVolume(uint8_t Volume);
void CODEC_Mute(uint32_t Command);
void CODEC_Reset(void);
void CODEC_PowerDown(uint32_t CodecPowerDown_Mode);

/* Low Layer Codec Function --------------------------------------------------*/
uint32_t CODEC_WriteRegister(uint32_t RegisterAddr, uint32_t RegisterValue);
uint32_t CODEC_ReadRegister(uint32_t RegisterAddr);

#endif /* __AUDIOCODEC_SI4711_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
