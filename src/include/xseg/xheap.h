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

#ifndef __XHEAP_H__
#define __XHEAP_H__

#include <xseg/util.h>
#include <xseg/xlock.h>

struct xheap_header {
    uint64_t magic;
     XPTR_TYPE(struct xheap) heap;
    uint64_t size;
};

struct xheap {
    uint32_t alignment_unit;
    uint64_t size;
    uint64_t cur;
    struct xlock lock;
     XPTR_TYPE(void) mem;
};

uint64_t xheap_get_chunk_size(void *ptr);
int xheap_init(struct xheap *xheap, uint64_t size, uint32_t alignment_unit,
               void *mem);
void *xheap_allocate(struct xheap *xheap, uint64_t bytes);
void xheap_free(void *ptr);

#endif
