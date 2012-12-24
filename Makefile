#
# Makefile
#
#   Copyright 2012  Oleg Kutkov <elenbert@gmail.com>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.  
#

BINARY		:= syshook
KERNEL		:= /lib/modules/$(shell uname -r)/build
ARCH		:= x86
C_FLAGS		:= -Wall
KMOD_DIR	:= $(shell pwd)


OBJECTS	:= \
	syscall_main.o \
	procfs_interface.o \
	hook_install.o \
	hooks/socket_hook.o

ccflags-y += $(C_FLAGS)

obj-m += $(BINARY).o

$(BINARY)-y := $(OBJECTS)


$(BINARY).ko:
	make -C $(KERNEL) M=$(KMOD_DIR) modules


clean:
	make -C $(KERNEL) M=$(KMOD_DIR) clean

