#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

COMMON_H=${CURRENT_DIR}/../common/common.h

# 1. gen CLASSES for makefile
# parse LDR_SIZE from COMMON_H like `#define LDR_SIZE 0x1000  // bootloader flash space`
LDR_SIZE=$(grep -oP '#define LDR_SIZE \K[0x0-9A-F]+' ${COMMON_H})
EPROM_START=$(printf "0x%04X" $((LDR_SIZE + 3)))
echo "EPROM_START=${EPROM_START}"
CLASSES="CODE (C:${EPROM_START}-C:0xFFFF), CONST (C:${EPROM_START}-C:0xFFFF), ECODE (C:${EPROM_START}-C:0xFFFF), HCONST (C:${EPROM_START}-C:0xFFFF)"
echo "CLASSES=${CLASSES}"

# 2. build app
rm -rf ./output || true
time make DEBUG=1 INTVECTOR=${LDR_SIZE} CLASSES="${CLASSES}" -j


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

STC_ROM_SIZE=$(grep -oP '#define STC_ROM_SIZE \K[0x0-9A-F]+' ${COMMON_H})
APP_MAX_SIZE=$((STC_ROM_SIZE - LDR_SIZE))
APP_MAX_SIZE_10=`printf "%d" ${APP_MAX_SIZE}` # convert 0xXXXX to decimal

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