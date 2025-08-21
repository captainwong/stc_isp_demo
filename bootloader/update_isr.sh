#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

COMMON_H=${CURRENT_DIR}/../common.h
ISR_ASM=${CURRENT_DIR}/src/isr.asm

# parse LDR_SIZE from COMMON_H like `#define LDR_SIZE 0x1000  // bootloader flash space`
LDR_SIZE=$(grep -oP '#define LDR_SIZE \K[0x0-9A-F]+' ${COMMON_H})

echo "LDR_SIZE=${LDR_SIZE}"

# convert the LDR_SIZE from `0xXXXX` to `XXXXH`
LDR_SIZE_HEX=$(printf "%04XH" ${LDR_SIZE})
echo "LDR_SIZE_HEX=${LDR_SIZE_HEX}"

# replace LDR_SIZE in ISR_ASM like `LDR_SIZE EQU 1000H`
sed -i "s/LDR_SIZE EQU .*/LDR_SIZE EQU ${LDR_SIZE_HEX}/" ${ISR_ASM}
