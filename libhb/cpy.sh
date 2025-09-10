#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

libhb_dir="H:/dev_hb/libhb"
src_dir=${libhb_dir}
dst_dir=${CURRENT_DIR}
files="
libemb/time/gmtime.c
libemb/time/localtime.c
libemb/time/mktime.c
libemb/time/time.c
libemb/emb_bitrev.h
libemb/emb_bitrev.c
libemb/emb_config.h
libemb/emb_stdint.h
libemb/emb_time.h
libemb/emb_version.h
libemb/ringbuf.h
libemb/libemb.h
libstc/c51io/c51io.c
libstc/c51io/c51io.sh
libstc/stc_helper_gpio.h
libstc/stc_helpers.h
libstc/stc8h.c
libstc/stc8h.h
libstc/disassembler/hex.h
libstc/disassembler/hex.cpp
libstc/disassembler/hex80.h
libstc/disassembler/hex80.cpp
libstc/disassembler/intel8051is.h
libstc/disassembler/intel8051is.cpp
libhbcheck/hb_check.h
libhbcheck/hb_check.c
libhbcheck/hbcheck_config.h
libhbcheck/hbcheck_version.h
libhbcheck/libhbcheck.h
"

for file in ${files}; do
    mkdir -p ${dst_dir}/$(dirname ${file})
    cp ${src_dir}/${file} ${dst_dir}/${file}
done

# patch libstc/c51io/c51io.sh
# replace `CC="/e/local_program/tcc-0.9.27-win64-bin/tcc/tcc.exe"` with `CC="${CURRENT_DIR}/../../../tcc-0.9.27-win64-bin/tcc/tcc.exe"`
# sed -i 's|CC="/e/local_program/tcc-0.9.27-win64-bin/tcc/tcc.exe"|CC="${CURRENT_DIR}/../../../tcc-0.9.27-win64-bin/tcc/tcc.exe"|' ${dst_dir}/libstc/c51io/c51io.sh
# replace `-I"H:/dev_hb/libhb" \` with `-I"../libhb" \`
# sed -i 's|-I"H:/dev_hb/libhb" |-I"../libhb" |' ${dst_dir}/libstc/c51io/c51io.sh

