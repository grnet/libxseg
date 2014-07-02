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

.PHONY: install-src

include $(XSEG_HOME)/base.mk

XTYPES = xq
XTYPES += xpool
XTYPES += xhash
XTYPES += xheap
XTYPES += xworkq
XTYPES += xwaitq
XTYPES += xbinheap
XTYPES += xcache
XTYPES += xobj

export XTYPES

FILES="Makefile"
FILES+=$(shell ls *.h)

SUBDIR:=$(subst $(XSEG_HOME)/,,$(CURDIR))
XSEG_CLEAN_TARGETS=$(patsubst %,clean-%,$(XSEG_DOMAIN_TARGETS))
XSEG_INSTALL_TARGETS=$(patsubst %,install-%,$(XSEG_DOMAIN_TARGETS))
XSEG_INSTALLSRC_TARGETS=$(patsubst %,install-src-%,$(XSEG_DOMAIN_TARGETS))


default: $(XSEG_DOMAIN_TARGETS)

install: $(XSEG_INSTALL_TARGETS)

clean: $(XSEG_CLEAN_TARGETS)

.PHONY: $(XSEG_DOMAIN_TARGETS)
$(XSEG_DOMAIN_TARGETS):
	@echo " ";
	@echo "=== $@ in $(SUBDIR) ===";
	@echo " ";
	@make -C $@ TARGET=$@ default || exit 1;
	@echo " ";

.PHONY: $(XSEG_INSTALL_TARGETS)
$(XSEG_INSTALL_TARGETS):
	@echo " ";
	@echo "=== $@ in $(SUBDIR)===";
	@echo " ";
	@make -C $(@:install-%=%) TARGET=$@ install || true;
	@echo " ";


.PHONY: $(XSEG_INSTALLSRC_TARGETS)
$(XSEG_INSTALLSRC_TARGETS):
	@echo " ";
	@echo "=== $@ in $(SUBDIR)===";
	@echo " ";
	@make -C $(@:install-src-%=%) TARGET=$@ install-src || true;
	@echo " ";

.PHONY: $(XSEG_CLEAN_TARGETS)
$(XSEG_CLEAN_TARGETS):
	@echo " ";
	@echo "=== $@ in $(SUBDIR)===";
	@echo " ";
	@make -C $(@:clean-%=%) TARGET=$@ clean || true;
	@echo " ";



install-src: $(XSEG_INSTALLSRC_TARGETS)
	@install -d $(DESTDIR)$(srcdir)$(SUBDIR) ;
	@for f in $(FILES) ; do \
		install -o 0 -g 0 -m 644 -t $(DESTDIR)$(srcdir)$(SUBDIR) $$f ; \
	done

