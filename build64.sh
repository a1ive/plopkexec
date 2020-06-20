#!/bin/sh

KERNEL=$(head kernel.txt -n 1)

if [ $(id -u) -ne 0 ]
then

    echo Building PlopKexec needs root permissions!
    exit 0
    
fi

BASE=$(pwd)

mkdir -p build		&&
cd src 			&&
make -f Makefile64	&&
cd $BASE/kernel 	&&
{
    if [ ! -d $KERNEL ]
    then
	echo "Extracting Linux kernel source code" &&
	tar xfJ $KERNEL.tar.xz
    fi
}			&&
cd $KERNEL		&&
if [ "$1" == "FB" ]
then
    cp -avr ../.configfb64 .config
else
    cp -avr ../.config64 .config
fi &&
tar xfvz ../initramfs.tar.gz &&
cp -av $BASE/src/init initramfs   &&
cd $BASE/busybox/ &&
sh build_busybox &&
cp -av $BASE/busybox/busybox/_install/* $BASE/kernel/$KERNEL/initramfs &&
cd $BASE/kexec 		 &&
sh build_kexec64 &&
cp -av kexec $BASE/kernel/$KERNEL/initramfs &&
cd $BASE/kernel/$KERNEL &&
make -j 4 bzImage 			  &&		
cp -av arch/x86/boot/bzImage $BASE/build/plopkexec64 &&
cd $BASE 		 &&
cp -av build/plopkexec64 build/bootx64.efi &&
cp -av build/plopkexec64 iso/iso/plopkexec &&
cp -av build/plopkexec64 iso/iso/EFI/boot/bootx64.efi &&
cd iso 			 &&
sh make-iso.sh 		 &&
cp -av plopkexec.iso ../build/plopkexec64.iso	 &&
echo && 
if [ "$1" == "FB" ]
then
    echo "Built in framebuffer drivers"
fi &&
echo "Built successfully plopkexec64 and plopkexec64.iso" &&
echo "Find them in the 'build/' directory."



