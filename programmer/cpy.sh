#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

libhb_dir="H:/dev_hb/libhb"
src_dir=${libhb_dir}/libstc/disassembler/
dst_dir=${CURRENT_DIR}/programmer/disassembler/
files="
hex.cpp
hex.h
hex80.cpp
hex80.h
intel8051is.cpp
intel8051is.h
"

for file in ${files}; do
    cp ${src_dir}/${file} ${dst_dir}/${file}
done
