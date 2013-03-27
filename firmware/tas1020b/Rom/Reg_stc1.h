//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
/****************************************************************************/
/* file - REG_STC1.H                                                        */
/* Header file for STC                                                      */
/****************************************************************************/
#ifndef _REGSTC1_H
#define _REGSTC1_H

#define stc_sfr(sfr_addr) (*((unsigned char volatile xdata *) sfr_addr))
#define stc_sfr16(sfr_addr) (*((unsigned int volatile xdata *) sfr_addr))
#define stc_sfr_array(sfr_addr) ((unsigned char volatile xdata *) sfr_addr)

// Setup Packet Block
#define SETPACK    stc_sfr_array(0xFF28)
#define SETPACK_ADDRESS 0xFF28

// Global
#define MEMCFG          stc_sfr(0xFFB0)
#define GLOBCTL         stc_sfr(0xFFB1)
#define VECINT          stc_sfr(0xFFB2)
#define IEPINT          stc_sfr(0xFFB3)
#define OEPINT          stc_sfr(0xFFB4)

#define MEMCFG_SDW_BIT  0x01

#define P3MSK            stc_sfr(0xFFCA)

// PWM
//#define PWMPWH     stc_sfr(0xFFBD)
//#define PWMPWL     stc_sfr(0xFFBE)
//#define PWMFRQ     stc_sfr(0xFFBF)

// I^2C-port
#define I2CCTL     stc_sfr(0xFFC0)
#define I2CSTA     stc_sfr(0xFFC0)
#define I2CDATO    stc_sfr(0xFFC1)
#define I2CDATI    stc_sfr(0xFFC2)
#define I2CADR     stc_sfr(0xFFC3)

// C-port
#define CPTVSLH     stc_sfr(0xFFD7)
#define CPTVSLL     stc_sfr(0xFFD8)
#define CPTDATH     stc_sfr(0xFFD9)
#define CPTDATL     stc_sfr(0xFFDA)
#define CPTADR      stc_sfr(0xFFDB)
#define CPTSTA      stc_sfr(0xFFDC)
#define CPTCTL      stc_sfr(0xFFDC)
#define CPTCNF4     stc_sfr(0xFFDD)
#define CPTCNF3     stc_sfr(0xFFDE)
#define CPTCNF2     stc_sfr(0xFFDF)
#define CPTCNF1     stc_sfr(0xFFE0)
#define CPTRXCNF4   stc_sfr(0xFFD4)
#define CPTRXCNF3   stc_sfr(0xFFD5)
#define CPTRXCNF2   stc_sfr(0xFFD6)

// Adaptive Clock Generator
#define ACGCTL     stc_sfr(0xFFE1)
#define ACGDCTL    stc_sfr(0xFFE2)

#define ACGCAPH    stc_sfr(0xFFE3)
#define ACGCAPL    stc_sfr(0xFFE4)

#define ACGFRQ2    stc_sfr(0xFFE5)
#define ACGFRQ1    stc_sfr(0xFFE6)
#define ACGFRQ0    stc_sfr(0xFFE7)

#define ACG2DCTL    stc_sfr(0xFFF6)
#define ACG2FRQ2    stc_sfr(0xFFF7)
#define ACG2FRQ1    stc_sfr(0xFFF8)
#define ACG2FRQ0    stc_sfr(0xFFF9)

// DMA
#define DMACTL0    stc_sfr(0xFFE8)
#define DMATSH0    stc_sfr(0xFFE9)
#define DMATSL0    stc_sfr(0xFFEA)

#define DMABCNT0H	stc_sfr(0xFFEC)
#define DMABCNT0L	stc_sfr(0xFFEB)

#define DMACTL1    stc_sfr(0xFFEE)
#define DMATSH1    stc_sfr(0xFFEF)
#define DMATSL1    stc_sfr(0xFFF0)
#define DMABCNT1H	stc_sfr(0xFFF4)
#define DMABCNT1L	stc_sfr(0xFFF3)

//#define DMACTL2    stc_sfr(0xFFF4)
//#define DMATSH2    stc_sfr(0xFFF5)
//#define DMATSL2    stc_sfr(0xFFF6)
//
//#define DMACTL3    stc_sfr(0xFFF7)
//#define DMATSH3    stc_sfr(0xFFF8)
//#define DMATSL3    stc_sfr(0xFFF9)

