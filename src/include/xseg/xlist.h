/*
Copyright (C) 2010-2014 GRNET S.A.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _XLIST_H
#define _XLIST_H

#include <xseg/xq.h>

struct xlist_node {
	XPTR_TYPE(struct xlist_node) head;
	XPTR_TYPE(struct xlist_node) tail;
	XPTR_TYPE(struct xlist) list;
};

struct xlist {
	XPTR_TYPE(struct xlist_node) node;
};

xqindex xlist_add(struct xlist *list, struct xlist_node *node);
void xlist_del(struct xlist_node *node);

#endif
