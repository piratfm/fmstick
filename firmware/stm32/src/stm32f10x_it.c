/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "hw_config.h"
#include "I2CRoutines.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DUMMYDATA  0x1111

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define SLAVE_DMA_USE
/* Private macro -------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t Tx_Idx1=0, Rx_Idx1=0;
__IO uint8_t Tx_Idx2=0, Rx_Idx2=0;
extern __IO uint32_t NumbOfBytes1;
extern __IO  uint32_t NumbOfBytes2;
extern uint8_t Buffer_Rx1[];
extern uint8_t Buffer_Tx1[];
extern uint8_t Buffer_Rx2[];
extern uint8_t Buffer_Tx2[];

extern __IO uint32_t I2CDirection ;
extern uint8_t Address;


/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval : None
  */
void I2C1_EV_IRQHandler(void)
{

    __IO uint32_t SR1Register =0;
    __IO uint32_t SR2Register =0;



#ifdef SLAVE_DMA_USE
    /* Read SR1 register */
    SR1Register = I2C1->SR1;

    /* If ADDR is set */
    if ((SR1Register & 0x0002) == 0x0002)
    {
        /* In slave Transmitter/Receiver mode, when using DMA, it is recommended to update the buffer
          base address and the buffer size before clearing ADDR flag. In fact, the only
          period when the slave has control  on the bus(SCL is stretched so master can not initiate
          transfers) is the period between ADDR is set and ADDR is cleared. Otherwise, the master can
          initiate transfers and the buffer size & the buffer address have not yet been updated.*/

        /* Update the DMA channels memory base address and count */
        I2C_DMAConfig (I2C1, Buffer_Tx1, 0xFFFF, I2C_DIRECTION_TX);
        I2C_DMAConfig (I2C1, Buffer_Rx1, 0xFFFF, I2C_DIRECTION_RX);
        /* Clear ADDR by reading SR2 register */
        SR2Register = I2C1->SR2;
    }
#else
    /* Read the I2C1 SR1 and SR2 status registers */
    SR1Register = I2C1->SR1;
    SR2Register = I2C1->SR2;

    /* If I2C1 is slave (MSL flag = 0) */
    if ((SR2Register &0x0001) != 0x0001)
    {
        /* If ADDR = 1: EV1 */
        if ((SR1Register & 0x0002) == 0x0002)
        {
            /* Clear SR1Register and SR2Register variables to prepare for next IT */
            SR1Register = 0;
            SR2Register = 0;
            /* Initialize the transmit/receive counters for next transmission/reception
            using Interrupt  */
            Tx_Idx1 = 0;
            Rx_Idx1 = 0;
        }
        /* If TXE = 1: EV3 */
        if ((SR1Register & 0x0080) == 0x0080)
        {
            /* Write data in data register */
            I2C1->DR = Buffer_Tx1[Tx_Idx1++];
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If RXNE = 1: EV2 */
        if ((SR1Register & 0x0040) == 0x0040)
        {
            /* Read data from data register */
            Buffer_Rx1[Rx_Idx1++] = I2C1->DR;
            SR1Register = 0;
            SR2Register = 0;

        }
        /* If STOPF =1: EV4 (Slave has detected a STOP condition on the bus */
        if (( SR1Register & 0x0010) == 0x0010)
        {
            I2C1->CR1 |= CR1_PE_Set;
            SR1Register = 0;
            SR2Register = 0;

        }
    } /* End slave mode */

#endif

    /* If SB = 1, I2C1 master sent a START on the bus: EV5) */
    if ((SR1Register &0x0001) == 0x0001)
    {

        /* Send the slave address for transmssion or for reception (according to the configured value
            in the write master write routine */
        I2C1->DR = Address;
        SR1Register = 0;
        SR2Register = 0;
    }
    /* If I2C1 is Master (MSL flag = 1) */

    if ((SR2Register &0x0001) == 0x0001)
    {
        /* If ADDR = 1, EV6 */
        if ((SR1Register &0x0002) == 0x0002)
        {
            /* Write the first data in case the Master is Transmitter */
            if (I2CDirection == I2C_DIRECTION_TX)
            {
                /* Initialize the Transmit counter */
                Tx_Idx1 = 0;
                /* Write the first data in the data register */
                I2C1->DR = Buffer_Tx1[Tx_Idx1++];
                /* Decrement the number of bytes to be written */
                NumbOfBytes1--;
                /* If no further data to be sent, disable the I2C BUF IT
                in order to not have a TxE  interrupt */
                if (NumbOfBytes1 == 0)
                {
                    I2C1->CR2 &= (uint16_t)~I2C_IT_BUF;
                }

            }
            /* Master Receiver */
            else

            {
                /* Initialize Receive counter */
                Rx_Idx1 = 0;
                /* At this stage, ADDR is cleared because both SR1 and SR2 were read.*/
                /* EV6_1: used for single byte reception. The ACK disable and the STOP
                Programming should be done just after ADDR is cleared. */
                if (NumbOfBytes1 == 1)
                {
                    /* Clear ACK */
                    I2C1->CR1 &= CR1_ACK_Reset;
                    /* Program the STOP */
                    I2C1->CR1 |= CR1_STOP_Set;
                }
            }
            SR1Register = 0;
            SR2Register = 0;

        }
        /* Master transmits the remaing data: from data2 until the last one.  */
        /* If TXE is set */
        if ((SR1Register &0x0084) == 0x0080)
        {
            /* If there is still data to write */
            if (NumbOfBytes1!=0)
            {
                /* Write the data in DR register */
                I2C1->DR = Buffer_Tx1[Tx_Idx1++];
                /* Decrment the number of data to be written */
                NumbOfBytes1--;
                /* If  no data remains to write, disable the BUF IT in order
                to not have again a TxE interrupt. */
                if (NumbOfBytes1 == 0)
                {
                    /* Disable the BUF IT */
                    I2C1->CR2 &= (uint16_t)~I2C_IT_BUF;
                }
            }
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If BTF and TXE are set (EV8_2), program the STOP */
        if ((SR1Register &0x0084) == 0x0084)
        {

            /* Program the STOP */
            I2C1->CR1 |= CR1_STOP_Set;
            /* Disable EVT IT In order to not have again a BTF IT */
            I2C1->CR2 &= (uint16_t)~I2C_IT_EVT;
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If RXNE is set */
        if ((SR1Register &0x0040) == 0x0040)
        {
            /* Read the data register */
            Buffer_Rx1[Rx_Idx1++] = I2C1->DR;
            /* Decrement the number of bytes to be read */
            NumbOfBytes1--;
            /* If it remains only one byte to read, disable ACK and program the STOP (EV7_1) */
            if (NumbOfBytes1 == 1)
            {
                /* Clear ACK */
                I2C1->CR1 &= CR1_ACK_Reset;
                /* Program the STOP */
                I2C1->CR1 |= CR1_STOP_Set;
            }
            SR1Register = 0;
            SR2Register = 0;
        }

    }


}

/**
  * @}
  */

/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval : None
  */
void I2C2_EV_IRQHandler(void)
{

    __IO uint32_t SR1Register =0;
    __IO uint32_t SR2Register =0;


#ifdef SLAVE_DMA_USE

    /* Read SR1 register */
    SR1Register = I2C2->SR1;

    /* If ADDR is set */
    if ((SR1Register & 0x0002) == 0x0002)
    {
        /* In slave Transmitter/Receiver mode, when using DMA, the update of the buffer base address
          and the buffer size should be done before clearing ADDR flag. In fact, the only
          period when the slave has control  on the bus(SCL is stretched so master can not initiate
          transfers) is the period between ADDR is set and ADDR is cleared. otherwise, the master can
          initiate transfers and the buffer size & the buffer address have not yet updated.*/

        /* Update the DMA channels memory base address and count */
        I2C_DMAConfig (I2C2, Buffer_Tx2, 0xFFFF, I2C_DIRECTION_TX);
        I2C_DMAConfig (I2C2, Buffer_Rx2, 0xFFFF, I2C_DIRECTION_RX);
        /* Clear ADDR by reading SR2 register */
        SR2Register = I2C2->SR2;
    }
#else
    /* Read the I2C1 SR1 and SR2 status registers */
    SR1Register = I2C2->SR1;
    SR2Register = I2C2->SR2;

    /* If I2C2 is slave (MSL flag = 0) */
    if ((SR2Register &0x0001) != 0x0001)
    {
        /* If ADDR = 1: EV1 */
        if ((SR1Register & 0x0002) == 0x0002)
        {
            /* Clear SR1Register SR2Register variables to prepare for next IT*/
            SR1Register = 0;
            SR2Register = 0;
            /* Initialize the transmit/receive counters for next transmission/reception
            using Interrupt  */
            Tx_Idx2 = 0;
            Rx_Idx2 = 0;
        }
        /* If TXE = 1: EV3 */
        if ((SR1Register & 0x0080) == 0x0080)
        {
            /* Write data in data register */
            I2C2->DR = Buffer_Tx2[Tx_Idx2++];
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If RXNE = 1: EV2 */
        if ((SR1Register & 0x0040) == 0x0040)
        {
            /* Read data from data register */
            Buffer_Rx2[Rx_Idx2++] = I2C2->DR;
            SR1Register = 0;
            SR2Register = 0;

        }
        /* If STOPF =1: EV4 (Slave has detected a STOP condition on the bus */
        if (( SR1Register & 0x0010) == 0x0010)
        {
            I2C2->CR1 |= CR1_PE_Set;
            SR1Register = 0;
            SR2Register = 0;
        }
    } /* End slave mode */

#endif

    /* If SB = 1, I2C1 master sent a START on the bus: EV5) */
    if ((SR1Register &0x0001) == 0x0001)
    {

        /* Send the slave address for transmssion or for reception (according to the configured value
            in the write master write routine */
        I2C2->DR = Address;
        SR1Register = 0;
        SR2Register = 0;
    }
    /* If I2C2 is Master (MSL flag = 1) */

    if ((SR2Register &0x0001) == 0x0001)
    {
        /* If ADDR = 1, EV6 */
        if ((SR1Register &0x0002) == 0x0002)
        {
            /* Write the first data in case the Master is Transmitter */
            if (I2CDirection == I2C_DIRECTION_TX)
            {
                /* Initialize the Transmit counter */
                Tx_Idx2 = 0;
                /* Write the first data in the data register */
                I2C2->DR = Buffer_Tx1[Tx_Idx2++];
                /* Decrement the number of bytes to be written */
                NumbOfBytes2--;
                /* If no further data to be sent, disable the I2C BUF IT
                in order to not have a TxE  interrupt */
                if (NumbOfBytes2 == 0)
                {
                    I2C2->CR2 &= (uint16_t)~I2C_IT_BUF;
                }

            }
            /* Master Receiver */
            else

            {
                /* Initialize Receive counter */
                Rx_Idx2 = 0;
                /* At this stage, ADDR is cleared because both SR1 and SR2 were read.*/
                /* EV6_1: used for single byte reception. The ACK disable and the STOP
                Programming should be done just after ADDR is cleared. */
                if (NumbOfBytes2 == 1)
                {
                    /* Clear ACK */
                    I2C2->CR1 &= CR1_ACK_Reset;
                    /* Program the STOP */
                    I2C2->CR1 |= CR1_STOP_Set;
                }
            }
            SR1Register = 0;
            SR2Register = 0;

        }
        /* Master transmits the remaing data: from data2 until the last one.  */
        /* If TXE is set */
        if ((SR1Register &0x0084) == 0x0080)
        {
            /* If there is still data to write */
            if (NumbOfBytes2!=0)
            {
                /* Write the data in DR register */
                I2C2->DR = Buffer_Tx2[Tx_Idx2++];
                /* Decrment the number of data to be written */
                NumbOfBytes2--;
                /* If  no data remains to write, disable the BUF IT in order
                to not have again a TxE interrupt. */
                if (NumbOfBytes2 == 0)
                {
                    /* Disable the BUF IT */
                    I2C2->CR2 &= (uint16_t)~I2C_IT_BUF;
                }
            }
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If BTF and TXE are set (EV8_2), program the STOP */
        if ((SR1Register &0x0084) == 0x0084)
        {

            /* Program the STOP */
            I2C2->CR1 |= CR1_STOP_Set;
            /* Disable EVT IT In order to not have again a BTF IT */
            I2C2->CR2 &= (uint16_t)~I2C_IT_EVT;
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If RXNE is set */
        if ((SR1Register &0x0040) == 0x0040)
        {
            /* Read the data register */
            Buffer_Rx2[Rx_Idx2++] = I2C2->DR;
            /* Decrement the number of bytes to be read */
            NumbOfBytes2--;

            /* If it remains only one byte to read, disable ACK and program the STOP (EV7_1) */
            if (NumbOfBytes2 == 1)
            {
                /* Clear ACK */
                I2C2->CR1 &= CR1_ACK_Reset;
                /* Program the STOP */
                I2C2->CR1 |= CR1_STOP_Set;
            }
            SR1Register = 0;
            SR2Register = 0;
        }



    }


}
/**
  * @}
  */

/**
  * @brief  This function handles I2C2 Error interrupt request.
  * @param  None
  * @retval : None
  */
void I2C2_ER_IRQHandler(void)
{

    __IO uint32_t SR1Register =0;

    /* Read the I2C1 status register */
    SR1Register = I2C2->SR1;
    /* If AF = 1 */
    if ((SR1Register & 0x0400) == 0x0400)
    {
        I2C2->SR1 &= 0xFBFF;
        SR1Register = 0;
    }
    /* If ARLO = 1 */
    if ((SR1Register & 0x0200) == 0x0200)
    {
        I2C2->SR1 &= 0xFBFF;
        SR1Register = 0;
    }
    /* If BERR = 1 */
    if ((SR1Register & 0x0100) == 0x0100)
    {
        I2C2->SR1 &= 0xFEFF;
        SR1Register = 0;
    }

    /* If OVR = 1 */

    if ((SR1Register & 0x0800) == 0x0800)
    {
        I2C2->SR1 &= 0xF7FF;
        SR1Register = 0;
    }
}



/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval : None
  */
void I2C1_ER_IRQHandler(void)
{

    __IO uint32_t SR1Register =0;

    /* Read the I2C1 status register */
    SR1Register = I2C1->SR1;
    /* If AF = 1 */
    if ((SR1Register & 0x0400) == 0x0400)
    {
        I2C1->SR1 &= 0xFBFF;
        SR1Register = 0;
    }
    /* If ARLO = 1 */
    if ((SR1Register & 0x0200) == 0x0200)
    {
        I2C1->SR1 &= 0xFBFF;
        SR1Register = 0;
    }
    /* If BERR = 1 */
    if ((SR1Register & 0x0100) == 0x0100)
    {
        I2C1->SR1 &= 0xFEFF;
        SR1Register = 0;
    }

    /* If OVR = 1 */

    if ((SR1Register & 0x0800) == 0x0800)
    {
        I2C1->SR1 &= 0xF7FF;
        SR1Register = 0;
    }
}

/**
  * @}
  */


/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
  /* Proceed for the I2S TXE interrupt */
  if ((SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET))
  {
    SPI_I2S_SendData(SPI2, DUMMYDATA);
  }
}

#ifdef STM32F10X_CL
/*******************************************************************************
* Function Name  : OTG_FS_IRQHandler
* Description    : This function handles USB-On-The-Go FS global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void OTG_FS_IRQHandler(void)
{
  STM32_PCD_OTG_ISR_Handler(); 
}
#endif /* STM32F10X_CL */

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
