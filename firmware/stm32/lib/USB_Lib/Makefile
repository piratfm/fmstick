BINPATH=~/sat/bin

CC=$(BINPATH)/arm-none-eabi-gcc
AR=$(BINPATH)/arm-none-eabi-ar

vpath %.c src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m3

CFLAGS += -msoft-float

CFLAGS += -ffreestanding -nostdlib

CFLAGS += -DSTM32F10X_CL
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -DEXTERNAL_CRYSTAL_14_7456MHz
CFLAGS += -DHSE_VALUE=14745600

# Includes
CFLAGS += -Iinc -I../Core/cmsis -I../Core/stm32 -I../StdPeriph/inc -I../../inc

# Sources
SRCS =  otgd_fs_cal.c otgd_fs_dev.c otgd_fs_int.c otgd_fs_pcd.c \
	usb_core.c usb_init.c usb_int.c usb_mem.c usb_regs.c usb_sil.c

OBJS = $(SRCS:.c=.o)
LIBNAME = libusbdev.a

all: $(LIBNAME)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIBNAME): $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBNAME)
