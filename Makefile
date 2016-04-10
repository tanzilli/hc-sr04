KERNELRELEASE	?= `uname -r`
KERNEL_DIR	?= /lib/modules/$(KERNELRELEASE)/build
PWD		:= $(shell pwd)
obj-m		:= hcsr04.o

PREFIX ?= /usr/local
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/share/man
MAN1DIR = $(MANDIR)/man1
INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -p -m 755
INSTALL_DIR     = $(INSTALL) -p -m 755 -d
INSTALL_DATA    = $(INSTALL) -m 644

#MODULE_OPTIONS = devices=2

##########################################
# note on build targets
#
# module-assistant makes some assumptions about targets, namely
#  <modulename>: must be present and build the module <modulename>
#                <modulename>.ko is not enough
# install: must be present (and should only install the module)
#
# we therefore make <modulename> a .PHONY alias to <modulename>.ko
# call 'make install-all' if you want to install everything
##########################################


.PHONY: all install clean distclean
.PHONY: install-all install-utils install-man
.PHONY: modprobe hcsr04 

# we don't control the .ko file dependencies, as it is done by kernel
.PHONY: hcsr04.ko

all: hcsr04.ko
hcsr04: hcsr04.ko
hcsr04.ko:
	@echo "Building hcsr04 driver..."
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

install: install
install:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules_install
	depmod -a  $(KERNELRELEASE)

clean:
	rm -f *~
	rm -f Module.symvers Module.markers modules.order
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

modprobe: hcsr04.ko
	chmod a+r hcsr04.ko
	-sudo rmmod hcsr04
	sudo insmod ./hcsr04.ko $(MODULE_OPTIONS)
