/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : audiocodec_si4711.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : This file includes the I2S Codec driver for SI4711
*                       Audio Codec.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include "audio_codec_si4711.h"
#include "stm32_hardware.h"
#include "hw_config.h"

#include "si47xx_commanddefs.h"
#include "si47xx_propertydefs.h"

#include "I2CRoutines.h"

/* Private typedef -----------------------------------------------------------*/
#define SI47XX_ADDR                        0xC6   /*!< SI47XX 0x63 address */

/* Uncomment this line to enable verifying data sent to codec after each write operation */
//#define VERIFY_WRITTENDATA

/* The 7 bits Codec address mask */
//#define CODEC_ADDRESS         0x93  /* b00100111 */
//#define CODEC_ADDRESS         0x63
#define POWERUP_TIME 110    // Powerup delay in milliseconds
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void SI47XX_Powerup(void);
static void si47XX_SetProperty(u16 propNumber, u16 propValue);
static void si47XX_Command(u8 cmd_size, u8 *cmd, u8 reply_size, u8 *reply);

#define CODEC_WriteRegister
#define CODEC_ReadRegister

/*******************************************************************************
* Function Name  : CODEC_Config
* Description    : Configure the Codec in Headphone mode.
* Input          :  - OutputDevice: OutputDeviceHEADPHONE or OutputDeviceSPEAKER
*                :  - I2S_Standard: I2S communication standard could be I2S_Standard_Phillips
*                :       I2S_Standard_MSB or I2S_Standard_LSB.
*                :  - I2S_MCLKOutput: could be I2S_MCLKOutput_
*                :  - Volume:
* Output         : None
* Return         : 0-> correct communication, else wrong communication
*******************************************************************************/
uint32_t CODEC_Config(uint16_t I2S_Standard, uint16_t I2S_MCLKOutput, uint8_t Volume)
{
	/* blah blah blah */
	I2C_LowLevel_Init(I2C1);
	SI47XX_Powerup();
	return 0;
}

