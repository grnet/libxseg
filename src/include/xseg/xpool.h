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

#ifndef XPOOL_H
#define XPOOL_H

#include <xseg/util.h>
#include <xseg/xlock.h>

typedef uint64_t xpool_index;
typedef uint64_t xpool_data;
#define NoIndex ((xpool_index) -1)


struct xpool_node {
    xpool_data data;
    //XPTR_TYPE(struct xpool_node) next;
    //XPTR_TYPE(struct xpool_node) prev;
    xpool_index next;
    xpool_index prev;
};

struct xpool {
    struct xlock lock;
    //XPTR_TYPE(struct xpool_node) list;
    //XPTR_TYPE(struct xpool_node) free;
    xpool_index list;
    xpool_index free;
    uint64_t size;
     XPTR_TYPE(struct xpool_node) mem;
};

void xpool_init(struct xpool *xp, uint64_t size, struct xpool_node *mem);
void xpool_clear(struct xpool *xp);
xpool_index xpool_add(struct xpool *xp, xpool_data data);
xpool_index xpool_remove(struct xpool *xp, xpool_index idx, xpool_data * data);
xpool_index xpool_peek(struct xpool *xp, xpool_data * data);
xpool_index xpool_peek_idx(struct xpool *xp, xpool_index idx,
                           xpool_data * data);
xpool_index xpool_peek_and_fwd(struct xpool *xp, xpool_data * data);
xpool_index xpool_set_idx(struct xpool *xp, xpool_index idx, xpool_data data);

void __xpool_clear(struct xpool *xp);
xpool_index __xpool_add(struct xpool *xp, xpool_data data);
xpool_index __xpool_remove(struct xpool *xp, xpool_index idx,
                           xpool_data * data);
xpool_index __xpool_peek(struct xpool *xp, xpool_data * data);
xpool_index __xpool_peek_idx(struct xpool *xp, xpool_index idx,
                             xpool_data * data);
xpool_index __xpool_peek_and_fwd(struct xpool *xp, xpool_data * data);
xpool_index __xpool_set_idx(struct xpool *xp, xpool_index idx,
                            xpool_data data);

#endif                          /* XPOOL_H */
