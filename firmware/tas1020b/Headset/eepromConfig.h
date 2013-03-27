
//last 512 bytes for config
#define TX_CONFIG_OFFSET 		0x1E00    //0x1C00
#define CONF_NUM_CMDS_OFFSEET   0x02


/*
data fortmat:

b0 0b FF

typedef struct {
	byte cmd;
	byte arg1;
	byte arg2;
	byte arg3;
	byte arg4;
	byte arg5;
	byte arg6;
	byte arg7;
}


1cmd = 8 bytes,
total max cmds: 32

POWER_UP
P: GPO_IEN
P: REFCLK_FREQ
P: REFCLK_PRESCALE
======================================
= 1 cmd + 3 prop for tune (hardcoded)
======================================

P: TX_COMPONENT_ENABLE (4)
P: TX_AUDIO_DEVIATION (4)
P: TX_PILOT_DEVIATION (4)
P: TX_RDS_DEVIATION (4)
P: TX_PREEMPHASIS (4)
P: TX_PILOT_FREQUENCY (4)

TX_TUNE_FREQ (4 => 8?)
TX_TUNE_POWER (5 => 8?)
>>> wait 110ms
TX_TUNE_STATUS
======================================
= 6 prop + 2 cmd for tune
= 12 + 2 + 2
= 36 + 4 + 5 => 45
= (6+2)*8 => 64bytes
======================================

hardcoded:
P: DIGITAL_INPUT_FORMAT
P: DIGITAL_INPUT_SAMPLE_RATE

P: TX_ACOMP_ENABLE
P: TX_ACOMP_THRESHOLD
P: TX_ACOMP_ATTACK_TIME
P: TX_ACOMP_RELEASE_TIME
P: TX_ACOMP_GAIN
P: TX_LIMITER_RELEASE_TIME
======================================
= 6 prop for audio compressor/limiter
=> 6*8 => 48bytes ()
======================================

??????
P: TX_ASQ_INT_SELECT
P: TX_ASQ_LEVEL_LOW
P: TX_ASQ_DURATION_LOW
P: TX_ASQ_LEVEL_HIGH
P: TX_ASQ_DURATION_HIGH
======================================
= 5 prop's for audio asq
= 5*8 => 40 bytes
======================================
??????

P: TX_RDS_PI
P: TX_RDS_PS_MIX
P: TX_RDS_PS_MISC
P: TX_RDS_PS_REPEAT_COUNT
P: TX_RDS_PS_MESSAGE_COUNT
P: TX_RDS_PS_AF
P: TX_RDS_FIFO_SIZE
======================================
= 7 prop's for RDS
= 7*8 => 56 bytes
======================================

TX_RDS_PS[0.0] {4bytes}
TX_RDS_PS[0.1] {4bytes}
TX_RDS_PS[1.0] {4bytes}
TX_RDS_PS[1.1] {4bytes}
   .
TX_RDS_PS[11.0] {4bytes}
TX_RDS_PS[11.1] {4bytes}

TX_RDS_BUFF[0] {6bytes}
TX_RDS_BUFF[1] {6bytes}
   .
TX_RDS_BUFF[n] {6bytes}
= 4*8 => 32 bytes
= 6*8 => 40 bytes
*/




#define CONF_FL_AUTOSTART 			0x01

#define CONF_FL_TX_SECT             0x02
#define CONF_FL_COMPRESSOR_SECT 	0x04
#define CONF_FL_LIMITER_SECT    	0x08
#define CONF_FL_ASQ_SECT            0x10
#define CONF_FL_RDS_SECT            0x20

#define TX_CONFIG_FLAGS_OFFSET 0

bit configCheckStartupNeeded();
bit configSetStartupConfig();

