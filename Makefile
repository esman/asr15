PROJECT := asr15
TOOLCHAIN_PREFIX := arm-none-eabi-
FLASH_ADDR := 0x08000000
SRCDIR := src
RESDIR := res
OBJDIR := build

C_SRCS := \
  main.c \
  st/core_cm3.c \
  st/system_stm32f10x.c \
  st/stm32f10x_gpio.c \
  st/stm32f10x_rcc.c \
  st/stm32f10x_i2c.c \
  periph/fram.c \
  periph/display.c \
  krnl/algorithm.c \
  krnl/menu.c \
  krnl/text.c \

S_SRCS := startup.S

RESOURCES := \
  font.res \

LINKER_SCRIPT := script.ld

CFLAGS := \
  -ffunction-sections \
  -fdata-sections \
  -fmessage-length=0 \
  -mcpu=cortex-m3 \
  -mthumb \
  -MMD \
  -nostdlib \

LDFLAGS := \
  --gc-sections \

SYMBOLS := \
  NDEBUG \
  USE_STDPERIPH_DRIVER \
  STM32F10X_LD_VL \

INCLUDE_DIRS := \
  $(SRCDIR)/st \

OPTIMIZATION := -Os
DEBUG := -ggdb3

WARNINGS := \
  -Wall \
  -Wextra \
  -Werror \

comma := ,
RESOURCES := $(addprefix res/,$(RESOURCES))

OBJS := $(C_SRCS:.c=.o)
OBJS += $(S_SRCS:.S=.o)
OBJS += $(RESOURCES:.res=.o)
OBJS := $(addprefix $(OBJDIR)/, $(OBJS))

SYMBOLS := $(addprefix -D, $(SYMBOLS))
INCLUDE_DIRS := $(addprefix -I, $(INCLUDE_DIRS))

CFLAGS += $(SYMBOLS) $(INCLUDE_DIRS) $(OPTIMIZATION) $(DEBUG) $(WARNINGS)
LDFLAGS := $(addprefix -Wl$(comma),$(LDFLAGS))

CC      := $(TOOLCHAIN_PREFIX)gcc
LD      := $(TOOLCHAIN_PREFIX)ld
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP := $(TOOLCHAIN_PREFIX)objdump
SIZE    := $(TOOLCHAIN_PREFIX)size

MAIN_TARGET = $(OBJDIR)/$(PROJECT).elf

all: objdir $(MAIN_TARGET)

objdir:
	@mkdir -p $(dir $(OBJS))

$(MAIN_TARGET): $(OBJS)
	@echo Linking "$@"...
	@$(CC) -T $(LINKER_SCRIPT) $(LDFLAGS) -Wl,-Map,$(@:.elf=.map) $(CFLAGS) -o "$@" $(OBJS)
	@$(OBJCOPY) -O binary "$@" $(@:.elf=.bin)
	@$(OBJCOPY) -O ihex "$@" $(@:.elf=.hex)
	@$(OBJDUMP) -h -S "$@" > $(@:.elf=.lst)
	@$(SIZE) --format=berkeley -t "$@"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo Compiling "$@"...
	@$(CC) $(CFLAGS) -c -o "$@" "$<"

$(OBJDIR)/%.o: $(SRCDIR)/%.S
	@echo Compiling "$@"...
	@$(CC) -x assembler-with-cpp $(CFLAGS) -c -o "$@" "$<"

BASENAME = $(basename $(notdir $(@)))

$(OBJDIR)/%.o: $(SRCDIR)/%.res
	@echo Compiling "$@"..
	@$(LD) -r -b binary -o "$@" "$<"
	@$(OBJCOPY) \
	--rename-section .data=.rodata,readonly,contents,alloc,load,data "$@" "$@" \
	--redefine-sym _binary_src_res_$(BASENAME)_res_start=$(BASENAME) \
	--redefine-sym _binary_src_res_$(BASENAME)_res_size=$(BASENAME)_size \

-include $(OBJS:.o=.d)

clean:
	@rm -vrf $(OBJDIR)/*

flash: all
	@st-flash write $(MAIN_TARGET:.elf=.bin) $(FLASH_ADDR)

.PHONY: all clean objdir flash