// USB
#define USBFNH     stc_sfr(0xFFFA)
#define USBFNL     stc_sfr(0xFFFB)
#define USBCTL     stc_sfr(0xFFFC)
#define USBIMSK    stc_sfr(0xFFFD)
#define USBSTA     stc_sfr(0xFFFE)
#define USBFADR    stc_sfr(0xFFFF)

/* Endpoint Descriptor Block */

// Input Endpoint
// #define IEPCNF     stc_sfr_array(0xFF30)
#define	IEPCNF0   stc_sfr(0xFF68)
#define	IEPCNF1   stc_sfr(0xFF60)
#define	IEPCNF2   stc_sfr(0xFF58)
#define	IEPCNF3   stc_sfr(0xFF50)
#define	IEPCNF4   stc_sfr(0xFF48)
#define	IEPCNF5   stc_sfr(0xFF40)
#define	IEPCNF6   stc_sfr(0xFF38)
#define	IEPCNF7   stc_sfr(0xFF30)

// #define IEPADRX    stc_sfr_array(0xFF38)
#define	IEPBBAX0  stc_sfr(0xFF69)
#define	IEPBBAX1  stc_sfr(0xFF61)
#define	IEPBBAX2  stc_sfr(0xFF59)
#define	IEPBBAX3  stc_sfr(0xFF51)
#define	IEPBBAX4  stc_sfr(0xFF49)
#define	IEPBBAX5  stc_sfr(0xFF41)
#define	IEPBBAX6  stc_sfr(0xFF39)
#define	IEPBBAX7  stc_sfr(0xFF31)

// #define IEPSIZX    stc_sfr_array(0xFF40)
#define	IEPBSIZ0  stc_sfr(0xFF6A)
#define	IEPBSIZ1  stc_sfr(0xFF62)
#define	IEPBSIZ2  stc_sfr(0xFF5A)
#define	IEPBSIZ3  stc_sfr(0xFF52)
#define	IEPBSIZ4  stc_sfr(0xFF4A)
#define	IEPBSIZ5  stc_sfr(0xFF42)
#define	IEPBSIZ6  stc_sfr(0xFF3A)
#define	IEPBSIZ7  stc_sfr(0xFF32)

// #define IEPDCTX    stc_sfr_array(0xFF48)
#define	IEPDCNTX0  stc_sfr(0xFF6B)
#define	IEPDCNTX1  stc_sfr(0xFF63)
#define	IEPDCNTX2  stc_sfr(0xFF5B)
#define	IEPDCNTX3  stc_sfr(0xFF53)
#define	IEPDCNTX4  stc_sfr(0xFF4B)
#define	IEPDCNTX5  stc_sfr(0xFF43)
#define	IEPDCNTX6  stc_sfr(0xFF3B)
#define	IEPDCNTX7  stc_sfr(0xFF33)
                               
// #define IEPADRY    stc_sfr_array(0xFF58)
#define	IEPBBAY0  stc_sfr(0xFF6D)
#define	IEPBBAY1  stc_sfr(0xFF65)
#define	IEPBBAY2  stc_sfr(0xFF5D)
#define	IEPBBAY3  stc_sfr(0xFF55)
#define	IEPBBAY4  stc_sfr(0xFF4D)
#define	IEPBBAY5  stc_sfr(0xFF45)
#define	IEPBBAY6  stc_sfr(0xFF3D)
#define	IEPBBAY7  stc_sfr(0xFF35)
                                 
// #define IEPDCTY    stc_sfr_array(0xFF68)
#define	IEPDCNTY0  stc_sfr(0xFF6F)
#define	IEPDCNTY1  stc_sfr(0xFF67)
#define	IEPDCNTY2  stc_sfr(0xFF5F)
#define	IEPDCNTY3  stc_sfr(0xFF57)
#define	IEPDCNTY4  stc_sfr(0xFF4F)
#define	IEPDCNTY5  stc_sfr(0xFF47)
#define	IEPDCNTY6  stc_sfr(0xFF3F)
#define	IEPDCNTY7  stc_sfr(0xFF37)

// Output Endpoint
// #define OEPCNF     stc_sfr_array(0xFF70)
#define	OEPCNF0   stc_sfr(0xFFA8)
#define	OEPCNF1   stc_sfr(0xFFA0)
#define	OEPCNF2   stc_sfr(0xFF98)
#define	OEPCNF3   stc_sfr(0xFF90)
#define	OEPCNF4   stc_sfr(0xFF88)
#define	OEPCNF5   stc_sfr(0xFF80)
#define	OEPCNF6   stc_sfr(0xFF78)
#define	OEPCNF7   stc_sfr(0xFF70)


