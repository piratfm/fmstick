
# Sources
SRCS = main.c stm32f10x_it.c system_stm32f10x.c newlib_stubs.c core_cm3.c

# USB specific files
SRCS += usb_desc.c  usb_endp.c  usb_istr.c  usb_prop.c  usb_pwr.c

# Board specific files
SRCS += audio_codec_si4711.c hw_config.c I2CRoutines.c stm32_hardware.c

# Project name

PROJ_NAME=fmstick
OUTPATH=build

###################################################

#########################
# Defines from eclipse
#########################
#USE_STM32_FMSTICK
#STM32F10X_CL
#USE_STDPERIPH_DRIVER
#EXTERNAL_CRYSTAL_14_7456MHz
#HSE_VALUE=14745600

#########################
# UNDefines from eclipse
#########################


BINPATH=~/sat/bin
CC=$(BINPATH)/arm-none-eabi-gcc
OBJCOPY=$(BINPATH)/arm-none-eabi-objcopy
STRIP=$(BINPATH)/arm-none-eabi-strip
SIZE=$(BINPATH)/arm-none-eabi-size
FLASHPROG=$(BINPATH)/st-flash

CFLAGS  = -std=gnu99 -g -O2 -Wall -Tstm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -mcpu=cortex-m3
CFLAGS += -fno-strict-aliasing -ffunction-sections
CFLAGS += -DARM_MATH_CM3 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -DUNALIGNED_SUPPORT_DISABLE
CFLAGS += -msoft-float

###################################################

vpath %.c src
vpath %.a lib

ROOT=$(shell pwd)

# Includes
CFLAGS += -Iinc -Ilib/Core/cmsis -Ilib/Core/stm32

# Flags
CFLAGS += -DUSE_STM32_FMSTICK
CFLAGS += -DSTM32F10X_CL
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -DEXTERNAL_CRYSTAL_14_7456MHz
CFLAGS += -DHSE_VALUE=14745600

# Library paths
LIBPATHS = -Llib/StdPeriph
LIBPATHS += -Llib/USB_Lib

# Libraries to link
LIBS = -lm -lstdperiph -lusbdev

# Extra includes
CFLAGS += -Ilib/StdPeriph/inc
CFLAGS += -Ilib/USB_Lib/inc

# add startup file to build
SRCS += lib/startup_stm32f10x_cl.S

OBJS = $(SRCS:.c=.o)

###################################################

.PHONY: lib proj flash

all: lib proj
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

lib:
	$(MAKE) -C lib

proj: 	$(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	mkdir -p $(OUTPATH)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBPATHS) $(LIBS)
	$(STRIP) $(OUTPATH)/$(PROJ_NAME).elf
	#$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	rm -f *.o
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
	$(MAKE) clean -C lib # Remove this line if you don't want to clean the libs as well
	
flash: proj
	$(FLASHPROG) write $(OUTPATH)/$(PROJ_NAME).bin 0x8000000
