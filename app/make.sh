#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

COMMON_H=${CURRENT_DIR}/../common/common.h
BUILD_TIME_H=${CURRENT_DIR}/src/sys/build_time.h
VERSION_H=${CURRENT_DIR}/src/sys/version.h

CRC=${CURRENT_DIR}/../programmer/Release/crc.exe
APP_HEX=${CURRENT_DIR}/output/APP.hex
META_BIN=${CURRENT_DIR}/output/meta.bin
LDR_SIZE=$(grep -oP '#define LDR_SIZE \K[0x0-9A-F]+' ${COMMON_H})
FACTORY_META_SIZE=$(grep -oP '#define FACTORY_META_SIZE \K[0x0-9A-F]+' ${COMMON_H})
STC_RAM_SIZE=$(grep -oP '#define STC_RAM_SIZE \K[0x0-9A-F]+' ${COMMON_H})
STC_ROM_SIZE=$(grep -oP '#define STC_ROM_SIZE \K[0x0-9A-F]+' ${COMMON_H})
# #define APP_BUILD_TIME 1757919463UL
BUILD_TIME=$(grep -oP '#define APP_BUILD_TIME \K[0-9]+UL' ${BUILD_TIME_H} | sed 's/UL//')
# convert to 0xXXXX format
BUILD_TIME_HEX=$(printf "0x%04X" ${BUILD_TIME})
APP_VERSION_MAJOR=$(grep -oP '#define APP_VERSION_MAJOR \K[0-9]+' ${VERSION_H})
APP_VERSION_MINOR=$(grep -oP '#define APP_VERSION_MINOR \K[0-9]+' ${VERSION_H})
APP_VERSION_PATCH=$(grep -oP '#define APP_VERSION_PATCH \K[0-9]+' ${VERSION_H})
APP_VERSION_HEX=$(printf "0x%02X%02X%04X" ${APP_VERSION_MAJOR} ${APP_VERSION_MINOR} ${APP_VERSION_PATCH})

LDR_SIZE_10=`printf "%d" ${LDR_SIZE}` # convert 0xXXXX to decimal
FACTORY_META_SIZE_10=`printf "%d" ${FACTORY_META_SIZE}` # convert 0xXXXX to decimal
RAM_SIZE_10=`printf "%d" ${STC_RAM_SIZE}` # convert 0xXXXX to decimal
ROM_SIZE_10=`printf "%d" ${STC_ROM_SIZE}` # convert 0xXXXX to decimal
IAP_ADDR_APP_START=$((LDR_SIZE + FACTORY_META_SIZE))
IAP_ADDR_APP_START_HEX=$(printf "%04XH" ${IAP_ADDR_APP_START})
APP_MAX_SIZE=$((STC_ROM_SIZE - LDR_SIZE - FACTORY_META_SIZE))
APP_MAX_SIZE_10=`printf "%d" ${APP_MAX_SIZE}`  # convert 0xXXXX to decimal
echo "IAP_ADDR_APP_START=${IAP_ADDR_APP_START}, ${IAP_ADDR_APP_START_HEX}"
echo "STC_RAM_SIZE=${STC_RAM_SIZE}, ${RAM_SIZE_10}"
echo "STC_ROM_SIZE=${STC_ROM_SIZE}, ${ROM_SIZE_10}"
echo "BUILD_TIME=${BUILD_TIME}, ${BUILD_TIME_HEX}"
echo "APP_VERSION=${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}, ${APP_VERSION_HEX}"
# exit

# 1. gen CLASSES for makefile
# parse LDR_SIZE from COMMON_H like `#define LDR_SIZE 0x1000  // bootloader flash space`
EPROM_START=$(printf "0x%04X" $((IAP_ADDR_APP_START + 3)))
echo "EPROM_START=${EPROM_START}"
CLASSES="CODE (C:${EPROM_START}-C:0xFFFF), CONST (C:${EPROM_START}-C:0xFFFF), ECODE (C:${EPROM_START}-C:0xFFFF), HCONST (C:${EPROM_START}-C:0xFFFF)"
echo "CLASSES=${CLASSES}"

# 2. build app
rm -rf ./output || true
utc_timestamp=$(date +%s)
utc_timestamp="${utc_timestamp}UL"
echo "#define APP_BUILD_TIME ${utc_timestamp}" > ${CURRENT_DIR}/src/sys/build_time.h
time make DEBUG=1 INTVECTOR=${IAP_ADDR_APP_START} CLASSES="${CLASSES}" -j


# 3. check app size is not bigger than APP_MAX_SIZE

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
kb_total=`echo $APP_MAX_SIZE_10 | awk '{print $1 / 1024}'`
# echo "first kb_total=$kb_total"
kb_total=`printf_value $kb_total`
# echo "second kb_total=$kb_total"
awk_param="{print (\$1 / ${APP_MAX_SIZE_10}) * 100}"
used=`echo $romsize | awk "$awk_param"`
printf "ROM size: %d/%d, %.2fKB/%sKB, %.2f%% Full\n" $romsize "${APP_MAX_SIZE_10}" $kb $kb_total $used | tee >(cat >&1) >> buildLog.txt

if [ $romsize -gt $APP_MAX_SIZE_10 ]; then
    echo "Error: app size $romsize exceeds APP_MAX_SIZE $APP_MAX_SIZE_10" >&2
    exit 1
fi


# 4. calculate app meta info and write to meta.bin
cmd="${CRC} -i ${APP_HEX} -l ${LDR_SIZE} -m ${FACTORY_META_SIZE} -f 0x00 -t ${BUILD_TIME_HEX} -v ${APP_VERSION_HEX} -M ${META_BIN}"
echo "Generating meta info binary with command: ${cmd}"
${CRC} -i ${APP_HEX} -l ${LDR_SIZE} -m ${FACTORY_META_SIZE} -f 0x00 -t ${BUILD_TIME_HEX} -v ${APP_VERSION_HEX} -M ${META_BIN}