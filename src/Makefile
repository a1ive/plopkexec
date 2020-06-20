
NAME 	= init
OBJS 	= plpkexec.o keyboard.o vt.o run.o tools.o log.o menu.o menuentry.o edit.o scan_syslinux.o scan_lilo.o scan_grub.o scan_grub2.o devices.o


# Add -m32 on 64bit OS
UNAME := $(shell uname -m)

ifeq ($(UNAME), x86_64)
M = -m32
endif

DATE := $(shell date)


# Set compile flags
CFLAGS 	= -g -c -Wunused -Wuninitialized -O2 -march=i486 $(M)
LDFLAGS = -Wall -D_FILE_OFFSET_BITS=64 --static -pthread $(M)
LINK 	= $(CC) -o


CC 	= g++
COMPILE = $(CC)  $(CFLAGS)


$(NAME):$(OBJS) 
	$(LINK) $(NAME) $(LDFLAGS) $(OBJS) $(LIBS)
	strip -s init


built: *.cpp *.h
	echo "#define BUILT \"$(DATE) on $(UNAME)\"" > built


keyboard.o: keyboard.cpp keyboard.h
	$(COMPILE) keyboard.cpp

vt.o: vt.cpp vt.h
	$(COMPILE) vt.cpp

run.o: run.cpp run.h
	$(COMPILE) run.cpp

log.o: log.cpp log.h
	$(COMPILE) log.cpp

tools.o: tools.cpp tools.h
	$(COMPILE) tools.cpp

menu.o: menu.cpp menu.h
	$(COMPILE) menu.cpp

menuentry.o: menuentry.cpp menuentry.h
	$(COMPILE) menuentry.cpp

plpkexec.o: plpkexec.cpp plpkexec.h built
	$(COMPILE) plpkexec.cpp

edit.o: edit.cpp edit.h
	$(COMPILE) edit.cpp

devices.o: devices.cpp devices.h
	$(COMPILE) devices.cpp

scan_syslinux.o: scan_syslinux.cpp scan_syslinux.h
	$(COMPILE) scan_syslinux.cpp

scan_lilo.o: scan_lilo.cpp scan_lilo.h
	$(COMPILE) scan_lilo.cpp

scan_grub.o: scan_grub.cpp scan_grub.h
	$(COMPILE) scan_grub.cpp

scan_grub2.o: scan_grub2.cpp scan_grub2.h
	$(COMPILE) scan_grub2.cpp

clean:
	rm -f built
	rm -f *.o
	rm -f $(NAME)
