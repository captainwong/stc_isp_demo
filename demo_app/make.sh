#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

COMMON_H=${CURRENT_DIR}/../common.h

# parse LDR_SIZE from COMMON_H like `#define LDR_SIZE 0x1000  // bootloader flash space`
LDR_SIZE=$(grep -oP '#define LDR_SIZE \K[0x0-9A-F]+' ${COMMON_H})
EPROM_START=$(printf "0x%04X" $((LDR_SIZE + 3)))
echo "EPROM_START=${EPROM_START}"
CLASSES="CODE (C:${EPROM_START}-C:0xFFFF), CONST (C:${EPROM_START}-C:0xFFFF), ECODE (C:${EPROM_START}-C:0xFFFF), HCONST (C:${EPROM_START}-C:0xFFFF)"
echo "CLASSES=${CLASSES}"

rm -rf ./output || true
time make DEBUG=1 INTVECTOR=${LDR_SIZE} CLASSES="${CLASSES}" -j

