PROJECT := asr15
TOOLCHAIN_PREFIX := arm-none-eabi-
BUILD_DIR := build

C_SRCS := \
  main.c \
  st/core_cm3.c \
  st/system_stm32f10x.c \
  st/stm32f10x_adc.c \
  st/stm32f10x_gpio.c \
  st/stm32f10x_rcc.c \
  st/stm32f10x_i2c.c \
  periph/fram.c \
  periph/display.c \
  krnl/algorithm.c \
  krnl/menu.c \
  krnl/text.c \
  krnl/font.c \

S_SRCS := startup.S

LINKER_SCRIPT := stm32f100_ld_vl.ld

CFLAGS := \
  -ffunction-sections \
  -fdata-sections \
  -fmessage-length=0 \
  -mcpu=cortex-m3 \
  -mthumb \
  -MMD \

SYMBOLS := \
  NDEBUG \
  USE_STDPERIPH_DRIVER \
  STM32F10X_LD_VL \

INCLUDE_DIRS := \
  st \

OPTIMIZATION := -Os
DEBUG := -ggdb3

WARNINGS := \
  -Wall \
  -Wextra \
  -Werror \

OBJS := $(C_SRCS:.c=.o)
OBJS += $(S_SRCS:.S=.o)
OBJS := $(addprefix $(BUILD_DIR)/, $(OBJS))

SYMBOLS := $(addprefix -D, $(SYMBOLS))
INCLUDE_DIRS := $(addprefix -I, $(INCLUDE_DIRS))

CFLAGS += $(SYMBOLS) $(INCLUDE_DIRS) $(OPTIMIZATION) $(DEBUG) $(WARNINGS)

CC      := $(TOOLCHAIN_PREFIX)gcc
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP := $(TOOLCHAIN_PREFIX)objdump
SIZE    := $(TOOLCHAIN_PREFIX)size

MAIN_TARGET = $(BUILD_DIR)/$(PROJECT).elf

all: objdirs $(MAIN_TARGET)

objdirs:
	@mkdir -p $(dir $(OBJS))

$(MAIN_TARGET): $(OBJS)
	@echo Linking "$@"...
	@$(CC) -T $(LINKER_SCRIPT) -nostdlib -Wl,--gc-sections -Wl,-Map,$(@:.elf=.map) $(CFLAGS) -o "$@" $(OBJS)
	@$(OBJCOPY) -O binary "$@" $(@:.elf=.bin)
	@$(OBJCOPY) -O ihex "$@" $(@:.elf=.hex)
	@$(OBJDUMP) -h -S "$@" > $(@:.elf=.lst)
	@$(SIZE) --format=berkeley -t "$@"

$(BUILD_DIR)/%.o: %.c
	@echo Compiling "$@"...
	@$(CC) $(CFLAGS) -c -o "$@" "$<"

$(BUILD_DIR)/%.o: %.S
	@echo Compiling "$@"...
	@$(CC) -x assembler-with-cpp $(CFLAGS) -c -o "$@" "$<"

-include $(OBJS:.o=.d)

clean:
	@rm -vrf $(BUILD_DIR)/*

.PHONY: all clean objdirs
