// This file contains the command definitions for the Si47xx Parts.
#ifndef _COMMAND_DEFS_H_
#define _COMMAND_DEFS_H_

//==================================================================
// General Commands
//==================================================================

// STATUS bits - Used by all methods
#define STCINT  0x01
#define ASQINT  0x02
#define RDSINT  0x04
#define RSQINT  0x08
#define ERR     0x40
#define CTS     0x80

// POWER_UP
#define POWER_UP                      0x01
#define POWER_UP_IN_FUNC_FMRX         0x00
#define POWER_UP_IN_FUNC_AMRX         0x01
#define POWER_UP_IN_FUNC_FMTX         0x02
#define POWER_UP_IN_FUNC_WBRX         0x03
#define POWER_UP_IN_FUNC_QUERY        0x0F
#define POWER_UP_IN_PATCH             0x20
#define POWER_UP_IN_GPO2OEN           0x40
#define POWER_UP_IN_CTSIEN            0x80
#define POWER_UP_IN_OPMODE_RX_ANALOG  0x05
#define POWER_UP_IN_OPMODE_TX_ANALOG  0x50

// GET_REV
#define GET_REV 0x10

// POWER_DOWN
#define POWER_DOWN 0x11

// SET_PROPERTY
#define SET_PROPERTY 0x12

// GET_PROPERTY
#define GET_PROPERTY 0x13

// GET_INT_STATUS
#define GET_INT_STATUS 0x14

//==================================================================
// FM Transmit Commands
//==================================================================

// TX_TUNE_FREQ
#define TX_TUNE_FREQ 0x30

// TX_TUNE_POWER
#define TX_TUNE_POWER 0x31

// TX_TUNE_MEASURE
#define TX_TUNE_MEASURE 0x32

// TX_TUNE_STATUS
#define TX_TUNE_STATUS           0x33
#define TX_TUNE_STATUS_IN_INTACK 0x01

// TX_ASQ_STATUS
#define TX_ASQ_STATUS             0x34
#define TX_ASQ_STATUS_IN_INTACK   0x01
#define TX_ASQ_STATUS_OUT_IALL    0x01
#define TX_ASQ_STATUS_OUT_IALH    0x02
#define TX_ASQ_STATUS_OUT_OVERMOD 0x04

// TX_RDS_BUFF
#define TX_RDS_BUFF               0x35
#define TX_RDS_BUFF_IN_INTACK     0x01
#define TX_RDS_BUFF_IN_MTBUFF     0x02
#define TX_RDS_BUFF_IN_LDBUFF     0x04
#define TX_RDS_BUFF_IN_FIFO       0x80
#define TX_RDS_BUFF_OUT_RDSPSXMIT 0x10
#define TX_RDS_BUFF_OUT_CBUFXMIT  0x08
#define TX_RDS_BUFF_OUT_FIFOXMIT  0x04
#define TX_RDS_BUFF_OUT_CBUFWRAP  0x02
#define TX_RDS_BUFF_OUT_FIFOMT    0x01

// TX_RDS_PS
#define TX_RDS_PS 0x36

#endif