static void SI47XX_Powerup(void)
{
   u8 cmd[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   u8 rsp[8];


  /*
   * POWER_UP POWER_UP_IN_FUNC_FMTX POWER_UP_IN_OPMODE_TX_DIGITAL
   * si47XX_SetProperty(GPO_IEN, 0);
   * si47XX_SetProperty(REFCLK_FREQ, 31250);
   * si47XX_SetProperty(REFCLK_PRESCALE, 192);
   * si47XX_SetProperty(TX_PREEMPHASIS, 1);
   * si47XX_SetProperty(TX_PILOT_FREQUENCY, 19000);
   * si47XX_SetProperty(TX_PILOT_DEVIATION, 675);
   * si47XX_SetProperty(TX_AUDIO_DEVIATION, 6825);
   * TX_TUNE_POWER 0x00 0x00 115 0x00
   * TX_TUNE_FREQ 0x00 0x25 0xf8 (9720)
   * * * * * * *
   * si47XX_SetProperty(DIGITAL_INPUT_SAMPLE_RATE, 48000);
   * si47XX_SetProperty(DIGITAL_INPUT_FORMAT, 0);
   * si47XX_SetProperty(TX_ACOMP_ENABLE, 0);
   * si47XX_SetProperty(TX_COMPONENT_ENABLE, TX_COMPONENT_ENABLE_PILOT_MASK|TX_COMPONENT_ENABLE_LMR_MASK);
   *
   * */
  cmd[0] = POWER_UP;
  cmd[1] = POWER_UP_IN_FUNC_FMTX;
  cmd[2] = POWER_UP_IN_OPMODE_TX_DIGITAL;
  si47XX_Command(3, cmd, 8, rsp);

  Delay(POWERUP_TIME);               // wait for si47xx to powerup

  si47XX_SetProperty(GPO_IEN, 0);
  si47XX_SetProperty(REFCLK_FREQ, 31250);
  si47XX_SetProperty(REFCLK_PRESCALE, 192);
  si47XX_SetProperty(TX_PREEMPHASIS, 1);
  si47XX_SetProperty(TX_PILOT_FREQUENCY, 19000);
  si47XX_SetProperty(TX_PILOT_DEVIATION, 675);
  si47XX_SetProperty(TX_AUDIO_DEVIATION, 6825);

  cmd[0] = TX_TUNE_POWER;
  cmd[1] = 0;
  cmd[2] = 0;
  cmd[3] = 115;
  cmd[4] = 0;
  si47XX_Command(5, cmd, 8, rsp);

  Delay(25);

  cmd[0] = TX_TUNE_FREQ;
  cmd[1] = 0;
  cmd[2] = 0x25;
  cmd[3] = 0xf8; // (9720)
  si47XX_Command(4, cmd, 8, rsp);

  Delay(125);

  //si47XX_SetProperty(DIGITAL_INPUT_SAMPLE_RATE, 48000);
  si47XX_SetProperty(DIGITAL_INPUT_FORMAT, 0);
  si47XX_SetProperty(TX_ACOMP_ENABLE, 0);
  si47XX_SetProperty(TX_COMPONENT_ENABLE, TX_COMPONENT_ENABLE_PILOT_MASK|TX_COMPONENT_ENABLE_LMR_MASK);

}

/*******************************************************************************
* Function Name  : CODEC_ControlVolume
* Description    : Controls the audio volume.
* Input          :  - Volume: the volume level to be set.
* Output         : None
* Return         : 0-> correct communication, else wrong communication
*******************************************************************************/
uint32_t CODEC_ControlVolume(uint8_t Volume)
{
  uint32_t counter = 0;

  /* 
     Max  = 0xFF --> 0x18, 
     Mean = 0x7F --> 0x00 
     Min  = 0x00 --> 0x19 
  */
#if 0
  if (Volume > 0xE6)
  {
    /* Set the Master volume */
    counter += CODEC_WriteRegister(0x20, Volume - 0xE7); 
    counter += CODEC_WriteRegister(0x21, Volume - 0xE7);     
  }
  else
  {
    /* Set the Master volume */
    counter += CODEC_WriteRegister(0x20, Volume + 0x19); 
    counter += CODEC_WriteRegister(0x21, Volume + 0x19); 
  }
#endif
  return counter;
}

/*******************************************************************************
* Function Name  : CODEC_Mute
* Description    : Enable or disable the MUTE mode by software
* Input          : - Command: could be MUTE_ON to mute sound or MUTE_OFF to restore volume
* Output         : None.
* Return         : None.
*******************************************************************************/
void CODEC_Mute(uint32_t Command)
{ 
	/* Set the Mute mode */
  if (Command == MUTE_ON)
  {
    //CODEC_WriteRegister(0x0D, 0x63);
    //CODEC_WriteRegister(0x0F, 0xF8);
	si47XX_SetProperty(DIGITAL_INPUT_FORMAT, 0);
    si47XX_SetProperty(DIGITAL_INPUT_SAMPLE_RATE, 0);
    STM_EVAL_LEDOff(LED2);
  }
  else /* MUTE_OFF Disable the Mute */
  {
    //CODEC_WriteRegister(0x0D, 0x60);
    //CODEC_WriteRegister(0x0F, 0x08);
	si47XX_SetProperty(DIGITAL_INPUT_FORMAT, 0);
    si47XX_SetProperty(DIGITAL_INPUT_SAMPLE_RATE, 48000);
    STM_EVAL_LEDOn(LED2);
  }
}

/*******************************************************************************
* Function Name  : CODEC_PowerDown
* Description    : Power down the Audio Codec.
* Input          : - CodecPowerDown_Mode: could be CodecPowerDown_SW for software
*                  power down (by register writing), CodecPowerDown_HW just shut
*                  down the codec physically.
* Output         : None
* Return         : None
*******************************************************************************/
void CODEC_PowerDown(uint32_t CodecPowerDown_Mode)
{
#if 0
  if (CodecPowerDown_Mode == CodecPowerDown_SW)
  {
    /* Power down the DAC and the speaker (PMDAC and PMSPK bits)*/
    CODEC_WriteRegister(0x02, 0x9F);
  }
  else /* CodecPowerDown_HW */
  {
    /* Mute the output first */
    CODEC_WriteRegister(0x1A, 0x80);
    CODEC_WriteRegister(0x1B, 0x80);    
    
    /* Power down the DAC components */
    CODEC_WriteRegister(0x02, 0x9F);
    
    /* Wait at least 100µs */
    Delay(1);
    
    /* Reset The pin */
    //IOE_WriteIOPin(AUDIO_RESET_PIN, BitReset);
  }
#endif
}

/*******************************************************************************
* Function Name  : CODEC_Reset
* Description    : Reset the Audio Codec.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CODEC_Reset(void)
{
  /* Power Down the codec */
  //IOE_WriteIOPin(AUDIO_RESET_PIN, BitReset);

  /* wait for a delay to insure registers erasing */
  Delay(20);
  
  /* Power on the codec */
  //IOE_WriteIOPin(AUDIO_RESET_PIN, BitSet);  
}



