mkisofs -J -r -V PlopKexec \
	-hide-joliet-trans-tbl -hide-rr-moved  \
	-allow-leading-dots \
        -o plopkexec.iso -no-emul-boot -boot-load-size 4 \
	-c boot.catalog -b isolinux.bin -boot-info-table -l iso &&
echo Creating hybrid ISO &&
isohybrid plopkexec.iso
