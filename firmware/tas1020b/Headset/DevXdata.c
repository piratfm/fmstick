//================================================== 
// Texas Instruments Strictly Private 
// Copyright 2000, Texas Instruments Inc. 
//================================================== 
#include "..\rom\types.h"
#include "..\rom\reg_stc1.h"
#include "..\rom\mmap.h"
#include "..\rom\usbeng.h"
#include "..\rom\usbdfu.h"
#include "usbapp.h"
#include "DevDesc.h"
#include "DevXdata.h"

//
// 8 byte aligned USB endpoint buffers.
//
ALIGNED_XDATA_BUFFERS xdata Xdata _at_ ALIGNED_XDATA_BUFFERS_ADDR;


