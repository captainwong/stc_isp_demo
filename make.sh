#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

./libhb/cpy.sh
./programmer/cpy.sh
./bootloader/make.sh
./demo_app/make.sh
