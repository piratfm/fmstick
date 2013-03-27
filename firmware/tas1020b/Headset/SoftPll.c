//================================================== 
// Texas Instruments Strictly Private 
// Copyright 2001, Texas Instruments Inc. 
//==================================================
/*================================================== 
Softpll.c: routines to handle ACG soft PLL
//================================================*/
#include <Reg52.h>
#include "..\rom\types.h"
#include "..\rom\Reg_stc1.h"
#include "..\rom\usbdfu.h"
#include "usbapp.h"
#include "devDesc.h"
#include "device.h"
#include "Softpll.h"

#define ACGFRQ2_VALUE(s)            (byte)((s) >> 16)
#define ACGFRQ1_VALUE(s)            (byte)((s) >> 8)
#define ACGFRQ0_VALUE(s)            (byte)((s))

#define SOF_EXPECTED_DELTA          11289
#define SOF_EXPECTED_REMAINDER      600
#define ACG_N                       0x006A4B20
#define ACGDCTL_VALUE               0x10
#define ACGCTL_VALUE                0x44
#define UPDATE_PERIOD               4

#define SOF_MAX_DELTA       (SOF_EXPECTED_DELTA + (SOF_EXPECTED_DELTA / 5))
#define SOF_MIN_DELTA       (SOF_EXPECTED_DELTA - (SOF_EXPECTED_DELTA / 5))

// Globals Definiton
bit SoftAcgFirstTime;
WORD idata SoftRefAccumulator;
WORD idata SoftRefRemainder;
WORD idata SoftRefQuotient;
WORD idata SoftAcgCap1;
WORD idata SoftAcgCap2;
WORD idata SoftAcgAcc;
WORD idata SoftFrqDAcgAcc;
WORD idata SoftErrDelta;
byte SoftFreqUpdateCount;
byte SoftDefFredUpdateCount;
WORD idata OldSoftAcgAcc;

/*====================================================================
softPllInit(): USB ACG initialization
=====================================================================*/
void softPllInit()  
{   
    SoftFreqUpdateCount = 0;
    SoftRefAccumulator = 0;
    SoftAcgFirstTime = 1;

 	ACGDCTL = ACGDCTL_VALUE; 	
    ACGCTL = ACGCTL_VALUE;

    ACGFRQ0 = ACGFRQ0_VALUE(ACG_N);    
    ACGFRQ1 = ACGFRQ1_VALUE(ACG_N);
    ACGFRQ2 = ACGFRQ2_VALUE(ACG_N);

	SoftFrqDAcgAcc = MAKE_WORD(ACGFRQ1_VALUE(ACG_N), ACGFRQ0_VALUE(ACG_N));

    OldSoftAcgAcc = 0;
 
}

/*====================================================================
softPll(): ISR for Start of Frame which is used for 
the synchronous mode
=====================================================================*/
void softPll() 
{
    SoftAcgCap2 = SoftAcgCap1;
    
    LOW(SoftAcgCap1)  = ACGCAPL; // read capture register 
    HIGH(SoftAcgCap1) = ACGCAPH;
    
    if (SoftAcgFirstTime) 
    {	
        // make sure SoftAcgCap2 hold last acgcap reg value
        SoftAcgFirstTime=0;
        SoftAcgCap2=SoftAcgCap1;
        SoftErrDelta = 0;
    }
    else 
    {
		SoftErrDelta = SoftAcgCap1 - SoftAcgCap2;
  

        //
        // Adjust for possible missed SOFs
        //

        while (SoftErrDelta > SOF_MAX_DELTA || SoftErrDelta < SOF_MIN_DELTA)
        {
            SoftErrDelta -= SOF_EXPECTED_DELTA;

	        SoftRefAccumulator += SOF_EXPECTED_REMAINDER;

	        if (SoftRefAccumulator > 1000)
	        {
	            SoftRefAccumulator -= 1000;
	            SoftErrDelta--;
	        }
 
            SoftFreqUpdateCount++;
        }
        
        SoftRefAccumulator += SOF_EXPECTED_REMAINDER;

        if (SoftRefAccumulator > 1000)
        {
            //
            // The expected delta will be one greater than usual, so will
            // account for this by subtracting 1 from the Error delta value.
            //
            
            SoftRefAccumulator-=1000;
            SoftErrDelta--;
        }

    }


    //
    // Accumulate ACG error
    //

	SoftAcgAcc = SoftAcgAcc + SOF_EXPECTED_DELTA - SoftErrDelta;    
   
    SoftFreqUpdateCount++;

    if (SoftFreqUpdateCount >= UPDATE_PERIOD)
    {
        SoftFreqUpdateCount = 0;

	  	SoftFrqDAcgAcc = SoftFrqDAcgAcc - (((SoftAcgAcc >> 8) + (SoftAcgAcc - OldSoftAcgAcc)) << 1);

        //
        // Adjust only the 16 MSb of the fraction
        //

		ACGFRQ2 = (ACGFRQ2_VALUE(ACG_N) & 0xFC) | MSB(SoftFrqDAcgAcc) >> 6;
		ACGFRQ1 = MSB(SoftFrqDAcgAcc << 2);
		ACGFRQ0 = LSB(SoftFrqDAcgAcc << 2);

		OldSoftAcgAcc = SoftAcgAcc;
    }  

	// Check if host drops part of a sample
    if (AppDevice.spkIf.curSetting != DEV_SPK_SETTING_0)
    {
        SoftAcgCap2 = DMABCNT0L | (DMABCNT0H << 8);
    	if (SoftAcgCap2 % DEV_NUM_BYTE_PER_SAMPLE )
    	{
    		// Alligning the UBM and DMA pointers
    		DMACTL0 = 0; // non-integral, so reset pointers
    		DMACTL0 = 0x81;
    	}
    }

}
