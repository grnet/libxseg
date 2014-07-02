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

.PHONY: default all clean install install-src

include $(XSEG_HOME)/base.mk

DEBUG=-g

FILES="Makefile"
#FILES+=$(shell ls *.h)
#FILES+=$(shell ls *.c)

SUBDIR:=$(subst $(XSEG_HOME),,$(CURDIR))

default: all

all: xpool.o xpool.pic.o 

$(BASE)/sys/user/xseg_user.o:
	make -C $(BASE)/sys/user xseg_user.o

xpool.o: $(BASE)/xtypes/xpool.c $(BASE)/xtypes/xpool.h $(BASE)/xtypes/xlock.h
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

xpool.pic.o: $(BASE)/xtypes/xpool.c $(BASE)/xtypes/xpool.h $(BASE)/xtypes/xlock.h
	$(CC) $(CFLAGS) $(INC) -fPIC -c -o $@ $<

install:

install-src:
	install -d $(DESTDIR)$(srcdir)$(SUBDIR) ;
	@for f in $(FILES) ; do \
		install -o 0 -g 0 -m 644 -t $(DESTDIR)$(srcdir)$(SUBDIR) $$f ; \
	done

clean:
	rm -f xpool.o xpool.pic.o
