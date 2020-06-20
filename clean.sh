#!/bin/sh

BASE=$(pwd)
KERNEL=$(head kernel.txt -n 1)

cd src && make clean

cd $BASE
rm -rf kernel/$KERNEL

rm -rf kexec/kexec-tools-2.0.19
rm -rf kexec/kexec

rm -rf busybox/busybox-1.24.2

rm -rf build/*

echo "Done"