// #define OEPADRX    stc_sfr_array(0xFF78)
#define	OEPBBAX0  stc_sfr(0xFFA9)
#define	OEPBBAX1  stc_sfr(0xFFA1)
#define	OEPBBAX2  stc_sfr(0xFF99)
#define	OEPBBAX3  stc_sfr(0xFF91)
#define	OEPBBAX4  stc_sfr(0xFF89)
#define	OEPBBAX5  stc_sfr(0xFF81)
#define	OEPBBAX6  stc_sfr(0xFF79)
#define	OEPBBAX7  stc_sfr(0xFF71)


// #define OEPSIZX    stc_sfr_array(0xFF80)
#define	OEPBSIZ0  stc_sfr(0xFFAA)
#define	OEPBSIZ1  stc_sfr(0xFFA2)
#define	OEPBSIZ2  stc_sfr(0xFF9A)
#define	OEPBSIZ3  stc_sfr(0xFF92)
#define	OEPBSIZ4  stc_sfr(0xFF8A)
#define	OEPBSIZ5  stc_sfr(0xFF82)
#define	OEPBSIZ6  stc_sfr(0xFF7A)
#define	OEPBSIZ7  stc_sfr(0xFF72)


// #define OEPDCTX    stc_sfr_array(0xFF88)
#define	OEPDCNTX0  stc_sfr(0xFFAB)
#define	OEPDCNTX1  stc_sfr(0xFFA3)
#define	OEPDCNTX2  stc_sfr(0xFF9B)
#define	OEPDCNTX3  stc_sfr(0xFF93)
#define	OEPDCNTX4  stc_sfr(0xFF8B)
#define	OEPDCNTX5  stc_sfr(0xFF83)
#define	OEPDCNTX6  stc_sfr(0xFF7B)
#define	OEPDCNTX7  stc_sfr(0xFF73)


// #define OEPADRY    stc_sfr_array(0xFF98)
#define	OEPBBAY0  stc_sfr(0xFFAD)
#define	OEPBBAY1  stc_sfr(0xFFA5)
#define	OEPBBAY2  stc_sfr(0xFF9D)
#define	OEPBBAY3  stc_sfr(0xFF95)
#define	OEPBBAY4  stc_sfr(0xFF8D)
#define	OEPBBAY5  stc_sfr(0xFF85)
#define	OEPBBAY6  stc_sfr(0xFF7D)
#define	OEPBBAY7  stc_sfr(0xFF75)
                                  
// #define OEPDCTY    stc_sfr_array(0xFFA8)
#define	OEPDCNTY0  stc_sfr(0xFFAF)
#define	OEPDCNTY1  stc_sfr(0xFFA7)
#define	OEPDCNTY2  stc_sfr(0xFF9F)
#define	OEPDCNTY3  stc_sfr(0xFF97)
#define	OEPDCNTY4  stc_sfr(0xFF8F)
#define	OEPDCNTY5  stc_sfr(0xFF87)
#define	OEPDCNTY6  stc_sfr(0xFF7F)
#define	OEPDCNTY7  stc_sfr(0xFF77)
                                  
// interrupt source
#define OEP0_INT   0x00
#define OEP1_INT   0x01
#define OEP2_INT   0x02
#define OEP3_INT   0x03
#define OEP4_INT   0x04
#define OEP5_INT   0x05
#define OEP6_INT   0x06
#define OEP7_INT   0x07
#define IEP0_INT   0x08
#define IEP1_INT   0x09
#define IEP2_INT   0x0A
#define IEP3_INT   0x0B
#define IEP4_INT   0x0C
#define IEP5_INT   0x0D
#define IEP6_INT   0x0E
#define IEP7_INT   0x0F

#define STPOW_INT  0x10
// reserved        0x11
#define SETUP_INT  0x12
#define PSOF_INT   0x13
#define SOF_INT    0x14
#define RESR_INT   0x15
#define SUSR_INT   0x16
#define RSTR_INT   0x17

#define CPRX_INT   0x18
#define CPTX_INT   0x19
#define DPRX_INT   0x1A
#define DPTX_INT   0x1B
#define I2CRX_INT  0x1C
#define I2CTX_INT  0x1D

// reserved        0x1E
#define XINT_INT   0x1F

#define NO_INT     0x24

#endif
