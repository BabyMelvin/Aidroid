#!/bin/sh

CURDIR=`pwd`

# this expectd that this is place as a first level folder relative
# to the other OP-TEE folder in a setup using default repo configuration
# as described by the documentation in optee_os(READ.md)

ROOT=${PWD}
ROOT=`dirname $ROOT`

# path to the toolchain
export PATH=${ROOT}/toolchains/aarch32/bin:$PATH

# path to the TA-DEV-KIT coming from optee_os
export TA_DEV_KIT_DIR=${ROOT}/optee_os/out/arm/export-ta_arm32

# path to the client library (GP Client API)
export TEEC_EXPORT=${ROOT}/optee_client/out/export

export PLATFORM=vexpress
export PLATFORM_FLAVOR=qemu_virt

# toolchain prefix for the user space code (normal world)
HOST_CROSS_COMPILE=arm-linux-geueabihf-

# Build the host application
cd $CURDIR/host

make CROSS_COMPILE=$HOST_CORSS_COMPILE $@

# Build the Trust Application
cd $CURDIR/ta
make CROSS_COMPILE=$TA_CROSS_COMPILE $@

