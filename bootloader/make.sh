#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

CRC=${CURRENT_DIR}/../programmer/Release/crc.exe
APP_HEX=${CURRENT_DIR}/../app/output/APP.hex
COMMON_H=${CURRENT_DIR}/../common/common.h
ISR_ASM=${CURRENT_DIR}/src/isr.asm
LDR_SIZE=$(grep -oP '#define LDR_SIZE \K[0x0-9A-F]+' ${COMMON_H})
STC_RAM_SIZE=$(grep -oP '#define STC_RAM_SIZE \K[0x0-9A-F]+' ${COMMON_H})
STC_ROM_SIZE=$(grep -oP '#define STC_ROM_SIZE \K[0x0-9A-F]+' ${COMMON_H})

LDR_SIZE_10=`printf "%d" ${LDR_SIZE}` # convert 0xXXXX to decimal
FACTORY_META_SIZE=$(grep -oP '#define FACTORY_META_SIZE \K[0x0-9A-F]+' ${COMMON_H})
RAM_SIZE_10=`printf "%d" ${STC_RAM_SIZE}` # convert 0xXXXX to decimal
ROM_SIZE_10=`printf "%d" ${STC_ROM_SIZE}` # convert 0xXXXX to decimal
IAP_ADDR_APP_START=$((LDR_SIZE + FACTORY_META_SIZE))
IAP_ADDR_APP_START_HEX=$(printf "%04XH" ${IAP_ADDR_APP_START})
APP_MAX_SIZE=$((STC_ROM_SIZE - LDR_SIZE - FACTORY_META_SIZE))
APP_MAX_SIZE_10=`printf "%d" ${APP_MAX_SIZE}`  # convert 0xXXXX to decimal
echo "IAP_ADDR_APP_START=${IAP_ADDR_APP_START}, ${IAP_ADDR_APP_START_HEX}"
echo "STC_RAM_SIZE=${STC_RAM_SIZE}, ${RAM_SIZE_10}"
echo "STC_ROM_SIZE=${STC_ROM_SIZE}, ${ROM_SIZE_10}"
# exit
rm -rf ./output || true

# 1. update isr.asm
# replace IAP_ADDR_APP_START in ISR_ASM like `IAP_ADDR_APP_START EQU 1000H`
sed -i "s/IAP_ADDR_APP_START EQU .*/IAP_ADDR_APP_START EQU ${IAP_ADDR_APP_START_HEX}/" ${ISR_ASM}

# 2. build bootloader
utc_timestamp=$(date +%s)
utc_timestamp="${utc_timestamp}UL"
echo "#define LDR_BUILD_TIME ${utc_timestamp}" > ${CURRENT_DIR}/src/sys/build_time.h
time make DEBUG=1 -j

# 3. check bootloader size is not bigger than LDR_SIZE

# print a value, if its a integer, print it as integer, else keep 2 decimal places
printf_value() {
    local value=$1
    # 检查是否为整数（小数部分全为0）
    if [[ $(awk -v n="$value" 'BEGIN{print (n == int(n))}') -eq 1 ]]; then
        printf "%d\n" "$value"  # 输出整数
    else
        printf "%.2f\n" "$value"  # 输出两位小数
    fi
}

# take out the last line of ./buildlog.txt
log=`tail -n 1 buildLog.txt`
const=`echo "${log}" | awk '{print $5}' | awk -F '=' '{print $2}'`
code=`echo "${log}" | awk '{print $6}' | awk -F '=' '{print $2}'`
romsize=$((const+code))
kb=`echo $romsize | awk '{print $1 / 1024}'`
kb_total=`echo $LDR_SIZE_10 | awk '{print $1 / 1024}'`
# echo "first kb_total=$kb_total"
kb_total=`printf_value $kb_total`
# echo "second kb_total=$kb_total"
awk_param="{print (\$1 / ${LDR_SIZE_10}) * 100}"
used=`echo $romsize | awk "$awk_param"`
printf "ROM size: %d/%d, %.2fKB/%sKB, %.2f%% Full\n" $romsize "${LDR_SIZE_10}" $kb $kb_total $used | tee >(cat >&1) >> buildLog.txt

if [ $romsize -gt $LDR_SIZE_10 ]; then
    echo "Error: Bootloader size $romsize exceeds LDR_SIZE $LDR_SIZE_10" >&2
    exit 1
fi