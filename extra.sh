#!/bin/sh

# This is experimental and not working

KERNEL=$(head kernel.txt -n 1)

if [ $(id -u) -ne 0 ]
then

    echo This needs root permissions!
    exit 0
    
fi

BASE=$(pwd)

if [ ! -d kernel/$KERNEL/initramfs ]
then

    echo At first, you have to build PlopKexec!
    exit 0
    
fi
VER=$(echo $KERNEL|cut -b7-)

cd kernel/$KERNEL &&
make modules && make modules_install &&
mkdir -p initramfs/lib/modules &&
cp -avr /lib/modules/$VER-plopkexec initramfs/lib/modules

echo Now build again PlopKexec