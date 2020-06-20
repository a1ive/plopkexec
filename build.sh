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
make 			&&
cd $BASE/kernel 	&&
{
    if [ ! -d $KERNEL ]
    then
	echo "Extracting Linux kernel source code" &&
	tar xfJ $KERNEL.tar.xz
    fi
}			&&
cd $KERNEL		&&
cp -avr ../.config . &&
tar xfvz ../initramfs.tar.gz &&
cp -av $BASE/src/init initramfs   &&
cd $BASE/busybox/ &&
sh build_busybox &&
cp -av $BASE/busybox/busybox/_install/* $BASE/kernel/$KERNEL/initramfs &&
cd $BASE/kexec 		 &&
sh build_kexec &&
cp -av kexec $BASE/kernel/$KERNEL/initramfs &&
cd $BASE/kernel/$KERNEL &&
make -j 4 bzImage 			  &&		
cp -av arch/x86/boot/bzImage $BASE/build/plopkexec &&
cd $BASE 		 &&
cp -av build/plopkexec iso/iso &&
cd iso 			 &&
sh make-iso.sh 		 &&
cp -av plopkexec.iso ../build	 &&
echo && 
echo "Built successfully plopkexec and plopkexec.iso" &&
echo "Find them in the 'build/' directory."



