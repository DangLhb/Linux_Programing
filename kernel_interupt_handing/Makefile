obj-m	:=	interupt.o
# KERNEL_SRC	?=	/lib/modules/$(shell uname  -r)/build
KERNEL_SRC	?=	/lib/modules/6.8.0-49-generic/build

all default:	modules
install:	modules_install
modules modules_install help clean:
	$(MAKE)	-C	$(KERNEL_SRC)	M=$(shell pwd)	$@
