# Copyright (C) 2010-2014 GRNET S.A.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# Setup for xseg Makefiles.

ifndef TARGET
TARGET:=$(shell basename $(shell pwd))
endif

export CC=gcc
ifndef MOPTS
export MOPTS=
endif
ifndef COPTS
export COPTS=-O2 -g -finline-functions $(MOPTS) $(DEBUG)
endif
ifndef CSTD
export CSTD=-std=gnu99 -pedantic
endif

export TOPDIR=$(shell dirname $(CURDIR))
ifeq (,$(VERSION))
export VERSION=$(shell cat $(TOPDIR)/version)
endif

ifeq (,$(DESTDIR))
export DESTDIR=/
endif

ifeq (,$(KVER))
export KVER=$(shell uname -r)
endif


bindir=/usr/bin/
libdir=/usr/lib/
pythondir=/usr/lib/python2.7/

INC=-I$(BASE)
INC+=-I$(BASE)/peers/$(TARGET)
INC+=-I$(BASE)/sys/$(TARGET)
INC+=-I$(BASE)/drivers/$(TARGET)
export INC

export LIB=$(BASE)/lib/$(TARGET)
export CFLAGS=-Wall $(COPTS) $(CSTD)

#ifeq (,$(XSEG_HOME))
#export XSEG_HOME=$(shell ${XSEG_HOME})
#endif

ifeq (,$(XSEG_HOME))
export XSEG_HOME=$(CURDIR)
endif

CONFIG=./config.mk

#default:

#.PHONY: clean-config

#clean: clean-config

#clean-config:
#	rm -f $(CONFIG)

ifndef BASE
exists=$(shell [ -f "$(CONFIG)" ] && echo exists)
ifeq (exists,$(exists))
include $(CONFIG)
else
$(shell $(XSEG_HOME)/envsetup show | sed -e 's/"//g' > "$(CONFIG)")
include $(CONFIG)
endif

ifeq (,$(XSEG_DOMAIN_TARGETS))
export XSEG_DOMAIN_TARGETS=$(shell $(XSEG_HOME)/tools/xseg-domain-targets | sed -e 's/^[^=]*=//;s/"//g')
endif
export BASE=$(XSEG_HOME)
endif
