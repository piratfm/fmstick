BINPATH=~/sat/bin

CC=$(BINPATH)/arm-none-eabi-gcc
AR=$(BINPATH)/arm-none-eabi-ar

vpath %.c src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m3

CFLAGS += -msoft-float

CFLAGS += -ffreestanding -nostdlib -DUSE_STDPERIPH_DRIVER

# Includes
CFLAGS += -Iinc -I../Core/cmsis -I../Core/stm32

# Sources
SRCS =  misc.c stm32f10x_can.c stm32f10x_dac.c stm32f10x_exti.c \
	stm32f10x_gpio.c stm32f10x_pwr.c stm32f10x_sdio.c stm32f10x_usart.c \
	stm32f10x_adc.c stm32f10x_cec.c stm32f10x_dbgmcu.c stm32f10x_flash.c \
	stm32f10x_i2c.c stm32f10x_rcc.c stm32f10x_spi.c stm32f10x_wwdg.c \
	stm32f10x_bkp.c stm32f10x_crc.c stm32f10x_dma.c stm32f10x_fsmc.c \
	stm32f10x_iwdg.c stm32f10x_rtc.c stm32f10x_tim.c

OBJS = $(SRCS:.c=.o)
LIBNAME = libstdperiph.a

all: $(LIBNAME)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIBNAME): $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBNAME)
