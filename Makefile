BIN=stm32vldiscovery-linux-template

TOOLS_PATH=/home/daniil/arm-cs-tools
TOOLS_PREFIX=arm-none-eabi-
TOOLS_VERSION=4.6.3

CFLAGS=-c -std=c99 -mcpu=cortex-m3 -mthumb -Wall -O0 -mapcs-frame -D__thumb2__=1 
CFLAGS+=-msoft-float -gdwarf-2 -mno-sched-prolog -fno-hosted -mtune=cortex-m3 
CFLAGS+=-march=armv7-m -mfix-cortex-m3-ldrd -ffunction-sections -fdata-sections 
CFLAGS+=-I./cmsis -I./stm32_lib -I.
ASFLAGS=-mcpu=cortex-m3 -I./cmsis -I./stm32_lib -gdwarf-2 -gdwarf-2
LDFLAGS=-static -mcpu=cortex-m3 -mthumb -mthumb-interwork -Wl,--start-group 
LDFLAGS+=-L$(TOOLS_PATH)/lib/gcc/arm-none-eabi/$(TOOLS_VERSION)/thumb2 
LDFLAGS+=-L$(TOOLS_PATH)/arm-none-eabi/lib/thumb2 -lc -lg -lstdc++ -lsupc++ -lgcc -lm 
# LDFLAGS+=--section-start=.text=0x08000000
LDFLAGS+=-Wl,--end-group -Xlinker -Map -Xlinker $(BIN).map -Xlinker 
LDFLAGS+=-T ./stm32_lib/device_support/gcc/stm32f100rb_flash.ld -o $(BIN).elf

CC=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)gcc-$(TOOLS_VERSION)
AS=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)as
SIZE=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)size

CMSISSRC=./cmsis/core_cm3.c
STM32_LIBSRC=./stm32_lib/system_stm32f10x.c ./stm32_lib/stm32f10x_it.c
STM32_LIBSRC+=./stm32_lib/stm32f10x_rcc.c ./stm32_lib/stm32f10x_gpio.c
STM32_LIBSRC+=./hd44780.c ./hd44780_stm32f10x.c ./newlib_stubs.c
SRC=main.c

OBJ=core_cm3.o system_stm32f10x.o stm32f10x_it.o startup_stm32f10x_md_vl.o
OBJ+=stm32f10x_rcc.o stm32f10x_gpio.o
OBJ+=hd44780.o hd44780_stm32f10x.o newlib_stubs.o
OBJ+=main.o

all: ccmsis cstm32_lib cc ldall mkbin
	$(SIZE) -B $(BIN).elf

ccmsis: $(CMSISSRC)
	$(CC) $(CFLAGS) $(CMSISSRC)

cstm32_lib: $(STM32_LIBSRC)
	$(CC) $(CFLAGS) $(STM32_LIBSRC)
	$(AS) $(ASFLAGS) ./stm32_lib/device_support/gcc/startup_stm32f10x_md_vl.S -o startup_stm32f10x_md_vl.o

cc: $(SRC)
	$(CC) $(CFLAGS) $(SRC)

ldall:
	$(CC) $(OBJ) $(LDFLAGS)

mkbin:
	arm-none-eabi-objcopy -Obinary $(BIN).elf $(BIN).bin

.PHONY: clean load

clean:
	rm -f 	$(OBJ) \
		$(BIN).map \
		$(BIN).elf \
		$(BIN).bin

load:
	./stlink/st-flash write /dev/sg0 $(BIN).bin 0x08000000
