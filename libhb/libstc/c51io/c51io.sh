#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

CC="/e/local_program/tcc-0.9.27-win64-bin/tcc/tcc.exe"

# check if CC exists
if [ ! -x "$CC" ]; then
    echo "C compiler '$CC' does not exist or is not executable. Please check the path."
    exit 1
fi

# check if SYS_DIR is defined
if [ -z "$SYS_DIR" ]; then
    echo "'SYS_DIR' is not defined. Please set 'SYS_DIR' to the path that contains your 'sys.h'."
    exit 1
fi

# split DEFINES with `;` and convert to -D flags
DEFINES=$(echo "$DEFINES" | tr ';' ' ')
if [ -z "$DEFINES" ]; then
    echo "No defines provided. Using default defines."
else
    echo "Using defines: $DEFINES"
fi
NEW_DEFINES=$(echo "$DEFINES" | sed 's/ / -D/g')
echo "New defines: $NEW_DEFINES"

sys_h=$SYS_DIR/sys.h
gpio_h=$SYS_DIR/gpio.h

# check if sys.h exists
if [ ! -f "$sys_h" ]; then
    echo "File '$sys_h' does not exist. Please check the 'SYS_DIR' path."
    exit 1
fi

$CC -I"$SYS_DIR" \
    -I"H:/dev_hb/libhb" \
    -DVSCODE \
    ${NEW_DEFINES} \
    -run \
    ${CURRENT_DIR}/c51io.c \
    > $gpio_h
