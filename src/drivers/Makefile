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

.PHONY: default all clean xseg lib

include $(XSEG_HOME)/base.mk

FILES="Makefile"
#FILES+=$(shell ls *.h)
FILES+=$(shell ls *.c)

SUBDIR:=$(subst $(XSEG_HOME),,$(CURDIR))


DRIVERS=xseg_posix xseg_segdev xseg_pthread xseg_posixfd
DRVOBJS=$(DRIVERS:=.o)
DRVSOS=$(DRIVERS:=.so)

default: all

all: xseg $(DRIVERS:=.so) lib

$(BASE)/sys/user/xseg_user.o:
	make -C $(BASE)/sys/user xseg_user.o

xseg_posix.o: xseg_posix.c $(BASE)/xseg/xseg.h $(BASE)/drivers/xseg_posix.h
	$(CC) $(CFLAGS) $(INC) -fPIC -c -o $@ $<

xseg_posix.so: xseg_posix.o $(BASE)/sys/user/xseg_user.o
	$(CC) -shared -o $@ $< $(BASE)/sys/user/xseg_user.o 

xseg_segdev.o: xseg_segdev.c $(BASE)/xseg/xseg.h $(BASE)/drivers/xseg_segdev.h
	$(CC) $(CFLAGS) $(INC) -fPIC -c -o $@ $<

xseg_segdev.so: xseg_segdev.o $(BASE)/sys/user/xseg_user.o
	$(CC) -shared -o $@ $< $(BASE)/sys/user/xseg_user.o 

xseg_pthread.o: xseg_pthread.c $(BASE)/xseg/xseg.h $(BASE)/drivers/xseg_pthread.h
	$(CC) $(CFLAGS) $(INC) -fPIC -c -o $@ $<

xseg_pthread.so: xseg_pthread.o $(BASE)/sys/user/xseg_user.o
	$(CC) -shared -lpthread -o $@ $< $(BASE)/sys/user/xseg_user.o 

xseg_posixfd.o: xseg_posixfd.c $(BASE)/xseg/xseg.h $(BASE)/drivers/xseg_posixfd.h
	$(CC) $(CFLAGS) $(INC) -fPIC -c -o $@ $<

xseg_posixfd.so: xseg_posixfd.o $(BASE)/sys/user/xseg_user.o
	$(CC) -shared -o $@ $< $(BASE)/sys/user/xseg_user.o

.PHONY: lib
lib:
	cp -vaf $(DRVSOS) $(LIB)

.PHONY: install-src

install-src:
	install -d $(DESTDIR)$(srcdir)$(SUBDIR) ;
	@for f in $(FILES) ; do \
		install -o 0 -g 0 -m 644 -t $(DESTDIR)$(srcdir)$(SUBDIR) $$f ; \
	done

.PHONY: install
install:


clean:
	rm -f $(DRVOBJS) $(DRVSOS)
