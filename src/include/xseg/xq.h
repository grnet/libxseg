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

#ifndef _XQ_H
#define _XQ_H

#include <xseg/xtypes.h>
#include <xseg/util.h>
#include <xseg/xlock.h>

typedef uint64_t xqindex;

struct xq {
        struct xlock lock;
        xqindex head, tail;
        XPTR_TYPE(xqindex) queue;
        xqindex size;
};

xqindex    * xq_alloc_empty  ( struct xq  * xq,
                               xqindex      size );

void         xq_init_empty   ( struct xq  * xq,
                               xqindex      size,
                               void       * mem );

xqindex    * xq_alloc_map    ( struct xq  * xq,
                               xqindex      size,
                               xqindex      count,
                               xqindex   (* mapfn ) (xqindex) );

void         xq_init_map     ( struct xq  * xq,
                               xqindex      size,
                               xqindex      count,
                               xqindex   (* mapfn ) (xqindex),
                               void       * mem );

xqindex    * xq_alloc_seq    ( struct xq  * xq,
                               xqindex      size,
                               xqindex      count );

void         xq_init_seq     ( struct xq  * xq,
                               xqindex      size,
                               xqindex      count,
                               void       * mem );

void         xq_free         ( struct xq  * xq  );

xqindex      __xq_append_head( struct xq  * xq,
                               xqindex      xqi );

xqindex      xq_append_head  ( struct xq  * xq,
                               xqindex      xqi );

xqindex      __xq_pop_head   ( struct xq  * xq );
xqindex      xq_pop_head     ( struct xq  * xq );

xqindex      __xq_append_tail( struct xq  * xq,
                               xqindex      xqi );

xqindex      xq_append_tail  ( struct xq  * xq,
                               xqindex      xqi );


xqindex      __xq_peek_head    ( struct xq  * xq);

xqindex      xq_peek_head    ( struct xq  * xq );

xqindex      __xq_peek_tail    ( struct xq  * xq);

xqindex      xq_peek_tail    ( struct xq  * xq );

xqindex      __xq_pop_tail   ( struct xq  * xq  );

xqindex      xq_pop_tail     ( struct xq  * xq );

int          xq_head_to_tail ( struct xq  * hq,
                               struct xq  * tq,
                               xqindex      nr );

xqindex      xq_size         ( struct xq  * xq  );

xqindex      xq_count        ( struct xq  * xq  );

void         xq_print        ( struct xq  * xq  );

int 	     __xq_check      ( struct xq  * xq, 
		               xqindex      idx );

int 	     xq_check        ( struct xq  * xq, 
		               xqindex      idx );

xqindex      __xq_resize     ( struct xq  * xq,
		               struct xq  * newxq);

xqindex      xq_resize       ( struct xq  * xq,
		               struct xq  * newxq );
#endif

