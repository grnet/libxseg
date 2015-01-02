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


#ifndef __XBINHEAP_H
#define __XBINHEAP_H

#include <xseg/xtypes.h>
#include <xseg/domain.h>
#include <xseg/util.h>

typedef uint64_t xbinheapidx;
typedef xbinheapidx xbinheap_handler;
#define NoNode (xbinheapidx)-1
#define XBINHEAP_MAX (uint32_t)(1<<0)
#define XBINHEAP_MIN (uint32_t)(1<<1)

struct xbinheap_node {
    xbinheapidx key;
    xbinheapidx value;
    xbinheapidx h;
};

struct xbinheap {
    xbinheapidx size;
    xbinheapidx count;
    uint32_t flags;
    xbinheapidx *indexes;
    struct xbinheap_node *nodes;
};

xbinheap_handler xbinheap_insert(struct xbinheap *h, xbinheapidx key,
                                 xbinheapidx value);
int xbinheap_empty(struct xbinheap *h);
xbinheapidx xbinheap_peak(struct xbinheap *h);
xbinheapidx xbinheap_extract(struct xbinheap *h);
int xbinheap_increasekey(struct xbinheap *h, xbinheap_handler idx,
                         xbinheapidx newkey);
int xbinheap_decreasekey(struct xbinheap *h, xbinheap_handler idx,
                         xbinheapidx newkey);
xbinheapidx xbinheap_getkey(struct xbinheap *h, xbinheap_handler idx);
int xbinheap_init(struct xbinheap *h, xbinheapidx size, uint32_t flags,
                  void *mem);
void xbinheap_free(struct xbinheap *h);

#endif                          /* __XBINHEAP_H */
