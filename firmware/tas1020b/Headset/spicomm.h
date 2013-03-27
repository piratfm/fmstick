#ifndef _SPICOMM_H
#define _SPICOMM_H

#define MAX_BIT_COUNT   16
#define EnableTS        0x88E6
#define DisableTS       EnableTS|0x4000

#ifndef _SI4711_
#define INIT_TSCRESET()	write_data[0] = 0x0880; write_data[1] = 0xBB00

#define INIT_TSCPg11()	write_data[0] = 0x0800;	\
						write_data[1] = DisableTS; \
						write_data[2] = 0x4000

#define INIT_TSCPg12() 	write_data[0] = 0x0860;	\
                        write_data[1] = 0x0015; \
						write_data[2] = 0x08A0;	\
						write_data[3] = 0x000B; \
						write_data[4] = 0x0800;	\
						write_data[5] = EnableTS

#define INIT_TSCPg21()	write_data[0] = 0x1000;	\
					    write_data[1] = 0x0000;	\
                        write_data[2] = 0x0000; \
                        write_data[3] = 0x8080; \
						write_data[4] = 0xC580; \
						write_data[5] = 0x44F0; \
						write_data[6] = 0x3400; \
						write_data[7] = 0x3000	

#define INIT_TSCPg22()  write_data[0] = 0x1360;	\
					    write_data[1] = 0x1120; \
						write_data[2] = 0x13A0;	\
					    write_data[3] = 0x0030

unsigned int idata bit_count = 0;
unsigned int idata data_received, data_to_send;
unsigned int idata end_shift;
unsigned int idata CounterT;
int idata CurrentADC;
#endif

byte idata PCCommand = 0;
byte idata PCRequest = RequestDone;

#endif

