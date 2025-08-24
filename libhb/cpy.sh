#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

libhb_dir="H:/dev_hb/libhb"
src_dir=${libhb_dir}/
dst_dir=${CURRENT_DIR}/
files="
libemb/time/gmtime.c
libemb/time/localtime.c
libemb/time/mktime.c
libemb/time/time.c
libemb/emb_config.h
libemb/emb_stdint.h
libemb/emb_time.h
libstc/c51io/c51io.c
libstc/c51io/c51io.sh
libstc/stc_helper_gpio.h
libstc/stc_helper.h
libstc/stc8h.c
libstc/stc8h.h
"

for file in ${files}; do
    cp ${src_dir}/${file} ${dst_dir}/${file}
done