//-----------------------------------------------------------------------------
// Helper function that is used to write to the part which abstracts what
// bus mode is currently being used.
//-----------------------------------------------------------------------------
static uint32_t si47xx_lowWrite(u8 number_bytes, u8 *data_out)
{
	return I2C_Master_BufferWrite(I2C1, data_out,  number_bytes, /*Interrupt*/Polling, SI47XX_ADDR);
	//I2C_Master_BufferWrite(I2C1, Buffer_Tx1,120,Interrupt, 0x28);
}

//-----------------------------------------------------------------------------
// Helper function that is used to read from the part which abstracts what
// bus mode is currently being used.
//-----------------------------------------------------------------------------
static uint32_t si47xx_lowRead(u8 number_bytes, u8 *data_in)
{
	return I2C_Master_BufferRead(I2C1, data_in, number_bytes, Polling, SI47XX_ADDR);
	//I2C_Master_BufferRead(I2C1,Buffer_Rx1,120,Polling, 0x28);
}



//-----------------------------------------------------------------------------
// This command returns the status
//-----------------------------------------------------------------------------
static u8 si47xx_readStatus()
{
    u8 status;
    si47xx_lowRead(1, &status);
    return status;
}

//-----------------------------------------------------------------------------
// Command that will wait for CTS before returning
//-----------------------------------------------------------------------------
static void si47xx_waitForCTS()
{
    u16 i=1000;

    // Loop until CTS is found or stop due to the counter running out.
    while (--i && !(si47xx_readStatus() & CTS)) {
    	Delay(1);
    }

    // If the i is equal to 0 then something must have happened.
    // It is recommended that the controller do some type of error
    // handling in this case.
}

//-----------------------------------------------------------------------------
// Sends a command to the part and returns the reply bytes
//-----------------------------------------------------------------------------
static void si47XX_Command(u8 cmd_size, u8 *cmd, u8 reply_size, u8 *reply)
{
    // It is always a good idea to check for cts prior to sending a command to
    // the part.
    si47xx_waitForCTS();

    // Write the command to the part
    si47xx_lowWrite(cmd_size, cmd);

    // Wait for CTS after sending the command
    si47xx_waitForCTS();

    // If the calling function would like to have results then read them.
    if(reply_size)
    {
        si47xx_lowRead(reply_size, reply);
    }
}

//-----------------------------------------------------------------------------
// Helper function that sends the GET_INT_STATUS command to the part
//
// Returns:
//   The status byte from the part.
//-----------------------------------------------------------------------------
static uint32_t getIntStatus(void)
{
	u8 cmd[1] = {GET_INT_STATUS};
	u8 rsp[1] = {0};

    // Invoke the command
	si47XX_Command(1, cmd, 1, rsp);

	// Return the status
	return rsp[0];
}

//-----------------------------------------------------------------------------
// Set the passed property number to the passed value.
//
// Inputs:
//      propNumber:  The number identifying the property to set
//      propValue:   The value of the property.
//-----------------------------------------------------------------------------
static void si47XX_SetProperty(u16 propNumber, u16 propValue)
{
	u8 cmd[6] = {SET_PROPERTY, 0, 0, 0, 0, 0};
	// Put the property number in the third and fourth bytes.
    cmd[2] = (u8)(propNumber >> 8);
	cmd[3] = (u8)(propNumber & 0x00FF);

	// Put the property value in the fifth and sixth bytes.
    cmd[4] = (u8)(propValue >> 8);
    cmd[5] = (u8)(propValue & 0x00FF);

    // Invoke the command
	si47XX_Command(6, cmd, 0, NULL);
}
