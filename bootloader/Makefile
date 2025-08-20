
# c51 options
C51_DIR=/D/mdk/C51
AS=${C51_DIR}/Bin/A51.EXE
CC=${C51_DIR}/Bin/C51.EXE
LINK=${C51_DIR}/Bin/LX51.EXE
LDFALGS=REMOVEUNUSED
HEX=${C51_DIR}/Bin/OHX51.EXE
MKDIR="C:/Program Files/Git/usr/bin/mkdir.exe"
SPACE :=
SPACE +=
comma := ,
DEBUG ?= 0

LIBHB_DIR=../libhb

INC_DIR=${LIBHB_DIR};${LIBHB_DIR}/libemb;${LIBHB_DIR}/libstc;src;src/sys
DEFINES=WHAT_EVER_YOU_WANT_TO_DEFINE_HERE
ifeq ($(DEBUG), 1)
DEFINES+=,DEBUG
endif

# MODP2 开启双DPTR，待测试，不确定效果
CFLAGS=SYMBOLS CODE DEBUG OBJECTEXTEND OMF2 OPTIMIZE(9,SPEED) BROWSE INCDIR(${INC_DIR}) DEFINE(${DEFINES}) SMALL
BUILD_DIR=./output
ASFLAGS=SET (SMALL) DEBUG EP
TARGET=BOOTLOADER

C_SOURCES= \
${LIBHB_DIR}/libstc/stc8h.c \
src/dfu.c \
src/main.c \
src/uart.c

ASM_SOURCES= \
src/isr.asm

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.obj)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.asm=.obj)))
vpath %.asm $(sort $(dir $(ASM_SOURCES)))

all: prepare $(BUILD_DIR)/$(TARGET).hex

gen_gpio_h:
	@(SYS_DIR=$(SYS_DIR) DEFINES="${C51IOSH_DEFINES}" ${C51IOSH})

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET)
	@echo "generating hex file..."
	@${HEX} ${BUILD_DIR}/${TARGET}.ABS "HEXFILE(${BUILD_DIR}/${TARGET}.hex)" > /dev/null
	@echo "hex file generated, output is ${BUILD_DIR}/${TARGET}.hex"

$(BUILD_DIR)/$(TARGET): $(OBJECTS) 
	@echo "linking..."
	@objs="$(OBJECTS)"; \
	objs=$$(echo $${objs} | sed 's/ /,/g'); \
	set +e; \
	msg=`${LINK} $${objs} TO ${BUILD_DIR}/${TARGET}.ABS ${LDFALGS} "PRINT(${BUILD_DIR}/${TARGET}.map)"`; \
	e=$$?; \
	if [ $$e -eq 0 ]; then \
		echo "Link success"; \
		log=`echo "$${msg}" | grep "Program Size"`; \
		echo "$${log}"; \
		echo "$${log}" >> buildLog.txt; \
	elif [ $$e -eq 1 ]; then \
		echo "Link success but with warnings, here is the message:"; \
		printf "%s\n" "$${msg}"; \
		log=`echo "$${msg}" | grep "Program Size"`; \
		echo "$${log}"; \
		echo "$${log}" >> buildLog.txt; \
	else \
		echo "Link failed, here is the message:"; \
		printf "%s\n" "$${msg}"; \
		exit 1; \
	fi; \
	set -e

$(BUILD_DIR)/%.obj: %.c | Makefile gen_gpio_h
	@echo "compiling $<..."
	@set +e; \
	msg=`${CC} $< "${CFLAGS}" "PRINT(${BUILD_DIR}/$(basename $(notdir $@)).lst)" "OBJECT($@)"`; \
	e=$$?; \
	if [ $$e -eq 0 ]; then \
		:; \
	elif [ $$e -eq 1 ]; then \
		echo "Compile success but with warnings, here is the message:"; \
		printf "%s\n" "$${msg}"; \
	else \
		echo "Compile failed, here is the message:"; \
		printf "%s\n" "$${msg}"; \
		exit 1; \
	fi; \
	set -e

$(BUILD_DIR)/%.obj: %.asm | Makefile
	@echo "compiling $<..."
	@set +e; \
	msg=`$(AS) $< "$(ASFLAGS)" "PRINT(${BUILD_DIR}/$(basename $(notdir $@)).lst)" "OBJECT($@)"`; \
	e=$$?; \
	if [ $$e -eq 0 ]; then \
		:; \
	elif [ $$e -eq 1 ]; then \
		echo "Compile success but with warnings, here is the message:"; \
		printf "%s\n" "$${msg}"; \
	else \
		echo "Compile failed, here is the message:"; \
		printf "%s\n" "$${msg}"; \
		exit 1; \
	fi; \
	set -e

prepare:
	@$(MKDIR) -p $(BUILD_DIR)

clean:
	@rm -f $(BUILD_DIR)/*.obj
	@rm -f $(BUILD_DIR)/*.lst
	@rm -f $(BUILD_DIR)/*.map
	@rm -f $(BUILD_DIR)/*.ABS
	@rm -f $(BUILD_DIR)/*.SBR
	@rm -f $(BUILD_DIR)/*.o
	@rm -f $(BUILD_DIR)/$(TARGET)
	@rm -f $(BUILD_DIR)/$(TARGET).hex
	

.PHONY: all clean prepare
