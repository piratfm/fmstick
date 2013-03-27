//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
#ifndef _CODEC_H_
#define _CODEC_H_

#define CODEC_PORT_ENABLE  	0x01

void coInitCodec();
void coSpkUpdate();
void coColdReset();

extern bit coCodecInited;


#endif
