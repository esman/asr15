PROJECT := asr15
TOOLCHAIN_PREFIX := arm-none-eabi
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

CC      := $(TOOLCHAIN_PREFIX)-gcc
OBJCOPY := $(TOOLCHAIN_PREFIX)-objcopy
OBJDUMP := $(TOOLCHAIN_PREFIX)-objdump
SIZE    := $(TOOLCHAIN_PREFIX)-size

TARGET_NAME = $(BUILD_DIR)/$(PROJECT)
TARGET.elf := $(TARGET_NAME).elf
TARGET.bin := $(TARGET_NAME).bin
TARGET.hex := $(TARGET_NAME).hex
TARGET.lst := $(TARGET_NAME).lst
TARGET.siz := $(TARGET_NAME).siz

all: objdirs $(TARGET.elf) $(TARGET.bin) $(TARGET.hex) $(TARGET.lst) $(TARGET.siz)

objdirs:
	@mkdir -p $(dir $(OBJS))

$(TARGET.elf): $(OBJS)
	$(CC) -T $(LINKER_SCRIPT) -nostdlib -Xlinker --gc-sections -Wl,-Map,$(@:.elf=.map) $(CFLAGS) -o "$@" $(OBJS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o "$@" "$<"

$(BUILD_DIR)/%.o: %.S
	$(CC) -x assembler-with-cpp $(CFLAGS) -c -o "$@" "$<"

$(TARGET.bin): $(TARGET.elf)
	@$(OBJCOPY) -O binary "$<" "$@"

$(TARGET.hex): $(TARGET.elf)
	@$(OBJCOPY) -O ihex "$<" "$@"

$(TARGET.lst): $(TARGET.elf)
	@$(OBJDUMP) -h -S "$<" > "$@"

$(TARGET.siz): $(TARGET.elf)
	@$(SIZE) --format=berkeley -t "$<"

-include $(OBJS:.o=.d)

clean:
	rm -vrf $(BUILD_DIR)/*

.PHONY: all clean
